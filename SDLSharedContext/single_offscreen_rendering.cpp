#include "single_offscreen_rendering.h"

SingleOffscreenRenderingApp::SingleOffscreenRenderingApp() {}
SingleOffscreenRenderingApp::~SingleOffscreenRenderingApp() {}

bool SingleOffscreenRenderingApp::Initialize()
{
	if (!InitSDLWindow())
		return 1;
	if (!InitGLContext())
		return 1;
	if (!InitSrcTexture())
		return 1;
	if (!InitDstTexture(m_mainWindow.width, m_mainWindow.height))
		return 1;
	if (!InitResourcesForRectangle())
		return 1;
	if (!InitOffscreenFramebuffer(m_mainWindowOffscreenBuffer))
		return 1;

	return true;
}

void SingleOffscreenRenderingApp::Run()
{
	SDL_Event event;

	RenderRectangle(m_mainWindowOffscreenBuffer);
	FBO_2_PPM_file("mainWindow.ppm", m_mainWindow.width, m_mainWindow.height);
}