// Link statically with GLEW
#define GLEW_STATIC

// Headers
#include <GL/glew.h>
#include <SOIL.h>
#include <SFML/Window.hpp>

// Shader sources
const GLchar* vertexSource = R"glsl(
    #version 150 core
    in vec2 position;
    in vec3 color;
    in vec2 texcoord;
    out vec3 Color;
    out vec2 Texcoord;
    void main()
    {
        Color = color;
        Texcoord = texcoord;
        gl_Position = vec4(position, 0.0, 1.0);
    }
)glsl";
const GLchar* fragmentSource = R"glsl(
    #version 150 core
    in vec3 Color;
    in vec2 Texcoord;
    out vec4 outColor;
    uniform sampler2D texKitten;
    uniform sampler2D texPuppy;
    void main()
    {
        outColor = mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), 0.5);
    }
)glsl";

int main()
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.majorVersion = 3;
	settings.minorVersion = 2;

	sf::Window window(sf::VideoMode(800, 600, 32), "OpenGL", sf::Style::Titlebar | sf::Style::Close, settings);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	// Create Vertex Array Object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint vbo;
	glGenBuffers(1, &vbo);

	GLfloat vertices[] = {
		//  Position      Color             Texcoords
			-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
			 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
			-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create an element array
	GLuint ebo;
	glGenBuffers(1, &ebo);

	GLuint elements[] = {
		0, 1, 2,
		2, 3, 0
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	// Create and compile the vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	// Create and compile the fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	// Link the vertex and fragment shader into a shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

	GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

	// Load textures
	GLuint textures[2];
	glGenTextures(2, textures);

	int width, height;
	unsigned char* image;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	image = SOIL_load_image("sample.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	image = SOIL_load_image("sample2.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	bool running = true;
	while (running)
	{
		sf::Event windowEvent;
		while (window.pollEvent(windowEvent))
		{
			switch (windowEvent.type)
			{
			case sf::Event::Closed:
				running = false;
				break;
			}
		}

		// Clear the screen to black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw a rectangle from the 2 triangles using 6 indices
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Swap buffers
		window.display();
	}

	glDeleteTextures(2, textures);

	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);

	glDeleteVertexArrays(1, &vao);

	window.close();

	return 0;
}




#include <iostream>
#define SDL_MAIN_HANDLED
#include "SDL/SDL.h"
#include "GL/glew.h"
#include "SDL/SDL_opengl.h"
#include "glm/gtc/matrix_transform.hpp"

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
GLuint s_squareVertexVBO;
GLuint s_MVPMatLoc;
GLuint s_textureLoc;
glm::mat4x4 s_MVPMat;

static const GLfloat s_squareVertices[] =
{
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,   // lower left
	 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,	  // lower right
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,	  // upper left
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,	  // upper left
	 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,	  // lower right
	 1.0f,  1.0f, 0.0f, 1.0f, 1.0f	  // upper right
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
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
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
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	s_mainContext = SDL_GL_CreateContext(s_window);
	if (!s_mainContext) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext(): %s\n", SDL_GetError());
		SDL_DestroyWindow(s_window);
		SDL_Quit();
		return 1;
	}

	SDL_GL_MakeCurrent(s_window, s_mainContext);
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	s_renderContext = SDL_GL_CreateContext(s_window);
	if (!s_renderContext) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext(): %s\n", SDL_GetError());
		SDL_DestroyWindow(s_window);
		SDL_Quit();
		return 1;
	}

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to init GLEW..." << std::endl;
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


bool InitGLResources()
{
	if (!CreateShaderProgram("triangle.vert", "triangle.frag", s_triangleProgram))
		return false;
	if (!CreateShaderProgram("texture.vert", "texture.frag", s_textureProgram))
		return false;

	s_MVPMatLoc = glGetUniformLocation(s_triangleProgram, "u_MVPMat");
	s_MVPMat = glm::mat4x4(1.0f);

	s_textureLoc = glGetUniformLocation(s_textureProgram, "u_renderedTexture");

	// Initialize vertex buffer object.
	glGenBuffers(1, &s_triangleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, s_triangleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_triangleVertices), s_triangleVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &s_squareVertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, s_squareVertexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_squareVertices), s_squareVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Generate render target.
	glGenFramebuffers(1, &s_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, s_frameBuffer);

	glGenTextures(1, &s_renderedTexture);
	glBindTexture(GL_TEXTURE_2D, s_renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &s_depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, s_depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 640, 480);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, s_depthBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, s_renderedTexture, 0);

	return true;
}

void Render()
{
	// Render triangle to texture.

	glBindFramebuffer(GL_FRAMEBUFFER, s_frameBuffer);
	glViewport(0, 0, 640, 480);

	glClearColor(0.4f, 0.4f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, s_triangleVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glUseProgram(s_triangleProgram);
	glProgramUniformMatrix4fv(s_triangleProgram, s_MVPMatLoc, 1, GL_FALSE, &s_MVPMat[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	// Render texture to window surface.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 640, 480);

	glClearColor(1.0f, 0.4f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, s_squareVertexVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, s_squareVertexVBO);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, s_renderedTexture);
	glUniform1i(s_textureLoc, 0);

	glUseProgram(s_textureProgram);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	SDL_GL_SwapWindow(s_window);
	return;
}

int main(int argc, char *argv[])
{
	if (!InitSDLWindow())
		return 1;
	if (!CreateGLContext())
		return 1;
	if (!InitGLResources())
		return 1;

	SDL_Event event;

	while (!s_windowShouldClose)
	{
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

		Render();
	}

	SDL_GL_DeleteContext(s_mainContext);
	SDL_DestroyWindow(s_window);
	SDL_Quit();

	return 0;
}