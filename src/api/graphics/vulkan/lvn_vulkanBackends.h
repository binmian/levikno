#ifndef HG_LEVIKNO_VULKAN_BACKENDS_H
#define HG_LEVIKNO_VULKAN_BACKENDS_H

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

#include "levikno_internal.h"


struct VulkanQueueFamilyIndices
{
    uint32_t graphicsIndex, presentIndex;
    bool has_graphics, has_present;
};

struct VulkanSwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct VulkanFrameBufferData
{
    uint32_t width, height;
    uint32_t totalAttachmentCount;
    VkSampleCountFlagBits sampleCount;
    bool multisampling, hasDepth;

    VkRenderPass renderPass;
    VkFramebuffer framebuffer;

    std::vector<LvnFrameBufferColorAttachment> colorAttachments;
    LvnFrameBufferDepthAttachment depthAttachment;

    std::vector<VkImage> colorImages;
    std::vector<VkImageView> colorImageViews;
    std::vector<VmaAllocation> colorImageMemory;

    std::vector<VkImage> msaaColorImages;
    std::vector<VkImageView> msaaColorImageViews;
    std::vector<VmaAllocation> msaaColorImageMemory;
    std::vector<VkClearValue> clearValues;

    std::vector<LvnTexture> frameBufferImages;
    LvnRenderPass frameBufferRenderPass;

    VkImage depthImage;
    VkImageView depthImageView;
    VmaAllocation depthImageMemory;

    VkSampler sampler;
};

struct VulkanWindowSurfaceData
{
    // core surface/swapchain data
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;

    // depth resources
    VkImage depthImage;
    VmaAllocation depthImageMemory;
    VkImageView depthImageView;

    // render pass & framebuffers
    VkRenderPass renderPass;
    std::vector<VkFramebuffer> frameBuffers;

    // command pool recording
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    // synchronization
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    // per frame data
    uint32_t imageIndex;
    uint32_t currentFrame;
    bool frameBufferResized;
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
    bool                                enableValidationLayers;
    VkInstance                          instance;
    VkDebugUtilsMessengerEXT            debugMessenger;

    std::vector<LvnPhysicalDevice>      lvnPhysicalDevices;
    VkPhysicalDevice                    physicalDevice;
    VkDevice                            device;
    VkQueue                             graphicsQueue;
    VkQueue                             presentQueue;
    VulkanQueueFamilyIndices            deviceIndices;
    VkPhysicalDeviceProperties          deviceProperties;
    VkPhysicalDeviceFeatures            deviceSupportedFeatures;
    VkCommandPool                       commandPool;
    VmaAllocator                        vmaAllocator;

    LvnPipelineSpecification            defaultPipelineSpecification;
    bool                                gammaCorrect;
    uint32_t                            maxFramesInFlight;
    VkFormat                            frameBufferColorFormat;
};


namespace lvn
{
    VulkanBackends* getVulkanBackends();
    void createVulkanWindowSurfaceData(LvnWindow* window);
    void destroyVulkanWindowSurfaceData(LvnWindow* window);
}

#endif
