#ifndef HG_LEVIKNO_INTERNAL_H
#define HG_LEVIKNO_INTERNAL_H

#include "levikno.h"


// ------------------------------------------------------------
// [SECTION]: Data Type Structures
// ------------------------------------------------------------
template <typename T>
struct LvnVector
{
	T* m_Data;            // pointer array to data
	uint32_t m_Size;      // number of elements that are in this vector; size of vector
	uint32_t m_Capacity;  // max number of elements allocated/reserved for this vector; note that m_Size can be less than or equal to the capacity

	LvnVector()
		: m_Data((T*)lvn::memAlloc(0)), m_Size(0), m_Capacity(0) {}

	~LvnVector() { lvn::memFree(m_Data); }

	LvnVector(uint32_t size)
	{
		m_Size = size;
		m_Capacity = size;
		m_Data = (T*)lvn::memAlloc(size * sizeof(T));
	}
	LvnVector(T* data, uint32_t size)
	{
		m_Size = size;
		m_Capacity = size;
		m_Data = (T*)lvn::memAlloc(size * sizeof(T));
		memcpy(m_Data, data, size * sizeof(T));
	}
	LvnVector(uint32_t size, const T& data)
	{
		m_Size = size;
		m_Capacity = size;
		m_Data = (T*)lvn::memAlloc(size * sizeof(T));
		for (uint32_t i = 0; i < size; i++)
			memcpy(&m_Data[i], &data, sizeof(T));
	}
	LvnVector(const LvnVector<T>& lvnvec)
	{
		m_Size = lvnvec.m_Size;
		m_Capacity = lvnvec.m_Size;
		m_Data = (T*)lvn::memAlloc(m_Size * sizeof(T));
		memcpy(m_Data, lvnvec.m_Data, m_Size * sizeof(T));
	}
	LvnVector<T>& operator=(const LvnVector<T>& lvnvec)
	{
		resize(lvnvec.m_Size);
		memcpy(m_Data, lvnvec.m_Data, m_Size * sizeof(T));
		return *this;
	}
	T& operator[](uint32_t i)
	{
		LVN_CORE_ASSERT(i < m_Size, "index out of vector size range");
		return m_Data[i];
	}
	const T& operator[](uint32_t i) const
	{
		LVN_CORE_ASSERT(i < m_Size, "index out of vector size range");
		return m_Data[i];
	}

	T*          begin() { return m_Data; }
	const T*    begin() const { return m_Data; }
	T*          end() { return m_Data + m_Size; }
	const T*    end() const { return m_Data + m_Size; }
	T&          front() { LVN_CORE_ASSERT(m_Size > 0, "cannot access index of empty vector"); return m_Data[0]; }
	const T&    front() const { LVN_CORE_ASSERT(m_Size > 0, "cannot access index of empty vector"); return m_Data[0]; }
	T&          back() { LVN_CORE_ASSERT(m_Size > 0, "cannot access index of empty vector"); return m_Data[m_Size - 1]; }
	const T&    back() const { LVN_CORE_ASSERT(m_Size > 0, "cannot access index of empty vector"); return m_Data[m_Size - 1]; }


	bool        empty() { return m_Size == 0; }
	void        clear() { if (m_Data) { memset(m_Data, 0, m_Size * sizeof(T)); } m_Size = 0; }
	void        clear_free() { if (m_Data) { lvn::memFree(m_Data); m_Size = 0; m_Capacity = 0; m_Data = nullptr; } }
	void        erase(const T* it) { LVN_CORE_ASSERT(it >= m_Data && it < m_Data + m_Size, "element not within vector index!"); size_t off = it - m_Data; memmove(m_Data + off, m_Data + off + 1, (m_Size - off - 1) * sizeof(T)); m_Size--; }
	T*          data() { return m_Data; }
	uint32_t    size() const { return m_Size; }
	uint32_t    memsize() { return m_Size * sizeof(T); }
	uint32_t    memcap() { return m_Capacity * sizeof(T); }
	void        resize(uint32_t size) { if (size > m_Capacity) { reserve(size); } m_Size = size; }
	void        reserve(uint32_t size) { if (size <= m_Size) return; T* newsize = (T*)lvn::memAlloc(size * sizeof(T)); memcpy(newsize, m_Data, m_Size * sizeof(T)); lvn::memFree(m_Data); m_Data = newsize; m_Capacity = size; }

	void        push_back(const T& e) { resize(m_Size + 1); memcpy(&m_Data[m_Size - 1], &e, sizeof(T)); }
	void        copy_back(T* data, uint32_t size) { resize(m_Size + size); memcpy(&m_Data[m_Size - size], data, size * sizeof(T)); }
	void        remove(uint32_t index) { LVN_CORE_ASSERT(index < m_Size, "index out of vector size range"); uint32_t aftIndex = m_Size - index - 1; if (aftIndex != 0) { memcpy(&m_Data[index], &m_Data[index + 1], aftIndex * sizeof(T)); } resize(--m_Size); }

