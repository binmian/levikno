#include "lvn_graphics_internal.h"

#include "lvn_platform.h"

#include <stb_image.h>
#include <stb_image_write.h>

#include <cstring>
#include <ctime>

#ifdef LVN_INCLUDE_VULKAN
#   include "lvn_impl_vk.h"
#endif


namespace lvn
{

static LvnResult   initGraphicsApiFuncs(LvnGraphicsContext* ctx);
static void        terminateGraphicsApiFuncs(LvnGraphicsContext* ctx);
static void        initStandardPipelineFixedFuncs(LvnGraphicsContext* graphicsctx);
static const char* getGraphicsApiNameEnum(LvnGraphicsApi api);

static LvnResult initGraphicsApiFuncs(LvnGraphicsContext* ctx)
{
    LvnContext* lvnctx = ctx->lvnctx;
    LvnResult result = Lvn_Result_Failure;

    switch (ctx->graphicsapi)
    {
        case Lvn_GraphicsApi_None:
        {
            LVN_CORE_TRACE(lvnctx, "no graphics api selected, graphics related functions will not be set");
            return Lvn_Result_Success;
        }
        case Lvn_GraphicsApi_opengl:
        {
            // TODO: add opengl impl
            result = Lvn_Result_Success;
            break;
        }
        case Lvn_GraphicsApi_vulkan:
        {
#ifdef LVN_INCLUDE_VULKAN
            result = lvn::implVkInitGraphicsContext(ctx);
#endif
            break;
        }

        default:
        {
            LVN_CORE_ERROR("unrecognized graphics api: (%d), cannot create graphics api related functions", ctx->graphicsapi);
            return Lvn_Result_Failure;
        }
    }

    if (result != Lvn_Result_Success)
    {
        LVN_CORE_ERROR("could not create graphics api related functions for: %s", lvn::getGraphicsApiNameEnum(ctx->graphicsapi));
        return Lvn_Result_Failure;
    }

    LVN_CORE_TRACE("graphics api set: %s", lvn::getGraphicsApiNameEnum(ctx->graphicsapi));
    return result;
}

static void terminateGraphicsApiFuncs(LvnGraphicsContext* ctx)
{
    switch (ctx->graphicsapi)
    {
        case Lvn_GraphicsApi_None: { break; }
        case Lvn_GraphicsApi_opengl:
        {
            break;
        }
        case Lvn_GraphicsApi_vulkan:
        {
#ifdef LVN_INCLUDE_VULKAN
            lvn::implVkTerminateGraphicsContext();
#endif
            break;
        }

        default:
        {
            LVN_CORE_ERROR("unrecognized graphics api: (%d), cannot terminate graphics api related functions", ctx->graphicsapi);
            return;
        }
    }

    LVN_CORE_TRACE("graphics api terminated: %s", lvn::getGraphicsApiNameEnum(ctx->graphicsapi));
}

static void initStandardPipelineFixedFuncs(LvnGraphicsContext* graphicsctx)
{
    LvnPipelineFixedFunctions pipelineFixedFuncs{};

    // Input Assembly
    pipelineFixedFuncs.inputAssembly.topology = Lvn_TopologyType_Triangle;
    pipelineFixedFuncs.inputAssembly.primitiveRestartEnable = false;

    // Viewport
    pipelineFixedFuncs.viewport.x = 0.0f;
    pipelineFixedFuncs.viewport.y = 0.0f;
    pipelineFixedFuncs.viewport.width = 800.0f;
    pipelineFixedFuncs.viewport.height = 600.0f;
    pipelineFixedFuncs.viewport.minDepth = 0.0f;
    pipelineFixedFuncs.viewport.maxDepth = 1.0f;

    // Scissor
    pipelineFixedFuncs.scissor.offset = { 0, 0 };
    pipelineFixedFuncs.scissor.extent = { 800, 600 };

    // Rasterizer
    pipelineFixedFuncs.rasterizer.depthClampEnable = false;
    pipelineFixedFuncs.rasterizer.rasterizerDiscardEnable = false;
    pipelineFixedFuncs.rasterizer.lineWidth = 1.0f;
    pipelineFixedFuncs.rasterizer.cullMode = Lvn_CullFaceMode_Disable;
    pipelineFixedFuncs.rasterizer.frontFace = Lvn_CullFrontFace_Clockwise;
    pipelineFixedFuncs.rasterizer.depthBiasEnable = false;
    pipelineFixedFuncs.rasterizer.depthBiasConstantFactor = 0.0f;
    pipelineFixedFuncs.rasterizer.depthBiasClamp = 0.0f;
    pipelineFixedFuncs.rasterizer.depthBiasSlopeFactor = 0.0f;

    // MultiSampling
    pipelineFixedFuncs.multisampling.sampleShadingEnable = false;
    pipelineFixedFuncs.multisampling.rasterizationSamples = Lvn_SampleCount_1_Bit;
    pipelineFixedFuncs.multisampling.minSampleShading = 1.0f;
    pipelineFixedFuncs.multisampling.sampleMask = nullptr;
    pipelineFixedFuncs.multisampling.alphaToCoverageEnable = false;
    pipelineFixedFuncs.multisampling.alphaToOneEnable = false;

    // Color Attachments
    pipelineFixedFuncs.colorBlend.colorBlendAttachmentCount = 0; // If no attachments are provided, an attachment will automatically be created
    pipelineFixedFuncs.colorBlend.pColorBlendAttachments = nullptr; 

    // Color Blend
    pipelineFixedFuncs.colorBlend.logicOpEnable = false;
    pipelineFixedFuncs.colorBlend.blendConstants[0] = 0.0f;
    pipelineFixedFuncs.colorBlend.blendConstants[1] = 0.0f;
    pipelineFixedFuncs.colorBlend.blendConstants[2] = 0.0f;
    pipelineFixedFuncs.colorBlend.blendConstants[3] = 0.0f;

    // Depth Stencil
    pipelineFixedFuncs.depthstencil.enableDepth = false;
    pipelineFixedFuncs.depthstencil.depthOpCompare = Lvn_CompareOp_Never;
    pipelineFixedFuncs.depthstencil.enableStencil = false;
    pipelineFixedFuncs.depthstencil.stencil.compareMask = 0x00;
    pipelineFixedFuncs.depthstencil.stencil.writeMask = 0x00;
    pipelineFixedFuncs.depthstencil.stencil.reference = 0;
    pipelineFixedFuncs.depthstencil.stencil.compareOp = Lvn_CompareOp_Never;
    pipelineFixedFuncs.depthstencil.stencil.depthFailOp = Lvn_StencilOp_Keep;
    pipelineFixedFuncs.depthstencil.stencil.failOp = Lvn_StencilOp_Keep;
    pipelineFixedFuncs.depthstencil.stencil.passOp = Lvn_StencilOp_Keep;

    graphicsctx->defaultPipelineFixedFuncs = pipelineFixedFuncs;
}

static const char* getGraphicsApiNameEnum(LvnGraphicsApi api)
{
    switch (api)
    {
        case Lvn_GraphicsApi_None:   { return "none";   }
        case Lvn_GraphicsApi_vulkan: { return "vulkan"; }
        case Lvn_GraphicsApi_opengl: { return "opengl"; }
    }

    return "";
}

LvnGraphicsApi getGraphicsApi()
{
    return lvn::getGraphicsContext()->graphicsapi;
}

LvnResult createGraphicsContext(LvnContext* ctx, LvnGraphicsContext** graphicsctx, LvnGraphicsContextCreateInfo* createInfo)
{
    if (!ctx || !graphicsctx || !createInfo)
        return Lvn_Result_Failure;

    *graphicsctx = lvn::memNew<LvnGraphicsContext>();
    LvnGraphicsContext* gctx = *graphicsctx;
    gctx->corectx = ctx;
    gctx->graphicsapi = createInfo->graphicsapi;
    gctx->enableGraphicsApiDebugLogs = createInfo->enableGraphicsApiDebugLogs;

    lvn::initStandardPipelineFixedFuncs(gctx);

    // set contexts
    if (lvn::initGraphicsApiFuncs(gctx) != Lvn_Result_Success)
    {
        lvn::memDelete<LvnGraphicsContext>(gctx);
        *graphicsctx = nullptr;
        return Lvn_Result_Failure;
    }

    return Lvn_Result_Success;
}

void destroyGraphicsContext(LvnGraphicsContext* graphicsctx)
{
    if (!graphicsctx)
        return;

    lvn::terminateGraphicsApiFuncs(graphicsctx);
    lvn::memDelete<LvnGraphicsContext>(graphicsctx);
}

void getPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* deviceCount)
{
    uint32_t getDeviceCount;
    lvn::getGraphicsContext()->getPhysicalDevices(nullptr, &getDeviceCount);

    if (deviceCount != nullptr)
        *deviceCount = getDeviceCount;

    if (pPhysicalDevices == nullptr)
        return;

    lvn::getGraphicsContext()->getPhysicalDevices(pPhysicalDevices, &getDeviceCount);

    return;
}

