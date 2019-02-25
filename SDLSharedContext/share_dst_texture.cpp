#include "share_dst_texture.h"

ShareDstTextureApp::ShareDstTextureApp() {}
ShareDstTextureApp::~ShareDstTextureApp() {}

void ShareDstTextureApp::SubWindowThreadMain()
{
	SDL_GL_MakeCurrent(m_subWindow.handle, m_subWindow.context);

	if (!InitOffscreenFramebuffer(m_subWindowOffscreenBuffer))
		return;
	if (!InitResourcesForRectangle())
		return;

	RenderRectangle(m_subWindowOffscreenBuffer);
	FBO_2_PPM_file("subWindow.ppm", m_subWindow.width, m_subWindow.height);

	return;
}

bool ShareDstTextureApp::Initialize()
{
	if (!InitSDLWindow())
		return 1;
	if (!InitGLContext())
		return 1;
	if (!InitSrcTexture())
		return 1;
	if (!InitDstTexture(m_subWindow.width, m_subWindow.height))
		return 1;

	m_subWindowThread = new std::thread(&ShareDstTextureApp::SubWindowThreadMain, this);

	if (!InitResourcesForRectangle())
		return 1;

	return true;
}

void ShareDstTextureApp::Run()
{
	SDL_Event event;

	RenderRectangle(0);
	SDL_GL_SwapWindow(m_mainWindow.handle);

	m_subWindowThread->join();
}