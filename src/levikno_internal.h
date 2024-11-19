#ifndef HG_LEVIKNO_INTERNAL_H
#define HG_LEVIKNO_INTERNAL_H

#include "levikno.h"
#include <cstring>


template <typename T>
struct LvnNode
{
	T data;
	LvnNode* next;
	LvnNode* prev;
};

template <typename T>
class LvnList
{
private:
	LvnNode<T>* m_Head;
	LvnNode<T>* m_Tail;
	uint32_t m_Size;

public:
	LvnList() : m_Head(nullptr), m_Tail(nullptr), m_Size(0) {}

	~LvnList()
	{
		while (m_Head != nullptr)
		{
			LvnNode<T>* node = m_Head;
			m_Head = node->next;
			delete node;
		}
	}

	T& operator [](uint32_t index)
	{
		LVN_CORE_ASSERT(index < m_Size, "list index out of range");

		LvnNode<T>* node = m_Head;

		for (uint32_t i = 0; i < index; i++)
		{
			node = node->next;
		}

		return node->data;
	}
	const T& operator [](uint32_t index) const
	{
		LVN_CORE_ASSERT(index < m_Size, "list index out of range");

		LvnNode<T>* node = m_Head;

		for (uint32_t i = 0; i < index; i++)
		{
			node = node->next;
		}

		return node->data;
	}

	uint32_t    size() { return m_Size; }
	bool        empty() { return m_Size == 0; }

	T&          front() { return m_Head->data; }
	const T&    front() const { return m_Head->data; }

	T&          back() { return m_Tail->data; }
	const T&    back() const { return m_Tail->data; }

	void push_back(const T& data)
	{
		if (!m_Size)
		{
			m_Head = new LvnNode<T>();
			m_Head->data = data;
			m_Tail = m_Head;
			m_Size++;
			return;
		}

		LvnNode<T>* node = m_Tail;
		node->next = new LvnNode<T>();
		m_Tail = node->next;
		m_Tail->data = data;
		m_Tail->prev = node;
		m_Size++;
	}

	void push_front(const T& data)
	{
		if (!m_Size)
		{
			m_Head = new LvnNode<T>();
			m_Head->data = data;
			m_Tail = m_Head;
			m_Size++;
			return;
		}

		LvnNode<T>* node = new LvnNode<T>();
		node->data = data;
		node->next = m_Head;
		m_Head->prev = node;
		m_Head = node;
		m_Size++;
	}

	void pop_back()
	{
		if (!m_Size) { return; }
		if (m_Size == 1) { delete m_Tail; m_Tail = m_Head = nullptr; m_Size--; return; }

		LvnNode<T>* node = m_Tail->prev;
		node->next = nullptr;
		delete m_Tail;
		m_Tail = node;
		m_Size--;
	}

	void pop_front()
	{
		if (!m_Size) { return; }
		if (m_Size == 1) { delete m_Head; m_Head = m_Tail = nullptr; m_Size--; return; }

		LvnNode<T>* node = m_Head->next;
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
		m_Memory = calloc(1, m_Size);
		LVN_CORE_ASSERT(m_Memory, "malloc failure when allocating memory block");
		memcpy(m_Memory, other.m_Memory, other.m_Size);
	}

