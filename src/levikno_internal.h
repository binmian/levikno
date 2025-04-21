#ifndef HG_LEVIKNO_INTERNAL_H
#define HG_LEVIKNO_INTERNAL_H

#include "levikno.h"

#include <set>


// ------------------------------------------------------------
// [SECTION]: Core Internal structs
// ------------------------------------------------------------


// [SUBSECT]: -- Internal Data Structures
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
	std::vector<LvnList<LvnMemoryBlock>> memBlocks;
	std::vector<LvnList<LvnMemoryBinding>> memBindings;
};

template <typename T>
class LvnIDQueue
{
	static_assert(std::is_same<T, uint8_t>::value ||
		std::is_same<T, uint16_t>::value ||
		std::is_same<T, uint32_t>::value ||
		std::is_same<T, uint64_t>::value ||
		std::is_same<T, size_t>::value,
		"ID queue type error, queue template type must be an unsigned integer");

private:
	std::queue<T> m_Available;
	std::set<T> m_Used;
	T m_NextID;

public:
	LvnIDQueue() : m_NextID(0) {}

	bool push_back(T element)
	{
		if (m_Used.find(element) != m_Used.end())
		{
			m_Available.push(element);
			m_Used.erase(element);
			return true;
		}

		return false;
	}

	// Takes the next element from the queue
	T take_next()
	{
		if (!m_Available.empty())
		{
			T id = m_Available.front();
			m_Available.pop();
			m_Used.insert(id);
			return id;
		}

		LVN_CORE_ASSERT(m_NextID < SIZE_MAX, "max IDs reached in IDQueue");

		m_NextID++;
		m_Used.insert(m_NextID);
		return m_NextID;
	}

	auto begin() { return m_Used.begin(); }
	auto end() { return m_Used.end(); }
};

typedef LvnIDQueue<uint8_t>  LvnIDQueue8;
typedef LvnIDQueue<uint16_t> LvnIDQueue16;
typedef LvnIDQueue<uint32_t> LvnIDQueue32;
typedef LvnIDQueue<uint64_t> LvnIDQueue64;


// [SUBSECT]: -- Logging
// ------------------------------------------------------------

struct LvnLogger
{
	std::string loggerName;
	std::string logPatternFormat;
	LvnLogLevel logLevel;
	std::vector<LvnLogPattern> logPatterns;

	LvnLogFile logfile;
};


// ------------------------------------------------------------
// [SECTION]: Window Internal structs
// ------------------------------------------------------------


// [SUBSECT]: -- Events
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


// [SUBSECT]: -- Windows
// ------------------------------------------------------------
struct LvnWindowData
{                                        // [Same use with LvnWindowCreateinfo]
	int width, height;                   // width and height of window
	std::string title;                   // title of window
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
	std::vector<uint8_t> cmdBuffer;  // command buffer to store draw commands in byte data
};

struct LvnWindowContext
{
	LvnWindowApi        windowapi;    /* window api enum */

	LvnResult           (*createWindow)(LvnWindow*, LvnWindowCreateInfo*);
	void                (*destroyWindow)(LvnWindow*);
	void                (*updateWindow)(LvnWindow*);
	bool                (*windowOpen)(LvnWindow*);
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
// [SECTION]: Graphics Internal structs
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

	LvnResult                   (*createShaderFromSrc)(LvnShader*, LvnShaderCreateInfo*);
	LvnResult                   (*createShaderFromFileSrc)(LvnShader*, LvnShaderCreateInfo*);
	LvnResult                   (*createShaderFromFileBin)(LvnShader*, LvnShaderCreateInfo*);
	LvnResult                   (*createDescriptorLayout)(LvnDescriptorLayout*, LvnDescriptorLayoutCreateInfo*);
	LvnResult                   (*allocateDescriptorSet)(LvnDescriptorSet*, LvnDescriptorLayout*);
	LvnResult                   (*createPipeline)(LvnPipeline*, LvnPipelineCreateInfo*);
	LvnResult                   (*createFrameBuffer)(LvnFrameBuffer*, LvnFrameBufferCreateInfo*);
	LvnResult                   (*createBuffer)(LvnBuffer*, LvnBufferCreateInfo*);
	LvnResult                   (*createUniformBuffer)(LvnUniformBuffer*, LvnUniformBufferCreateInfo*);
	LvnResult                   (*createSampler)(LvnSampler*, LvnSamplerCreateInfo*);
	LvnResult                   (*createTexture)(LvnTexture*, LvnTextureCreateInfo*);
	LvnResult                   (*createTextureSampler)(LvnTexture*, LvnTextureSamplerCreateInfo*);
	LvnResult                   (*createCubemap)(LvnCubemap*, LvnCubemapCreateInfo*);
	LvnResult                   (*createCubemapHdr)(LvnCubemap*, LvnCubemapHdrCreateInfo*);

