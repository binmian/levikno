#ifndef HG_LVN_VULKAN_H
#define HG_LVN_VULKAN_H

#include "lvn_graphics_internal.h"


namespace lvn
{
    LvnResult implVkInitGraphicsContext(LvnGraphicsContext* graphicsContext);
    void implVkTerminateGraphicsContext();

    void vksImplGetPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* physicalDeviceCount);
    LvnResult vksImplCheckPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice);
    LvnResult vksImplSetPhysicalDevice(LvnPhysicalDevice* physicalDevice);

    LvnResult vksImplCreateShaderFromSrc(LvnShader* shader, const LvnShaderCreateInfo* createInfo);
    LvnResult vksImplCreateShaderFromBin(LvnShader* shader, const LvnShaderBinCreateInfo* createInfo);
    LvnResult vksImplCreateShaderFromFileSrc(LvnShader* shader, const LvnShaderCreateInfo* createInfo);
    LvnResult vksImplCreateShaderFromFileBin(LvnShader* shader, const LvnShaderCreateInfo* createInfo);
    LvnResult vksImplCreateDescriptorLayout(LvnDescriptorLayout* descriptorLayout, const LvnDescriptorLayoutCreateInfo* createInfo);
    LvnResult vksImplAllocateDescriptorSet(LvnDescriptorLayout* descriptorLayout, LvnDescriptorSet** pDescriptorSets, uint32_t count);
    LvnResult vksImplCreatePipeline(LvnPipeline* pipeline, const LvnPipelineCreateInfo* createInfo);
    LvnResult vksImplCreateCommandPool(LvnCommandPool* cmdPool);
    LvnResult vksImplAllocateCommandBuffers(LvnCommandPool* cmdPool, LvnCommandBuffer** pCmdBuffers, uint32_t count);
    LvnResult vksImplCreateFrameBuffer(LvnFrameBuffer* frameBuffer, const LvnFrameBufferCreateInfo* createInfo);
    LvnResult vksImplCreateBuffer(LvnBuffer* buffer, const LvnBufferCreateInfo* createInfo);
    LvnResult vksImplCreateSampler(LvnSampler* sampler, const LvnSamplerCreateInfo* createInfo);
    LvnResult vksImplCreateTexture(LvnTexture* texture, const LvnTextureCreateInfo* createInfo);
    LvnResult vksImplCreateCubemap(LvnCubemap* cubemap, const LvnCubemapCreateInfo* createInfo);

    void vksImplDestroyShader(LvnShader* shader);
    void vksImplDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout);
    void vksImplDestroyPipeline(LvnPipeline* pipeline);
    void vksImplDestroyCommandPool(LvnCommandPool* cmdPool);
    void vksImplDestroyFrameBuffer(LvnFrameBuffer* frameBuffer);
    void vksImplDestroyBuffer(LvnBuffer* buffer);
    void vksImplDestroySampler(LvnSampler* sampler);
    void vksImplDestroyTexture(LvnTexture* texture);
    void vksImplDestroyCubemap(LvnCubemap* cubemap);

    void vksImplRenderBeginNextFrame(LvnWindow* window, LvnCommandBuffer* cmdBuffer);
    void vksImplRenderDrawSubmit(LvnWindow* window, LvnCommandBuffer* cmdBuffer);
    void vksImplRenderBeginCommandRecording(LvnCommandBuffer* cmdBuffer);
    void vksImplRenderEndCommandRecording(LvnCommandBuffer* cmdBuffer);
    void vksImplRenderCmdDraw(LvnCommandBuffer* cmdBuffer, uint32_t vertexCount);
    void vksImplRenderCmdDrawIndexed(LvnCommandBuffer* cmdBuffer, uint32_t indexCount);
    void vksImplRenderCmdDrawInstanced(LvnCommandBuffer* cmdBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
    void vksImplRenderCmdDrawIndexedInstanced(LvnCommandBuffer* cmdBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
    void vksImplRenderCmdSetStencilReference(uint32_t reference);
    void vksImplRenderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask);
    void vksImplRenderCmdBeginRenderPass(LvnCommandBuffer* cmdBuffer, LvnWindow* window, float r, float g, float b, float a);
    void vksImplRenderCmdEndRenderPass(LvnCommandBuffer* cmdBuffer);
    void vksImplRenderCmdBindPipeline(LvnCommandBuffer* cmdBuffer, LvnPipeline* pipeline);
    void vksImplRenderCmdBindVertexBuffer(LvnCommandBuffer* cmdBuffer, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets);
    void vksImplRenderCmdBindIndexBuffer(LvnCommandBuffer* cmdBuffer, LvnBuffer* buffer, uint64_t offset);
    void vksImplRenderCmdBindDescriptorSets(LvnCommandBuffer* cmdBuffer, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets);
    void vksImplRenderCmdBeginFrameBuffer(LvnCommandBuffer* cmdBuffer, LvnFrameBuffer* frameBuffer);
    void vksImplRenderCmdEndFrameBuffer(LvnCommandBuffer* cmdBuffer, LvnFrameBuffer* frameBuffer);

    void vksImplBufferUpdateData(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset);
    void vksImplBufferResize(LvnBuffer* buffer, uint64_t size);
    void vksImplUpdateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count);
    LvnTexture* vksImplFrameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex);
    LvnRenderPass* vksImplFrameBufferGetRenderPass(LvnFrameBuffer* frameBuffer);
    void vksImplFrameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height);
    void vksImplFrameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a);

    LvnDepthImageFormat vksImplFindSupportedDepthImageFormat(LvnDepthImageFormat* pDepthImageFormats, uint32_t count);
    void vksImplInternalWindowListenEventFn(LvnWindow* window, LvnEvent* event);
}

#endif /* !HG_LVN_VULKAN_H */
