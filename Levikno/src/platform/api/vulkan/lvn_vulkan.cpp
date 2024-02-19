#include "lvn_vulkan.h"

#include "lvn_vulkanBackends.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
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
	static VKAPI_ATTR VkBool32 VKAPI_CALL		debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	static bool									checkValidationLayerSupport();
	static LvnVector<const char*>				getRequiredExtensions(VulkanBackends* vkBackends);
	static VkResult								createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	static void									destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	static void									fillVulkanDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	static void									setupDebugMessenger(VulkanBackends* vkBackends);
	static LvnPhysicalDeviceType				getPhysicalDeviceTypeEnum(VkPhysicalDeviceType type);
	static bool									checkDeviceExtensionSupport(VkPhysicalDevice device);
	static VulkanSwapChainSupportDetails		querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice device);
	static VulkanQueueFamilyIndices				findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	static void									createLogicalDevice(VulkanBackends* vkBackends, VkSurfaceKHR surface);
	static void									createRenderPass(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData, VkFormat format);
	static VkSurfaceFormatKHR					chooseSwapSurfaceFormat(const VkSurfaceFormatKHR* pAvailableFormats, uint32_t count);
	static VkPresentModeKHR						chooseSwapPresentMode(const VkPresentModeKHR* pAvailablePresentModes, uint32_t count);
	static VkExtent2D							chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR* capabilities);
	static void									createSwapChain(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData, VulkanSwapChainSupportDetails swapChainSupport, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode, VkExtent2D extent);
	static void									createImageViews(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
	static void									createFrameBuffers(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
	static void									createCommandBuffers(VulkanBackends* vkBackends);
	static void									createSyncObjects(VulkanBackends* vkBackends);
	static VkPrimitiveTopology					getVulkanTopologyTypeEnum(LvnTopologyType topologyType);
	static VkCullModeFlags						getVulkanCullModeFlagEnum(LvnCullFaceMode cullFaceMode);
	static VkFrontFace							getVulkanCullFrontFaceEnum(LvnCullFrontFace cullFrontFace);
	static VkFormat								getVulkanFormatEnum(LvnImageFormat format);
	static VkAttachmentLoadOp					getVulkanLoadOpEnum(LvnAttachmentLoadOperation loadOp);
	static VkAttachmentStoreOp					getVulkanStoreOpEnum(LvnAttachmentStoreOperation storeOp);
	static VkImageLayout						getVulkanImageLayoutEnum(LvnImageLayout layout);
	static VkColorComponentFlags				getColorComponents(LvnPipelineColorWriteMask colorMask);
	static VkBlendFactor						getBlendFactorEnum(LvnColorBlendFactor blendFactor);
	static VkBlendOp							getBlendOperationEnum(LvnColorBlendOperation blendOp);
	static VkCompareOp							getCompareOpEnum(LvnCompareOperation compare);
	static VkStencilOp							getStencilOpEnum(LvnStencilOperation stencilOp);
	static VkPipelineColorBlendAttachmentState	createColorAttachment();
	static VkSampleCountFlagBits				getMaxUsableSampleCount(VulkanBackends* vkBackends);
	static VkSampleCountFlagBits				getSampleCountFlagEnum(LvnSampleCount samples);
	static uint32_t								getSampleCountValue(VkSampleCountFlagBits samples);
	static VkSampleCountFlagBits				getSupportedSampleCount(VulkanBackends* vkBackends, LvnSampleCount samples);
	static void									initStandardVulkanPipelineSpecification(VulkanBackends* vkBackends, LvnContext* lvnctx);
	static VulkanPipeline						createVulkanPipeline(VulkanBackends* vkBackends, VulkanPipelineCreateData* createData);
	static VkShaderModule						createShaderModule(VulkanBackends* vkBackends, const uint8_t* code, uint32_t size);
	static void									createGraphicsPipeline(VulkanBackends* vkBackends);


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
				LVN_CORE_INFO("vulkan validation Layer: %s", pCallbackData->pMessage);
				return VK_TRUE;
			}
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			{
				LVN_CORE_WARN("vulkan validation Layer: %s", pCallbackData->pMessage);
				return VK_TRUE;
			}
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			{
				LVN_CORE_ERROR("vulkan validation Layer: %s", pCallbackData->pMessage);
				return VK_TRUE;
			}
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
			{
				LVN_CORE_ERROR("vulkan validation Layer: %s", pCallbackData->pMessage);
				return VK_TRUE;
			}
			default:
			{
				return VK_FALSE;
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

	static void createLogicalDevice(VulkanBackends* vkBackends, VkSurfaceKHR surface)
	{
		// Find queue families
		VulkanQueueFamilyIndices queueIndices = vks::findQueueFamilies(vkBackends->physicalDevice, surface);
		vkBackends->deviceIndices = queueIndices;

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

	static void createRenderPass(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData, VkFormat format)
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		LVN_CORE_ASSERT(vkCreateRenderPass(vkBackends->device, &renderPassInfo, nullptr, &surfaceData->renderPass) == VK_SUCCESS, "vulkan - failed to create render pass!");
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

	static VulkanSwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice device)
	{
		VulkanSwapChainSupportDetails details{};

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

	static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR* pAvailableFormats, uint32_t count)
	{
		for (uint32_t i = 0; i < count; i++)
		{
			if (pAvailableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && pAvailableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return pAvailableFormats[i];
			}
		}

		return pAvailableFormats[0];
	}

	static VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR* pAvailablePresentModes, uint32_t count)
	{
		for (uint32_t i = 0; i < count; i++)
		{
			if (pAvailablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return pAvailablePresentModes[i];
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	static VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR* capabilities)
	{
		if (capabilities->currentExtent.width != UINT32_MAX)
		{
			return capabilities->currentExtent;
		}
		
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		actualExtent.width = lvn::clamp(actualExtent.width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
		actualExtent.height = lvn::clamp(actualExtent.height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);

		return actualExtent;
	}

	static void createSwapChain(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData, VulkanSwapChainSupportDetails swapChainSupport, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode, VkExtent2D extent)
	{
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surfaceData->surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		VulkanQueueFamilyIndices indices = vks::findQueueFamilies(vkBackends->physicalDevice, surfaceData->surface);
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

		LVN_CORE_ASSERT(vkCreateSwapchainKHR(vkBackends->device, &createInfo, nullptr, &surfaceData->swapChain) == VK_SUCCESS, "vulkan - failed to create swap chain!");

		vkGetSwapchainImagesKHR(vkBackends->device, surfaceData->swapChain, &imageCount, nullptr);
		surfaceData->swapChainImages = (VkImage*)lvn::memAlloc(imageCount * sizeof(VkImage));
		surfaceData->swapChainImageCount = imageCount;
		vkGetSwapchainImagesKHR(vkBackends->device, surfaceData->swapChain, &imageCount, surfaceData->swapChainImages);

		surfaceData->swapChainImageFormat = surfaceFormat.format;
		surfaceData->swapChainExtent = extent;
	}

	static void createImageViews(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData)
	{
		surfaceData->swapChainImageViews = (VkImageView*)lvn::memAlloc(surfaceData->swapChainImageCount * sizeof(VkImageView));
		surfaceData->swapChainImageViewCount = surfaceData->swapChainImageCount;

		for (size_t i = 0; i < surfaceData->swapChainImageCount; i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = surfaceData->swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = surfaceData->swapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			LVN_CORE_ASSERT(vkCreateImageView(vkBackends->device, &createInfo, nullptr, &surfaceData->swapChainImageViews[i]) == VK_SUCCESS, "vulkan - failed to create image views!");
		}
	}

	static void createFrameBuffers(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData)
	{
		surfaceData->frameBuffers = (VkFramebuffer*)lvn::memAlloc(surfaceData->swapChainImageViewCount * sizeof(VkFramebuffer));
		surfaceData->frameBufferCount = surfaceData->swapChainImageViewCount;

		for (size_t i = 0; i < surfaceData->frameBufferCount; i++)
		{
			VkImageView attachments[] =
			{
				surfaceData->swapChainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = surfaceData->renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = surfaceData->swapChainExtent.width;
			framebufferInfo.height = surfaceData->swapChainExtent.height;
			framebufferInfo.layers = 1;

			LVN_CORE_ASSERT(vkCreateFramebuffer(vkBackends->device, &framebufferInfo, nullptr, &surfaceData->frameBuffers[i]) == VK_SUCCESS, "vulkan - failed to create framebuffer!");
		}
	}

	static void createCommandBuffers(VulkanBackends* vkBackends)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = vkBackends->commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = vkBackends->maxFramesInFlight;

		vkBackends->commandBuffers = (VkCommandBuffer*)lvn::memAlloc(vkBackends->maxFramesInFlight * sizeof(VkCommandBuffer));
		LVN_CORE_ASSERT(vkAllocateCommandBuffers(vkBackends->device, &allocInfo, vkBackends->commandBuffers) == VK_SUCCESS, "vulkan - failed to allocate command buffers!");
	}

	static void createSyncObjects(VulkanBackends* vkBackends)
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		vkBackends->imageAvailableSemaphores = (VkSemaphore*)lvn::memAlloc(vkBackends->maxFramesInFlight * sizeof(VkSemaphore));
		vkBackends->renderFinishedSemaphores = (VkSemaphore*)lvn::memAlloc(vkBackends->maxFramesInFlight * sizeof(VkSemaphore));
		vkBackends->inFlightFences = (VkFence*)lvn::memAlloc(vkBackends->maxFramesInFlight * sizeof(VkFence));

		for (uint32_t i = 0; i < vkBackends->maxFramesInFlight; i++)
		{
			LVN_CORE_ASSERT(vkCreateSemaphore(vkBackends->device, &semaphoreInfo, nullptr, &vkBackends->imageAvailableSemaphores[i]) == VK_SUCCESS, "vulkan - failed to create semaphore");
			LVN_CORE_ASSERT(vkCreateSemaphore(vkBackends->device, &semaphoreInfo, nullptr, &vkBackends->renderFinishedSemaphores[i]) == VK_SUCCESS, "vulkan - failed to create semaphore");
			LVN_CORE_ASSERT(vkCreateFence(vkBackends->device, &fenceInfo, nullptr, &vkBackends->inFlightFences[i]) == VK_SUCCESS, "vulkan - failed to create fence");
		}
	}

	static VkPrimitiveTopology getVulkanTopologyTypeEnum(LvnTopologyType topologyType)
	{
		switch (topologyType)
		{
			case Lvn_TopologyType_Point: { return VK_PRIMITIVE_TOPOLOGY_POINT_LIST; }
			case Lvn_TopologyType_Line: { return VK_PRIMITIVE_TOPOLOGY_LINE_LIST; }
			case Lvn_TopologyType_LineStrip: { return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; }
			case Lvn_TopologyType_Triangle: { return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; }
			case Lvn_TopologyType_TriangleStrip: { return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; }
			default:
			{
				LVN_CORE_WARN("unknown topology type enum (%d), setting to triangle topology type (default)", topologyType);
				return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			}
		}
	}

	static VkCullModeFlags getVulkanCullModeFlagEnum(LvnCullFaceMode cullFaceMode)
	{
		switch (cullFaceMode)
		{
			case Lvn_CullFaceMode_Disable: { return VK_CULL_MODE_NONE; }
			case Lvn_CullFaceMode_Front: { return VK_CULL_MODE_FRONT_BIT; }
			case Lvn_CullFaceMode_Back: { return VK_CULL_MODE_BACK_BIT; }
			case Lvn_CullFaceMode_Both: { return VK_CULL_MODE_FRONT_AND_BACK; }
			default:
			{
				LVN_CORE_WARN("unknown cull face mode enum (%d), setting to cull face mode none (default)", cullFaceMode);
				return VK_CULL_MODE_NONE;
			}
		}
	}

	static VkFrontFace getVulkanCullFrontFaceEnum(LvnCullFrontFace cullFrontFace)
	{
		switch (cullFrontFace)
		{
			case Lvn_CullFrontFace_Clockwise: { return VK_FRONT_FACE_CLOCKWISE; }
			case Lvn_CullFrontFace_CounterClockwise: { return VK_FRONT_FACE_COUNTER_CLOCKWISE; }
			default:
			{
				LVN_CORE_WARN("unknown cull front face enum (%d), setting to cull front face clockwise (default)", cullFrontFace);
				return VK_FRONT_FACE_CLOCKWISE;
			}
		}

	}

	static VkFormat	getVulkanFormatEnum(LvnImageFormat format)
	{
		switch (format)
		{
			case Lvn_ImageFormat_None: { return VK_FORMAT_UNDEFINED; }
			case Lvn_ImageFormat_RGB: { return VK_FORMAT_R8G8B8_UNORM; }
			case Lvn_ImageFormat_RGBA: { return VK_FORMAT_R8G8B8A8_UNORM; }
			case Lvn_ImageFormat_RGBA8: { return VK_FORMAT_R8G8B8A8_UNORM; }
			case Lvn_ImageFormat_RGBA16F: { return VK_FORMAT_R16G16B16A16_SFLOAT; }
			case Lvn_ImageFormat_RGBA32F: { return VK_FORMAT_R32G32B32A32_SFLOAT; }
			case Lvn_ImageFormat_RedInt: { return VK_FORMAT_R8_SINT; }
			// case Lvn_ImageFormat_DepthComponent: { return VK_FORMAT_DEPTH_COMPONENT; }
			case Lvn_ImageFormat_Depth24Stencil8: { return VK_FORMAT_D24_UNORM_S8_UINT; }
			default:
			{
				LVN_CORE_WARN("unknown image format enum (%d), setting image format to undefined", format);
				return VK_FORMAT_UNDEFINED;
			}
		}
	}

	static VkAttachmentLoadOp getVulkanLoadOpEnum(LvnAttachmentLoadOperation loadOp)
	{
		switch (loadOp)
		{
			case Lvn_AttachmentLoadOp_Load: { return VK_ATTACHMENT_LOAD_OP_LOAD; }
			case Lvn_AttachmentLoadOp_Clear: { return VK_ATTACHMENT_LOAD_OP_CLEAR; }
			case Lvn_AttachmentLoadOp_DontCare: { return VK_ATTACHMENT_LOAD_OP_DONT_CARE; }
			default:
			{
				LVN_CORE_WARN("unknown attachment load operation enum (%d), setting load op to dont_care", loadOp);
				return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			}
		}
	}

	static VkAttachmentStoreOp getVulkanStoreOpEnum(LvnAttachmentStoreOperation storeOp)
	{
		switch (storeOp)
		{
			case Lvn_AttachmentStoreOp_Store: { return VK_ATTACHMENT_STORE_OP_STORE; }
			case Lvn_AttachmentStoreOp_DontCare: { return VK_ATTACHMENT_STORE_OP_DONT_CARE; }
			default:
			{
				LVN_CORE_WARN("unknown attachment store operation enum (%d), setting store op to dont_care", storeOp);
				return VK_ATTACHMENT_STORE_OP_DONT_CARE;
			}
		}
	}

	static VkImageLayout getVulkanImageLayoutEnum(LvnImageLayout layout)
	{
		switch (layout)
		{
			case Lvn_ImageLayout_Undefined: { return VK_IMAGE_LAYOUT_UNDEFINED; }
			case Lvn_ImageLayout_Present: { return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; }
			case Lvn_ImageLayout_ColorAttachment: { return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; }
			case Lvn_ImageLayout_DepthStencilAttachment: { return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; }
			default:
			{
				LVN_CORE_WARN("unknown image layout enum (%d), setting image layout to undefined", layout);
				return VK_IMAGE_LAYOUT_UNDEFINED;
			}
		}
	}

	static VkColorComponentFlags getColorComponents(LvnPipelineColorWriteMask colorMask)
	{
		VkColorComponentFlags colorComponentsFlag = 0;

		if (colorMask.colorComponentR) colorComponentsFlag |= VK_COLOR_COMPONENT_R_BIT;
		if (colorMask.colorComponentG) colorComponentsFlag |= VK_COLOR_COMPONENT_G_BIT;
		if (colorMask.colorComponentB) colorComponentsFlag |= VK_COLOR_COMPONENT_B_BIT;
		if (colorMask.colorComponentA) colorComponentsFlag |= VK_COLOR_COMPONENT_A_BIT;

		return colorComponentsFlag;
	}

	static VkBlendFactor getBlendFactorEnum(LvnColorBlendFactor blendFactor)
	{
		switch (blendFactor)
		{
			case Lvn_ColorBlendFactor_Zero: { return VK_BLEND_FACTOR_ZERO; }
			case Lvn_ColorBlendFactor_One: { return VK_BLEND_FACTOR_ONE; }
			case Lvn_ColorBlendFactor_SrcColor: { return VK_BLEND_FACTOR_SRC_COLOR; }
			case Lvn_ColorBlendFactor_OneMinusSrcColor: { return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR; }
			case Lvn_ColorBlendFactor_DstColor: { return VK_BLEND_FACTOR_DST_COLOR; }
			case Lvn_ColorBlendFactor_OneMinusDstColor: { return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR; }
			case Lvn_ColorBlendFactor_SrcAlpha: { return VK_BLEND_FACTOR_SRC_ALPHA; }
			case Lvn_ColorBlendFactor_OneMinusSrcAlpha: { return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; }
			case Lvn_ColorBlendFactor_DstAlpha: { return VK_BLEND_FACTOR_DST_ALPHA; }
			case Lvn_ColorBlendFactor_OneMinusDstAlpha: { return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA; }
			case Lvn_ColorBlendFactor_ConstantColor: { return VK_BLEND_FACTOR_CONSTANT_COLOR; }
			case Lvn_ColorBlendFactor_OneMinusConstantColor: { return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR; }
			case Lvn_ColorBlendFactor_ConstantAlpha: { return VK_BLEND_FACTOR_CONSTANT_ALPHA; }
			case Lvn_ColorBlendFactor_OneMinusConstantAlpha: { return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA; }
			case Lvn_ColorBlendFactor_SrcAlphaSaturate: { return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE; }
			case Lvn_ColorBlendFactor_Src1Color: { return VK_BLEND_FACTOR_SRC1_COLOR; }
			case Lvn_ColorBlendFactor_OneMinusSrc1Color: { return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR; }
			case Lvn_ColorBlendFactor_Src1_Alpha: { return VK_BLEND_FACTOR_SRC1_ALPHA; }
			case Lvn_ColorBlendFactor_OneMinusSrc1Alpha: { return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA; }
			default:
			{
				LVN_CORE_WARN("unknown blend factor enum (%d), setting to blend factor zero (default)", blendFactor);
				return VK_BLEND_FACTOR_ZERO;
			}
		}
	}

	static VkBlendOp getBlendOperationEnum(LvnColorBlendOperation blendOp)
	{
		switch (blendOp)
		{
			case Lvn_ColorBlendOperation_Add: { return VK_BLEND_OP_ADD; }
			case Lvn_ColorBlendOperation_Subtract: { return VK_BLEND_OP_SUBTRACT; }
			case Lvn_ColorBlendOperation_ReverseSubtract: { return VK_BLEND_OP_REVERSE_SUBTRACT; }
			case Lvn_ColorBlendOperation_Min: { return VK_BLEND_OP_MIN; }
			case Lvn_ColorBlendOperation_Max: { return VK_BLEND_OP_MAX; }
			case Lvn_ColorBlendOperation_ZeroExt: { return VK_BLEND_OP_ZERO_EXT; }
			case Lvn_ColorBlendOperation_SrcExt: { return VK_BLEND_OP_SRC_EXT; }
			case Lvn_ColorBlendOperation_DstExt: { return VK_BLEND_OP_DST_EXT; }
			case Lvn_ColorBlendOperation_SrcOverExt: { return VK_BLEND_OP_SRC_OVER_EXT; }
			case Lvn_ColorBlendOperation_DstOverExt: { return VK_BLEND_OP_DST_OVER_EXT; }
			case Lvn_ColorBlendOperation_SrcInExt: { return VK_BLEND_OP_SRC_IN_EXT; }
			case Lvn_ColorBlendOperation_DstInExt: { return VK_BLEND_OP_DST_IN_EXT; }
			case Lvn_ColorBlendOperation_SrcOutExt: { return VK_BLEND_OP_SRC_OUT_EXT; }
			case Lvn_ColorBlendOperation_DstOutExt: { return VK_BLEND_OP_DST_OUT_EXT; }
			case Lvn_ColorBlendOperation_SrcAtopExt: { return VK_BLEND_OP_SRC_ATOP_EXT; }
			case Lvn_ColorBlendOperation_DstAtopExt: { return VK_BLEND_OP_DST_ATOP_EXT; }
			case Lvn_ColorBlendOperation_XorExt: { return VK_BLEND_OP_XOR_EXT; }
			case Lvn_ColorBlendOperation_MultiplyExt: { return VK_BLEND_OP_MULTIPLY_EXT; }
			case Lvn_ColorBlendOperation_ScreenExt: { return VK_BLEND_OP_SCREEN_EXT; }
			case Lvn_ColorBlendOperation_OverlayExt: { return VK_BLEND_OP_OVERLAY_EXT; }
			case Lvn_ColorBlendOperation_DarkenExt: { return VK_BLEND_OP_DARKEN_EXT; }
			case Lvn_ColorBlendOperation_LightenExt: { return VK_BLEND_OP_LIGHTEN_EXT; }
			case Lvn_ColorBlendOperation_ColorDodgeExt: { return VK_BLEND_OP_COLORDODGE_EXT; }
			case Lvn_ColorBlendOperation_ColorBurnExt: { return VK_BLEND_OP_COLORBURN_EXT; }
			case Lvn_ColorBlendOperation_HardLightExt: { return VK_BLEND_OP_HARDLIGHT_EXT; }
			case Lvn_ColorBlendOperation_SoftLightExt: { return VK_BLEND_OP_SOFTLIGHT_EXT; }
			case Lvn_ColorBlendOperation_DifferenceExt: { return VK_BLEND_OP_DIFFERENCE_EXT; }
			case Lvn_ColorBlendOperation_ExclusionExt: { return VK_BLEND_OP_EXCLUSION_EXT; }
			case Lvn_ColorBlendOperation_InvertExt: { return VK_BLEND_OP_INVERT_EXT; }
			case Lvn_ColorBlendOperation_InvertRGBExt: { return VK_BLEND_OP_INVERT_RGB_EXT; }
			case Lvn_ColorBlendOperation_LinearDodgeExt: { return VK_BLEND_OP_LINEARDODGE_EXT; }
			case Lvn_ColorBlendOperation_LinearBurnExt: { return VK_BLEND_OP_LINEARBURN_EXT; }
			case Lvn_ColorBlendOperation_VividLightExt: { return VK_BLEND_OP_VIVIDLIGHT_EXT; }
			case Lvn_ColorBlendOperation_LinearLightExt: { return VK_BLEND_OP_LINEARLIGHT_EXT; }
			case Lvn_ColorBlendOperation_PinLightExt: { return VK_BLEND_OP_PINLIGHT_EXT; }
			case Lvn_ColorBlendOperation_HardMixExt: { return VK_BLEND_OP_HARDMIX_EXT; }
			case Lvn_ColorBlendOperation_HSLHueExt: { return VK_BLEND_OP_HSL_HUE_EXT; }
			case Lvn_ColorBlendOperation_HSLSaturationExt: { return VK_BLEND_OP_HSL_SATURATION_EXT; }
			case Lvn_ColorBlendOperation_HSLColorExt: { return VK_BLEND_OP_HSL_COLOR_EXT; }
			case Lvn_ColorBlendOperation_HSLLuminosityExt: { return VK_BLEND_OP_HSL_LUMINOSITY_EXT; }
			case Lvn_ColorBlendOperation_PlusExt: { return VK_BLEND_OP_PLUS_EXT; }
			case Lvn_ColorBlendOperation_PlusClampedExt: { return VK_BLEND_OP_PLUS_CLAMPED_EXT; }
			case Lvn_ColorBlendOperation_PlusClampedAlphaExt: { return VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT; }
			case Lvn_ColorBlendOperation_PlusDarkerExt: { return VK_BLEND_OP_PLUS_DARKER_EXT; }
			case Lvn_ColorBlendOperation_MinusExt: { return VK_BLEND_OP_MINUS_EXT; }
			case Lvn_ColorBlendOperation_MinusClampedExt: { return VK_BLEND_OP_MINUS_CLAMPED_EXT; }
			case Lvn_ColorBlendOperation_ContrastExt: { return VK_BLEND_OP_CONTRAST_EXT; }
			case Lvn_ColorBlendOperation_InvertOVGExt: { return VK_BLEND_OP_INVERT_OVG_EXT; }
			case Lvn_ColorBlendOperation_RedExt: { return VK_BLEND_OP_RED_EXT; }
			case Lvn_ColorBlendOperation_GreenExt: { return VK_BLEND_OP_GREEN_EXT; }
			case Lvn_ColorBlendOperation_BlueExt: { return VK_BLEND_OP_BLUE_EXT; }
			default:
			{
				LVN_CORE_WARN("unknown blend operation enum (%d), setting to blend operation enum add (default)", blendOp);
				return VK_BLEND_OP_ADD;
			}
		}
	}

	static VkCompareOp getCompareOpEnum(LvnCompareOperation compare)
	{
		switch (compare)
		{
			case Lvn_CompareOperation_Never: { return VK_COMPARE_OP_NEVER; }
			case Lvn_CompareOperation_Less: { return VK_COMPARE_OP_LESS; }
			case Lvn_CompareOperation_Equal: { return VK_COMPARE_OP_EQUAL; }
			case Lvn_CompareOperation_LessOrEqual: { return VK_COMPARE_OP_LESS_OR_EQUAL; }
			case Lvn_CompareOperation_Greater: { return VK_COMPARE_OP_GREATER; }
			case Lvn_CompareOperation_NotEqual: { return VK_COMPARE_OP_NOT_EQUAL; }
			case Lvn_CompareOperation_GreaterOrEqual: { return VK_COMPARE_OP_GREATER_OR_EQUAL; }
			case Lvn_CompareOperation_Always: { return VK_COMPARE_OP_ALWAYS; }
			default:
			{
				LVN_CORE_WARN("unknown compare enum (%d), setting to compare enum never", compare);
				return VK_COMPARE_OP_NEVER;
			}
		}
	}

	static VkStencilOp getStencilOpEnum(LvnStencilOperation stencilOp)
	{
		switch (stencilOp)
		{
			case Lvn_StencilOperation_Keep: { return VK_STENCIL_OP_KEEP; }
			case Lvn_StencilOperation_Zero: { return VK_STENCIL_OP_ZERO; }
			case Lvn_StencilOperation_Replace: { return VK_STENCIL_OP_REPLACE; }
			case Lvn_StencilOperation_IncrementAndClamp: { return VK_STENCIL_OP_INCREMENT_AND_CLAMP; }
			case Lvn_StencilOperation_DecrementAndClamp: { return VK_STENCIL_OP_DECREMENT_AND_CLAMP; }
			case Lvn_StencilOperation_Invert: { return VK_STENCIL_OP_INVERT; }
			case Lvn_StencilOperation_IncrementAndWrap: { return VK_STENCIL_OP_INCREMENT_AND_WRAP; }
			case Lvn_StencilOperation_DecrementAndWrap: { return VK_STENCIL_OP_DECREMENT_AND_WRAP; }
			default:
			{
				LVN_CORE_WARN("unknown stencil operation enum (%d), setting to stencil operation enum keep (default)", stencilOp);
				return VK_STENCIL_OP_KEEP;
			}
		}
	}

	static VkPipelineColorBlendAttachmentState createColorAttachment()
	{
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = true;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		return colorBlendAttachment;
	}

	static VkSampleCountFlagBits getMaxUsableSampleCount(VulkanBackends* vkBackends)
	{
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(vkBackends->physicalDevice, &physicalDeviceProperties);

		VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		if (counts & VK_SAMPLE_COUNT_8_BIT)  { return VK_SAMPLE_COUNT_8_BIT; }
		if (counts & VK_SAMPLE_COUNT_4_BIT)  { return VK_SAMPLE_COUNT_4_BIT; }
		if (counts & VK_SAMPLE_COUNT_2_BIT)  { return VK_SAMPLE_COUNT_2_BIT; }

		return VK_SAMPLE_COUNT_1_BIT;
	}

	static VkSampleCountFlagBits getSampleCountFlagEnum(LvnSampleCount samples)
	{
		switch (samples)
		{
			case Lvn_SampleCount_1_Bit: { return VK_SAMPLE_COUNT_1_BIT; }
			case Lvn_SampleCount_2_Bit: { return VK_SAMPLE_COUNT_2_BIT; }
			case Lvn_SampleCount_4_Bit: { return VK_SAMPLE_COUNT_4_BIT; }
			case Lvn_SampleCount_8_Bit: { return VK_SAMPLE_COUNT_8_BIT; }
			case Lvn_SampleCount_16_Bit: { return VK_SAMPLE_COUNT_16_BIT; }
			case Lvn_SampleCount_32_Bit: { return VK_SAMPLE_COUNT_32_BIT; }
			case Lvn_SampleCount_64_Bit: { return VK_SAMPLE_COUNT_64_BIT; }
			case Lvn_SampleCount_Max_Bit: { return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM; }
			default:
			{
				LVN_CORE_WARN("unknown sampler count enum (%d), setting to sample count enum 1 bit (default)", samples);
				return VK_SAMPLE_COUNT_1_BIT;
			}
		}
	}

	static uint32_t getSampleCountValue(VkSampleCountFlagBits samples)
	{
		switch (samples)
		{
			case VK_SAMPLE_COUNT_1_BIT: { return 1; }
			case VK_SAMPLE_COUNT_2_BIT: { return 2; }
			case VK_SAMPLE_COUNT_4_BIT: { return 4; }
			case VK_SAMPLE_COUNT_8_BIT: { return 8; }
			case VK_SAMPLE_COUNT_16_BIT: { return 16; }
			case VK_SAMPLE_COUNT_32_BIT: { return 32; }
			case VK_SAMPLE_COUNT_64_BIT: { return 64; }
			case VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM: { return 0x7FFFFFFF; }
			default:
			{
				LVN_CORE_WARN("could not find vulkan sample enum match!");
				return 1;
			}
		}
	}

	static VkSampleCountFlagBits getSupportedSampleCount(VulkanBackends* vkBackends, LvnSampleCount samples)
	{
		VkSampleCountFlagBits fbSampleCount = vks::getSampleCountFlagEnum(samples);
		VkSampleCountFlagBits maxSampleCount = vks::getMaxUsableSampleCount(vkBackends);

		uint32_t fbCount = getSampleCountValue(fbSampleCount);
		uint32_t maxCount = getSampleCountValue(maxSampleCount);

		if (fbCount > maxCount)
		{
			LVN_CORE_WARN("specified sample count (%u) is higher than the max sample count that is supported by the device (%u); using supported sample count instead!", fbCount, maxCount);
			return maxSampleCount;
		}

		return fbSampleCount;
	}

	static void initStandardVulkanPipelineSpecification(VulkanBackends* vkBackends, LvnContext* lvnctx)
	{
		LvnPipelineSpecification pipelineSpecification{};

		// Input Assembly
		pipelineSpecification.inputAssembly.topology = Lvn_TopologyType_Triangle;
		pipelineSpecification.inputAssembly.primitiveRestartEnable = false;

		// Viewport
		pipelineSpecification.viewport.x = 0.0f;
		pipelineSpecification.viewport.y = 0.0f;
		pipelineSpecification.viewport.width = 800.0f;
		pipelineSpecification.viewport.height = 600.0f;
		pipelineSpecification.viewport.minDepth = 0.0f;
		pipelineSpecification.viewport.maxDepth = 1.0f;

		// Scissor
		pipelineSpecification.scissor.offset = { 0, 0 };
		pipelineSpecification.scissor.extent = { 800, 600 };

		// Rasterizer
		pipelineSpecification.rasterizer.depthClampEnable = false;
		pipelineSpecification.rasterizer.rasterizerDiscardEnable = false;
		pipelineSpecification.rasterizer.lineWidth = 1.0f;
		pipelineSpecification.rasterizer.cullMode = Lvn_CullFaceMode_Back;
		pipelineSpecification.rasterizer.frontFace = Lvn_CullFrontFace_Clockwise;
		pipelineSpecification.rasterizer.depthBiasEnable = false;
		pipelineSpecification.rasterizer.depthBiasConstantFactor = 0.0f;
		pipelineSpecification.rasterizer.depthBiasClamp = 0.0f;
		pipelineSpecification.rasterizer.depthBiasSlopeFactor = 0.0f;

		// MultiSampling
		pipelineSpecification.multisampling.sampleShadingEnable = false;
		pipelineSpecification.multisampling.rasterizationSamples = Lvn_SampleCount_1_Bit;
		pipelineSpecification.multisampling.minSampleShading = 1.0f;
		pipelineSpecification.multisampling.sampleMask = nullptr;
		pipelineSpecification.multisampling.alphaToCoverageEnable = false;
		pipelineSpecification.multisampling.alphaToOneEnable = false;

		// Color Attachments
		pipelineSpecification.colorBlend.colorBlendAttachmentCount = 0; // If no attachments are provided, an attachment will automatically be created
		pipelineSpecification.colorBlend.pColorBlendAttachments = nullptr; 

		// Color Blend
		pipelineSpecification.colorBlend.logicOpEnable = false;
		pipelineSpecification.colorBlend.blendConstants[0] = 0.0f;
		pipelineSpecification.colorBlend.blendConstants[1] = 0.0f;
		pipelineSpecification.colorBlend.blendConstants[2] = 0.0f;
		pipelineSpecification.colorBlend.blendConstants[3] = 0.0f;

		// Depth Stencil
		pipelineSpecification.depthstencil.enableDepth = false;
		pipelineSpecification.depthstencil.depthOpCompare = Lvn_CompareOperation_Never;
		pipelineSpecification.depthstencil.enableStencil = false;
		pipelineSpecification.depthstencil.stencil.compareMask = 0x00;
		pipelineSpecification.depthstencil.stencil.writeMask = 0x00;
		pipelineSpecification.depthstencil.stencil.reference = 0;
		pipelineSpecification.depthstencil.stencil.compareOp = Lvn_CompareOperation_Never;
		pipelineSpecification.depthstencil.stencil.depthFailOp = Lvn_StencilOperation_Keep;
		pipelineSpecification.depthstencil.stencil.failOp = Lvn_StencilOperation_Keep;
		pipelineSpecification.depthstencil.stencil.passOp = Lvn_StencilOperation_Keep;

		lvnctx->defaultPipelineSpecification = pipelineSpecification;
		vkBackends->defaultPipelineSpecification = pipelineSpecification;
	}

	static VulkanPipeline createVulkanPipeline(VulkanBackends* vkBackends, VulkanPipelineCreateData* createData)
	{
		VulkanPipeline pipeline{};
		
		LvnPipelineSpecification* pipelineSpecification = createData->pipelineSpecification;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		
		inputAssembly.topology = vks::getVulkanTopologyTypeEnum(pipelineSpecification->inputAssembly.topology);
		inputAssembly.primitiveRestartEnable = pipelineSpecification->inputAssembly.primitiveRestartEnable;
		
		VkViewport viewport{};
		viewport.x = pipelineSpecification->viewport.x;
		viewport.y = pipelineSpecification->viewport.y;
		viewport.width = pipelineSpecification->viewport.width;
		viewport.height = pipelineSpecification->viewport.height;
		viewport.minDepth = pipelineSpecification->viewport.minDepth;
		viewport.maxDepth = pipelineSpecification->viewport.maxDepth;

		VkRect2D scissor{};
		scissor.offset.x = pipelineSpecification->scissor.offset.x;
		scissor.offset.y = pipelineSpecification->scissor.offset.y;
		scissor.extent.width = pipelineSpecification->scissor.extent.width;
		scissor.extent.height = pipelineSpecification->scissor.extent.height;

		LvnVector<VkDynamicState> dynamicStates;
		dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
		dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

		if (pipelineSpecification->depthstencil.enableStencil)
		{
			dynamicStates.push_back(VK_DYNAMIC_STATE_STENCIL_REFERENCE);
			dynamicStates.push_back(VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK);
			dynamicStates.push_back(VK_DYNAMIC_STATE_STENCIL_WRITE_MASK);
		}

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = pipelineSpecification->rasterizer.depthClampEnable;
		rasterizer.rasterizerDiscardEnable = pipelineSpecification->rasterizer.rasterizerDiscardEnable;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = pipelineSpecification->rasterizer.lineWidth;
		rasterizer.cullMode = vks::getVulkanCullModeFlagEnum(pipelineSpecification->rasterizer.cullMode);
		rasterizer.frontFace = vks::getVulkanCullFrontFaceEnum(pipelineSpecification->rasterizer.frontFace);
		rasterizer.depthBiasEnable = pipelineSpecification->rasterizer.depthBiasEnable;
		rasterizer.depthBiasConstantFactor = pipelineSpecification->rasterizer.depthBiasConstantFactor; // Optional
		rasterizer.depthBiasClamp = pipelineSpecification->rasterizer.depthBiasClamp; // Optional
		rasterizer.depthBiasSlopeFactor = pipelineSpecification->rasterizer.depthBiasSlopeFactor; // Optional

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = pipelineSpecification->multisampling.sampleShadingEnable;
		multisampling.rasterizationSamples = vks::getSupportedSampleCount(vkBackends, pipelineSpecification->multisampling.rasterizationSamples);
		multisampling.minSampleShading = pipelineSpecification->multisampling.minSampleShading; // Optional
		multisampling.pSampleMask = pipelineSpecification->multisampling.sampleMask; // Optional
		multisampling.alphaToCoverageEnable = pipelineSpecification->multisampling.alphaToCoverageEnable; // Optional
		multisampling.alphaToOneEnable = pipelineSpecification->multisampling.alphaToOneEnable; // Optional

		LvnVector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
		
		if (pipelineSpecification->colorBlend.colorBlendAttachmentCount == 0 || pipelineSpecification->colorBlend.pColorBlendAttachments == nullptr)
		{
			colorBlendAttachments.push_back(createColorAttachment());
		}
		else
		{
			for (uint32_t i = 0; i < pipelineSpecification->colorBlend.colorBlendAttachmentCount; i++)
			{
				LvnPipelineColorBlendAttachment attachment = pipelineSpecification->colorBlend.pColorBlendAttachments[i];

				VkPipelineColorBlendAttachmentState colorBlendAttachment{};
				colorBlendAttachment.colorWriteMask = vks::getColorComponents(attachment.colorWriteMask);
				colorBlendAttachment.blendEnable = attachment.blendEnable;
				colorBlendAttachment.srcColorBlendFactor = vks::getBlendFactorEnum(attachment.srcColorBlendFactor);
				colorBlendAttachment.dstColorBlendFactor = vks::getBlendFactorEnum(attachment.dstColorBlendFactor);
				colorBlendAttachment.colorBlendOp = vks::getBlendOperationEnum(attachment.colorBlendOp);
				colorBlendAttachment.srcAlphaBlendFactor = vks::getBlendFactorEnum(attachment.srcAlphaBlendFactor);
				colorBlendAttachment.dstAlphaBlendFactor = vks::getBlendFactorEnum(attachment.dstAlphaBlendFactor);
				colorBlendAttachment.alphaBlendOp = vks::getBlendOperationEnum(attachment.alphaBlendOp);

				colorBlendAttachments.push_back(colorBlendAttachment);
			}
		}

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = pipelineSpecification->colorBlend.logicOpEnable;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
		colorBlending.pAttachments = colorBlendAttachments.data();
		colorBlending.blendConstants[0] = pipelineSpecification->colorBlend.blendConstants[0]; // Optional
		colorBlending.blendConstants[1] = pipelineSpecification->colorBlend.blendConstants[1]; // Optional
		colorBlending.blendConstants[2] = pipelineSpecification->colorBlend.blendConstants[2]; // Optional
		colorBlending.blendConstants[3] = pipelineSpecification->colorBlend.blendConstants[3]; // Optional

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = pipelineSpecification->depthstencil.enableDepth ? VK_TRUE : VK_FALSE;
		depthStencil.depthWriteEnable = pipelineSpecification->depthstencil.enableDepth ? VK_TRUE : VK_FALSE;
		depthStencil.depthCompareOp = vks::getCompareOpEnum(pipelineSpecification->depthstencil.depthOpCompare);
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = pipelineSpecification->depthstencil.enableStencil ? VK_TRUE : VK_FALSE;
		depthStencil.back.compareMask = pipelineSpecification->depthstencil.stencil.compareMask;
		depthStencil.back.writeMask = pipelineSpecification->depthstencil.stencil.writeMask;
		depthStencil.back.reference = pipelineSpecification->depthstencil.stencil.reference;
		depthStencil.back.compareOp = vks::getCompareOpEnum(pipelineSpecification->depthstencil.stencil.compareOp);
		depthStencil.back.depthFailOp = vks::getStencilOpEnum(pipelineSpecification->depthstencil.stencil.depthFailOp);
		depthStencil.back.failOp = vks::getStencilOpEnum(pipelineSpecification->depthstencil.stencil.failOp);
		depthStencil.back.passOp = vks::getStencilOpEnum(pipelineSpecification->depthstencil.stencil.passOp);
		depthStencil.front = depthStencil.back;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		if (createData->descriptorSetLayoutCount != 0)
		{
			pipelineLayoutInfo.setLayoutCount = createData->descriptorSetLayoutCount;
			pipelineLayoutInfo.pSetLayouts = createData->pDescrptorSetLayouts;
		}
		else
		{
			pipelineLayoutInfo.setLayoutCount = 0;
			pipelineLayoutInfo.pSetLayouts = nullptr;
		}

		if (createData->pushConstantCount != 0)
		{
			pipelineLayoutInfo.pushConstantRangeCount = createData->pushConstantCount;
			pipelineLayoutInfo.pPushConstantRanges = createData->pPushConstants;
		}
		else
		{
			pipelineLayoutInfo.pushConstantRangeCount = 0;
			pipelineLayoutInfo.pPushConstantRanges = nullptr;
		}

		LVN_CORE_ASSERT(vkCreatePipelineLayout(vkBackends->device, &pipelineLayoutInfo, nullptr, &pipeline.pipelineLayout) == VK_SUCCESS, "vulkan - failed to create pipeline layout!");

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.renderPass = createData->renderPass;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = createData->shaderStages;
		pipelineInfo.pVertexInputState = &createData->vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipeline.pipelineLayout;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		LVN_CORE_ASSERT(vkCreateGraphicsPipelines(vkBackends->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.pipeline) == VK_SUCCESS, "vulkan - failed to create graphics pipeline!");

		return pipeline;
	}

	static VkShaderModule createShaderModule(VulkanBackends* vkBackends, const uint8_t* code, uint32_t size)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = size;
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code);
		
		VkShaderModule shaderModule;
		LVN_CORE_ASSERT(vkCreateShaderModule(vkBackends->device, &createInfo, nullptr, &shaderModule) == VK_SUCCESS, "vulkan - failed to create shader module!");

		return shaderModule;
	}

	static void createGraphicsPipeline(VulkanBackends* vkBackends)
	{
		LvnVector<uint8_t> vertbin = lvn::getFileSrcBin("/home/bma/Documents/dev/levikno/LeviknoEditor/res/shaders/vkvert.spv");
		LvnVector<uint8_t> fragbin = lvn::getFileSrcBin("/home/bma/Documents/dev/levikno/LeviknoEditor/res/shaders/vkfrag.spv");

		VkShaderModule vertShaderModule = createShaderModule(vkBackends, vertbin.data(), vertbin.size());
		VkShaderModule fragShaderModule = createShaderModule(vkBackends, fragbin.data(), fragbin.size());

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

		vkDestroyShaderModule(vkBackends->device, fragShaderModule, nullptr);
	    vkDestroyShaderModule(vkBackends->device, vertShaderModule, nullptr);
	}

} /* namespace vks */

