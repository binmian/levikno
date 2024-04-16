#ifndef HG_LEVIKNO_INTERNAL_H
#define HG_LEVIKNO_INTERNAL_H

#include "levikno.h"

#include <vector>


struct LvnMemoryBlockPointers
{
	LvnMemoryBlock* memBlockWindows;
	LvnMemoryBlock* memBlockLoggers;
	LvnMemoryBlock* memBlockRenderPasses;
	LvnMemoryBlock* memBlockFrameBuffers;
	LvnMemoryBlock* memBlockShaders;
	LvnMemoryBlock* memBlockPipelines;
	LvnMemoryBlock* memBlockBuffers;
};

// ------------------------------------------------------------
// [SECTION]: Core Internal structs
// ------------------------------------------------------------

struct LvnLogger
{
	const char* loggerName;
	LvnLogLevel logLevel;
	const char* logPatternFormat;
	std::vector<LvnLogPattern> logPatterns;
};

struct LvnMemoryPool
{
	LvnMemoryBlock* pMemoryBlocks;
	uint32_t memoryBlockCount;
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
	LvnWindowData data;    /* holds data of window (eg. width, height) */
	void* nativeWindow;    /* pointer to window api handle (eg. GLFWwindow) */
	void* apiData;         /* used for graphics api related uses */
};

struct LvnWindowContext
{
	LvnWindowApi        windowapi;    /* window api enum */

	LvnResult           (*createWindow)(LvnWindow*, LvnWindowCreateInfo*);
	void                (*destroyWindow)(LvnWindow*);
	void                (*updateWindow)(LvnWindow*);
	bool                (*windowOpen)(LvnWindow*);
	LvnWindowDimensions (*getDimensions)(LvnWindow*);
	unsigned int        (*getWindowWidth)(LvnWindow*);
	unsigned int        (*getWindowHeight)(LvnWindow*);
	void                (*setWindowVSync)(LvnWindow*, bool);
	bool                (*getWindowVSync)(LvnWindow*);
	void                (*setWindowContextCurrent)(LvnWindow*);
};


// ------------------------------------------------------------
// [SECTION]: Graphics Internal structs
// ------------------------------------------------------------

struct LvnGraphicsContext
{
	LvnGraphicsApi              graphicsapi;

	void                        (*getPhysicalDevices)(LvnPhysicalDevice**, uint32_t*);
	LvnResult                   (*renderInit)(LvnRenderInitInfo*);

	LvnResult                   (*createShaderFromSrc)(LvnShader*, LvnShaderCreateInfo*);
	LvnResult                   (*createShaderFromFileSrc)(LvnShader*, LvnShaderCreateInfo*);
	LvnResult                   (*createShaderFromFileBin)(LvnShader*, LvnShaderCreateInfo*);
	LvnResult                   (*createDescriptorLayout)(LvnDescriptorLayout*, LvnDescriptorLayoutCreateInfo*);
	LvnResult                   (*createPipeline)(LvnPipeline*, LvnPipelineCreateInfo*);
	LvnResult                   (*createFrameBuffer)(LvnFrameBuffer*, LvnFrameBufferCreateInfo*);
	LvnResult                   (*createBuffer)(LvnBuffer*, LvnBufferCreateInfo*);
	LvnResult                   (*createUniformBuffer)(LvnUniformBuffer*, LvnUniformBufferCreateInfo*);
	LvnResult                   (*createTexture)(LvnTexture*, LvnTextureCreateInfo*);

	void                        (*destroyShader)(LvnShader*);
	void                        (*destroyDescriptorLayout)(LvnDescriptorLayout*);
	void                        (*destroyPipeline)(LvnPipeline*);
	void                        (*destroyFrameBuffer)(LvnFrameBuffer*);
	void                        (*destroyBuffer)(LvnBuffer*);
	void                        (*destroyUniformBuffer)(LvnUniformBuffer*);
	void                        (*destroyTexture)(LvnTexture*);

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
	void                        (*renderCmdBindDescriptorLayout)(LvnWindow*, LvnPipeline*, LvnDescriptorLayout*);
	void                        (*renderCmdBeginFrameBuffer)(LvnWindow*, LvnFrameBuffer*);
	void                        (*renderCmdEndFrameBuffer)(LvnWindow*, LvnFrameBuffer*);

	void                        (*setDefaultPipelineSpecification)(LvnPipelineSpecification*);
	LvnPipelineSpecification    (*getDefaultPipelineSpecification)();
	void                        (*updateUniformBufferData)(LvnWindow*, LvnUniformBuffer*, void*, uint64_t);
	void                        (*updateDescriptorLayoutData)(LvnDescriptorLayout*, LvnDescriptorUpdateInfo*, uint32_t);
	LvnTexture*                 (*getFrameBufferImage)(LvnFrameBuffer*, uint32_t);
	LvnRenderPass*              (*getFrameBufferRenderPass)(LvnFrameBuffer*);
	void                        (*updateFrameBuffer)(LvnFrameBuffer*, uint32_t, uint32_t);
	void                        (*setFrameBufferClearColor)(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a);
};

struct LvnPhysicalDevice
{
	typedef void* LvnPhysicalDeviceHandle;

	LvnPhysicalDeviceInfo info;
	LvnPhysicalDeviceHandle device;
};

struct LvnRenderPass
{
	void* nativeRenderPass;
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

struct LvnMesh
{
	LvnBuffer* buffer;    // single buffer that contains both vertex and index buffer
	LvnMat4 matrix;       // model matrix of mesh
};

struct LvnContext
{
	LvnWindowApi                windowapi;
	LvnWindowContext            windowContext;
	LvnGraphicsApi              graphicsapi;
	LvnGraphicsContext          graphicsContext;

	bool                        logging;
	LvnLogger                   coreLogger;
	LvnLogger                   clientLogger;
	std::vector<LvnLogPattern>  userLogPatterns;

	LvnPipelineSpecification    defaultPipelineSpecification;
	LvnMemoryPool               memoryPool;
	LvnMemoryBlockPointers      memBlockPtrs;
	size_t                      numMemoryAllocations;
};

#endif
