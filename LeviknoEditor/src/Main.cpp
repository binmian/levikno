#include <levikno/levikno.h>

bool windowMoved(LvnWindowMovedEvent* e)
{
	LVN_TRACE("%s: (x:%d,y:%d)", e->name, e->x, e->y);
	return true;
}

bool windowResize(LvnWindowResizeEvent* e)
{
	LVN_TRACE("%s: (x:%d,y:%d)", e->name, e->width, e->height);
	return true;
}

bool mousePos(LvnMouseMovedEvent* e)
{
	LVN_TRACE("%s: (x:%d,y:%d)", e->name, e->x, e->y);
	return true;
}

bool keyPress(LvnKeyPressedEvent* e)
{
	LVN_TRACE("%s: code: %d", e->name, e->keyCode);
	return true;
}

bool keyRelease(LvnKeyReleasedEvent* e)
{
	LVN_TRACE("%s: code: %d", e->name, e->keyCode);
	return true;
}

bool keyHold(LvnKeyHoldEvent* e)
{
	LVN_TRACE("%s: code: %d (%d)", e->name, e->keyCode, e->repeat);
	return true;
}

bool keyTyped(LvnKeyTypedEvent* e)
{
	LVN_TRACE("%s: key: %c", e->name, e->key);
	return true;
}

void eventsCallbackFn(LvnEvent* e)
{
	lvn::dispatchKeyPressedEvent(e, keyPress);
	lvn::dispatchKeyHoldEvent(e, keyHold);
	lvn::dispatchKeyReleasedEvent(e, keyRelease);
	lvn::dispatchKeyTypedEvent(e, keyTyped);
}

int main()
{
	LvnContextCreateInfo lvnCreateInfo{};
	lvnCreateInfo.useLogging = true;
	lvnCreateInfo.vulkanValidationLayers = true;
	lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
	lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;

	lvn::createContext(&lvnCreateInfo);

	//lvn::logInit();
	lvn::logSetPatternFormat(lvn::getCoreLogger(), "[%T] [%#%l%^] %n: %v%$");
	lvn::logSetPatternFormat(lvn::getClientLogger(), "[%T] [%#%l%^] %n: %v%$");

	LvnWindowCreateInfo windowInfo{};
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

	LvnWindow* window = lvn::createWindow(&windowInfo);

	lvn::setWindowEventCallback(window, eventsCallbackFn);

	uint32_t deviceCount = 0;
	LvnPhysicalDevice** devices = nullptr;
	lvn::getPhysicalDevices(nullptr, &deviceCount);

	devices = (LvnPhysicalDevice**)malloc(deviceCount * sizeof(LvnPhysicalDevice*));
	if (!devices) { return 1; }
	lvn::getPhysicalDevices(devices, &deviceCount);

	for (uint32_t i = 0; i < deviceCount; i++)
	{
		LvnPhysicalDeviceInfo deviceInfo = lvn::getPhysicalDeviceInfo(devices[i]);
		LVN_TRACE("name: %s\tversion: %d", deviceInfo.name, deviceInfo.driverVersion);
	}

	LvnRendererBackends renderBackends{};
	renderBackends.physicalDevice = devices[0];
	renderBackends.pWindows = &window;
	renderBackends.windowCount = 1;
	lvn::renderInit(&renderBackends);

	LvnRenderPassAttachment colorAttachment{};
	colorAttachment.type = Lvn_AttachmentType_Color;
	colorAttachment.format = Lvn_ImageFormat_RGBA8;
	colorAttachment.loadOp = Lvn_AttachmentLoadOp_Clear;
	colorAttachment.storeOp = Lvn_AttachmentStoreOp_Store;
	colorAttachment.stencilLoadOp = Lvn_AttachmentLoadOp_DontCare;
	colorAttachment.stencilStoreOp = Lvn_AttachmentStoreOp_DontCare;
	colorAttachment.samples = Lvn_SampleCount_1_Bit;
	colorAttachment.initialLayout = Lvn_ImageLayout_Undefined;
	colorAttachment.finalLayout = Lvn_ImageLayout_Present;

	LvnRenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachment;

	LvnRenderPass* renderPass;
	lvn::createRenderPass(&renderPass, &renderPassCreateInfo);

	free(devices);

	lvn::vec2 a = { 1.0f, 2.0f };
	lvn::vec2 b = { 3.0f, 5.0f };


	lvn::vec2 d = lvn::vec4(1.0f, 3.0f, 2.0f, 4.0f);

	a[1] = 4.0f;

	float c = a[1];

	a += b;

	lvn::vec4 g = -lvn::vec4(a, 1.0f, 0.0f);
	lvn::mat4 matrix = lvn::mat4(lvn::vec4(1.0f, 2.0f, 7.0f, 4.0f), lvn::vec4(5.0f, 2.0f, 7.0f, 8.0f), lvn::vec4(9.0f, 12.0f, 11.0f, 32.0f), lvn::vec4(15.0f, 14.0f, 18.0f, 26.0f));
	lvn::mat4 matrix2 = lvn::mat4(lvn::vec4(1.0f, 2.0f, 3.0f, 4.0f), lvn::vec4(5.0f, 6.0f, 7.0f, 8.0f), lvn::vec4(9.0f, 10.0f, 11.0f, 12.0f), lvn::vec4(13.0f, 14.0f, 15.0f, 16.0f));
	lvn::mat4x3 matrix4x3 = LvnMat4x3(1.0f);

	matrix = matrix * matrix2;

	g = matrix * g;

	int frame = 0;


	while (lvn::windowOpen(window))
	{
		lvn::updateWindow(window);

		auto [x, y] = lvn::getWindowDimensions(window);

		LVN_TRACE("(x:%d,y:%d)", x, y);
		break;
	}

	lvn::destroyRenderPass(renderPass);

	lvn::destroyWindow(window);
	lvn::terminateContext();

	return 0;
}
