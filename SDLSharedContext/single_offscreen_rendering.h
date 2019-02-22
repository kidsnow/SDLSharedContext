#pragma once

#include "application.h"

class SingleOffscreenRenderingApp : public Application
{
public:
	SingleOffscreenRenderingApp();
	~SingleOffscreenRenderingApp();

public:
	virtual bool Initialize();
	virtual void Run();

private:
	GLuint m_dstTexture;
	GLuint m_mainWindowOffscreenBuffer;
	GLuint m_subWindowOffscreenBuffer;

private:
	bool InitGLResourcesForFramebuffer(GLuint& offscreenBuffer, GLuint& dstTexture, int width, int height);
	void RenderTriangle();
	void SubWindowThreadMain();
};