LvnPhysicalDeviceProperties getPhysicalDeviceProperties(LvnPhysicalDevice* physicalDevice)
{
    return physicalDevice->properties;
}

LvnPhysicalDeviceFeatures getPhysicalDeviceFeatures(LvnPhysicalDevice* physicalDevice)
{
    return physicalDevice->features;
}

LvnResult checkPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice)
{
    if (physicalDevice == nullptr)
    {
        LVN_CORE_ERROR("cannot check physical device support, physicalDevice is nullptr");
        return Lvn_Result_Failure;
    }

    return lvn::getGraphicsContext()->checkPhysicalDeviceSupport(physicalDevice);
}

LvnResult setPhysicalDevice(LvnPhysicalDevice* physicalDevice)
{
    if (physicalDevice == nullptr)
    {
        LVN_CORE_ERROR("cannot set physical device, physicalDevice is nullptr");
        return Lvn_Result_Failure;
    }

    return lvn::getGraphicsContext()->setPhysicalDevice(physicalDevice);
}

void renderCmdDraw(LvnCommandBuffer* cmdBuffer, uint32_t vertexCount)
{
    lvn::getGraphicsContext()->renderCmdDraw(cmdBuffer, vertexCount);
}

void renderCmdDrawIndexed(LvnCommandBuffer* cmdBuffer, uint32_t indexCount)
{
    lvn::getGraphicsContext()->renderCmdDrawIndexed(cmdBuffer, indexCount);
}

void renderCmdDrawInstanced(LvnCommandBuffer* cmdBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance)
{
    lvn::getGraphicsContext()->renderCmdDrawInstanced(cmdBuffer, vertexCount, instanceCount, firstInstance);
}

void renderCmdDrawIndexedInstanced(LvnCommandBuffer* cmdBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance)
{
    lvn::getGraphicsContext()->renderCmdDrawIndexedInstanced(cmdBuffer, indexCount, instanceCount, firstInstance);
}

void renderCmdSetStencilReference(uint32_t reference)
{

}

void renderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask)
{

}

void renderBeginNextFrame(LvnWindow* window, LvnCommandBuffer* cmdBuffer)
{
    lvn::getGraphicsContext()->renderBeginNextFrame(window, cmdBuffer);
}

void renderDrawSubmit(LvnWindow* window, LvnCommandBuffer* cmdBuffer)
{
    lvn::getGraphicsContext()->renderDrawSubmit(window, cmdBuffer);
}

void renderBeginCommandRecording(LvnCommandBuffer* cmdBuffer)
{
    lvn::getGraphicsContext()->renderBeginCommandRecording(cmdBuffer);
}

void renderEndCommandRecording(LvnCommandBuffer* cmdBuffer)
{
    lvn::getGraphicsContext()->renderEndCommandRecording(cmdBuffer);
}

void renderCmdBeginRenderPass(LvnCommandBuffer* cmdBuffer, LvnWindow* window, float r, float g, float b, float a)
{
    lvn::getGraphicsContext()->renderCmdBeginRenderPass(cmdBuffer, window, r, g, b, a);
}

void renderCmdEndRenderPass(LvnCommandBuffer* cmdBuffer)
{
    lvn::getGraphicsContext()->renderCmdEndRenderPass(cmdBuffer);
}

void renderCmdBindPipeline(LvnCommandBuffer* cmdBuffer, LvnPipeline* pipeline)
{
    lvn::getGraphicsContext()->renderCmdBindPipeline(cmdBuffer, pipeline);
}

void renderCmdBindVertexBuffer(LvnCommandBuffer* cmdBuffer, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets)
{
    uint64_t offsets[] = {0};
    lvn::getGraphicsContext()->renderCmdBindVertexBuffer(cmdBuffer, firstBinding, bindingCount, pBuffers, pOffsets ? pOffsets : offsets);
}

void renderCmdBindIndexBuffer(LvnCommandBuffer* cmdBuffer, LvnBuffer* buffer, uint64_t offset)
{
    lvn::getGraphicsContext()->renderCmdBindIndexBuffer(cmdBuffer, buffer, offset);
}

void renderCmdBindDescriptorSets(LvnCommandBuffer* cmdBuffer, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets)
{
    lvn::getGraphicsContext()->renderCmdBindDescriptorSets(cmdBuffer, pipeline, firstSetIndex, descriptorSetCount, pDescriptorSets);
}

void renderCmdBeginFrameBuffer(LvnCommandBuffer* cmdBuffer, LvnFrameBuffer* frameBuffer)
{
    lvn::getGraphicsContext()->renderCmdBeginFrameBuffer(cmdBuffer, frameBuffer);
}

void renderCmdEndFrameBuffer(LvnCommandBuffer* cmdBuffer, LvnFrameBuffer* frameBuffer)
{
    lvn::getGraphicsContext()->renderCmdEndFrameBuffer(cmdBuffer, frameBuffer);
}

LvnResult createShaderFromSrc(LvnShader** shader, const LvnShaderCreateInfo* createInfo)
{
    if (createInfo->vertexSrc.empty())
    {
        LVN_CORE_ERROR("createShaderFromSrc(LvnShader**, LvnShaderCreateInfo*) | createInfo->vertexSrc is nullptr, cannot create shader without the vertex shader source");
        return Lvn_Result_Failure;
    }

    if (createInfo->fragmentSrc.empty())
    {
        LVN_CORE_ERROR("createShaderFromSrc(LvnShader**, LvnShaderCreateInfo*) | createInfo->fragmentSrc is nullptr, cannot create shader without the fragment shader source");
        return Lvn_Result_Failure;
    }

    *shader = lvn::createObject<LvnShader>(Lvn_Stype_Shader);

    LVN_CORE_TRACE("created shader (from source): (%p)", *shader);
    return lvn::getGraphicsContext()->createShaderFromSrc(*shader, createInfo);
}

