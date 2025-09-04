#ifndef HG_LVN_GRAPHICS_H
#define HG_LVN_GRAPHICS_H

// [LAYOUT]:
// ------------------------------------------------------------
//
// [SECTION]: Enums
// [SECTION]: Struct Declaration
// [SECTION]: Functions
// [SECTION]: Struct Implementation


#include "lvn_config.h"
#include "levikno.h"
#include "lvn_lmath.h"


// [SECTION]: Enums
// ------------------------------------------------------------

enum LvnPhysicalDeviceType
{
    Lvn_PhysicalDeviceType_Other           = 0,
    Lvn_PhysicalDeviceType_Integrated_GPU  = 1,
    Lvn_PhysicalDeviceType_Discrete_GPU    = 2,
    Lvn_PhysicalDeviceType_Virtual_GPU     = 3,
    Lvn_PhysicalDeviceType_CPU             = 4,

    Lvn_PhysicalDeviceType_Unknown = Lvn_PhysicalDeviceType_Other,
};

enum LvnBufferType
{
    Lvn_BufferType_Unknown  = 0,
    Lvn_BufferType_Vertex   = (1U << 0),
    Lvn_BufferType_Index    = (1U << 1),
    Lvn_BufferType_Uniform  = (1U << 2),
    Lvn_BufferType_Storage  = (1U << 3),
};
typedef uint32_t LvnBufferTypeFlagBits;

enum LvnBufferUsage
{
    Lvn_BufferUsage_Static,
    Lvn_BufferUsage_Dynamic,
    Lvn_BufferUsage_Resize,
};

enum LvnCullFaceMode
{
    Lvn_CullFaceMode_Front,
    Lvn_CullFaceMode_Back,
    Lvn_CullFaceMode_Both,
    Lvn_CullFaceMode_Disable,
};

enum LvnCullFrontFace
{
    Lvn_CullFrontFace_Clockwise,
    Lvn_CullFrontFace_CounterClockwise,

    Lvn_CullFrontFace_CW = Lvn_CullFrontFace_Clockwise,
    Lvn_CullFrontFace_CCW = Lvn_CullFrontFace_CounterClockwise,
};

enum LvnColorBlendFactor
{
    Lvn_ColorBlendFactor_Zero                   = 0,
    Lvn_ColorBlendFactor_One                    = 1,
    Lvn_ColorBlendFactor_SrcColor               = 2,
    Lvn_ColorBlendFactor_OneMinusSrcColor       = 3,
    Lvn_ColorBlendFactor_DstColor               = 4,
    Lvn_ColorBlendFactor_OneMinusDstColor       = 5,
    Lvn_ColorBlendFactor_SrcAlpha               = 6,
    Lvn_ColorBlendFactor_OneMinusSrcAlpha       = 7,
    Lvn_ColorBlendFactor_DstAlpha               = 8,
    Lvn_ColorBlendFactor_OneMinusDstAlpha       = 9,
    Lvn_ColorBlendFactor_ConstantColor          = 10,
    Lvn_ColorBlendFactor_OneMinusConstantColor  = 11,
    Lvn_ColorBlendFactor_ConstantAlpha          = 12,
    Lvn_ColorBlendFactor_OneMinusConstantAlpha  = 13,
    Lvn_ColorBlendFactor_SrcAlphaSaturate       = 14,
    Lvn_ColorBlendFactor_Src1Color              = 15,
    Lvn_ColorBlendFactor_OneMinusSrc1Color      = 16,
    Lvn_ColorBlendFactor_Src1_Alpha             = 17,
    Lvn_ColorBlendFactor_OneMinusSrc1Alpha      = 18,
};

enum LvnColorBlendOperation
{
    Lvn_ColorBlendOp_Add                 = 0,
    Lvn_ColorBlendOp_Subtract            = 1,
    Lvn_ColorBlendOp_ReverseSubtract     = 2,
    Lvn_ColorBlendOp_Min                 = 3,
    Lvn_ColorBlendOp_Max                 = 4,
};

enum LvnColorImageFormat
{
    Lvn_ColorImageFormat_None = 0,
    Lvn_ColorImageFormat_RGB,
    Lvn_ColorImageFormat_RGBA,
    Lvn_ColorImageFormat_RGBA8,
    Lvn_ColorImageFormat_RGBA16F,
    Lvn_ColorImageFormat_RGBA32F,
    Lvn_ColorImageFormat_SRGB,
    Lvn_ColorImageFormat_SRGBA,
    Lvn_ColorImageFormat_SRGBA8,
    Lvn_ColorImageFormat_SRGBA16F,
    Lvn_ColorImageFormat_SRGBA32F,
    Lvn_ColorImageFormat_RedInt,
};

