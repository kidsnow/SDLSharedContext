﻿#include <iostream>
#include <thread>

#define GLEW_NO_GLU
#include "GL/glew.h"
#define SDL_MAIN_HANDLED
#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "glm/glm.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.h"
#include "texture.h"

typedef struct _testWindow
{
	SDL_Window* handle;
	SDL_GLContext context;
	std::string name;
	int posX, posY;
	int width, height;
} testWindow;
bool windowShouldClose = false;
testWindow s_mainWindow, s_subWindow;
static const GLfloat s_triangleVertices[] =
{
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,	// lower left
	 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// lower right
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,	// upper left
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,	// upper left
	 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// lower right
	 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,	// upper right
};
Texture2D* s_texture2D;

bool CreateAndShowWindow(testWindow& window)
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

bool InitSDLWindow()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SDL_Log("Couldn't initialize video driver: %s\n", SDL_GetError());
		return false;
	}

	s_mainWindow.name = "Main";
	s_mainWindow.posX = 100;
	s_mainWindow.posY = 100;
	s_mainWindow.width = 960;
	s_mainWindow.height = 540;

	s_subWindow.name = "Sub";
	s_subWindow.posX = 1200;
	s_subWindow.posY = 100;
	s_subWindow.width = 640;
	s_subWindow.height = 360;

	if (!CreateAndShowWindow(s_mainWindow))
		return false;
	if (!CreateAndShowWindow(s_subWindow))
		return false;

	return true;
}

bool CreateGLContext()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	s_mainWindow.context = SDL_GL_CreateContext(s_mainWindow.handle);
	if (!s_mainWindow.context) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext(): %s\n", SDL_GetError());
		SDL_DestroyWindow(s_mainWindow.handle);
		SDL_Quit();
		return 1;
	}

	SDL_GL_MakeCurrent(s_mainWindow.handle, s_mainWindow.context);

	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	s_subWindow.context = SDL_GL_CreateContext(s_subWindow.handle);
	if (!s_mainWindow.context) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext(): %s\n", SDL_GetError());
		SDL_DestroyWindow(s_subWindow.handle);
		SDL_Quit();
		return 1;
	}
	SDL_GL_MakeCurrent(s_mainWindow.handle, s_mainWindow.context);

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

bool InitGLCommonResources()
{
	int width, height, nrChannels;
	unsigned char *data = stbi_load("PolygonPlanet.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		s_texture2D = new Texture2D();
		s_texture2D->Generate(width, height, data);
	}
	else
	{
		std::cerr << "Failed to load texture." << std::endl;
	}

	stbi_image_free(data);

	return true;
}

bool InitGLResourcesForTriangle()
{
	GLuint triangleProgram;
	GLuint triangleVAO;
	GLuint triangleVBO;

	Shader* s_shader = new Shader();
	s_shader->CompileRenderingShader("triangle.vert", "triangle.frag");
	s_shader->Use();

	// Initialize vertex array object.
	glGenVertexArrays(1, &triangleVAO);
	glBindVertexArray(triangleVAO);
	
	// Initialize vertex buffer object.
	glGenBuffers(1, &triangleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_triangleVertices), s_triangleVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glActiveTexture(GL_TEXTURE0);
	s_texture2D->Bind();

	return true;
}

void RenderTriangle()
{
	glClearColor(0.4f, 0.4f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	SDL_GL_SwapWindow(s_mainWindow.handle);

	glFinish();

	return;
}

void subWindowThreadMain()
{
	SDL_GL_MakeCurrent(s_subWindow.handle, s_subWindow.context);
	if (!InitGLResourcesForTriangle())
		return;

	while (!windowShouldClose)
	{
		RenderTriangle();
		SDL_GL_SwapWindow(s_subWindow.handle);
	}

	return;
}

int main(int argc, char *argv[])
{
	if (!InitSDLWindow())
		return 1;
	if (!CreateGLContext())
		return 1;
	if (!InitGLCommonResources())
		return 1;

	std::thread subWindowThread(subWindowThreadMain);

	if (!InitGLResourcesForTriangle())
		return 1;

	SDL_Event event;

	while (!windowShouldClose)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_CLOSE:
					windowShouldClose = true;
					break;
				}
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					windowShouldClose = true;
					break;
				}

				break;
			}
		}
		RenderTriangle();
	}

	subWindowThread.join();

	SDL_GL_DeleteContext(s_subWindow.context);
	SDL_DestroyWindow(s_subWindow.handle);
	SDL_GL_DeleteContext(s_mainWindow.context);
	SDL_DestroyWindow(s_mainWindow.handle);
	SDL_Quit();

	return 0;
}
