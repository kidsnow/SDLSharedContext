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
	void SubWindowThreadMain();
};