#include <levikno/levikno.h>

int main(int argc, char** argv)
{
	// [Create Context]
	// create the context to load the library

	LvnContextCreateInfo lvnCreateInfo{};
	lvnCreateInfo.logging.enableLogging = true;
	lvnCreateInfo.logging.enableGraphicsApiDebugLogs = true;
	lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
	lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;

	lvn::createContext(&lvnCreateInfo);


	// window create info struct
	LvnWindowCreateInfo windowInfo{};
	windowInfo.title = "simpleWindow";
	windowInfo.width = 800;
	windowInfo.height = 600;
	windowInfo.minWidth = 300;
	windowInfo.minHeight = 200;

	LvnWindow* window;
	lvn::createWindow(&window, &windowInfo);

	// [Main Render Loop]
	while (lvn::windowOpen(window))
	{
		lvn::windowUpdate(window);
		lvn::windowPollEvents();


		// get next window swapchain image
		lvn::renderBeginNextFrame(window);
		lvn::renderBeginCommandRecording(window);

		// set background color and begin render pass
		lvn::renderCmdBeginRenderPass(window, 0.0f, 0.0f, 0.0f, 1.0f);

		// [Rendering commands would be placed here]

		// end render pass and submit rendering
		lvn::renderCmdEndRenderPass(window);
		lvn::renderEndCommandRecording(window);
		lvn::renderDrawSubmit(window); // note that this function is where we actually submit our render data to the GPU
	}

	// destroy window after window closes
	lvn::destroyWindow(window);

	// terminate the context at the end of the program
	lvn::terminateContext();

	return 0;
}
