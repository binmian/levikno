#ifndef HG_LVN_GRAPHICS_INTERNAL_H
#define HG_LVN_GRAPHICS_INTERNAL_H

#include "lvn_graphics.h"
#include "levikno_internal.h"


template <typename T>
struct LvnLinkedNode
{
    T value;
    LvnLinkedNode<T>* next;
    LvnLinkedNode<T>* prev;

    T* operator->() { return &value; }
};

template <typename T>
using LvnLNode = LvnLinkedNode<T>;

template <typename T>
class LvnList
{
private:
    LvnLNode<T>* m_Head;
    LvnLNode<T>* m_Tail;
    uint32_t m_Size;

public:
    LvnList() : m_Head(nullptr), m_Tail(nullptr), m_Size(0) {}
    ~LvnList() { clear_free(); }

    LvnList(const LvnList<T>& other)
    {
        m_Size = other.m_Size;
        LvnLNode<T>* node = other.m_Head;
        if (node)
        {
            m_Head = lvn::memNew<LvnLNode<T>>();
            m_Head->value = node->value;
            m_Head->next = nullptr;
            m_Head->prev = nullptr;
        }
        m_Tail = m_Head;
        node = node->next;
        while (node != nullptr)
        {
            m_Tail->next = lvn::memNew<LvnLNode<T>>();
            m_Tail->next->value = node->value;
            m_Tail->next->prev = m_Tail;
            m_Tail = m_Tail->next;
            node = node->next;
        }
    }
    LvnList(LvnList<T>&& other)
    {
        m_Size = other.m_Size;
        m_Head = other.m_Head;
        m_Tail = other.m_Tail;
        other.m_Size = 0;
        other.m_Head = nullptr;
        other.m_Tail = nullptr;
    }
    LvnList& operator=(const LvnList<T>& other)
    {
        if (this == &other) return *this;
        clear_free();

        m_Size = other.m_Size;
        LvnLNode<T>* node = other.m_Head;
        if (node)
        {
            m_Head = lvn::memNew<LvnLNode<T>>();
            m_Head->value = node->value;
            m_Head->next = nullptr;
            m_Head->prev = nullptr;
        }
        m_Tail = m_Head;
        node = node->next;
        while (node != nullptr)
        {
            m_Tail->next = lvn::memNew<LvnLNode<T>*>();
            m_Tail->next->value = node->value;
            m_Tail->next->prev = m_Tail;
            m_Tail = m_Tail->next;
            node = node->next;
        }

        return *this;
    }
    LvnList& operator=(LvnList<T>&& other)
    {
        if (this == &other) return *this;
        m_Size = other.m_Size;
        m_Head = other.m_Head;
        m_Tail = other.m_Tail;
        other.m_Size = 0;
        other.m_Head = nullptr;
        other.m_Tail = nullptr;
        return *this;
    }

    T& operator [](uint32_t index)
    {
        LVN_ASSERT(index < m_Size, "list index out of range");
        LvnLNode<T>* node = m_Head;
        for (uint32_t i = 0; i < index; i++)
            node = node->next;
        return node->value;
    }
    const T& operator [](uint32_t index) const
    {
        LVN_ASSERT(index < m_Size, "list index out of range");
        LvnLNode<T>* node = m_Head;
        for (uint32_t i = 0; i < index; i++)
            node = node->next;
        return node->value;
    }

    void erase_index(const uint32_t index)
    {
        LVN_ASSERT(index < m_Size, "list index out of range");

        if (index == 0) { pop_front(); return; }
        else if (index == m_Size - 1) { pop_back(); return; }

        LvnLNode<T>* node = m_Head;
        for (uint32_t i = 0; i < index; i++)
            node = node->next;
        if (node->prev)
            node->prev->next = node->next;
        if (node->next)
            node->next->prev = node->prev;
        lvn::memDelete<LvnLNode<T>>(node);
        m_Size--;
    }
    void insert_index(const uint32_t index, const T& value)
    {
        LVN_ASSERT(index <= m_Size, "list index out of range");

        if (index == 0) { push_front(value); return; }
        if (index == m_Size) { push_back(value); return; }

        LvnLNode<T>* node = m_Head;
        for (uint32_t i = 0; i < index; i++)
            node = node->next;
        node->prev->next = lvn::memNew<LvnLNode<T>>();
        node->prev->next->value = value;
        node->prev->next->prev = node->prev;
        node->prev->next->next = node;
        node->prev = node->prev->next;
        m_Size++;
    }
    void push_back(const T& data)
    {
        if (!m_Size)
        {
            m_Head = lvn::memNew<LvnLNode<T>>();
            m_Head->value = data;
            m_Head->next = nullptr;
            m_Head->prev = nullptr;
            m_Tail = m_Head;
            m_Size++;
            return;
        }

        LvnLNode<T>* node = m_Tail;
        node->next = lvn::memNew<LvnLNode<T>>();
        m_Tail = node->next;
        m_Tail->value = data;
        m_Tail->prev = node;
        m_Size++;
    }
    void push_front(const T& data)
    {
        if (!m_Size)
        {
            m_Head = lvn::memNew<LvnLNode<T>>();
            m_Head->value = data;
            m_Tail = m_Head;
            m_Size++;
            return;
        }

        LvnLNode<T>* node = lvn::memNew<LvnLNode<T>>();
        node->value = data;
        node->next = m_Head;
        m_Head->prev = node;
        m_Head = node;
        m_Size++;
    }
    void pop_back()
    {
        if (!m_Size) { return; }
        if (m_Size == 1) { lvn::memDelete<LvnLNode<T>>(m_Tail); m_Tail = m_Head = nullptr; m_Size--; return; }

        LvnLNode<T>* node = m_Tail->prev;
        node->next = nullptr;
        lvn::memDelete<LvnLNode<T>>(m_Tail);
        m_Tail = node;
        m_Size--;
    }
    void pop_front()
    {
        if (!m_Size) { return; }
        if (m_Size == 1) { lvn::memDelete<LvnLNode<T>>(m_Head); m_Head = m_Tail = nullptr; m_Size--; return; }

        LvnLNode<T>* node = m_Head->next;
        node->prev = nullptr;
        lvn::memDelete<LvnLNode<T>>(m_Head);
        m_Head = node;
        m_Size--;
    }

