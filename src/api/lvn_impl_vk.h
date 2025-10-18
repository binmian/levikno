#ifndef HG_LVN_IMPL_VK_H
#define HG_LVN_IMPL_VK_H

#include "lvn_graphics_internal.h"


namespace lvn
{
    LvnResult              implVkInitGraphicsContext(LvnGraphicsContext* graphicsContext);
    void                   implVkTerminateGraphicsContext();

    void                   implVksGetPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* physicalDeviceCount);
    LvnResult              implVksCheckPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice);
    LvnResult              implVksSetPhysicalDevice(LvnPhysicalDevice* physicalDevice);

    LvnResult              implVksCreateShaderFromSrc(LvnShader* shader, const LvnShaderCreateInfo* createInfo);
    LvnResult              implVksCreateShaderFromBin(LvnShader* shader, const LvnShaderBinCreateInfo* createInfo);
    LvnResult              implVksCreateShaderFromFileSrc(LvnShader* shader, const LvnShaderCreateInfo* createInfo);
    LvnResult              implVksCreateShaderFromFileBin(LvnShader* shader, const LvnShaderCreateInfo* createInfo);
    LvnResult              implVksCreateDescriptorLayout(LvnDescriptorLayout* descriptorLayout, const LvnDescriptorLayoutCreateInfo* createInfo);
    LvnResult              implVksAllocateDescriptorSet(LvnDescriptorLayout* descriptorLayout, LvnDescriptorSet** pDescriptorSets, uint32_t count);
    LvnResult              implVksCreatePipeline(LvnPipeline* pipeline, const LvnPipelineCreateInfo* createInfo);
    LvnResult              implVksCreateCommandPool(LvnCommandPool* cmdPool);
    LvnResult              implVksAllocateCommandBuffers(LvnCommandPool* cmdPool, LvnCommandBuffer** pCmdBuffers, uint32_t count);
    LvnResult              implVksCreateFrameBuffer(LvnFrameBuffer* frameBuffer, const LvnFrameBufferCreateInfo* createInfo);
    LvnResult              implVksCreateBuffer(LvnBuffer* buffer, const LvnBufferCreateInfo* createInfo);
    LvnResult              implVksCreateSampler(LvnSampler* sampler, const LvnSamplerCreateInfo* createInfo);
    LvnResult              implVksCreateTexture(LvnTexture* texture, const LvnTextureCreateInfo* createInfo);
    LvnResult              implVksCreateCubemap(LvnCubemap* cubemap, const LvnCubemapCreateInfo* createInfo);

    void                   implVksDestroyShader(LvnShader* shader);
    void                   implVksDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout);
    void                   implVksDestroyPipeline(LvnPipeline* pipeline);
    void                   implVksDestroyCommandPool(LvnCommandPool* cmdPool);
    void                   implVksDestroyFrameBuffer(LvnFrameBuffer* frameBuffer);
    void                   implVksDestroyBuffer(LvnBuffer* buffer);
    void                   implVksDestroySampler(LvnSampler* sampler);
    void                   implVksDestroyTexture(LvnTexture* texture);
    void                   implVksDestroyCubemap(LvnCubemap* cubemap);

    void                   implVksRenderBeginNextFrame(LvnWindow* window, LvnCommandBuffer* cmdBuffer);
    void                   implVksRenderDrawSubmit(LvnWindow* window, LvnCommandBuffer* cmdBuffer);
    void                   implVksRenderBeginCommandRecording(LvnCommandBuffer* cmdBuffer);
    void                   implVksRenderEndCommandRecording(LvnCommandBuffer* cmdBuffer);
    void                   implVksRenderCmdDraw(LvnCommandBuffer* cmdBuffer, uint32_t vertexCount);
    void                   implVksRenderCmdDrawIndexed(LvnCommandBuffer* cmdBuffer, uint32_t indexCount);
    void                   implVksRenderCmdDrawInstanced(LvnCommandBuffer* cmdBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
    void                   implVksRenderCmdDrawIndexedInstanced(LvnCommandBuffer* cmdBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
    void                   implVksRenderCmdSetStencilReference(uint32_t reference);
    void                   implVksRenderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask);
    void                   implVksRenderCmdBeginRenderPass(LvnCommandBuffer* cmdBuffer, LvnWindow* window, float r, float g, float b, float a);
    void                   implVksRenderCmdEndRenderPass(LvnCommandBuffer* cmdBuffer);
    void                   implVksRenderCmdBindPipeline(LvnCommandBuffer* cmdBuffer, LvnPipeline* pipeline);
    void                   implVksRenderCmdBindVertexBuffer(LvnCommandBuffer* cmdBuffer, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets);
    void                   implVksRenderCmdBindIndexBuffer(LvnCommandBuffer* cmdBuffer, LvnBuffer* buffer, uint64_t offset);
    void                   implVksRenderCmdBindDescriptorSets(LvnCommandBuffer* cmdBuffer, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets);
    void                   implVksRenderCmdBeginFrameBuffer(LvnCommandBuffer* cmdBuffer, LvnFrameBuffer* frameBuffer);
    void                   implVksRenderCmdEndFrameBuffer(LvnCommandBuffer* cmdBuffer, LvnFrameBuffer* frameBuffer);

    void                   implVksBufferUpdateData(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset);
    void                   implVksBufferResize(LvnBuffer* buffer, uint64_t size);
    void                   implVksUpdateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count);
    LvnTexture*            implVksFrameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex);
    LvnRenderPass*         implVksFrameBufferGetRenderPass(LvnFrameBuffer* frameBuffer);
    void                   implVksFrameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height);
    void                   implVksFrameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a);

    LvnDepthImageFormat    implVksFindSupportedDepthImageFormat(LvnDepthImageFormat* pDepthImageFormats, uint32_t count);
    void                   implVksInternalWindowListenEventFn(LvnWindow* window, LvnEvent* event);
}

#endif /* !HG_LVN_IMPL_VK_H */
