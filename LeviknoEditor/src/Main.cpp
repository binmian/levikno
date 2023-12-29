#include <iostream>
#include <levikno/levikno.h>
#include <levikno/Core.h>
#include <levikno/Graphics.h>
#include <format>

int main()
{
	lvn::logInit();

	lvn::createWindowContext(lvn::WindowAPI::glfw);
	lvn::createGraphicsContext(lvn::GraphicsAPI::vulkan);

	lvn::WindowCreateInfo windowInfo{};
	windowInfo.width = 800;
	windowInfo.height = 600;
	windowInfo.title = "window";
	windowInfo.fullscreen = false;
	windowInfo.resizable = true;
	windowInfo.minWidth = 300;
	windowInfo.minHeight = 200;
	windowInfo.maxWidth = -1;
	windowInfo.maxHeight = -1;
	windowInfo.pIcons = nullptr;
	windowInfo.iconCount = 0;

	lvn::Window* window = lvn::createWindow(&windowInfo);

	lvn::vec2 a = { 1.0f, 2.0f };
	lvn::vec2 b = { 3.0f, 5.0f };


	lvn::vec2 d = lvn::vec4(1.0f, 3.0f, 2.0f, 4.0f);

	a[1] = 4.0f;

	float c = a[1];

	a += b;

	lvn::vec4 g = -lvn::vec4(a, 1.0f, 0.0f);
	lvn::mat4 matrix = lvn::mat4(lvn::vec4(1.0f, 2.0f, 7.0f, 4.0f), lvn::vec4(5.0f, 2.0f, 7.0f, 8.0f), lvn::vec4(9.0f, 12.0f, 11.0f, 32.0f), lvn::vec4(15.0f, 14.0f, 18.0f, 26.0f));
	lvn::mat4 matrix2 = lvn::mat4(lvn::vec4(1.0f, 2.0f, 3.0f, 4.0f), lvn::vec4(5.0f, 6.0f, 7.0f, 8.0f), lvn::vec4(9.0f, 10.0f, 11.0f, 12.0f), lvn::vec4(13.0f, 14.0f, 15.0f, 16.0f));
	lvn::mat4x3 matrix4x3 = lvn::mat4x3(1.0f);

	matrix = matrix * matrix2;

	g = matrix * g;

	int frame = 0;

	while (lvn::windowOpen(window))
	{
		lvn::updateWindow(window);
	}


	lvn::terminateWindowContext();
	lvn::terminateGraphicsContext();
	lvn::logTerminate();

	return 0;
}