LvnResult createShaderFromBin(LvnShader** shader, const LvnShaderBinCreateInfo* createInfo)
{
    if (!createInfo->vertexBin)
    {
        LVN_CORE_ERROR("createShaderFromFileSrc(LvnShader**, LvnShaderBinCreateInfo*) | createInfo->vertexBin is nullptr, cannot create shader without the vertex shader source");
        return Lvn_Result_Failure;
    }

    if (!createInfo->fragmentBin)
    {
        LVN_CORE_ERROR("createShaderFromFileSrc(LvnShader**, LvnShaderBinCreateInfo*) | createInfo->fragmentBin is nullptr, cannot create shader without the fragment shader source");
        return Lvn_Result_Failure;
    }

    *shader = lvn::createObject<LvnShader>(Lvn_Stype_Shader);

    LVN_CORE_TRACE("created shader (from source bin): (%p)", *shader);
    return lvn::getGraphicsContext()->createShaderFromBin(*shader, createInfo);
}

LvnResult createShaderFromFileSrc(LvnShader** shader, const LvnShaderCreateInfo* createInfo)
{
    if (createInfo->vertexSrc.empty())
    {
        LVN_CORE_ERROR("createShaderFromFileSrc(LvnShader**, LvnShaderCreateInfo*) | createInfo->vertexSrc is nullptr, cannot create shader without the vertex shader source");
        return Lvn_Result_Failure;
    }

    if (createInfo->fragmentSrc.empty())
    {
        LVN_CORE_ERROR("createShaderFromFileSrc(LvnShader**, LvnShaderCreateInfo*) | createInfo->fragmentSrc is nullptr, cannot create shader without the fragment shader source");
        return Lvn_Result_Failure;
    }

    *shader = lvn::createObject<LvnShader>(Lvn_Stype_Shader);

    LVN_CORE_TRACE("created shader (from source file): (%p), vertex file: %s, fragment file: %s", *shader, createInfo->vertexSrc.c_str(), createInfo->fragmentSrc.c_str());
    return lvn::getGraphicsContext()->createShaderFromFileSrc(*shader, createInfo);
}

LvnResult createShaderFromFileBin(LvnShader** shader, const LvnShaderCreateInfo* createInfo)
{
    if (createInfo->vertexSrc.empty())
    {
        LVN_CORE_ERROR("createShaderFileBin(LvnShader**, LvnShaderCreateInfo*) | createInfo->vertexSrc is nullptr, cannot create shader without the vertex shader source");
        return Lvn_Result_Failure;
    }

    if (createInfo->fragmentSrc.empty())
    {
        LVN_CORE_ERROR("createShaderFileBin(LvnShader**, LvnShaderCreateInfo*) | createInfo->fragmentSrc is nullptr, cannot create shader without the fragment shader source");
        return Lvn_Result_Failure;
    }

    *shader = lvn::createObject<LvnShader>(Lvn_Stype_Shader);

    LVN_CORE_TRACE("created shader (from binary file): (%p), vertex file: %s, fragment file: %s", *shader, createInfo->vertexSrc.c_str(), createInfo->fragmentSrc.c_str());
    return lvn::getGraphicsContext()->createShaderFromFileBin(*shader, createInfo);
}

LvnResult createDescriptorLayout(LvnDescriptorLayout** descriptorLayout, const LvnDescriptorLayoutCreateInfo* createInfo)
{
    if (!createInfo->descriptorBindingCount)
    {
        LVN_CORE_ERROR("createDescriptorLayout(LvnDescriptorLayout**, LvnDescriptorLayoutCreateInfo*) | createInfo->descriptorBindingCount is 0, cannot create descriptor layout without the descriptor bindings count");
        return Lvn_Result_Failure;
    }

    if (!createInfo->pDescriptorBindings)
    {
        LVN_CORE_ERROR("createDescriptorLayout(LvnDescriptorLayout**, LvnDescriptorLayoutCreateInfo*) | createInfo->pDescriptorBindings is nullptr, cannot create descriptor layout without the pointer to the array of descriptor bindings");
        return Lvn_Result_Failure;
    }

    for (uint32_t i = 0; i < createInfo->descriptorBindingCount; i++)
    {
        if (createInfo->pDescriptorBindings[i].maxAllocations == 0)
            LVN_CORE_WARN("createDescriptorLayout(LvnDescriptorLayout**, LvnDescriptorLayoutCreateInfo*) | createInfo->pDescriptorBindings[%u].maxAllocations is 0, no descriptors will be allocated for this binding which may not be intentional", i);

        if (createInfo->pDescriptorBindings[i].descriptorCount == 0)
            LVN_CORE_WARN("createDescriptorLayout(LvnDescriptorLayout**, LvnDescriptorLayoutCreateInfo*) | createInfo->pDescriptorBindings[%u].descriptorCount is 0, no descriptors will be created for this binding which may not be intentional", i);
    }

    *descriptorLayout = lvn::createObject<LvnDescriptorLayout>(Lvn_Stype_DescriptorLayout);

    LvnDescriptorLayout* descriptorLayoutPtr = *descriptorLayout;
    descriptorLayoutPtr->maxSets = createInfo->maxSets;
    descriptorLayoutPtr->descriptorSetIndex = 0;

    LVN_CORE_TRACE("created descriptorLayout: (%p), descriptor binding count: %u", *descriptorLayout, createInfo->descriptorBindingCount);
    return lvn::getGraphicsContext()->createDescriptorLayout(*descriptorLayout, createInfo);
}

LvnResult allocateCommandBuffers(LvnCommandPool* cmdPool, LvnCommandBuffer** pCmdBuffers, uint32_t count)
{
    if (!cmdPool)
    {
        LVN_CORE_ERROR("cannot allocate command buffers, cmdPool is nullptr");
        return Lvn_Result_Failure;
    }

    if (!pCmdBuffers)
    {
        LVN_CORE_ERROR("cannot allocate command buffers, pCmdBuffers is nullptr");
        return Lvn_Result_Failure;
    }

    return lvn::getGraphicsContext()->allocateCommandBuffers(cmdPool, pCmdBuffers, count);
}

LvnResult allocateDescriptorSets(LvnDescriptorLayout* descriptorLayout, LvnDescriptorSet** pDescriptorSets, uint32_t count)
{
    LVN_ASSERT(descriptorLayout != nullptr, "descriptorLayout is nullptr");
    LVN_ASSERT(pDescriptorSets != nullptr, "pDescriptorSets is nullptr");

    LvnVector<LvnDescriptorSet> descripoptorSets(count);
    descriptorLayout->descriptorSets.push_back(lvn::move(descripoptorSets));
    *pDescriptorSets = descriptorLayout->descriptorSets.back().data();

    LVN_CORE_TRACE("allocated descriptorSets: (%p) from descriptorLayout: (%p)", *pDescriptorSets, descriptorLayout);
    return lvn::getGraphicsContext()->allocateDescriptorSet(descriptorLayout, pDescriptorSets, count);
}

