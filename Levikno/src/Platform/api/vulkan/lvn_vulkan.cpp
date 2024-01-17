#include "lvn_vulkan.h"

#include "lvn_vulkanBackends.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
#include <vulkan/vulkan.h>


static const char* s_ValidationLayers[] =
{
	"VK_LAYER_KHRONOS_validation"
};

static const char* s_DeviceExtensions[] =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))

namespace lvn
{

static VulkanBackends* s_VkBackends = nullptr;

namespace vks
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL	debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	static bool								checkValidationLayerSupport();
	static LvnVector<const char*>			getRequiredExtensions(VulkanBackends* vkBackends);
	static VkResult							createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	static void								destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	static void								fillVulkanDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	static void								setupDebugMessenger(VulkanBackends* vkBackends);
	static LvnPhysicalDeviceType			getPhysicalDeviceTypeEnum(VkPhysicalDeviceType type);
	static bool								checkDeviceExtensionSupport(VkPhysicalDevice device);
	static SwapChainSupportDetails			querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice device);
	static void								createLogicalDevice(VulkanBackends* vkBackends);
	static VkSurfaceFormatKHR				chooseSwapSurfaceFormat(const LvnVector<VkSurfaceFormatKHR>& availableFormats);
	static VkPresentModeKHR					chooseSwapPresentMode(const LvnVector<VkPresentModeKHR>& availablePresentModes);
	static VkExtent2D						chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);
	static void								createSwapChain(VulkanBackends* vkbackends, GLFWwindow* window);
	static void								createImageViews(VulkanBackends* vkbackends);
	static void								createGraphicsPipeline(VulkanBackends* vkbackends);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		switch (messageSeverity)
		{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			{
				LVN_CORE_INFO("Vulkan Validation Layer: %s", pCallbackData->pMessage);
				return VK_TRUE;
			}
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			{
				LVN_CORE_WARN("Vulkan Validation Layer: %s", pCallbackData->pMessage);
				return VK_TRUE;
			}
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			{
				LVN_CORE_ERROR("Vulkan Validation Layer: %s", pCallbackData->pMessage);
				return VK_TRUE;
			}
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
			{
				LVN_CORE_ERROR("Vulkan Validation Layer: %s", pCallbackData->pMessage);
				return VK_TRUE;
			}
		}

		return VK_FALSE;
	}

	static bool checkValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		LvnVector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : s_ValidationLayers)
		{
			for (uint32_t i = 0; i < layerCount; i++)
			{
				if (strcmp(layerName, availableLayers[i].layerName) == 0)
					return true;
			}
		}
		return false;
	}

	static LvnVector<const char*> getRequiredExtensions(VulkanBackends* vkBackends)
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		LvnVector<const char*> extensions(glfwExtensions, glfwExtensionCount);

		if (vkBackends->enableValidationLayers)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}
		
	static VkResult createDebugUtilsMessengerEXT(
		VkInstance instance, 
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
		const VkAllocationCallbacks* pAllocator, 
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	static void destroyDebugUtilsMessengerEXT(
		VkInstance instance, 
		VkDebugUtilsMessengerEXT debugMessenger, 
		const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
			func(instance, debugMessenger, pAllocator);
	}

	static void fillVulkanDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = 
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT	   | 
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}

	static void setupDebugMessenger(VulkanBackends* vkBackends)
	{
		if (!vkBackends->enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		fillVulkanDebugMessengerCreateInfo(createInfo);

		LVN_CORE_ASSERT(createDebugUtilsMessengerEXT(vkBackends->instance, &createInfo, nullptr, &vkBackends->debugMessenger) == VK_SUCCESS, "vulkan - failed to set up debug messenger!");
	}

	static LvnPhysicalDeviceType getPhysicalDeviceTypeEnum(VkPhysicalDeviceType type)
	{
		switch (type)
		{
			case VK_PHYSICAL_DEVICE_TYPE_OTHER:			 { return Lvn_PhysicalDeviceType_Other; }
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: { return Lvn_PhysicalDeviceType_Integrated_GPU; }
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:	 { return Lvn_PhysicalDeviceType_Discrete_GPU; }
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:	 { return Lvn_PhysicalDeviceType_Virtual_GPU; }
			case VK_PHYSICAL_DEVICE_TYPE_CPU:			 { return Lvn_PhysicalDeviceType_CPU; }
			default: { break; }
		}

		LVN_CORE_WARN("vulkan - No physical device matches type");
		return Lvn_PhysicalDeviceType_Unknown;
	}

	static VulkanQueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		VulkanQueueFamilyIndices indices{};

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		LvnVector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsIndex = i;
				indices.has_graphics = true;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (presentSupport)
			{
				indices.presentIndex = i;
				indices.has_present = true;
			}

			if (indices.has_graphics && indices.has_present)
			{
				break;
			}

			i++;
		}

		return indices;
	}

	static void createLogicalDevice(VulkanBackends* vkBackends)
	{
		// Find queue families
		VulkanQueueFamilyIndices queueIndices = vks::findQueueFamilies(vkBackends->physicalDevice, vkBackends->surface);

		// Check queue families
		LVN_CORE_ASSERT(queueIndices.has_graphics && queueIndices.has_present, "vulkan - physical device does not support queue families needed!");

		// Check device extension support
		LVN_CORE_ASSERT(vks::checkDeviceExtensionSupport(vkBackends->physicalDevice), "vulkan - physical device does not support required extensions!");

		float queuePriority = 1.0f;
		LvnVector<VkDeviceQueueCreateInfo> queueCreateInfos;
		VkDeviceQueueCreateInfo queuePresentCreateInfo{};
		queuePresentCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queuePresentCreateInfo.queueFamilyIndex = queueIndices.presentIndex;
		queuePresentCreateInfo.queueCount = 1;
		queuePresentCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queuePresentCreateInfo);

		if (queueIndices.presentIndex != queueIndices.graphicsIndex)
		{
			VkDeviceQueueCreateInfo queueGraphicsCreateInfo{};
			queueGraphicsCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueGraphicsCreateInfo.queueFamilyIndex = queueIndices.graphicsIndex;
			queueGraphicsCreateInfo.queueCount = 1;
			queueGraphicsCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueGraphicsCreateInfo);
		}

		// Create Logical Device
		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = queueCreateInfos.size();

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.ppEnabledExtensionNames = s_DeviceExtensions;
		createInfo.enabledExtensionCount = ARRAY_LEN(s_DeviceExtensions);

		if (vkBackends->enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(ARRAY_LEN(s_ValidationLayers));
			createInfo.ppEnabledLayerNames = s_ValidationLayers;
		}
		else
			createInfo.enabledLayerCount = 0;

		LVN_CORE_ASSERT(vkCreateDevice(vkBackends->physicalDevice, &createInfo, nullptr, &vkBackends->device) == VK_SUCCESS, "vulkan - failed to create logical device!");


		// Get device queues
		vkGetDeviceQueue(vkBackends->device, queueIndices.presentIndex, 0, &vkBackends->presentQueue);
		vkGetDeviceQueue(vkBackends->device, queueIndices.graphicsIndex, 0, &vkBackends->graphicsQueue);
	}

	static bool checkDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		LvnVector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		LvnVector<const char*> requiredExtensions(s_DeviceExtensions, ARRAY_LEN(s_DeviceExtensions));

		for (uint32_t i = 0; i < requiredExtensions.size(); i++)
		{
			bool extensionFound = false;
			for (uint32_t j = 0; j < extensionCount; j++)
			{
				if (!strcmp(requiredExtensions[i], availableExtensions[j].extensionName))
				{
					LVN_CORE_TRACE("Vulkan extension found: %s", availableExtensions[j].extensionName);
					extensionFound = true;
					break;
				}
			}

			if (!extensionFound)
			{
				LVN_CORE_ERROR("required Vulkan extension not found: %s", requiredExtensions[i]);
				return false;
			}
		}

		return true;
	}

	static SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice device)
	{
		SwapChainSupportDetails details{};

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const LvnVector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	static VkPresentModeKHR chooseSwapPresentMode(const LvnVector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	static VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}
		
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		actualExtent.width = lvn::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = lvn::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}

	static void createSwapChain(VulkanBackends* vkbackends, GLFWwindow* window)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(vkbackends->surface, vkbackends->physicalDevice);
		LVN_CORE_ASSERT(!swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty(), "vulkan - physical device does not have swap chain support formats or present modes!");

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(window, swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = vkbackends->surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		VulkanQueueFamilyIndices indices = vks::findQueueFamilies(vkbackends->physicalDevice, vkbackends->surface);
		uint32_t queueFamilyIndices[] = { indices.graphicsIndex, indices.presentIndex };

		if (indices.graphicsIndex != indices.presentIndex)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE; // TODO: add swap chain recreation (window resize)

		LVN_CORE_ASSERT(vkCreateSwapchainKHR(vkbackends->device, &createInfo, nullptr, &vkbackends->swapChain) == VK_SUCCESS, "vulkan - failed to create swap chain!");

		vkGetSwapchainImagesKHR(vkbackends->device, vkbackends->swapChain, &imageCount, nullptr);
		vkbackends->swapChainImages = (VkImage*)lvn::memAlloc(imageCount * sizeof(VkImage));
		vkbackends->swapChainImageCount = imageCount;
		vkGetSwapchainImagesKHR(vkbackends->device, vkbackends->swapChain, &imageCount, vkbackends->swapChainImages);

		vkbackends->swapChainImageFormat = surfaceFormat.format;
		vkbackends->swapChainExtent = extent;
	}

	static void createImageViews(VulkanBackends* vkbackends)
	{
		vkbackends->swapChainImageViews = (VkImageView*)lvn::memAlloc(vkbackends->swapChainImageCount * sizeof(VkImageView));
		vkbackends->swapChainImageViewCount = vkbackends->swapChainImageCount;

		for (size_t i = 0; i < vkbackends->swapChainImageCount; i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = vkbackends->swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = vkbackends->swapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			LVN_CORE_ASSERT(vkCreateImageView(vkbackends->device, &createInfo, nullptr, &vkbackends->swapChainImageViews[i]) == VK_SUCCESS, "vulkan - failed to create image views!");
		}
	}

	static void createGraphicsPipeline(VulkanBackends* vkbackends)
	{

	}

} /* namespace vks */

