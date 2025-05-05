#ifndef HG_LEVIKNO_OPENGL_H
#define HG_LEVIKNO_OPENGL_H

#include "levikno_internal.h"

namespace lvn
{
    LvnResult oglsImplCreateContext(LvnGraphicsContext* graphicsContext);
    void oglsImplTerminateContext();
    void oglsImplGetPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* physicalDeviceCount);
    LvnResult oglsImplCheckPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice);
    LvnResult oglsImplSetPhysicalDevice(LvnPhysicalDevice* physicalDevice);

    LvnResult oglsImplCreateShaderFromSrc(LvnShader* shader, const LvnShaderCreateInfo* createInfo);
    LvnResult oglsImplCreateShaderFromFileSrc(LvnShader* shader, const LvnShaderCreateInfo* createInfo);
    LvnResult oglsImplCreateShaderFromFileBin(LvnShader* shader, const LvnShaderCreateInfo* createInfo);
    LvnResult oglsImplCreateDescriptorLayout(LvnDescriptorLayout* descriptorLayout, const LvnDescriptorLayoutCreateInfo* createInfo);
    LvnResult oglsImplAllocateDescriptorSet(LvnDescriptorSet* descriptorSet, LvnDescriptorLayout* descriptorLayout);
    LvnResult oglsImplCreatePipeline(LvnPipeline* pipeline, const LvnPipelineCreateInfo* createInfo);
    LvnResult oglsImplCreateFrameBuffer(LvnFrameBuffer* frameBuffer, const LvnFrameBufferCreateInfo* createInfo);
    LvnResult oglsImplCreateBuffer(LvnBuffer* buffer, const LvnBufferCreateInfo* createInfo);
    LvnResult oglsImplCreateSampler(LvnSampler* sampler, const LvnSamplerCreateInfo* createInfo);
    LvnResult oglsImplCreateTexture(LvnTexture* texture, const LvnTextureCreateInfo* createInfo);
    LvnResult oglsImplCreateTextureSampler(LvnTexture* texture, const LvnTextureSamplerCreateInfo* createInfo);
    LvnResult oglsImplCreateCubemap(LvnCubemap* cubemap, const LvnCubemapCreateInfo* createInfo);
    LvnResult oglsImplCreateCubemapHdr(LvnCubemap* cubemap, const LvnCubemapHdrCreateInfo* createInfo);

    void oglsImplDestroyShader(LvnShader* shader);
    void oglsImplDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout);
    void oglsImplDestroyPipeline(LvnPipeline* pipeline);
    void oglsImplDestroyFrameBuffer(LvnFrameBuffer* frameBuffer);
    void oglsImplDestroyBuffer(LvnBuffer* buffer);
    void oglsImplDestroySampler(LvnSampler* sampler);
    void oglsImplDestroyTexture(LvnTexture* texture);
    void oglsImplDestroyCubemap(LvnCubemap* cubemap);

    void oglsImplRenderBeginNextFrame(LvnWindow* window);
    void oglsImplRenderDrawSubmit(LvnWindow* window);
    void oglsImplRenderBeginCommandRecording(LvnWindow* window);
    void oglsImplRenderEndCommandRecording(LvnWindow* window);
    void oglsImplRenderCmdDraw(LvnWindow* window, uint32_t vertexCount);
    void oglsImplRenderCmdDrawIndexed(LvnWindow* window, uint32_t indexCount);
    void oglsImplRenderCmdDrawInstanced(LvnWindow* window, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
    void oglsImplRenderCmdDrawIndexedInstanced(LvnWindow* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
    void oglsImplRenderCmdSetStencilReference(uint32_t reference);
    void oglsImplRenderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask);
    void oglsImplRenderCmdBeginRenderPass(LvnWindow* window, float r, float g, float b, float a);
    void oglsImplRenderCmdEndRenderPass(LvnWindow* window);
    void oglsImplRenderCmdBindPipeline(LvnWindow* window, LvnPipeline* pipeline);
    void oglsImplRenderCmdBindVertexBuffer(LvnWindow* window, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets);
    void oglsImplRenderCmdBindIndexBuffer(LvnWindow* window, LvnBuffer* buffer, uint64_t offset);
    void oglsImplRenderCmdBindDescriptorSets(LvnWindow* window, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets);
    void oglsImplRenderCmdBeginFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);
    void oglsImplRenderCmdEndFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);

    void oglsImplBufferUpdateData(LvnBuffer* buffer, void* vertices, uint64_t size, uint64_t offset);
    void oglsImplBufferResize(LvnBuffer* buffer, uint64_t size);
    void oglsImplUpdateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count);
    LvnTexture* oglsImplFrameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex);
    LvnRenderPass* oglsImplFrameBufferGetRenderPass(LvnFrameBuffer* frameBuffer);
    void oglsImplFrameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height);
    void oglsImplFrameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a);

    LvnDepthImageFormat oglsImplFindSupportedDepthImageFormat(LvnDepthImageFormat* pDepthImageFormats, uint32_t count);
    void setOglWindowContextValues();
    void* getMainOglWindowContext();
}

#endif