LvnResult createPipeline(LvnPipeline** pipeline, const LvnPipelineCreateInfo* createInfo)
{
    // vertex binding descriptions
    if (!createInfo->pVertexBindingDescriptions)
    {
        LVN_CORE_ERROR("createBuffer(LvnBuffer*, LvnBufferCreateInfo*) | createInfo->pVertexBindingDescriptions is nullptr; cannot create vertex buffer without the vertex binding descriptions");
        return Lvn_Result_Failure;
    }
    else if (!createInfo->vertexBindingDescriptionCount)
    {
        LVN_CORE_ERROR("createBuffer(LvnBuffer*, LvnBufferCreateInfo*) | createInfo->vertexBindingDescriptionCount is 0; cannot create vertex buffer without the vertex binding descriptions");
        return Lvn_Result_Failure;
    }

    // vertex attributes
    if (!createInfo->pVertexAttributes)
    {
        LVN_CORE_ERROR("createBuffer(LvnBuffer*, LvnBufferCreateInfo*) | createInfo->pVertexAttributes is nullptr; cannot create vertex buffer without the vertex attributes");
        return Lvn_Result_Failure;
    }
    else if (!createInfo->vertexAttributeCount)
    {
        LVN_CORE_ERROR("createBuffer(LvnBuffer*, LvnBufferCreateInfo*) | createInfo->vertexAttributeCount is 0; cannot create vertex buffer without the vertex attributes");
        return Lvn_Result_Failure;
    }

    for (uint32_t i = 0; i < createInfo->vertexAttributeCount; i++)
    {
        if (createInfo->pVertexAttributes[i].format == Lvn_AttributeFormat_Undefined)
        {
            LVN_CORE_ERROR("createBuffer(LvnBuffer*, LvnBufferCreateInfo*) | createInfo->pVertexAttributes[%d].type is Lvn_AttributeFormat_Undefined, cannot create vertex buffer without a vertex data type", i);
            return Lvn_Result_Failure;
        }
    }

    *pipeline = lvn::createObject<LvnPipeline>(Lvn_Stype_Pipeline);

    LVN_CORE_TRACE("created pipeline: (%p)", *pipeline);
    return lvn::getGraphicsContext()->createPipeline(*pipeline, createInfo);
}

LvnResult createCommandPool(LvnCommandPool** cmdPool)
{
    *cmdPool = lvn::createObject<LvnCommandPool>(Lvn_Stype_CommandPool);

    LVN_CORE_TRACE("created command pool: (%p)", *cmdPool);
    return lvn::getGraphicsContext()->createCommandPool(*cmdPool);
}

LvnResult createFrameBuffer(LvnFrameBuffer** frameBuffer, const LvnFrameBufferCreateInfo* createInfo)
{
    if (createInfo->pColorAttachments == nullptr)
    {
        LVN_CORE_ERROR("createFrameBuffer(LvnFrameBuffer**, LvnFrameBufferCreateInfo*) | createInfo->pColorAttachments is nullptr, cannot create framebuffer without one or more color attachments");
        return Lvn_Result_Failure;
    }

    uint32_t totalAttachments = createInfo->colorAttachmentCount + (createInfo->depthAttachment != nullptr ? 1 : 0);

    for (uint32_t i = 0; i < createInfo->colorAttachmentCount; i++)
    {
        if (createInfo->pColorAttachments[i].index >= totalAttachments)
        {
            LVN_CORE_ERROR("createFrameBuffer(LvnFrameBuffer**, LvnFrameBufferCreateInfo*) | createInfo->pColorAttachments[%u].index is greater than or equal to total attachments, color attachment index must be less than the total number of attachments", i);
            return Lvn_Result_Failure;
        }
        if (createInfo->depthAttachment != nullptr && createInfo->pColorAttachments[i].index == createInfo->depthAttachment->index)
        {
            LVN_CORE_ERROR("createFrameBuffer(LvnFrameBuffer**, LvnFrameBufferCreateInfo*) | createInfo->pColorAttachments[%u].index has the same value as createInfo->depthAttachment->index, color attachment index must not be the same as the depth attachment index", i);
            return Lvn_Result_Failure;
        }
    }

    if (createInfo->depthAttachment != nullptr)
    {
        if (createInfo->depthAttachment->index >= totalAttachments)
        {
            LVN_CORE_ERROR("createFrameBuffer(LvnFrameBuffer**, LvnFrameBufferCreateInfo*) | createInfo->pColorAttachments[%u].index is greater than or equal to total attachments, depth attachment index must be less than the total number of attachments");
            return Lvn_Result_Failure;
        }
    }

    *frameBuffer = lvn::createObject<LvnFrameBuffer>(Lvn_Stype_FrameBuffer);

    LVN_CORE_TRACE("created framebuffer: (%p)", *frameBuffer);
    return lvn::getGraphicsContext()->createFrameBuffer(*frameBuffer, createInfo);
}

LvnResult createBuffer(LvnBuffer** buffer, const LvnBufferCreateInfo* createInfo)
{
    // check valid buffer type
    if (createInfo->type == Lvn_BufferType_Unknown)
    {
        LVN_CORE_ERROR("createBuffer(LvnBuffer*, LvnBufferCreateInfo*) | createInfo->type is \'Lvn_BufferType_Unknown\'; cannot create vertex buffer without knowing the type of buffer usage");
        return Lvn_Result_Failure;
    }

    *buffer = lvn::createObject<LvnBuffer>(Lvn_Stype_Buffer);

    LVN_CORE_TRACE("created buffer: (%p)", *buffer);
    return lvn::getGraphicsContext()->createBuffer(*buffer, createInfo);
}

LvnResult createSampler(LvnSampler** sampler, const LvnSamplerCreateInfo* createInfo)
{
    *sampler = lvn::createObject<LvnSampler>(Lvn_Stype_Sampler);

    LVN_CORE_TRACE("created sampler: (%p)");
    return lvn::getGraphicsContext()->createSampler(*sampler, createInfo);
}

LvnResult createTexture(LvnTexture** texture, const LvnTextureCreateInfo* createInfo)
{
    *texture = lvn::createObject<LvnTexture>(Lvn_Stype_Texture);

    LVN_CORE_TRACE("created texture: (%p) using image data: (%p), (w:%u,h:%u,ch:%u), total size: %u bytes",
        *texture,
        createInfo->imageData.pixels.data(),
        createInfo->imageData.width,
        createInfo->imageData.height,
        createInfo->imageData.channels,
        createInfo->imageData.pixels.memsize());

    return lvn::getGraphicsContext()->createTexture(*texture, createInfo);
}

LvnResult createCubemap(LvnCubemap** cubemap, const LvnCubemapCreateInfo* createInfo)
{
    if (createInfo->posx.pixels.data() == nullptr)
    {
        LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->posx.pixels.data() is nullptr");
        return Lvn_Result_Failure;
    }
    if (createInfo->negx.pixels.data() == nullptr)
    {
        LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->negx.pixels.data() is nullptr");
        return Lvn_Result_Failure;
    }
    if (createInfo->posy.pixels.data() == nullptr)
    {
        LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->posy.pixels.data() is nullptr");
        return Lvn_Result_Failure;
    }
    if (createInfo->negy.pixels.data() == nullptr)
    {
        LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->negy.pixels.data() is nullptr");
        return Lvn_Result_Failure;
    }
    if (createInfo->posz.pixels.data() == nullptr)
    {
        LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->posz.pixels.data() is nullptr");
        return Lvn_Result_Failure;
    }
    if (createInfo->negz.pixels.data() == nullptr)
    {
        LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->negz.pixels.data() is nullptr");
        return Lvn_Result_Failure;
    }

    // if(!(createInfo->posx.width * createInfo->posx.height ==
    //  createInfo->negx.width * createInfo->negx.height ==
    //  createInfo->posy.width * createInfo->posy.height ==
    //  createInfo->negy.width * createInfo->negy.height ==
    //  createInfo->posz.width * createInfo->posz.height ==
    //  createInfo->negz.width * createInfo->negz.height))
    // {
    //  LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | not all images have the same dimensions, all cubemap images must have the same width and height");
    //  return Lvn_Result_Failure;
    // }

    *cubemap = lvn::createObject<LvnCubemap>(Lvn_Stype_Cubemap);

    LVN_CORE_TRACE("created cubemap: (%p)", *cubemap);
    return lvn::getGraphicsContext()->createCubemap(*cubemap, createInfo);
}

