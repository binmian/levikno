#include "lvn_vulkan.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <glslang/Include/glslang_c_interface.h>

#ifdef LVN_CONFIG_DEBUG
	#define VMA_ASSERT(expr) (static_cast<bool>(expr) ? void(0) : LVN_CORE_ERROR("[VMA]: " #expr))
#endif

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
#include "lvn_vulkanBackends.h"

#ifdef LVN_CONFIG_DEBUG
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
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
};

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))



namespace lvn
{

static VulkanBackends* s_VkBackends = nullptr;

namespace vks
{
	static LvnResult                            createVulkanInstace(VulkanBackends* vkBackends, bool enableValidationLayers);
	static VKAPI_ATTR VkBool32 VKAPI_CALL       debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	static bool                                 checkValidationLayerSupport();
	static std::vector<const char*>             getRequiredExtensions(VulkanBackends* vkBackends);
	static VkResult                             createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	static void                                 destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	static void                                 fillVulkanDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo);
	static void                                 setupDebugMessenger(VulkanBackends* vkBackends);
	static LvnPhysicalDeviceType                getPhysicalDeviceTypeEnum(VkPhysicalDeviceType type);
	static bool                                 checkDeviceExtensionSupport(VkPhysicalDevice device);
	static VulkanSwapChainSupportDetails        querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice device);
	static VulkanQueueFamilyIndices             findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	static VkFormat                             findSupportedFormat(VkPhysicalDevice physicalDevice, const VkFormat* candidates, uint32_t count, VkImageTiling tiling, VkFormatFeatureFlags features);
	static VkFormat                             findDepthFormat(VkPhysicalDevice physicalDevice);
	static bool                                 hasStencilComponent(VkFormat format);
	static LvnResult                            createLogicalDevice(VulkanBackends* vkBackends, VkSurfaceKHR surface);
	static void                                 createRenderPass(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData, VkFormat format);
	static VkSurfaceFormatKHR                   chooseSwapSurfaceFormat(VulkanBackends* vkBackends, const VkSurfaceFormatKHR* pAvailableFormats, uint32_t count);
	static VkPresentModeKHR                     chooseSwapPresentMode(const VkPresentModeKHR* pAvailablePresentModes, uint32_t count, bool vSync);
	static VkExtent2D                           chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR* capabilities);
	static void                                 createSwapChain(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData, VulkanSwapChainSupportDetails swapChainSupport, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode, VkExtent2D extent);
	static VkImageView                          createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	static void                                 createImageViews(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
	static void                                 createDepthResources(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
	static void                                 createFrameBuffers(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
	static void                                 createCommandBuffers(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
	static void                                 createSyncObjects(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
	static LvnResult                            createOffscreenFrameBuffer(VulkanBackends* vkBackends, LvnFrameBuffer* frameBuffer);
	static void                                 cleanSwapChain(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
	static void                                 recreateSwapChain(VulkanBackends* vkBackends, LvnWindow* window);
	static VkPrimitiveTopology                  getVulkanTopologyTypeEnum(LvnTopologyType topologyType);
	static VkCullModeFlags                      getVulkanCullModeFlagEnum(LvnCullFaceMode cullFaceMode);
	static VkFrontFace                          getVulkanCullFrontFaceEnum(LvnCullFrontFace cullFrontFace);
	static VkFormat                             getVulkanColorFormatEnum(LvnColorImageFormat format);
	static VkFormat                             getVulkanDepthFormatEnum(LvnDepthImageFormat format);
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
	static VkBufferUsageFlags                   getUniformBufferTypeEnum(LvnBufferType type);
	static VkShaderStageFlags                   getShaderStageFlagEnum(LvnShaderStage stage);
	static VkFilter                             getTextureFilterEnum(LvnTextureFilter filter);
	static VkSamplerAddressMode                 getTextureWrapModeEnum(LvnTextureMode mode);
	static VulkanPipeline                       createVulkanPipeline(VulkanBackends* vkBackends, VulkanPipelineCreateData* createData);
	static VkShaderModule                       createShaderModule(VulkanBackends* vkBackends, const uint8_t* code, uint32_t size);
	static LvnResult                            compileShaderToSPIRV(glslang_stage_t stage, const char* shaderSource, std::vector<uint8_t>& bin);
	static LvnResult                            createBuffer(VulkanBackends* vkBackends, VkBuffer* buffer, VmaAllocation* bufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memUsage);
	static void                                 copyBuffer(VulkanBackends* vkBackends, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset);
	static LvnResult                            createImage(VulkanBackends* vkBackends, VkImage* image, VmaAllocation* imageMemory, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkSampleCountFlagBits samples, VmaMemoryUsage memUsage);
	static void                                 transitionImageLayout(VulkanBackends* vkBackends, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount);
	static void                                 copyBufferToImage(VulkanBackends* vkBackends, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

	static LvnResult createVulkanInstace(VulkanBackends* vkBackends, bool enableValidationLayers)
	{
		// Create Vulkan Instance
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "levikno";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "levikno";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_2;


		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		std::vector<const char*> extensions = vks::getRequiredExtensions(vkBackends);
		createInfo.enabledExtensionCount = extensions.size();
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = ARRAY_LEN(s_ValidationLayers);
			createInfo.ppEnabledLayerNames = s_ValidationLayers;

			vks::fillVulkanDebugMessengerCreateInfo(&debugCreateInfo);
			createInfo.pNext = &debugCreateInfo;
		}

		// Create Instance
		if (vkCreateInstance(&createInfo, nullptr, &vkBackends->instance) != VK_SUCCESS)
		{
			LVN_CORE_ERROR("[vulkan] failed to create instance at (%p)", vkBackends->instance);
			return Lvn_Result_Failure;
		}

		return Lvn_Result_Success;
	}

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

		std::vector<VkLayerProperties> availableLayers(layerCount);
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

	static std::vector<const char*> getRequiredExtensions(VulkanBackends* vkBackends)
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

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

	static void fillVulkanDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo)
	{
		createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo->messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo->messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo->pfnUserCallback = debugCallback;
	}

	static void setupDebugMessenger(VulkanBackends* vkBackends)
	{
		if (!vkBackends->enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		fillVulkanDebugMessengerCreateInfo(&createInfo);

		LVN_CORE_CALL_ASSERT(createDebugUtilsMessengerEXT(vkBackends->instance, &createInfo, nullptr, &vkBackends->debugMessenger) == VK_SUCCESS, "[vulkan] failed to set up debug messenger!");
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

		LVN_CORE_WARN("[vulkan] No physical device matches type");
		return Lvn_PhysicalDeviceType_Unknown;
	}

	static VulkanQueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		VulkanQueueFamilyIndices indices{};

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
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

	static VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const VkFormat* candidates, uint32_t count, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (uint32_t i = 0; i < count; i++)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, candidates[i], &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			{
				return candidates[i];
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			{
				return candidates[i];
			}
		}

		LVN_CORE_ASSERT(false, "[vulkan] failed to find supported format type for physical device!");
		return VK_FORMAT_UNDEFINED;
	}

	static VkFormat findDepthFormat(VkPhysicalDevice physicalDevice)
	{
		VkFormat formats[] = { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT };
		return findSupportedFormat(physicalDevice, formats, ARRAY_LEN(formats), VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	static bool hasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	static LvnResult createLogicalDevice(VulkanBackends* vkBackends, VkSurfaceKHR surface)
	{
		// Find queue families
		VulkanQueueFamilyIndices queueIndices = vks::findQueueFamilies(vkBackends->physicalDevice, surface);
		vkBackends->deviceIndices = queueIndices;

		// Check queue families
		if (!queueIndices.has_graphics || !queueIndices.has_present)
		{
			LVN_CORE_ERROR("[vulkan] failed to create logical device, physical device does not support queue families needed");
			return Lvn_Result_Failure;
		}

		// Check device extension support
		if (!vks::checkDeviceExtensionSupport(vkBackends->physicalDevice))
		{
			LVN_CORE_ERROR("[vulkan] failed to create logical device, physical device does not support required extensions");
			return Lvn_Result_Failure;
		}

		float queuePriority = 1.0f;
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
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

		if (vkBackends->deviceSupportedFeatures.samplerAnisotropy)
			deviceFeatures.samplerAnisotropy = VK_TRUE;

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

		if (vkCreateDevice(vkBackends->physicalDevice, &createInfo, nullptr, &vkBackends->device) != VK_SUCCESS)
		{
			LVN_CORE_ERROR("[vulkan] failed to create logical device <VkDevice> (%p)", vkBackends->device);
			return Lvn_Result_Failure;
		}

		// Get device queues
		vkGetDeviceQueue(vkBackends->device, queueIndices.presentIndex, 0, &vkBackends->presentQueue);
		vkGetDeviceQueue(vkBackends->device, queueIndices.graphicsIndex, 0, &vkBackends->graphicsQueue);

		return Lvn_Result_Success;
	}

	static void createRenderPass(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData, VkFormat format)
	{
		// color attachment
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

		// depth attachment
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = vks::findDepthFormat(vkBackends->physicalDevice);
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcAccessMask = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


		VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = ARRAY_LEN(attachments);
		renderPassInfo.pAttachments = attachments;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		LVN_CORE_CALL_ASSERT(vkCreateRenderPass(vkBackends->device, &renderPassInfo, nullptr, &surfaceData->renderPass) == VK_SUCCESS, "[vulkan] failed to create render pass!");
	}

	static bool checkDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::vector<const char*> requiredExtensions(s_DeviceExtensions, s_DeviceExtensions + ARRAY_LEN(s_DeviceExtensions));

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

	static VkSurfaceFormatKHR chooseSwapSurfaceFormat(VulkanBackends* vkBackends, const VkSurfaceFormatKHR* pAvailableFormats, uint32_t count)
	{
		for (uint32_t i = 0; i < count; i++)
		{
			if (pAvailableFormats[i].format == vkBackends->frameBufferColorFormat && pAvailableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return pAvailableFormats[i];
			}
		}

		return pAvailableFormats[0];
	}

	static VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR* pAvailablePresentModes, uint32_t count, bool vSync)
	{
		VkPresentModeKHR presentMode = vSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR;

		for (uint32_t i = 0; i < count; i++)
		{
			if (pAvailablePresentModes[i] == presentMode)
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
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		LVN_CORE_CALL_ASSERT(vkCreateSwapchainKHR(vkBackends->device, &createInfo, nullptr, &surfaceData->swapChain) == VK_SUCCESS, "[vulkan] failed to create swap chain!");

		vkGetSwapchainImagesKHR(vkBackends->device, surfaceData->swapChain, &imageCount, nullptr);
		surfaceData->swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(vkBackends->device, surfaceData->swapChain, &imageCount, surfaceData->swapChainImages.data());

		surfaceData->swapChainImageFormat = surfaceFormat.format;
		surfaceData->swapChainExtent = extent;
	}

	static VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		LVN_CORE_CALL_ASSERT(vkCreateImageView(device, &viewInfo, nullptr, &imageView) == VK_SUCCESS, "[vulkan] failed to create image view!");

		return imageView;
	}

	static void createImageViews(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData)
	{
		surfaceData->swapChainImageViews.resize(surfaceData->swapChainImages.size());

		for (size_t i = 0; i < surfaceData->swapChainImageViews.size(); i++)
		{
			surfaceData->swapChainImageViews[i] = vks::createImageView(vkBackends->device, surfaceData->swapChainImages[i], surfaceData->swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	static void createDepthResources(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData)
	{
		VkFormat depthFormat = vks::findDepthFormat(vkBackends->physicalDevice);

		vks::createImage(vkBackends, &surfaceData->depthImage, &surfaceData->depthImageMemory, surfaceData->swapChainExtent.width, surfaceData->swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_SAMPLE_COUNT_1_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
		surfaceData->depthImageView = vks::createImageView(vkBackends->device, surfaceData->depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

		vks::transitionImageLayout(vkBackends, surfaceData->depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
	}

	static void createFrameBuffers(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData)
	{
		surfaceData->frameBuffers.resize(surfaceData->swapChainImages.size());

		for (size_t i = 0; i < surfaceData->frameBuffers.size(); i++)
		{
			VkImageView attachments[] =
			{
				surfaceData->swapChainImageViews[i],
				surfaceData->depthImageView
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = surfaceData->renderPass;
			framebufferInfo.attachmentCount = ARRAY_LEN(attachments);
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = surfaceData->swapChainExtent.width;
			framebufferInfo.height = surfaceData->swapChainExtent.height;
			framebufferInfo.layers = 1;

			LVN_CORE_CALL_ASSERT(vkCreateFramebuffer(vkBackends->device, &framebufferInfo, nullptr, &surfaceData->frameBuffers[i]) == VK_SUCCESS, "[vulkan] failed to create framebuffer!");
		}
	}

	static void createCommandBuffers(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = vkBackends->commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = vkBackends->maxFramesInFlight;

		surfaceData->commandBuffers.resize(vkBackends->maxFramesInFlight);
		LVN_CORE_CALL_ASSERT(vkAllocateCommandBuffers(vkBackends->device, &allocInfo, surfaceData->commandBuffers.data()) == VK_SUCCESS, "[vulkan] failed to allocate command buffers!");
	}

	static void createSyncObjects(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData)
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		surfaceData->imageAvailableSemaphores.resize(vkBackends->maxFramesInFlight);
		surfaceData->renderFinishedSemaphores.resize(vkBackends->maxFramesInFlight);
		surfaceData->inFlightFences.resize(vkBackends->maxFramesInFlight);

		for (uint32_t i = 0; i < vkBackends->maxFramesInFlight; i++)
		{
			LVN_CORE_CALL_ASSERT(vkCreateSemaphore(vkBackends->device, &semaphoreInfo, nullptr, &surfaceData->imageAvailableSemaphores[i]) == VK_SUCCESS, "[vulkan] failed to create semaphore");
			LVN_CORE_CALL_ASSERT(vkCreateSemaphore(vkBackends->device, &semaphoreInfo, nullptr, &surfaceData->renderFinishedSemaphores[i]) == VK_SUCCESS, "[vulkan] failed to create semaphore");
			LVN_CORE_CALL_ASSERT(vkCreateFence(vkBackends->device, &fenceInfo, nullptr, &surfaceData->inFlightFences[i]) == VK_SUCCESS, "[vulkan] failed to create fence");
		}
	}

	static LvnResult createOffscreenFrameBuffer(VulkanBackends* vkBackends, LvnFrameBuffer* frameBuffer)
	{
		VulkanFrameBufferData* frameBufferData = static_cast<VulkanFrameBufferData*>(frameBuffer->frameBufferData);
		frameBufferData->frameBufferImages.resize(frameBufferData->totalAttachmentCount);

		std::vector<VkImageView> attachments(frameBufferData->totalAttachmentCount);

		for (uint32_t i = 0; i < frameBufferData->colorAttachments.size(); i++)
		{
			VkFormat colorFormat = vks::getVulkanColorFormatEnum(frameBufferData->colorAttachments[i].format);

			if (vks::createImage(vkBackends, &frameBufferData->colorImages[i], &frameBufferData->colorImageMemory[i], frameBufferData->width, frameBufferData->height, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, frameBufferData->sampleCount, VMA_MEMORY_USAGE_GPU_ONLY) != Lvn_Result_Success)
			{
				LVN_CORE_ERROR("[vulkan] failed to create image <VkImage> when creating framebuffer at (%p)", frameBuffer);
				return Lvn_Result_Failure;
			}

			VkImageViewCreateInfo colorImageView{};
			colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
			colorImageView.format = colorFormat;
			colorImageView.flags = 0;
			colorImageView.subresourceRange = {};
			colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			colorImageView.subresourceRange.baseMipLevel = 0;
			colorImageView.subresourceRange.levelCount = 1;
			colorImageView.subresourceRange.baseArrayLayer = 0;
			colorImageView.subresourceRange.layerCount = 1;
			colorImageView.image = frameBufferData->colorImages[i];

			if (vkCreateImageView(vkBackends->device, &colorImageView, nullptr, &frameBufferData->colorImageViews[i]) != VK_SUCCESS)
			{
				LVN_CORE_ERROR("[vulkan] failed to create image view <VkImageView> when creating framebuffer at (%p)", frameBuffer);
				return Lvn_Result_Failure;
			}

			attachments[frameBufferData->colorAttachments[i].index] = frameBufferData->colorImageViews[i];

			if (!frameBufferData->multisampling)
			{
				LvnTexture textureImage{};
				textureImage.image = frameBufferData->colorImageViews[i];
				textureImage.imageView = frameBufferData->colorImageViews[i];
				textureImage.imageMemory = frameBufferData->colorImageMemory[i];
				textureImage.sampler = frameBufferData->sampler;
				frameBufferData->frameBufferImages[frameBufferData->colorAttachments[i].index] = textureImage;
			}
		}

		// depth stencil attachment
		if (frameBufferData->hasDepth)
		{
			VkFormat depthFormat = vks::getVulkanDepthFormatEnum(frameBufferData->depthAttachment.format);

			if (vks::createImage(vkBackends, &frameBufferData->depthImage, &frameBufferData->depthImageMemory, frameBufferData->width, frameBufferData->height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, frameBufferData->sampleCount, VMA_MEMORY_USAGE_GPU_ONLY) != Lvn_Result_Success)
			{
				LVN_CORE_ERROR("[vulkan] failed to create image <VkImage> when creating framebuffer at (%p)", frameBuffer);
				return Lvn_Result_Failure;
			}

			VkImageViewCreateInfo depthStencilView{};
			depthStencilView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
			depthStencilView.format = depthFormat;
			depthStencilView.flags = 0;
			depthStencilView.subresourceRange = {};
			depthStencilView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			if (vks::hasStencilComponent(depthFormat))
				depthStencilView.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

			depthStencilView.subresourceRange.baseMipLevel = 0;
			depthStencilView.subresourceRange.levelCount = 1;
			depthStencilView.subresourceRange.baseArrayLayer = 0;
			depthStencilView.subresourceRange.layerCount = 1;

			depthStencilView.image = frameBufferData->depthImage;
			if (vkCreateImageView(vkBackends->device, &depthStencilView, nullptr, &frameBufferData->depthImageView) != VK_SUCCESS)
			{
				LVN_CORE_ERROR("[vulkan] failed to create image view <VkImageView> when creating frambuffer at (%p)", frameBuffer);
				return Lvn_Result_Failure;
			}

			attachments[frameBufferData->depthAttachment.index] = frameBufferData->depthImageView;

			LvnTexture textureImage{};
			textureImage.image = frameBufferData->depthImage;
			textureImage.imageView = frameBufferData->depthImageView;
			textureImage.imageMemory = frameBufferData->depthImageMemory;
			textureImage.sampler = frameBufferData->sampler;
			frameBufferData->frameBufferImages[frameBufferData->depthAttachment.index] = textureImage;
		}

		// multisampling
		if (frameBufferData->multisampling)
		{
			uint32_t colorAttachmentCount = frameBufferData->colorAttachments.size();
			frameBufferData->msaaColorImages.resize(colorAttachmentCount);
			frameBufferData->msaaColorImageViews.resize(colorAttachmentCount);
			frameBufferData->msaaColorImageMemory.resize(colorAttachmentCount);

			for (uint32_t i = 0; i < colorAttachmentCount; i++)
			{
				VkFormat colorFormat = vks::getVulkanColorFormatEnum(frameBufferData->colorAttachments[i].format);

				if (vks::createImage(vkBackends, &frameBufferData->msaaColorImages[i], &frameBufferData->msaaColorImageMemory[i], frameBufferData->width, frameBufferData->height, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT, VMA_MEMORY_USAGE_GPU_ONLY) != Lvn_Result_Success)
				{
					LVN_CORE_ERROR("[vulkan] failed to create image <VkImage> when creating framebuffer at (%p)", frameBuffer);
					return Lvn_Result_Failure;
				}

				VkImageViewCreateInfo colorImageView{};
				colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
				colorImageView.format = colorFormat;
				colorImageView.flags = 0;
				colorImageView.subresourceRange = {};
				colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				colorImageView.subresourceRange.baseMipLevel = 0;
				colorImageView.subresourceRange.levelCount = 1;
				colorImageView.subresourceRange.baseArrayLayer = 0;
				colorImageView.subresourceRange.layerCount = 1;
				colorImageView.image = frameBufferData->msaaColorImages[i];

				if (vkCreateImageView(vkBackends->device, &colorImageView, nullptr, &frameBufferData->msaaColorImageViews[i]) != VK_SUCCESS)
				{
					LVN_CORE_ERROR("[vulkan] failed to create image view <VkImageView> when creating framebuffer at (%p)", frameBuffer);
					return Lvn_Result_Failure;
				}

				// msaa images should be in the same order to the attachments in render pass
				attachments.push_back(frameBufferData->msaaColorImageViews[i]);

				LvnTexture textureImage{};
				textureImage.image = frameBufferData->msaaColorImages[i];
				textureImage.imageView = frameBufferData->msaaColorImageViews[i];
				textureImage.imageMemory = frameBufferData->msaaColorImageMemory[i];
				textureImage.sampler = frameBufferData->sampler;
				frameBufferData->frameBufferImages[frameBufferData->colorAttachments[i].index] = textureImage;
			}
		}

		VkFramebufferCreateInfo fbufCreateInfo{};
		fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fbufCreateInfo.renderPass = frameBufferData->renderPass;
		fbufCreateInfo.attachmentCount = attachments.size();
		fbufCreateInfo.pAttachments = attachments.data();
		fbufCreateInfo.width = frameBufferData->width;
		fbufCreateInfo.height = frameBufferData->height;
		fbufCreateInfo.layers = 1;

		if (vkCreateFramebuffer(vkBackends->device, &fbufCreateInfo, nullptr, &frameBufferData->framebuffer) != VK_SUCCESS)
		{
			LVN_CORE_ERROR("[vulkan] failed to create offscreen framebuffer <VkFrameBuffer> when creating framebuffer object at (%p)", frameBuffer);
			return Lvn_Result_Failure;
		}

		return Lvn_Result_Success;
	}

	static void cleanSwapChain(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData)
	{
		// swap chain images
		for (uint32_t i = 0; i < surfaceData->swapChainImageViews.size(); i++)
		{
			vkDestroyImageView(vkBackends->device, surfaceData->swapChainImageViews[i], nullptr);
		}

		vkDestroyImageView(vkBackends->device, surfaceData->depthImageView, nullptr);
		vkDestroyImage(vkBackends->device, surfaceData->depthImage, nullptr);
		vmaFreeMemory(vkBackends->vmaAllocator, surfaceData->depthImageMemory);

		// frame buffers
		for (uint32_t i = 0; i < surfaceData->frameBuffers.size(); i++)
		{
			vkDestroyFramebuffer(vkBackends->device, surfaceData->frameBuffers[i], nullptr);
		}

		// swap chain
		vkDestroySwapchainKHR(vkBackends->device, surfaceData->swapChain, nullptr);
	}

	static void recreateSwapChain(VulkanBackends* vkBackends, LvnWindow* window)
	{
		vkDeviceWaitIdle(vkBackends->device);

		VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);
		bool vSync = window->data.vSync;

		vks::cleanSwapChain(vkBackends, surfaceData);

		VulkanSwapChainSupportDetails swapChainSupport = vks::querySwapChainSupport(surfaceData->surface, vkBackends->physicalDevice);
		LVN_CORE_ASSERT(!swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty(), "[vulkan] physical device does not have swap chain support formats or present modes!");

		VkSurfaceFormatKHR surfaceFormat = vks::chooseSwapSurfaceFormat(vkBackends, swapChainSupport.formats.data(), swapChainSupport.formats.size());
		VkPresentModeKHR presentMode = vks::chooseSwapPresentMode(swapChainSupport.presentModes.data(), swapChainSupport.presentModes.size(), vSync);
		VkExtent2D extent = vks::chooseSwapExtent(glfwWin, &swapChainSupport.capabilities);

		vks::createSwapChain(vkBackends, surfaceData, swapChainSupport, surfaceFormat, presentMode, extent);
		vks::createImageViews(vkBackends, surfaceData);
		vks::createDepthResources(vkBackends, surfaceData);
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

	static VkFormat	getVulkanColorFormatEnum(LvnColorImageFormat format)
	{
		switch (format)
		{
			case Lvn_ColorImageFormat_None: { return VK_FORMAT_UNDEFINED; }
			case Lvn_ColorImageFormat_RGB: { return VK_FORMAT_R8G8B8_UNORM; }
			case Lvn_ColorImageFormat_RGBA: { return VK_FORMAT_R8G8B8A8_UNORM; }
			case Lvn_ColorImageFormat_RGBA8: { return VK_FORMAT_R8G8B8A8_UNORM; }
			case Lvn_ColorImageFormat_RGBA16F: { return VK_FORMAT_R16G16B16A16_SFLOAT; }
			case Lvn_ColorImageFormat_RGBA32F: { return VK_FORMAT_R32G32B32A32_SFLOAT; }
			case Lvn_ColorImageFormat_SRGB: { return VK_FORMAT_R8G8B8_SRGB; }
			case Lvn_ColorImageFormat_SRGBA: { return VK_FORMAT_R8G8B8A8_SRGB; }
			case Lvn_ColorImageFormat_SRGBA8: { return VK_FORMAT_R8G8B8A8_SRGB; }
			case Lvn_ColorImageFormat_SRGBA16F: { return VK_FORMAT_R16G16B16A16_SFLOAT; }
			case Lvn_ColorImageFormat_SRGBA32F: { return VK_FORMAT_R32G32B32A32_SFLOAT; }
			case Lvn_ColorImageFormat_RedInt: { return VK_FORMAT_R8_SINT; }

			default:
			{
				LVN_CORE_WARN("unknown image format enum (%d), setting image format to undefined", format);
				return VK_FORMAT_UNDEFINED;
			}
		}
	}

	static VkFormat getVulkanDepthFormatEnum(LvnDepthImageFormat format)
	{
		switch (format)
		{
			case Lvn_DepthImageFormat_Depth16: { return VK_FORMAT_D16_UNORM; }
			case Lvn_DepthImageFormat_Depth32: { return VK_FORMAT_D32_SFLOAT; }
			case Lvn_DepthImageFormat_Depth24Stencil8: { return VK_FORMAT_D24_UNORM_S8_UINT; }
			case Lvn_DepthImageFormat_Depth32Stencil8: { return VK_FORMAT_D32_SFLOAT_S8_UINT; }

			default:
			{
				LVN_CORE_WARN("unknown image format enum (%u), image format must be a depth component format", format);
				return VK_FORMAT_UNDEFINED;
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
			case Lvn_ColorBlendOp_Add: { return VK_BLEND_OP_ADD; }
			case Lvn_ColorBlendOp_Subtract: { return VK_BLEND_OP_SUBTRACT; }
			case Lvn_ColorBlendOp_ReverseSubtract: { return VK_BLEND_OP_REVERSE_SUBTRACT; }
			case Lvn_ColorBlendOp_Min: { return VK_BLEND_OP_MIN; }
			case Lvn_ColorBlendOp_Max: { return VK_BLEND_OP_MAX; }

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
			case Lvn_CompareOp_Never: { return VK_COMPARE_OP_NEVER; }
			case Lvn_CompareOp_Less: { return VK_COMPARE_OP_LESS; }
			case Lvn_CompareOp_Equal: { return VK_COMPARE_OP_EQUAL; }
			case Lvn_CompareOp_LessOrEqual: { return VK_COMPARE_OP_LESS_OR_EQUAL; }
			case Lvn_CompareOp_Greater: { return VK_COMPARE_OP_GREATER; }
			case Lvn_CompareOp_NotEqual: { return VK_COMPARE_OP_NOT_EQUAL; }
			case Lvn_CompareOp_GreaterOrEqual: { return VK_COMPARE_OP_GREATER_OR_EQUAL; }
			case Lvn_CompareOp_Always: { return VK_COMPARE_OP_ALWAYS; }
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
			case Lvn_StencilOp_Keep: { return VK_STENCIL_OP_KEEP; }
			case Lvn_StencilOp_Zero: { return VK_STENCIL_OP_ZERO; }
			case Lvn_StencilOp_Replace: { return VK_STENCIL_OP_REPLACE; }
			case Lvn_StencilOp_IncrementAndClamp: { return VK_STENCIL_OP_INCREMENT_AND_CLAMP; }
			case Lvn_StencilOp_DecrementAndClamp: { return VK_STENCIL_OP_DECREMENT_AND_CLAMP; }
			case Lvn_StencilOp_Invert: { return VK_STENCIL_OP_INVERT; }
			case Lvn_StencilOp_IncrementAndWrap: { return VK_STENCIL_OP_INCREMENT_AND_WRAP; }
			case Lvn_StencilOp_DecrementAndWrap: { return VK_STENCIL_OP_DECREMENT_AND_WRAP; }
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
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
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
			case Lvn_DescriptorType_ImageSampler: { return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; }
			case Lvn_DescriptorType_ImageSamplerBindless: { return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; }
			case Lvn_DescriptorType_UniformBuffer: { return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; }
			case Lvn_DescriptorType_StorageBuffer: { return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; }

			default:
			{
				LVN_CORE_WARN("unknown descriptor type enum (%d), setting to descriptor type sampler (defualt)", type);
				return VK_DESCRIPTOR_TYPE_SAMPLER;
			}
		}
	}

	static VkBufferUsageFlags getUniformBufferTypeEnum(LvnBufferType type)
	{
		switch (type)
		{
			case Lvn_BufferType_Uniform: { return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; }
			case Lvn_BufferType_Storage: { return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT; }

			default:
			{
				LVN_CORE_WARN("unknown buffer enum type (%u), setting to buffer type uniform buffer (defualt)", type);
				return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			}
		}
	}

	static VkShaderStageFlags getShaderStageFlagEnum(LvnShaderStage stage)
	{
		switch (stage)
		{
			case Lvn_ShaderStage_All: { return VK_SHADER_STAGE_ALL; }
			case Lvn_ShaderStage_Vertex: { return VK_SHADER_STAGE_VERTEX_BIT; }
			case Lvn_ShaderStage_Fragment: { return VK_SHADER_STAGE_FRAGMENT_BIT; }

			default:
			{
				LVN_CORE_WARN("unknown shader stage enum type (%u), setting stage to \'VK_SHADER_STAGE_ALL\' as default", stage);
				return VK_SHADER_STAGE_ALL;
			}
		}
	}

	static VkFilter getTextureFilterEnum(LvnTextureFilter filter)
	{
		switch (filter)
		{
			case Lvn_TextureFilter_Nearest: { return VK_FILTER_NEAREST; }
			case Lvn_TextureFilter_Linear: { return VK_FILTER_LINEAR; }

			default:
			{
				LVN_CORE_WARN("unknown sampler filter enum type (%u), setting filter to \'VK_FILTER_NEAREST\' as default", filter);
				return VK_FILTER_NEAREST;
			}
		}
	}

	static VkSamplerAddressMode getTextureWrapModeEnum(LvnTextureMode mode)
	{
		switch (mode)
		{
			case Lvn_TextureMode_Repeat: { return VK_SAMPLER_ADDRESS_MODE_REPEAT; }
			case Lvn_TextureMode_MirrorRepeat: { return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT; }
			case Lvn_TextureMode_ClampToEdge: { return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; }
			case Lvn_TextureMode_ClampToBorder: { return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER; }

			default:
			{
				LVN_CORE_WARN("unknown sampler address mode enum type (%u), setting mode to \'VK_SAMPLER_ADDRESS_MODE_REPEAT\' as default", mode);
				return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			}
		}
	}

	static VulkanPipeline createVulkanPipeline(VulkanBackends* vkBackends, VulkanPipelineCreateData* createData)
	{
		VulkanPipeline pipeline{};
		
		LvnPipelineSpecification* pipelineSpecification = createData->pipelineSpecification;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		
		inputAssembly.topology = vks::getVulkanTopologyTypeEnum(pipelineSpecification->inputAssembly.topology);
		inputAssembly.primitiveRestartEnable = pipelineSpecification->inputAssembly.primitiveRestartEnable;

		std::vector<VkDynamicState> dynamicStates;
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

		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
		
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

		LVN_CORE_CALL_ASSERT(vkCreatePipelineLayout(vkBackends->device, &pipelineLayoutInfo, nullptr, &pipeline.pipelineLayout) == VK_SUCCESS, "[vulkan] failed to create pipeline layout!");

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

		LVN_CORE_CALL_ASSERT(vkCreateGraphicsPipelines(vkBackends->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.pipeline) == VK_SUCCESS, "[vulkan] failed to create graphics pipeline!");

		return pipeline;
	}

	static VkShaderModule createShaderModule(VulkanBackends* vkBackends, const uint8_t* code, uint32_t size)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = size;
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code);
		
		VkShaderModule shaderModule;
		LVN_CORE_CALL_ASSERT(vkCreateShaderModule(vkBackends->device, &createInfo, nullptr, &shaderModule) == VK_SUCCESS, "[vulkan] failed to create shader module!");

		return shaderModule;
	}

	static LvnResult compileShaderToSPIRV(glslang_stage_t stage, const char* shaderSource, std::vector<uint8_t>& bin)
	{
		glslang_initialize_process();

		glslang_resource_t defaultResources =
		{
			/* .max_lights = */                                        32,
			/* .max_clip_planes = */                                   6,
			/* .max_texture_units = */                                 32,
			/* .max_texture_coords = */                                32,
			/* .max_vertex_attribs = */                                64,
			/* .max_vertex_uniform_components = */                     4096,
			/* .max_varying_floats = */                                64,
			/* .max_vertex_texture_image_units = */                    32,
			/* .max_combined_texture_image_units = */                  80,
			/* .max_texture_image_units = */                           32,
			/* .max_fragment_uniform_components = */                   4096,
			/* .max_draw_buffers = */                                  32,
			/* .max_vertex_uniform_vectors = */                        128,
			/* .max_varying_vectors = */                               8,
			/* .max_fragment_uniform_vectors = */                      16,
			/* .max_vertex_output_vectors = */                         16,
			/* .max_fragment_input_vectors = */                        15,
			/* .min_program_texel_offset = */                          -8,
			/* .max_program_texel_offset = */                          7,
			/* .max_clip_distances = */                                8,
			/* .max_compute_work_group_count_x = */                    65535,
			/* .max_compute_work_group_count_y = */                    65535,
			/* .max_compute_work_group_count_z = */                    65535,
			/* .max_compute_work_group_size_x = */                     1024,
			/* .max_compute_work_group_size_y = */                     1024,
			/* .max_compute_work_group_size_z = */                     64,
			/* .max_compute_uniform_components = */                    1024,
			/* .max_compute_texture_image_units = */                   16,
			/* .max_compute_image_uniforms = */                        8,
			/* .max_compute_atomic_counters = */                       8,
			/* .max_compute_atomic_counter_buffers = */                1,
			/* .max_varying_components = */                            60,
			/* .max_vertex_output_components = */                      64,
			/* .max_geometry_input_components = */                     64,
			/* .max_geometry_output_components = */                    128,
			/* .max_fragment_input_components = */                     128,
			/* .max_image_units = */                                   8,
			/* .max_combined_image_units_and_fragment_outputs = */     8,
			/* .max_combined_shader_output_resources = */              8,
			/* .max_image_samples = */                                 0,
			/* .max_vertex_image_uniforms = */                         0,
			/* .max_tess_control_image_uniforms = */                   0,
			/* .max_tess_evaluation_image_uniforms = */                0,
			/* .max_geometry_image_uniforms = */                       0,
			/* .max_fragment_image_uniforms = */                       8,
			/* .max_combined_image_uniforms = */                       8,
			/* .max_geometry_texture_image_units = */                  16,
			/* .max_geometry_output_vertices = */                      256,
			/* .max_geometry_total_output_components = */              1024,
			/* .max_geometry_uniform_components = */                   1024,
			/* .max_geometry_varying_components = */                   64,
			/* .max_tess_control_input_components = */                 128,
			/* .max_tess_control_output_components = */                128,
			/* .max_tess_control_texture_image_units = */              16,
			/* .max_tess_control_uniform_components = */               1024,
			/* .max_tess_control_total_output_components = */          4096,
			/* .max_tess_evaluation_input_components = */              128,
			/* .max_tess_evaluation_output_components = */             128,
			/* .max_tess_evaluation_texture_image_units = */           16,
			/* .max_tess_evaluation_uniform_components = */            1024,
			/* .max_tess_patch_components = */                         120,
			/* .max_patch_vertices = */                                32,
			/* .max_tess_gen_level = */                                64,
			/* .max_viewports = */                                     16,
			/* .max_vertex_atomic_counters = */                        0,
			/* .max_tess_control_atomic_counters = */                  0,
			/* .max_tess_evaluation_atomic_counters = */               0,
			/* .max_geometry_atomic_counters = */                      0,
			/* .max_fragment_atomic_counters = */                      8,
			/* .max_combined_atomic_counters = */                      8,
			/* .max_atomic_counter_bindings = */                       1,
			/* .max_vertex_atomic_counter_buffers = */                 0,
			/* .max_tess_control_atomic_counter_buffers = */           0,
			/* .max_tess_evaluation_atomic_counter_buffers = */        0,
			/* .max_geometry_atomic_counter_buffers = */               0,
			/* .max_fragment_atomic_counter_buffers = */               1,
			/* .max_combined_atomic_counter_buffers = */               1,
			/* .max_atomic_counter_buffer_size = */                    16384,
			/* .max_transform_feedback_buffers = */                    4,
			/* .max_transform_feedback_interleaved_components = */     64,
			/* .max_cull_distances = */                                8,
			/* .max_combined_clip_and_cull_distances = */              8,
			/* .max_samples = */                                       4,
			/* .max_mesh_output_vertices_nv = */                       256,
			/* .max_mesh_output_primitives_nv = */                     512,
			/* .max_mesh_work_group_size_x_nv = */                     32,
			/* .max_mesh_work_group_size_y_nv = */                     1,
			/* .max_mesh_work_group_size_z_nv = */                     1,
			/* .max_task_work_group_size_x_nv = */                     32,
			/* .max_task_work_group_size_y_nv = */                     1,
			/* .max_task_work_group_size_z_nv = */                     1,
			/* .max_mesh_view_count_nv = */                            4,
			/* .max_mesh_output_vertices_ext = */                      256,
			/* .max_mesh_output_primitives_ext = */                    256,
			/* .max_mesh_work_group_size_x_ext = */                    128,
			/* .max_mesh_work_group_size_y_ext = */                    128,
			/* .max_mesh_work_group_size_z_ext = */                    128,
			/* .max_task_work_group_size_x_ext = */                    128,
			/* .max_task_work_group_size_y_ext = */                    128,
			/* .max_task_work_group_size_z_ext = */                    128,
			/* .max_mesh_view_count_ext = */                           4,
			/* .maxDualSourceDrawBuffersEXT = */                       1,

			/* .limits = */
			{
				/* .non_inductive_for_loops = */                       1,
				/* .while_loops = */                                   1,
				/* .do_while_loops = */                                1,
				/* .general_uniform_indexing = */                      0,
				/* .general_attribute_matrix_vector_indexing = */      1,
				/* .general_varying_indexing = */                      1,
				/* .general_sampler_indexing = */                      1,
				/* .general_variable_indexing = */                     1,
				/* .general_constant_matrix_vector_indexing = */       1,
			}
		};


		const glslang_input_t input =
		{
			/* .language = */                             GLSLANG_SOURCE_GLSL,
			/* .stage = */                                stage,
			/* .client = */                               GLSLANG_CLIENT_VULKAN,
			/* .client_version = */                       GLSLANG_TARGET_VULKAN_1_2,
			/* .target_language = */                      GLSLANG_TARGET_SPV,
			/* .target_language_version = */              GLSLANG_TARGET_SPV_1_5,
			/* .code = */                                 shaderSource,
			/* .default_version = */                      100,
			/* .default_profile = */                      GLSLANG_NO_PROFILE,
			/* .force_default_version_and_profile = */    false,
			/* .forward_compatible = */                   false,
			/* .messages = */                             GLSLANG_MSG_DEFAULT_BIT,
			/* .resource = */                             &defaultResources,
		};

		glslang_shader_t* shader = glslang_shader_create(&input);

		if (!glslang_shader_preprocess(shader, &input))
		{
			LVN_CORE_ERROR("[vulkan-glslang] GLSL preprocessing failed when compiling from shader source:\ncode: %s\ninfo: %s\ndebug: %s", input.code, glslang_shader_get_info_log(shader), glslang_shader_get_info_debug_log(shader));
			glslang_shader_delete(shader);
			return Lvn_Result_Failure;
		}

		if (!glslang_shader_parse(shader, &input))
		{
			LVN_CORE_ERROR("[vulkan-glslang] GLSL parsing failed when compiling from shader source:\ncode: %s\ninfo: %s\ndebug: %s\npreprocessed code: %s", input.code, glslang_shader_get_info_log(shader), glslang_shader_get_info_debug_log(shader), glslang_shader_get_preprocessed_code(shader));
			glslang_shader_delete(shader);
			return Lvn_Result_Failure;
		}

		glslang_program_t* program = glslang_program_create();
		glslang_program_add_shader(program, shader);

		if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
		{
			LVN_CORE_ERROR("[vulkan-glslang] GLSL linking failed when compiling from shader source:\ncode: %s\ninfo: %s\ndebug: %s", input.code, glslang_shader_get_info_log(shader), glslang_shader_get_info_debug_log(shader));
			glslang_program_delete(program);
			glslang_shader_delete(shader);
			return Lvn_Result_Failure;
		}

		glslang_program_SPIRV_generate(program, stage);

		bin.resize(4 * glslang_program_SPIRV_get_size(program)); // NOTE: 4 bytes (uint8_t) in one uint32_t, we need to convert to uint8_t* for shader module creation in Vulkan

		glslang_program_SPIRV_get(program, (uint32_t*)bin.data());

		const char* spirv_messages = glslang_program_SPIRV_get_messages(program);
		if (spirv_messages)
		{
			LVN_CORE_TRACE("[vulkan-glslang] %s", spirv_messages);
		}

		glslang_program_delete(program);
		glslang_shader_delete(shader);

		glslang_finalize_process();

		return Lvn_Result_Success;
	}

	static LvnResult createBuffer(VulkanBackends* vkBackends, VkBuffer* buffer, VmaAllocation* bufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memUsage)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = memUsage;

		if (vmaCreateBuffer(vkBackends->vmaAllocator, &bufferInfo, &allocInfo, buffer, bufferMemory, nullptr) != VK_SUCCESS)
		{
			LVN_CORE_ERROR("[vulkan] failed to create buffer <VkBuffer> (%p), buffer memory: (%p), buffer size: %zu bytes", *buffer, *bufferMemory, size);
			return Lvn_Result_Failure;
		}

		return Lvn_Result_Success;
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

	static LvnResult createImage(VulkanBackends* vkBackends, VkImage* image, VmaAllocation* imageMemory, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkSampleCountFlagBits samples, VmaMemoryUsage memUsage)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = samples;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		if (vmaCreateImage(vkBackends->vmaAllocator, &imageInfo, &allocInfo, image, imageMemory, nullptr) != VK_SUCCESS)
		{
			LVN_CORE_ERROR("[vulkan] failed to create image <VkImage>, image size: (w:%u, h:%u)", width, height);
			return Lvn_Result_Failure;
		}

		return Lvn_Result_Success;
	}

	static void transitionImageLayout(VulkanBackends* vkBackends, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount)
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


		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = layerCount;

		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (vks::hasStencilComponent(format))
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		else
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		} 
		else
		{
			LVN_CORE_ASSERT(false, "[vulkan] unsupported layout transition during image layout transition");
		}

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);


		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(vkBackends->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(vkBackends->graphicsQueue);

		vkFreeCommandBuffers(vkBackends->device, vkBackends->commandPool, 1, &commandBuffer);
	}

	static void copyBufferToImage(VulkanBackends* vkBackends, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount)
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


		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layerCount;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

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

VulkanBackends* getVulkanBackends()
{
	return s_VkBackends;
}

void createVulkanWindowSurfaceData(LvnWindow* window)
{
	VulkanBackends* vkBackends = s_VkBackends;

	GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window->nativeWindow);
	window->apiData = new VulkanWindowSurfaceData();
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
	bool vSync = window->data.vSync;

	LVN_CORE_CALL_ASSERT(glfwCreateWindowSurface(vkBackends->instance, glfwWindow, nullptr, &surfaceData->surface) == VK_SUCCESS, "[vulkan] failed to create temporary window surface at (%p)", surfaceData->surface);

	// get and check swap chain specs
	VulkanSwapChainSupportDetails swapChainSupport = vks::querySwapChainSupport(surfaceData->surface, vkBackends->physicalDevice);
	LVN_CORE_ASSERT(!swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty(), "[vulkan] device does not have supported swap chain formats or present modes");

	VkSurfaceFormatKHR surfaceFormat = vks::chooseSwapSurfaceFormat(vkBackends, swapChainSupport.formats.data(), swapChainSupport.formats.size());
	VkPresentModeKHR presentMode = vks::chooseSwapPresentMode(swapChainSupport.presentModes.data(), swapChainSupport.presentModes.size(), vSync);
	VkExtent2D extent = vks::chooseSwapExtent(glfwWindow, &swapChainSupport.capabilities);

	vks::createSwapChain(vkBackends, surfaceData, swapChainSupport, surfaceFormat, presentMode, extent);
	vks::createImageViews(vkBackends, surfaceData);
	vks::createDepthResources(vkBackends, surfaceData);
	vks::createRenderPass(vkBackends, surfaceData, surfaceFormat.format);
	vks::createFrameBuffers(vkBackends, surfaceData);
	vks::createCommandBuffers(vkBackends, surfaceData);
	vks::createSyncObjects(vkBackends, surfaceData);

	surfaceData->clearColor = {{ 0.0f, 0.0f, 0.0f, 1.0f }};

	window->renderPass.nativeRenderPass = static_cast<VulkanWindowSurfaceData*>(window->apiData)->renderPass;
}

void destroyVulkanWindowSurfaceData(LvnWindow* window)
{
	if (!window->apiData) { return; }

	VulkanBackends* vkBackends = s_VkBackends;
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);

	vkDeviceWaitIdle(vkBackends->device);

	// sync objects
	for (uint32_t i = 0; i < vkBackends->maxFramesInFlight; i++)
	{
		vkDestroySemaphore(vkBackends->device, surfaceData->imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(vkBackends->device, surfaceData->renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(vkBackends->device, surfaceData->inFlightFences[i], nullptr);
	}

	// swap chain images
	for (uint32_t i = 0; i < surfaceData->swapChainImageViews.size(); i++)
	{
		vkDestroyImageView(vkBackends->device, surfaceData->swapChainImageViews[i], nullptr);
	}

	vkDestroyImageView(vkBackends->device, surfaceData->depthImageView, nullptr);
    vkDestroyImage(vkBackends->device, surfaceData->depthImage, nullptr);
	vmaFreeMemory(vkBackends->vmaAllocator, surfaceData->depthImageMemory);

	// frame buffers
	for (uint32_t i = 0; i < surfaceData->frameBuffers.size(); i++)
	{
		vkDestroyFramebuffer(vkBackends->device, surfaceData->frameBuffers[i], nullptr);
	}

	// swap chain
	vkDestroySwapchainKHR(vkBackends->device, surfaceData->swapChain, nullptr);

	// render pass
	vkDestroyRenderPass(vkBackends->device, surfaceData->renderPass, nullptr);

	// window surface
	vkDestroySurfaceKHR(vkBackends->instance, surfaceData->surface, nullptr);

	delete static_cast<VulkanWindowSurfaceData*>(window->apiData);
}


LvnResult vksImplCreateContext(LvnGraphicsContext* graphicsContext)
{
	if (s_VkBackends == nullptr)
	{
		s_VkBackends = new VulkanBackends();
	}

	VulkanBackends* vkBackends = s_VkBackends;

	vkBackends->enableValidationLayers = graphicsContext->enableValidationLayers;
	vkBackends->maxFramesInFlight = graphicsContext->maxFramesInFlight;

	switch (graphicsContext->frameBufferColorFormat)
	{
		case Lvn_TextureFormat_Unorm: { vkBackends->frameBufferColorFormat = VK_FORMAT_B8G8R8A8_UNORM; break; }
		case Lvn_TextureFormat_Srgb: { vkBackends->frameBufferColorFormat = VK_FORMAT_B8G8R8A8_SRGB; break; }

		default:
		{
			LVN_CORE_ERROR("[vulkan]: invalid framebuffer color format for swapchain surface creation: (%u)", graphicsContext->frameBufferColorFormat);
			return Lvn_Result_Failure;
		}
	}

	graphicsContext->getPhysicalDevices = vksImplGetPhysicalDevices;
	graphicsContext->checkPhysicalDeviceSupport = vksImplCheckPhysicalDeviceSupport;
	graphicsContext->renderInit = vksImplRenderInit;
	graphicsContext->createShaderFromSrc = vksImplCreateShaderFromSrc;
	graphicsContext->createShaderFromFileSrc = vksImplCreateShaderFromFileSrc;
	graphicsContext->createShaderFromFileBin = vksImplCreateShaderFromFileBin;
	graphicsContext->createDescriptorLayout = vksImplCreateDescriptorLayout;
	graphicsContext->createPipeline = vksImplCreatePipeline;
	graphicsContext->createFrameBuffer = vksImplCreateFrameBuffer;
	graphicsContext->createBuffer = vksImplCreateBuffer;
	graphicsContext->createUniformBuffer = vksImplCreateUniformBuffer;
	graphicsContext->createSampler = vksImplCreateSampler;
	graphicsContext->createTexture = vksImplCreateTexture;
	graphicsContext->createTextureSampler = vksImplCreateTextureSampler;
	graphicsContext->createCubemap = vksImplCreateCubemap;
	graphicsContext->createCubemapHdr = vksImplCreateCubemapHdr;
	
	graphicsContext->destroyShader = vksImplDestroyShader;
	graphicsContext->destroyDescriptorLayout = vksImplDestroyDescriptorLayout;
	graphicsContext->destroyPipeline = vksImplDestroyPipeline;
	graphicsContext->destroyFrameBuffer = vksImplDestroyFrameBuffer;
	graphicsContext->destroyBuffer = vksImplDestroyBuffer;
	graphicsContext->destroyUniformBuffer = vksImplDestroyUniformBuffer;
	graphicsContext->destroySampler = vksImplDestroySampler;
	graphicsContext->destroyTexture = vksImplDestroyTexture;
	graphicsContext->destroyCubemap = vksImplDestroyCubemap;

	graphicsContext->renderClearColor = vksImplRenderClearColor;
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
	graphicsContext->renderCmdBindDescriptorSets = vksImplRenderCmdBindDescriptorSets;
	graphicsContext->renderCmdBeginFrameBuffer = vksImplRenderCmdBeginFrameBuffer;
	graphicsContext->renderCmdEndFrameBuffer = vksImplRenderCmdEndFrameBuffer;

	graphicsContext->bufferUpdateVertexData = vksImplBufferUpdateVertexData;
	graphicsContext->bufferUpdateIndexData = vksImplBufferUpdateIndexData;
	graphicsContext->bufferResizeVertexBuffer = vksImplBufferResizeVertexBuffer;
	graphicsContext->bufferResizeIndexBuffer = vksImplBufferResizeIndexBuffer;
	graphicsContext->allocateDescriptorSet = vksImplAllocateDescriptorSet;
	graphicsContext->updateUniformBufferData = vksImplUpdateUniformBufferData;
	graphicsContext->updateDescriptorSetData = vksImplUpdateDescriptorSetData;
	graphicsContext->frameBufferGetImage = vksImplFrameBufferGetImage;
	graphicsContext->frameBufferGetRenderPass = vksImplFrameBufferGetRenderPass;
	graphicsContext->framebufferResize = vksImplFrameBufferResize;
	graphicsContext->frameBufferSetClearColor = vksImplFrameBufferSetClearColor;
	graphicsContext->findSupportedDepthImageFormat = vksImplFindSupportedDepthImageFormat;

	// Create Vulkan Instance
	if (vks::createVulkanInstace(vkBackends, graphicsContext->enableValidationLayers) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[vulkan] failed to create vulkan instance when creating graphics context");
		return Lvn_Result_Failure;
	}

	return Lvn_Result_Success;
}

void vksImplTerminateContext()
{
	VulkanBackends* vkBackends = s_VkBackends;

	if (vkBackends->renderInit)
	{
		vkDeviceWaitIdle(vkBackends->device);

		vkDestroyCommandPool(vkBackends->device, vkBackends->commandPool, nullptr);

		// VmaAllocator
		vmaDestroyAllocator(vkBackends->vmaAllocator);

		// logical device
		vkDestroyDevice(vkBackends->device, nullptr);

		// debug validation layers
		if (vkBackends->enableValidationLayers)
			vks::destroyDebugUtilsMessengerEXT(vkBackends->instance, vkBackends->debugMessenger, nullptr);
	}

	// instance
	vkDestroyInstance(vkBackends->instance, nullptr);

	delete s_VkBackends;
}

void vksImplGetPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* physicalDeviceCount)
{
	VulkanBackends* vkBackends = s_VkBackends;


	// Get Physical Devices
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vkBackends->instance, &deviceCount, nullptr);

	if (pPhysicalDevices == nullptr)
	{
		*physicalDeviceCount = deviceCount;
		return;
	}
	if (!deviceCount)
	{
		LVN_CORE_ERROR("[vulkan] failed to find physical devices with vulkan support");
		return;
	}


	// Create vulkan physical devices
	vkBackends->lvnPhysicalDevices.resize(deviceCount);
	vkBackends->physicalDevices.resize(deviceCount);
	vkEnumeratePhysicalDevices(vkBackends->instance, &deviceCount, vkBackends->physicalDevices.data());

	for (uint32_t i = 0; i < vkBackends->physicalDevices.size(); i++)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(vkBackends->physicalDevices[i], &deviceProperties);

		LvnPhysicalDeviceInfo deviceInfo{};
		deviceInfo.type = vks::getPhysicalDeviceTypeEnum(deviceProperties.deviceType);
			
		deviceInfo.apiVersion = deviceProperties.apiVersion;
		deviceInfo.driverVersion = deviceProperties.driverVersion;
		memcpy(deviceInfo.name, deviceProperties.deviceName, 226);
		vkBackends->lvnPhysicalDevices[i].info = deviceInfo;
		vkBackends->lvnPhysicalDevices[i].device = vkBackends->physicalDevices[i];
	}

	*pPhysicalDevices = vkBackends->lvnPhysicalDevices.data();
}

LvnResult vksImplCheckPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice)
{
	VulkanBackends* vkBackends = s_VkBackends;
	VkPhysicalDevice vkDevice = static_cast<VkPhysicalDevice>(physicalDevice->device);

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	VkSurfaceKHR surface;
	GLFWwindow* glfwWindow = glfwCreateWindow(1, 1, "", nullptr, nullptr);

	if (glfwCreateWindowSurface(vkBackends->instance, glfwWindow, nullptr, &surface) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("[vulkan] check physical device support, failed to create temporary window surface at (%p) when checking physical device support", surface);
		return Lvn_Result_Failure;
	}


	VulkanQueueFamilyIndices queueIndices = vks::findQueueFamilies(vkDevice, surface);

	// Check queue families
	if (!queueIndices.has_graphics || !queueIndices.has_present)
	{
		LVN_CORE_ERROR("[vulkan] check physical device support, physical device does not support queue families needed");
		return Lvn_Result_Failure;
	}

	// Check device extension support
	if (!vks::checkDeviceExtensionSupport(vkDevice))
	{
		LVN_CORE_ERROR("[vulkan] check physical device support, physical device does not support required extensions");
		return Lvn_Result_Failure;
	}

	vkDestroySurfaceKHR(vkBackends->instance, surface, nullptr);
	glfwDestroyWindow(glfwWindow);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

	return Lvn_Result_Success;
}

LvnResult vksImplRenderInit(LvnRenderInitInfo* renderInfo)
{
	VulkanBackends* vkBackends = s_VkBackends;
	vkBackends->renderInit = true;
	vkBackends->defaultPipelineSpecification = lvn::pipelineSpecificationGetConfig();

	vkBackends->physicalDevice = static_cast<VkPhysicalDevice>(renderInfo->physicalDevice->device);
	vkBackends->maxFramesInFlight = renderInfo->maxFramesInFlight != 0 ? renderInfo->maxFramesInFlight : 1;

	if (vkBackends->enableValidationLayers && !vks::checkValidationLayerSupport())
		LVN_CORE_WARN("vulkan validation layers enabled, but not available!");
	else
		vks::setupDebugMessenger(vkBackends);

	VkPhysicalDeviceProperties physicalDeviceProperties{};
	vkGetPhysicalDeviceProperties(vkBackends->physicalDevice, &physicalDeviceProperties);
	LVN_CORE_TRACE("[vulkan] physical device (GPU) will be used: \"%s\", driverVersion: (%u), apiVersion: (%u)", physicalDeviceProperties.deviceName, physicalDeviceProperties.driverVersion, physicalDeviceProperties.apiVersion);
	vkBackends->deviceProperties = physicalDeviceProperties;

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(vkBackends->physicalDevice, &supportedFeatures);
	vkBackends->deviceSupportedFeatures = supportedFeatures;

	// create dummy window and surface to get device queue indices support
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	VkSurfaceKHR surface;
	GLFWwindow* glfwWindow = glfwCreateWindow(1, 1, "", nullptr, nullptr);

	if (glfwCreateWindowSurface(vkBackends->instance, glfwWindow, nullptr, &surface) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("[vulkan] failed to create temporary window surface at (%p)", surface);
		return Lvn_Result_Failure;
	}

	// create logical device once
	vks::createLogicalDevice(vkBackends, surface);

	// get and check swap chain specs
	VulkanSwapChainSupportDetails swapChainSupport = vks::querySwapChainSupport(surface, vkBackends->physicalDevice);
	LVN_CORE_ASSERT(!swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty(), "[vulkan] selected physical device does not have supported swap chain formats or present modes");

	// destroy dummy window and surface
	vkDestroySurfaceKHR(vkBackends->instance, surface, nullptr);
	glfwDestroyWindow(glfwWindow);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

	// create command buffer pool
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = vkBackends->deviceIndices.graphicsIndex;

	LVN_CORE_CALL_ASSERT(vkCreateCommandPool(vkBackends->device, &poolInfo, nullptr, &vkBackends->commandPool) == VK_SUCCESS, "[vulkan] failed to create command pool!");


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

void vksImplRenderClearColor(LvnWindow* window, float r, float g, float b, float a)
{
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
	surfaceData->clearColor = {{ r, g, b, a }};
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
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
	vkCmdDraw(surfaceData->commandBuffers[surfaceData->currentFrame], vertexCount, instanceCount, 0, firstInstance);
}

void vksImplRenderCmdDrawIndexedInstanced(LvnWindow* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance)
{
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
	vkCmdDrawIndexed(surfaceData->commandBuffers[surfaceData->currentFrame], indexCount, instanceCount, 0, 0, firstInstance);
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
	LVN_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "[vulkan] failed to acquire swap chain image!");

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

	LVN_CORE_CALL_ASSERT(vkQueueSubmit(vkBackends->graphicsQueue, 1, &submitInfo, surfaceData->inFlightFences[surfaceData->currentFrame]) == VK_SUCCESS, "[vulkan] failed to submit draw command buffer!");


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
		LVN_CORE_ASSERT(result == VK_SUCCESS, "[vulkan] failed to present swap chain image");
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
	LVN_CORE_CALL_ASSERT(vkBeginCommandBuffer(surfaceData->commandBuffers[surfaceData->currentFrame], &beginInfo) == VK_SUCCESS, "[vulkan] failed to begin recording command buffer!");
}

void vksImplRenderEndCommandRecording(LvnWindow* window)
{
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
	LVN_CORE_CALL_ASSERT(vkEndCommandBuffer(surfaceData->commandBuffers[surfaceData->currentFrame]) == VK_SUCCESS, "[vulkan] failed to record command buffer!");
}

void vksImplRenderCmdBeginRenderPass(LvnWindow* window)
{
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = surfaceData->renderPass;
	renderPassInfo.framebuffer = surfaceData->frameBuffers[surfaceData->imageIndex];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = surfaceData->swapChainExtent;

	VkClearValue clearColor[2];
	clearColor[0].color = surfaceData->clearColor;
	clearColor[1].depthStencil = {1.0f, 0};

	renderPassInfo.clearValueCount = ARRAY_LEN(clearColor);
	renderPassInfo.pClearValues = clearColor;

	vkCmdBeginRenderPass(surfaceData->commandBuffers[surfaceData->currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = static_cast<float>(surfaceData->swapChainExtent.height);
	viewport.width = static_cast<float>(surfaceData->swapChainExtent.width);
	viewport.height = -static_cast<float>(surfaceData->swapChainExtent.height);
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
	VkBuffer vertexBuffer = static_cast<VkBuffer>(buffer->vertexBuffer);
	VkDeviceSize offsets[] = {0};

	vkCmdBindVertexBuffers(surfaceData->commandBuffers[surfaceData->currentFrame], 0, 1, &vertexBuffer, offsets);
}

void vksImplRenderCmdBindIndexBuffer(LvnWindow* window, LvnBuffer* buffer)
{
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
	VkBuffer indexBuffer = static_cast<VkBuffer>(buffer->indexBuffer);

	vkCmdBindIndexBuffer(surfaceData->commandBuffers[surfaceData->currentFrame], indexBuffer, buffer->indexOffset, VK_INDEX_TYPE_UINT32);
}

void vksImplRenderCmdBindDescriptorSets(LvnWindow* window, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets)
{
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
	VkPipelineLayout pipelineLayout = static_cast<VkPipelineLayout>(pipeline->nativePipelineLayout);

	std::vector<VkDescriptorSet> descriptorSets(descriptorSetCount);
	for (uint32_t i = 0; i < descriptorSetCount; i++)
	{
		descriptorSets[i] = static_cast<VkDescriptorSet>(pDescriptorSets[i]->descriptorSets[surfaceData->currentFrame]);
	}

	vkCmdBindDescriptorSets(surfaceData->commandBuffers[surfaceData->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, firstSetIndex, descriptorSetCount, descriptorSets.data(), 0, nullptr);
}

void vksImplRenderCmdBeginFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer)
{
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
	VulkanFrameBufferData* frameBufferData = static_cast<VulkanFrameBufferData*>(frameBuffer->frameBufferData);

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = frameBufferData->renderPass;
	renderPassInfo.framebuffer = frameBufferData->framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent.width = frameBufferData->width;
	renderPassInfo.renderArea.extent.height = frameBufferData->height;
	renderPassInfo.clearValueCount = frameBufferData->clearValues.size();
	renderPassInfo.pClearValues = frameBufferData->clearValues.data();

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(frameBufferData->width);
	viewport.height = static_cast<float>(frameBufferData->height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(surfaceData->commandBuffers[surfaceData->currentFrame], 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent.width = frameBufferData->width;
	scissor.extent.height = frameBufferData->height;
	vkCmdSetScissor(surfaceData->commandBuffers[surfaceData->currentFrame], 0, 1, &scissor);

	vkCmdBeginRenderPass(surfaceData->commandBuffers[surfaceData->currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void vksImplRenderCmdEndFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer)
{
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
	vkCmdEndRenderPass(surfaceData->commandBuffers[surfaceData->currentFrame]);
}

LvnResult vksImplCreateShaderFromSrc(LvnShader* shader, LvnShaderCreateInfo* createInfo)
{
	VulkanBackends* vkBackends = s_VkBackends;

	std::vector<uint8_t> vertData;
	std::vector<uint8_t> fragData;

	if (vks::compileShaderToSPIRV(GLSLANG_STAGE_VERTEX, createInfo->vertexSrc.c_str(), vertData) == Lvn_Result_Failure)
	{
		LVN_CORE_ERROR("[vulkan] failed to create vertex shader module for shader at (%p)", shader);
		return Lvn_Result_Failure;
	}

	if (vks::compileShaderToSPIRV(GLSLANG_STAGE_FRAGMENT, createInfo->fragmentSrc.c_str(), fragData) == Lvn_Result_Failure)
	{
		LVN_CORE_ERROR("[vulkan] failed to create fragment shader module for shader at (%p)", shader);
		return Lvn_Result_Failure;
	}

	VkShaderModule vertShaderModule = vks::createShaderModule(vkBackends, vertData.data(), vertData.size());
	VkShaderModule fragShaderModule = vks::createShaderModule(vkBackends, fragData.data(), fragData.size());

	shader->nativeVertexShaderModule = vertShaderModule;
	shader->nativeFragmentShaderModule = fragShaderModule;

	return Lvn_Result_Success;
}

LvnResult vksImplCreateShaderFromFileSrc(LvnShader* shader, LvnShaderCreateInfo* createInfo)
{
	VulkanBackends* vkBackends = s_VkBackends;

	std::string fileVertSrc = lvn::loadFileSrc(createInfo->vertexSrc.c_str());
	std::string fileFragSrc = lvn::loadFileSrc(createInfo->fragmentSrc.c_str());

	std::vector<uint8_t> vertData;
	std::vector<uint8_t> fragData;

	if (vks::compileShaderToSPIRV(GLSLANG_STAGE_VERTEX, fileVertSrc.c_str(), vertData) == Lvn_Result_Failure)
	{
		LVN_CORE_ERROR("[vulkan] failed to create vertex shader module for shader at (%p), filepath: %s", shader, createInfo->vertexSrc.c_str());
		return Lvn_Result_Failure;
	}

	if (vks::compileShaderToSPIRV(GLSLANG_STAGE_FRAGMENT, fileFragSrc.c_str(), fragData) == Lvn_Result_Failure)
	{
		LVN_CORE_ERROR("[vulkan] failed to create fragment shader module for shader at (%p), filepath: %s", shader, createInfo->fragmentSrc.c_str());
		return Lvn_Result_Failure;
	}

	VkShaderModule vertShaderModule = vks::createShaderModule(vkBackends, vertData.data(), vertData.size());
	VkShaderModule fragShaderModule = vks::createShaderModule(vkBackends, fragData.data(), fragData.size());

	shader->nativeVertexShaderModule = vertShaderModule;
	shader->nativeFragmentShaderModule = fragShaderModule;

	return Lvn_Result_Success;
}

LvnResult vksImplCreateShaderFromFileBin(LvnShader* shader, LvnShaderCreateInfo* createInfo)
{
	VulkanBackends* vkBackends = s_VkBackends;

	LvnData<uint8_t> vertbin = lvn::loadFileSrcBin(createInfo->vertexSrc.c_str());
	LvnData<uint8_t> fragbin = lvn::loadFileSrcBin(createInfo->fragmentSrc.c_str());

	VkShaderModule vertShaderModule = vks::createShaderModule(vkBackends, vertbin.data(), vertbin.size());
	VkShaderModule fragShaderModule = vks::createShaderModule(vkBackends, fragbin.data(), fragbin.size());

	shader->nativeVertexShaderModule = vertShaderModule;
	shader->nativeFragmentShaderModule = fragShaderModule;

	return Lvn_Result_Success;
}

LvnResult vksImplCreateDescriptorLayout(LvnDescriptorLayout* descriptorLayout, LvnDescriptorLayoutCreateInfo* createInfo)
{
	VulkanBackends* vkBackends = s_VkBackends;

	std::vector<VkDescriptorSetLayoutBinding> layoutBindings(createInfo->descriptorBindingCount);
	std::vector<VkDescriptorPoolSize> poolSizes(createInfo->descriptorBindingCount);

	for (uint32_t i = 0; i < createInfo->descriptorBindingCount; i++)
	{
		VkDescriptorType descriptorType = vks::getDescriptorTypeEnum(createInfo->pDescriptorBindings[i].descriptorType);

		layoutBindings[i].binding = createInfo->pDescriptorBindings[i].binding;
		layoutBindings[i].descriptorType = descriptorType;
		layoutBindings[i].descriptorCount = createInfo->pDescriptorBindings[i].descriptorCount;
		layoutBindings[i].pImmutableSamplers = nullptr;
		layoutBindings[i].stageFlags = vks::getShaderStageFlagEnum(createInfo->pDescriptorBindings[i].shaderStage);

		poolSizes[i].type = descriptorType;
		poolSizes[i].descriptorCount = createInfo->pDescriptorBindings[i].descriptorCount * createInfo->pDescriptorBindings[i].maxAllocations * vkBackends->maxFramesInFlight;
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = layoutBindings.size();
	layoutInfo.pBindings = layoutBindings.data();

	VkDescriptorSetLayout vkDescriptorLayout;
	if (vkCreateDescriptorSetLayout(vkBackends->device, &layoutInfo, nullptr, &vkDescriptorLayout) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("[vulkan] failed to create descriptor set layout <VkDescriptorSetLayout> at (%p)", vkDescriptorLayout);
		return Lvn_Result_Failure;
	}

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = createInfo->maxSets * vkBackends->maxFramesInFlight;

	VkDescriptorPool descriptorPool;

	if (vkCreateDescriptorPool(vkBackends->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("[vulkan] failed to create descriptor pool at (%p)", descriptorPool);
		return Lvn_Result_Failure;
	}

	descriptorLayout->descriptorLayout = vkDescriptorLayout;
	descriptorLayout->descriptorPool = descriptorPool;

	return Lvn_Result_Success;
}

LvnResult vksImplAllocateDescriptorSet(LvnDescriptorSet* descriptorSet, LvnDescriptorLayout* descriptorLayout)
{
	VulkanBackends* vkBackends = s_VkBackends;

	VkDescriptorSetLayout vkDescriptorLayout = static_cast<VkDescriptorSetLayout>(descriptorLayout->descriptorLayout);
	VkDescriptorPool descriptorPool = static_cast<VkDescriptorPool>(descriptorLayout->descriptorPool);

	std::vector<VkDescriptorSetLayout> layouts(vkBackends->maxFramesInFlight, vkDescriptorLayout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = vkBackends->maxFramesInFlight;
	allocInfo.pSetLayouts = layouts.data();

	descriptorSet->descriptorSets.resize(vkBackends->maxFramesInFlight);

	if (vkAllocateDescriptorSets(vkBackends->device, &allocInfo, (VkDescriptorSet*)descriptorSet->descriptorSets.data()) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("[vulkan] failed to allocate descriptor sets <VkDescriptorSet> at (%p)", descriptorSet->descriptorSets.data());
		return Lvn_Result_Failure;
	}

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
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(createInfo->vertexBindingDescriptionCount);
	std::vector<VkVertexInputAttributeDescription> vertexAttributes(createInfo->vertexAttributeCount);

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
	std::vector<VkDescriptorSetLayout> descriptorLayouts(createInfo->descriptorLayoutCount);
	for (uint32_t i = 0; i < createInfo->descriptorLayoutCount; i++)
	{
		VkDescriptorSetLayout descriptorLayout = static_cast<VkDescriptorSetLayout>(createInfo->pDescriptorLayouts[i]->descriptorLayout);
		descriptorLayouts[i] = descriptorLayout;
	}

	// render pass
	VkRenderPass renderPass = static_cast<VkRenderPass>(createInfo->renderPass->nativeRenderPass);

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
	VulkanBackends* vkBackends = s_VkBackends;

	frameBuffer->frameBufferData = new VulkanFrameBufferData();
	VulkanFrameBufferData* frameBufferData = static_cast<VulkanFrameBufferData*>(frameBuffer->frameBufferData);

	frameBufferData->width = createInfo->width;
	frameBufferData->height = createInfo->height;

	frameBufferData->colorAttachments = std::vector<LvnFrameBufferColorAttachment>(createInfo->pColorAttachments, createInfo->pColorAttachments + createInfo->colorAttachmentCount);
	frameBufferData->colorImages.resize(createInfo->colorAttachmentCount);
	frameBufferData->colorImageMemory.resize(createInfo->colorAttachmentCount);
	frameBufferData->colorImageViews.resize(createInfo->colorAttachmentCount);

	VkFilter filter = vks::getTextureFilterEnum(createInfo->textureFilter);
	VkSamplerAddressMode addressMode = vks::getTextureWrapModeEnum(createInfo->textureMode);

	frameBufferData->sampleCount = vks::getSampleCountFlagEnum(createInfo->sampleCount);
	frameBufferData->multisampling = createInfo->sampleCount != Lvn_SampleCount_1_Bit;
	frameBufferData->hasDepth = createInfo->depthAttachment != nullptr;
	frameBufferData->totalAttachmentCount = createInfo->colorAttachmentCount + (frameBufferData->hasDepth ? 1 : 0);
	frameBufferData->clearValues.resize(frameBufferData->totalAttachmentCount);

	std::vector<VkAttachmentDescription> attachmentDescriptions(frameBufferData->totalAttachmentCount);

	std::vector<VkAttachmentReference> colorReference(createInfo->colorAttachmentCount);
	VkAttachmentReference depthReference;

	// Color attachments
	for (uint32_t i = 0; i < createInfo->colorAttachmentCount; i++)
	{
		LvnFrameBufferColorAttachment colorAttachment = createInfo->pColorAttachments[i];
		VkFormat colorFormat = vks::getVulkanColorFormatEnum(createInfo->pColorAttachments[i].format);

		VkAttachmentDescription attchmentDescription{};
		attchmentDescription.format = colorFormat;
		attchmentDescription.samples = frameBufferData->sampleCount;
		attchmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attchmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attchmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attchmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attchmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attchmentDescription.finalLayout = frameBufferData->multisampling ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		attachmentDescriptions[colorAttachment.index] = attchmentDescription;
		colorReference[i] = { colorAttachment.index, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
		frameBufferData->clearValues[createInfo->pColorAttachments[i].index].color = {{ 0.0f, 0.0f, 0.0f, 1.0f }};
	}

	// Depth attachment
	if (createInfo->depthAttachment != nullptr)
	{
		frameBufferData->depthAttachment = *createInfo->depthAttachment;

		VkAttachmentDescription attchmentDescription{};
		attchmentDescription.format = vks::getVulkanDepthFormatEnum(createInfo->depthAttachment->format);
		attchmentDescription.samples = frameBufferData->sampleCount;
		attchmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attchmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attchmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attchmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attchmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attchmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		attachmentDescriptions[createInfo->depthAttachment->index] = attchmentDescription;
		depthReference = { createInfo->depthAttachment->index, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
		frameBufferData->clearValues[createInfo->depthAttachment->index].depthStencil = { 1.0f, 0 };
	}

	// Multisample
	std::vector<VkAttachmentReference> attachReferenceResolves(createInfo->colorAttachmentCount);

	if (frameBufferData->multisampling)
	{
		uint32_t attachmentCount = attachmentDescriptions.size();

		for (uint32_t i = 0; i < createInfo->colorAttachmentCount; i++)
		{
			VkFormat colorFormat = vks::getVulkanColorFormatEnum(frameBufferData->colorAttachments[i].format);

			VkAttachmentDescription attachmentResolve{};
			attachmentResolve.format = colorFormat;
			attachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
			attachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachmentResolve.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			attachmentDescriptions.push_back(attachmentResolve);

			VkAttachmentReference attachResolve{};
			attachResolve.attachment = attachmentCount + i;
			attachResolve.layout = attachmentDescriptions[i].finalLayout;
			attachReferenceResolves[i] = attachResolve;
		}
	}

	VkSubpassDescription subpassDescription{};
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.colorAttachmentCount = colorReference.size();
	subpassDescription.pColorAttachments = colorReference.data();

	if (frameBufferData->hasDepth)
		subpassDescription.pDepthStencilAttachment = &depthReference;
	if (frameBufferData->multisampling)
		subpassDescription.pResolveAttachments = attachReferenceResolves.data();

	// Use subpass dependencies for layout transitions
	VkSubpassDependency dependencies[2];

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// create the renderpass framebuffer
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = attachmentDescriptions.size();
	renderPassInfo.pAttachments = attachmentDescriptions.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpassDescription;
	renderPassInfo.dependencyCount = ARRAY_LEN(dependencies);
	renderPassInfo.pDependencies = dependencies;

	VkRenderPass renderPass;
	if (vkCreateRenderPass(vkBackends->device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("[vulkan] failed to create render pass <VkRenderPass> (%p) when creating framebuffer at (%p)", renderPass, frameBuffer);
		return Lvn_Result_Failure;
	}
	frameBufferData->renderPass = renderPass;
	frameBufferData->frameBufferRenderPass.nativeRenderPass = renderPass;

	// create texture info
	VkSamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = filter;
	samplerCreateInfo.minFilter = filter;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = addressMode;
	samplerCreateInfo.addressModeV = addressMode;
	samplerCreateInfo.addressModeW = addressMode;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.maxAnisotropy = 1.0f;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 1.0f;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

	VkSampler sampler;
	if (vkCreateSampler(vkBackends->device, &samplerCreateInfo, nullptr, &sampler) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("[vulkan] failed to create sampler <VkSampler> (%p) when creating framebuffer at (%p)", sampler, frameBuffer);
		return Lvn_Result_Failure;
	}
	frameBufferData->sampler = sampler;

	// create actual framebuffer
	vks::createOffscreenFrameBuffer(vkBackends, frameBuffer);

	return Lvn_Result_Success;
}

LvnResult vksImplCreateBuffer(LvnBuffer* buffer, LvnBufferCreateInfo* createInfo)
{
	VulkanBackends* vkBackends = s_VkBackends;

	buffer->type = createInfo->type;
	buffer->vertexBufferSize = createInfo->vertexBufferSize;
	buffer->indexBufferSize = createInfo->indexBufferSize;

	bool vertexOnly = createInfo->type & (Lvn_BufferType_Index | Lvn_BufferType_DynamicIndex) ? false : true;

	bool dynamicVertex = false, dynamicIndex = false;

	if (createInfo->type & Lvn_BufferType_DynamicVertex)
		dynamicVertex = true;
	if (createInfo->type & Lvn_BufferType_DynamicIndex)
		dynamicIndex = true;

	if (dynamicVertex || dynamicIndex) // buffers need to be seperate if either buffer is dynamic
	{
		// dynamic buffers will have their memory stored on the cpu

		// vertex 
		VkBuffer vertexBuffer;
		VmaAllocation vertexMemory;

		vks::createBuffer(vkBackends, &vertexBuffer, &vertexMemory, createInfo->vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

		if (createInfo->pVertices)
		{
			void* data;
			vmaMapMemory(vkBackends->vmaAllocator, vertexMemory, &data);
			memcpy(data, createInfo->pVertices, createInfo->vertexBufferSize);
			vmaUnmapMemory(vkBackends->vmaAllocator, vertexMemory);
		}

		buffer->vertexBuffer = vertexBuffer;
		buffer->vertexBufferMemory = vertexMemory;

		// index
		if (!vertexOnly)
		{
			VkBuffer indexBuffer;
			VmaAllocation indexMemory;

			vks::createBuffer(vkBackends, &indexBuffer, &indexMemory, createInfo->indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

			if (createInfo->pIndices)
			{
				void* data;
				vmaMapMemory(vkBackends->vmaAllocator, indexMemory, &data);
				memcpy(data, createInfo->pIndices, createInfo->indexBufferSize);
				vmaUnmapMemory(vkBackends->vmaAllocator, indexMemory);
			}

			buffer->indexBuffer = indexBuffer;
			buffer->indexBufferMemory = indexMemory;
		}

		buffer->indexOffset = 0;
	}
	else
	{
		VkDeviceSize bufferSize = createInfo->vertexBufferSize + createInfo->indexBufferSize;

		VkBuffer stagingBuffer;
		VmaAllocation stagingMemory;

		VkBuffer vkBuffer;
		VmaAllocation bufferMemory;

		// create staging buffer to pass vertex data into
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

		// create the main buffer to be used
		VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		vks::createBuffer(vkBackends, &vkBuffer, &bufferMemory, bufferSize, bufferUsage, VMA_MEMORY_USAGE_GPU_ONLY);
		vks::copyBuffer(vkBackends, stagingBuffer, vkBuffer, bufferSize, 0, 0);

		vkDestroyBuffer(vkBackends->device, stagingBuffer, nullptr);
		vmaFreeMemory(vkBackends->vmaAllocator, stagingMemory);

		// copy data to buffer object
		buffer->vertexBuffer = vkBuffer;
		buffer->vertexBufferMemory = bufferMemory;
		buffer->indexBuffer = vkBuffer;
		buffer->indexBufferMemory = bufferMemory;
		buffer->indexOffset = createInfo->vertexBufferSize;
	}


	return Lvn_Result_Success;
}

LvnResult vksImplCreateUniformBuffer(LvnUniformBuffer* uniformBuffer, LvnUniformBufferCreateInfo* createInfo)
{
	VulkanBackends* vkBackends = s_VkBackends;

	VkBuffer vkUniformBuffer;
	VmaAllocation uniformBufferMemory;

	VkBufferUsageFlags bufferUsageType = vks::getUniformBufferTypeEnum(createInfo->type);
	vks::createBuffer(vkBackends, &vkUniformBuffer, &uniformBufferMemory, createInfo->size * vkBackends->maxFramesInFlight, bufferUsageType, VMA_MEMORY_USAGE_CPU_ONLY);

	uniformBuffer->uniformBuffer = vkUniformBuffer;
	uniformBuffer->uniformBufferMemory = uniformBufferMemory;
	uniformBuffer->size = createInfo->size;

	vmaMapMemory(vkBackends->vmaAllocator, uniformBufferMemory, &uniformBuffer->uniformBufferMapped);

	return Lvn_Result_Success;
}

LvnResult vksImplCreateSampler(LvnSampler* sampler, LvnSamplerCreateInfo* createInfo)
{
	VulkanBackends* vkBackends = s_VkBackends;

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.minFilter = vks::getTextureFilterEnum(createInfo->minFilter);
	samplerInfo.magFilter = vks::getTextureFilterEnum(createInfo->magFilter);
	samplerInfo.addressModeU = vks::getTextureWrapModeEnum(createInfo->wrapS);
	samplerInfo.addressModeV = vks::getTextureWrapModeEnum(createInfo->wrapT);
	samplerInfo.addressModeW = vks::getTextureWrapModeEnum(createInfo->wrapT);

	if (vkBackends->deviceSupportedFeatures.samplerAnisotropy)
	{
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = vkBackends->deviceProperties.limits.maxSamplerAnisotropy;
	}
	else
	{
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1.0f;
	}

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	VkSampler textureSampler;

	if (vkCreateSampler(vkBackends->device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("[vulkan] failed to create texture sampler <VkSampler> (%p)", textureSampler);
		return Lvn_Result_Failure;
	}

	sampler->sampler = textureSampler;

	return Lvn_Result_Success;
}

LvnResult vksImplCreateTexture(LvnTexture* texture, LvnTextureCreateInfo* createInfo)
{
	VulkanBackends* vkBackends = s_VkBackends;

	VkBuffer stagingBuffer;
	VmaAllocation stagingBufferMemory;
	VkDeviceSize imageSize = createInfo->imageData.pixels.memsize();

	vks::createBuffer(vkBackends, &stagingBuffer, &stagingBufferMemory, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	void* data;
	vmaMapMemory(vkBackends->vmaAllocator, stagingBufferMemory, &data);
	memcpy(data, createInfo->imageData.pixels.data(), imageSize);
	vmaUnmapMemory(vkBackends->vmaAllocator, stagingBufferMemory);

	VkFormat format = createInfo->format == Lvn_TextureFormat_Unorm ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8A8_SRGB;
	switch (createInfo->imageData.channels)
	{
		case 1: { format = createInfo->format == Lvn_TextureFormat_Unorm ? VK_FORMAT_R8_UNORM : VK_FORMAT_R8_SRGB; break; }
		case 2: { format = createInfo->format == Lvn_TextureFormat_Unorm ? VK_FORMAT_R8G8_UNORM : VK_FORMAT_R8G8_SRGB; break; }
		case 4: { format = createInfo->format == Lvn_TextureFormat_Unorm ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8A8_SRGB; break; }
	}

	// create texture image
	VkImage textureImage;
	VmaAllocation textureImageMemory;

	if (vks::createImage(vkBackends, 
		&textureImage,
		&textureImageMemory,
		createInfo->imageData.width,
		createInfo->imageData.height,
		format,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_SAMPLE_COUNT_1_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[vulkan] failed to create texture image <VkImage> for texture (%p)", texture);
		return Lvn_Result_Failure;
	}

	// transition buffer to image
	vks::transitionImageLayout(vkBackends, textureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);
	vks::copyBufferToImage(vkBackends, stagingBuffer, textureImage, createInfo->imageData.width, createInfo->imageData.height, 1);
	vks::transitionImageLayout(vkBackends, textureImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);


	// texture image view
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = textureImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(vkBackends->device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("[vulkan] failed to create texture image view <VkImageView> for texture (%p)", texture);
		return Lvn_Result_Failure;
	}

	// sampler
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.minFilter = vks::getTextureFilterEnum(createInfo->minFilter);
	samplerInfo.magFilter = vks::getTextureFilterEnum(createInfo->magFilter);
	samplerInfo.addressModeU = vks::getTextureWrapModeEnum(createInfo->wrapS);
	samplerInfo.addressModeV = vks::getTextureWrapModeEnum(createInfo->wrapT);
	samplerInfo.addressModeW = vks::getTextureWrapModeEnum(createInfo->wrapT);

	if (vkBackends->deviceSupportedFeatures.samplerAnisotropy)
	{
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = vkBackends->deviceProperties.limits.maxSamplerAnisotropy;
	}
	else
	{
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1.0f;
	}

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	VkSampler textureSampler;
	if (vkCreateSampler(vkBackends->device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("[vulkan] failed to create texture sampler <VkSampler> (%p)", textureSampler);
		return Lvn_Result_Failure;
	}

	texture->image = textureImage;
	texture->imageMemory = textureImageMemory;
	texture->imageView = imageView;
	texture->sampler = textureSampler;
	texture->width = createInfo->imageData.width;
	texture->height = createInfo->imageData.height;
	texture->seperateSampler = false;

	vkDestroyBuffer(vkBackends->device, stagingBuffer, nullptr);
	vmaFreeMemory(vkBackends->vmaAllocator, stagingBufferMemory);

	return Lvn_Result_Success;
}

LvnResult vksImplCreateTextureSampler(LvnTexture* texture, LvnTextureSamplerCreateInfo* createInfo)
{
	VulkanBackends* vkBackends = s_VkBackends;

	VkBuffer stagingBuffer;
	VmaAllocation stagingBufferMemory;
	VkDeviceSize imageSize = createInfo->imageData.pixels.memsize();

	vks::createBuffer(vkBackends, &stagingBuffer, &stagingBufferMemory, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	void* data;
	vmaMapMemory(vkBackends->vmaAllocator, stagingBufferMemory, &data);
	memcpy(data, createInfo->imageData.pixels.data(), imageSize);
	vmaUnmapMemory(vkBackends->vmaAllocator, stagingBufferMemory);

	VkFormat format = createInfo->format == Lvn_TextureFormat_Unorm ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8A8_SRGB;
	switch (createInfo->imageData.channels)
	{
		case 1: { format = createInfo->format == Lvn_TextureFormat_Unorm ? VK_FORMAT_R8_UNORM : VK_FORMAT_R8_SRGB; break; }
		case 2: { format = createInfo->format == Lvn_TextureFormat_Unorm ? VK_FORMAT_R8G8_UNORM : VK_FORMAT_R8G8_SRGB; break; }
		case 4: { format = createInfo->format == Lvn_TextureFormat_Unorm ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8A8_SRGB; break; }
	}

	// create texture image
	VkImage textureImage;
	VmaAllocation textureImageMemory;

	if (vks::createImage(vkBackends,
		&textureImage,
		&textureImageMemory,
		createInfo->imageData.width,
		createInfo->imageData.height,
		format,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_SAMPLE_COUNT_1_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[vulkan] failed to create texture image <VkImage> for texture (%p)", texture);
		return Lvn_Result_Failure;
	}

	// transition buffer to image
	vks::transitionImageLayout(vkBackends, textureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);
	vks::copyBufferToImage(vkBackends, stagingBuffer, textureImage, createInfo->imageData.width, createInfo->imageData.height, 1);
	vks::transitionImageLayout(vkBackends, textureImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);


	// texture image view
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = textureImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(vkBackends->device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("[vulkan] failed to create texture image view <VkImageView> for texture (%p)", texture);
		return Lvn_Result_Failure;
	}

	texture->image = textureImage;
	texture->imageMemory = textureImageMemory;
	texture->imageView = imageView;
	texture->sampler = createInfo->sampler->sampler;
	texture->width = createInfo->imageData.width;
	texture->height = createInfo->imageData.height;
	texture->seperateSampler = true;

	vkDestroyBuffer(vkBackends->device, stagingBuffer, nullptr);
	vmaFreeMemory(vkBackends->vmaAllocator, stagingBufferMemory);

	return Lvn_Result_Success;
}

LvnResult vksImplCreateCubemap(LvnCubemap* cubemap, LvnCubemapCreateInfo* createInfo)
{
	VulkanBackends* vkBackends = getVulkanBackends();
	VmaAllocator vmaAllocator = vkBackends->vmaAllocator;

	uint32_t imageWidth = createInfo->posx.width; // Note that all images should have the same width
	uint32_t imageHeight = createInfo->posx.height; // Note that all images should have the same height

	VkDeviceSize layerSize = createInfo->posx.size; // size of image of one side of cubemap, Note that all images should have the same size
	VkDeviceSize imageSize = layerSize * 6; // size of total combinded 6 images of cubemap

	const uint8_t* texImages[6] = { createInfo->posx.pixels.data(), createInfo->negx.pixels.data(), createInfo->posy.pixels.data(), createInfo->negy.pixels.data(), createInfo->posz.pixels.data(), createInfo->negz.pixels.data() };
	std::vector<uint8_t> texData(imageSize);

	for (uint32_t i = 0; i < 6; i++)
	{
		memcpy(texData.data() + layerSize * i, texImages[i], layerSize);
	}

	VkBuffer stagingBuffer;
	VmaAllocation stagingBufferMemory;

	vks::createBuffer(vkBackends, &stagingBuffer, &stagingBufferMemory, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	void* bufferData;
	vmaMapMemory(vmaAllocator, stagingBufferMemory, &bufferData);
	memcpy(bufferData, texData.data(), imageSize);
	vmaUnmapMemory(vmaAllocator, stagingBufferMemory);

	VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = imageWidth;
	imageInfo.extent.height = imageHeight;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 6; // cubemap has 6 sides
	imageInfo.format = format;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

	VkImage cubemapImage;
	VmaAllocation cubemapImageMemory;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	if (vmaCreateImage(vmaAllocator, &imageInfo, &allocInfo, &cubemapImage, &cubemapImageMemory, nullptr) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("failed to create image <VkImage> when creating cubemap at (%p)", cubemap);
		return Lvn_Result_Failure;
	}

	vks::transitionImageLayout(vkBackends, cubemapImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 6);
	vks::copyBufferToImage(vkBackends, stagingBuffer, cubemapImage, imageWidth, imageHeight, 6);

	vks::transitionImageLayout(vkBackends, cubemapImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 6);

	vkDestroyBuffer(vkBackends->device, stagingBuffer, nullptr);
	vmaFreeMemory(vmaAllocator, stagingBufferMemory);

	// image view
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = cubemapImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 6;

	VkImageView cubemapImageView;
	if (vkCreateImageView(vkBackends->device, &viewInfo, nullptr, &cubemapImageView) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("failed to create texture image view <VkImageView> when creating cubemap at (%p)", cubemap);
		return Lvn_Result_Failure;
	}

	// sampler
	VkSamplerCreateInfo sampler{};
	sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler.magFilter = VK_FILTER_LINEAR;
	sampler.minFilter = VK_FILTER_LINEAR;
	sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler.compareOp = VK_COMPARE_OP_ALWAYS;
	sampler.mipLodBias = 0.0f;
	sampler.minLod = 0.0f;
	sampler.maxLod = 0.0f;
	sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	sampler.maxAnisotropy = 1.0f;
	if (vkBackends->deviceSupportedFeatures.samplerAnisotropy)
	{
		sampler.maxAnisotropy = vkBackends->deviceProperties.limits.maxSamplerAnisotropy;
		sampler.anisotropyEnable = VK_TRUE;
	}

	VkSampler cubemapSampler;
	if (vkCreateSampler(vkBackends->device, &sampler, nullptr, &cubemapSampler) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("failed to create image sampler when creating cubemap at (%p)", cubemap);
		return Lvn_Result_Failure;
	}

	LvnTexture cubemapTexture{};
	cubemapTexture.image = cubemapImage;
	cubemapTexture.imageView = cubemapImageView;
	cubemapTexture.imageMemory = cubemapImageMemory;
	cubemapTexture.sampler = cubemapSampler;

	cubemap->textureData = cubemapTexture;

	return Lvn_Result_Success;
}

LvnResult vksImplCreateCubemapHdr(LvnCubemap* cubemap, LvnCubemapHdrCreateInfo* createInfo)
{
	VulkanBackends* vkBackends = getVulkanBackends();
	VmaAllocator vmaAllocator = vkBackends->vmaAllocator;

	uint32_t imageWidth = createInfo->hdr.width;
	uint32_t imageHeight = createInfo->hdr.height;
	VkDeviceSize imageSize = createInfo->hdr.size;

	VkBuffer stagingBuffer;
	VmaAllocation stagingBufferMemory;

	vks::createBuffer(vkBackends, &stagingBuffer, &stagingBufferMemory, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	void* bufferData;
	vmaMapMemory(vmaAllocator, stagingBufferMemory, &bufferData);
	memcpy(bufferData, createInfo->hdr.pixels.data(), imageSize);
	vmaUnmapMemory(vmaAllocator, stagingBufferMemory);

	VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = imageWidth;
	imageInfo.extent.height = imageHeight;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 6; // cubemap has 6 sides
	imageInfo.format = format;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

	VkImage cubemapImage;
	VmaAllocation cubemapImageMemory;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	if (vmaCreateImage(vmaAllocator, &imageInfo, &allocInfo, &cubemapImage, &cubemapImageMemory, nullptr) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("failed to create image <VkImage> when creating cubemap at (%p)", cubemap);
		return Lvn_Result_Failure;
	}

	vks::transitionImageLayout(vkBackends, cubemapImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 6);
	vks::copyBufferToImage(vkBackends, stagingBuffer, cubemapImage, imageWidth, imageHeight, 6);

	vks::transitionImageLayout(vkBackends, cubemapImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 6);

	vkDestroyBuffer(vkBackends->device, stagingBuffer, nullptr);
	vmaFreeMemory(vmaAllocator, stagingBufferMemory);

	// image view
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = cubemapImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 6;

	VkImageView cubemapImageView;
	if (vkCreateImageView(vkBackends->device, &viewInfo, nullptr, &cubemapImageView) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("failed to create texture image view <VkImageView> when creating cubemap at (%p)", cubemap);
		return Lvn_Result_Failure;
	}

	// sampler
	VkSamplerCreateInfo sampler{};
	sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler.magFilter = VK_FILTER_LINEAR;
	sampler.minFilter = VK_FILTER_LINEAR;
	sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler.compareOp = VK_COMPARE_OP_ALWAYS;
	sampler.mipLodBias = 0.0f;
	sampler.minLod = 0.0f;
	sampler.maxLod = 0.0f;
	sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	sampler.maxAnisotropy = 1.0f;
	if (vkBackends->deviceSupportedFeatures.samplerAnisotropy)
	{
		sampler.maxAnisotropy = vkBackends->deviceProperties.limits.maxSamplerAnisotropy;
		sampler.anisotropyEnable = VK_TRUE;
	}

	VkSampler cubemapSampler;
	if (vkCreateSampler(vkBackends->device, &sampler, nullptr, &cubemapSampler) != VK_SUCCESS)
	{
		LVN_CORE_ERROR("failed to create image sampler when creating cubemap at (%p)", cubemap);
		return Lvn_Result_Failure;
	}

	LvnTexture cubemapTexture{};
	cubemapTexture.image = cubemapImage;
	cubemapTexture.imageView = cubemapImageView;
	cubemapTexture.imageMemory = cubemapImageMemory;
	cubemapTexture.sampler = cubemapSampler;

	cubemap->textureData = cubemapTexture;

	return Lvn_Result_Success;
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
	VulkanBackends* vkBackends = s_VkBackends;
	vkDeviceWaitIdle(vkBackends->device);

	VulkanFrameBufferData* frameBufferData = static_cast<VulkanFrameBufferData*>(frameBuffer->frameBufferData);

	for (uint32_t i = 0; i < frameBufferData->colorAttachments.size(); i++)
	{
		vkDestroyImage(vkBackends->device, frameBufferData->colorImages[i], nullptr);
		vkDestroyImageView(vkBackends->device, frameBufferData->colorImageViews[i], nullptr);
		vmaFreeMemory(vkBackends->vmaAllocator, frameBufferData->colorImageMemory[i]);
	}

	if (frameBufferData->hasDepth)
	{
		vkDestroyImage(vkBackends->device, frameBufferData->depthImage, nullptr);
		vkDestroyImageView(vkBackends->device, frameBufferData->depthImageView, nullptr);
		vmaFreeMemory(vkBackends->vmaAllocator, frameBufferData->depthImageMemory);
	}

	if (frameBufferData->multisampling)
	{
		for (uint32_t i = 0; i < frameBufferData->colorAttachments.size(); i++)
		{
			vkDestroyImage(vkBackends->device, frameBufferData->msaaColorImages[i], nullptr);
			vkDestroyImageView(vkBackends->device, frameBufferData->msaaColorImageViews[i], nullptr);
			vmaFreeMemory(vkBackends->vmaAllocator, frameBufferData->msaaColorImageMemory[i]);
		}
	}

	vkDestroyRenderPass(vkBackends->device, frameBufferData->renderPass, nullptr);
	vkDestroySampler(vkBackends->device, frameBufferData->sampler, nullptr);
	vkDestroyFramebuffer(vkBackends->device, frameBufferData->framebuffer, nullptr);

	delete frameBufferData;
}

void vksImplDestroyBuffer(LvnBuffer* buffer)
{
	VulkanBackends* vkBackends = s_VkBackends;
	vkDeviceWaitIdle(vkBackends->device);

	bool dynamic = buffer->type & (Lvn_BufferType_DynamicVertex | Lvn_BufferType_DynamicIndex);

	if (dynamic)
	{
		VkBuffer vertexBuffer = static_cast<VkBuffer>(buffer->vertexBuffer);
		VmaAllocation vertexMemory = static_cast<VmaAllocation>(buffer->vertexBufferMemory);
		VkBuffer indexBuffer = static_cast<VkBuffer>(buffer->indexBuffer);
		VmaAllocation indexMemory = static_cast<VmaAllocation>(buffer->indexBufferMemory);

		vkDestroyBuffer(vkBackends->device, vertexBuffer, nullptr);
		vmaFreeMemory(vkBackends->vmaAllocator, vertexMemory);
		vkDestroyBuffer(vkBackends->device, indexBuffer, nullptr);
		vmaFreeMemory(vkBackends->vmaAllocator, indexMemory);
	}
	else
	{
		VkBuffer vkBuffer = static_cast<VkBuffer>(buffer->vertexBuffer);
		VmaAllocation bufferMemory = static_cast<VmaAllocation>(buffer->vertexBufferMemory);

		vkDestroyBuffer(vkBackends->device, vkBuffer, nullptr);
		vmaFreeMemory(vkBackends->vmaAllocator, bufferMemory);
	}
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
}

void vksImplDestroySampler(LvnSampler* sampler)
{
	VulkanBackends* vkBackends = s_VkBackends;
	vkDeviceWaitIdle(vkBackends->device);

	VkSampler textureSampler = static_cast<VkSampler>(sampler->sampler);
	vkDestroySampler(vkBackends->device, textureSampler, nullptr);
}

void vksImplDestroyTexture(LvnTexture* texture)
{
	VulkanBackends* vkBackends = s_VkBackends;
	vkDeviceWaitIdle(vkBackends->device);

	VkImage image = static_cast<VkImage>(texture->image);
	VmaAllocation imageMemory = static_cast<VmaAllocation>(texture->imageMemory);
	VkImageView imageView = static_cast<VkImageView>(texture->imageView);

	vkDestroyImage(vkBackends->device, image, nullptr);
	vmaFreeMemory(vkBackends->vmaAllocator, imageMemory);;
	vkDestroyImageView(vkBackends->device, imageView, nullptr);

	if (!texture->seperateSampler)
	{
		VkSampler textureSampler = static_cast<VkSampler>(texture->sampler);
		vkDestroySampler(vkBackends->device, textureSampler, nullptr);
	}
}

void vksImplDestroyCubemap(LvnCubemap* cubemap)
{
	VulkanBackends* vkBackends = s_VkBackends;
	vkDeviceWaitIdle(vkBackends->device);

	LvnTexture* texture = &cubemap->textureData;

	VkImage image = static_cast<VkImage>(texture->image);
	VmaAllocation imageMemory = static_cast<VmaAllocation>(texture->imageMemory);
	VkImageView imageView = static_cast<VkImageView>(texture->imageView);
	VkSampler textureSampler = static_cast<VkSampler>(texture->sampler);

	vkDestroyImage(vkBackends->device, image, nullptr);
	vmaFreeMemory(vkBackends->vmaAllocator, imageMemory);;
	vkDestroyImageView(vkBackends->device, imageView, nullptr);
	vkDestroySampler(vkBackends->device, textureSampler, nullptr);
}

void vksImplBufferUpdateVertexData(LvnBuffer* buffer, void* vertices, uint64_t size, uint64_t offset)
{
	LVN_CORE_ASSERT(buffer->type & Lvn_BufferType_DynamicVertex, "[vulkan] cannot change vertex data of buffer that does not have dynamic vertex buffer type set (Lvn_BufferType_DynamicVertex)");

	VulkanBackends* vkBackends = s_VkBackends;
	VmaAllocator vmaAllocator = vkBackends->vmaAllocator;
	VmaAllocation vertexMemory = static_cast<VmaAllocation>(buffer->vertexBufferMemory);

	void* data;
	vmaMapMemory(vmaAllocator, vertexMemory, &data);
	memcpy((char*)data + offset, vertices, size);
	vmaUnmapMemory(vmaAllocator, vertexMemory);
}

void vksImplBufferUpdateIndexData(LvnBuffer* buffer, uint32_t* indices, uint64_t size, uint64_t offset)
{
	LVN_CORE_ASSERT(buffer->type & Lvn_BufferType_DynamicIndex, "[vulkan] cannot change index data of buffer that does not have dynamic index buffer type set (Lvn_BufferType_DynamicIndex)");

	VulkanBackends* vkBackends = s_VkBackends;
	VmaAllocator vmaAllocator = vkBackends->vmaAllocator;
	VmaAllocation indexMemory = static_cast<VmaAllocation>(buffer->indexBufferMemory);

	void* data;
	vmaMapMemory(vmaAllocator, indexMemory, &data);
	memcpy((char*)data + offset, indices, size);
	vmaUnmapMemory(vmaAllocator, indexMemory);

}

void vksImplBufferResizeVertexBuffer(LvnBuffer* buffer, uint64_t size)
{
	LVN_CORE_ASSERT(buffer->type & Lvn_BufferType_DynamicVertex, "[vulkan] cannot change vertex data of buffer that does not have dynamic vertex buffer type set (Lvn_BufferType_DynamicVertex)");

	VulkanBackends* vkBackends = s_VkBackends;
	VmaAllocator vmaAllocator = vkBackends->vmaAllocator;

	VkBuffer vertexBuffer = static_cast<VkBuffer>(buffer->vertexBuffer);
	VmaAllocation vertexMemory = static_cast<VmaAllocation>(buffer->vertexBufferMemory);

	vkDestroyBuffer(vkBackends->device, vertexBuffer, nullptr);
	vmaFreeMemory(vmaAllocator, vertexMemory);

	vks::createBuffer(vkBackends, &vertexBuffer, &vertexMemory, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	buffer->vertexBuffer = vertexBuffer;
	buffer->vertexBufferMemory = vertexMemory;
}

void vksImplBufferResizeIndexBuffer(LvnBuffer* buffer, uint64_t size)
{
	LVN_CORE_ASSERT(buffer->type & Lvn_BufferType_DynamicIndex, "[vulkan] cannot change index data of buffer that does not have dynamic index buffer type set (Lvn_BufferType_DynamicIndex)");

	VulkanBackends* vkBackends = s_VkBackends;
	VmaAllocator vmaAllocator = vkBackends->vmaAllocator;

	VkBuffer indexBuffer = static_cast<VkBuffer>(buffer->indexBuffer);
	VmaAllocation indexMemory = static_cast<VmaAllocation>(buffer->indexBufferMemory);

	vkDestroyBuffer(vkBackends->device, indexBuffer, nullptr);
	vmaFreeMemory(vmaAllocator, indexMemory);

	vks::createBuffer(vkBackends, &indexBuffer, &indexMemory, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	buffer->indexBuffer = indexBuffer;
	buffer->indexBufferMemory = indexMemory;
}

void vksImplUpdateUniformBufferData(LvnWindow* window, LvnUniformBuffer* uniformBuffer, void* data, uint64_t size, uint64_t offset)
{
	VulkanBackends* vkBackends = s_VkBackends;
	VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);

	for (uint32_t i = 0; i < vkBackends->maxFramesInFlight; i++)
	{
		memcpy(static_cast<char*>(uniformBuffer->uniformBufferMapped) + offset + uniformBuffer->size * i, data, size);
	}
}

void vksImplUpdateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count)
{
	VulkanBackends* vkBackends = s_VkBackends;
	VkDescriptorSet* descriptorSets = (VkDescriptorSet*)descriptorSet->descriptorSets.data();

	vkDeviceWaitIdle(vkBackends->device);

	for (uint32_t i = 0; i < count; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		std::vector<VkDescriptorImageInfo> imageInfos(pUpdateInfo[i].descriptorCount);

		if (pUpdateInfo[i].descriptorType == Lvn_DescriptorType_ImageSampler ||
			pUpdateInfo[i].descriptorType == Lvn_DescriptorType_ImageSamplerBindless)
		{
			for (uint32_t j = 0; j < pUpdateInfo[i].descriptorCount; j++)
			{
				imageInfos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfos[j].imageView = static_cast<VkImageView>(pUpdateInfo[i].pTextureInfos[j]->imageView);
				imageInfos[j].sampler = static_cast<VkSampler>(pUpdateInfo[i].pTextureInfos[j]->sampler);
			}
		}

		for (uint32_t j = 0; j < vkBackends->maxFramesInFlight; j++)
		{
			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[j]; // update descriptor set for each frame in flight
			descriptorWrite.dstBinding = pUpdateInfo[i].binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = vks::getDescriptorTypeEnum(pUpdateInfo[i].descriptorType);
			descriptorWrite.descriptorCount = pUpdateInfo[i].descriptorCount;

			// if descriptor using uniform buffers
			if (pUpdateInfo[i].descriptorType == Lvn_DescriptorType_UniformBuffer || pUpdateInfo[i].descriptorType == Lvn_DescriptorType_StorageBuffer)
			{
				bufferInfo.buffer = static_cast<VkBuffer>(pUpdateInfo[i].bufferInfo->buffer->uniformBuffer);
				bufferInfo.offset = pUpdateInfo[i].bufferInfo->offset + pUpdateInfo[i].bufferInfo->buffer->size * j; // offset buffer size for each frame in flight
				bufferInfo.range = pUpdateInfo[i].bufferInfo->range;
				descriptorWrite.pBufferInfo = &bufferInfo;
			}

			// if descriptor using textures
			else if (pUpdateInfo[i].descriptorType == Lvn_DescriptorType_ImageSampler ||
				pUpdateInfo[i].descriptorType == Lvn_DescriptorType_ImageSamplerBindless)
			{
				descriptorWrite.pImageInfo = imageInfos.data();
			}

			vkUpdateDescriptorSets(vkBackends->device, 1, &descriptorWrite, 0, nullptr);
		}
	}
}

LvnTexture* vksImplFrameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex)
{
	VulkanFrameBufferData* frameBufferData = static_cast<VulkanFrameBufferData*>(frameBuffer->frameBufferData);
	LVN_CORE_ASSERT(attachmentIndex < frameBufferData->totalAttachmentCount, "attachment index out of range, cannot have an attachment index (%u) greater or equal to the total attachment count (%u) within framebuffer (%p)", attachmentIndex, frameBufferData->totalAttachmentCount, frameBuffer);

	return &frameBufferData->frameBufferImages[attachmentIndex];
}

LvnRenderPass* vksImplFrameBufferGetRenderPass(LvnFrameBuffer* frameBuffer)
{
	VulkanFrameBufferData* frameBufferData = static_cast<VulkanFrameBufferData*>(frameBuffer->frameBufferData);
	return &frameBufferData->frameBufferRenderPass;
}

void vksImplFrameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height)
{
	VulkanBackends* vkBackends = s_VkBackends;
	vkDeviceWaitIdle(vkBackends->device);

	VulkanFrameBufferData* frameBufferData = static_cast<VulkanFrameBufferData*>(frameBuffer->frameBufferData);

	for (uint32_t i = 0; i < frameBufferData->colorAttachments.size(); i++)
	{
		vkDestroyImage(vkBackends->device, frameBufferData->colorImages[i], nullptr);
		vkDestroyImageView(vkBackends->device, frameBufferData->colorImageViews[i], nullptr);
		vmaFreeMemory(vkBackends->vmaAllocator, frameBufferData->colorImageMemory[i]);
	}

	if (frameBufferData->hasDepth)
	{
		vkDestroyImage(vkBackends->device, frameBufferData->depthImage, nullptr);
		vkDestroyImageView(vkBackends->device, frameBufferData->depthImageView, nullptr);
		vmaFreeMemory(vkBackends->vmaAllocator, frameBufferData->depthImageMemory);
	}

	if (frameBufferData->multisampling)
	{
		for (uint32_t i = 0; i < frameBufferData->colorAttachments.size(); i++)
		{
			vkDestroyImage(vkBackends->device, frameBufferData->msaaColorImages[i], nullptr);
			vkDestroyImageView(vkBackends->device, frameBufferData->msaaColorImageViews[i], nullptr);
			vmaFreeMemory(vkBackends->vmaAllocator, frameBufferData->msaaColorImageMemory[i]);
		}
	}

	vkDestroyFramebuffer(vkBackends->device, frameBufferData->framebuffer, nullptr);

	frameBufferData->width = width;
	frameBufferData->height = height;
	vks::createOffscreenFrameBuffer(vkBackends, frameBuffer);
}

void vksImplFrameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a)
{
	VulkanFrameBufferData* frameBufferData = static_cast<VulkanFrameBufferData*>(frameBuffer->frameBufferData);
	LVN_CORE_ASSERT(attachmentIndex < frameBufferData->totalAttachmentCount, "attachment index out of range, cannot have an attachment index (%u) greater or equal to the total attachment count (%u) within framebuffer (%p)", attachmentIndex, frameBufferData->totalAttachmentCount, frameBuffer);

	frameBufferData->clearValues[attachmentIndex].color = {{ r, g, b, a }};
}

LvnDepthImageFormat vksImplFindSupportedDepthImageFormat(LvnDepthImageFormat* pDepthImageFormats, uint32_t count)
{
	VulkanBackends* vkBackends = s_VkBackends;
	
	VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
	VkFormatFeatureFlagBits features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

	std::vector<VkFormat> candidates(count);

	for (uint32_t i = 0; i < count; i++)
	{
		candidates[i] = vks::getVulkanDepthFormatEnum(pDepthImageFormats[i]);
	}

	for (uint32_t i = 0; i < count; i++)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(vkBackends->physicalDevice, candidates[i], &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
		{
			return pDepthImageFormats[i];
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
		{
			return pDepthImageFormats[i];
		}
	}

	LVN_CORE_ERROR("failed to find supported depth format");
	return pDepthImageFormats[0];
}

} /* namespace lvn */