enum LvnCompareOperation
{
    Lvn_CompareOp_Never          = 0,
    Lvn_CompareOp_Less           = 1,
    Lvn_CompareOp_Equal          = 2,
    Lvn_CompareOp_LessOrEqual    = 3,
    Lvn_CompareOp_Greater        = 4,
    Lvn_CompareOp_NotEqual       = 5,
    Lvn_CompareOp_GreaterOrEqual = 6,
    Lvn_CompareOp_Always         = 7,
};

enum LvnDepthImageFormat
{
    Lvn_DepthImageFormat_Depth16,
    Lvn_DepthImageFormat_Depth32,
    Lvn_DepthImageFormat_Depth24Stencil8,
    Lvn_DepthImageFormat_Depth32Stencil8,
};

enum LvnDescriptorType
{
    Lvn_DescriptorType_None = 0,
    Lvn_DescriptorType_ImageSampler,
    Lvn_DescriptorType_ImageSamplerBindless,
    Lvn_DescriptorType_UniformBuffer,
    Lvn_DescriptorType_StorageBuffer,
};

enum LvnSampleCount
{
    Lvn_SampleCount_1_Bit  = (1U << 0),
    Lvn_SampleCount_2_Bit  = (1U << 1),
    Lvn_SampleCount_4_Bit  = (1U << 2),
    Lvn_SampleCount_8_Bit  = (1U << 3),
    Lvn_SampleCount_16_Bit = (1U << 4),
    Lvn_SampleCount_32_Bit = (1U << 5),
    Lvn_SampleCount_64_Bit = (1U << 6),
};

enum LvnShaderStage
{
    Lvn_ShaderStage_All,
    Lvn_ShaderStage_Vertex,
    Lvn_ShaderStage_Fragment,
};

enum LvnStencilOperation
{
    Lvn_StencilOp_Keep              = 0,
    Lvn_StencilOp_Zero              = 1,
    Lvn_StencilOp_Replace           = 2,
    Lvn_StencilOp_IncrementAndClamp = 3,
    Lvn_StencilOp_DecrementAndClamp = 4,
    Lvn_StencilOp_Invert            = 5,
    Lvn_StencilOp_IncrementAndWrap  = 6,
    Lvn_StencilOp_DecrementAndWrap  = 7,
};

enum LvnTextureFilter
{
    Lvn_TextureFilter_Nearest,
    Lvn_TextureFilter_Linear,
};

enum LvnTextureFormat
{
    Lvn_TextureFormat_Unorm = 0,
    Lvn_TextureFormat_Srgb  = 1,
};

enum LvnTextureMode
{
    Lvn_TextureMode_Repeat,
    Lvn_TextureMode_MirrorRepeat,
    Lvn_TextureMode_ClampToEdge,
    Lvn_TextureMode_ClampToBorder,
};

enum LvnTopologyType
{
    Lvn_TopologyType_None = 0,
    Lvn_TopologyType_Point,
    Lvn_TopologyType_Line,
    Lvn_TopologyType_LineStrip,
    Lvn_TopologyType_Triangle,
    Lvn_TopologyType_TriangleStrip,
};

