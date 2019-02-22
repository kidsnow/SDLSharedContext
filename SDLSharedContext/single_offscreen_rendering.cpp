#include "single_offscreen_rendering.h"

SingleOffscreenRenderingApp::SingleOffscreenRenderingApp() {}
SingleOffscreenRenderingApp::~SingleOffscreenRenderingApp() {}

bool SingleOffscreenRenderingApp::InitGLResourcesForFramebuffer(GLuint& offscreenBuffer, GLuint& dstTexture, int width, int height)
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

void SingleOffscreenRenderingApp::RenderTriangle()
{
	glClearColor(0.4f, 0.4f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	SDL_GL_SwapWindow(m_mainWindow.handle);

	glFinish();

	return;
}

bool SingleOffscreenRenderingApp::Initialize()
{
	if (!InitSDLWindow())
		return 1;
	if (!CreateGLContext())
		return 1;
	if (!InitGLCommonResources())
		return 1;
	if (!InitGLResourcesForTriangle())
		return 1;

	glGenTextures(1, &m_dstTexture);
	glBindTexture(GL_TEXTURE_2D, m_dstTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_subWindow.width, m_subWindow.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glFinish();

	return true;
}

void SingleOffscreenRenderingApp::Run()
{
	SDL_Event event;

	glBindFramebuffer(GL_FRAMEBUFFER, m_mainWindowOffscreenBuffer);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_srcTexture);
	RenderTriangle();
}