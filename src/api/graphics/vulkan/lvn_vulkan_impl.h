#ifndef HG_LVN_VULKAN_H
#define HG_LVN_VULKAN_H

#include "lvn_graphics_internal.h"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>


namespace lvn
{
    void vksImplGetPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* physicalDeviceCount);
    LvnResult vksImplCheckPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice);
    LvnResult vksImplSetPhysicalDevice(LvnPhysicalDevice* physicalDevice);

    LvnResult vksImplCreateShaderFromSrc(LvnShader* shader, const LvnShaderCreateInfo* createInfo);
    LvnResult vksImplCreateShaderFromFileSrc(LvnShader* shader, const LvnShaderCreateInfo* createInfo);
    LvnResult vksImplCreateShaderFromFileBin(LvnShader* shader, const LvnShaderCreateInfo* createInfo);
    LvnResult vksImplCreateDescriptorLayout(LvnDescriptorLayout* descriptorLayout, const LvnDescriptorLayoutCreateInfo* createInfo);
    LvnResult vksImplAllocateDescriptorSet(LvnDescriptorSet* descriptorSet, LvnDescriptorLayout* descriptorLayout);
    LvnResult vksImplCreatePipeline(LvnPipeline* pipeline, const LvnPipelineCreateInfo* createInfo);
    LvnResult vksImplCreateFrameBuffer(LvnFrameBuffer* frameBuffer, const LvnFrameBufferCreateInfo* createInfo);
    LvnResult vksImplCreateBuffer(LvnBuffer* buffer, const LvnBufferCreateInfo* createInfo);
    LvnResult vksImplCreateSampler(LvnSampler* sampler, const LvnSamplerCreateInfo* createInfo);
    LvnResult vksImplCreateTexture(LvnTexture* texture, const LvnTextureCreateInfo* createInfo);
    LvnResult vksImplCreateTextureSampler(LvnTexture* texture, const LvnTextureSamplerCreateInfo* createInfo);
    LvnResult vksImplCreateCubemap(LvnCubemap* cubemap, const LvnCubemapCreateInfo* createInfo);

    void vksImplDestroyShader(LvnShader* shader);
    void vksImplDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout);
    void vksImplDestroyPipeline(LvnPipeline* pipeline);
    void vksImplDestroyFrameBuffer(LvnFrameBuffer* frameBuffer);
    void vksImplDestroyBuffer(LvnBuffer* vertexArrayBuffer);
    void vksImplDestroySampler(LvnSampler* sampler);
    void vksImplDestroyTexture(LvnTexture* texture);
    void vksImplDestroyCubemap(LvnCubemap* cubemap);

    void vksImplRenderBeginNextFrame(LvnCommandBuffer* window);
    void vksImplRenderDrawSubmit(LvnCommandBuffer* window);
    void vksImplRenderBeginCommandRecording(LvnCommandBuffer* window);
    void vksImplRenderEndCommandRecording(LvnCommandBuffer* window);
    void vksImplRenderCmdDraw(LvnCommandBuffer* window, uint32_t vertexCount);
    void vksImplRenderCmdDrawIndexed(LvnCommandBuffer* window, uint32_t indexCount);
    void vksImplRenderCmdDrawInstanced(LvnCommandBuffer* window, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
    void vksImplRenderCmdDrawIndexedInstanced(LvnCommandBuffer* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
    void vksImplRenderCmdSetStencilReference(uint32_t reference);
    void vksImplRenderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask);
    void vksImplRenderCmdBeginRenderPass(LvnCommandBuffer* window, float r, float g, float b, float a);
    void vksImplRenderCmdEndRenderPass(LvnCommandBuffer* window);
    void vksImplRenderCmdBindPipeline(LvnCommandBuffer* window, LvnPipeline* pipeline);
    void vksImplRenderCmdBindVertexBuffer(LvnCommandBuffer* window, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets);
    void vksImplRenderCmdBindIndexBuffer(LvnCommandBuffer* window, LvnBuffer* buffer, uint64_t offset);
    void vksImplRenderCmdBindDescriptorSets(LvnCommandBuffer* window, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets);
    void vksImplRenderCmdBeginFrameBuffer(LvnCommandBuffer* window, LvnFrameBuffer* frameBuffer);
    void vksImplRenderCmdEndFrameBuffer(LvnCommandBuffer* window, LvnFrameBuffer* frameBuffer);

    void vksImplBufferUpdateData(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset);
    void vksImplBufferResize(LvnBuffer* buffer, uint64_t size);
    void vksImplUpdateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count);
    LvnTexture* vksImplFrameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex);
    LvnRenderPass* vksImplFrameBufferGetRenderPass(LvnFrameBuffer* frameBuffer);
    void vksImplFrameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height);
    void vksImplFrameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a);

    LvnDepthImageFormat vksImplFindSupportedDepthImageFormat(LvnDepthImageFormat* pDepthImageFormats, uint32_t count);
}

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
    LvnPipelineFixedFunctions* pipelineSpecification;
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

    LvnPipelineFixedFunctions           defaultPipelineSpecification;
    bool                                gammaCorrect;
    uint32_t                            maxFramesInFlight;
    VkFormat                            frameBufferColorFormat;
};

#endif /* !HG_LVN_VULKAN_H */
