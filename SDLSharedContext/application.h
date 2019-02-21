#pragma once

#include "SDL/SDL.h"

#include <iostream>
#include <thread>

#include "GL/glew.h"
#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "glm/glm.hpp"

class Application
{
public:
	Application() {}
	~Application() {}

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
	unsigned char* LoadImage(const char* fileName, int& width, int& height);
	void FreeImage(unsigned char* data);
};