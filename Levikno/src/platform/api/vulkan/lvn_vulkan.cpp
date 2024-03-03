#include "lvn_vulkan.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "lvn_vulkanBackends.h"
#include "vk_mem_alloc.h"

#ifdef LVN_DEBUG
	#define LVN_CORE_CALL_ASSERT(x, ...) LVN_CORE_ASSERT(x, __VA_ARGS__)
#else
	#define LVN_CORE_CALL_ASSERT(x, ...) (x)
#endif

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
	static VKAPI_ATTR VkBool32 VKAPI_CALL       debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	static bool                                 checkValidationLayerSupport();
	static LvnVector<const char*>               getRequiredExtensions(VulkanBackends* vkBackends);
	static VkResult                             createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	static void                                 destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	static void                                 fillVulkanDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	static void                                 setupDebugMessenger(VulkanBackends* vkBackends);
	static LvnPhysicalDeviceType                getPhysicalDeviceTypeEnum(VkPhysicalDeviceType type);
	static bool                                 checkDeviceExtensionSupport(VkPhysicalDevice device);
	static VulkanSwapChainSupportDetails        querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice device);
	static VulkanQueueFamilyIndices             findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	static uint32_t                             findMemoryType(VulkanBackends* vkBackends, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	static void                                 createLogicalDevice(VulkanBackends* vkBackends, VkSurfaceKHR surface);
	static void                                 createRenderPass(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData, VkFormat format);
	static VkSurfaceFormatKHR                   chooseSwapSurfaceFormat(const VkSurfaceFormatKHR* pAvailableFormats, uint32_t count);
	static VkPresentModeKHR                     chooseSwapPresentMode(const VkPresentModeKHR* pAvailablePresentModes, uint32_t count);
	static VkExtent2D                           chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR* capabilities);
	static void                                 createSwapChain(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData, VulkanSwapChainSupportDetails swapChainSupport, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode, VkExtent2D extent);
	static void                                 createImageViews(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
	static void                                 createFrameBuffers(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
	static void                                 createCommandBuffers(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
	static void                                 createSyncObjects(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
	static void                                 cleanSwapChain(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
	static void                                 recreateSwapChain(VulkanBackends* vkBackends, LvnWindow* window);
	static VkPrimitiveTopology                  getVulkanTopologyTypeEnum(LvnTopologyType topologyType);
	static VkCullModeFlags                      getVulkanCullModeFlagEnum(LvnCullFaceMode cullFaceMode);
	static VkFrontFace                          getVulkanCullFrontFaceEnum(LvnCullFrontFace cullFrontFace);
	static VkFormat                             getVulkanFormatEnum(LvnImageFormat format);
	static VkAttachmentLoadOp                   getVulkanLoadOpEnum(LvnAttachmentLoadOperation loadOp);
	static VkAttachmentStoreOp                  getVulkanStoreOpEnum(LvnAttachmentStoreOperation storeOp);
	static VkImageLayout                        getVulkanImageLayoutEnum(LvnImageLayout layout);
	static VkColorComponentFlags                getColorComponents(LvnPipelineColorWriteMask colorMask);
	static VkBlendFactor                        getBlendFactorEnum(LvnColorBlendFactor blendFactor);
	static VkBlendOp                            getBlendOperationEnum(LvnColorBlendOperation blendOp);
	static VkCompareOp                          getCompareOpEnum(LvnCompareOperation compare);
	static VkStencilOp                          getStencilOpEnum(LvnStencilOperation stencilOp);
	static VkFormat                             getVertexAttributeFormatEnum(LvnVertexDataType type);
	static VkPipelineColorBlendAttachmentState  createColorAttachment();
	static VkSampleCountFlagBits                getMaxUsableSampleCount(VulkanBackends* vkBackends);
	static VkSampleCountFlagBits                getSampleCountFlagEnum(LvnSampleCount samples);
	static uint32_t                             getSampleCountValue(VkSampleCountFlagBits samples);
	static VkSampleCountFlagBits                getSupportedSampleCount(VulkanBackends* vkBackends, LvnSampleCount samples);
	static VkDescriptorType                     getDescriptorTypeEnum(LvnDescriptorType type);
	static VkDescriptorType                     getUniformBufferTypeEnum(LvnUniformBufferType type);
	static VkShaderStageFlags                   getShaderStageFlagEnum(LvnShaderStage stage);
	static void                                 initStandardVulkanPipelineSpecification(VulkanBackends* vkBackends, LvnContext* lvnctx);
	static VulkanPipeline                       createVulkanPipeline(VulkanBackends* vkBackends, VulkanPipelineCreateData* createData);
	static VkShaderModule                       createShaderModule(VulkanBackends* vkBackends, const uint8_t* code, uint32_t size);
	static void                                 createGraphicsPipeline(VulkanBackends* vkBackends);
	static void                                 createBuffer(VulkanBackends* vkBackends, VkBuffer* buffer, VmaAllocation* bufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memUsage);
	static void                                 copyBuffer(VulkanBackends* vkBackends, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset);


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

		LVN_CORE_CALL_ASSERT(createDebugUtilsMessengerEXT(vkBackends->instance, &createInfo, nullptr, &vkBackends->debugMessenger) == VK_SUCCESS, "vulkan - failed to set up debug messenger!");
	}

	static LvnPhysicalDeviceType getPhysicalDeviceTypeEnum(VkPhysicalDeviceType type)
	{
		switch (type)
		{
			case VK_PHYSICAL_DEVICE_TYPE_OTHER:          { return Lvn_PhysicalDeviceType_Other; }
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: { return Lvn_PhysicalDeviceType_Integrated_GPU; }
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   { return Lvn_PhysicalDeviceType_Discrete_GPU; }
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    { return Lvn_PhysicalDeviceType_Virtual_GPU; }
			case VK_PHYSICAL_DEVICE_TYPE_CPU:            { return Lvn_PhysicalDeviceType_CPU; }
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

	static uint32_t findMemoryType(VulkanBackends* vkBackends, uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(vkBackends->physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if (typeFilter & (1 << i)) { return i; }
		}

		LVN_CORE_ASSERT(false, "vulkan - failed to find suitable memory type for physical device!");
	}

	static void createLogicalDevice(VulkanBackends* vkBackends, VkSurfaceKHR surface)
	{
		// Find queue families
		VulkanQueueFamilyIndices queueIndices = vks::findQueueFamilies(vkBackends->physicalDevice, surface);
		vkBackends->deviceIndices = queueIndices;

		// Check queue families
		LVN_CORE_ASSERT(queueIndices.has_graphics && queueIndices.has_present, "vulkan - physical device does not support queue families needed!");

		// Check device extension support
		LVN_CORE_CALL_ASSERT(vks::checkDeviceExtensionSupport(vkBackends->physicalDevice), "vulkan - physical device does not support required extensions!");

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

		LVN_CORE_CALL_ASSERT(vkCreateDevice(vkBackends->physicalDevice, &createInfo, nullptr, &vkBackends->device) == VK_SUCCESS, "vulkan - failed to create logical device!");


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

		LVN_CORE_CALL_ASSERT(vkCreateRenderPass(vkBackends->device, &renderPassInfo, nullptr, &surfaceData->renderPass) == VK_SUCCESS, "vulkan - failed to create render pass!");
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
					LVN_CORE_TRACE("[vulkan] vulkan extension found: %s", availableExtensions[j].extensionName);
					extensionFound = true;
					break;
				}
			}

			if (!extensionFound)
			{
				LVN_CORE_ERROR("[vulkan] required vulkan extension not found: %s", requiredExtensions[i]);
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

		LVN_CORE_CALL_ASSERT(vkCreateSwapchainKHR(vkBackends->device, &createInfo, nullptr, &surfaceData->swapChain) == VK_SUCCESS, "vulkan - failed to create swap chain!");

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

			LVN_CORE_CALL_ASSERT(vkCreateImageView(vkBackends->device, &createInfo, nullptr, &surfaceData->swapChainImageViews[i]) == VK_SUCCESS, "vulkan - failed to create image views!");
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

			LVN_CORE_CALL_ASSERT(vkCreateFramebuffer(vkBackends->device, &framebufferInfo, nullptr, &surfaceData->frameBuffers[i]) == VK_SUCCESS, "vulkan - failed to create framebuffer!");
		}
	}

	static void createCommandBuffers(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = vkBackends->commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = vkBackends->maxFramesInFlight;

		surfaceData->commandBuffers = (VkCommandBuffer*)lvn::memAlloc(vkBackends->maxFramesInFlight * sizeof(VkCommandBuffer));
		LVN_CORE_CALL_ASSERT(vkAllocateCommandBuffers(vkBackends->device, &allocInfo, surfaceData->commandBuffers) == VK_SUCCESS, "vulkan - failed to allocate command buffers!");
	}

	static void createSyncObjects(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData)
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		surfaceData->imageAvailableSemaphores = (VkSemaphore*)lvn::memAlloc(vkBackends->maxFramesInFlight * sizeof(VkSemaphore));
		surfaceData->renderFinishedSemaphores = (VkSemaphore*)lvn::memAlloc(vkBackends->maxFramesInFlight * sizeof(VkSemaphore));
		surfaceData->inFlightFences = (VkFence*)lvn::memAlloc(vkBackends->maxFramesInFlight * sizeof(VkFence));

		for (uint32_t i = 0; i < vkBackends->maxFramesInFlight; i++)
		{
			LVN_CORE_CALL_ASSERT(vkCreateSemaphore(vkBackends->device, &semaphoreInfo, nullptr, &surfaceData->imageAvailableSemaphores[i]) == VK_SUCCESS, "vulkan - failed to create semaphore");
			LVN_CORE_CALL_ASSERT(vkCreateSemaphore(vkBackends->device, &semaphoreInfo, nullptr, &surfaceData->renderFinishedSemaphores[i]) == VK_SUCCESS, "vulkan - failed to create semaphore");
			LVN_CORE_CALL_ASSERT(vkCreateFence(vkBackends->device, &fenceInfo, nullptr, &surfaceData->inFlightFences[i]) == VK_SUCCESS, "vulkan - failed to create fence");
		}
	}

	static void cleanSwapChain(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData)
	{
		// swap chain images
		for (uint32_t j = 0; j < surfaceData->swapChainImageViewCount; j++)
		{
			vkDestroyImageView(vkBackends->device, surfaceData->swapChainImageViews[j], nullptr);
		}

		// frame buffers
		for (uint32_t j = 0; j < surfaceData->frameBufferCount; j++)
		{
			vkDestroyFramebuffer(vkBackends->device, surfaceData->frameBuffers[j], nullptr);
		}

		if (surfaceData->frameBuffers)
			lvn::memFree(surfaceData->frameBuffers);

		if (surfaceData->swapChainImageViews)
			lvn::memFree(surfaceData->swapChainImageViews);

		if (surfaceData->swapChainImages)
			lvn::memFree(surfaceData->swapChainImages);

		// swap chain
		vkDestroySwapchainKHR(vkBackends->device, surfaceData->swapChain, nullptr);
	}

	static void recreateSwapChain(VulkanBackends* vkBackends, LvnWindow* window)
	{
		vkDeviceWaitIdle(vkBackends->device);

		VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);

		vks::cleanSwapChain(vkBackends, surfaceData);

		VulkanSwapChainSupportDetails swapChainSupport = vks::querySwapChainSupport(surfaceData->surface, vkBackends->physicalDevice);
		LVN_CORE_ASSERT(!swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty(), "vulkan - physical device does not have swap chain support formats or present modes!");

		VkSurfaceFormatKHR surfaceFormat = vks::chooseSwapSurfaceFormat(swapChainSupport.formats.data(), swapChainSupport.formats.size());
		VkPresentModeKHR presentMode = vks::chooseSwapPresentMode(swapChainSupport.presentModes.data(), swapChainSupport.presentModes.size());
		VkExtent2D extent = vks::chooseSwapExtent(glfwWin, &swapChainSupport.capabilities);

		vks::createSwapChain(vkBackends, surfaceData, swapChainSupport, surfaceFormat, presentMode, extent);
		vks::createImageViews(vkBackends, surfaceData);
		vks::createFrameBuffers(vkBackends, surfaceData);
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

	static VkFormat getVertexAttributeFormatEnum(LvnVertexDataType type)
	{
		switch (type)
		{
			case Lvn_VertexDataType_None: { return VK_FORMAT_UNDEFINED; }
			case Lvn_VertexDataType_Float: { return VK_FORMAT_R32_SFLOAT; }
			case Lvn_VertexDataType_Double: { return VK_FORMAT_R64_SFLOAT; }
			case Lvn_VertexDataType_Int: { return VK_FORMAT_R32_SINT; }
			case Lvn_VertexDataType_UnsignedInt: { return VK_FORMAT_R32_UINT; }
			case Lvn_VertexDataType_Bool: { return VK_FORMAT_R8_SINT; }
			case Lvn_VertexDataType_Vec2: { return VK_FORMAT_R32G32_SFLOAT; }
			case Lvn_VertexDataType_Vec3: { return VK_FORMAT_R32G32B32_SFLOAT; }
			case Lvn_VertexDataType_Vec4: { return VK_FORMAT_R32G32B32A32_SFLOAT; }
			case Lvn_VertexDataType_Vec2i: { return VK_FORMAT_R32G32_SINT; }
			case Lvn_VertexDataType_Vec3i: { return VK_FORMAT_R32G32B32_SINT; }
			case Lvn_VertexDataType_Vec4i: { return VK_FORMAT_R32G32B32A32_SINT; }
			case Lvn_VertexDataType_Vec2ui: { return VK_FORMAT_R32G32_UINT; }
			case Lvn_VertexDataType_Vec3ui: { return VK_FORMAT_R32G32B32_UINT; }
			case Lvn_VertexDataType_Vec4ui: { return VK_FORMAT_R32G32B32A32_UINT; }
			case Lvn_VertexDataType_Vec2d: { return VK_FORMAT_R64G64_SFLOAT; }
			case Lvn_VertexDataType_Vec3d: { return VK_FORMAT_R64G64B64_SFLOAT; }
			case Lvn_VertexDataType_Vec4d: { return VK_FORMAT_R64G64B64A64_SFLOAT; }
			default:
			{
				LVN_CORE_WARN("uknown vertex attribute format type enum (%d), setting to format type undefined", type);
				return VK_FORMAT_UNDEFINED;
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

	static VkDescriptorType getDescriptorTypeEnum(LvnDescriptorType type)
	{
		switch (type)
		{
			case Lvn_DescriptorType_Sampler: { return VK_DESCRIPTOR_TYPE_SAMPLER; }
			case Lvn_DescriptorType_CombinedImageSampler: { return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; }
			case Lvn_DescriptorType_SampledImage: { return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE; }
			case Lvn_DescriptorType_UniformBuffer: { return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; }
			case Lvn_DescriptorType_StorageBuffer: { return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; }

			default:
			{
				LVN_CORE_WARN("unknown descriptor type enum (%d), setting to descriptor type sampler (defualt)", type);
				return VK_DESCRIPTOR_TYPE_SAMPLER;
			}
		}
	}

	static VkDescriptorType getUniformBufferTypeEnum(LvnUniformBufferType type)
	{
		switch (type)
		{
			case Lvn_UniformBufferType_Uniform: { return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; }
			case Lvn_UniformBufferType_Storage: { return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; }

			default:
			{
				LVN_CORE_WARN("unknown buffer enum type (%u), setting to buffer type uniform buffer (defualt)", type);
				return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			}
		}
	}

	static VkShaderStageFlags getShaderStageFlagEnum(LvnShaderStage stage)
	{
		switch (stage)
		{
			case Lvn_ShaderStage_All: { return VK_SHADER_STAGE_ALL; }
			case Lvn_ShaderStage_Vertex: { return VK_SHADER_STAGE_VERTEX_BIT; }
			case LvnShaderStage_Fragment: { return VK_SHADER_STAGE_FRAGMENT_BIT; }

			default:
			{
				LVN_CORE_WARN("unknown shader stage enum type (%u), setting stage to \'VK_SHADER_STAGE_ALL\' as default", stage);
				return VK_SHADER_STAGE_ALL;
			}
		}
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

		LVN_CORE_CALL_ASSERT(vkCreatePipelineLayout(vkBackends->device, &pipelineLayoutInfo, nullptr, &pipeline.pipelineLayout) == VK_SUCCESS, "vulkan - failed to create pipeline layout!");

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

		LVN_CORE_CALL_ASSERT(vkCreateGraphicsPipelines(vkBackends->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.pipeline) == VK_SUCCESS, "vulkan - failed to create graphics pipeline!");

		return pipeline;
	}

	static VkShaderModule createShaderModule(VulkanBackends* vkBackends, const uint8_t* code, uint32_t size)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = size;
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code);
		
		VkShaderModule shaderModule;
		LVN_CORE_CALL_ASSERT(vkCreateShaderModule(vkBackends->device, &createInfo, nullptr, &shaderModule) == VK_SUCCESS, "vulkan - failed to create shader module!");

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

	static void createBuffer(VulkanBackends* vkBackends, VkBuffer* buffer, VmaAllocation* bufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memUsage)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = memUsage;

		LVN_CORE_CALL_ASSERT(vmaCreateBuffer(vkBackends->vmaAllocator, &bufferInfo, &allocInfo, buffer, bufferMemory, nullptr) == VK_SUCCESS, "vulkan - failed to create buffer!");
		LVN_CORE_TRACE("[vulkan] created buffer <VkBuffer> (%p), buffer memory: (%p), buffer size: %zu bytes", *buffer, *bufferMemory, size);
	}

	void copyBuffer(VulkanBackends* vkBackends, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = vkBackends->commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(vkBackends->device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		copyRegion.srcOffset = srcOffset;
		copyRegion.dstOffset = dstOffset;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(vkBackends->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(vkBackends->graphicsQueue);

		vkFreeCommandBuffers(vkBackends->device, vkBackends->commandPool, 1, &commandBuffer);
	}

} /* namespace vks */

LvnResult vksImplCreateContext(LvnGraphicsContext* graphicsContext, bool enableValidationLayers)
{
	VulkanBackends* vkBackends = new VulkanBackends();
	s_VkBackends = vkBackends;
	LvnContext* lvnctx = lvn::getContext();
	vkBackends->enableValidationLayers = lvnctx->vulkanValidationLayers;

	graphicsContext->createRenderPass = vksImplCreateRenderPass;
	graphicsContext->createShaderFromSrc = vksImplCreateShaderFromFileSrc;
	graphicsContext->createShaderFromFileSrc = vksImplCreateShaderFromFileSrc;
	graphicsContext->createShaderFromFileBin = vksImplCreateShaderFromFileBin;
	graphicsContext->createDescriptorLayout = vksImplCreateDescriptorLayout;
	graphicsContext->createPipeline = vksImplCreatePipeline;
	graphicsContext->createFrameBuffer = vksImplCreateFrameBuffer;
	graphicsContext->createBuffer = vksImplCreateBuffer;
	graphicsContext->createUniformBuffer = vksImplCreateUniformBuffer;
	
	graphicsContext->destroyRenderPass = vksImplDestroyRenderPass;
	graphicsContext->destroyShader = vksImplDestroyShader;
	graphicsContext->destroyDescriptorLayout = vksImplDestroyDescriptorLayout;
	graphicsContext->destroyPipeline = vksImplDestroyPipeline;
	graphicsContext->destroyFrameBuffer = vksImplDestroyFrameBuffer;
	graphicsContext->destroyBuffer = vksImplDestroyBuffer;
	graphicsContext->destroyUniformBuffer = vksImplDestroyUniformBuffer;

	graphicsContext->renderCmdDraw = vksImplRenderCmdDraw;
	graphicsContext->renderCmdDrawIndexed = vksImplRenderCmdDrawIndexed;
	graphicsContext->renderCmdDrawInstanced = vksImplRenderCmdDrawInstanced;
	graphicsContext->renderCmdDrawIndexedInstanced = vksImplRenderCmdDrawIndexedInstanced;
	graphicsContext->renderCmdSetStencilReference = vksImplRenderCmdSetStencilReference;
	graphicsContext->renderCmdSetStencilMask = vksImplRenderCmdSetStencilMask;
	graphicsContext->renderBeginNextFrame = vksImplRenderBeginNextFrame;
	graphicsContext->renderDrawSubmit = vksImplRenderDrawSubmit;
	graphicsContext->renderBeginCommandRecording = vksImplRenderBeginCommandRecording;
	graphicsContext->renderEndCommandRecording = vksImplRenderEndCommandRecording;
	graphicsContext->renderCmdBeginRenderPass = vksImplRenderCmdBeginRenderPass;
	graphicsContext->renderCmdEndRenderPass = vksImplRenderCmdEndRenderPass;
	graphicsContext->renderCmdBindPipeline = vksImplRenderCmdBindPipeline;
	graphicsContext->renderCmdBindVertexBuffer = vksImplRenderCmdBindVertexBuffer;
	graphicsContext->renderCmdBindIndexBuffer = vksImplRenderCmdBindIndexBuffer;
	graphicsContext->renderCmdBindDescriptorLayout = vksImplRenderCmdBindDescriptorLayout;

	graphicsContext->setDefaultPipelineSpecification = vksImplSetDefaultPipelineSpecification;
	graphicsContext->getDefaultPipelineSpecification = vksImplGetDefaultPipelineSpecification;
	graphicsContext->updateUniformBufferData = vksImplUpdateUniformBufferData;

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

	vkDestroyCommandPool(vkBackends->device, vkBackends->commandPool, nullptr);

	for (uint32_t i = 0; i < vkBackends->windowSurfaceData.size(); i++)
	{
		// sync objects
		for (uint32_t j = 0; j < vkBackends->maxFramesInFlight; j++)
		{
			vkDestroySemaphore(vkBackends->device, vkBackends->windowSurfaceData[i].imageAvailableSemaphores[j], nullptr);
			vkDestroySemaphore(vkBackends->device, vkBackends->windowSurfaceData[i].renderFinishedSemaphores[j], nullptr);
			vkDestroyFence(vkBackends->device, vkBackends->windowSurfaceData[i].inFlightFences[j], nullptr);
		}

		lvn::memFree(vkBackends->windowSurfaceData[i].renderFinishedSemaphores);
		lvn::memFree(vkBackends->windowSurfaceData[i].imageAvailableSemaphores);
		lvn::memFree(vkBackends->windowSurfaceData[i].inFlightFences);

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

		if (vkBackends->windowSurfaceData[i].commandBuffers)
			lvn::memFree(vkBackends->windowSurfaceData[i].commandBuffers);

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

	// VmaAllocator
	vmaDestroyAllocator(vkBackends->vmaAllocator);

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

	VkPhysicalDeviceProperties physicalDeviceProperties{};
	vkGetPhysicalDeviceProperties(vkBackends->physicalDevice, &physicalDeviceProperties);
	LVN_CORE_TRACE("[vulkan] the physical device (GPU) will be used: \"%s\", driverVersion: (%u), apiVersion: (%u)", physicalDeviceProperties.deviceName, physicalDeviceProperties.driverVersion, physicalDeviceProperties.apiVersion);

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
	LVN_CORE_CALL_ASSERT(glfwCreateWindowSurface(vkBackends->instance, glfwWin, nullptr, &surfaceData.surface) == VK_SUCCESS, "vulkan - failed to create window surface!");
	vks::createLogicalDevice(vkBackends, surfaceData.surface); // create logical device once

	// create command buffer pool
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = vkBackends->deviceIndices.graphicsIndex;

	LVN_CORE_CALL_ASSERT(vkCreateCommandPool(vkBackends->device, &poolInfo, nullptr, &vkBackends->commandPool) == VK_SUCCESS, "vulkan - failed to create command pool!");

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
	vks::createCommandBuffers(vkBackends, &surfaceData);
	vks::createSyncObjects(vkBackends, &surfaceData);

	vkBackends->windowSurfaceData.push_back(surfaceData);
	renderBackends->pWindows[0]->apiData = &vkBackends->windowSurfaceData[0];

	if (renderBackends->windowCount > 1)
	{
		for (uint32_t i = 1; i < renderBackends->windowCount; i++) // create surface for each window if there is more than one window
		{
			VulkanWindowSurfaceData surfaceDataExt{};
			GLFWwindow* glfwWin = static_cast<GLFWwindow*>(renderBackends->pWindows[i]->nativeWindow);
			LVN_CORE_CALL_ASSERT(glfwCreateWindowSurface(vkBackends->instance, glfwWin, nullptr, &surfaceDataExt.surface) == VK_SUCCESS, "vulkan - failed to create window surface!");
			vks::createSwapChain(vkBackends, &surfaceDataExt, swapChainSupport, surfaceFormat, presentMode, extent);
			vks::createImageViews(vkBackends, &surfaceDataExt);
			vks::createRenderPass(vkBackends, &surfaceDataExt, surfaceFormat.format);
			vks::createFrameBuffers(vkBackends, &surfaceDataExt);
			vks::createCommandBuffers(vkBackends, &surfaceDataExt);
			vks::createSyncObjects(vkBackends, &surfaceDataExt);

			vkBackends->windowSurfaceData.push_back(surfaceDataExt);
			renderBackends->pWindows[i]->apiData = &vkBackends->windowSurfaceData.back();
		}
	}

	// create VmaAllocator
	VmaAllocatorCreateInfo allocatorInfo{};
	allocatorInfo.device = vkBackends->device;
	allocatorInfo.physicalDevice = vkBackends->physicalDevice;
	allocatorInfo.instance = vkBackends->instance;

	vmaCreateAllocator(&allocatorInfo, &vkBackends->vmaAllocator);


	return Lvn_Result_Success;
}

void vksImplRenderCmdClearColor(const float r, const float g, const float b, const float w)
{

}

void vksImplRenderCmdClear()
{

}

void vksImplRenderCmdDraw(LvnWindow* window, uint32_t vertexCount)
{
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
	vkCmdDraw(surfaceData->commandBuffers[surfaceData->currentFrame], vertexCount, 1, 0, 0);
}

void vksImplRenderCmdDrawIndexed(LvnWindow* window, uint32_t indexCount)
{
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
	vkCmdDrawIndexed(surfaceData->commandBuffers[surfaceData->currentFrame], indexCount, 1, 0, 0, 0);
}

void vksImplRenderCmdDrawInstanced(LvnWindow* window, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance)
{

}

void vksImplRenderCmdDrawIndexedInstanced(LvnWindow* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance)
{

}

void vksImplRenderCmdSetStencilReference(uint32_t reference)
{

}

void vksImplRenderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask)
{

}

void vksImplRenderBeginNextFrame(LvnWindow* window)
{
	GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);
	int width, height;
	glfwGetFramebufferSize(glfwWin, &width, &height);
	if (width == 0 || height == 0) { return; }

	VulkanBackends* vkBackends = s_VkBackends;
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);

	vkWaitForFences(vkBackends->device, 1, &surfaceData->inFlightFences[surfaceData->currentFrame], VK_TRUE, UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(vkBackends->device, surfaceData->swapChain, UINT64_MAX, surfaceData->imageAvailableSemaphores[surfaceData->currentFrame], VK_NULL_HANDLE, &surfaceData->imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		vks::recreateSwapChain(vkBackends, window);
		return;
	}
	LVN_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "vulkan - failed to acquire swap chain image!");

	vkResetFences(vkBackends->device, 1, &surfaceData->inFlightFences[surfaceData->currentFrame]);

}

void vksImplRenderDrawSubmit(LvnWindow* window)
{
	GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);
	int width, height;
	glfwGetFramebufferSize(glfwWin, &width, &height);
	if (width == 0 || height == 0) { return; }

	VulkanBackends* vkBackends = s_VkBackends;
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { surfaceData->imageAvailableSemaphores[surfaceData->currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &surfaceData->commandBuffers[surfaceData->currentFrame];

	VkSemaphore signalSemaphores[] = { surfaceData->renderFinishedSemaphores[surfaceData->currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	LVN_CORE_CALL_ASSERT(vkQueueSubmit(vkBackends->graphicsQueue, 1, &submitInfo, surfaceData->inFlightFences[surfaceData->currentFrame]) == VK_SUCCESS, "vulkan - failed to submit draw command buffer!");


	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { surfaceData->swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &surfaceData->imageIndex;
	presentInfo.pResults = nullptr; // Optional

	VkResult result = vkQueuePresentKHR(vkBackends->presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || surfaceData->frameBufferResized)
	{
		surfaceData->frameBufferResized = false;
		vks::recreateSwapChain(vkBackends, window);
	}
	else
	{
		LVN_CORE_ASSERT(result == VK_SUCCESS, "vulkan - failed to present swap chain image");
	}

	// advance to next frame in flight
	surfaceData->currentFrame = (surfaceData->currentFrame + 1) % vkBackends->maxFramesInFlight;
}

void vksImplRenderBeginCommandRecording(LvnWindow* window)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
	vkResetCommandBuffer(surfaceData->commandBuffers[surfaceData->currentFrame], 0);
	LVN_CORE_CALL_ASSERT(vkBeginCommandBuffer(surfaceData->commandBuffers[surfaceData->currentFrame], &beginInfo) == VK_SUCCESS, "vulkan - failed to begin recording command buffer!");
}

void vksImplRenderEndCommandRecording(LvnWindow* window)
{
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
	LVN_CORE_CALL_ASSERT(vkEndCommandBuffer(surfaceData->commandBuffers[surfaceData->currentFrame]) == VK_SUCCESS, "vulkan - failed to record command buffer!");
}

void vksImplRenderCmdBeginRenderPass(LvnWindow* window)
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

	vkCmdBeginRenderPass(surfaceData->commandBuffers[surfaceData->currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(surfaceData->swapChainExtent.width);
	viewport.height = static_cast<float>(surfaceData->swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(surfaceData->commandBuffers[surfaceData->currentFrame], 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = surfaceData->swapChainExtent;
	vkCmdSetScissor(surfaceData->commandBuffers[surfaceData->currentFrame], 0, 1, &scissor);

}

void vksImplRenderCmdEndRenderPass(LvnWindow* window)
{
	VulkanBackends* vkBackends = s_VkBackends;
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);

	vkCmdEndRenderPass(surfaceData->commandBuffers[surfaceData->currentFrame]);
}

void vksImplRenderCmdBindPipeline(LvnWindow* window, LvnPipeline* pipeline)
{
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);

	VkPipeline graphicsPipeline = static_cast<VkPipeline>(pipeline->nativePipeline);
	vkCmdBindPipeline(surfaceData->commandBuffers[surfaceData->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
}

void vksImplRenderCmdBindVertexBuffer(LvnWindow* window, LvnBuffer* buffer)
{
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
	VkBuffer vkBuffer = static_cast<VkBuffer>(buffer->vertexBuffer);
	VkDeviceSize offsets[] = {0};

	vkCmdBindVertexBuffers(surfaceData->commandBuffers[surfaceData->currentFrame], 0, 1, &vkBuffer, offsets);
}

void vksImplRenderCmdBindIndexBuffer(LvnWindow* window, LvnBuffer* buffer)
{
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
	VkBuffer vkBuffer = static_cast<VkBuffer>(buffer->indexBuffer);

	vkCmdBindIndexBuffer(surfaceData->commandBuffers[surfaceData->currentFrame], vkBuffer, buffer->indexOffset, VK_INDEX_TYPE_UINT32);
}

void vksImplRenderCmdBindDescriptorLayout(LvnWindow* window, LvnPipeline* pipeline, LvnDescriptorLayout* descriptorLayout)
{
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
	VkPipelineLayout pipelineLayout = static_cast<VkPipelineLayout>(pipeline->nativePipelineLayout);
	VkDescriptorSet descriptorSet = static_cast<VkDescriptorSet>(descriptorLayout->descriptorSets[surfaceData->currentFrame]);

	vkCmdBindDescriptorSets(surfaceData->commandBuffers[surfaceData->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
}

LvnResult vksImplCreateRenderPass(LvnRenderPass* renderPass, LvnRenderPassCreateInfo* createInfo)
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
	LVN_CORE_CALL_ASSERT(vkCreateRenderPass(vkBackends->device, &renderPassInfo, nullptr, &vkRenderPass) == VK_SUCCESS, "vulkan - failed to create render pass!");
	
	renderPass->nativeRenderPass = vkRenderPass;

	return Lvn_Result_Success;
}

LvnResult vksImplCreateShaderFromSrc(LvnShader* shader, LvnShaderCreateInfo* createInfo)
{
	return Lvn_Result_Failure; // TODO: add shader src function
}

LvnResult vksImplCreateShaderFromFileSrc(LvnShader* shader, LvnShaderCreateInfo* createInfo)
{
	return Lvn_Result_Failure; // TODO: add shader src function
}

LvnResult vksImplCreateShaderFromFileBin(LvnShader* shader, LvnShaderCreateInfo* createInfo)
{
	VulkanBackends* vkBackends = s_VkBackends;

	LvnVector<uint8_t> vertbin = lvn::getFileSrcBin(createInfo->vertexSrc);
	LvnVector<uint8_t> fragbin = lvn::getFileSrcBin(createInfo->fragmentSrc);

	VkShaderModule vertShaderModule = vks::createShaderModule(vkBackends, vertbin.data(), vertbin.size());
	VkShaderModule fragShaderModule = vks::createShaderModule(vkBackends, fragbin.data(), fragbin.size());

	shader->nativeVertexShaderModule = vertShaderModule;
	shader->nativeFragmentShaderModule = fragShaderModule;

	return Lvn_Result_Success;
}

LvnResult vksImplCreateDescriptorLayout(LvnDescriptorLayout* descriptorLayout, LvnDescriptorLayoutCreateInfo* createInfo)
{
	VulkanBackends* vkBackends = s_VkBackends;

	LvnVector<VkDescriptorSetLayoutBinding> layoutBindings(createInfo->descriptorBindingCount);

	for (uint32_t i = 0; i < createInfo->descriptorBindingCount; i++)
	{
		layoutBindings[i].binding = createInfo->pDescriptorBindings[i].binding;
		layoutBindings[i].descriptorType = vks::getDescriptorTypeEnum(createInfo->pDescriptorBindings[i].descriptorType);
		layoutBindings[i].descriptorCount = createInfo->pDescriptorBindings[i].descriptorCount;
		layoutBindings[i].stageFlags = vks::getShaderStageFlagEnum(createInfo->pDescriptorBindings[i].shaderStage);
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = createInfo->descriptorBindingCount;
	layoutInfo.pBindings = layoutBindings.data();

	VkDescriptorSetLayout vkDescriptorLayout;
	if (vkCreateDescriptorSetLayout(vkBackends->device, &layoutInfo, nullptr, &vkDescriptorLayout) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("[vulkan] failed to create descriptor set layout <VkDescriptorSetLayout> at (%p)", vkDescriptorLayout);
		return Lvn_Result_Failure;
	}

	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = vkBackends->maxFramesInFlight;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = vkBackends->maxFramesInFlight;

	VkDescriptorPool descriptorPool;

	if (vkCreateDescriptorPool(vkBackends->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("[vulkan] failed to create descriptor pool at (%p)", descriptorPool);
		return Lvn_Result_Failure;
	}

	LvnVector<VkDescriptorSetLayout> layouts(vkBackends->maxFramesInFlight, vkDescriptorLayout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = vkBackends->maxFramesInFlight;
	allocInfo.pSetLayouts = layouts.data();

	descriptorLayout->descriptorSets = (void**)lvn::memAlloc(vkBackends->maxFramesInFlight * sizeof(VkDescriptorSet));

	if (vkAllocateDescriptorSets(vkBackends->device, &allocInfo, (VkDescriptorSet*)descriptorLayout->descriptorSets) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("[vulkan] failed to allocate descriptor sets <VkDescriptorSet*> at (%p)", descriptorLayout->descriptorSets);
		return Lvn_Result_Failure;
	}

	descriptorLayout->descriptorLayout = vkDescriptorLayout;
	descriptorLayout->descriptorPool = descriptorPool;

	return Lvn_Result_Success;
}

LvnResult vksImplCreatePipeline(LvnPipeline* pipeline, LvnPipelineCreateInfo* createInfo)
{
	VulkanBackends* vkBackends = s_VkBackends;

	// shader modules
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = static_cast<VkShaderModule>(createInfo->shader->nativeVertexShaderModule);
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = static_cast<VkShaderModule>(createInfo->shader->nativeFragmentShaderModule);
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

	// vertex binding descriptions & attributes
	LvnVector<VkVertexInputBindingDescription> bindingDescriptions(createInfo->vertexBindingDescriptionCount);
	LvnVector<VkVertexInputAttributeDescription> vertexAttributes(createInfo->vertexAttributeCount);

	for (uint32_t i = 0; i < createInfo->vertexBindingDescriptionCount; i++)
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = createInfo->pVertexBindingDescriptions[i].binding;
		bindingDescription.stride = createInfo->pVertexBindingDescriptions[i].stride;
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		bindingDescriptions[i] = bindingDescription;
	}

	for (uint32_t i = 0; i < createInfo->vertexAttributeCount; i++)
	{
		if (createInfo->pVertexAttributes[i].type == Lvn_VertexDataType_None)
			LVN_CORE_WARN("createPipeline(LvnPipeline**, LvnPipelineCreateInfo*) | createInfo->pVertexAttributes[%d].type is \'Lvn_VertexDataType_None\'; vertex data type is set to None, vertex input attribute format will be undefined", i);

		VkVertexInputAttributeDescription attributeDescription{};
		attributeDescription.binding = createInfo->pVertexAttributes[i].binding;
		attributeDescription.location = createInfo->pVertexAttributes[i].layout;
		attributeDescription.format = vks::getVertexAttributeFormatEnum(createInfo->pVertexAttributes[i].type);
		attributeDescription.offset = createInfo->pVertexAttributes[i].offset;

		vertexAttributes[i] = attributeDescription;
	}

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	if (createInfo->pVertexBindingDescriptions && createInfo->vertexBindingDescriptionCount > 0)
	{
		vertexInputInfo.vertexBindingDescriptionCount = createInfo->vertexBindingDescriptionCount;
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
	}

	if (createInfo->pVertexAttributes && createInfo->vertexAttributeCount > 0)
	{
		vertexInputInfo.vertexAttributeDescriptionCount = createInfo->vertexAttributeCount;
		vertexInputInfo.pVertexAttributeDescriptions = vertexAttributes.data();
	}

	// descriptor layouts
	LvnVector<VkDescriptorSetLayout> descriptorLayouts(createInfo->descriptorLayoutCount);
	for (uint32_t i = 0; i < createInfo->descriptorLayoutCount; i++)
	{
		VkDescriptorSetLayout descriptorLayout = static_cast<VkDescriptorSetLayout>(createInfo->pDescriptorLayouts[i]->descriptorLayout);
		descriptorLayouts[i] = descriptorLayout;
	}

	// render pass
	VkRenderPass renderPass = createInfo->renderPass != nullptr ? static_cast<VkRenderPass>(createInfo->renderPass->nativeRenderPass) : static_cast<VulkanWindowSurfaceData*>(createInfo->window->apiData)->renderPass;

	// prepare pipeline create info
	VulkanPipelineCreateData pipelineCreateData{};
	pipelineCreateData.shaderStages = shaderStages;
	pipelineCreateData.shaderStageCount = ARRAY_LEN(shaderStages);
	pipelineCreateData.vertexInputInfo = vertexInputInfo;
	pipelineCreateData.renderPass = renderPass;
	pipelineCreateData.pipelineSpecification = createInfo->pipelineSpecification != nullptr ? createInfo->pipelineSpecification : &vkBackends->defaultPipelineSpecification;
	pipelineCreateData.pDescrptorSetLayouts = descriptorLayouts.data();
	pipelineCreateData.descriptorSetLayoutCount = createInfo->descriptorLayoutCount;

	// create pipeline
	VulkanPipeline vkPipeline = vks::createVulkanPipeline(vkBackends, &pipelineCreateData);

	pipeline->nativePipeline = vkPipeline.pipeline;
	pipeline->nativePipelineLayout = vkPipeline.pipelineLayout;

	return Lvn_Result_Success;
}

LvnResult vksImplCreateFrameBuffer(LvnFrameBuffer* frameBuffer, LvnFrameBufferCreateInfo* createInfo)
{
	return Lvn_Result_Success;
}

LvnResult vksImplCreateBuffer(LvnBuffer* buffer, LvnBufferCreateInfo* createInfo) // TODO: create buffers for static and dynamic data
{
	VulkanBackends* vkBackends = s_VkBackends;

	VkDeviceSize bufferSize = createInfo->vertexBufferSize + createInfo->indexBufferSize;

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = bufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkBuffer stagingBuffer;
	VmaAllocation stagingMemory;

	vks::createBuffer(vkBackends, &stagingBuffer, &stagingMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	void* data;
	vmaMapMemory(vkBackends->vmaAllocator, stagingMemory, &data);

	if (createInfo->pVertices)
	{
		memcpy(data, createInfo->pVertices, createInfo->vertexBufferSize);
	}
	if (createInfo->pIndices)
	{
		memcpy((char*)data + createInfo->vertexBufferSize, createInfo->pIndices, createInfo->indexBufferSize);
	}

	vmaUnmapMemory(vkBackends->vmaAllocator, stagingMemory);

	VkBuffer vkBuffer;
	VmaAllocation bufferMemory;
	VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

	vks::createBuffer(vkBackends, &vkBuffer, &bufferMemory, bufferSize, bufferUsage, VMA_MEMORY_USAGE_GPU_ONLY);

	vks::copyBuffer(vkBackends, stagingBuffer, vkBuffer, bufferSize, 0, 0);

	vkDestroyBuffer(vkBackends->device, stagingBuffer, nullptr);
	vmaFreeMemory(vkBackends->vmaAllocator, stagingMemory);

	buffer->vertexBuffer = vkBuffer;
	buffer->vertexBufferMemory = bufferMemory;
	buffer->indexBuffer = vkBuffer;
	buffer->indexBufferMemory = bufferMemory;
	buffer->indexOffset = createInfo->vertexBufferSize;


	return Lvn_Result_Success;
}

LvnResult vksImplCreateUniformBuffer(LvnUniformBuffer* uniformBuffer, LvnUniformBufferCreateInfo* createInfo)
{
	VulkanBackends* vkBackends = s_VkBackends;

	VkBuffer vkUniformBuffer;
	VmaAllocation uniformBufferMemory;
	vks::createBuffer(vkBackends, &vkUniformBuffer, &uniformBufferMemory, createInfo->size * vkBackends->maxFramesInFlight, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	uniformBuffer->uniformBuffer = vkUniformBuffer;
	uniformBuffer->uniformBufferMemory = uniformBufferMemory;
	uniformBuffer->uniformBufferMapped = (void**)lvn::memAlloc(vkBackends->maxFramesInFlight * sizeof(void*));

	VkDescriptorSet* descriptorSets = (VkDescriptorSet*)createInfo->descriptorLayout->descriptorSets;

	for (uint32_t i = 0; i < vkBackends->maxFramesInFlight; i++)
	{
		vmaMapMemory(vkBackends->vmaAllocator, uniformBufferMemory, &uniformBuffer->uniformBufferMapped[i]);

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = vkUniformBuffer;
		bufferInfo.offset = createInfo->size * i;
		bufferInfo.range = createInfo->size;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = createInfo->binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = vks::getUniformBufferTypeEnum(createInfo->type);
		descriptorWrite.descriptorCount = 1; // TODO: change descriptor count
		descriptorWrite.pBufferInfo = &bufferInfo;
		vkUpdateDescriptorSets(vkBackends->device, 1, &descriptorWrite, 0, nullptr);
	}

	return Lvn_Result_Success;
}

void vksImplDestroyRenderPass(LvnRenderPass* renderPass)
{
	VulkanBackends* vkBackends = s_VkBackends;

	VkRenderPass vkRenderPass = static_cast<VkRenderPass>(renderPass->nativeRenderPass);
	vkDestroyRenderPass(vkBackends->device, vkRenderPass, nullptr);
}

void vksImplDestroyShader(LvnShader* shader)
{
	VulkanBackends* vkBackends = s_VkBackends;

	VkShaderModule vertShaderModule = static_cast<VkShaderModule>(shader->nativeVertexShaderModule);
	VkShaderModule fragShaderModule = static_cast<VkShaderModule>(shader->nativeFragmentShaderModule);
	vkDestroyShaderModule(vkBackends->device, fragShaderModule, nullptr);
	vkDestroyShaderModule(vkBackends->device, vertShaderModule, nullptr);
}

void vksImplDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout)
{
	VulkanBackends* vkBackends = s_VkBackends;

	VkDescriptorSetLayout vkDescriptorLayout = static_cast<VkDescriptorSetLayout>(descriptorLayout->descriptorLayout);
	VkDescriptorPool descriptorPool = static_cast<VkDescriptorPool>(descriptorLayout->descriptorPool);

	vkDestroyDescriptorPool(vkBackends->device, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(vkBackends->device, vkDescriptorLayout, nullptr);

	if (descriptorLayout->descriptorSets)
		lvn::memFree(descriptorLayout->descriptorSets);
}

void vksImplDestroyPipeline(LvnPipeline* pipeline)
{
	VulkanBackends* vkBackends = s_VkBackends;
	vkDeviceWaitIdle(vkBackends->device);

	VkPipeline vkPipeline = static_cast<VkPipeline>(pipeline->nativePipeline);
	VkPipelineLayout vkPipelineLayout = static_cast<VkPipelineLayout>(pipeline->nativePipelineLayout);

	vkDestroyPipeline(vkBackends->device, vkPipeline, nullptr);
    vkDestroyPipelineLayout(vkBackends->device, vkPipelineLayout, nullptr);
}

void vksImplDestroyFrameBuffer(LvnFrameBuffer* frameBuffer)
{

}

void vksImplDestroyBuffer(LvnBuffer* buffer)
{
	VulkanBackends* vkBackends = s_VkBackends;
	vkDeviceWaitIdle(vkBackends->device);

	// TODO: delete logic for dynamic buffers
	VkBuffer vkBuffer = static_cast<VkBuffer>(buffer->vertexBuffer);
	VmaAllocation bufferMemory = static_cast<VmaAllocation>(buffer->vertexBufferMemory);

	vkDestroyBuffer(vkBackends->device, vkBuffer, nullptr);
	vmaFreeMemory(vkBackends->vmaAllocator, bufferMemory);
}

void vksImplDestroyUniformBuffer(LvnUniformBuffer* uniformBuffer)
{
	VulkanBackends* vkBackends = s_VkBackends;
	vkDeviceWaitIdle(vkBackends->device);

	VkBuffer vkUniformBuffer = static_cast<VkBuffer>(uniformBuffer->uniformBuffer);
	VmaAllocation uniformBufferMemory = static_cast<VmaAllocation>(uniformBuffer->uniformBufferMemory);
	vkDestroyBuffer(vkBackends->device, vkUniformBuffer, nullptr);
	vmaUnmapMemory(vkBackends->vmaAllocator, uniformBufferMemory);
	vmaFreeMemory(vkBackends->vmaAllocator, uniformBufferMemory);

	if (uniformBuffer->uniformBufferMapped)
		lvn::memFree(uniformBuffer->uniformBufferMapped);
}

void vksImplSetDefaultPipelineSpecification(LvnPipelineSpecification* pipelineSpecification)
{
	s_VkBackends->defaultPipelineSpecification = *pipelineSpecification;
}

LvnPipelineSpecification vksImplGetDefaultPipelineSpecification()
{
	return s_VkBackends->defaultPipelineSpecification;
}

void vksImplUpdateUniformBufferData(LvnWindow* window, LvnUniformBuffer* uniformBuffer, void* data, uint64_t size)
{
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
	memcpy(uniformBuffer->uniformBufferMapped[surfaceData->currentFrame], data, size);
}


} /* namespace lvn */
