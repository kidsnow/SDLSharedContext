// #define MULTITHREADING
// #define MULTICONTEXT

#include <iostream>

#ifdef MULTITHREADING
#include <thread>
#include <mutex>
#include <condition_variable>
#endif

#define GLEW_NO_GLU
#include "GL/glew.h"
#define SDL_MAIN_HANDLED
#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "glm/glm.hpp"

#ifdef MULTITHREADING
std::mutex s_m;
std::condition_variable s_cv;
#endif
bool s_render = false;
bool s_processed = false;

SDL_Window* s_window;
SDL_GLContext s_mainContext, s_renderContext;
std::string s_windowName = "Hello";
int s_width = 640, s_height = 480;
bool s_windowShouldClose = false;

GLuint s_triangleProgram;
GLuint s_textureProgram;
static const GLfloat s_triangleVertices[] =
{
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f,  1.0f, 0.0f,
};
GLuint s_triangleVBO;
GLuint s_squareVertexVBO, s_squareTexCoordVBO;
GLuint s_textureLoc;
GLuint s_MVPMatLoc;
glm::mat4x4 s_MVPMat;
GLuint s_colorLoc;
GLfloat s_color;

static const GLfloat s_squareVertices[] =
{
	-1.0f, -1.0f, 0.0f,// lower left
	 1.0f, -1.0f, 0.0f, // lower right
	-1.0f,  1.0f, 0.0f,// upper left
	-1.0f,  1.0f, 0.0f,// upper left
	 1.0f, -1.0f, 0.0f, // lower right
	 1.0f,  1.0f, 0.0f, // upper right
};
static const GLfloat s_squareTexCoords[] =
{
	0.0f, 0.0f, // lower left
	1.0f, 0.0f,	// lower right
	0.0f, 1.0f,	// upper left
	0.0f, 1.0f,	// upper left
	1.0f, 0.0f,	// lower right
	1.0f, 1.0f	// upper right
};

GLuint s_frameBuffer;
GLuint s_renderedTexture;
GLuint s_depthBuffer;

bool InitSDLWindow()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SDL_Log("Couldn't initialize video driver: %s\n", SDL_GetError());
		return false;
	}

	SDL_DisplayMode displayMode;
	uint32_t windowFlags;

	SDL_zero(displayMode);
	displayMode.format = SDL_PIXELFORMAT_RGB888;
	displayMode.refresh_rate = 0;

	windowFlags = 0;
	windowFlags |= SDL_WINDOW_RESIZABLE;
	windowFlags |= SDL_WINDOW_OPENGL;
	
	s_window = SDL_CreateWindow(s_windowName.c_str(), 100, 100, 640, 480, windowFlags);
	if (s_window == NULL)
	{
		SDL_Log("Couldn't create window: %s\n", SDL_GetError());
		SDL_Quit();
		return false;
	}

	int createdWindowWidth, createdWindowHeight;
	SDL_GetWindowSize(s_window, &createdWindowWidth, &createdWindowHeight);
	if (!(windowFlags & SDL_WINDOW_RESIZABLE) &&
		(createdWindowWidth != s_width || createdWindowHeight != s_height)) {
		printf("Window requested size %dx%d, got %dx%d\n",
			s_width, s_height, createdWindowWidth, createdWindowHeight);
		s_width = createdWindowWidth;
		s_height = createdWindowHeight;
	}

	SDL_ShowWindow(s_window);

	return true;
}

bool CreateGLContext()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	s_mainContext = SDL_GL_CreateContext(s_window);
	if (!s_mainContext) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext(): %s\n", SDL_GetError());
		SDL_DestroyWindow(s_window);
		SDL_Quit();
		return 1;
	}

	SDL_GL_MakeCurrent(s_window, s_mainContext);
#ifdef MULTICONTEXT
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	s_renderContext = SDL_GL_CreateContext(s_window);
	if (!s_renderContext) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext(): %s\n", SDL_GetError());
		SDL_DestroyWindow(s_window);
		SDL_Quit();
		return 1;
	}
#endif 

	glewExperimental = true;
    GLenum err;
	if (err = glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to init GLEW..." << std::endl;
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return false;
	}

	std::cout << glGetString(GL_VERSION);

	return true;
}

GLchar* ReadShader(const char* fileName) {
	FILE* infile = fopen(fileName, "rb");
	if (!infile) {
		std::cerr << "Unable to open file '" << fileName << "'" << std::endl;
		return NULL;
	}

	fseek(infile, 0, SEEK_END);
	int len = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	GLchar* source = (GLchar*)malloc((len + 1) * sizeof(GLchar));

	fread(source, 1, len, infile);
	fclose(infile);

	source[len] = 0;

	return source;
}

bool CompileShader(const char* shaderFileName, GLenum shaderType, GLuint& shader)
{
	shader = glCreateShader(shaderType);
	if (shader == NULL)
	{
		std::cerr << ("Failed to create shader object.") << std::endl;
		return false;
	}

	GLchar* source = ReadShader(shaderFileName);
	if (source == NULL)
	{
		glDeleteShader(shader);
		return false;
	}

	glShaderSource(shader, 1, &source, NULL);
	free(source);

	glCompileShader(shader);

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLsizei asdf;
		GLchar* infoLog = new GLchar[10000];
		glGetShaderInfoLog(shader, 10000, &asdf, infoLog);
		glDeleteShader(shader);
		return false;
	}

	return true;
}

bool CreateShaderProgram(const char* vsFileName, const char* fsFileName, GLuint& program)
{
	program = glCreateProgram();
	if (program == NULL)
	{
		std::cerr << "Failed to create m_program object." << std::endl;
		return false;
	}

	GLuint vertexShader, fragmentShader;
	if (!CompileShader(vsFileName, GL_VERTEX_SHADER, vertexShader)) return false;
	if (!CompileShader(fsFileName, GL_FRAGMENT_SHADER, fragmentShader)) return false;
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return false;
	}

	return true;
}

