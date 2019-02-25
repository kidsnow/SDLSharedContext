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
};