LvnResult vksImplCreateContext(LvnGraphicsContext* graphicsContext, bool enableValidationLayers)
{
	VulkanBackends* vkBackends = new VulkanBackends();
	s_VkBackends = vkBackends;
	LvnContext* lvnctx = lvn::getContext();
	vkBackends->enableValidationLayers = lvnctx->vulkanValidationLayers;

	graphicsContext->renderClearColor = vksImplRenderClearColor;
	graphicsContext->renderClear = vksImplRenderClear;
	graphicsContext->renderDraw = vksImplRenderDraw;
	graphicsContext->renderDrawIndexed = vksImplRenderDrawIndexed;
	graphicsContext->renderDrawInstanced = vksImplRenderDrawInstanced;
	graphicsContext->renderDrawIndexedInstanced = vksImplRenderDrawIndexedInstanced;
	graphicsContext->renderSetStencilReference = vksImplRenderSetStencilReference;
	graphicsContext->renderSetStencilMask = vksImplRenderSetStencilMask;
	graphicsContext->renderBeginNextFrame = vksImplRenderBeginNextFrame;
	graphicsContext->renderDrawSubmit = vksImplRenderDrawSubmit;
	graphicsContext->renderBeginRenderPass = vksImplRenderBeginRenderPass;
	graphicsContext->renderEndRenderPass = vksImplRenderEndRenderPass;

	// Create Vulkan Instance
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "levikno editor";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "levikno";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;


	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	LvnVector<const char*> extensions = vks::getRequiredExtensions(vkBackends);
	createInfo.enabledExtensionCount = extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (vkBackends->enableValidationLayers)
	{
		createInfo.enabledLayerCount = ARRAY_LEN(s_ValidationLayers);
		createInfo.ppEnabledLayerNames = s_ValidationLayers;

		vks::fillVulkanDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	// Create Instance
	if (vkCreateInstance(&createInfo, nullptr, &vkBackends->instance) != VK_SUCCESS)
	{
		LVN_CORE_ASSERT(false, "vulkan - failed to create instance!");
		return Lvn_Result_Failure;
	}

		
	// Get Physical Devices
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vkBackends->instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		LVN_CORE_ASSERT(false, "vulkan - failed to find Physical Devices with Vulkan support!");
		return Lvn_Result_Failure;
	}

	// Create vulkan physical devices
	VkPhysicalDevice* devices = (VkPhysicalDevice*)memAlloc(deviceCount * sizeof(VkPhysicalDevice));
	vkEnumeratePhysicalDevices(vkBackends->instance, &deviceCount, devices);

	vkBackends->pPhysicalDevices = devices;
	vkBackends->physicalDeviceCount = deviceCount;

	// Create our own implementation of physical devices
	LvnPhysicalDevice* lvnDevices = (LvnPhysicalDevice*)memAlloc(deviceCount * sizeof(LvnPhysicalDevice));
	for (uint32_t i = 0; i < deviceCount; i++)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);

		LvnPhysicalDeviceInfo deviceInfo{};
		deviceInfo.type = vks::getPhysicalDeviceTypeEnum(deviceProperties.deviceType);
			
		deviceInfo.apiVersion = deviceProperties.apiVersion;
		deviceInfo.driverVersion = deviceProperties.driverVersion;
		memcpy(deviceInfo.name, deviceProperties.deviceName, 226);
		lvnDevices[i].info = deviceInfo;
		lvnDevices[i].device = devices[i];
	}

	lvnctx->physicalDevices = lvnDevices;
	lvnctx->physicalDeviceCount = deviceCount;

	return Lvn_Result_Success;
}