bool InitGLCommonResources()
{
#ifdef MULTICONTEXT
	SDL_GL_MakeCurrent(s_window, s_mainContext);
#endif

	glGenTextures(1, &s_renderedTexture);
	glBindTexture(GL_TEXTURE_2D, s_renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return true;
}

bool InitGLResourcesForTriangle()
{
#ifdef MULTICONTEXT
	SDL_GL_MakeCurrent(s_window, s_renderContext);
#endif

	if (!CreateShaderProgram("triangle.vert", "triangle.frag", s_triangleProgram))
		return false;
	
	s_MVPMatLoc = glGetUniformLocation(s_triangleProgram, "u_MVPMat");
	s_MVPMat = glm::mat4x4(1.0f);

	s_colorLoc = glGetUniformLocation(s_triangleProgram, "u_color");
	s_color = 0.0f;
	
	// Initialize vertex buffer object.
	glGenBuffers(1, &s_triangleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, s_triangleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_triangleVertices), s_triangleVertices, GL_STATIC_DRAW);

	// Generate render target.
	glGenFramebuffers(1, &s_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, s_frameBuffer);

	glGenRenderbuffers(1, &s_depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, s_depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 640, 480);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, s_depthBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, s_renderedTexture, 0);

	return true;
}

bool InitGLResourcesForTexture()
{
#ifdef MULTICONTEXT
	SDL_GL_MakeCurrent(s_window, s_mainContext);
#endif

	if (!CreateShaderProgram("texture.vert", "texture.frag", s_textureProgram))
		return false;

	s_textureLoc = glGetUniformLocation(s_textureProgram, "u_renderedTexture");

	// Initialize vertex buffer object.

	glGenBuffers(1, &s_squareVertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, s_squareVertexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_squareVertices), s_squareVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &s_squareTexCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, s_squareTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_squareTexCoords), s_squareTexCoords, GL_STATIC_DRAW);

	return true;
}

void RenderTriangleToTexture()
{
	// Render triangle to texture.
#ifdef MULTICONTEXT
	SDL_GL_MakeCurrent(s_window, s_renderContext);
#endif

	glBindFramebuffer(GL_FRAMEBUFFER, s_frameBuffer);
	glViewport(0, 0, 640, 480);

	glClearColor(0.4f, 0.4f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, s_triangleVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glUseProgram(s_triangleProgram);
	glProgramUniformMatrix4fv(s_triangleProgram, s_MVPMatLoc, 1, GL_FALSE, &s_MVPMat[0][0]);
	s_color += 0.0005f;
	if (s_color > 1.0f)
		s_color = 0.0f;
	glProgramUniform1fv(s_triangleProgram, s_colorLoc, 1, &s_color);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glFinish();

	return;
}

void RenderTextureToSurface()
{
	// Render texture to window surface.
#ifdef MULTICONTEXT
	SDL_GL_MakeCurrent(s_window, s_mainContext);
#endif

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 640, 480);

	glClearColor(1.0f, 0.4f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, s_squareVertexVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, s_squareTexCoordVBO);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, s_renderedTexture);
	glUniform1i(s_textureLoc, 0);

	glUseProgram(s_textureProgram);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	SDL_GL_SwapWindow(s_window);

	glFinish();

	return;
}

#ifdef MULTITHREADING
void renderThreadMain()
{
#ifndef MULTICONTEXT
	SDL_GL_MakeCurrent(s_window, s_mainContext);
#endif

	if (!InitGLResourcesForTriangle())
	{
		std::cerr << "Failed to init resources for triangle." << std::endl;
		return;
	}

	while (true)
	{
		std::unique_lock<std::mutex> lk(s_m);
		s_cv.wait(lk, [] { return s_render; });
		
		RenderTriangleToTexture();

		s_processed = true;
		s_render = false;
		lk.unlock();
		s_cv.notify_one();

		if (s_windowShouldClose)
			break;
	}

	std::cout << "Received close signal!" << std::endl;
}
#endif

int main(int argc, char *argv[])
{
	if (!InitSDLWindow())
		return 1;
	if (!CreateGLContext())
		return 1;
	if (!InitGLCommonResources())
		return 1;
#ifndef MULTITHREADING
	if (!InitGLResourcesForTriangle())
		return 1;
#endif
	if (!InitGLResourcesForTexture())
		return 1;

	SDL_Event event;

#ifdef MULTITHREADING
	std::thread renderThread(renderThreadMain);

	{
		std::lock_guard<std::mutex> lk(s_m);
		s_render = true;
		s_processed = false;
	}
	s_cv.notify_one();
#endif

	while (!s_windowShouldClose)
	{
#ifdef MULTITHREADING
		{
			std::unique_lock<std::mutex> lk(s_m);
			s_cv.wait(lk, [] { return s_processed; });
		}
#endif

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_CLOSE:
					s_windowShouldClose = true;
					break;
				}
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					s_windowShouldClose = true;
					break;
				}

				break;
			}
		}
#ifndef MULTITHREADING
		RenderTriangleToTexture();
		RenderTextureToSurface();
#else
		RenderTextureToSurface();
		{
			std::lock_guard<std::mutex> lk(s_m);
			s_render = true;
			s_processed = false;
		}
		s_cv.notify_one();
#endif
	}

#ifdef MULTITHREADING
	renderThread.join();
#endif

	SDL_GL_DeleteContext(s_mainContext);
	SDL_DestroyWindow(s_window);
	SDL_Quit();

	return 0;
}