enum LvnAttributeFormat
{
    Lvn_AttributeFormat_Undefined = 0,
    Lvn_AttributeFormat_Scalar_f32,
    Lvn_AttributeFormat_Scalar_f64,
    Lvn_AttributeFormat_Scalar_i32,
    Lvn_AttributeFormat_Scalar_ui32,
    Lvn_AttributeFormat_Scalar_i8,
    Lvn_AttributeFormat_Scalar_ui8,
    Lvn_AttributeFormat_Vec2_f32,
    Lvn_AttributeFormat_Vec3_f32,
    Lvn_AttributeFormat_Vec4_f32,
    Lvn_AttributeFormat_Vec2_f64,
    Lvn_AttributeFormat_Vec3_f64,
    Lvn_AttributeFormat_Vec4_f64,
    Lvn_AttributeFormat_Vec2_i32,
    Lvn_AttributeFormat_Vec3_i32,
    Lvn_AttributeFormat_Vec4_i32,
    Lvn_AttributeFormat_Vec2_ui32,
    Lvn_AttributeFormat_Vec3_ui32,
    Lvn_AttributeFormat_Vec4_ui32,
    Lvn_AttributeFormat_Vec2_i8,
    Lvn_AttributeFormat_Vec3_i8,
    Lvn_AttributeFormat_Vec4_i8,
    Lvn_AttributeFormat_Vec2_ui8,
    Lvn_AttributeFormat_Vec3_ui8,
    Lvn_AttributeFormat_Vec4_ui8,
    Lvn_AttributeFormat_Vec2_n8,
    Lvn_AttributeFormat_Vec3_n8,
    Lvn_AttributeFormat_Vec4_n8,
    Lvn_AttributeFormat_Vec2_un8,
    Lvn_AttributeFormat_Vec3_un8,
    Lvn_AttributeFormat_Vec4_un8,
    Lvn_AttributeFormat_2_10_10_10_ile,
    Lvn_AttributeFormat_2_10_10_10_uile,
    Lvn_AttributeFormat_2_10_10_10_nle,
    Lvn_AttributeFormat_2_10_10_10_unle,
};

enum LvnInterpolationMode
{
    Lvn_InterpolationMode_Step,
    Lvn_InterpolationMode_Linear,
};

enum LvnAnimationPath
{
    Lvn_AnimationPath_Translation,
    Lvn_AnimationPath_Rotation,
    Lvn_AnimationPath_Scale,
};


// [SECTION]: Struct Declaration
// ------------------------------------------------------------

struct LvnAnimation;
struct LvnAnimationChannel;
struct LvnBuffer;
struct LvnBufferCreateInfo;
struct LvnCamera;
struct LvnCommandBuffer;
struct LvnCubemap;
struct LvnCubemapCreateInfo;
struct LvnDescriptorBinding;
struct LvnDescriptorLayout;
struct LvnDescriptorLayoutCreateInfo;
struct LvnDescriptorSet;
struct LvnDescriptorUpdateInfo;
struct LvnFont;
struct LvnFontGlyph;
struct LvnFrameBuffer;
struct LvnFrameBufferCreateInfo;
struct LvnFrameBufferColorAttachment;
struct LvnFrameBufferDepthAttachment;
struct LvnGraphicsContext;
struct LvnGraphicsContextCreateInfo;
struct LvnImageData;
struct LvnImageHdrData;
struct LvnMaterial;
struct LvnMesh;
struct LvnModel;
struct LvnNode;
struct LvnOrthoCamera;
struct LvnPhysicalDevice;
struct LvnPhysicalDeviceFeatures;
struct LvnPhysicalDeviceProperties;
struct LvnPipeline;
struct LvnPipelineColorBlend;
struct LvnPipelineColorBlendAttachment;
struct LvnPipelineColorWriteMask;
struct LvnPipelineCreateInfo;
struct LvnPipelineDepthStencil;
struct LvnPipelineFixedFunctions;
struct LvnPipelineInputAssembly;
struct LvnPipelineMultiSampling;
struct LvnPipelineRasterizer;
struct LvnPipelineScissor;
struct LvnPipelineStencilAttachment;
struct LvnPipelineViewport;
struct LvnPrimitive;
struct LvnRenderPass;
struct LvnSampler;
struct LvnSamplerCreateInfo;
struct LvnShader;
struct LvnSkin;
struct LvnShaderCreateInfo;
struct LvnTexture;
struct LvnTextureCreateInfo;
struct LvnTextureSamplerCreateInfo;
struct LvnUniformBufferInfo;
struct LvnVertexAttribute;
struct LvnVertexBindingDescription;

typedef LvnVec<4, uint8_t>             LvnColorImageData;

// [SECTION]: Functions
// ------------------------------------------------------------
// - Render functions with the prefix CmdDraw can only
//     be used during command recording
// - New graphics objects cannot be created or destroyed
//     during command recording

namespace lvn
{
    LVN_API LvnGraphicsApi              getGraphicsApi();
    LVN_API LvnResult                   initGraphicsContext(LvnGraphicsContextCreateInfo* createInfo);
    LVN_API void                        terminateGraphicsContext();
    LVN_API LvnGraphicsContext*         getGraphicsContext();