    uint32_t    size() const { return m_Size; }
    bool        empty() const { return m_Size == 0; }
    void        clear_free() { while (m_Head != nullptr) { LvnLNode<T>* node = m_Head; m_Head = node->next; lvn::memDelete<LvnLNode<T>>(node); } }

    T&          front() { LVN_ASSERT(m_Size, "cannot call front on empty list"); return m_Head->value; }
    const T&    front() const { LVN_ASSERT(m_Size, "cannot call front on empty list"); return m_Head->value; }

    T&          back() { LVN_ASSERT(m_Size, "cannot call back on empty list"); return m_Tail->value; }
    const T&    back() const { LVN_ASSERT(m_Size, "cannot call back on empty list"); return m_Tail->value; }
};

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

struct LvnRenderPass
{
    void* nativeRenderPass;
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
    void* apiData;                       // graphics api related data tied to the window
    LvnRenderPass renderPass;           // vulkan render pass needed to retreive render pass pointer
};

struct LvnPhysicalDevice
{
    LvnPhysicalDeviceProperties properties;
    LvnPhysicalDeviceFeatures features;
    void* physicalDevice;
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

    LvnList<LvnVector<LvnDescriptorSet>> descriptorSets;
    uint32_t maxSets;
    uint64_t descriptorSetIndex;
};

struct LvnDescriptorSet
{
    void* descriptorSet;
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
};

struct LvnFrameBuffer
{
    void* frameBufferData;
};

struct LvnCubemap
{
    LvnTexture textureData;
};

struct LvnCommandPool
{
    void* commandPool;
    LvnList<LvnVector<LvnCommandBuffer>> commandBuffers;
};

struct LvnCommandBuffer
{
    void* commandBuffers;

    // synchronization
    void* imageAvailableSemaphores; // VkSemaphore
    void* inFlightFences;           // VkFence
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
    LvnRenderPass*              (*getWindowRenderPass)(LvnWindow*);
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
    LvnResult                   (*createShaderFromBin)(LvnShader*, const LvnShaderBinCreateInfo*);
    LvnResult                   (*createShaderFromFileSrc)(LvnShader*, const LvnShaderCreateInfo*);
    LvnResult                   (*createShaderFromFileBin)(LvnShader*, const LvnShaderCreateInfo*);
    LvnResult                   (*createDescriptorLayout)(LvnDescriptorLayout*, const LvnDescriptorLayoutCreateInfo*);
    LvnResult                   (*allocateCommandBuffers)(LvnCommandPool*, LvnCommandBuffer**, uint32_t);
    LvnResult                   (*allocateDescriptorSet)(LvnDescriptorLayout*, LvnDescriptorSet**, uint32_t);
    LvnResult                   (*createPipeline)(LvnPipeline*, const LvnPipelineCreateInfo*);
    LvnResult                   (*createCommandPool)(LvnCommandPool*);
    LvnResult                   (*createFrameBuffer)(LvnFrameBuffer*, const LvnFrameBufferCreateInfo*);
    LvnResult                   (*createBuffer)(LvnBuffer*, const LvnBufferCreateInfo*);
    LvnResult                   (*createSampler)(LvnSampler*, const LvnSamplerCreateInfo*);
    LvnResult                   (*createTexture)(LvnTexture*, const LvnTextureCreateInfo*);
    LvnResult                   (*createCubemap)(LvnCubemap*, const LvnCubemapCreateInfo*);

    void                        (*destroyShader)(LvnShader*);
    void                        (*destroyDescriptorLayout)(LvnDescriptorLayout*);
    void                        (*destroyPipeline)(LvnPipeline*);
    void                        (*destroyCommandPool)(LvnCommandPool*);
    void                        (*destroyFrameBuffer)(LvnFrameBuffer*);
    void                        (*destroyBuffer)(LvnBuffer*);
    void                        (*destroySampler)(LvnSampler*);
    void                        (*destroyTexture)(LvnTexture*);
    void                        (*destroyCubemap)(LvnCubemap*);

    void                        (*renderBeginNextFrame)(LvnWindow*, LvnCommandBuffer*);
    void                        (*renderDrawSubmit)(LvnWindow*, LvnCommandBuffer*);
    void                        (*renderBeginCommandRecording)(LvnCommandBuffer*);
    void                        (*renderEndCommandRecording)(LvnCommandBuffer*);
    void                        (*renderCmdDraw)(LvnCommandBuffer*, uint32_t);
    void                        (*renderCmdDrawIndexed)(LvnCommandBuffer*, uint32_t);
    void                        (*renderCmdDrawInstanced)(LvnCommandBuffer*, uint32_t, uint32_t, uint32_t);
    void                        (*renderCmdDrawIndexedInstanced)(LvnCommandBuffer*, uint32_t, uint32_t, uint32_t);
    void                        (*renderCmdSetStencilReference)(uint32_t);
    void                        (*renderCmdSetStencilMask)(uint32_t, uint32_t);
    void                        (*renderCmdBeginRenderPass)(LvnCommandBuffer*, LvnWindow*, float, float, float, float);
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
