#pragma once

#include "SDL/SDL.h"

#include <iostream>
#include <thread>

#include "GL/glew.h"
#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "glm/glm.hpp"

#include "shader.h"

class Application
{
public:
	Application();
	~Application();

public:
	virtual bool Initialize() = 0;
	virtual void Run() = 0;

protected:
	typedef struct _testWindow
	{
		SDL_Window* handle;
		SDL_GLContext context;
		std::string name;
		int posX, posY;
		int width, height;
	} testWindow;

	testWindow m_mainWindow, m_subWindow;
	std::thread* m_subWindowThread;
	bool m_windowShouldClose = false;
	Shader* m_shader;
	GLfloat* m_rectangleVertices;
	GLuint m_triangleVAO;
	GLuint m_srcTexture;
	GLuint m_dstTexture;
	GLuint m_mainWindowOffscreenBuffer;
	GLuint m_subWindowOffscreenBuffer;

protected:	// Utility functions.
	void FBO_2_PPM_file(const char* fileName, int width, int height);
	unsigned char* LoadImage(const char* fileName, int& width, int& height);
	void FreeImage(unsigned char* data);

protected:	// Common logic for test applications.
	bool CreateAndShowWindow(testWindow& window);
	bool InitSDLWindow();
	bool InitGLContext();
	bool InitSrcTexture();
	bool InitDstTexture(int width, int hegiht);
	bool InitOffscreenFramebuffer(GLuint& offscreenBuffer);
	bool InitResourcesForRectangle();
	void RenderRectangle(GLuint target);
};