void destroyShader(LvnShader* shader)
{
    if (shader == nullptr) { return; }
    lvn::getGraphicsContext()->destroyShader(shader);
    lvn::destroyObject<LvnShader>(shader, Lvn_Stype_Shader);
}

void destroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout)
{
    if (descriptorLayout == nullptr) { return; }
    lvn::getGraphicsContext()->destroyDescriptorLayout(descriptorLayout);
    lvn::destroyObject<LvnDescriptorLayout>(descriptorLayout, Lvn_Stype_DescriptorLayout);
}

void destroyPipeline(LvnPipeline* pipeline)
{
    if (pipeline == nullptr) { return; }
    lvn::getGraphicsContext()->destroyPipeline(pipeline);
    lvn::destroyObject<LvnPipeline>(pipeline, Lvn_Stype_Pipeline);
}

void destroyCommandPool(LvnCommandPool* cmdPool)
{
    lvn::getGraphicsContext()->destroyCommandPool(cmdPool);
    lvn::destroyObject<LvnCommandPool>(cmdPool, Lvn_Stype_CommandPool);
}

void destroyFrameBuffer(LvnFrameBuffer* frameBuffer)
{
    if (frameBuffer == nullptr) { return; }
    lvn::getGraphicsContext()->destroyFrameBuffer(frameBuffer);
    lvn::destroyObject<LvnFrameBuffer>(frameBuffer, Lvn_Stype_FrameBuffer);
}

void destroyBuffer(LvnBuffer* buffer)
{
    if (buffer == nullptr) { return; }
    lvn::getGraphicsContext()->destroyBuffer(buffer);
    lvn::destroyObject<LvnBuffer>(buffer, Lvn_Stype_Buffer);
}

void destroySampler(LvnSampler* sampler)
{
    if (sampler == nullptr) { return; }
    lvn::getGraphicsContext()->destroySampler(sampler);
    lvn::destroyObject<LvnSampler>(sampler, Lvn_Stype_Sampler);
}

void destroyTexture(LvnTexture* texture)
{
    if (texture == nullptr) { return; }
    lvn::getGraphicsContext()->destroyTexture(texture);
    lvn::destroyObject<LvnTexture>(texture, Lvn_Stype_Texture);
}

void destroyCubemap(LvnCubemap* cubemap)
{
    if (cubemap == nullptr) { return; }
    lvn::getGraphicsContext()->destroyCubemap(cubemap);
    lvn::destroyObject<LvnCubemap>(cubemap, Lvn_Stype_Cubemap);
}

uint32_t getAttributeFormatSize(LvnAttributeFormat format)
{
    switch (format)
    {
        case Lvn_AttributeFormat_Undefined:        { return 0; }
        case Lvn_AttributeFormat_Scalar_f32:       { return sizeof(float); }
        case Lvn_AttributeFormat_Scalar_f64:       { return sizeof(double); }
        case Lvn_AttributeFormat_Scalar_i32:       { return sizeof(int32_t); }
        case Lvn_AttributeFormat_Scalar_ui32:      { return sizeof(uint32_t); }
        case Lvn_AttributeFormat_Scalar_i8:        { return sizeof(int8_t); }
        case Lvn_AttributeFormat_Scalar_ui8:       { return sizeof(uint8_t); }
        case Lvn_AttributeFormat_Vec2_f32:         { return 2 * sizeof(float); }
        case Lvn_AttributeFormat_Vec3_f32:         { return 3 * sizeof(float); }
        case Lvn_AttributeFormat_Vec4_f32:         { return 4 * sizeof(float); }
        case Lvn_AttributeFormat_Vec2_f64:         { return 2 * sizeof(double); }
        case Lvn_AttributeFormat_Vec3_f64:         { return 3 * sizeof(double); }
        case Lvn_AttributeFormat_Vec4_f64:         { return 4 * sizeof(double); }
        case Lvn_AttributeFormat_Vec2_i32:         { return 2 * sizeof(int32_t); }
        case Lvn_AttributeFormat_Vec3_i32:         { return 3 * sizeof(int32_t); }
        case Lvn_AttributeFormat_Vec4_i32:         { return 4 * sizeof(int32_t); }
        case Lvn_AttributeFormat_Vec2_ui32:        { return 2 * sizeof(uint32_t); }
        case Lvn_AttributeFormat_Vec3_ui32:        { return 3 * sizeof(uint32_t); }
        case Lvn_AttributeFormat_Vec4_ui32:        { return 4 * sizeof(uint32_t); }
        case Lvn_AttributeFormat_Vec2_i8:          { return 2 * sizeof(int8_t); }
        case Lvn_AttributeFormat_Vec3_i8:          { return 3 * sizeof(int8_t); }
        case Lvn_AttributeFormat_Vec4_i8:          { return 4 * sizeof(int8_t); }
        case Lvn_AttributeFormat_Vec2_ui8:         { return 2 * sizeof(uint8_t); }
        case Lvn_AttributeFormat_Vec3_ui8:         { return 3 * sizeof(uint8_t); }
        case Lvn_AttributeFormat_Vec4_ui8:         { return 4 * sizeof(uint8_t); }
        case Lvn_AttributeFormat_Vec2_n8:          { return 2 * sizeof(int8_t); }
        case Lvn_AttributeFormat_Vec3_n8:          { return 3 * sizeof(int8_t); }
        case Lvn_AttributeFormat_Vec4_n8:          { return 4 * sizeof(int8_t); }
        case Lvn_AttributeFormat_Vec2_un8:         { return 2 * sizeof(uint8_t); }
        case Lvn_AttributeFormat_Vec3_un8:         { return 3 * sizeof(uint8_t); }
        case Lvn_AttributeFormat_Vec4_un8:         { return 4 * sizeof(uint8_t); }
        case Lvn_AttributeFormat_2_10_10_10_ile:   { return sizeof(int32_t); }
        case Lvn_AttributeFormat_2_10_10_10_uile:  { return sizeof(uint32_t); }
        case Lvn_AttributeFormat_2_10_10_10_nle:   { return sizeof(int32_t); }
        case Lvn_AttributeFormat_2_10_10_10_unle:  { return sizeof(uint32_t); }

        default:
        {
            LVN_CORE_WARN("unknown vertex data type enum: (%u)", format);
            return 0;
        }
    }
}