    LVN_API void                        getPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* deviceCount);
    LVN_API LvnPhysicalDeviceProperties getPhysicalDeviceProperties(LvnPhysicalDevice* physicalDevice);
    LVN_API LvnPhysicalDeviceFeatures   getPhysicalDeviceFeatures(LvnPhysicalDevice* physicalDevice);
    LVN_API LvnResult                   checkPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice);
    LVN_API LvnResult                   setPhysicalDevice(LvnPhysicalDevice* physicalDevice);

    LVN_API void                        renderBeginNextFrame(LvnCommandBuffer* cmdBuffer);                                                                          // begins the next frame of the window
    LVN_API void                        renderDrawSubmit(LvnCommandBuffer* cmdBuffer);                                                                              // submits all draw commands recorded and presents to window
    LVN_API void                        renderBeginCommandRecording(LvnCommandBuffer* cmdBuffer);                                                                   // begins command buffer when recording draw commands start
    LVN_API void                        renderEndCommandRecording(LvnCommandBuffer* cmdBuffer);                                                                     // ends command buffer when finished recording draw commands
    LVN_API void                        renderCmdDraw(LvnCommandBuffer* cmdBuffer, uint32_t vertexCount);
    LVN_API void                        renderCmdDrawIndexed(LvnCommandBuffer* cmdBuffer, uint32_t indexCount);
    LVN_API void                        renderCmdDrawInstanced(LvnCommandBuffer* cmdBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
    LVN_API void                        renderCmdDrawIndexedInstanced(LvnCommandBuffer* cmdBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
    LVN_API void                        renderCmdSetStencilReference(uint32_t reference);
    LVN_API void                        renderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask);
    LVN_API void                        renderCmdBeginRenderPass(LvnCommandBuffer* cmdBuffer, float r, float g, float b, float a);                                  // begins renderpass when rendering starts
    LVN_API void                        renderCmdEndRenderPass(LvnCommandBuffer* cmdBuffer);                                                                        // ends renderpass when rendering has finished
    LVN_API void                        renderCmdBindPipeline(LvnCommandBuffer* cmdBuffer, LvnPipeline* pipeline);                                                  // bind a pipeline to begin shading during rendering
    LVN_API void                        renderCmdBindVertexBuffer(LvnCommandBuffer* cmdBuffer, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets); // binds the vertex buffer within an LvnBuffer object
    LVN_API void                        renderCmdBindIndexBuffer(LvnCommandBuffer* cmdBuffer, LvnBuffer* buffer, uint64_t offset);                                  // binds the index buffer within an LvnBuffer object
    LVN_API void                        renderCmdBindDescriptorSets(LvnCommandBuffer* cmdBuffer, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets); // bind multiple descriptor sets to the shader (if multiple sets are used), Note that descriptor sets must be in order to how the sets are ordered in the pipeline
    LVN_API void                        renderCmdBeginFrameBuffer(LvnCommandBuffer* cmdBuffer, LvnFrameBuffer* frameBuffer);                                        // begins the framebuffer for recording offscreen render calls, similar to beginning the render pass
    LVN_API void                        renderCmdEndFrameBuffer(LvnCommandBuffer* cmdBuffer, LvnFrameBuffer* frameBuffer);                                          // ends recording to the framebuffer

    LVN_API LvnResult                   createShaderFromSrc(LvnShader** shader, const LvnShaderCreateInfo* createInfo);                                   // create shader with the source code as input
    LVN_API LvnResult                   createShaderFromFileBin(LvnShader** shader, const LvnShaderCreateInfo* createInfo);                               // create shader with the file paths to the binary files (.spv) as input
    LVN_API LvnResult                   createShaderFromFileSrc(LvnShader** shader, const LvnShaderCreateInfo* createInfo);                               // create shader with the file paths to the source files as input
    LVN_API LvnResult                   createDescriptorLayout(LvnDescriptorLayout** descriptorLayout, const LvnDescriptorLayoutCreateInfo* createInfo);  // create descriptor layout for the pipeline
    LVN_API LvnResult                   createPipeline(LvnPipeline** pipeline, const LvnPipelineCreateInfo* createInfo);                                  // create pipeline to describe rendering function
    LVN_API LvnResult                   createFrameBuffer(LvnFrameBuffer** frameBuffer, const LvnFrameBufferCreateInfo* createInfo);                      // create framebuffer to render images to
    LVN_API LvnResult                   createBuffer(LvnBuffer** buffer, const LvnBufferCreateInfo* createInfo);                                          // create a single buffer object that can hold both the vertex and index buffers
    LVN_API LvnResult                   createSampler(LvnSampler** sampler, const LvnSamplerCreateInfo* createInfo);                                      // create a sampler object to store texture sampler data
    LVN_API LvnResult                   createTexture(LvnTexture** texture, const LvnTextureCreateInfo* createInfo);                                      // create a texture object to store image data
    LVN_API LvnResult                   createTexture(LvnTexture** texture, const LvnTextureSamplerCreateInfo* createInfo);                               // create a texture object to store image data given a sampler object
    LVN_API LvnResult                   createCubemap(LvnCubemap** cubemap, const LvnCubemapCreateInfo* createInfo);                                      // create a cubemap texture object that holds the textures of the cubemap

    LVN_API void                        destroyShader(LvnShader* shader);                                                                                 // destroy shader module object
    LVN_API void                        destroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout);                                                   // destroy descriptor layout
    LVN_API void                        destroyPipeline(LvnPipeline* pipeline);                                                                           // destroy pipeline object
    LVN_API void                        destroyFrameBuffer(LvnFrameBuffer* frameBuffer);                                                                  // destroy framebuffer object
    LVN_API void                        destroyBuffer(LvnBuffer* buffer);                                                                                 // destory buffers object
    LVN_API void                        destroySampler(LvnSampler* sampler);                                                                              // destroy sampler object
    LVN_API void                        destroyTexture(LvnTexture* texture);                                                                              // destroy texture object
    LVN_API void                        destroyCubemap(LvnCubemap* cubemap);                                                                              // destroy cubemap object

    LVN_API uint32_t                    getAttributeFormatSize(LvnAttributeFormat format);
    LVN_API uint32_t                    getAttributeFormatComponentSize(LvnAttributeFormat format);
    LVN_API bool                        isAttributeFormatNormalizedType(LvnAttributeFormat format);
    LVN_API void                        pipelineFixedFuncSetConfig(LvnPipelineFixedFunctions* pipelineFixedFuncs);
    LVN_API LvnPipelineFixedFunctions   configPipelineFixedFuncInit();
    LVN_API LvnResult                   allocateDescriptorSet(LvnDescriptorSet** descriptorSet, LvnDescriptorLayout* descriptorLayout);                   // create descriptor set to uplaod uniform data to pipeline

    LVN_API void                        bufferUpdateData(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset);
    LVN_API void                        bufferResize(LvnBuffer* buffer, uint64_t size);

    LVN_API LvnTexture*                 cubemapGetTextureData(LvnCubemap* cubemap);                                                                               // get the cubemap texture from the cubemap

    LVN_API void                        updateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count);           // update the descriptor content within a descroptor set

    LVN_API LvnTexture*                 frameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex);                                               // get the texture image data (render pass attachment) from the framebuffer via the attachment index
    LVN_API LvnRenderPass*              frameBufferGetRenderPass(LvnFrameBuffer* frameBuffer);                                                                    // get the render pass from the framebuffer
    LVN_API void                        frameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height);                                          // update the width and height of the new framebuffer (updates the image data dimensions), Note: call only when the image dimensions need to be changed
    LVN_API void                        frameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a);      // set the background color for the framebuffer for offscreen rendering
    LVN_API LvnDepthImageFormat         findSupportedDepthImageFormat(LvnDepthImageFormat* pDepthImageFormats, uint32_t count);

    LVN_API LvnImageData                loadImageData(const char* filepath, int forceChannels = 0, bool flipVertically = false);
    LVN_API LvnImageData                loadImageDataMemory(const uint8_t* data, int length, int forceChannels = 0, bool flipVertically = false);
    LVN_API LvnImageData                loadImageDataThread(const LvnString filepath, int forceChannels = 0, bool flipVertically = false);
    LVN_API LvnImageData                loadImageDataMemoryThread(const uint8_t* data, int length, int forceChannels = 0, bool flipVertically = false);
    LVN_API LvnImageHdrData             loadHdrImageData(const char* filepath, int forceChannels = 0, bool flipVertically = false);

    LVN_API LvnResult                   writeImagePng(const LvnImageData& imageData, const char* filename);               // writes the image data into a png file with the filename/filepath
    LVN_API LvnResult                   writeImageJpg(const LvnImageData& imageData, const char* filename, int quality);  // writes the image data into a jpg file with the filename/filepath and the jpg quality (from 0...100)
    LVN_API LvnResult                   writeImageBmp(const LvnImageData& imageData, const char* filename);               // writes the image data into a bmp file with the filename/filepath

    LVN_API void                        imageFlipVertically(LvnImageData& imageData);                                     // flips the image vertically
    LVN_API void                        imageFlipHorizontally(LvnImageData& imageData);                                   // flips the image horizontally
    LVN_API void                        imageRotateCW(LvnImageData& imageData);                                           // rotates the image clockwise (right)
    LVN_API void                        imageRotateCCW(LvnImageData& imageData);                                          // rotates the image counter clockwise (left)

    LVN_API LvnImageData                imageGenColor(uint32_t width, uint32_t height, uint32_t channels, const LvnColorImageData& color);
    LVN_API LvnImageData                imageGenWhiteNoise(uint32_t width, uint32_t height, uint32_t channels);
    LVN_API LvnImageData                imageGenWhiteNoise(uint32_t width, uint32_t height, uint32_t channels, uint32_t seed);
    LVN_API LvnImageData                imageGenGrayScaleNoise(uint32_t width, uint32_t height, uint32_t channels);
    LVN_API LvnImageData                imageGenGrayScaleNoise(uint32_t width, uint32_t height, uint32_t channels, uint32_t seed);

    LVN_API LvnModel                    loadModel(const char* filepath);
    LVN_API void                        unloadModel(LvnModel* model);
}


