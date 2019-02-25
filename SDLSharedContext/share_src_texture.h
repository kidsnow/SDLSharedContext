#pragma once

#include "application.h"

class ShareSrcTextureApp : public Application
{
public:
	ShareSrcTextureApp();
	~ShareSrcTextureApp();

public:
	virtual bool Initialize();
	virtual void Run();

private:
	void SubWindowThreadMain();
};