uint32_t getAttributeFormatComponentSize(LvnAttributeFormat format)
{
    switch (format)
    {
        case Lvn_AttributeFormat_Undefined:        { return 0; }
        case Lvn_AttributeFormat_Scalar_f32:       { return 1; }
        case Lvn_AttributeFormat_Scalar_f64:       { return 1; }
        case Lvn_AttributeFormat_Scalar_i32:       { return 1; }
        case Lvn_AttributeFormat_Scalar_ui32:      { return 1; }
        case Lvn_AttributeFormat_Scalar_i8:        { return 1; }
        case Lvn_AttributeFormat_Scalar_ui8:       { return 1; }
        case Lvn_AttributeFormat_Vec2_f32:         { return 2; }
        case Lvn_AttributeFormat_Vec3_f32:         { return 3; }
        case Lvn_AttributeFormat_Vec4_f32:         { return 4; }
        case Lvn_AttributeFormat_Vec2_f64:         { return 2; }
        case Lvn_AttributeFormat_Vec3_f64:         { return 3; }
        case Lvn_AttributeFormat_Vec4_f64:         { return 4; }
        case Lvn_AttributeFormat_Vec2_i32:         { return 2; }
        case Lvn_AttributeFormat_Vec3_i32:         { return 3; }
        case Lvn_AttributeFormat_Vec4_i32:         { return 4; }
        case Lvn_AttributeFormat_Vec2_ui32:        { return 2; }
        case Lvn_AttributeFormat_Vec3_ui32:        { return 3; }
        case Lvn_AttributeFormat_Vec4_ui32:        { return 4; }
        case Lvn_AttributeFormat_Vec2_i8:          { return 2; }
        case Lvn_AttributeFormat_Vec3_i8:          { return 3; }
        case Lvn_AttributeFormat_Vec4_i8:          { return 4; }
        case Lvn_AttributeFormat_Vec2_ui8:         { return 2; }
        case Lvn_AttributeFormat_Vec3_ui8:         { return 3; }
        case Lvn_AttributeFormat_Vec4_ui8:         { return 4; }
        case Lvn_AttributeFormat_Vec2_n8:          { return 2; }
        case Lvn_AttributeFormat_Vec3_n8:          { return 3; }
        case Lvn_AttributeFormat_Vec4_n8:          { return 4; }
        case Lvn_AttributeFormat_Vec2_un8:         { return 2; }
        case Lvn_AttributeFormat_Vec3_un8:         { return 3; }
        case Lvn_AttributeFormat_Vec4_un8:         { return 4; }
        case Lvn_AttributeFormat_2_10_10_10_ile:   { return 4; }
        case Lvn_AttributeFormat_2_10_10_10_uile:  { return 4; }
        case Lvn_AttributeFormat_2_10_10_10_nle:   { return 4; }
        case Lvn_AttributeFormat_2_10_10_10_unle:  { return 4; }

        default:
        {
            LVN_CORE_WARN("unknown vertex data type enum: (%u)", format);
            return 0;
        }
    }
}

bool isAttributeFormatNormalizedType(LvnAttributeFormat format)
{
    switch (format)
    {
        case Lvn_AttributeFormat_Vec2_n8:          { return true; }
        case Lvn_AttributeFormat_Vec3_n8:          { return true; }
        case Lvn_AttributeFormat_Vec4_n8:          { return true; }
        case Lvn_AttributeFormat_Vec2_un8:         { return true; }
        case Lvn_AttributeFormat_Vec3_un8:         { return true; }
        case Lvn_AttributeFormat_Vec4_un8:         { return true; }
        case Lvn_AttributeFormat_2_10_10_10_nle:   { return true; }
        case Lvn_AttributeFormat_2_10_10_10_unle:  { return true; }

        default: { return false; }
    }
}

void pipelineSpecificationSetConfig(LvnPipelineFixedFunctions* pipelineFixedFunc)
{
    LVN_ASSERT(pipelineFixedFunc != nullptr, "pipeline specification points to nullptr when setting pipeline specification config");
    lvn::getGraphicsContext()->defaultPipelineFixedFuncs = *pipelineFixedFunc;
}

LvnPipelineFixedFunctions configPipelineFixedFuncInit()
{
    return lvn::getGraphicsContext()->defaultPipelineFixedFuncs;
}

void bufferUpdateData(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset)
{
    if (buffer->usage == Lvn_BufferUsage_Static)
    {
        LVN_CORE_ERROR("[opengl] cannot change data of buffer that has static buffer usage set Lvn_BufferUsage_Static, buffer: (%p)", buffer);
        return;
    }

    lvn::getGraphicsContext()->bufferUpdateData(buffer, data, size, offset);
}

void bufferResize(LvnBuffer* buffer, uint64_t size)
{
    if (buffer->usage != Lvn_BufferUsage_Resize)
    {
        LVN_CORE_ERROR("[opengl] cannot change data of buffer that does not have resize buffer usage set Lvn_BufferUsage_Resize, buffer: (%p)", buffer);
        return;
    }

    lvn::getGraphicsContext()->bufferResize(buffer, size);
}

LvnTexture* cubemapGetTextureData(LvnCubemap* cubemap)
{
    return &cubemap->textureData;
}

void updateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count)
{
    // TODO: add update error logs
    lvn::getGraphicsContext()->updateDescriptorSetData(descriptorSet, pUpdateInfo, count);
}

LvnTexture* frameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex)
{
    return lvn::getGraphicsContext()->frameBufferGetImage(frameBuffer, attachmentIndex);
}

LvnRenderPass* frameBufferGetRenderPass(LvnFrameBuffer* frameBuffer)
{
    return lvn::getGraphicsContext()->frameBufferGetRenderPass(frameBuffer);
}

void frameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height)
{
    if (width * height == 0)
        return;

    lvn::getGraphicsContext()->framebufferResize(frameBuffer, width, height);
}

void frameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a)
{
    lvn::getGraphicsContext()->frameBufferSetClearColor(frameBuffer, attachmentIndex, r, g, b, a);
}

LvnDepthImageFormat findSupportedDepthImageFormat(LvnDepthImageFormat* pDepthImageFormats, uint32_t count)
{
    if (pDepthImageFormats == nullptr)
    {
        LVN_CORE_ERROR("cannot find supported depth image format, no depth image candidates given");
        return (LvnDepthImageFormat)(0);
    }

    return lvn::getGraphicsContext()->findSupportedDepthImageFormat(pDepthImageFormats, count);
}

LvnImage loadImageData(const char* filepath, int forceChannels, bool flipVertically)
{
    if (filepath == nullptr)
    {
        LVN_CORE_ERROR("loadImageData(const char*, int, bool) | invalid filepath, filepath must not be nullptr");
        return {};
    }

    if (forceChannels < 0)
    {
        LVN_CORE_ERROR("loadImageData(const char*, int, bool) | forceChannels < 0, channels cannot be negative");
        return {};
    }
    else if (forceChannels > 4)
    {
        LVN_CORE_ERROR("loadImageData(const char*, int, bool) | forceChannels > 4, channels cannot be higher than 4 components (rgba)");
        return {};
    }

    stbi_set_flip_vertically_on_load(flipVertically);
    int imageWidth, imageHeight, imageChannels;
    stbi_uc* pixels = stbi_load(filepath, &imageWidth, &imageHeight, &imageChannels, forceChannels);

    if (!pixels)
    {
        LVN_CORE_ERROR("loadImageData(const char*, int, bool) | failed to load image pixel data from file: %s", filepath);
        return {};
    }

    LvnImage imageData{};
    imageData.width = imageWidth;
    imageData.height = imageHeight;
    imageData.channels = forceChannels ? forceChannels : imageChannels;
    uint64_t imgsize = imageData.width * imageData.height * imageData.channels;
    imageData.pixels = LvnVector<uint8_t>(pixels, imgsize);

    LVN_CORE_TRACE("loaded image data <unsigned char*> (%p), (w:%u,h:%u,ch:%u), total memory size: %u bytes, filepath: %s", pixels, imageData.width, imageData.height, imageData.channels, imgsize, filepath);

    stbi_image_free(pixels);

    return imageData;
}

