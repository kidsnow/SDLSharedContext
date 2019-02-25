#include "share_src_texture.h"

ShareSrcTextureApp::ShareSrcTextureApp() {}
ShareSrcTextureApp::~ShareSrcTextureApp() {}

void ShareSrcTextureApp::SubWindowThreadMain()
{
	SDL_GL_MakeCurrent(m_subWindow.handle, m_subWindow.context);

	if (!InitResourcesForRectangle())
		return;

	while (!m_windowShouldClose)
	{
		RenderRectangle(0);
		SDL_GL_SwapWindow(m_subWindow.handle);
	}

	return;
}

bool ShareSrcTextureApp::Initialize()
{
	if (!InitSDLWindow())
		return 1;
	if (!InitGLContext())
		return 1;
	if (!InitSrcTexture())
		return 1;

	m_subWindowThread = new std::thread(&ShareSrcTextureApp::SubWindowThreadMain, this);

	if (!InitResourcesForRectangle())
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
		RenderRectangle(0);
		SDL_GL_SwapWindow(m_mainWindow.handle);
	}

	m_subWindowThread->join();
}