LvnResult vksImplCreateContext(LvnGraphicsContext* graphicsContext, bool enableValidationLayers)
{
	VulkanBackends* vkBackends = new VulkanBackends();
	s_VkBackends = vkBackends;
	LvnContext* lvnctx = lvn::getContext();
	vkBackends->enableValidationLayers = lvnctx->vulkanValidationLayers;

	graphicsContext->createRenderPass = vksImplCreateRenderPass;
	graphicsContext->createPipeline = vksImplCreatePipeline;
	graphicsContext->createFrameBuffer = vksImplCreateFrameBuffer;
	graphicsContext->setDefaultPipelineSpecification = vksImplSetDefaultPipelineSpecification;
	graphicsContext->getDefaultPipelineSpecification = vksImplGetDefaultPipelineSpecification;
	graphicsContext->destroyRenderPass = vksImplDestroyRenderPass;
	graphicsContext->destroyPipeline = vksImplDestroyPipeline;
	graphicsContext->destroyFrameBuffer = vksImplDestroyFrameBuffer;
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
	graphicsContext->renderBindPipeline = vksImplBindPipeline;

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
	vkBackends->physicalDevices.resize(deviceCount);
	vkEnumeratePhysicalDevices(vkBackends->instance, &deviceCount, vkBackends->physicalDevices.data());

	// Create our own implementation of physical devices
	LvnPhysicalDevice* lvnDevices = (LvnPhysicalDevice*)lvn::memAlloc(deviceCount * sizeof(LvnPhysicalDevice));
	for (uint32_t i = 0; i < vkBackends->physicalDevices.size(); i++)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(vkBackends->physicalDevices[i], &deviceProperties);

		LvnPhysicalDeviceInfo deviceInfo{};
		deviceInfo.type = vks::getPhysicalDeviceTypeEnum(deviceProperties.deviceType);
			
		deviceInfo.apiVersion = deviceProperties.apiVersion;
		deviceInfo.driverVersion = deviceProperties.driverVersion;
		memcpy(deviceInfo.name, deviceProperties.deviceName, 226);
		lvnDevices[i].info = deviceInfo;
		lvnDevices[i].device = vkBackends->physicalDevices[i];
	}

	lvnctx->physicalDevices = lvnDevices;
	lvnctx->physicalDeviceCount = deviceCount;


	return Lvn_Result_Success;
}