// [SECTION]: Struct Implementation
// ------------------------------------------------------------
struct LvnPhysicalDeviceProperties
{
    LvnString name;
    LvnPhysicalDeviceType type;
    uint32_t apiVersion;
    uint32_t driverVersion;
    uint32_t vendorID;
};

struct LvnPhysicalDeviceFeatures
{
    bool robustBufferAccess;
    bool fullDrawIndexUint32;
    bool imageCubeArray;
    bool independentBlend;
    bool geometryShader;
    bool tessellationShader;
    bool sampleRateShading;
    bool dualSrcBlend;
    bool logicOp;
    bool multiDrawIndirect;
    bool drawIndirectFirstInstance;
    bool depthClamp;
    bool depthBiasClamp;
    bool fillModeNonSolid;
    bool depthBounds;
    bool wideLines;
    bool largePoints;
    bool alphaToOne;
    bool multiViewport;
    bool samplerAnisotropy;
    bool textureCompressionETC2;
    bool textureCompressionASTC_LDR;
    bool textureCompressionBC;
    bool occlusionQueryPrecise;
    bool pipelineStatisticsQuery;
    bool vertexPipelineStoresAndAtomics;
    bool fragmentStoresAndAtomics;
    bool shaderTessellationAndGeometryPointSize;
    bool shaderImageGatherExtended;
    bool shaderStorageImageExtendedFormats;
    bool shaderStorageImageMultisample;
    bool shaderStorageImageReadWithoutFormat;
    bool shaderStorageImageWriteWithoutFormat;
    bool shaderUniformBufferArrayDynamicIndexing;
    bool shaderSampledImageArrayDynamicIndexing;
    bool shaderStorageBufferArrayDynamicIndexing;
    bool shaderStorageImageArrayDynamicIndexing;
    bool shaderClipDistance;
    bool shaderCullDistance;
    bool shaderFloat64;
    bool shaderInt64;
    bool shaderInt16;
    bool shaderResourceResidency;
    bool shaderResourceMinLod;
    bool sparseBinding;
    bool sparseResidencyBuffer;
    bool sparseResidencyImage2D;
    bool sparseResidencyImage3D;
    bool sparseResidency2Samples;
    bool sparseResidency4Samples;
    bool sparseResidency8Samples;
    bool sparseResidency16Samples;
    bool sparseResidencyAliased;
    bool variableMultisampleRate;
    bool inheritedQueries;
};

