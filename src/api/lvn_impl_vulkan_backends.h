#ifndef HG_LVN_VULKAN_PLATFORM_H
#define HG_LVN_VULKAN_PLATFORM_H

#include "lvn_graphics_internal.h"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

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

struct VulkanFrameBufferData
{
    uint32_t width, height;
    uint32_t totalAttachmentCount;
    VkSampleCountFlagBits sampleCount;
    bool multisampling, hasDepth;

    VkRenderPass renderPass;
    VkFramebuffer framebuffer;

    LvnVector<LvnFrameBufferColorAttachment> colorAttachments;
    LvnFrameBufferDepthAttachment depthAttachment;

    LvnVector<VkImage> colorImages;
    LvnVector<VkImageView> colorImageViews;
    LvnVector<VmaAllocation> colorImageMemory;

    LvnVector<VkImage> msaaColorImages;
    LvnVector<VkImageView> msaaColorImageViews;
    LvnVector<VmaAllocation> msaaColorImageMemory;
    LvnVector<VkClearValue> clearValues;

    LvnVector<LvnTexture> frameBufferImages;
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
    LvnVector<VkImage> swapChainImages;
    LvnVector<VkImageView> swapChainImageViews;

    // depth resources
    VkImage depthImage;
    VmaAllocation depthImageMemory;
    VkImageView depthImageView;

    // render pass & framebuffers
    VkRenderPass renderPass;
    LvnVector<VkFramebuffer> frameBuffers;

    // command pool recording
    VkCommandPool commandPool;
    LvnVector<VkCommandBuffer> commandBuffers;

    // synchronization
    LvnVector<VkSemaphore> imageAvailableSemaphores;
    LvnVector<VkSemaphore> renderFinishedSemaphores;
    LvnVector<VkFence> inFlightFences;

    // per frame data
    uint32_t imageIndex;
    uint32_t currentFrame;
    bool frameBufferResized;
};

struct VulkanPipelineCreateData
{
    LvnPipelineFixedFunctions* pipelineFixedFuncs;
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

struct VulkanPlatform
{
    struct
    {
        bool surfaceSupport;
        bool win32Support;
        bool cocoaSupport;
        bool waylandSupport;
        bool x11xcbSupport;
        bool x11libSupport;
        bool debugSupport;
    } extensions;
};

struct VulkanBackends
{
    bool                                enableValidationLayers;
    bool                                physicalDeviceSet;
    VkInstance                          instance;
    VkDebugUtilsMessengerEXT            debugMessenger;
    VulkanPlatform                      vkplatform;

    LvnVector<LvnPhysicalDevice>        lvnPhysicalDevices;
    VkPhysicalDevice                    physicalDevice;
    VkDevice                            device;
    VkQueue                             graphicsQueue;
    VkQueue                             presentQueue;
    VulkanQueueFamilyIndices            deviceIndices;
    VkPhysicalDeviceProperties          deviceProperties;
    VkPhysicalDeviceFeatures            deviceSupportedFeatures;
    VkCommandPool                       commandPool;
    VmaAllocator                        vmaAllocator;

    LvnPipelineFixedFunctions           defaultPipelineFixedFuncs;
    bool                                gammaCorrect;
    uint32_t                            maxFramesInFlight;
    VkFormat                            frameBufferColorFormat;
};



#endif
