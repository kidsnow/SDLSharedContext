#include "application.h"
#include "stb/stb_image.h"

Application::Application() {}

Application::~Application()
{
	SDL_GL_DeleteContext(m_subWindow.context);
	SDL_DestroyWindow(m_subWindow.handle);
	SDL_GL_DeleteContext(m_mainWindow.context);
	SDL_DestroyWindow(m_mainWindow.handle);
	SDL_Quit();
}

void Application::FBO_2_PPM_file(const char* fileName, int width, int height)
{
	FILE *fp;

	unsigned char   *pixels = (unsigned char*)malloc(width*height * 3);

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	fp = fopen(fileName, "wt");
	fprintf(fp, "P3\n");
	fprintf(fp, "%d %d\n", width, height);
	fprintf(fp, "255\n");

	for (int i = height - 1; i >= 0; i--)
	{
		for (int j = 0; j < width; j++)
		{
			int idx = (i * width + j) * 3;
			fprintf(fp, "%u %u %u ",
					(unsigned int)pixels[idx],
					(unsigned int)pixels[idx + 1],
					(unsigned int)pixels[idx + 2]);
		}
	}
	fclose(fp);
	free(pixels);
}

unsigned char* Application::LoadImage(const char* fileName, int& width, int& height)
{
	int temp;
	return stbi_load("PolygonPlanet.png", &width, &height, &temp, 0);
}

void Application::FreeImage(unsigned char* data)
{
	stbi_image_free(data);
}

bool Application::CreateAndShowWindow(testWindow& window)
{
	uint32_t windowFlags;

	windowFlags = 0;
	windowFlags |= SDL_WINDOW_RESIZABLE;
	windowFlags |= SDL_WINDOW_OPENGL;

	window.handle = SDL_CreateWindow(window.name.c_str(),
		window.posX, window.posY,
		window.width, window.height, windowFlags);
	if (window.handle == NULL)
	{
		SDL_Log("Couldn't create window: %s\n", SDL_GetError());
		SDL_Quit();
		return false;
	}

	int createdWindowWidth, createdWindowHeight;
	SDL_GetWindowSize(window.handle, &createdWindowWidth, &createdWindowHeight);
	if (!(windowFlags & SDL_WINDOW_RESIZABLE) &&
		(createdWindowWidth != window.width || createdWindowHeight != window.height)) {
		printf("Window requested size %dx%d, got %dx%d\n",
			window.width, window.height, createdWindowWidth, createdWindowHeight);
		window.width = createdWindowWidth;
		window.height = createdWindowHeight;
	}

	SDL_ShowWindow(window.handle);

	return true;
}

bool Application::InitSDLWindow()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SDL_Log("Couldn't initialize video driver: %s\n", SDL_GetError());
		return false;
	}

	m_mainWindow.name = "Main";
	m_mainWindow.posX = 100;
	m_mainWindow.posY = 100;
	m_mainWindow.width = 960;
	m_mainWindow.height = 540;

	m_subWindow.name = "Sub";
	m_subWindow.posX = 1200;
	m_subWindow.posY = 100;
	m_subWindow.width = 640;
	m_subWindow.height = 360;

	if (!CreateAndShowWindow(m_mainWindow))
		return false;
	if (!CreateAndShowWindow(m_subWindow))
		return false;

	return true;
}

bool Application::InitGLContext()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	m_mainWindow.context = SDL_GL_CreateContext(m_mainWindow.handle);
	if (!m_mainWindow.context) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext(): %s\n", SDL_GetError());
		SDL_DestroyWindow(m_mainWindow.handle);
		SDL_Quit();
		return 1;
	}

	SDL_GL_MakeCurrent(m_mainWindow.handle, m_mainWindow.context);

	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	m_subWindow.context = SDL_GL_CreateContext(m_subWindow.handle);
	if (!m_subWindow.context) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext(): %s\n", SDL_GetError());
		SDL_DestroyWindow(m_subWindow.handle);
		SDL_Quit();
		return 1;
	}
	SDL_GL_MakeCurrent(m_mainWindow.handle, m_mainWindow.context);

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

bool Application::InitSrcTexture()
{
	// Initialize source texture.
	int width, height;
	unsigned char *data = LoadImage("PolygonPlanet.png", width, height);
	if (data)
	{
		glGenTextures(1, &m_srcTexture);
		glBindTexture(GL_TEXTURE_2D, m_srcTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFinish();
	}
	else
	{
		std::cerr << "Failed to load texture." << std::endl;
	}

	FreeImage(data);

	return true;
}

bool Application::InitDstTexture(int width, int height)
{
	glGenTextures(1, &m_dstTexture);
	glBindTexture(GL_TEXTURE_2D, m_dstTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFinish();

	return true;
}

bool Application::InitOffscreenFramebuffer(GLuint& offscreenBuffer)
{
	glGenFramebuffers(1, &offscreenBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, offscreenBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_dstTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Framebuffer not complete: " << result << std::endl;
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

bool Application::InitResourcesForRectangle()
{
	m_rectangleVertices = new GLfloat[30]
	{
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,	// lower left
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// lower right
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,	// upper left
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,	// upper left
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// lower right
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,	// upper right
	};
	m_shader = new Shader();
	m_shader->CompileRenderingShader("triangle.vert", "triangle.frag");

	// Initialize vertex array object.
	glGenVertexArrays(1, &m_triangleVAO);
	glBindVertexArray(m_triangleVAO);

	// Initialize vertex buffer object.
	GLuint triangleVBO;
	glGenBuffers(1, &triangleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 5, m_rectangleVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

void Application::RenderRectangle(GLuint target)
{
	glBindFramebuffer(GL_FRAMEBUFFER, target);
	glClearColor(0.4f, 0.4f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(m_triangleVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_srcTexture);

	m_shader->Use();

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glFinish();

	return;
}