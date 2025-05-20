#ifndef HG_LEVIKNO_INTERNAL_H
#define HG_LEVIKNO_INTERNAL_H

#include "levikno.h"

#include <unordered_map>
#include <queue>

// ------------------------------------------------------------
// Layout: levikno_internal.h
// ------------------------------------------------------------
//
// [SECTION]: Core Internal Structs
// -- [SUBSECT]: Internal Data Structures
// -- [SUBSECT]: Logging Data Structures
// [SECTION]: Window Internal Structs
// -- [SUBSECT]: Event Data Structures
// -- [SUBSECT]: Window Data Structures
// -- [SUBSECT]: Window Context Structure
// [SECTION]: Graphics Internal Structs
// -- [SUBSECT]: Graphics Context
// -- [SUBSECT]: Draw Command Buffer Structures
// -- [SUBSECT]: General Graphics Structures
// [SECTION]: Context Internal Structs
// -- [SUBSECT]: Memory Alloc Structures
// -- [SUBSECT]: Renderer Structures
// -- [SUBSECT]: Context Structure
// [SECTION]: Internal Functions



// ------------------------------------------------------------
// [SECTION]: Core Internal Structs
// ------------------------------------------------------------


// -- [SUBSECT]: Internal Data Structures
// ------------------------------------------------------------

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

    ~LvnList()
    {
        while (m_Head != nullptr)
        {
            LvnLNode<T>* node = m_Head;
            m_Head = node->next;
            delete node;
        }
    }

    T& operator [](uint32_t index)
    {
        LVN_CORE_ASSERT(index < m_Size, "list index out of range");

        LvnLNode<T>* node = m_Head;

        for (uint32_t i = 0; i < index; i++)
        {
            node = node->next;
        }

        return node->value;
    }
    const T& operator [](uint32_t index) const
    {
        LVN_CORE_ASSERT(index < m_Size, "list index out of range");

        LvnLNode<T>* node = m_Head;

        for (uint32_t i = 0; i < index; i++)
        {
            node = node->next;
        }

        return node->value;
    }

    uint32_t    size() { return m_Size; }
    bool        empty() { return m_Size == 0; }

    T&          front() { return m_Head->value; }
    const T&    front() const { return m_Head->value; }

    T&          back() { return m_Tail->value; }
    const T&    back() const { return m_Tail->value; }

    void push_back(const T& data)
    {
        if (!m_Size)
        {
            m_Head = new LvnLNode<T>();
            m_Head->value = data;
            m_Tail = m_Head;
            m_Size++;
            return;
        }

        LvnLNode<T>* node = m_Tail;
        node->next = new LvnLNode<T>();
        m_Tail = node->next;
        m_Tail->value = data;
        m_Tail->prev = node;
        m_Size++;
    }

    void push_front(const T& data)
    {
        if (!m_Size)
        {
            m_Head = new LvnLNode<T>();
            m_Head->value = data;
            m_Tail = m_Head;
            m_Size++;
            return;
        }

        LvnLNode<T>* node = new LvnLNode<T>();
        node->value = data;
        node->next = m_Head;
        m_Head->prev = node;
        m_Head = node;
        m_Size++;
    }

    void pop_back()
    {
        if (!m_Size) { return; }
        if (m_Size == 1) { delete m_Tail; m_Tail = m_Head = nullptr; m_Size--; return; }

        LvnLNode<T>* node = m_Tail->prev;
        node->next = nullptr;
        delete m_Tail;
        m_Tail = node;
        m_Size--;
    }

    void pop_front()
    {
        if (!m_Size) { return; }
        if (m_Size == 1) { delete m_Head; m_Head = m_Tail = nullptr; m_Size--; return; }

        LvnLNode<T>* node = m_Head->next;
        node->prev = nullptr;
        delete m_Head;
        m_Head = node;
        m_Size--;
    }
};

class LvnMemoryBlock
{
private:
    void* m_Memory;
    uint64_t m_Size;

public:
    LvnMemoryBlock() : m_Memory(nullptr), m_Size(0) {}

    LvnMemoryBlock(uint64_t memsize)
        : m_Size(memsize)
    {
        m_Memory = calloc(1, memsize);
        LVN_CORE_ASSERT(m_Memory, "malloc failure when allocating memory block");
    }

