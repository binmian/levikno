#include <levikno/levikno.h>


int main(int argc, char** argv)
{
	LvnContextCreateInfo lvnCreateInfo{};
	lvnCreateInfo.logging.enableLogging = true;
	lvnCreateInfo.logging.enableGraphicsApiDebugLogs = true;
	lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
	lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_opengl;

	lvn::createContext(&lvnCreateInfo);

	lvn::renderInit("basicUsage", 800, 600);

	while (lvn::renderWindowOpen())
	{
		lvn::windowPollEvents();

		lvn::drawBegin();
		lvn::drawClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		lvn::drawRect({0.0f, 0.0f}, {50.0f, 50.0f}, {255,255,255,255});
		lvn::drawRect({0.0f, 100.0f}, {120.0f, 50.0f}, {255,0,127,255});
		lvn::drawRect({cos(lvn::getContextTime()) * 200.0f, sin(lvn::getContextTime()) * 200.0f}, {20.0f, 80.0f}, {127,127,255,255});

		lvn::drawTriangle({-400.0f, 0.0f}, {-350.0f, 50.0f}, {-300.0f, 0.0f}, {255,255,0,255});
		lvn::drawTriangle({-400.0f, -400.0f}, {-350.0f, -350.0f}, {-300.0f, sin(lvn::getContextTime() * 2) * 25.0f - 375.0f}, {255,0,255,255});

		lvn::drawEnd();
	}

	lvn::terminateContext();

	return 0;
}
