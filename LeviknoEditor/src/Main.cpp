#include <levikno/levikno.h>
#include <levikno/Core.h>
#include <levikno/Graphics.h>

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
	lvn::logInit();

	LvnRendererBackends renderBackends{};
	renderBackends.enableValidationLayers = true;

	lvn::createWindowContext(Lvn_WindowApi_glfw);
	lvn::createGraphicsContext(Lvn_GraphicsApi_vulkan);

	uint32_t deviceCount;
	lvn::getPhysicalDevices(nullptr, &deviceCount);

	LvnPhysicalDevice* devices = static_cast<LvnPhysicalDevice*>(malloc(deviceCount * sizeof(LvnPhysicalDevice)));
	lvn::getPhysicalDevices(devices, &deviceCount);

	for (uint32_t i = 0; i < deviceCount; i++)
	{
		LVN_TRACE("name: %s\tversion: %d", devices[i].info.name, devices[i].info.driverVersion);
	}

	renderBackends.physicalDevice = &devices[0];
	lvn::renderInit(&renderBackends);

	free(devices);

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

		auto dim = lvn::getWindowDimensions(window);

		LVN_TRACE("abc123!@#\n445\t45\"df\"{}\n%ssaf%d%%", "fasdf", 22);
	}


	lvn::terminateWindowContext();
	lvn::terminateGraphicsContext();
	lvn::logTerminate();

	return 0;
}