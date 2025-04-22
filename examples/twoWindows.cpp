#include <levikno/levikno.h>


void renderWindow(LvnWindow* window)
{
	lvn::windowSetContextCurrent(window);

	// [Main Render Loop]
	while (lvn::windowOpen(window))
	{
		lvn::windowUpdate(window);

		lvn::renderBeginNextFrame(window);
		lvn::renderBeginCommandRecording(window);

		lvn::renderClearColor(window, 0.0f, 0.0f, 0.0f, 1.0f);
		lvn::renderCmdBeginRenderPass(window);


		lvn::renderCmdEndRenderPass(window);
		lvn::renderEndCommandRecording(window);
		lvn::renderDrawSubmit(window);
	}

	lvn::destroyWindow(window);
}

int main(int argc, char** argv)
{
	LvnContextCreateInfo lvnCreateInfo{};
	lvnCreateInfo.logging.enableLogging = true;
	lvnCreateInfo.logging.enableGraphicsApiDebugLogs = true;
	lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
	lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_opengl;

	lvn::createContext(&lvnCreateInfo);

	// window create info struct
	LvnWindowCreateInfo windowInfo{};
	windowInfo.title = "window1";
	windowInfo.width = 800;
	windowInfo.height = 600;
	windowInfo.minWidth = 300;
	windowInfo.minHeight = 200;

	LvnWindow* window1;
	lvn::createWindow(&window1, &windowInfo);

	windowInfo.title = "window2";

	LvnWindow* window2;
	lvn::createWindow(&window2, &windowInfo);

	lvn::windowSetContextCurrent(nullptr);

	std::thread t1(renderWindow, window1);
	std::thread t2(renderWindow, window2);

	while (lvn::windowOpen(window1) || lvn::windowOpen(window2))
	{
		lvn::windowPollEvents();
	}

	t1.join();
	t2.join();

	lvn::terminateContext();

	return 0;
}
