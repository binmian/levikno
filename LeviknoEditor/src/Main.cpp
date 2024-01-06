#include <levikno/levikno.h>
#include <levikno/Core.h>
#include <levikno/Graphics.h>

bool windowMoved(lvn::WindowMovedEvent* e)
{
	LVN_TRACE("%s: (x:%d,y:%d)", e->name, e->x, e->y);
	return true;
}

bool windowResize(lvn::WindowResizeEvent* e)
{
	LVN_TRACE("%s: (x:%d,y:%d)", e->name, e->width, e->height);
	return true;
}

bool mousePos(lvn::MouseMovedEvent* e)
{
	LVN_TRACE("%s: (x:%d,y:%d)", e->name, e->x, e->y);
	return true;
}

bool keyPress(lvn::KeyPressedEvent* e)
{
	LVN_TRACE("%s: code: %d", e->name, e->keyCode);
	return true;
}

bool keyRelease(lvn::KeyReleasedEvent* e)
{
	LVN_TRACE("%s: code: %d", e->name, e->keyCode);
	return true;
}

bool keyHold(lvn::KeyHoldEvent* e)
{
	LVN_TRACE("%s: code: %d (%d)", e->name, e->keyCode, e->repeat);
	return true;
}

bool keyTyped(lvn::KeyTypedEvent* e)
{
	LVN_TRACE("%s: key: %c", e->name, e->key);
	return true;
}

void eventsCallbackFn(lvn::Event* e)
{
	lvn::dispatchKeyPressedEvent(e, keyPress);
	lvn::dispatchKeyHoldEvent(e, keyHold);
	lvn::dispatchKeyReleasedEvent(e, keyRelease);
	lvn::dispatchKeyTypedEvent(e, keyTyped);
}

int main()
{
	lvn::logInit();

	lvn::RendererBackends renderBackends{};
	renderBackends.enableValidationLayers = true;

	lvn::createWindowContext(lvn::WindowAPI::glfw);
	lvn::createGraphicsContext(lvn::GraphicsAPI::vulkan, &renderBackends);

	uint32_t deviceCount;
	lvn::getPhysicalDevices(nullptr, &deviceCount);

	lvn::PhysicalDevice* devices = static_cast<lvn::PhysicalDevice*>(malloc(deviceCount * sizeof(lvn::PhysicalDevice)));
	lvn::getPhysicalDevices(devices, &deviceCount);

	for (uint32_t i = 0; i < deviceCount; i++)
	{
		LVN_TRACE("name: %s\tversion: %d", devices[i].info.name, devices[i].info.driverVersion);
	}

	free(devices);

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
	lvn::mat4x3 matrix4x3 = lvn::mat4x3(1.0f);

	matrix = matrix * matrix2;

	g = matrix * g;

	int frame = 0;


	while (lvn::windowOpen(window))
	{
		lvn::updateWindow(window);

		auto dim = lvn::getWindowDimensions(window);

	}


	lvn::terminateWindowContext();
	lvn::terminateGraphicsContext();
	lvn::logTerminate();

	return 0;
}