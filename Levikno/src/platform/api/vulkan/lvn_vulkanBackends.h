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
		VkRenderPass renderPass;
		VkSwapchainKHR swapChain;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		VkImage* swapChainImages;
		VkImageView* swapChainImageViews;
		VkFramebuffer* frameBuffers;
		uint32_t swapChainImageCount;
		uint32_t swapChainImageViewCount;
		uint32_t frameBufferCount;
		uint32_t imageIndex;
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
		VulkanQueueFamilyIndices			deviceIndices;

		LvnVector<VulkanWindowSurfaceData>	windowSurfaceData;
		VkFormat							defaultSwapChainFormat;

		/* command buffers and semaphore/fence object count are dependent on the max frames in flight */
		VkCommandPool						commandPool;
		VkCommandBuffer*				  	commandBuffers;

		VkSemaphore*						imageAvailableSemaphores;
		VkSemaphore*						renderFinishedSemaphores;
		VkFence*							inFlightFences;

		LvnPipelineSpecification			defaultPipelineSpecification;
		bool								gammaCorrect;
		uint32_t							maxFramesInFlight;
		uint32_t							currentFrame;
	};
}

#endif
