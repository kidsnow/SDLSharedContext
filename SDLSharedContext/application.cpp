#include "application.h"
#include "stb_image.h"

unsigned char* Application::LoadImage(const char* fileName, int& width, int& height)
{
	int temp;
	return stbi_load("PolygonPlanet.png", &width, &height, &temp, 0);
}

void Application::FreeImage(unsigned char* data)
{
	stbi_image_free(data);
}