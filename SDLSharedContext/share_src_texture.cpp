#include "share_src_texture.h"

#include "stb_image.h"

bool ShareSrcTextureApp::CreateAndShowWindow(testWindow& window)
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

bool ShareSrcTextureApp::InitSDLWindow()
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

bool ShareSrcTextureApp::CreateGLContext()
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

bool ShareSrcTextureApp::InitGLCommonResources()
{
	int width, height, nrChannels;
	unsigned char *data = stbi_load("PolygonPlanet.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		m_texture2D = new Texture2D();
		m_texture2D->Generate(width, height, data);
	}
	else
	{
		std::cerr << "Failed to load texture." << std::endl;
	}

	stbi_image_free(data);

	return true;
}

bool ShareSrcTextureApp::InitGLResourcesForTriangle()
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
	m_texture2D->Bind();

	return true;
}

void ShareSrcTextureApp::RenderTriangle()
{
	glClearColor(0.4f, 0.4f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	SDL_GL_SwapWindow(m_mainWindow.handle);

	glFinish();

	return;
}

void ShareSrcTextureApp::SubWindowThreadMain()
{
	SDL_GL_MakeCurrent(m_subWindow.handle, m_subWindow.context);
	if (!InitGLResourcesForTriangle())
		return;

	while (!m_windowShouldClose)
	{
		RenderTriangle();
		SDL_GL_SwapWindow(m_subWindow.handle);
	}

	return;
}

ShareSrcTextureApp::ShareSrcTextureApp() {}
ShareSrcTextureApp::~ShareSrcTextureApp()
{
	SDL_GL_DeleteContext(m_subWindow.context);
	SDL_DestroyWindow(m_subWindow.handle);
	SDL_GL_DeleteContext(m_mainWindow.context);
	SDL_DestroyWindow(m_mainWindow.handle);
	SDL_Quit();
}


bool ShareSrcTextureApp::Initialize()
{
	if (!InitSDLWindow())
		return 1;
	if (!CreateGLContext())
		return 1;
	if (!InitGLCommonResources())
		return 1;

	m_subWindowThread = new std::thread(&ShareSrcTextureApp::SubWindowThreadMain, this);

	if (!InitGLResourcesForTriangle())
		return 1;

	return true;
}

void ShareSrcTextureApp::Run()
{
	SDL_Event event;

	while (!m_windowShouldClose)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_CLOSE:
					m_windowShouldClose = true;
					break;
				}
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					m_windowShouldClose = true;
					break;
				}

				break;
			}
		}
		RenderTriangle();
	}

	m_subWindowThread->join();
}