    LvnMemoryBlock(const LvnMemoryBlock& other)
    {
        m_Size = other.m_Size;
        m_Memory = malloc(m_Size);
        LVN_CORE_ASSERT(m_Memory, "malloc failure when allocating memory block");
        memcpy(m_Memory, other.m_Memory, other.m_Size);
    }

    LvnMemoryBlock& operator =(const LvnMemoryBlock& other)
    {
        m_Size = other.m_Size;
        void* memptr = realloc(m_Memory, m_Size);
        LVN_CORE_ASSERT(memptr, "malloc failure when allocating memory block");
        memcpy(memptr, other.m_Memory, other.m_Size);
        m_Memory = memptr;

        return *this;
    }

    ~LvnMemoryBlock()
    {
        free(m_Memory);
    }

    void* operator [](uint64_t bytes)
    {
        LVN_CORE_ASSERT(bytes < m_Size, "memory block byte index out of range");

        return (void*)((uint8_t*)m_Memory + bytes);
    }

    uint64_t size() { return m_Size; }
};

class LvnMemoryBinding
{
private:
    void* m_Data;
    uint64_t m_ObjSize, m_Size, m_Capacity;
    std::queue<void*> m_Available;

    LvnMemoryBinding* m_Next;

public:
    LvnMemoryBinding() : m_Data(nullptr), m_ObjSize(0), m_Size(0), m_Capacity(0), m_Next(nullptr) {}
    LvnMemoryBinding(void* data, uint64_t objSize, uint64_t count) : m_Data(data), m_ObjSize(objSize), m_Size(0), m_Capacity(count), m_Next(nullptr) {}

    void                 set_next_memory_binding(LvnMemoryBinding* next) { m_Next = next; }
    LvnMemoryBinding*    get_next_memory_binding() { return m_Next; }
    bool                 full() { return m_Size == m_Capacity && m_Available.empty(); }

    LvnMemoryBinding* find_empty_memory_binding()
    {
        if (!LvnMemoryBinding::full()) { return this; }

        if (m_Next != nullptr)
            return m_Next->find_empty_memory_binding();

        return nullptr;
    }

    void* take_next()
    {
        if (!m_Available.empty())
        {
            void* value = m_Available.front();
            m_Available.pop();
            return value;
        }

        if (m_Size == m_Capacity) // current memory block is full, get next memory block
        {
            LVN_CORE_ASSERT(m_Next != nullptr, "cannot take next memory index, next memory binding not set");
            return m_Next->take_next();
        }

        uint64_t index = m_Size;
        m_Size++;
        return &static_cast<uint8_t*>(m_Data)[index * m_ObjSize];
    }

    void push_back(void* value)
    {
        LVN_CORE_ASSERT(value != nullptr, "value is nullptr when pushing back into memory binding queue");

        m_Available.push(value);
    }
};

struct LvnMemoryPool
{
    LvnMemoryBlock baseMemoryBlock;
    LvnVector<LvnList<LvnMemoryBlock>> memBlocks;
    LvnVector<LvnList<LvnMemoryBinding>> memBindings;
};


// -- [SUBSECT]: Logging Data Structures
// ------------------------------------------------------------

struct LvnLogger
{
    LvnString loggerName;
    LvnString logPatternFormat;
    LvnLogLevel logLevel;
    LvnVector<LvnLogPattern> logPatterns;

    LvnLogFile logfile;
};


// ------------------------------------------------------------
// [SECTION]: Window Internal Structs
// ------------------------------------------------------------

// -- [SUBSECT]: Event Data Structures
// ------------------------------------------------------------

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


// -- [SUBSECT]: Window Data Structures
// ------------------------------------------------------------

struct LvnWindowData
{                                        // [Same use with LvnWindowCreateinfo]
    int width, height;                   // width and height of window
    LvnString title;                     // title of window
    int minWidth, minHeight;             // minimum width and height of window
    int maxWidth, maxHeight;             // maximum width and height of window
    bool fullscreen, resizable, vSync;   // sets window to fullscreen; enables window resizing; vSync controls window framerate
    LvnWindowIconData* pIcons;           // icon images used for window/app icon
    uint32_t iconCount;                  // iconCount is the number of icons in pIcons
    void (*eventCallBackFn)(LvnEvent*);  // function ptr used as a callback to get events from this window
    void* userData;
};