void vksImplTerminateContext()
{
	VulkanBackends* vkBackends = s_VkBackends;
	LvnContext* lvnctx = getContext();

	vkDeviceWaitIdle(vkBackends->device);

	// sync objects
	for (uint32_t i = 0; i < vkBackends->maxFramesInFlight; i++)
	{
		vkDestroySemaphore(vkBackends->device, vkBackends->imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(vkBackends->device, vkBackends->renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(vkBackends->device, vkBackends->inFlightFences[i], nullptr);
	}

	lvn::memFree(vkBackends->renderFinishedSemaphores);
	lvn::memFree(vkBackends->imageAvailableSemaphores);
	lvn::memFree(vkBackends->inFlightFences);

	vkDestroyCommandPool(vkBackends->device, vkBackends->commandPool, nullptr);
	lvn::memFree(vkBackends->commandBuffers);

	for (uint32_t i = 0; i < vkBackends->windowSurfaceData.size(); i++)
	{
		// swap chain images
		for (uint32_t j = 0; j < vkBackends->windowSurfaceData[i].swapChainImageViewCount; j++)
		{
			vkDestroyImageView(vkBackends->device, vkBackends->windowSurfaceData[i].swapChainImageViews[j], nullptr);
		}

		// frame buffers
		for (uint32_t j = 0; j < vkBackends->windowSurfaceData[i].frameBufferCount; j++)
		{
			vkDestroyFramebuffer(vkBackends->device, vkBackends->windowSurfaceData[i].frameBuffers[j], nullptr);
		}

		if (vkBackends->windowSurfaceData[i].frameBuffers)
			lvn::memFree(vkBackends->windowSurfaceData[i].frameBuffers);

		if (vkBackends->windowSurfaceData[i].swapChainImageViews)
			lvn::memFree(vkBackends->windowSurfaceData[i].swapChainImageViews);

		if (vkBackends->windowSurfaceData[i].swapChainImages)
			lvn::memFree(vkBackends->windowSurfaceData[i].swapChainImages);

		// swap chain
		vkDestroySwapchainKHR(vkBackends->device, vkBackends->windowSurfaceData[i].swapChain, nullptr);

		// render pass
		vkDestroyRenderPass(vkBackends->device, vkBackends->windowSurfaceData[i].renderPass, nullptr);

		// window surface
		vkDestroySurfaceKHR(vkBackends->instance, vkBackends->windowSurfaceData[i].surface, nullptr);
	}


	// logical device
	vkDestroyDevice(vkBackends->device, nullptr);

	// debug validation layers
	if (vkBackends->enableValidationLayers)
		vks::destroyDebugUtilsMessengerEXT(vkBackends->instance, vkBackends->debugMessenger, nullptr);
		
	// allocated physical devices
	if (lvnctx->physicalDevices)
		memFree(lvnctx->physicalDevices);

	// instance
	vkDestroyInstance(vkBackends->instance, nullptr);

	delete s_VkBackends;
}

LvnResult vksImplRenderInit(LvnRendererBackends* renderBackends)
{
	VulkanBackends* vkBackends = s_VkBackends;
	vks::initStandardVulkanPipelineSpecification(vkBackends, lvn::getContext()); // set default pipeline fixed functions so that they don't need to be set on every pipeline creation

	vkBackends->physicalDevice = static_cast<VkPhysicalDevice>(renderBackends->physicalDevice->device);
	vkBackends->maxFramesInFlight = renderBackends->maxFramesInFlight != 0 ? renderBackends->maxFramesInFlight : 1;

	if (vkBackends->enableValidationLayers && !vks::checkValidationLayerSupport())
		LVN_CORE_WARN("vulkan validation layers enabled, but not available!");
	else
		vks::setupDebugMessenger(vkBackends);

	// create surface
	if (!renderBackends->pWindows)
	{
		LVN_CORE_ERROR("renderBackends->pWindows was nullptr, cannot initialize rendering without a window first");
		return Lvn_Result_Failure;
	}
	else if (!renderBackends->windowCount)
	{
		LVN_CORE_WARN("renderBackends->windowCount was 0, cannot initialize rendering without a window first");
		return Lvn_Result_Failure;
	}

	VulkanWindowSurfaceData surfaceData{};
	GLFWwindow* glfwWin = static_cast<GLFWwindow*>(renderBackends->pWindows[0]->nativeWindow);
	LVN_CORE_ASSERT(glfwCreateWindowSurface(vkBackends->instance, glfwWin, nullptr, &surfaceData.surface) == VK_SUCCESS, "vulkan - failed to create window surface!");
	vks::createLogicalDevice(vkBackends, surfaceData.surface); // create logical device once

	// create command buffer pool
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = vkBackends->deviceIndices.graphicsIndex;

	LVN_CORE_ASSERT(vkCreateCommandPool(vkBackends->device, &poolInfo, nullptr, &vkBackends->commandPool) == VK_SUCCESS, "vulkan - failed to create command pool!");

	// get swap chain specs
	VulkanSwapChainSupportDetails swapChainSupport = vks::querySwapChainSupport(surfaceData.surface, vkBackends->physicalDevice);
	LVN_CORE_ASSERT(!swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty(), "vulkan - physical device does not have swap chain support formats or present modes!");

	VkSurfaceFormatKHR surfaceFormat = vks::chooseSwapSurfaceFormat(swapChainSupport.formats.data(), swapChainSupport.formats.size());
	VkPresentModeKHR presentMode = vks::chooseSwapPresentMode(swapChainSupport.presentModes.data(), swapChainSupport.presentModes.size());
	VkExtent2D extent = vks::chooseSwapExtent(glfwWin, &swapChainSupport.capabilities);
	vkBackends->defaultSwapChainFormat = surfaceFormat.format;

	vks::createSwapChain(vkBackends, &surfaceData, swapChainSupport, surfaceFormat, presentMode, extent);
	vks::createImageViews(vkBackends, &surfaceData);
	vks::createRenderPass(vkBackends, &surfaceData, surfaceFormat.format);
	vks::createFrameBuffers(vkBackends, &surfaceData);

	vkBackends->windowSurfaceData.push_back(surfaceData);
	renderBackends->pWindows[0]->apiData = &vkBackends->windowSurfaceData[0];

	if (renderBackends->windowCount > 1)
	{
		for (uint32_t i = 1; i < renderBackends->windowCount; i++) // create surface for each window if there is more than one window
		{
			VulkanWindowSurfaceData surfaceDataExt{};
			GLFWwindow* glfwWin = static_cast<GLFWwindow*>(renderBackends->pWindows[i]->nativeWindow);
			LVN_CORE_ASSERT(glfwCreateWindowSurface(vkBackends->instance, glfwWin, nullptr, &surfaceDataExt.surface) == VK_SUCCESS, "vulkan - failed to create window surface!");
			vks::createSwapChain(vkBackends, &surfaceDataExt, swapChainSupport, surfaceFormat, presentMode, extent);
			vks::createImageViews(vkBackends, &surfaceDataExt);
			vks::createRenderPass(vkBackends, &surfaceDataExt, surfaceFormat.format);
			vks::createFrameBuffers(vkBackends, &surfaceDataExt);

			vkBackends->windowSurfaceData.push_back(surfaceDataExt);
			renderBackends->pWindows[i]->apiData = &vkBackends->windowSurfaceData.back();
		}
	}

	// vks::createGraphicsPipeline(vkBackends);
	vks::createCommandBuffers(vkBackends);
	vks::createSyncObjects(vkBackends);

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

void vksImplRenderBeginNextFrame(LvnWindow* window)
{
	VulkanBackends* vkBackends = s_VkBackends;
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);

	vkWaitForFences(vkBackends->device, 1, &vkBackends->inFlightFences[vkBackends->currentFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(vkBackends->device, 1, &vkBackends->inFlightFences[vkBackends->currentFrame]);

    vkAcquireNextImageKHR(vkBackends->device, surfaceData->swapChain, UINT64_MAX, vkBackends->imageAvailableSemaphores[vkBackends->currentFrame], VK_NULL_HANDLE, &surfaceData->imageIndex);


	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	vkResetCommandBuffer(vkBackends->commandBuffers[vkBackends->currentFrame], 0);
	LVN_CORE_ASSERT(vkBeginCommandBuffer(vkBackends->commandBuffers[vkBackends->currentFrame], &beginInfo) == VK_SUCCESS, "vulkan - failed to begin recording command buffer!");
}

void vksImplRenderDrawSubmit(LvnWindow* window)
{
	VulkanBackends* vkBackends = s_VkBackends;
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);


	LVN_CORE_ASSERT(vkEndCommandBuffer(vkBackends->commandBuffers[vkBackends->currentFrame]) == VK_SUCCESS, "vulkan - failed to record command buffer!");

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { vkBackends->imageAvailableSemaphores[vkBackends->currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vkBackends->commandBuffers[vkBackends->currentFrame];

	VkSemaphore signalSemaphores[] = { vkBackends->renderFinishedSemaphores[vkBackends->currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	LVN_CORE_ASSERT(vkQueueSubmit(vkBackends->graphicsQueue, 1, &submitInfo, vkBackends->inFlightFences[vkBackends->currentFrame]) == VK_SUCCESS, "vulkan - failed to submit draw command buffer!");


	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { surfaceData->swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &surfaceData->imageIndex;
	presentInfo.pResults = nullptr; // Optional

	vkQueuePresentKHR(vkBackends->presentQueue, &presentInfo);

	// advance to next frame in flight
	vkBackends->currentFrame = (vkBackends->currentFrame + 1) % vkBackends->maxFramesInFlight;
}

void vksImplRenderBeginRenderPass(LvnWindow* window)
{
	VulkanBackends* vkBackends = s_VkBackends;
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = surfaceData->renderPass;
	renderPassInfo.framebuffer = surfaceData->frameBuffers[surfaceData->imageIndex];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = surfaceData->swapChainExtent;
	VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(vkBackends->commandBuffers[vkBackends->currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(surfaceData->swapChainExtent.width);
	viewport.height = static_cast<float>(surfaceData->swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(vkBackends->commandBuffers[vkBackends->currentFrame], 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = surfaceData->swapChainExtent;
	vkCmdSetScissor(vkBackends->commandBuffers[vkBackends->currentFrame], 0, 1, &scissor);

}

void vksImplRenderEndRenderPass(LvnWindow* window)
{
	VulkanBackends* vkBackends = s_VkBackends;

	vkCmdDraw(vkBackends->commandBuffers[vkBackends->currentFrame], 3, 1, 0, 0); // temporary

	vkCmdEndRenderPass(vkBackends->commandBuffers[vkBackends->currentFrame]);
}

void vksImplBindPipeline(LvnWindow* window, LvnPipeline* pipeline)
{
	VulkanBackends* vkBackends = s_VkBackends;

	VkPipeline graphicsPipeline = static_cast<VkPipeline>(pipeline->nativePipeline);
	vkCmdBindPipeline(vkBackends->commandBuffers[vkBackends->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
}

LvnResult vksImplCreateRenderPass(LvnRenderPass** renderPass, LvnRenderPassCreateInfo* createInfo)
{
	VulkanBackends* vkBackends = s_VkBackends;

	LvnVector<VkAttachmentDescription> attachments(createInfo->attachmentCount);

	LvnVector<VkAttachmentReference> colorReferences;
	LvnVector<VkAttachmentReference> resolveReferences;
	VkAttachmentReference depthReference;
	bool hasDepth = false, hasResolve = false, multisampling = false;

	for (uint32_t i = 0; i < createInfo->attachmentCount; i++)
	{
		LvnRenderPassAttachment* attachment = &createInfo->pAttachments[i];

		VkAttachmentDescription attachmentDescription{};
		attachmentDescription.format = vks::getVulkanFormatEnum(attachment->format);
		attachmentDescription.samples = vks::getSampleCountFlagEnum(attachment->samples);
		attachmentDescription.loadOp = vks::getVulkanLoadOpEnum(attachment->loadOp);
		attachmentDescription.storeOp = vks::getVulkanStoreOpEnum(attachment->storeOp);
		attachmentDescription.stencilLoadOp = vks::getVulkanLoadOpEnum(attachment->stencilLoadOp);
		attachmentDescription.stencilStoreOp = vks::getVulkanStoreOpEnum(attachment->stencilStoreOp);
		attachmentDescription.initialLayout = vks::getVulkanImageLayoutEnum(attachment->initialLayout);
		attachmentDescription.finalLayout = vks::getVulkanImageLayoutEnum(attachment->finalLayout);

		attachments[i] = attachmentDescription;

		if (attachment->samples != Lvn_SampleCount_1_Bit) { multisampling = true; }

		if (attachment->type == Lvn_AttachmentType_Color) // color
		{
			colorReferences.push_back({ i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
		}
		else if (attachment->type == Lvn_AttachmentType_Depth) // depth
		{
			if (hasDepth) // check depth included once
			{
				LVN_CORE_ERROR("attachment->type is \'Lvn_AttachmentType_Depth\' but a depth attachment has already been included before, a render pass cannot have more than one depth attachments");
				return Lvn_Result_Failure;
			}
			hasDepth = true;

			depthReference = { i, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
		}
		else if (attachment->type == Lvn_AttachmentType_Resolve) // multisampling
		{
			resolveReferences.push_back({ i, vks::getVulkanImageLayoutEnum(attachment->finalLayout) });
			hasResolve = true;
		}
		else
		{
			LVN_CORE_ERROR("cannot identify attachment->type to its attachment reference");
			return Lvn_Result_Failure;
		}
	}

	if (hasResolve != multisampling)
	{
		LVN_CORE_ERROR("attachment->samples has samples greater than one (multisampling) but not resolve attachments were included");
		return Lvn_Result_Failure;
	}

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = colorReferences.size();
	subpass.pColorAttachments = colorReferences.data();
	if (hasDepth) { subpass.pDepthStencilAttachment = &depthReference; }
	if (hasResolve) { subpass.pResolveAttachments = resolveReferences.data(); }

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = attachments.size();
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	VkRenderPass vkRenderPass;
	LVN_CORE_ASSERT(vkCreateRenderPass(vkBackends->device, &renderPassInfo, nullptr, &vkRenderPass) == VK_SUCCESS, "vulkan - failed to create render pass!");
	
	*renderPass = new LvnRenderPass();
	LvnRenderPass* renderPassPtr = *renderPass;
	renderPassPtr->nativeRenderPass = vkRenderPass;

	return Lvn_Result_Success;
}

LvnResult vksImplCreatePipeline(LvnPipeline** pipeline, LvnPipelineCreateInfo* createInfo)
{
	VulkanBackends* vkBackends = s_VkBackends;

	LvnVector<uint8_t> vertbin = lvn::getFileSrcBin("/home/bma/Documents/dev/levikno/LeviknoEditor/res/shaders/vkvert.spv");
	LvnVector<uint8_t> fragbin = lvn::getFileSrcBin("/home/bma/Documents/dev/levikno/LeviknoEditor/res/shaders/vkfrag.spv");

	VkShaderModule vertShaderModule = vks::createShaderModule(vkBackends, vertbin.data(), vertbin.size());
	VkShaderModule fragShaderModule = vks::createShaderModule(vkBackends, fragbin.data(), fragbin.size());

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

	VkRenderPass renderPass = createInfo->renderPass != nullptr ? static_cast<VkRenderPass>(createInfo->renderPass->nativeRenderPass) : static_cast<VulkanWindowSurfaceData*>(createInfo->window->apiData)->renderPass;

	VulkanPipelineCreateData pipelineCreateData{};
	pipelineCreateData.shaderStages = shaderStages;
	pipelineCreateData.shaderStageCount = ARRAY_LEN(shaderStages);
	pipelineCreateData.vertexInputInfo = vertexInputInfo;
	pipelineCreateData.renderPass = renderPass;
	pipelineCreateData.pipelineSpecification = createInfo->pipelineSpecification != nullptr ? createInfo->pipelineSpecification : &vkBackends->defaultPipelineSpecification;

	VulkanPipeline vkPipeline = vks::createVulkanPipeline(vkBackends, &pipelineCreateData);

	vkDestroyShaderModule(vkBackends->device, fragShaderModule, nullptr);
	vkDestroyShaderModule(vkBackends->device, vertShaderModule, nullptr);

	*pipeline = new LvnPipeline();
	LvnPipeline* pipelinePtr = *pipeline;
	pipelinePtr->nativePipeline = vkPipeline.pipeline;
	pipelinePtr->nativePipelineLayout = vkPipeline.pipelineLayout;

	return Lvn_Result_Success;
}

LvnResult vksImplCreateFrameBuffer(LvnFrameBuffer** frameBuffer, LvnFrameBufferCreateInfo* createInfo)
{
	return Lvn_Result_Success;
}

void vksImplSetDefaultPipelineSpecification(LvnPipelineSpecification* pipelineSpecification)
{
	s_VkBackends->defaultPipelineSpecification = *pipelineSpecification;
}

LvnPipelineSpecification vksImplGetDefaultPipelineSpecification()
{
	return s_VkBackends->defaultPipelineSpecification;
}

void vksImplDestroyRenderPass(LvnRenderPass* renderPass)
{
	VkRenderPass vkRenderPass = static_cast<VkRenderPass>(renderPass->nativeRenderPass);
	vkDestroyRenderPass(s_VkBackends->device, vkRenderPass, nullptr);
	delete renderPass;
}

void vksImplDestroyPipeline(LvnPipeline* pipeline)
{
	VulkanBackends* vkBackends = s_VkBackends;

	VkPipeline vkPipeline = static_cast<VkPipeline>(pipeline->nativePipeline);
	VkPipelineLayout vkPipelineLayout = static_cast<VkPipelineLayout>(pipeline->nativePipelineLayout);

	vkDestroyPipeline(vkBackends->device, vkPipeline, nullptr);
    vkDestroyPipelineLayout(vkBackends->device, vkPipelineLayout, nullptr);
	delete pipeline;
}

void vksImplDestroyFrameBuffer(LvnFrameBuffer* frameBuffer)
{

}


} /* namespace lvn */