void vksImplTerminateContext()
{
	VulkanBackends* vkBackends = s_VkBackends;
	LvnContext* lvnctx = getContext();

	// swap chain image views
	for (uint32_t i = 0; i < vkBackends->swapChainImageViewCount; i++)
	{
		vkDestroyImageView(vkBackends->device, vkBackends->swapChainImageViews[i], nullptr);
	}

	if (vkBackends->swapChainImageViews)
		lvn::memFree(vkBackends->swapChainImageViews);

	// swap chain images
	if (vkBackends->swapChainImages)
		lvn::memFree(vkBackends->swapChainImages);

	// swap chain
	vkDestroySwapchainKHR(vkBackends->device, vkBackends->swapChain, nullptr);

	// logical device
	vkDestroyDevice(vkBackends->device, nullptr);

	// debug validation layers
	if (vkBackends->enableValidationLayers)
		vks::destroyDebugUtilsMessengerEXT(vkBackends->instance, vkBackends->debugMessenger, nullptr);
		
	// allocated physical devices
	if (lvnctx->physicalDevices)
		memFree(lvnctx->physicalDevices);
	if (vkBackends->pPhysicalDevices || vkBackends->physicalDeviceCount)
		memFree(vkBackends->pPhysicalDevices);

	// window surface
	vkDestroySurfaceKHR(vkBackends->instance, vkBackends->surface, nullptr);

	// instance
	vkDestroyInstance(vkBackends->instance, nullptr);

	delete s_VkBackends;
}