struct LvnRenderPass
{
    void* nativeRenderPass;
};

/*
  LvnWindow struct is used to create a window on the system
  - Stores window data (eg. width, height, title)
  - window data is changed from the window api being used (eg. glfw)
  Note: LvnWindow is an imcomplete data type; LvnWindow needs to be
        allocated and destroyed with its corresponding functions.
        Use lvn::createWindow() and lvn::destroyWindow()
*/
struct LvnWindow
{
    LvnWindowData data;              // holds data of window (eg. width, height)
    void* nativeWindow;              // pointer to window api handle (eg. GLFWwindow)
    void* apiData;                   // used for graphics api related uses
    LvnRenderPass renderPass;        // pointer to native render pass for this window (vulkan)
    uint32_t topologyTypeEnum;       // topologyType used to render primitives (opengl)
    uint32_t vao;                    // vertex array object per pipeline object (opengl)
    uint32_t indexOffset;            // index offset when binding index buffer (opengl)
    std::unordered_map<uint32_t, uint32_t>* bindingDescriptions;
    LvnVector<uint8_t> cmdBuffer;    // command buffer to store draw commands in byte data
};


// -- [SUBSECT]: Window Context Structure
// ------------------------------------------------------------

struct LvnWindowContext
{
    LvnWindowApi        windowapi;    /* window api enum */

    LvnResult           (*createWindow)(LvnWindow*, const LvnWindowCreateInfo*);
    void                (*destroyWindow)(LvnWindow*);
    void                (*updateWindow)(LvnWindow*);
    bool                (*windowOpen)(LvnWindow*);
    void                (*windowPollEvents)();
    LvnPair<int>        (*getDimensions)(LvnWindow*);
    unsigned int        (*getWindowWidth)(LvnWindow*);
    unsigned int        (*getWindowHeight)(LvnWindow*);
    void                (*setWindowVSync)(LvnWindow*, bool);
    bool                (*getWindowVSync)(LvnWindow*);
    void                (*setWindowContextCurrent)(LvnWindow*);

    bool                (*keyPressed)(LvnWindow*, int);
    bool                (*keyReleased)(LvnWindow*, int);
    bool                (*mouseButtonPressed)(LvnWindow*, int);
    bool                (*mouseButtonReleased)(LvnWindow*, int);

    LvnPair<float>      (*getMousePos)(LvnWindow*);
    void                (*getMousePosPtr)(LvnWindow*, float*, float*);
    float               (*getMouseX)(LvnWindow*);
    float               (*getMouseY)(LvnWindow*);
    void                (*setMouseCursor)(LvnWindow*, LvnMouseCursor);
    void                (*SetMouseInputMode)(LvnWindow*, LvnMouseInputMode);

    LvnPair<int>        (*getWindowPos)(LvnWindow*);
    void                (*getWindowPosPtr)(LvnWindow*, int*, int*);
    LvnPair<int>        (*getWindowSize)(LvnWindow*);
    void                (*getWindowSizePtr)(LvnWindow*, int*, int*);
};


// ------------------------------------------------------------
// [SECTION]: Graphics Internal Structs
// ------------------------------------------------------------

// -- [SUBSECT]: Graphics Context
// ------------------------------------------------------------

struct LvnGraphicsContext
{
    LvnGraphicsApi              graphicsapi;
    bool                        enableGraphicsApiDebugLogs;
    LvnTextureFormat            frameBufferColorFormat;
    uint32_t                    maxFramesInFlight;

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
    LvnResult                   (*createCubemapHdr)(LvnCubemap*, const LvnCubemapHdrCreateInfo*);

    void                        (*destroyShader)(LvnShader*);
    void                        (*destroyDescriptorLayout)(LvnDescriptorLayout*);
    void                        (*destroyPipeline)(LvnPipeline*);
    void                        (*destroyFrameBuffer)(LvnFrameBuffer*);
    void                        (*destroyBuffer)(LvnBuffer*);
    void                        (*destroySampler)(LvnSampler*);
    void                        (*destroyTexture)(LvnTexture*);
    void                        (*destroyCubemap)(LvnCubemap*);

