#ifndef HG_LEVIKNO_VULKAN_BACKENDS_H
#define HG_LEVIKNO_VULKAN_BACKENDS_H

#include <vulkan/vulkan.h>

namespace lvn
{
	struct VulkanQueueFamilyIndices
	{
		uint32_t graphicsIndex, presentIndex;
		bool has_graphics, has_present;
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		LvnVector<VkSurfaceFormatKHR> formats;
		LvnVector<VkPresentModeKHR> presentModes;
	};

	struct VulkanBackends
	{
		bool						enableValidationLayers;
		VkInstance					instance;
		VkDebugUtilsMessengerEXT	debugMessenger;
		VkPhysicalDevice			physicalDevice;
		VkPhysicalDevice*			pPhysicalDevices;
		uint32_t					physicalDeviceCount;
		VkDevice					device;
		VkQueue						graphicsQueue;
		VkSurfaceKHR				surface;
		VkQueue						presentQueue;

		VkSwapchainKHR				swapChain;
		VkImage*					swapChainImages;
		uint32_t					swapChainImageCount;
		VkFormat					swapChainImageFormat;
		VkExtent2D					swapChainExtent;
		VkImageView*				swapChainImageViews;
		uint32_t					swapChainImageViewCount;
	};
}

#endif