#include "lvn_vulkan.h"

#include "Platform/Window/glfw/lvn_glfw.h"
#include "lvn_vulkanBackends.h"

#include <vulkan/vulkan.h>
#include <vector>

#ifdef LVN_DEBUG
	static const bool s_EnableValidationLayers = true;
#else
	static const bool s_EnableValidationLayers = false;
#endif

static const char* s_ValidationLayers[1] =
{
	"VK_LAYER_KHRONOS_validation"
};

namespace lvn
{
	static VulkanBackends* s_VkBackends = nullptr;

	namespace vks
	{
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
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

		bool checkValidationLayerSupport()
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

		std::vector<const char*> getRequiredExtensions()
		{
			uint32_t glfwExtensionCount = 0;
			const char** glfwExtensions;
			glfwExtensions = glfwImplGetInstanceExtensions(&glfwExtensionCount);

			std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

			if (s_EnableValidationLayers)
				extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

			return extensions;
		}
		
		VkResult createDebugUtilsMessengerEXT(
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

		void destroyDebugUtilsMessengerEXT(
			VkInstance instance, 
			VkDebugUtilsMessengerEXT debugMessenger, 
			const VkAllocationCallbacks* pAllocator)
		{
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
			if (func != nullptr)
				func(instance, debugMessenger, pAllocator);
		}

		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
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

		void setupDebugMessenger(VulkanBackends* vkBackends)
		{
			if (!vkBackends->enableValidationLayers) return;

			VkDebugUtilsMessengerCreateInfoEXT createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createInfo.messageSeverity = 
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT	|
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			createInfo.messageType = 
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT		| 
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  | 
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			createInfo.pfnUserCallback = debugCallback;
			createInfo.pUserData = nullptr;

			populateDebugMessengerCreateInfo(createInfo);

			LVN_CORE_ASSERT(createDebugUtilsMessengerEXT(vkBackends->instance, &createInfo, nullptr, &vkBackends->debugMessenger) == VK_SUCCESS, "vulkan - failed to set up debug messenger!");
		}

		void createInstance(VulkanBackends* vkBackends)
		{
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

			std::vector<const char*> extensions = vks::getRequiredExtensions();
			createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			createInfo.ppEnabledExtensionNames = extensions.data();

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
			if (s_EnableValidationLayers)
			{
				createInfo.enabledLayerCount = 1;
				createInfo.ppEnabledLayerNames = s_ValidationLayers;

				vks::populateDebugMessengerCreateInfo(debugCreateInfo);
				createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
			}
			else
			{
				createInfo.enabledLayerCount = 0;
				createInfo.pNext = nullptr;
			}

			LVN_CORE_ASSERT(vkCreateInstance(&createInfo, nullptr, &vkBackends->instance) == VK_SUCCESS, "vulkan - failed to create instance!");
		}

		PhysicalDeviceType getPhysicalDeviceTypeEnum(VkPhysicalDeviceType type)
		{
			switch (type)
			{
				case VK_PHYSICAL_DEVICE_TYPE_OTHER:			 { return PhysicalDeviceType::Other; }
				case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: { return PhysicalDeviceType::Integrated_GPU; }
				case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:	 { return PhysicalDeviceType::Discrete_GPU; }
				case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:	 { return PhysicalDeviceType::Virtual_GPU; }
				case VK_PHYSICAL_DEVICE_TYPE_CPU:			 { return PhysicalDeviceType::CPU; }
				default: { break; }
			}

			LVN_CORE_WARN("vulkan - No physical device matches type");
			return PhysicalDeviceType::Other;
		}

		void pickPhysicalDevice(VulkanBackends* vkBackends)
		{
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(vkBackends->instance, &deviceCount, nullptr);

			if (deviceCount == 0)
			{
				LVN_CORE_ASSERT(false, "vulkan - failed to find GPU with Vulkan support!");
				return;
			}
			
			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(vkBackends->instance, &deviceCount, devices.data());

			VkPhysicalDeviceProperties deviceProperties;
			for (const auto& device : devices)
			{
				vkGetPhysicalDeviceProperties(device, &deviceProperties);
			}
		}

		void getPhysicalDevices(VulkanBackends* vkBackends)
		{
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(vkBackends->instance, &deviceCount, nullptr);

			if (deviceCount == 0)
			{
				LVN_CORE_ASSERT(false, "vulkan - failed to find Physical Devices with Vulkan support!");
				return;
			}

			VkPhysicalDevice* devices = static_cast<VkPhysicalDevice*>(malloc(deviceCount * sizeof(VkPhysicalDevice)));
			vkEnumeratePhysicalDevices(vkBackends->instance, &deviceCount, devices);

			vkBackends->pPhysicalDevices = devices;
			vkBackends->physicalDeviceCount = deviceCount;
		}
	}

	void vksImplCreateContext(GraphicsContext* graphicsContext, RendererBackends* renderBackends)
	{
		VulkanBackends* vkBackends = new VulkanBackends();
		vkBackends->enableValidationLayers = renderBackends->enableValidationLayers;

		if (s_EnableValidationLayers)
			LVN_CORE_ASSERT(vks::checkValidationLayerSupport(), "vulkan - validation layers enabled, but not available!");

		vks::createInstance(vkBackends);
		vks::setupDebugMessenger(vkBackends);
		vks::getPhysicalDevices(vkBackends);

		s_VkBackends = vkBackends;
	}

	void vksImplTerminateContext()
	{
		VulkanBackends& vkBackends = *s_VkBackends;

		if (s_EnableValidationLayers)
			vks::destroyDebugUtilsMessengerEXT(vkBackends.instance, vkBackends.debugMessenger, nullptr);
		if (vkBackends.pPhysicalDevices || vkBackends.physicalDeviceCount)
			free(vkBackends.pPhysicalDevices);

		vkDestroyInstance(vkBackends.instance, nullptr);

		delete s_VkBackends;
	}

	void vksImplGetPhysicalDevices(PhysicalDevice* pPhysicalDevices, uint32_t* deviceCount)
	{
		LVN_CORE_ASSERT(s_VkBackends != nullptr, "vulkan - Vulkan Instance not initiated!");
		VulkanBackends& vkBackends = *s_VkBackends;

		*deviceCount = vkBackends.physicalDeviceCount;
		if (vkBackends.physicalDeviceCount == 0)
		{
			LVN_CORE_WARN("vulkan - no physical devices found that support vulkan.");
			return;
		}

		if (pPhysicalDevices == nullptr)
			return;

		for (uint32_t i = 0; i < vkBackends.physicalDeviceCount; i++)
		{
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(vkBackends.pPhysicalDevices[i], &deviceProperties);

			PhysicalDeviceInfo deviceInfo{};
			deviceInfo.type = vks::getPhysicalDeviceTypeEnum(deviceProperties.deviceType);
			deviceInfo.name = deviceProperties.deviceName;
			deviceInfo.apiVersion = deviceProperties.apiVersion;
			deviceInfo.driverVersion = deviceProperties.driverVersion;

			pPhysicalDevices[i].info = deviceInfo;
			pPhysicalDevices[i].device = vkBackends.pPhysicalDevices[i];
		}
	}
}