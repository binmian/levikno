#include <levikno/levikno.h>

#include <vector>


bool keyHold(LvnKeyHoldEvent* event, void* userData)
{
	LVN_INFO("%s, key: %d, repeat: %s", event->name, event->keyCode, event->repeat ? "true" : "false");
	return true;
}

bool keyPressed(LvnKeyPressedEvent* event, void* userData)
{
	LVN_INFO("%s, key: %d", event->name, event->keyCode);
	return true;
}

bool keyReleased(LvnKeyReleasedEvent* event, void* userData)
{
	LVN_INFO("%s, key: %d", event->name, event->keyCode);
	return true;
}

bool keyTyped(LvnKeyTypedEvent* event, void* userData)
{
	LVN_INFO("%s, key: %c", event->name, event->key);
	return true;
}

bool mouseButtonPressed(LvnMouseButtonPressedEvent* event, void* userData)
{
	LVN_INFO("%s, button: %u", event->name, event->buttonCode);
	return true;
}

bool mouseButtonReleased(LvnMouseButtonReleasedEvent* event, void* userData)
{
	LVN_INFO("%s, button: %u", event->name, event->buttonCode);
	return true;
}

bool mouseMovedEvent(LvnMouseMovedEvent* event, void* userData)
{
	LVN_INFO("%s, (x:%d,y:%d)", event->name, event->x, event->y);
	return true;
}

bool mouseScrolledEvent(LvnMouseScrolledEvent* event, void* userData)
{
	LVN_INFO("%s, (x:%.2f,y:%.2f)", event->name, event->x, event->y);
	return true;
}

bool windowClosedEvent(LvnWindowCloseEvent* event, void* userData)
{
	LVN_INFO("%s", event->name);
	return true;
}

bool windowFrameBufferResizedEvent(LvnWindowFramebufferResizeEvent* event, void* userData)
{
	LVN_INFO("%s, (w:%u,h:%u)", event->name, event->width, event->height);
	return true;
}

bool windowFocusedEvent(LvnWindowFocusEvent* event, void* userData)
{
	LVN_INFO("%s", event->name);
	return true;
}

bool windowLostFocusedEvent(LvnWindowLostFocusEvent* event, void* userData)
{
	LVN_INFO("%s", event->name);
	return true;
}

bool windowMovedEvent(LvnWindowMovedEvent* event, void* userData)
{
	LVN_INFO("%s, (x:%d,y:%d)", event->name, event->x, event->y);
	return true;
}

bool windowResizeEvent(LvnWindowResizeEvent* event, void* userData)
{
	LVN_INFO("%s, (x:%d,y:%d)", event->name, event->width, event->height);
	return true;
}

void eventCallBackFn(LvnEvent* event)
{
	lvn::dispatchKeyHoldEvent(event, keyHold);
	lvn::dispatchKeyPressedEvent(event, keyPressed);
	lvn::dispatchKeyReleasedEvent(event, keyReleased);
	lvn::dispatchKeyTypedEvent(event, keyTyped);
	lvn::dispatchMouseButtonPressedEvent(event, mouseButtonPressed);
	lvn::dispatchMouseButtonReleasedEvent(event, mouseButtonReleased);
	lvn::dispatchMouseMovedEvent(event, mouseMovedEvent);
	lvn::dispatchMouseScrolledEvent(event, mouseScrolledEvent);
	lvn::dispatchWindowCloseEvent(event, windowClosedEvent);
	lvn::dispatchWindowFramebufferResizeEvent(event, windowFrameBufferResizedEvent);
	lvn::dispatchWindowFocusEvent(event, windowFocusedEvent);
	lvn::dispatchWindowLostFocusEvent(event, windowLostFocusedEvent);
	lvn::dispatchWindowMovedEvent(event, windowMovedEvent);
	lvn::dispatchWindowResizeEvent(event, windowResizeEvent);
}

int main(int argc, char** argv)
{
	// [Create Context]
	// create the context to load the library

	LvnContextCreateInfo lvnCreateInfo{};
	lvnCreateInfo.logging.enableLogging = true;
	lvnCreateInfo.logging.enableVulkanValidationLayers = true;
	lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
	lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;

	lvn::createContext(&lvnCreateInfo);


	// [Choose Device]
	// choose a physical device to render to

	uint32_t deviceCount = 0;
	std::vector<LvnPhysicalDevice*> devices;

	// first get number of devices, note that the first parameter is null
	lvn::getPhysicalDevices(nullptr, &deviceCount);

	// get an array of physical devices now that we know the number of devices
	devices.resize(deviceCount);
	lvn::getPhysicalDevices(devices.data(), &deviceCount);


	// initialize rendering, pass the physical device in the init struct
	LvnRenderInitInfo renderInfo{};

	// find and check if physical device is supported
	for (uint32_t i = 0; i < deviceCount; i++)
	{
		if (lvn::checkPhysicalDeviceSupport(devices[i]) == Lvn_Result_Success)
		{
			renderInfo.physicalDevice = devices[i];
			break;
		}
	}

	lvn::renderInit(&renderInfo);


	// window create info struct
	LvnWindowCreateInfo windowInfo{};
	windowInfo.title = "simpleWindow";
	windowInfo.width = 800;
	windowInfo.height = 600;
	windowInfo.minWidth = 300;
	windowInfo.minHeight = 200;
	windowInfo.eventCallBack = eventCallBackFn;

	LvnWindow* window;
	lvn::createWindow(&window, &windowInfo);

	// [Main Render Loop]
	while (lvn::windowOpen(window))
	{
		lvn::windowUpdate(window);

		// get next window swapchain image
		lvn::renderBeginNextFrame(window);
		lvn::renderBeginCommandRecording(window);

		// set background color and begin render pass
		lvn::renderClearColor(window, 0.0f, 0.0f, 0.0f, 1.0f);
		lvn::renderCmdBeginRenderPass(window);

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