LvnImage loadImageDataMemory(const uint8_t* data, int length, int forceChannels, bool flipVertically)
{
    if (!data)
    {
        LVN_CORE_ERROR("loadImageDataMemory(const unsigned char*, int, int, bool) | invalid data, image memory data must not be nullptr");
        return {};
    }

    if (forceChannels < 0)
    {
        LVN_CORE_ERROR("loadImageDataMemory(conts unsigned char*, int, int, bool) | forceChannels < 0, channels cannot be negative");
        return {};
    }
    else if (forceChannels > 4)
    {
        LVN_CORE_ERROR("loadImageDataMemory(const unsigned char*, int, int, bool) | forceChannels > 4, channels cannot be higher than 4 components (rgba)");
        return {};
    }

    stbi_set_flip_vertically_on_load(flipVertically);
    int imageWidth, imageHeight, imageChannels;
    stbi_uc* pixels = stbi_load_from_memory(data, length, &imageWidth, &imageHeight, &imageChannels, forceChannels);

    if (!pixels)
    {
        LVN_CORE_ERROR("loadImageDataMemory(const unsigned char*) | failed to load image pixel data from memory: %p", data);
        return {};
    }

    LvnImage imageData{};
    imageData.width = imageWidth;
    imageData.height = imageHeight;
    imageData.channels = forceChannels ? forceChannels : imageChannels;
    uint64_t imgsize = imageData.width * imageData.height * imageData.channels;
    imageData.pixels = LvnVector<uint8_t>(pixels, imgsize);

    LVN_CORE_TRACE("loaded image data from memory <unsigned char*> (%p), (w:%u,h:%u,ch:%u), total memory size: %u bytes", pixels, imageData.width, imageData.height, imageData.channels, imgsize);

    stbi_image_free(pixels);

    return imageData;
}

LvnImage loadImageDataThread(const LvnString filepath, int forceChannels, bool flipVertically)
{
    if (filepath.empty())
    {
        LVN_CORE_ERROR("loadImageDataThread(const char*, int, bool) | invalid filepath, filepath is empty string");
        return {};
    }

    if (forceChannels < 0)
    {
        LVN_CORE_ERROR("loadImageDataThread(const char*, int, bool) | forceChannels < 0, channels cannot be negative");
        return {};
    }
    else if (forceChannels > 4)
    {
        LVN_CORE_ERROR("loadImageDataThread(const char*, int, bool) | forceChannels > 4, channels cannot be higher than 4 components (rgba)");
        return {};
    }

    stbi_set_flip_vertically_on_load_thread(flipVertically);
    int imageWidth, imageHeight, imageChannels;
    stbi_uc* pixels = stbi_load(filepath.c_str(), &imageWidth, &imageHeight, &imageChannels, forceChannels);

    if (!pixels)
    {
        LVN_CORE_ERROR("loadImageDataThread(const char*, int, bool) | failed to load image pixel data from file: %s", filepath.c_str());
        return {};
    }

    LvnImage imageData{};
    imageData.width = imageWidth;
    imageData.height = imageHeight;
    imageData.channels = forceChannels ? forceChannels : imageChannels;
    uint64_t imgsize = imageData.width * imageData.height * imageData.channels;
    imageData.pixels = LvnVector<uint8_t>(pixels, imgsize);

    LVN_CORE_TRACE("loaded image data <unsigned char*> (%p), (w:%u,h:%u,ch:%u), total memory size: %u bytes, filepath: %s", pixels, imageData.width, imageData.height, imageData.channels, imgsize, filepath.c_str());

    stbi_image_free(pixels);

    return imageData;
}

LvnImage loadImageDataMemoryThread(const uint8_t* data, int length, int forceChannels, bool flipVertically)
{
    if (!data)
    {
        LVN_CORE_ERROR("loadImageDataMemoryThread(const unsigned char*, int, int, bool) | invalid data, image memory data must not be nullptr");
        return {};
    }

    if (forceChannels < 0)
    {
        LVN_CORE_ERROR("loadImageDataMemoryThread(conts unsigned char*, int, int, bool) | forceChannels < 0, channels cannot be negative");
        return {};
    }
    else if (forceChannels > 4)
    {
        LVN_CORE_ERROR("loadImageDataMemoryThread(const unsigned char*, int, int, bool) | forceChannels > 4, channels cannot be higher than 4 components (rgba)");
        return {};
    }

    stbi_set_flip_vertically_on_load_thread(flipVertically);
    int imageWidth, imageHeight, imageChannels;
    stbi_uc* pixels = stbi_load_from_memory(data, length, &imageWidth, &imageHeight, &imageChannels, forceChannels);

    if (!pixels)
    {
        LVN_CORE_ERROR("loadImageDataMemoryThread(const unsigned char*) | failed to load image pixel data from memory: %p", data);
        return {};
    }

    LvnImage imageData{};
    imageData.width = imageWidth;
    imageData.height = imageHeight;
    imageData.channels = forceChannels ? forceChannels : imageChannels;
    uint64_t imgsize = imageData.width * imageData.height * imageData.channels;
    imageData.pixels = LvnVector<uint8_t>(pixels, imgsize);

    LVN_CORE_TRACE("loaded image data from memory <unsigned char*> (%p), (w:%u,h:%u,ch:%u), total memory size: %u bytes", pixels, imageData.width, imageData.height, imageData.channels, imgsize);

    stbi_image_free(pixels);

    return imageData;
}

LvnResult writeImagePng(const LvnImage& imageData, const char* filename)
{
    int stride = imageData.width * imageData.channels;
    int result = stbi_write_png(filename, (int)imageData.width, (int)imageData.height, (int)imageData.channels, imageData.pixels.data(), stride);
    return result ? Lvn_Result_Success : Lvn_Result_Failure;
}

LvnResult writeImageJpg(const LvnImage& imageData, const char* filename, int quality)
{
    int result = stbi_write_jpg(filename, imageData.width, imageData.height, imageData.channels, imageData.pixels.data(), quality);
    return result ? Lvn_Result_Success : Lvn_Result_Failure;
}

LvnResult writeImageBmp(const LvnImage& imageData, const char* filename)
{
    int result = stbi_write_bmp(filename, imageData.width, imageData.height, imageData.channels, imageData.pixels.data());
    return result ? Lvn_Result_Success : Lvn_Result_Failure;
}

void imageFlipVertically(LvnImage& imageData)
{
    uint8_t* data = imageData.pixels.data();
    uint32_t rowSize = imageData.width * imageData.channels;
    LvnVector<uint8_t> tempRow(rowSize);

    for (uint32_t y = 0; y < imageData.height / 2; y++)
    {
        uint8_t* rowTop = data + y * rowSize;
        uint8_t* rowBottom = data + (imageData.height - y - 1) * rowSize;

        memcpy(tempRow.data(), rowTop, rowSize);
        memcpy(rowTop, rowBottom, rowSize);
        memcpy(rowBottom, tempRow.data(), rowSize);
    }
}

