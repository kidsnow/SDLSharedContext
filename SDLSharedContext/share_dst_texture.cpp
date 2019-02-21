#include "share_dst_texture.h"

bool ShareDstTextureApp::CreateAndShowWindow(testWindow& window)
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

bool ShareDstTextureApp::InitSDLWindow()
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

bool ShareDstTextureApp::CreateGLContext()
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
	if (!m_mainWindow.context) {
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

bool ShareDstTextureApp::InitGLCommonResources()
{
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

bool ShareDstTextureApp::InitGLResourcesForTriangle()
{
	GLuint triangleVAO;
	GLuint triangleVBO;
	m_rectangleVertices = new GLfloat[30]
	{
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,	// lower left
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// lower right
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,	// upper left
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,	// upper left
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// lower right
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,	// upper right
	};
	Shader* s_shader = new Shader();
	s_shader->CompileRenderingShader("triangle.vert", "triangle.frag");
	s_shader->Use();

	// Initialize vertex array object.
	glGenVertexArrays(1, &triangleVAO);
	glBindVertexArray(triangleVAO);

	// Initialize vertex buffer object.
	glGenBuffers(1, &triangleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 5, m_rectangleVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_srcTexture);

	return true;
}

bool ShareDstTextureApp::InitGLResourcesForFramebuffer(GLuint& offscreenBuffer, GLuint& dstTexture, int width, int height)
{
	glGenTextures(1, &dstTexture);
	glBindTexture(GL_TEXTURE_2D, dstTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glGenFramebuffers(1, &offscreenBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, offscreenBuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTexture, 0);

	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		std::cerr << "Failed to generate framebuffer!" << std::endl;
		return false;
	}

	return true;
}

void ShareDstTextureApp::RenderTriangle()
{
	glClearColor(0.4f, 0.4f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	SDL_GL_SwapWindow(m_mainWindow.handle);

	glFinish();

	return;
}

void FBO_2_PPM_file(const char* fileName, int width, int height)
{
	FILE    *output_image;
	int     output_width, output_height;

	output_width = width;
	output_height = height;

	/// READ THE PIXELS VALUES from FBO AND SAVE TO A .PPM FILE
	int             i, j, k;
	unsigned char   *pixels = (unsigned char*)malloc(output_width*output_height * 3);

	/// READ THE CONTENT FROM THE FBO
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, output_width, output_height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	output_image = fopen(fileName, "wt");
	fprintf(output_image, "P3\n");
	fprintf(output_image, "# Created by Ricao\n");
	fprintf(output_image, "%d %d\n", output_width, output_height);
	fprintf(output_image, "255\n");

	k = 0;
	for (i = 0; i < output_width; i++)
	{
		for (j = 0; j < output_height; j++)
		{
			fprintf(output_image, "%u %u %u ", (unsigned int)pixels[k], (unsigned int)pixels[k + 1],
				(unsigned int)pixels[k + 2]);
			k = k + 3;
		}
		fprintf(output_image, "\n");
	}
	free(pixels);
}

void ShareDstTextureApp::SubWindowThreadMain()
{
	SDL_GL_MakeCurrent(m_subWindow.handle, m_subWindow.context);

	if (!InitGLResourcesForTriangle())
		return;
	if (!InitGLResourcesForFramebuffer(m_subWindowOffscreenBuffer, m_subWindowDstTexture, m_subWindow.width, m_subWindow.height))
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, m_subWindowOffscreenBuffer);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_srcTexture);
	RenderTriangle();
	FBO_2_PPM_file("subWindow.ppm", m_subWindow.width, m_subWindow.height);

	return;
}

ShareDstTextureApp::ShareDstTextureApp() {}
ShareDstTextureApp::~ShareDstTextureApp()
{
	SDL_GL_DeleteContext(m_subWindow.context);
	SDL_DestroyWindow(m_subWindow.handle);
	SDL_GL_DeleteContext(m_mainWindow.context);
	SDL_DestroyWindow(m_mainWindow.handle);
	SDL_Quit();
}


bool ShareDstTextureApp::Initialize()
{
	if (!InitSDLWindow())
		return 1;
	if (!CreateGLContext())
		return 1;
	if (!InitGLCommonResources())
		return 1;

	m_subWindowThread = new std::thread(&ShareDstTextureApp::SubWindowThreadMain, this);

	if (!InitGLResourcesForTriangle())
		return 1;
	if (!InitGLResourcesForFramebuffer(m_mainWindowOffscreenBuffer, m_mainWindowDstTexture, m_mainWindow.width, m_mainWindow.height))
		return 1;

	return true;
}

void ShareDstTextureApp::Run()
{
	SDL_Event event;

	glBindFramebuffer(GL_FRAMEBUFFER, m_mainWindowOffscreenBuffer);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_srcTexture);
	RenderTriangle();
	FBO_2_PPM_file("mainWindow.ppm", m_mainWindow.width, m_mainWindow.height);

	m_subWindowThread->join();
}