struct LvnPipelineInputAssembly
{
    LvnTopologyType topology;
    bool primitiveRestartEnable;
};

// width and height are based on GLFW window framebuffer size
// Note: GLFW framebuffer size and window pixel coordinates may not be the same on different systems
// Set width and height to -1 if it does not need to be specified, width and height will then be automatically set to the framebuffer size
struct LvnPipelineViewport
{
    float x, y;
    float width, height;
    float minDepth, maxDepth;
};

struct LvnPipelineScissor
{
    struct { uint32_t x, y; } offset;
    struct { uint32_t width, height; } extent;
};

struct LvnPipelineRasterizer
{
    LvnCullFaceMode cullMode;
    LvnCullFrontFace frontFace;

    float lineWidth;
    float depthBiasConstantFactor;
    float depthBiasClamp;
    float depthBiasSlopeFactor;

    bool depthClampEnable;
    bool rasterizerDiscardEnable;
    bool depthBiasEnable;
};

struct LvnPipelineColorWriteMask
{
    bool colorComponentR;
    bool colorComponentG;
    bool colorComponentB;
    bool colorComponentA;
};

struct LvnPipelineMultiSampling
{
    LvnSampleCount rasterizationSamples;
    float minSampleShading;
    uint32_t* sampleMask;
    bool sampleShadingEnable;
    bool alphaToCoverageEnable;
    bool alphaToOneEnable;
};