	T*          find(const T& e) { T* begin = m_Data; const T* end = m_Data + m_Size; while (begin < end) { if (*begin == e) break; begin++; } return begin; }
	uint32_t    find_index(const T& e) { T* begin = m_Data; const T* end = m_Data + m_Size; uint32_t i = 0; while (begin < end) { if (*begin == e) break; begin++; i++; } return i; } // NOTE: find_index acts similar to find function; if no value was found, returns the index one greater than the last index in the vector (m_Size)
};

// ------------------------------------------------------------
// [SECTION]: Core Internal structs
// ------------------------------------------------------------

struct LvnLogger
{
	const char* loggerName;
	LvnLogLevel logLevel;
	const char* logPatternFormat;
	LvnVector<LvnLogPattern> logPatterns;
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
	const char* title;                   // title of window
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
	LvnWindowData data;          // holds data of window (eg. width, height)
	void* nativeWindow;          // pointer to window api handle (eg. GLFWwindow)
	LvnRenderPass renderPass;    // pointer to native render pass for this window
	void* apiData;               // used for graphics api related uses
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

	void                (*setMousePos)(LvnWindow*, float, float);

	LvnPair<float>      (*getMousePos)(LvnWindow*);
	void                (*getMousePosPtr)(LvnWindow*, float*, float*);
	float               (*getMouseX)(LvnWindow*);
	float               (*getMouseY)(LvnWindow*);

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

	void                        (*destroyShader)(LvnShader*);
	void                        (*destroyDescriptorLayout)(LvnDescriptorLayout*);
	void                        (*destroyDescriptorSet)(LvnDescriptorSet*);
	void                        (*destroyPipeline)(LvnPipeline*);
	void                        (*destroyFrameBuffer)(LvnFrameBuffer*);
	void                        (*destroyBuffer)(LvnBuffer*);
	void                        (*destroyUniformBuffer)(LvnUniformBuffer*);
	void                        (*destroyTexture)(LvnTexture*);
	void                        (*destroyCubemap)(LvnCubemap*);

	void                        (*renderClearColor)(LvnWindow* window, float r, float g, float b, float a);
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

	void                        (*setDefaultPipelineSpecification)(LvnPipelineSpecification*);
	LvnPipelineSpecification    (*getDefaultPipelineSpecification)();
	void                        (*bufferUpdateVertexData)(LvnBuffer*, void*, uint32_t, uint32_t);
	void                        (*bufferUpdateIndexData)(LvnBuffer*, uint32_t*, uint32_t, uint32_t);
	void                        (*bufferResizeVertexBuffer)(LvnBuffer*, uint32_t);
	void                        (*bufferResizeIndexBuffer)(LvnBuffer*, uint32_t);
	void                        (*updateUniformBufferData)(LvnWindow*, LvnUniformBuffer*, void*, uint64_t);
	void                        (*updateDescriptorSetData)(LvnDescriptorSet*, LvnDescriptorUpdateInfo*, uint32_t);
	LvnTexture*                 (*getFrameBufferImage)(LvnFrameBuffer*, uint32_t);
	LvnRenderPass*              (*getFrameBufferRenderPass)(LvnFrameBuffer*);
	void                        (*updateFrameBuffer)(LvnFrameBuffer*, uint32_t, uint32_t);
	void                        (*setFrameBufferClearColor)(LvnFrameBuffer*, uint32_t, float, float, float, float);
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
};

struct LvnDescriptorLayout
{
	void* descriptorLayout;
	void* descriptorPool;
};

struct LvnDescriptorSet
{
	void** descriptorSets;
	uint32_t descriptorCount;
};

struct LvnPipeline
{
	void* nativePipeline;
	void* nativePipelineLayout;
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
};

struct LvnUniformBuffer
{
	void* uniformBuffer;
	void* uniformBufferMemory;
	void** uniformBufferMapped;
	uint64_t size;
};

struct LvnTexture
{
	void* image;
	void* imageMemory;
	void* imageView;
	void* sampler;
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
// [SECTION]: Audio Internal structs
// ------------------------------------------------------------

struct LvnSound
{
	float volume;
	float pan;
	float pitch;
	bool looping;

	LvnVec3 pos;

	void* soundPtr;
};


struct LvnContext
{
	LvnWindowApi                windowapi;
	LvnWindowContext            windowContext;
	LvnGraphicsApi              graphicsapi;
	LvnGraphicsContext          graphicsContext;
	void*                       audioEngineContextPtr;

	bool                        logging;
	LvnLogger                   coreLogger;
	LvnLogger                   clientLogger;
	LvnVector<LvnLogPattern>    userLogPatterns;

	LvnPipelineSpecification    defaultPipelineSpecification;
	size_t                      numMemoryAllocations;
};

#endif