	LvnMemoryBlock& operator =(const LvnMemoryBlock& other)
	{
		m_Size = other.m_Size;
		void* memptr = realloc(m_Memory, m_Size);
		LVN_CORE_ASSERT(memptr, "malloc failure when allocating memory block");
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
	bool                 full() { return m_Size == m_Capacity; }

	void* take_next()
	{
		if (!m_Available.empty())
		{
			void* value = m_Available.front();
			m_Available.pop();
			return value;
		}

		if (full()) // current memory block is full, get next memory block
		{
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
	LvnList<LvnMemoryBlock> memBlocks;

	std::vector<LvnList<LvnMemoryBinding>> memBindings;
};


// ------------------------------------------------------------
// [SECTION]: Core Internal structs
// ------------------------------------------------------------

struct LvnLogger
{
	std::string loggerName;
	std::string logPatternFormat;
	LvnLogLevel logLevel;
	std::vector<LvnLogPattern> logPatterns;
};


// ------------------------------------------------------------
// [SECTION]: Window Internal structs
// ------------------------------------------------------------

/* [Events] */
struct LvnEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;
	void* userData;

	union data
	{
		struct /* Mouse/Window pos & size */
		{
			union
			{
				struct
				{
					int x, y;
				};
				struct
				{
					double xd, yd;
				};
			};
		};
		struct /* key/mouse button codes */
		{
			int code;
			unsigned int ucode;
			bool repeat;
		};
	} data;
};


/* [Window] */
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
	bool                        enableValidationLayers;
	LvnTextureFormat            frameBufferColorFormat;
	uint32_t                    maxFramesInFlight;

	void                        (*getPhysicalDevices)(LvnPhysicalDevice**, uint32_t*);
	LvnResult                   (*checkPhysicalDeviceSupport)(LvnPhysicalDevice*);
	LvnResult                   (*renderInit)(LvnRenderInitInfo*);

	LvnResult                   (*createShaderFromSrc)(LvnShader*, LvnShaderCreateInfo*);
	LvnResult                   (*createShaderFromFileSrc)(LvnShader*, LvnShaderCreateInfo*);
	LvnResult                   (*createShaderFromFileBin)(LvnShader*, LvnShaderCreateInfo*);
	LvnResult                   (*createDescriptorLayout)(LvnDescriptorLayout*, LvnDescriptorLayoutCreateInfo*);
	LvnResult                   (*createDescriptorSet)(LvnDescriptorSet*, LvnDescriptorLayout*);
	LvnResult                   (*createPipeline)(LvnPipeline*, LvnPipelineCreateInfo*);
	LvnResult                   (*createFrameBuffer)(LvnFrameBuffer*, LvnFrameBufferCreateInfo*);
	LvnResult                   (*createBuffer)(LvnBuffer*, LvnBufferCreateInfo*);
	LvnResult                   (*createUniformBuffer)(LvnUniformBuffer*, LvnUniformBufferCreateInfo*);
	LvnResult                   (*createTexture)(LvnTexture*, LvnTextureCreateInfo*);
	LvnResult                   (*createCubemap)(LvnCubemap*, LvnCubemapCreateInfo*);
	LvnResult                   (*createCubemapHdr)(LvnCubemap*, LvnCubemapHdrCreateInfo*);

	void                        (*destroyShader)(LvnShader*);
	void                        (*destroyDescriptorLayout)(LvnDescriptorLayout*);
	void                        (*destroyDescriptorSet)(LvnDescriptorSet*);
	void                        (*destroyPipeline)(LvnPipeline*);
	void                        (*destroyFrameBuffer)(LvnFrameBuffer*);
	void                        (*destroyBuffer)(LvnBuffer*);
	void                        (*destroyUniformBuffer)(LvnUniformBuffer*);
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
	void                        (*updateUniformBufferData)(LvnWindow*, LvnUniformBuffer*, void*, uint64_t);
	void                        (*updateDescriptorSetData)(LvnDescriptorSet*, LvnDescriptorUpdateInfo*, uint32_t);
	LvnTexture*                 (*frameBufferGetImage)(LvnFrameBuffer*, uint32_t);
	LvnRenderPass*              (*frameBufferGetRenderPass)(LvnFrameBuffer*);
	void                        (*framebufferResize)(LvnFrameBuffer*, uint32_t, uint32_t);
	void                        (*frameBufferSetClearColor)(LvnFrameBuffer*, uint32_t, float, float, float, float);

	LvnDepthImageFormat         (*findSupportedDepthImageFormat)(LvnDepthImageFormat*, uint32_t);
};

struct LvnPhysicalDevice
{
	typedef void* LvnPhysicalDeviceHandle;

	LvnPhysicalDeviceInfo info;
	LvnPhysicalDeviceHandle device;
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
	std::vector<void*> uniformBufferMapped;
	uint64_t size;

	uint32_t id;
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
	LvnWindowApi                         windowapi;
	LvnWindowContext                     windowContext;
	LvnGraphicsApi                       graphicsapi;
	LvnGraphicsContext                   graphicsContext;
	void*                                audioEngineContextPtr;
	LvnClipRegion                        matrixClipRegion;

	bool                                 logging;
	bool                                 enableCoreLogging;
	LvnLogger                            coreLogger;
	LvnLogger                            clientLogger;
	std::vector<LvnLogPattern>           userLogPatterns;
	std::string                          appName;

	LvnPipelineSpecification             defaultPipelineSpecification;
	LvnTimer                             contexTime;
	LvnMemAllocMode                      memoryMode;
	LvnMemoryPool                        memoryPool;
	std::vector<LvnStructureTypeInfo>    sTypeMemAllocInfos;
	std::vector<LvnStructureTypeInfo>    blockMemAllocInfos;
	uint64_t                             blockMemSize;

	size_t                               numMemoryAllocations;
	LvnObjectMemAllocCount               objectMemoryAllocations;
};


#endif
