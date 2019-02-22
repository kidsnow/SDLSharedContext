#pragma once

#include "application.h"

class ShareDstTextureApp : public Application
{
public:
	ShareDstTextureApp();
	~ShareDstTextureApp();

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