struct LvnPipelineColorBlendAttachment
{
    LvnPipelineColorWriteMask colorWriteMask;
    LvnColorBlendFactor srcColorBlendFactor;
    LvnColorBlendFactor dstColorBlendFactor;
    LvnColorBlendOperation colorBlendOp;
    LvnColorBlendFactor srcAlphaBlendFactor;
    LvnColorBlendFactor dstAlphaBlendFactor;
    LvnColorBlendOperation alphaBlendOp;
    bool blendEnable;
};

struct LvnPipelineColorBlend
{
    LvnPipelineColorBlendAttachment* pColorBlendAttachments;
    uint32_t colorBlendAttachmentCount;
    float blendConstants[4];
    bool logicOpEnable;
};

struct LvnPipelineStencilAttachment
{
    LvnStencilOperation failOp;
    LvnStencilOperation passOp;
    LvnStencilOperation depthFailOp;
    LvnCompareOperation compareOp;
    uint32_t compareMask;
    uint32_t writeMask;
    uint32_t reference;
};

struct LvnPipelineDepthStencil
{
    LvnCompareOperation depthOpCompare;
    LvnPipelineStencilAttachment stencil;
    bool enableDepth, enableStencil;
};

struct LvnPipelineFixedFunctions
{
    LvnPipelineInputAssembly inputAssembly;
    LvnPipelineViewport viewport;
    LvnPipelineScissor scissor;
    LvnPipelineRasterizer rasterizer;
    LvnPipelineMultiSampling multisampling;
    LvnPipelineColorBlend colorBlend;
    LvnPipelineDepthStencil depthstencil;
};

struct LvnVertexBindingDescription
{
    uint32_t binding, stride;
};

struct LvnVertexAttribute
{
    uint32_t binding;
    uint32_t layout;
    LvnAttributeFormat format;
    uint64_t offset;
};

struct LvnDescriptorBinding
{
    uint32_t binding;
    LvnDescriptorType descriptorType;
    uint32_t descriptorCount;
    uint32_t maxAllocations;
    LvnShaderStage shaderStage;
};

struct LvnDescriptorLayoutCreateInfo
{
    LvnDescriptorBinding* pDescriptorBindings;
    uint32_t descriptorBindingCount;
    uint32_t maxSets;
};

struct LvnDescriptorUpdateInfo
{
    uint32_t binding;
    LvnDescriptorType descriptorType;
    uint32_t firstIndex;
    uint32_t descriptorCount;
    const LvnUniformBufferInfo* bufferInfo;
    const LvnTexture* const* pTextureInfos;
};

struct LvnPipelineCreateInfo
{
    LvnPipelineFixedFunctions* pipelineFixedFuncs;
    LvnVertexBindingDescription* pVertexBindingDescriptions;
    uint32_t vertexBindingDescriptionCount;
    LvnVertexAttribute* pVertexAttributes;
    uint32_t vertexAttributeCount;
    const LvnDescriptorLayout* const* pDescriptorLayouts;
    uint32_t descriptorLayoutCount;
    const LvnShader* shader;
    const LvnRenderPass* renderPass;
};

struct LvnShaderCreateInfo
{
    LvnString vertexSrc;
    LvnString fragmentSrc;
};

struct LvnFrameBufferColorAttachment
{
    uint32_t index;
    LvnColorImageFormat format;
};

struct LvnFrameBufferDepthAttachment
{
    uint32_t index;
    LvnDepthImageFormat format;
};

struct LvnFrameBufferCreateInfo
{
    uint32_t width, height;
    LvnSampleCount sampleCount;
    LvnFrameBufferColorAttachment* pColorAttachments;
    uint32_t colorAttachmentCount;
    LvnFrameBufferDepthAttachment* depthAttachment;
    LvnTextureFilter textureFilter;
    LvnTextureMode textureMode;
};

