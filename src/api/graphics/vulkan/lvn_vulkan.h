#ifndef HG_LEVIKNO_VULKAN_H
#define HG_LEVIKNO_VULKAN_H

#include "levikno_internal.h"

namespace lvn
{
    LvnResult vksImplCreateContext(LvnGraphicsContext* graphicsContext);
    void vksImplTerminateContext();
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
    LvnResult vksImplCreateCubemapHdr(LvnCubemap* cubemap, const LvnCubemapHdrCreateInfo* createInfo);

    void vksImplDestroyShader(LvnShader* shader);
    void vksImplDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout);
    void vksImplDestroyPipeline(LvnPipeline* pipeline);
    void vksImplDestroyFrameBuffer(LvnFrameBuffer* frameBuffer);
    void vksImplDestroyBuffer(LvnBuffer* vertexArrayBuffer);
    void vksImplDestroySampler(LvnSampler* sampler);
    void vksImplDestroyTexture(LvnTexture* texture);
    void vksImplDestroyCubemap(LvnCubemap* cubemap);

    void vksImplRenderBeginNextFrame(LvnWindow* window);
    void vksImplRenderDrawSubmit(LvnWindow* window);
    void vksImplRenderBeginCommandRecording(LvnWindow* window);
    void vksImplRenderEndCommandRecording(LvnWindow* window);
    void vksImplRenderCmdDraw(LvnWindow* window, uint32_t vertexCount);
    void vksImplRenderCmdDrawIndexed(LvnWindow* window, uint32_t indexCount);
    void vksImplRenderCmdDrawInstanced(LvnWindow* window, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
    void vksImplRenderCmdDrawIndexedInstanced(LvnWindow* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
    void vksImplRenderCmdSetStencilReference(uint32_t reference);
    void vksImplRenderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask);
    void vksImplRenderCmdBeginRenderPass(LvnWindow* window, float r, float g, float b, float a);
    void vksImplRenderCmdEndRenderPass(LvnWindow* window);
    void vksImplRenderCmdBindPipeline(LvnWindow* window, LvnPipeline* pipeline);
    void vksImplRenderCmdBindVertexBuffer(LvnWindow* window, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets);
    void vksImplRenderCmdBindIndexBuffer(LvnWindow* window, LvnBuffer* buffer, uint64_t offset);
    void vksImplRenderCmdBindDescriptorSets(LvnWindow* window, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets);
    void vksImplRenderCmdBeginFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);
    void vksImplRenderCmdEndFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);

    void vksImplBufferUpdateData(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset);
    void vksImplBufferResize(LvnBuffer* buffer, uint64_t size);
    void vksImplUpdateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count);
    LvnTexture* vksImplFrameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex);
    LvnRenderPass* vksImplFrameBufferGetRenderPass(LvnFrameBuffer* frameBuffer);
    void vksImplFrameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height);
    void vksImplFrameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a);

    LvnDepthImageFormat vksImplFindSupportedDepthImageFormat(LvnDepthImageFormat* pDepthImageFormats, uint32_t count);
}

#endif