	void                        (*destroyShader)(LvnShader*);
	void                        (*destroyDescriptorLayout)(LvnDescriptorLayout*);
	void                        (*destroyPipeline)(LvnPipeline*);
	void                        (*destroyFrameBuffer)(LvnFrameBuffer*);
	void                        (*destroyBuffer)(LvnBuffer*);
	void                        (*destroyUniformBuffer)(LvnUniformBuffer*);
	void                        (*destroySampler)(LvnSampler*);
	void                        (*destroyTexture)(LvnTexture*);
	void                        (*destroyCubemap)(LvnCubemap*);

	void                        (*renderClearColor)(LvnWindow*, float r, float g, float b, float a);
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
	void                        (*renderCmdBeginRenderPass)(LvnWindow*);
	void                        (*renderCmdEndRenderPass)(LvnWindow*);
	void                        (*renderCmdBindPipeline)(LvnWindow*, LvnPipeline*);
	void                        (*renderCmdBindVertexBuffer)(LvnWindow*, LvnBuffer*);
	void                        (*renderCmdBindIndexBuffer)(LvnWindow*, LvnBuffer*);
	void                        (*renderCmdBindDescriptorSets)(LvnWindow*, LvnPipeline*, uint32_t, uint32_t, LvnDescriptorSet**);
	void                        (*renderCmdBeginFrameBuffer)(LvnWindow*, LvnFrameBuffer*);
	void                        (*renderCmdEndFrameBuffer)(LvnWindow*, LvnFrameBuffer*);

	void                        (*bufferUpdateVertexData)(LvnBuffer*, void*, uint64_t, uint64_t);
	void                        (*bufferUpdateIndexData)(LvnBuffer*, uint32_t*, uint64_t, uint64_t);
	void                        (*bufferResizeVertexBuffer)(LvnBuffer*, uint64_t);
	void                        (*bufferResizeIndexBuffer)(LvnBuffer*, uint64_t);
	void                        (*updateUniformBufferData)(LvnUniformBuffer*, void*, uint64_t, uint64_t);
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
	LvnBuffer* buffer;
};

struct LvnCmdBindIndexBuffer
{
	LvnDrawCmdHeader header;
	LvnWindow* window;
	LvnBuffer* buffer;
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

	std::vector<LvnDescriptorSet> descriptorSets;
	uint64_t descriptorSetIndex;
};

struct LvnDescriptorSet
{
	std::vector<void*> descriptorSets;
	void* singleSet;
};

struct LvnPipeline
{
	void* nativePipeline;
	void* nativePipelineLayout;

	uint32_t id;
	uint32_t vaoId;
};

struct LvnBuffer
{
	uint32_t type;

	uint32_t vertexBufferSize;
	uint32_t indexBufferSize;

	void* vertexBuffer;
	void* vertexBufferMemory;
	void* indexBuffer;
	void* indexBufferMemory;

	uint64_t indexOffset;

	uint32_t id;
	uint32_t vboId;
	uint32_t iboId;
};

struct LvnUniformBuffer
{
	void* uniformBuffer;
	void* uniformBufferMemory;
	void* uniformBufferMapped;
	uint64_t size;

	uint32_t id;
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
// [SECTION]: Context Internal structs
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

	std::vector<LvnStructCounts> sTypes;
};

struct LvnContext
{
	// api specification
	LvnWindowApi                         windowapi;
	LvnWindowContext                     windowContext;
	LvnGraphicsApi                       graphicsapi;
	LvnGraphicsContext                   graphicsContext;
	void*                                audioEngineContextPtr;
	LvnClipRegion                        matrixClipRegion;
	std::string                          appName;
	LvnPipelineSpecification             defaultPipelineSpecification;
	bool                                 multithreading;

	// logging
	bool                                 logging;
	bool                                 enableCoreLogging;
	LvnLogger                            coreLogger;
	LvnLogger                            clientLogger;
	std::vector<LvnLogPattern>           userLogPatterns;

	// memory pools and bindings
	LvnMemAllocMode                      memoryMode;
	LvnMemoryPool                        memoryPool;
	std::vector<LvnStructureTypeInfo>    sTypeMemAllocInfos;
	std::vector<LvnStructureTypeInfo>    blockMemAllocInfos;

	// memory object allocations
	size_t                               numMemoryAllocations;
	LvnObjectMemAllocCount               objectMemoryAllocations;

	// ecs entity id manager
	std::queue<LvnEntity>                availableEntityIDs;
	uint64_t                             entityIndexID, maxEntityIDs;

	// misc
	LvnTimer                             contexTime;       // timer
	LvnComponentManager                  componentManager; // ECS component manager

};


#endif
