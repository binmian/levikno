#ifndef HG_LVN_IMPL_GL_H
#define HG_LVN_IMPL_GL_H

#include "lvn_graphics_internal.h"


struct OglDescriptorBinding
{
    LvnDescriptorType type;
    uint32_t binding;
    uint32_t id;
    uint64_t range;
    uint64_t offset;
};

struct OglBindlessTextureBinding
{
    uint32_t ssbo;
    uint32_t binding;
    LvnVector<uint64_t> textureHandles;
};

struct OglDescriptorSet
{
    LvnVector<OglDescriptorBinding> uniformBuffers;
    LvnVector<OglDescriptorBinding> textures;
    LvnVector<OglBindlessTextureBinding> bindlessTextures;
};

struct OglPipelineEnums
{
    uint32_t depthCompareOp;
    uint32_t topologyType;
    uint32_t srcBlendFactor;
    uint32_t dstBlendFactor;
    uint32_t cullMode;
    uint32_t frontFace;
    bool enableDepth;
    bool enableBlending;
    bool enableCulling;
};

struct OglFramebufferData
{
    uint32_t id, msaaId;
    uint32_t x, y, width, height;
    LvnTextureFilter textureFilter;
    LvnTextureMode textureMode;
    LvnSampleCount sampleCount;

    LvnVector<LvnFrameBufferColorAttachment> colorAttachmentSpecifications;
    LvnFrameBufferDepthAttachment depthAttachmentSpecification;
    LvnVector<uint32_t> colorAttachments, msaaColorAttachments;
    uint32_t depthAttachment, msaaDepthAttachment;
    bool multisampling, hasDepth;

    LvnVector<LvnTexture> colorAttachmentTextures;
};

struct OglSampler
{
    LvnTextureFilter minFilter, magFilter;
    LvnTextureMode wrapS, wrapT, wrapR;
};

struct OglBackends
{
    const char* deviceName;
    uint32_t versionMajor;
    uint32_t versionMinor;
    LvnPhysicalDevice physicalDevice;
    LvnPipelineFixedFunctions defaultOglPipelineFixedFuncs;

    int maxTextureUnitSlots;
    bool framebufferColorFormatSrgb;
};