    void                        (*renderBeginNextFrame)(LvnWindow*);
    void                        (*renderDrawSubmit)(LvnWindow*);
    void                        (*renderBeginCommandRecording)(LvnWindow*);
    void                        (*renderEndCommandRecording)(LvnWindow*);
    void                        (*renderCmdDraw)(LvnWindow*, uint32_t);
    void                        (*renderCmdDrawIndexed)(LvnWindow*, uint32_t);
    void                        (*renderCmdDrawInstanced)(LvnWindow*, uint32_t, uint32_t, uint32_t);
    void                        (*renderCmdDrawIndexedInstanced)(LvnWindow*, uint32_t, uint32_t, uint32_t);
    void                        (*renderCmdSetStencilReference)(uint32_t);
    void                        (*renderCmdSetStencilMask)(uint32_t, uint32_t);
    void                        (*renderCmdBeginRenderPass)(LvnWindow*, float r, float g, float b, float a);
    void                        (*renderCmdEndRenderPass)(LvnWindow*);
    void                        (*renderCmdBindPipeline)(LvnWindow*, LvnPipeline*);
    void                        (*renderCmdBindVertexBuffer)(LvnWindow*, uint32_t, uint32_t, LvnBuffer**, uint64_t*);
    void                        (*renderCmdBindIndexBuffer)(LvnWindow*, LvnBuffer*, uint64_t);
    void                        (*renderCmdBindDescriptorSets)(LvnWindow*, LvnPipeline*, uint32_t, uint32_t, LvnDescriptorSet**);
    void                        (*renderCmdBeginFrameBuffer)(LvnWindow*, LvnFrameBuffer*);
    void                        (*renderCmdEndFrameBuffer)(LvnWindow*, LvnFrameBuffer*);

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


// -- [SUBSECT]: Draw Command Buffer Structures
// ------------------------------------------------------------

struct LvnDrawCmdHeader
{
    uint64_t size;
    void (*callFunc)(void*);
};

struct LvnCmdDraw
{
    LvnDrawCmdHeader header;
    LvnWindow* window;
    uint32_t vertexCount;
};

struct LvnCmdDrawIndexed
{
    LvnDrawCmdHeader header;
    LvnWindow* window;
    uint32_t indexCount;
};

struct LvnCmdDrawInstanced
{
    LvnDrawCmdHeader header;
    LvnWindow* window;
    uint32_t vertexCount;
    uint32_t instanceCount;
    uint32_t firstInstance;
};

struct LvnCmdDrawIndexedInstanced
{
    LvnDrawCmdHeader header;
    LvnWindow* window;
    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstInstance;
};

struct LvnCmdSetStencilReference
{
    LvnDrawCmdHeader header;
    uint32_t reference;
};

struct LvnCmdSetStencilMask
{
    LvnDrawCmdHeader header;
    uint32_t compareMask;
    uint32_t writeMask;
};

struct LvnCmdBeginRenderPass
{
    LvnDrawCmdHeader header;
    LvnWindow* window;
    float r, g, b, a;
};

struct LvnCmdEndRenderPass
{
    LvnDrawCmdHeader header;
    LvnWindow* window;
};

struct LvnCmdBindPipeline
{
    LvnDrawCmdHeader header;
    LvnWindow* window;
    LvnPipeline* pipeline;
};

struct LvnCmdBindVertexBuffer
{
    LvnDrawCmdHeader header;
    LvnWindow* window;
    uint32_t firstBinding;
    uint32_t bindingCount;
    LvnBuffer** pBuffers;
    uint64_t* pOffsets;
};

struct LvnCmdBindIndexBuffer
{
    LvnDrawCmdHeader header;
    LvnWindow* window;
    LvnBuffer* buffer;
    uint64_t offset;
};

struct LvnCmdBindDescriptorSets
{
    LvnDrawCmdHeader header;
    LvnWindow* window;
    LvnPipeline* pipeline;
    uint32_t firstSetIndex;
    uint32_t descriptorSetCount;
    LvnDescriptorSet** pDescriptorSets;
};

struct LvnCmdBeginFrameBuffer
{
    LvnDrawCmdHeader header;
    LvnWindow* window;
    LvnFrameBuffer* frameBuffer;
};

struct LvnCmdEndFrameBuffer
{
    LvnDrawCmdHeader header;
    LvnWindow* window;
    LvnFrameBuffer* frameBuffer;
};


// -- [SUBSECT]: General Graphics Structures
// ------------------------------------------------------------

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

