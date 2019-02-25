#include "application.h"
#include "share_src_texture.h"
#include "share_dst_texture.h"
#include "single_offscreen_rendering.h"

int main(int argc, char** argv)
{
	Application* app = new ShareSrcTextureApp();
	//Application* app = new SingleOffscreenRenderingApp();
	//Application* app = new ShareDstTextureApp();

	app->Initialize();
	app->Run();

	delete app;

	return 0;
}