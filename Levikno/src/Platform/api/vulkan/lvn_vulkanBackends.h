#ifndef HG_LEVIKNO_VULKAN_BACKENDS_H
#define HG_LEVIKNO_VULKAN_BACKENDS_H

#include <vulkan/vulkan.h>

namespace lvn
{
	struct VulkanBackends
	{
		bool						enableValidationLayers;
		VkInstance					instance;
		VkDebugUtilsMessengerEXT	debugMessenger;
		VkPhysicalDevice			physicalDevice;
		VkPhysicalDevice*			pPhysicalDevices;
		uint32_t					physicalDeviceCount;
	};
}

#endif