    std::unordered_map<uint32_t, uint32_t> bindingDescriptions;
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


// ------------------------------------------------------------
// [SECTION]: Context Internal Structs
// ------------------------------------------------------------

// -- [SUBSECT]: Memory Alloc Structures
// ------------------------------------------------------------

struct LvnStructureTypeInfo
{
    LvnStructureType sType;
    uint64_t size;
    uint64_t count;
};

struct LvnObjectMemAllocCount
{
    struct LvnStructCounts
    {
        LvnStructureType sType;
        size_t count;
    };

    LvnVector<LvnStructCounts> sTypes;
};


// -- [SUBSECT]: Renderer Structures
// ------------------------------------------------------------

struct LvnRenderMode
{
    using LvnRenderModeFunc = void (*)(LvnRenderer*, LvnRenderMode&);

    LvnRenderModeEnum modes;
    LvnDrawList drawList;

    LvnPipeline* pipeline;
    LvnDescriptorLayout* descriptorLayout;
    LvnDescriptorSet* descriptorSet;
    LvnBuffer* buffer;

    uint64_t maxVertexCount;
    uint64_t maxIndexCount;
    uint64_t indexOffset;
    uint64_t uniformOffset;

    LvnRenderModeFunc drawFunc;
};

struct LvnRenderer
{
    LvnWindow* window;
    LvnVec4 clearColor;
    LvnFont defaultFont;
    LvnTexture* defaultWhiteTexture;
    LvnTexture* defaultFontTexture;
    LvnVector<LvnRenderMode> renderModes;
};


// -- [SUBSECT]: Context Structure
// ------------------------------------------------------------

struct LvnContext
{
    // api specification
    LvnWindowApi                         windowapi;
    LvnWindowContext                     windowContext;
    LvnGraphicsApi                       graphicsapi;
    LvnGraphicsContext                   graphicsContext;
    void*                                audioEngineContextPtr;
    LvnClipRegion                        matrixClipRegion;
    LvnString                            appName;
    LvnPipelineSpecification             defaultPipelineSpecification;
    bool                                 multithreading;

    // logging
    bool                                 logging;
    bool                                 enableCoreLogging;
    LvnLogger                            coreLogger;
    LvnLogger                            clientLogger;
    LvnVector<LvnLogPattern>             userLogPatterns;

    // memory pools and bindings
    LvnMemAllocMode                      memoryMode;
    LvnMemoryPool                        memoryPool;
    LvnVector<LvnStructureTypeInfo>      sTypeMemAllocInfos;
    LvnVector<LvnStructureTypeInfo>      blockMemAllocInfos;

    // memory object allocations
    size_t                               numMemoryAllocations;
    size_t                               numClassObjectAllocations;
    LvnObjectMemAllocCount               objectMemoryAllocations;

    // renderer
    std::unique_ptr<LvnRenderer>         renderer;

    // misc
    LvnTimer                             contexTime;       // timer
    LvnData<uint32_t>                    defaultCodePoints;

};


// ------------------------------------------------------------
// [SECTION]: Internal Functions
// ------------------------------------------------------------

template<typename T> struct LvnRemoveReference { using type = T; };
template<typename T> struct LvnRemoveReference<T&> { using type = T; };
template<typename T> struct LvnRemoveReference<T&&> { using type = T; };

namespace lvn
{
    template <typename T>
    constexpr typename LvnRemoveReference<T>::type&& move(T&& arg)
    {
        return static_cast<typename LvnRemoveReference<T>::type&&>(arg);
    }

    template <typename T>
    void swap(T& arg1, T& arg2)
    {
        T temp = lvn::move(arg1);
        arg1 = lvn::move(arg2);
        arg2 = lvn::move(temp);
    }

    template <typename T, size_t N>
    void swap(T (&arg1)[N], T (&arg2)[N])
    {
        for (size_t i = 0; i < N; i++)
            lvn::swap(arg1[i], arg2[i]);
    }
}

#endif
