#include <levikno/levikno.h>


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
	windowInfo.eventCallBack = eventCallBackFn;

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
