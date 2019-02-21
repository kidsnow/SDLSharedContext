#include "application.h"
#include "share_src_texture.h"

int main(int argc, char** argv)
{
	ShareSrcTextureApp* app = new ShareSrcTextureApp();

	app->Initialize();
	app->Run();

	delete app;

	return 0;
}