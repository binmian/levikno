#include <levikno/levikno.h>


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

	LvnImageData imageData = lvn::loadImageData("res/images/debug.png", 4, true); // NOTE: image data is loaded as an argument

	// texture create info struct
	LvnTextureCreateInfo textureCreateInfo{};
	textureCreateInfo.imageData = imageData;
	textureCreateInfo.format = Lvn_TextureFormat_Unorm;
	textureCreateInfo.wrapMode = Lvn_TextureMode_Repeat;
	textureCreateInfo.minFilter = Lvn_TextureFilter_Linear;
	textureCreateInfo.magFilter = Lvn_TextureFilter_Linear;

	LvnRendererCreateInfo rendererCreateInfo{};
	rendererCreateInfo.windowCreateInfo = lvn::windowCreateInfoGetConfig(800, 600, "2dScene");
	rendererCreateInfo.rendererModes = Lvn_RendererMode_2d | Lvn_RendererMode_Line;

	LvnRenderer renderer{};
	lvn::rendererInit(&renderer, &rendererCreateInfo);

	LvnSprite sprite = lvn::createSprite(&renderer, &textureCreateInfo, { -200.0f, -200.0f });

	while (lvn::windowOpen(renderer.window))
	{
		lvn::rendererSetBackgroundColor(&renderer, 1.0f, 1.0f, 1.0f, 1.0f);

		lvn::rendererBeginDraw(&renderer);

		lvn::drawRect(&renderer, { {0.0f, 0.0f}, {200.0f, 200.0f} }, { 1.0f, 0.0f, 0.0f, 1.0f });
		lvn::drawTriangle(&renderer, { {-100.0f, 0.0f}, {-50.0f, 0.0f}, { -75.0f, 50.0f } }, { 0.0f, 1.0f, 0.0f, 1.0f});
		lvn::drawPoly(&renderer, { { 0.0f, 300.0f }, 100.0f, 8 }, { 0.0f, 0.0f, 1.0f, 1.0f });
		lvn::drawLine(&renderer, { {-200.0f, -400.0f, 0.0f}, {100.0f, 200.0f, 0.0f} }, { 0.0f, 0.0f, 0.0f, 1.0f });
		lvn::drawLine(&renderer, { {200.0f, -400.0f, 0.0f}, {-100.0f, 200.0f, 0.0f} }, { 1.0f, 0.0f, 1.0f, 1.0f });
		lvn::drawSpriteEx(&renderer, sprite, { 1.0f, 1.0f, 1.0f, 1.0f}, 0.1f, 0.0f);

		lvn::rendererEndDraw(&renderer);
	}

	lvn::destroySprite(&sprite);

	lvn::rendererTerminate(&renderer);
	lvn::terminateContext();
	return 0;
}
