#ifndef HG_LVN_GRAPHICS_INTERNAL_H
#define HG_LVN_GRAPHICS_INTERNAL_H

#include "levikno_internal.h"
#include "lvn_graphics.h"


struct LvnGraphicsContext
{
    LvnGraphicsApi              graphicsapi;
    bool                        enableGraphicsApiDebugLogs;
    LvnTextureFormat            frameBufferColorFormat;
    uint32_t                    maxFramesInFlight;
    LvnPipelineFixedFunctions   defaultPipelineFixedFuncs;

    LvnResult                   (*implInitGraphicsContext)(LvnGraphicsContext*);
    void                        (*implTerminateGraphicsContext)();

    void                        (*getPhysicalDevices)(LvnPhysicalDevice**, uint32_t*);
    LvnResult                   (*checkPhysicalDeviceSupport)(LvnPhysicalDevice*);
    LvnResult                   (*setPhysicalDevice)(LvnPhysicalDevice*);

    LvnResult                   (*createShaderFromSrc)(LvnShader*, const LvnShaderCreateInfo*);
    LvnResult                   (*createShaderFromFileSrc)(LvnShader*, const LvnShaderCreateInfo*);
    LvnResult                   (*createShaderFromFileBin)(LvnShader*, const LvnShaderCreateInfo*);
    LvnResult                   (*createDescriptorLayout)(LvnDescriptorLayout*, const LvnDescriptorLayoutCreateInfo*);
    LvnResult                   (*allocateDescriptorSet)(LvnDescriptorSet*, LvnDescriptorLayout*);
    LvnResult                   (*createPipeline)(LvnPipeline*, const LvnPipelineCreateInfo*);
    LvnResult                   (*createFrameBuffer)(LvnFrameBuffer*, const LvnFrameBufferCreateInfo*);
    LvnResult                   (*createBuffer)(LvnBuffer*, const LvnBufferCreateInfo*);
    LvnResult                   (*createSampler)(LvnSampler*, const LvnSamplerCreateInfo*);
    LvnResult                   (*createTexture)(LvnTexture*, const LvnTextureCreateInfo*);
    LvnResult                   (*createTextureSampler)(LvnTexture*, const LvnTextureSamplerCreateInfo*);
    LvnResult                   (*createCubemap)(LvnCubemap*, const LvnCubemapCreateInfo*);

    void                        (*destroyShader)(LvnShader*);
    void                        (*destroyDescriptorLayout)(LvnDescriptorLayout*);
    void                        (*destroyPipeline)(LvnPipeline*);
    void                        (*destroyFrameBuffer)(LvnFrameBuffer*);
    void                        (*destroyBuffer)(LvnBuffer*);
    void                        (*destroySampler)(LvnSampler*);
    void                        (*destroyTexture)(LvnTexture*);
    void                        (*destroyCubemap)(LvnCubemap*);

    void                        (*renderBeginNextFrame)(LvnCommandBuffer*);
    void                        (*renderDrawSubmit)(LvnCommandBuffer*);
    void                        (*renderBeginCommandRecording)(LvnCommandBuffer*);
    void                        (*renderEndCommandRecording)(LvnCommandBuffer*);
    void                        (*renderCmdDraw)(LvnCommandBuffer*, uint32_t);
    void                        (*renderCmdDrawIndexed)(LvnCommandBuffer*, uint32_t);
    void                        (*renderCmdDrawInstanced)(LvnCommandBuffer*, uint32_t, uint32_t, uint32_t);
    void                        (*renderCmdDrawIndexedInstanced)(LvnCommandBuffer*, uint32_t, uint32_t, uint32_t);
    void                        (*renderCmdSetStencilReference)(uint32_t);
    void                        (*renderCmdSetStencilMask)(uint32_t, uint32_t);
    void                        (*renderCmdBeginRenderPass)(LvnCommandBuffer*, float r, float g, float b, float a);
    void                        (*renderCmdEndRenderPass)(LvnCommandBuffer*);
    void                        (*renderCmdBindPipeline)(LvnCommandBuffer*, LvnPipeline*);
    void                        (*renderCmdBindVertexBuffer)(LvnCommandBuffer*, uint32_t, uint32_t, LvnBuffer**, uint64_t*);
    void                        (*renderCmdBindIndexBuffer)(LvnCommandBuffer*, LvnBuffer*, uint64_t);
    void                        (*renderCmdBindDescriptorSets)(LvnCommandBuffer*, LvnPipeline*, uint32_t, uint32_t, LvnDescriptorSet**);
    void                        (*renderCmdBeginFrameBuffer)(LvnCommandBuffer*, LvnFrameBuffer*);
    void                        (*renderCmdEndFrameBuffer)(LvnCommandBuffer*, LvnFrameBuffer*);

    void                        (*bufferUpdateData)(LvnBuffer*, void*, uint64_t, uint64_t);
    void                        (*bufferResize)(LvnBuffer*, uint64_t);
    void                        (*updateDescriptorSetData)(LvnDescriptorSet*, LvnDescriptorUpdateInfo*, uint32_t);
    LvnTexture*                 (*frameBufferGetImage)(LvnFrameBuffer*, uint32_t);
    LvnRenderPass*              (*frameBufferGetRenderPass)(LvnFrameBuffer*);
    void                        (*framebufferResize)(LvnFrameBuffer*, uint32_t, uint32_t);
    void                        (*frameBufferSetClearColor)(LvnFrameBuffer*, uint32_t, float, float, float, float);

    LvnDepthImageFormat         (*findSupportedDepthImageFormat)(LvnDepthImageFormat*, uint32_t);
};

struct LvnPhysicalDevice
{
    LvnPhysicalDeviceProperties properties;
    LvnPhysicalDeviceFeatures features;
    void* physicalDevice;
};

struct LvnRenderPass
{
    void* nativeRenderPass;
};

struct LvnShader
{
    void* nativeVertexShaderModule;
    void* nativeFragmentShaderModule;

    uint32_t vertexShaderId;
    uint32_t fragmentShaderId;
};

struct LvnDescriptorLayout
{
    void* descriptorLayout;
    void* descriptorPool;

    LvnVector<LvnDescriptorSet> descriptorSets;
    uint64_t descriptorSetIndex;
};

struct LvnDescriptorSet
{
    LvnVector<void*> descriptorSets;
    void* singleSet;
};

struct LvnPipeline
{
    void* nativePipeline;
    void* nativePipelineLayout;

    uint32_t id;
    uint32_t vaoId;

    LvnHashMap<uint32_t, uint32_t> bindingDescriptions;
};

struct LvnBuffer
{
    LvnBufferTypeFlagBits type;
    LvnBufferUsage usage;
    uint32_t id;
    uint64_t size;

    void* buffer;
    void* bufferMemory;
    void* bufferMap;
};

struct LvnSampler
{
    void* sampler;
};

struct LvnTexture
{
    void* image;
    void* imageMemory;
    void* imageView;
    void* sampler;

    uint32_t width, height;
    uint32_t id;

    bool seperateSampler;
};

struct LvnFrameBuffer
{
    void* frameBufferData;
};

struct LvnCubemap
{
    LvnTexture textureData;
};

#endif /* !HG_LVN_GRAPHICS_INTERNAL_H */
