#ifndef HG_LEVIKNO_VULKAN_BACKENDS_H
#define HG_LEVIKNO_VULKAN_BACKENDS_H

#include <vulkan/vulkan.h>

#include "levikno_internal.h"

namespace lvn
{
	struct VulkanQueueFamilyIndices
	{
		uint32_t graphicsIndex, presentIndex;
		bool has_graphics, has_present;
	};

	struct VulkanSwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		LvnVector<VkSurfaceFormatKHR> formats;
		LvnVector<VkPresentModeKHR> presentModes;
	};

	struct VulkanWindowSurfaceData
	{
		VkSurfaceKHR surface;
		VkSwapchainKHR swapChain;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		VkImage* swapChainImages;
		VkImageView* swapChainImageViews;
		uint32_t swapChainImageCount;
		uint32_t swapChainImageViewCount;
	};

	struct VulkanPipelineCreateData
	{
		LvnPipelineSpecification* pipelineSpecification;
		VkRenderPass renderPass;
		VkPipelineVertexInputStateCreateInfo vertexInputInfo;
		VkExtent2D* swapChainExtent;
		VkPipelineShaderStageCreateInfo* shaderStages;
		uint32_t shaderStageCount;
		VkDescriptorSetLayout* pDescrptorSetLayouts;
		uint32_t descriptorSetLayoutCount;
		VkPushConstantRange* pPushConstants;
		uint32_t pushConstantCount;
	};

	struct VulkanPipeline
	{
		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
	};

	struct VulkanBackends
	{
		bool								enableValidationLayers;
		VkInstance							instance;
		VkDebugUtilsMessengerEXT			debugMessenger;
		LvnVector<VkPhysicalDevice>			physicalDevices;
		
		VkPhysicalDevice					physicalDevice;
		VkDevice							device;
		VkQueue								graphicsQueue;
		VkQueue								presentQueue;

		LvnVector<VulkanWindowSurfaceData>	windowSurfaceData;
		VkFormat							defaultSwapChainFormat;

		LvnPipelineSpecification			defaultPipelineSpecification;
		bool								gammaCorrect;
	};
}

#endif