void imageFlipHorizontally(LvnImage& imageData)
{
    uint8_t* data = imageData.pixels.data();

    for (uint32_t y = 0; y < imageData.height; y++)
    {
        uint8_t* row = data + y * imageData.width * imageData.channels;

        for (uint32_t x = 0; x < imageData.width / 2; x++)
        {
            uint8_t* leftpx = row + x * imageData.channels;
            uint8_t* rightpx = row + (imageData.width - x - 1) * imageData.channels;

            for (uint32_t c = 0; c < imageData.channels; c++)
                lvn::swap(leftpx[c], rightpx[c]);
        }
    }
}

void imageRotateCW(LvnImage& imageData)
{
    uint8_t* data = imageData.pixels.data();
    uint32_t newWidth = imageData.height;
    uint32_t newHeight = imageData.width;

    LvnVector<uint8_t> rotated(newWidth * newHeight * imageData.channels);

    for (uint32_t y = 0; y < imageData.height; y++)
    {
        for (uint32_t x = 0; x < imageData.width; x++)
        {
            for (uint32_t c = 0; c < imageData.channels; c++)
            {
                uint32_t srcIndex = (y * imageData.width + x) * imageData.channels + c;
                uint32_t dstx = imageData.height - 1 - y;
                uint32_t dsty = x;
                uint32_t dstIndex = (dsty * newWidth + dstx) * imageData.channels + c;
                rotated[dstIndex] = data[srcIndex];
            }
        }
    }

    imageData.pixels = lvn::move(LvnVector<uint8_t>(rotated.data(), rotated.size()));
    lvn::swap(imageData.width, imageData.height);
}

void imageRotateCCW(LvnImage& imageData)
{
    uint8_t* data = imageData.pixels.data();
    uint32_t newWidth = imageData.height;
    uint32_t newHeight = imageData.width;

    LvnVector<uint8_t> rotated(newWidth * newHeight * imageData.channels);

    for (uint32_t y = 0; y < imageData.height; y++)
    {
        for (uint32_t x = 0; x < imageData.width; x++)
        {
            for (uint32_t c = 0; c < imageData.channels; c++)
            {
                uint32_t srcIndex = (y * imageData.width + x) * imageData.channels + c;
                uint32_t dstx = y;
                uint32_t dsty = imageData.width - 1 - x;
                uint32_t dstIndex = (dsty * newWidth + dstx) * imageData.channels + c;
                rotated[dstIndex] = data[srcIndex];
            }
        }
    }

    imageData.pixels = lvn::move(LvnVector<uint8_t>(rotated.data(), rotated.size()));
    lvn::swap(imageData.width, imageData.height);
}

LvnImage imageGenColor(uint32_t width, uint32_t height, uint32_t channels, const LvnColorImageData& color)
{
    LVN_ASSERT(channels > 0 && channels <= 4, "channels must be within 0 to 4");

    uint32_t imgSize = width * height * channels;
    uint8_t* imgBuff = (uint8_t*)lvn::memAlloc(imgSize);

    for (uint32_t y = 0; y < height; y++)
        for (uint32_t x = 0; x < width; x++)
            for (uint32_t c = 0; c < channels; c++)
                imgBuff[y * width * channels + x * channels + c] = color[c];

    LvnImage imageData{};
    imageData.width = width;
    imageData.height = height;
    imageData.channels = channels;
    uint64_t imgsize = imageData.width * imageData.height * imageData.channels;
    imageData.pixels = LvnVector<uint8_t>(imgBuff, imgSize);

    lvn::memFree(imgBuff);
    return imageData;
}

LvnImage imageGenWhiteNoise(uint32_t width, uint32_t height, uint32_t channels)
{
    return lvn::imageGenWhiteNoise(width, height, channels, time(0));
}

LvnImage imageGenWhiteNoise(uint32_t width, uint32_t height, uint32_t channels, uint32_t seed)
{
    LVN_ASSERT(channels > 0 && channels <= 4, "channels must be within 0 to 4");
    srand(seed);

    uint32_t imgSize = width * height * channels;
    uint8_t* imgBuff = (uint8_t*)lvn::memAlloc(imgSize);

    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
            int rn = rand() % 2;
            for (uint32_t c = 0; c < channels; c++)
                imgBuff[y * width * channels + x * channels + c] = (c == 3 ? 255 : (rn ? 255 : 0));
        }
    }

    LvnImage imageData{};
    imageData.width = width;
    imageData.height = height;
    imageData.channels = channels;
    uint64_t imgsize = imageData.width * imageData.height * imageData.channels;
    imageData.pixels = LvnVector<uint8_t>(imgBuff, imgsize);

    lvn::memFree(imgBuff);
    return imageData;
}

LvnImage imageGenGrayScaleNoise(uint32_t width, uint32_t height, uint32_t channels)
{
    return lvn::imageGenGrayScaleNoise(width, height, channels, time(0));
}

LvnImage imageGenGrayScaleNoise(uint32_t width, uint32_t height, uint32_t channels, uint32_t seed)
{
    LVN_ASSERT(channels > 0 && channels <= 4, "channels must be within 0 to 4");
    srand(seed);

    uint32_t imgSize = width * height * channels;
    uint8_t* imgBuff = (uint8_t*)lvn::memAlloc(imgSize);

    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
            int rn = rand() % 256;
            for (uint32_t c = 0; c < channels; c++)
                imgBuff[y * width * channels + x * channels + c] = (c == 3 ? 255 : rn);
        }
    }

    LvnImage imageData{};
    imageData.width = width;
    imageData.height = height;
    imageData.channels = channels;
    uint64_t imgsize = imageData.width * imageData.height * imageData.channels;
    imageData.pixels = LvnVector<uint8_t>(imgBuff, imgSize);

    lvn::memFree(imgBuff);
    return imageData;
}

LvnModel loadModel(const char* filepath)
{
    /*
    LvnString filepathstr(filepath);
    LvnString extensionType = filepathstr.substr(filepathstr.find_last_of(".") + 1);

    if (extensionType == "gltf")
    {
        return lvn::loadGltfModel(filepath);
    }
    else if (extensionType == "glb")
    {
        return lvn::loadGlbModel(filepath);
    }
    else if (extensionType == "obj")
    {
        return lvn::loadObjModel(filepath);
    }

    LVN_CORE_WARN("loadModel(const char*) | could not load model, file extension type not recognized (%s), Filepath: %s", extensionType.c_str(), filepath);
    */
    return {};
}

void unloadModel(LvnModel* model)
{
    for (uint32_t i = 0; i < model->samplers.size(); i++)
    {
        lvn::destroySampler(model->samplers[i]);
    }
    for (uint32_t i = 0; i < model->textures.size(); i++)
    {
        lvn::destroyTexture(model->textures[i]);
    }
    for (uint32_t i = 0; i < model->buffers.size(); i++)
    {
        lvn::destroyBuffer(model->buffers[i]);
    }
    for (uint32_t i = 0; i < model->skins.size(); i++)
    {
        lvn::destroyBuffer(model->skins[i].ssbo);
    }
}

} /* namespace lvn */