namespace lvn
{
    LvnResult             implGlsCreateContext(LvnGraphicsContext* graphicsContext);
    void                  implGlsTerminateContext();
    void                  implGlsGetPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* physicalDeviceCount);
    LvnResult             implGlsCheckPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice);
    LvnResult             implGlsSetPhysicalDevice(LvnPhysicalDevice* physicalDevice);

    LvnResult             implGlsCreateShaderFromSrc(LvnShader* shader, const LvnShaderCreateInfo* createInfo);
    LvnResult             implGlsCreateShaderFromFileSrc(LvnShader* shader, const LvnShaderCreateInfo* createInfo);
    LvnResult             implGlsCreateShaderFromFileBin(LvnShader* shader, const LvnShaderCreateInfo* createInfo);
    LvnResult             implGlsCreateDescriptorLayout(LvnDescriptorLayout* descriptorLayout, const LvnDescriptorLayoutCreateInfo* createInfo);
    LvnResult             implGlsAllocateDescriptorSet(LvnDescriptorSet* descriptorSet, LvnDescriptorLayout* descriptorLayout);
    LvnResult             implGlsCreatePipeline(LvnPipeline* pipeline, const LvnPipelineCreateInfo* createInfo);
    LvnResult             implGlsCreateFrameBuffer(LvnFrameBuffer* frameBuffer, const LvnFrameBufferCreateInfo* createInfo);
    LvnResult             implGlsCreateBuffer(LvnBuffer* buffer, const LvnBufferCreateInfo* createInfo);
    LvnResult             implGlsCreateSampler(LvnSampler* sampler, const LvnSamplerCreateInfo* createInfo);
    LvnResult             implGlsCreateTexture(LvnTexture* texture, const LvnTextureCreateInfo* createInfo);
    LvnResult             implGlsCreateCubemap(LvnCubemap* cubemap, const LvnCubemapCreateInfo* createInfo);

    void                  implGlsDestroyShader(LvnShader* shader);
    void                  implGlsDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout);
    void                  implGlsDestroyPipeline(LvnPipeline* pipeline);
    void                  implGlsDestroyFrameBuffer(LvnFrameBuffer* frameBuffer);
    void                  implGlsDestroyBuffer(LvnBuffer* buffer);
    void                  implGlsDestroySampler(LvnSampler* sampler);
    void                  implGlsDestroyTexture(LvnTexture* texture);
    void                  implGlsDestroyCubemap(LvnCubemap* cubemap);

    void                  implGlsRenderBeginNextFrame(LvnWindow* window);
    void                  implGlsRenderDrawSubmit(LvnWindow* window);
    void                  implGlsRenderBeginCommandRecording(LvnWindow* window);
    void                  implGlsRenderEndCommandRecording(LvnWindow* window);
    void                  implGlsRenderCmdDraw(LvnWindow* window, uint32_t vertexCount);
    void                  implGlsRenderCmdDrawIndexed(LvnWindow* window, uint32_t indexCount);
    void                  implGlsRenderCmdDrawInstanced(LvnWindow* window, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
    void                  implGlsRenderCmdDrawIndexedInstanced(LvnWindow* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
    void                  implGlsRenderCmdSetStencilReference(uint32_t reference);
    void                  implGlsRenderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask);
    void                  implGlsRenderCmdBeginRenderPass(LvnWindow* window, float r, float g, float b, float a);
    void                  implGlsRenderCmdEndRenderPass(LvnWindow* window);
    void                  implGlsRenderCmdBindPipeline(LvnWindow* window, LvnPipeline* pipeline);
    void                  implGlsRenderCmdBindVertexBuffer(LvnWindow* window, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets);
    void                  implGlsRenderCmdBindIndexBuffer(LvnWindow* window, LvnBuffer* buffer, uint64_t offset);
    void                  implGlsRenderCmdBindDescriptorSets(LvnWindow* window, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets);
    void                  implGlsRenderCmdBeginFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);
    void                  implGlsRenderCmdEndFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);

    void                  implGlsBufferUpdateData(LvnBuffer* buffer, void* vertices, uint64_t size, uint64_t offset);
    void                  implGlsBufferResize(LvnBuffer* buffer, uint64_t size);
    void                  implGlsUpdateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count);
    LvnTexture*           implGlsFrameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex);
    LvnRenderPass*        implGlsFrameBufferGetRenderPass(LvnFrameBuffer* frameBuffer);
    void                  glsImplFrameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height);
    void                  glsImplFrameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a);

    LvnDepthImageFormat   implGlsFindSupportedDepthImageFormat(LvnDepthImageFormat* pDepthImageFormats, uint32_t count);

    void                  setOglWindowContextValues();
    void*                 getMainOglWindowContext();

    void                  implGlsRecordCmdDraw(LvnWindow* window, uint32_t vertexCount);
    void                  implGlsRecordCmdDrawIndexed(LvnWindow* window, uint32_t indexCount);
    void                  implGlsRecordCmdDrawInstanced(LvnWindow* window, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
    void                  implGlsRecordCmdDrawIndexedInstanced(LvnWindow* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
    void                  implGlsRecordCmdSetStencilReference(uint32_t reference);
    void                  implGlsRecordCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask);
    void                  implGlsRecordCmdBeginRenderPass(LvnWindow* window, float r, float g, float b, float a);
    void                  implGlsRecordCmdEndRenderPass(LvnWindow* window);
    void                  implGlsRecordCmdBindPipeline(LvnWindow* window, LvnPipeline* pipeline);
    void                  implGlsRecordCmdBindVertexBuffer(LvnWindow* window, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets);
    void                  implGlsRecordCmdBindIndexBuffer(LvnWindow* window, LvnBuffer* buffer, uint64_t offset);
    void                  implGlsRecordCmdBindDescriptorSets(LvnWindow* window, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets);
    void                  implGlsRecordCmdBeginFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);
    void                  implGlsRecordCmdEndFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);

    void                  implGlsDrawBuffCmdDraw(void* data);
    void                  implGlsDrawBuffCmdDrawIndexed(void* data);
    void                  implGlsDrawBuffCmdDrawInstanced(void* data);
    void                  implGlsDrawBuffCmdDrawIndexedInstanced(void* data);
    void                  implGlsDrawBuffCmdSetStencilReference(void* data);
    void                  implGlsDrawBuffCmdSetStencilMask(void* data);
    void                  implGlsDrawBuffCmdBeginRenderPass(void* data);
    void                  implGlsDrawBuffCmdEndRenderPass(void* data);
    void                  implGlsDrawBuffCmdBindPipeline(void* data);
    void                  implGlsDrawBuffCmdBindVertexBuffer(void* data);
    void                  implGlsDrawBuffCmdBindIndexBuffer(void* data);
    void                  implGlsDrawBuffCmdBindDescriptorSets(void* data);
    void                  implGlsDrawBuffCmdBeginFrameBuffer(void* data);
    void                  implGlsDrawBuffCmdEndFrameBuffer(void* data);
}

#endif /* HG_LVN_IMPL_GL_H */
