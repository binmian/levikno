#ifndef HG_LVN_GRAPHICS_INTERNAL_H
#define HG_LVN_GRAPHICS_INTERNAL_H

#include "lvn_graphics.h"
#include "levikno_internal.h"


struct LvnEvent
{
    LvnEventType type;
    int category;
    const char* name;
    bool handled;
    void* userData;

    struct Data
    {
        union
        {
            int x;
            double xd;
        };
        union
        {
            int y;
            double yd;
        };

        int code;
        unsigned int ucode;
        bool repeat;
    } data;
};

struct LvnNativeWindowData
{
    struct
    {
        void* nativeWindow;
    } win32;
    struct
    {
        void* nativeWindow;
    } cocoa;
    struct
    {
        void* surface;
        void* display;
    } wl;
    struct
    {
        unsigned long nativeWindow;
        bool xcbSupport;
        bool xlibSupport;
    } x11;
};

struct LvnWindow
{
    int width, height;                   // width and height of window
    LvnString title;                     // title of window
    int minWidth, minHeight;             // minimum width and height of window
    int maxWidth, maxHeight;             // maximum width and height of window
    bool fullscreen, resizable, vSync;   // sets window to fullscreen; enables window resizing; vSync controls window framerate
    LvnVector<LvnImage> icons;           // icon images used for window
    void (*eventCallBackFn)(LvnEvent*);  // function ptr used as a callback to get events from this window
    void* userData;
    void* nativeWindow;                  // pointer to native window handle depending on window backend (eg. x11, wayland), if using glfw, GLFWwindow handle is used for this
    int nativeId;                        // window id for x11 windows

    LvnNativeWindowData nativeWindowData;
    void* apiData;
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

struct LvnGraphicsContext
{
    LvnWindowApi                windowapi;
    LvnGraphicsApi              graphicsapi;

    bool                        enableGraphicsApiDebugLogs;
    LvnTextureFormat            frameBufferColorFormat;
    uint32_t                    maxFramesInFlight;
    LvnPipelineFixedFunctions   defaultPipelineFixedFuncs;

    LvnResult                   (*createWindow)(LvnWindow*, const LvnWindowCreateInfo*);
    void                        (*destroyWindow)(LvnWindow*);
    void                        (*updateWindow)(LvnWindow*);
    bool                        (*windowOpen)(LvnWindow*);
    void                        (*windowPollEvents)();
    LvnPair<int>                (*getDimensions)(LvnWindow*);
    unsigned int                (*getWindowWidth)(LvnWindow*);
    unsigned int                (*getWindowHeight)(LvnWindow*);
    void                        (*setWindowVSync)(LvnWindow*, bool);
    bool                        (*getWindowVSync)(LvnWindow*);
    void*                       (*getNativeWindow)(LvnWindow*);
    void                        (*setWindowContextCurrent)(LvnWindow*);
    LvnWindowApi                (*getNativeWindowApi)();

    bool                        (*keyPressed)(LvnWindow*, int);
    bool                        (*keyReleased)(LvnWindow*, int);
    bool                        (*mouseButtonPressed)(LvnWindow*, int);
    bool                        (*mouseButtonReleased)(LvnWindow*, int);

    LvnPair<float>              (*getMousePos)(LvnWindow*);
    void                        (*getMousePosPtr)(LvnWindow*, float*, float*);
    float                       (*getMouseX)(LvnWindow*);
    float                       (*getMouseY)(LvnWindow*);
    void                        (*setMouseCursor)(LvnWindow*, LvnMouseCursor);
    void                        (*SetMouseInputMode)(LvnWindow*, LvnMouseInputMode);

    LvnPair<int>                (*getWindowPos)(LvnWindow*);
    void                        (*getWindowPosPtr)(LvnWindow*, int*, int*);
    LvnPair<int>                (*getWindowSize)(LvnWindow*);
    void                        (*getWindowSizePtr)(LvnWindow*, int*, int*);


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
    void                        (*renderCmdBeginRenderPass)(LvnCommandBuffer*, float, float, float, float);
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
    void                        (*internalWindowListenEventFn)(LvnWindow*, LvnEvent*);
};


namespace lvn
{
    const char* getWindowApiNameEnum(LvnWindowApi api);
    const char* getGraphicsApiNameEnum(LvnGraphicsApi api);

    void        internalWindowEventCallbackFn(LvnWindow* window, LvnEvent* event);
}

#endif /* !HG_LVN_GRAPHICS_INTERNAL_H */
