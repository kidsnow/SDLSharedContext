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
	GLfloat* m_rectangleVertices;
	GLuint m_srcTexture;
	GLuint m_dstTexture;
	//GLuint m_mainWindowDstTexture;
	//GLuint m_subWindowDstTexture;
	GLuint m_mainWindowOffscreenBuffer;
	GLuint m_subWindowOffscreenBuffer;

private:
	bool CreateAndShowWindow(testWindow& window);
	bool InitSDLWindow();
	bool CreateGLContext();
	bool InitGLCommonResources();
	bool InitGLResourcesForTriangle();
	bool InitGLResourcesForFramebuffer(GLuint& offscreenBuffer, GLuint& dstTexture, int width, int height);
	void RenderTriangle();
	void SubWindowThreadMain();
};