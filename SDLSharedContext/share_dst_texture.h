#pragma once

#include "application.h"
#include "shader.h"

class ShareDstTextureApp : public Application
{
public:
	ShareDstTextureApp();
	~ShareDstTextureApp();

public:
	virtual bool Initialize();
	virtual void Run();

private:
	testWindow m_mainWindow, m_subWindow;
	std::thread* m_subWindowThread;
	bool m_windowShouldClose = false;
	GLfloat* m_rectangleVertices;
	GLuint m_texture;

private:
	bool CreateAndShowWindow(testWindow& window);
	bool InitSDLWindow();
	bool CreateGLContext();
	bool InitGLCommonResources();
	bool InitGLResourcesForTriangle();
	void RenderTriangle();
	void SubWindowThreadMain();
};