struct LvnBufferCreateInfo
{
    LvnBufferTypeFlagBits type;
    LvnBufferUsage usage;
    uint64_t size;
    const void* data;
};

struct LvnUniformBufferInfo
{
    LvnBuffer* buffer;
    uint64_t range;
    uint64_t offset;
};

struct LvnImageData
{
    LvnVector<uint8_t> pixels;
    uint32_t width, height, channels;
    uint64_t size;
};

struct LvnImageHdrData
{
    LvnVector<float> pixels;
    uint32_t width, height, channels;
    uint64_t size;
};

struct LvnSamplerCreateInfo
{
    LvnTextureFilter minFilter, magFilter;
    LvnTextureMode wrapS, wrapT;
};

struct LvnTextureCreateInfo
{
    LvnImageData imageData;
    LvnTextureFormat format;
    LvnTextureFilter minFilter, magFilter;
    LvnTextureMode wrapS, wrapT;
};

struct LvnTextureSamplerCreateInfo
{
    LvnImageData imageData;
    LvnTextureFormat format;
    LvnSampler* sampler;
};

struct LvnTransform
{
    LvnVec3 translation;
    LvnQuat rotation;
    LvnVec3 scale;
};

struct LvnMaterial
{
    LvnVec3 baseColorFactor;
    LvnVec3 emissiveFactor;
    float metallicFactor;
    float roughnessFactor;

    LvnTexture* albedo;
    LvnTexture* metallicRoughnessOcclusion;
    LvnTexture* normal;
    LvnTexture* emissive;

    bool doubleSided;
};

struct LvnPrimitive
{
    LvnTopologyType topology;
    LvnMaterial material;
    uint32_t vertexCount;
    uint32_t indexCount;
    uint64_t indexOffset;

    LvnBuffer* buffer;
    LvnDescriptorSet* descriptorSet;
};

struct LvnMesh
{
    LvnVector<LvnPrimitive> primitives;
};

struct LvnNode
{
    int32_t parent;
    LvnVector<int32_t> children;

    int32_t mesh;
    int32_t skin;
    LvnTransform transform;
    LvnMat4 matrix;
};

struct LvnSkin
{
    LvnString name;
    LvnVector<LvnMat4> inverseBindMatrices;
    LvnVector<int32_t> joints;
    LvnBuffer* ssbo;
};

struct LvnAnimationChannel
{
    LvnAnimationPath path;
    LvnInterpolationMode interpolation;
    LvnVector<float> keyFrames;
    LvnVector<LvnVec4> outputs;
    int32_t node;
};

struct LvnAnimation
{
    LvnVector<LvnAnimationChannel> channels;
    float start;
    float end;
    float currentTime;
};

struct LvnModel
{
    LvnVector<int32_t> rootNodes;
    LvnVector<LvnNode> nodes;
    LvnVector<LvnMesh> meshes;
    LvnVector<LvnAnimation> animations;
    LvnVector<LvnSkin> skins;
    LvnVector<LvnBuffer*> buffers;
    LvnVector<LvnSampler*> samplers;
    LvnVector<LvnTexture*> textures;
    LvnMat4 matrix;
};

struct LvnCamera
{
    float aspectRatio;           // aspect ratio (width / height)
    float fov;                   // field of view
    float zNear;                 // near plane
    float zFar;                  // far plane
};

struct LvnOrthoCamera
{
    float right;                 // posx bound
    float left;                  // negx bound
    float top;                   // posy bound
    float bottom;                // negy bound
    float zNear;                 // far plane
    float zFar;                  // near plane
};

struct LvnCubemapCreateInfo
{
    LvnImageData posx, negx, posy, negy, posz, negz;
};

struct LvnFontGlyph
{
    struct
    {
        float x0, y0, x1, y1;
    } uv;

    struct
    {
        float x, y;
    } size, bearing;

    uint32_t unicode;
    int advance;
};

struct LvnFont
{
    LvnImageData atlas;
    float fontSize;

    LvnVector<uint32_t> codepoints;
    LvnVector<LvnFontGlyph> glyphs;
};

struct LvnGraphicsContextCreateInfo
{
    LvnResult (*graphicsContextInitFunc)(LvnGraphicsContext*);
    void (*graphicsContextTerminateFunc)();

    uint32_t maxFramesInFlight;
    LvnTextureFormat fbFormat;
};

#endif /* !HG_LVN_GRAPHICS_H */
