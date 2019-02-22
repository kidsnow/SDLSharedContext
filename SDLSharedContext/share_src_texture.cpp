#include "share_src_texture.h"


ShareSrcTextureApp::ShareSrcTextureApp() {}
ShareSrcTextureApp::~ShareSrcTextureApp() {}

void ShareSrcTextureApp::RenderTriangle()
{
	glClearColor(0.4f, 0.4f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	SDL_GL_SwapWindow(m_mainWindow.handle);

	glFinish();

	return;
}

void ShareSrcTextureApp::SubWindowThreadMain()
{
	SDL_GL_MakeCurrent(m_subWindow.handle, m_subWindow.context);
	if (!InitGLResourcesForTriangle())
		return;

	while (!m_windowShouldClose)
	{
		RenderTriangle();
		SDL_GL_SwapWindow(m_subWindow.handle);
	}

	return;
}

bool ShareSrcTextureApp::Initialize()
{
	if (!InitSDLWindow())
		return 1;
	if (!CreateGLContext())
		return 1;
	if (!InitGLCommonResources())
		return 1;

	m_subWindowThread = new std::thread(&ShareSrcTextureApp::SubWindowThreadMain, this);

	if (!InitGLResourcesForTriangle())
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
		RenderTriangle();
	}

	m_subWindowThread->join();
}