LvnResult vksImplRenderInit(LvnRendererBackends* renderBackends)
{
	VulkanBackends* vkBackends = s_VkBackends;

	vkBackends->physicalDevice = static_cast<VkPhysicalDevice>(renderBackends->physicalDevice->device);

	if (vkBackends->enableValidationLayers && !vks::checkValidationLayerSupport())
		LVN_CORE_WARN("vulkan - validation layers enabled, but not available!");
	else 
		vks::setupDebugMessenger(vkBackends);

	// create surface
	GLFWwindow* glfwWin = static_cast<GLFWwindow*>(renderBackends->pWindows[0]->nativeWindow);
	LVN_CORE_ASSERT(glfwCreateWindowSurface(vkBackends->instance, glfwWin, nullptr, &vkBackends->surface) == VK_SUCCESS, "vulkan - failed to create window surface!");
	
	vks::createLogicalDevice(vkBackends);
	vks::createSwapChain(vkBackends, glfwWin);
	vks::createImageViews(vkBackends);
	vks::createGraphicsPipeline(vkBackends);

	return Lvn_Result_Success;
}

void vksImplRenderClearColor(const float r, const float g, const float b, const float w)
{

}

void vksImplRenderClear()
{

}

void vksImplRenderDraw(uint32_t vertexCount)
{

}

void vksImplRenderDrawIndexed(uint32_t indexCount)
{

}

void vksImplRenderDrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance)
{

}

void vksImplRenderDrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance)
{

}

void vksImplRenderSetStencilReference(uint32_t reference)
{

}

void vksImplRenderSetStencilMask(uint32_t compareMask, uint32_t writeMask)
{

}

void vksImplRenderBeginNextFrame()
{

}

void vksImplRenderDrawSubmit()
{

}

void vksImplRenderBeginRenderPass()
{

}

void vksImplRenderEndRenderPass()
{

}

}