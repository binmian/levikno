#include <levikno/levikno.h>


#define MAX_OBJECTS (5000)

int main(int argc, char** argv)
{
	LvnContextCreateInfo lvnCreateInfo{};
	lvnCreateInfo.logging.enableLogging = true;
	lvnCreateInfo.logging.enableVulkanValidationLayers = true;
	lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
	lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;

	lvn::createContext(&lvnCreateInfo);


	uint32_t deviceCount = 0;
	std::vector<LvnPhysicalDevice*> devices;

	lvn::getPhysicalDevices(nullptr, &deviceCount);

	devices.resize(deviceCount);
	lvn::getPhysicalDevices(devices.data(), &deviceCount);


	LvnRenderInitInfo renderInfo{};

	for (uint32_t i = 0; i < deviceCount; i++)
	{
		if (lvn::checkPhysicalDeviceSupport(devices[i]) == Lvn_Result_Success)
		{
			renderInfo.physicalDevice = devices[i];
			break;
		}
	}

	lvn::renderInit(&renderInfo);


	LvnRendererCreateInfo rendererCreateInfo{};
	rendererCreateInfo.windowCreateInfo = lvn::windowCreateInfoGetConfig(800, 600, "2dScene");
	rendererCreateInfo.vertexBufferSize = MAX_OBJECTS * 4 * 8 * sizeof(float);
	rendererCreateInfo.indexBufferSize = MAX_OBJECTS * 6 * sizeof(uint32_t);

	LvnRenderer renderer;
	lvn::rendererInit(&renderer, &rendererCreateInfo);

	while (lvn::windowOpen(renderer.window))
	{
		lvn::rendererSetBackgroundColor(&renderer, 1.0f, 1.0f, 1.0f, 1.0f);

		lvn::rendererBeginDraw2d(&renderer);

		lvn::drawRect(&renderer, { {0.0f, 0.0f}, {200.0f, 200.0f} }, { 1.0f, 0.0f, 0.0f, 1.0f });
		lvn::drawTriangle(&renderer, { {-100.0f, 0.0f}, {-50.0f, 0.0f}, { -75.0f, 50.0f } }, { 0.0f, 1.0f, 0.0f, 1.0f});
		lvn::drawPoly(&renderer, { { 0.0f, 300.0f }, 100.0f, 8 }, { 0.0f, 0.0f, 1.0f, 1.0f });

		lvn::rendererEndDraw2d(&renderer);
	}

	lvn::rendererTerminate(&renderer);
	lvn::terminateContext();
	return 0;
}
