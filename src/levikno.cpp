#include "levikno.h"
#include "levikno_internal.h"

#include <ctime>
#include <unordered_map>

#include "stb_image.h"
#include "miniaudio.h"
#include "freetype/freetype.h"
#include "enet/enet.h"

#ifdef LVN_PLATFORM_WINDOWS
	#include <windows.h>
#endif

#define LVN_ABORT throw std::bad_alloc{};
#define LVN_EMPTY_STR "\0"
#define LVN_DEFAULT_LOG_PATTERN "[%Y-%m-%d] [%T] [%#%l%^] %n: %v%$"

#include "lvn_glfw.h"

#if defined(LVN_GRAPHICS_API_INCLUDE_VULKAN)
	#include "lvn_vulkan.h"
#endif

#include "lvn_opengl.h"

#include "lvn_loadModel.h"

static LvnContext* s_LvnContext = nullptr;


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

	ma_sound sound;
	LvnSoundBoard* soundBoard;
};

struct LvnSoundBoard
{
	float masterVolume;
	float masterPan;
	float masterPitch;

	std::unordered_map<uint32_t, LvnSound> sounds;
};


// ------------------------------------------------------------
// [SECTION]: Network Internal structs
// ------------------------------------------------------------

struct LvnSocket
{
	LvnSocketType type;

	ENetHost* socket;
	ENetPeer* connection;
	ENetPacket* packet;

	LvnAddress address;
	uint32_t channelCount;
	uint32_t connectionCount;
	uint32_t inBandWidth;
	uint32_t outBandWidth;
};


namespace lvn
{

static void                         enableLogANSIcodeColors();
static std::vector<LvnLogPattern>   logParseFormat(const char* fmt);
static const char*                  getLogLevelColor(LvnLogLevel level);
static const char*                  getLogLevelName(LvnLogLevel level);
static const char*                  getWindowApiNameEnum(LvnWindowApi api);
static const char*                  getGraphicsApiNameEnum(LvnGraphicsApi api);
static LvnResult                    setWindowContext(LvnContext* lvnctx, LvnWindowApi windowapi);
static void                         terminateWindowContext(LvnContext* lvnctx);
static LvnResult                    setGraphicsContext(LvnContext* lvnctx, LvnGraphicsApi graphicsapi);
static void                         terminateGraphicsContext(LvnContext* lvnctx);
static LvnResult                    initAudioContext(LvnContext* lvnctx);
static void                         terminateAudioContext(LvnContext* lvnctx);
static LvnResult                    initNetworkingContext();
static void                         terminateNetworkingContext();
static void                         initStandardPipelineSpecification(LvnContext* lvnctx);
static void                         setDefaultStructTypeMemAllocInfos(LvnContext* lvnctx);
static const char*                  getStructTypeEnumStr(LvnStructureType stype);
static uint64_t                     getStructTypeSize(LvnStructureType sType);
static void                         setMemoryBlockBindings(LvnMemoryPool* memPool, uint32_t blockIndex, LvnStructureTypeInfo* pStructInfos, uint32_t structInfoCount);
static void                         createContextMemoryPool(LvnContext* lvnctx, LvnContextCreateInfo* createInfo);
static void                         createBlockMemoryPool(LvnContext* lvnctx);

template <typename T>
static T* createObject(LvnContext* lvnctx, LvnStructureType sType);

template <typename T>
static void destroyObject(LvnContext* lvnctx, T* obj, LvnStructureType sType);


static void enableLogANSIcodeColors()
{
#ifdef LVN_PLATFORM_WINDOWS
	DWORD consoleMode;
	HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (GetConsoleMode(outputHandle, &consoleMode))
	{
		SetConsoleMode(outputHandle, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	}
#endif
}

static const char* getLogLevelColor(LvnLogLevel level)
{
	switch (level)
	{
		case Lvn_LogLevel_None:     { return LVN_LOG_COLOR_RESET; }
		case Lvn_LogLevel_Trace:    { return LVN_LOG_COLOR_TRACE; }
		case Lvn_LogLevel_Debug:    { return LVN_LOG_COLOR_DEBUG; }
		case Lvn_LogLevel_Info:     { return LVN_LOG_COLOR_INFO; }
		case Lvn_LogLevel_Warn:     { return LVN_LOG_COLOR_WARN; }
		case Lvn_LogLevel_Error:    { return LVN_LOG_COLOR_ERROR; }
		case Lvn_LogLevel_Fatal:    { return LVN_LOG_COLOR_FATAL; }
	}

	return nullptr;
}

static const char* getLogLevelName(LvnLogLevel level)
{
	switch (level)
	{
		case Lvn_LogLevel_None:     { return "none"; }
		case Lvn_LogLevel_Trace:    { return "trace"; }
		case Lvn_LogLevel_Debug:    { return "debug"; }
		case Lvn_LogLevel_Info:     { return "info"; }
		case Lvn_LogLevel_Warn:     { return "warn"; }
		case Lvn_LogLevel_Error:    { return "error"; }
		case Lvn_LogLevel_Fatal:    { return "fatal"; }
	}

	return nullptr;
}

static const char* getWindowApiNameEnum(LvnWindowApi api)
{
	switch (api)
	{
		case Lvn_WindowApi_None:  { return "None";  }
		case Lvn_WindowApi_glfw:  { return "glfw";  }
		// case Lvn_WindowApi_win32: { return "win32"; }
	}

	return LVN_EMPTY_STR;
}

static const char* getGraphicsApiNameEnum(LvnGraphicsApi api)
{
	switch (api)
	{
		case Lvn_GraphicsApi_None:   { return "None";   }
		case Lvn_GraphicsApi_vulkan: { return "vulkan"; }
		case Lvn_GraphicsApi_opengl: { return "opengl"; }
	}

	return LVN_EMPTY_STR;
}

static LvnResult setWindowContext(LvnContext* lvnctx, LvnWindowApi windowapi)
{
	LvnResult result = Lvn_Result_Failure;
	switch (windowapi)
	{
		case Lvn_WindowApi_None:
		{
			LVN_CORE_TRACE("no window context selected; window related function calls will not be used");
			return Lvn_Result_Success;
		}
		case Lvn_WindowApi_glfw:
		{
			result = glfwImplInitWindowContext(&lvnctx->windowContext);
			break;
		}
		// case Lvn_WindowApi_win32:
		// {
		// 	break;
		// }
	}

	//windowInputInit();

	if (result != Lvn_Result_Success)
		LVN_CORE_ERROR("could not create window context for: %s", getWindowApiNameEnum(windowapi));
	else
		LVN_CORE_TRACE("window context set: %s", getWindowApiNameEnum(windowapi));

	return result;
}

static void terminateWindowContext(LvnContext* lvnctx)
{
	switch (lvnctx->windowapi)
	{
		case Lvn_WindowApi_None:
		{
			LVN_CORE_TRACE("no window api selected, no window context to terminate");
			return;
		}
		case Lvn_WindowApi_glfw:
		{
			glfwImplTerminateWindowContext();
			break;
		}
		// case Lvn_WindowApi_win32:
		// {
		// 	break;
		// }
		default:
		{
			LVN_CORE_ERROR("unknown windows api selected, cannot terminate window context");
			return;
		}
	}

	LVN_CORE_TRACE("window context terminated: %s", getWindowApiNameEnum(lvnctx->windowapi));
}

static LvnResult setGraphicsContext(LvnContext* lvnctx, LvnGraphicsApi graphicsapi)
{
	LvnResult result = Lvn_Result_Failure;
	switch (graphicsapi)
	{
		case Lvn_GraphicsApi_None:
		{
			LVN_CORE_TRACE("no graphics context selected; graphics related function calls will not be used");
			return Lvn_Result_Success;
		}
		case Lvn_GraphicsApi_vulkan:
		{
		#if defined(LVN_GRAPHICS_API_INCLUDE_VULKAN)
			result = vksImplCreateContext(&lvnctx->graphicsContext);
		#endif
			break;
		}
		case Lvn_GraphicsApi_opengl:
		{
			result = oglsImplCreateContext(&lvnctx->graphicsContext);
			break;
		}
	}

	if (result != Lvn_Result_Success)
		LVN_CORE_ERROR("could not create graphics context for: %s", getGraphicsApiNameEnum(graphicsapi));
	else
		LVN_CORE_TRACE("graphics context set: %s", getGraphicsApiNameEnum(graphicsapi));

	return result;
}

static void terminateGraphicsContext(LvnContext* lvnctx)
{
	switch (lvnctx->graphicsapi)
	{
		case Lvn_GraphicsApi_None:
		{
			LVN_CORE_TRACE("no graphics api selected, no graphics context to terminate");
			return;
		}
		case Lvn_GraphicsApi_vulkan:
		{
		#if defined(LVN_GRAPHICS_API_INCLUDE_VULKAN)
			vksImplTerminateContext();
		#endif
			break;
		}
		case Lvn_GraphicsApi_opengl:
		{
			oglsImplTerminateContext();
			break;
		}
		default:
		{
			LVN_CORE_ERROR("unknown graphics api selected, cannot terminate graphics context");
		}
	}

	LVN_CORE_TRACE("graphics context terminated: %s", getGraphicsApiNameEnum(lvnctx->graphicsapi));
}

static LvnResult initAudioContext(LvnContext* lvnctx)
{
	ma_engine* pEngine = (ma_engine*)lvn::memAlloc(sizeof(ma_engine));

	if (ma_engine_init(nullptr, pEngine) != MA_SUCCESS)
	{
		LVN_CORE_ERROR("failed to initialize audio engine context");
		return Lvn_Result_Failure;
	}

	lvnctx->audioEngineContextPtr = pEngine;

	LVN_CORE_TRACE("audio context initialized");
	return Lvn_Result_Success;
}

static void terminateAudioContext(LvnContext* lvnctx)
{
	if (lvnctx->audioEngineContextPtr != nullptr)
	{
		ma_engine_uninit(static_cast<ma_engine*>(lvnctx->audioEngineContextPtr));
		lvn::memFree(lvnctx->audioEngineContextPtr);
	}

	LVN_CORE_TRACE("audio context terminated");
}

static LvnResult initNetworkingContext()
{
	if (enet_initialize() != 0)
	{
		LVN_CORE_ERROR("failed to initialize networking context");
		return Lvn_Result_Failure;
	}

	LVN_CORE_TRACE("networking context initialized");
	return Lvn_Result_Success;
}

static void terminateNetworkingContext()
{
	enet_deinitialize();
	LVN_CORE_TRACE("networking context terminated");
}

static void initStandardPipelineSpecification(LvnContext* lvnctx)
{
	LvnPipelineSpecification pipelineSpecification{};

	// Input Assembly
	pipelineSpecification.inputAssembly.topology = Lvn_TopologyType_Triangle;
	pipelineSpecification.inputAssembly.primitiveRestartEnable = false;

	// Viewport
	pipelineSpecification.viewport.x = 0.0f;
	pipelineSpecification.viewport.y = 0.0f;
	pipelineSpecification.viewport.width = 800.0f;
	pipelineSpecification.viewport.height = 600.0f;
	pipelineSpecification.viewport.minDepth = 0.0f;
	pipelineSpecification.viewport.maxDepth = 1.0f;

	// Scissor
	pipelineSpecification.scissor.offset = { 0, 0 };
	pipelineSpecification.scissor.extent = { 800, 600 };

	// Rasterizer
	pipelineSpecification.rasterizer.depthClampEnable = false;
	pipelineSpecification.rasterizer.rasterizerDiscardEnable = false;
	pipelineSpecification.rasterizer.lineWidth = 1.0f;
	pipelineSpecification.rasterizer.cullMode = Lvn_CullFaceMode_Disable;
	pipelineSpecification.rasterizer.frontFace = Lvn_CullFrontFace_Clockwise;
	pipelineSpecification.rasterizer.depthBiasEnable = false;
	pipelineSpecification.rasterizer.depthBiasConstantFactor = 0.0f;
	pipelineSpecification.rasterizer.depthBiasClamp = 0.0f;
	pipelineSpecification.rasterizer.depthBiasSlopeFactor = 0.0f;

	// MultiSampling
	pipelineSpecification.multisampling.sampleShadingEnable = false;
	pipelineSpecification.multisampling.rasterizationSamples = Lvn_SampleCount_1_Bit;
	pipelineSpecification.multisampling.minSampleShading = 1.0f;
	pipelineSpecification.multisampling.sampleMask = nullptr;
	pipelineSpecification.multisampling.alphaToCoverageEnable = false;
	pipelineSpecification.multisampling.alphaToOneEnable = false;

	// Color Attachments
	pipelineSpecification.colorBlend.colorBlendAttachmentCount = 0; // If no attachments are provided, an attachment will automatically be created
	pipelineSpecification.colorBlend.pColorBlendAttachments = nullptr; 

	// Color Blend
	pipelineSpecification.colorBlend.logicOpEnable = false;
	pipelineSpecification.colorBlend.blendConstants[0] = 0.0f;
	pipelineSpecification.colorBlend.blendConstants[1] = 0.0f;
	pipelineSpecification.colorBlend.blendConstants[2] = 0.0f;
	pipelineSpecification.colorBlend.blendConstants[3] = 0.0f;

	// Depth Stencil
	pipelineSpecification.depthstencil.enableDepth = false;
	pipelineSpecification.depthstencil.depthOpCompare = Lvn_CompareOperation_Never;
	pipelineSpecification.depthstencil.enableStencil = false;
	pipelineSpecification.depthstencil.stencil.compareMask = 0x00;
	pipelineSpecification.depthstencil.stencil.writeMask = 0x00;
	pipelineSpecification.depthstencil.stencil.reference = 0;
	pipelineSpecification.depthstencil.stencil.compareOp = Lvn_CompareOperation_Never;
	pipelineSpecification.depthstencil.stencil.depthFailOp = Lvn_StencilOperation_Keep;
	pipelineSpecification.depthstencil.stencil.failOp = Lvn_StencilOperation_Keep;
	pipelineSpecification.depthstencil.stencil.passOp = Lvn_StencilOperation_Keep;

	lvnctx->defaultPipelineSpecification = pipelineSpecification;
}

static void setDefaultStructTypeMemAllocInfos(LvnContext* lvnctx)
{
	auto& stInfos = lvnctx->sTypeMemAllocInfos;

	stInfos.resize(Lvn_Stype_Max);

	stInfos[Lvn_Stype_Undefined]        = { Lvn_Stype_Undefined, 0, 0 };
	stInfos[Lvn_Stype_Window]           = { Lvn_Stype_Window, sizeof(LvnWindow), 8 };
	stInfos[Lvn_Stype_Logger]           = { Lvn_Stype_Logger, sizeof(LvnLogger), 8 };
	stInfos[Lvn_Stype_FrameBuffer]      = { Lvn_Stype_FrameBuffer, sizeof(LvnFrameBuffer), 16 };
	stInfos[Lvn_Stype_Shader]           = { Lvn_Stype_Shader, sizeof(LvnShader), 32 };
	stInfos[Lvn_Stype_DescriptorLayout] = { Lvn_Stype_DescriptorLayout, sizeof(LvnDescriptorLayout), 64 };
	stInfos[Lvn_Stype_DescriptorSet]    = { Lvn_Stype_DescriptorSet, sizeof(LvnDescriptorSet), 64 };
	stInfos[Lvn_Stype_Pipeline]         = { Lvn_Stype_Pipeline, sizeof(LvnPipeline), 64 };
	stInfos[Lvn_Stype_Buffer]           = { Lvn_Stype_Buffer, sizeof(LvnBuffer), 256 };
	stInfos[Lvn_Stype_UniformBuffer]    = { Lvn_Stype_UniformBuffer, sizeof(LvnUniformBuffer), 64 };
	stInfos[Lvn_Stype_Texture]          = { Lvn_Stype_Texture, sizeof(LvnTexture), 256 };
	stInfos[Lvn_Stype_Cubemap]          = { Lvn_Stype_Cubemap, sizeof(LvnCubemap), 256 };
	stInfos[Lvn_Stype_Sound]            = { Lvn_Stype_Sound, sizeof(LvnSound), 256 };
	stInfos[Lvn_Stype_SoundBoard]       = { Lvn_Stype_SoundBoard, sizeof(LvnSoundBoard), 128 };
	stInfos[Lvn_Stype_Socket]           = { Lvn_Stype_Socket, sizeof(LvnSocket), 256 };
}

static const char* getStructTypeEnumStr(LvnStructureType stype)
{
	switch (stype)
	{
		case Lvn_Stype_Window:            { return "LvnWindow"; }
		case Lvn_Stype_Logger:            { return "LvnLogger"; }
		case Lvn_Stype_FrameBuffer:       { return "LvnFrameBuffer"; }
		case Lvn_Stype_Shader:            { return "LvnShader"; }
		case Lvn_Stype_DescriptorLayout:  { return "LvnDescriptorLayout"; }
		case Lvn_Stype_DescriptorSet:     { return "LvnDescriptorSet"; }
		case Lvn_Stype_Pipeline:          { return "LvnPipeline"; }
		case Lvn_Stype_Buffer:            { return "LvnBuffer"; }
		case Lvn_Stype_UniformBuffer:     { return "LvnUniformBuffer"; }
		case Lvn_Stype_Texture:           { return "LvnTexture"; }
		case Lvn_Stype_Cubemap:           { return "LvnCubemap"; }
		case Lvn_Stype_Sound:             { return "LvnSound"; }
		case Lvn_Stype_SoundBoard:        { return "LvnSoundBoard"; }
		case Lvn_Stype_Socket:            { return "LvnSocket"; }

		default:                          { return "undefined"; }
	}
}

static uint64_t getStructTypeSize(LvnStructureType sType)
{
	return lvn::getContext()->sTypeMemAllocInfos[sType].size;
}

static void setMemoryBlockBindings(LvnMemoryPool* memPool, uint32_t blockIndex, LvnStructureTypeInfo* pStructInfos, uint32_t structInfoCount)
{
	uint64_t memIndex = 0;

	for (uint32_t i = 0; i < structInfoCount; i++)
	{
		auto& memBinding = memPool->memBindings[pStructInfos[i].sType];

		LvnMemoryBinding* prevMemBinding = nullptr;
		if (!memBinding.empty())
			prevMemBinding = &memBinding.back();

		uint64_t count = pStructInfos[i].count;
		memBinding.push_back(LvnMemoryBinding(memPool->memBlocks[blockIndex][memIndex], pStructInfos[i].size, count));
		memIndex += count * pStructInfos[i].size;

		if (prevMemBinding != nullptr)
			prevMemBinding->set_next_memory_binding(&memBinding.back());
	}
}

static void createContextMemoryPool(LvnContext* lvnctx, LvnContextCreateInfo* createInfo)
{
	lvnctx->memoryMode = createInfo->memoryInfo.memAllocMode;
	if (lvnctx->memoryMode == Lvn_MemAllocMode_Individual) { return; }

	// set struct memory configs
	auto structTypes = lvnctx->sTypeMemAllocInfos;
	for (uint64_t i = 0; i < createInfo->memoryInfo.memoryBindingCount; i++)
		structTypes[createInfo->memoryInfo.memoryBindings[i].sType].count = createInfo->memoryInfo.memoryBindings[i].count;

	// get total memory in bytes for memory pool
	uint64_t memSize = 0;
	for (uint64_t i = 0; i < structTypes.size(); i++)
		memSize += lvn::getStructTypeSize(structTypes[i].sType) * structTypes[i].count;

	// create the first memory block
	LvnMemoryPool* memPool = &lvnctx->memoryPool;
	memPool->memBlocks.push_back(LvnMemoryBlock(memSize));

	memPool->memBindings.resize(Lvn_Stype_Max);
	lvn::setMemoryBlockBindings(memPool, 0, structTypes.data(), structTypes.size());


	// set struct block memory configs
	lvnctx->blockMemAllocInfos = lvnctx->sTypeMemAllocInfos;
	for (uint64_t i = 0; i < createInfo->memoryInfo.blockMemoryBindingCount; i++)
		lvnctx->blockMemAllocInfos[createInfo->memoryInfo.blockMemoryBindings[i].sType].count = createInfo->memoryInfo.blockMemoryBindings[i].count;

	// get total memory in bytes for memory pool
	lvnctx->blockMemSize = 0;
	for (uint64_t i = 0; i < lvnctx->blockMemAllocInfos.size(); i++)
		lvnctx->blockMemSize += lvnctx->blockMemAllocInfos[i].size * lvnctx->blockMemAllocInfos[i].count;


	LVN_CORE_TRACE("memory allocation mode set to memory pool, %u custom memory bindings created, total memory block size: %zu bytes", createInfo->memoryInfo.memoryBindingCount, memSize);
}

static void createBlockMemoryPool(LvnContext* lvnctx)
{
	// create the next memory block
	LvnMemoryPool* memPool = &lvnctx->memoryPool;
	memPool->memBlocks.push_back(LvnMemoryBlock(lvnctx->blockMemSize));

	lvn::setMemoryBlockBindings(memPool, memPool->memBlocks.size() - 1, lvnctx->blockMemAllocInfos.data(), lvnctx->blockMemAllocInfos.size());
}

template <typename T>
static T* createObject(LvnContext* lvnctx, LvnStructureType sType)
{
	T* object;
	if (lvnctx->memoryMode == Lvn_MemAllocMode_Individual)
	{
		object = new T();
	}
	else if (lvnctx->memoryMode == Lvn_MemAllocMode_MemPool)
	{
		auto& memBinding = lvnctx->memoryPool.memBindings[sType][0];
		if (memBinding.full() && memBinding.get_next_memory_binding() == nullptr)
			lvn::createBlockMemoryPool(lvnctx);

		object = new (static_cast<T*>(memBinding.take_next())) T();
	}
	else
	{
		LVN_CORE_ASSERT(false, "create object failed, no requirment was met before hand"); return nullptr;
	}

	lvnctx->objectMemoryAllocations.sTypes[sType].count++;
	return object;
}

template <typename T>
static void destroyObject(LvnContext* lvnctx, T* obj, LvnStructureType sType)
{
	if (lvnctx->memoryMode == Lvn_MemAllocMode_Individual)
	{
		delete obj;
		obj = nullptr;
	}
	else if (lvnctx->memoryMode == Lvn_MemAllocMode_MemPool)
	{
		lvnctx->memoryPool.memBindings[sType][0].push_back(obj);
	}
	else
	{
		LVN_CORE_ASSERT(false, "destroy object failed, no requirment was met before hand");
	}

	lvnctx->objectMemoryAllocations.sTypes[sType].count--;
}

// ------------------------------------------------------------
// [SECTION]: Core Functions
// ------------------------------------------------------------

LvnResult createContext(LvnContextCreateInfo* createInfo)
{
	if (s_LvnContext != nullptr) { return Lvn_Result_AlreadyCalled; }
	s_LvnContext = new LvnContext();

	s_LvnContext->contexTime.reset();

	s_LvnContext->appName = createInfo->applicationName;
	s_LvnContext->windowapi = createInfo->windowapi;
	s_LvnContext->graphicsapi = createInfo->graphicsapi;

	s_LvnContext->enableCoreLogging = !createInfo->logging.disableCoreLogging;

	s_LvnContext->graphicsContext.enableValidationLayers = createInfo->logging.enableVulkanValidationLayers;
	s_LvnContext->graphicsContext.frameBufferColorFormat = createInfo->frameBufferColorFormat;
	s_LvnContext->graphicsContext.maxFramesInFlight = createInfo->maxFramesInFlight;

	lvn::setDefaultStructTypeMemAllocInfos(s_LvnContext);

	// logging
	if (createInfo->logging.enableLogging) { logInit(); }

	// memory
	s_LvnContext->objectMemoryAllocations.sTypes.resize(Lvn_Stype_Max);
	for (uint32_t i = 0; i < s_LvnContext->objectMemoryAllocations.sTypes.size(); i++)
	{
		s_LvnContext->objectMemoryAllocations.sTypes[i] = { (LvnStructureType)i, 0 };
	}

	lvn::createContextMemoryPool(s_LvnContext, createInfo);

	// window context
	LvnResult result = setWindowContext(s_LvnContext, createInfo->windowapi);
	if (result != Lvn_Result_Success) { return result; }

	// graphics context
	result = setGraphicsContext(s_LvnContext, createInfo->graphicsapi);
	if (result != Lvn_Result_Success) { return result; }

	// audio context
	result = initAudioContext(s_LvnContext);
	if (result != Lvn_Result_Success) { return result; }

	// networking context
	result = initNetworkingContext();
	if (result != Lvn_Result_Success) { return result; }

	// config
	initStandardPipelineSpecification(s_LvnContext);

	if (createInfo->matrixClipRegion == Lvn_ClipRegion_ApiSpecific)
	{
		switch(createInfo->graphicsapi)
		{
			case Lvn_GraphicsApi_opengl:
			{
				s_LvnContext->matrixClipRegion = Lvn_ClipRegion_RHNO;
				break;
			}
			case Lvn_GraphicsApi_vulkan:
			{
				s_LvnContext->matrixClipRegion = Lvn_ClipRegion_LHZO;
				break;
			}

			default: { break; }
		}
	}
	else
	{
		s_LvnContext->matrixClipRegion = createInfo->matrixClipRegion;
	}

	return Lvn_Result_Success;
}

void terminateContext()
{
	if (s_LvnContext == nullptr) { return; }

	terminateWindowContext(s_LvnContext);
	terminateGraphicsContext(s_LvnContext);
	terminateAudioContext(s_LvnContext);
	terminateNetworkingContext();

	for (uint32_t i = 0; i < s_LvnContext->objectMemoryAllocations.sTypes.size(); i++)
	{
		if (s_LvnContext->objectMemoryAllocations.sTypes[i].count > 0)
		{
			const char* stype = lvn::getStructTypeEnumStr(s_LvnContext->objectMemoryAllocations.sTypes[i].sType);
			LVN_CORE_ERROR("sType = %s | not all objects of this sType (%s) have been destroyed, number of %s objects remaining: %zu", stype, stype, stype, s_LvnContext->objectMemoryAllocations.sTypes[i].count);
		}
	}

	if (s_LvnContext->numMemoryAllocations > 0) { LVN_CORE_WARN("not all memory allocations have been freed, number of allocations remaining: %zu", s_LvnContext->numMemoryAllocations); }

	delete s_LvnContext;
	s_LvnContext = nullptr;
}

LvnContext* getContext()
{
	LVN_CORE_ASSERT(s_LvnContext != nullptr, "levikno context is nullptr, context was probably not created or initiated before using the library")
	return s_LvnContext;
}

// ------------------------------------------------------------
// [SECTION]: Date Time Functions
// ------------------------------------------------------------

int dateGetYear()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	return tm.tm_year + 1900;
}
int dateGetYear02d()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	return (tm.tm_year + 1900) % 100;
}
int dateGetMonth()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	return tm.tm_mon + 1;
}
int dateGetDay()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	return tm.tm_mday;
}
int dateGetHour()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	return tm.tm_hour;
}
int dateGetHour12()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	return ((tm.tm_hour + 11) % 12) + 1;
}
int dateGetMinute()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	return tm.tm_min;
}
int dateGetSecond()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	return tm.tm_sec;
}

long long dateGetSecondsSinceEpoch()
{
	return time(NULL);
}

static const char* const s_MonthName[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
static const char* const s_MonthNameShort[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static const char* const s_WeekDayName[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
static const char* const s_WeekDayNameShort[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

const char* dateGetMonthName()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	return s_MonthName[tm.tm_mon];
}
const char* dateGetMonthNameShort()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	return s_MonthNameShort[tm.tm_mon];
}
const char* dateGetWeekDayName()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	return s_WeekDayName[tm.tm_wday];
}
const char* dateGetWeekDayNameShort()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	return s_WeekDayNameShort[tm.tm_wday];
}
std::string dateGetTimeHHMMSS()
{
	char buff[9];
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	snprintf(buff, 9, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
	return std::string(buff);
}
std::string dateGetTime12HHMMSS()
{
	char buff[9];
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	snprintf(buff, 9, "%02d:%02d:%02d", ((tm.tm_hour + 11) % 12) + 1, tm.tm_min, tm.tm_sec);
	return std::string(buff);
}
const char* dateGetTimeMeridiem()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	if (tm.tm_hour < 12)
		return "AM";
	else
		return "PM";
}
const char* dateGetTimeMeridiemLower()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	if (tm.tm_hour < 12)
		return "am";
	else
		return "pm";
}

std::string dateGetYearStr()
{
	char buff[5];
	snprintf(buff, 5, "%d", dateGetYear());
	return std::string(buff);
}
std::string dateGetYear02dStr()
{
	char buff[3];
	snprintf(buff, 3, "%d", dateGetYear02d());
	return std::string(buff);
}
std::string dateGetMonthNumStr()
{
	char buff[3];
	snprintf(buff, 3, "%02d", dateGetMonth());
	return std::string(buff);
}
std::string dateGetDayNumStr()
{
	char buff[3];
	snprintf(buff, 3, "%02d", dateGetDay());
	return std::string(buff);
}
std::string dateGetHourNumStr()
{
	char buff[3];
	snprintf(buff, 3, "%02d", dateGetHour());
	return std::string(buff);
}
std::string dateGetHour12NumStr()
{
	char buff[3];
	snprintf(buff, 3, "%02d", dateGetHour12());
	return std::string(buff);
}
std::string dateGetMinuteNumStr()
{
	char buff[3];
	snprintf(buff, 3, "%02d", dateGetMinute());
	return std::string(buff);
}
std::string dateGetSecondNumStr()
{
	char buff[3];
	snprintf(buff, 3, "%02d", dateGetSecond());
	return std::string(buff);
}


std::string loadFileSrc(const char* filepath)
{
	FILE* fileptr;
	fileptr = fopen(filepath, "r");
	
	fseek(fileptr, 0, SEEK_END);
	long int size = ftell(fileptr);
	fseek(fileptr, 0, SEEK_SET);
	
	std::vector<char> src(size);
	fread(src.data(), sizeof(char), size, fileptr);
	fclose(fileptr);

	return std::string(src.data(), src.data() + src.size());
}

float getContextTime()
{
	return lvn::getContext()->contexTime.elapsed();
}

LvnData<uint8_t> loadFileSrcBin(const char* filepath)
{
	FILE* fileptr;
	fileptr = fopen(filepath, "rb");

	fseek(fileptr, 0, SEEK_END);
	long int size = ftell(fileptr);
	fseek(fileptr, 0, SEEK_SET);

	std::vector<uint8_t> bin(size);
	fread(bin.data(), sizeof(uint8_t), size, fileptr);
	fclose(fileptr);

	return LvnData<uint8_t>(bin.data(), bin.size());
}

LvnFont loadFontFromFileTTF(const char* filepath, uint32_t fontSize, LvnCharset charset)
{
	LvnFont font{};

	LvnData<uint8_t> fontData = lvn::loadFileSrcBin(filepath);
	std::vector<uint8_t> fontBuffer(fontData.data(), fontData.data() + fontData.size());

	FT_Library ft;
	FT_Face face;

	if (FT_Init_FreeType(&ft))
	{
		LVN_CORE_ERROR("[freetype]: failed to load freetype library");
		LVN_CORE_ASSERT(false, "failed to load freetype");
		return font;
	}

	if (FT_New_Face(ft, filepath, 0, &face))
	{
		LVN_CORE_ERROR("[freetype]: failed to load font face!");
		LVN_CORE_ASSERT(false, "failed to load font face");
		return font;
	}

	FT_Set_Char_Size(face, 0, fontSize << 6, 96, 96);

	int maxDim = (1 + (face->size->metrics.height >> 6)) * ceilf(sqrtf(charset.last - charset.first));
	int width = 1;
	while (width < maxDim) width <<= 1;
	int height = width;

	// render glyphs to atlas
	std::vector<uint8_t> pixels(width * height);
	int penx = 0, peny = 0;

	std::vector<LvnFontGlyph> glyphs(charset.last - charset.first + 1);

	for (int8_t i = charset.first; i <= charset.last; i++)
	{
		FT_Load_Char(face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
		FT_Bitmap* bmp = &face->glyph->bitmap;

		if (penx + bmp->width >= (uint32_t)width)
		{
			penx = 0;
			peny += ((face->size->metrics.height >> 6) + 1);
		}

		for (uint32_t row = 0; row < bmp->rows; row++)
		{
			for (uint32_t col = 0; col < bmp->width; col++)
			{
				int x = penx + col;
				int y = peny + row;
				pixels[y * width + x] = bmp->buffer[row * bmp->pitch + col];
			}
		}


		LvnFontGlyph glyph{};
		glyph.uv.x0 = (float)penx / (float)width;
		glyph.uv.y0 = (float)peny / (float)height;
		glyph.uv.x1 = (float)(penx + bmp->width) / (float)width;
		glyph.uv.y1 = (float)(peny + bmp->rows) / (float)height;

		glyph.size.x = bmp->width;
		glyph.size.y = bmp->rows;
		glyph.bearing.x = face->glyph->bitmap_left;
		glyph.bearing.y = face->glyph->bitmap_top;
		glyph.advance = face->glyph->advance.x >> 6;

		glyphs[i - charset.first] = glyph;

		penx += bmp->width + 1;
	}

	FT_Done_FreeType(ft);
	
	LvnImageData atlas{};
	atlas.width = width;
	atlas.height = height;
	atlas.channels = 1;
	atlas.size = width * height;
	atlas.pixels = LvnData<uint8_t>(pixels.data(), pixels.size());

	font.atlas = atlas;
	font.glyphs = LvnData<LvnFontGlyph>(glyphs.data(), glyphs.size());
	font.codepoints = charset;
	font.fontSize = fontSize;

	return font;
}

LvnFontGlyph fontGetGlyph(LvnFont* font, int8_t codepoint)
{
	LVN_CORE_ASSERT(codepoint >= font->codepoints.first && codepoint <= font->codepoints.last, "codepoint out of charset range");
	return font->glyphs[codepoint - font->codepoints.first];
}

void* memAlloc(size_t size)
{
	if (size == 0) { return nullptr; }
	void* allocmem = calloc(1, size);
	if (!allocmem) { LVN_CORE_ERROR("malloc failure, could not allocate memory!"); LVN_ABORT; }
	if (s_LvnContext) { s_LvnContext->numMemoryAllocations++; }
	return allocmem;
}

void memFree(void* ptr)
{
	if (ptr == nullptr) { return; }
	free(ptr);
	ptr = nullptr;
	if (s_LvnContext) s_LvnContext->numMemoryAllocations--;
}

void* memRealloc(void* ptr, size_t size)
{
	void* allocmem = realloc(ptr, size);
	if (!allocmem) { LVN_CORE_ERROR("malloc failure, could not allocate memory!"); LVN_ABORT; }
	return allocmem;
}

/* [Logging] */
const static LvnLogPattern s_LogPatterns[] =
{
	{ '$', [](LvnLogMessage* msg) -> std::string { return "\n"; } },
	{ 'n', [](LvnLogMessage* msg) -> std::string { return msg->loggerName; } },
	{ 'l', [](LvnLogMessage* msg) -> std::string { return getLogLevelName(msg->level); }},
	{ '#', [](LvnLogMessage* msg) -> std::string { return getLogLevelColor(msg->level); }},
	{ '^', [](LvnLogMessage* msg) -> std::string { return LVN_LOG_COLOR_RESET; }},
	{ 'v', [](LvnLogMessage* msg) -> std::string { return msg->msg; }},
	{ '%', [](LvnLogMessage* msg) -> std::string { return "%"; } },
	{ 'T', [](LvnLogMessage* msg) -> std::string { return dateGetTimeHHMMSS(); } },
	{ 't', [](LvnLogMessage* msg) -> std::string { return dateGetTime12HHMMSS(); } },
	{ 'Y', [](LvnLogMessage* msg) -> std::string { return dateGetYearStr(); }},
	{ 'y', [](LvnLogMessage* msg) -> std::string { return dateGetYear02dStr(); } },
	{ 'm', [](LvnLogMessage* msg) -> std::string { return dateGetMonthNumStr(); } },
	{ 'B', [](LvnLogMessage* msg) -> std::string { return dateGetMonthName(); } },
	{ 'b', [](LvnLogMessage* msg) -> std::string { return dateGetMonthNameShort(); } },
	{ 'd', [](LvnLogMessage* msg) -> std::string { return dateGetDayNumStr(); } },
	{ 'A', [](LvnLogMessage* msg) -> std::string { return dateGetWeekDayName(); } },
	{ 'a', [](LvnLogMessage* msg) -> std::string { return dateGetWeekDayNameShort(); } },
	{ 'H', [](LvnLogMessage* msg) -> std::string { return dateGetHourNumStr(); } },
	{ 'h', [](LvnLogMessage* msg) -> std::string { return dateGetHour12NumStr(); } },
	{ 'M', [](LvnLogMessage* msg) -> std::string { return dateGetMinuteNumStr(); } },
	{ 'S', [](LvnLogMessage* msg) -> std::string { return dateGetSecondNumStr(); } },
	{ 'P', [](LvnLogMessage* msg) -> std::string { return dateGetTimeMeridiem(); } },
	{ 'p', [](LvnLogMessage* msg) -> std::string { return dateGetTimeMeridiemLower(); }},
};

static std::vector<LvnLogPattern> logParseFormat(const char* fmt)
{
	if (!fmt || fmt[0] == '\0') { return {}; }

	std::vector<LvnLogPattern> patterns;

	for (uint32_t i = 0; i < strlen(fmt) - 1; i++)
	{
		if (fmt[i] != '%') // Other characters in format
		{
			LvnLogPattern pattern = { .symbol = fmt[i], .func = nullptr };
			patterns.push_back(pattern);
			continue;
		}

		// find pattern with matching symbol
		for (uint32_t j = 0; j < sizeof(s_LogPatterns) / sizeof(LvnLogPattern); j++)
		{
			if (fmt[i + 1] != s_LogPatterns[j].symbol)
				continue;

			patterns.push_back(s_LogPatterns[j]);
		}

		// find and add user defined patterns
		for (uint32_t j = 0; j < s_LvnContext->userLogPatterns.size(); j++)
		{
			if (fmt[i + 1] != s_LvnContext->userLogPatterns[j].symbol)
				continue;

			patterns.push_back(s_LvnContext->userLogPatterns[j]);
		}

		i++; // incramant past symbol on next character in format
	}

	return patterns;
}

LvnResult logInit()
{
	LvnContext* lvnctx = lvn::getContext();

	if (!lvnctx->logging)
	{
		lvnctx->logging = true;

		lvnctx->coreLogger.loggerName = "CORE";

		if (!lvnctx->appName.empty())
			lvnctx->clientLogger.loggerName = lvnctx->appName;
		else
			lvnctx->clientLogger.loggerName = "CLIENT";

		lvnctx->coreLogger.logLevel = lvnctx->clientLogger.logLevel = Lvn_LogLevel_None;
		lvnctx->coreLogger.logPatternFormat = lvnctx->clientLogger.logPatternFormat = LVN_DEFAULT_LOG_PATTERN;
		lvnctx->coreLogger.logPatterns = lvnctx->clientLogger.logPatterns = lvn::logParseFormat(LVN_DEFAULT_LOG_PATTERN);


		#ifdef LVN_PLATFORM_WINDOWS 
		enableLogANSIcodeColors();
		#endif

		return Lvn_Result_Success;
	}

	return Lvn_Result_AlreadyCalled;
}

void logEnable(bool enable)
{
	lvn::getContext()->logging = enable;
}

void logEnableCoreLogging(bool enable)
{
	lvn::getContext()->enableCoreLogging = enable;
}

void logSetLevel(LvnLogger* logger, LvnLogLevel level)
{
	logger->logLevel = level;
}

bool logCheckLevel(LvnLogger* logger, LvnLogLevel level)
{
	return (level >= logger->logLevel);
}

void logRenameLogger(LvnLogger* logger, const char* name)
{
	logger->loggerName = name;
}

void logOutputMessage(LvnLogger* logger, LvnLogMessage* msg)
{
	if (!lvn::getContext()->logging) { return; }

	std::string msgstr;

	for (uint32_t i = 0; i < logger->logPatterns.size(); i++)
	{
		if (logger->logPatterns[i].func == nullptr) // no special format character '%' found
		{
			msgstr += logger->logPatterns[i].symbol;
		}
		else // call func of special format
		{
			msgstr += logger->logPatterns[i].func(msg);
		}
	}

	printf("%s", msgstr.c_str());
}

void logMessage(LvnLogger* logger, LvnLogLevel level, const char* msg)
{
	if (!lvn::getContext()->logging) { return; }

	LvnLogMessage logMsg{};
	logMsg.msg = msg;
	logMsg.loggerName = logger->loggerName.c_str();
	logMsg.level = level;
	logMsg.timeEpoch = dateGetSecondsSinceEpoch();

	logOutputMessage(logger, &logMsg);
}

void logMessageTrace(LvnLogger* logger, const char* fmt, ...)
{
	if (!s_LvnContext || !s_LvnContext->logging) { return; }
	if (!s_LvnContext->enableCoreLogging && logger == &s_LvnContext->coreLogger) { return; }
	if (!logCheckLevel(logger, Lvn_LogLevel_Trace)) { return; }

	std::vector<char> buff;

	va_list argptr, argcopy;
	va_start(argptr, fmt);
	va_copy(argcopy, argptr);

	int len = vsnprintf(nullptr, 0, fmt, argptr);
	buff.resize(len + 1);
	vsnprintf(&buff[0], len + 1, fmt, argcopy);
	logMessage(logger, Lvn_LogLevel_Trace, buff.data());

	va_end(argcopy);
	va_end(argptr);
}

void logMessageDebug(LvnLogger* logger, const char* fmt, ...)
{
	if (!s_LvnContext || !s_LvnContext->logging) { return; }
	if (!s_LvnContext->enableCoreLogging && logger == &s_LvnContext->coreLogger) { return; }
	if (!logCheckLevel(logger, Lvn_LogLevel_Debug)) { return; }

	std::vector<char> buff;

	va_list argptr, argcopy;
	va_start(argptr, fmt);
	va_copy(argcopy, argptr);

	int len = vsnprintf(nullptr, 0, fmt, argptr);
	buff.resize(len + 1);
	vsnprintf(&buff[0], len + 1, fmt, argcopy);
	logMessage(logger, Lvn_LogLevel_Debug, buff.data());

	va_end(argcopy);
	va_end(argptr);
}

void logMessageInfo(LvnLogger* logger, const char* fmt, ...)
{
	if (!s_LvnContext || !s_LvnContext->logging) { return; }
	if (!s_LvnContext->enableCoreLogging && logger == &s_LvnContext->coreLogger) { return; }
	if (!logCheckLevel(logger, Lvn_LogLevel_Info)) { return; }

	std::vector<char> buff;

	va_list argptr, argcopy;
	va_start(argptr, fmt);
	va_copy(argcopy, argptr);

	int len = vsnprintf(nullptr, 0, fmt, argptr);
	buff.resize(len + 1);
	vsnprintf(&buff[0], len + 1, fmt, argcopy);
	logMessage(logger, Lvn_LogLevel_Info, buff.data());

	va_end(argcopy);
	va_end(argptr);
}

void logMessageWarn(LvnLogger* logger, const char* fmt, ...)
{
	if (!s_LvnContext || !s_LvnContext->logging) { return; }
	if (!s_LvnContext->enableCoreLogging && logger == &s_LvnContext->coreLogger) { return; }
	if (!logCheckLevel(logger, Lvn_LogLevel_Warn)) { return; }

	std::vector<char> buff;

	va_list argptr, argcopy;
	va_start(argptr, fmt);
	va_copy(argcopy, argptr);

	int len = vsnprintf(nullptr, 0, fmt, argptr);
	buff.resize(len + 1);
	vsnprintf(&buff[0], len + 1, fmt, argcopy);
	logMessage(logger, Lvn_LogLevel_Warn, buff.data());

	va_end(argcopy);
	va_end(argptr);
}

void logMessageError(LvnLogger* logger, const char* fmt, ...)
{
	if (!s_LvnContext || !s_LvnContext->logging) { return; }
	if (!s_LvnContext->enableCoreLogging && logger == &s_LvnContext->coreLogger) { return; }
	if (!logCheckLevel(logger, Lvn_LogLevel_Error)) { return; }

	std::vector<char> buff;

	va_list argptr, argcopy;
	va_start(argptr, fmt);
	va_copy(argcopy, argptr);

	int len = vsnprintf(nullptr, 0, fmt, argptr);
	buff.resize(len + 1);
	vsnprintf(&buff[0], len + 1, fmt, argcopy);
	logMessage(logger, Lvn_LogLevel_Error, buff.data());

	va_end(argcopy);
	va_end(argptr);
}

void logMessageFatal(LvnLogger* logger, const char* fmt, ...)
{
	if (!s_LvnContext || !s_LvnContext->logging) { return; }
	if (!s_LvnContext->enableCoreLogging && logger == &s_LvnContext->coreLogger) { return; }
	if (!logCheckLevel(logger, Lvn_LogLevel_Fatal)) { return; }

	std::vector<char> buff;

	va_list argptr, argcopy;
	va_start(argptr, fmt);
	va_copy(argcopy, argptr);

	int len = vsnprintf(nullptr, 0, fmt, argptr);
	buff.resize(len + 1);
	vsnprintf(&buff[0], len + 1, fmt, argcopy);
	logMessage(logger, Lvn_LogLevel_Fatal, buff.data());

	va_end(argcopy);
	va_end(argptr);
}

LvnLogger* logGetCoreLogger()
{
	return &lvn::getContext()->coreLogger;
}

LvnLogger* logGetClientLogger()
{
	return &lvn::getContext()->clientLogger;
}

const char* logGetANSIcodeColor(LvnLogLevel level)
{
	switch (level)
	{
		case Lvn_LogLevel_None:     { return LVN_LOG_COLOR_RESET; }
		case Lvn_LogLevel_Trace:    { return LVN_LOG_COLOR_TRACE; }
		case Lvn_LogLevel_Debug:    { return LVN_LOG_COLOR_DEBUG; }
		case Lvn_LogLevel_Info:     { return LVN_LOG_COLOR_INFO; }
		case Lvn_LogLevel_Warn:     { return LVN_LOG_COLOR_WARN; }
		case Lvn_LogLevel_Error:    { return LVN_LOG_COLOR_ERROR; }
		case Lvn_LogLevel_Fatal:    { return LVN_LOG_COLOR_FATAL; }
	}

	return nullptr;
}

LvnResult logSetPatternFormat(LvnLogger* logger, const char* patternfmt)
{
	if (!logger) { return Lvn_Result_Failure; }
	if (!patternfmt || patternfmt[0] == '\0') { return Lvn_Result_Failure; }

	logger->logPatternFormat = patternfmt;

	logger->logPatterns = lvn::logParseFormat(patternfmt);

	return Lvn_Result_Success;
}

LvnResult logAddPatterns(LvnLogPattern* pLogPatterns, uint32_t count)
{
	if (!pLogPatterns) { return Lvn_Result_Failure; }
	if (pLogPatterns->symbol == '\0') { return Lvn_Result_Failure; }

	for (uint32_t i = 0; i < sizeof(s_LogPatterns) / sizeof(LvnLogPattern); i++)
	{
		for (uint32_t j = 0; j < count; j++)
		{
			if (pLogPatterns[j].symbol == s_LogPatterns[i].symbol) { return Lvn_Result_Failure; }
		}
	}

	LvnContext* lvnctx = lvn::getContext();
	lvnctx->userLogPatterns.insert(lvnctx->userLogPatterns.end(), pLogPatterns, pLogPatterns + count);

	return Lvn_Result_Success;
}

LvnResult createLogger(LvnLogger** logger, LvnLoggerCreateInfo* loggerCreateInfo)
{
	LvnContext* lvnctx = lvn::getContext();

	*logger = lvn::createObject<LvnLogger>(lvnctx, Lvn_Stype_Logger);
	LvnLogger* loggerPtr = *logger;

	loggerPtr->loggerName = loggerCreateInfo->loggerName;
	loggerPtr->logPatternFormat = loggerCreateInfo->logPatternFormat;
	loggerPtr->logLevel = loggerCreateInfo->logLevel;

	loggerPtr->logPatterns = lvn::logParseFormat(loggerCreateInfo->logPatternFormat.c_str());

	LVN_CORE_TRACE("created logger: (%p), name: \"%s\"", *logger, loggerCreateInfo->loggerName.c_str());
	return Lvn_Result_Success;
}

void destroyLogger(LvnLogger* logger)
{
	if (logger == nullptr) { return; }
	LvnContext* lvnctx = lvn::getContext();
	lvn::destroyObject(lvnctx, logger, Lvn_Stype_Logger);
}

// ------------------------------------------------------------
// [SECTION]: Event Functions
// ------------------------------------------------------------

bool dispatchKeyHoldEvent(LvnEvent* event, bool(*func)(LvnKeyHoldEvent*, void*))
{
	if (event->type == Lvn_EventType_KeyHold)
	{
		LvnKeyHoldEvent eventType{};
		eventType.type = Lvn_EventType_KeyHold;
		eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
		eventType.name = "LvnKeyHoldEvent";
		eventType.handled = false;
		eventType.keyCode = event->data.code;
		eventType.repeat = event->data.repeat;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchKeyPressedEvent(LvnEvent* event, bool(*func)(LvnKeyPressedEvent*, void*))
{
	if (event->type == Lvn_EventType_KeyPressed)
	{
		LvnKeyPressedEvent eventType{};
		eventType.type = Lvn_EventType_KeyPressed;
		eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
		eventType.name = "LvnKeyPressedEvent";
		eventType.handled = false;
		eventType.keyCode = event->data.code;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchKeyReleasedEvent(LvnEvent* event, bool(*func)(LvnKeyReleasedEvent*, void*))
{
	if (event->type == Lvn_EventType_KeyReleased)
	{
		LvnKeyReleasedEvent eventType{};
		eventType.type = Lvn_EventType_KeyReleased;
		eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
		eventType.name = "LvnKeyReleasedEvent";
		eventType.handled = false;
		eventType.keyCode = event->data.code;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchKeyTypedEvent(LvnEvent* event, bool(*func)(LvnKeyTypedEvent*, void*))
{
	if (event->type == Lvn_EventType_KeyTyped)
	{
		LvnKeyTypedEvent eventType{};
		eventType.type = Lvn_EventType_KeyTyped;
		eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
		eventType.name = "LvnKeyTypedEvent";
		eventType.handled = false;
		eventType.key = event->data.ucode;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchMouseButtonPressedEvent(LvnEvent* event, bool(*func)(LvnMouseButtonPressedEvent*, void*))
{
	if (event->type == Lvn_EventType_MouseButtonPressed)
	{
		LvnMouseButtonPressedEvent eventType{};
		eventType.type = Lvn_EventType_MouseButtonPressed;
		eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_MouseButton | Lvn_EventCategory_Mouse;
		eventType.name = "LvnMouseButtonPressedEvent";
		eventType.handled = false;
		eventType.buttonCode = event->data.code;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchMouseButtonReleasedEvent(LvnEvent* event, bool(*func)(LvnMouseButtonReleasedEvent*, void*))
{
	if (event->type == Lvn_EventType_MouseButtonReleased)
	{
		LvnMouseButtonReleasedEvent eventType{};
		eventType.type = Lvn_EventType_MouseButtonReleased;
		eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_MouseButton | Lvn_EventCategory_Mouse;
		eventType.name = "LvnMouseButtonReleasedEvent";
		eventType.handled = false;
		eventType.buttonCode = event->data.code;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchMouseMovedEvent(LvnEvent* event, bool(*func)(LvnMouseMovedEvent*, void*))
{
	if (event->type == Lvn_EventType_MouseMoved)
	{
		LvnMouseMovedEvent eventType{};
		eventType.type = Lvn_EventType_MouseMoved;
		eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_Mouse;
		eventType.name = "LvnMouseMovedEvent";
		eventType.handled = false;
		eventType.x = event->data.xd;
		eventType.y = event->data.yd;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchMouseScrolledEvent(LvnEvent* event, bool(*func)(LvnMouseScrolledEvent*, void*))
{
	if (event->type == Lvn_EventType_MouseScrolled)
	{
		LvnMouseScrolledEvent eventType{};
		eventType.type = Lvn_EventType_MouseScrolled;
		eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_MouseButton | Lvn_EventCategory_Mouse;
		eventType.name = "LvnMouseScrolledEvent";
		eventType.handled = false;
		eventType.x = static_cast<float>(event->data.xd);
		eventType.y = static_cast<float>(event->data.yd);

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchWindowCloseEvent(LvnEvent* event, bool(*func)(LvnWindowCloseEvent*, void*))
{
	if (event->type == Lvn_EventType_WindowClose)
	{
		LvnWindowCloseEvent eventType{};
		eventType.type = Lvn_EventType_WindowClose;
		eventType.category = Lvn_EventCategory_Window;
		eventType.name = "LvnWindowCloseEvent";
		eventType.handled = false;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchWindowFocusEvent(LvnEvent* event, bool(*func)(LvnWindowFocusEvent*, void*))
{
	if (event->type == Lvn_EventType_WindowFocus)
	{
		LvnWindowFocusEvent eventType{};
		eventType.type = Lvn_EventType_WindowFocus;
		eventType.category = Lvn_EventCategory_Window;
		eventType.name = "LvnWindowFocusEvent";
		eventType.handled = false;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchWindowFramebufferResizeEvent(LvnEvent* event, bool(*func)(LvnWindowFramebufferResizeEvent*, void*))
{
	if (event->type == Lvn_EventType_WindowFramebufferResize)
	{
		LvnWindowFramebufferResizeEvent eventType{};
		eventType.type = Lvn_EventType_WindowFramebufferResize;
		eventType.category = Lvn_EventCategory_Window;
		eventType.name = "LvnWindowFramebufferResizeEvent";
		eventType.handled = false;
		eventType.width = event->data.x;
		eventType.height = event->data.y;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchWindowLostFocusEvent(LvnEvent* event, bool(*func)(LvnWindowLostFocusEvent*, void*))
{
	if (event->type == Lvn_EventType_WindowLostFocus)
	{
		LvnWindowLostFocusEvent eventType{};
		eventType.type = Lvn_EventType_WindowLostFocus;
		eventType.category = Lvn_EventCategory_Window;
		eventType.name = "LvnWindowLostFocusEvent";
		eventType.handled = false;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchWindowMovedEvent(LvnEvent* event, bool(*func)(LvnWindowMovedEvent*, void*))
{
	if (event->type == Lvn_EventType_WindowMoved)
	{
		LvnWindowMovedEvent eventType{};
		eventType.type = Lvn_EventType_WindowMoved;
		eventType.category = Lvn_EventCategory_Window;
		eventType.name = "LvnWindowMovedEvent";
		eventType.handled = false;
		eventType.x = event->data.x;
		eventType.y = event->data.y;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchWindowResizeEvent(LvnEvent* event, bool(*func)(LvnWindowResizeEvent*, void*))
{

	if (event->type == Lvn_EventType_WindowResize)
	{
		LvnWindowResizeEvent eventType{};
		eventType.type = Lvn_EventType_WindowResize;
		eventType.category = Lvn_EventCategory_Window;
		eventType.name = "LvnWindowResizeEvent";
		eventType.handled = false;
		eventType.width = event->data.x;
		eventType.height = event->data.y;

		return func(&eventType, event->userData);
	}

	return false;
}


// ------------------------------------------------------------
// [SECTION]: Window Functions
// ------------------------------------------------------------

LvnWindowApi getWindowApi()
{
	return lvn::getContext()->windowapi;
}

const char* getWindowApiName()
{
	switch (lvn::getContext()->windowapi)
	{
		case Lvn_WindowApi_None:  { return "None";  }
		case Lvn_WindowApi_glfw:  { return "glfw";  }
		// case Lvn_WindowApi_win32: { return "win32"; }
	}

	LVN_CORE_ERROR("Unknown Windows API selected!");
	return LVN_EMPTY_STR;
}

LvnResult createWindow(LvnWindow** window, LvnWindowCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

	if (createInfo->width < 0 || createInfo->height < 0)
	{
		LVN_CORE_ERROR("createWindow(LvnWindow**, LvnWindowCreateInfo*) | cannot create window with negative dimensions (w:%d,h:%d)", createInfo->width, createInfo->height);
		return Lvn_Result_Failure;
	}

	*window = lvn::createObject<LvnWindow>(lvnctx, Lvn_Stype_Window);

	LVN_CORE_TRACE("created window: (%p), \"%s\" (w:%d,h:%d)", *window, createInfo->title, createInfo->width, createInfo->height);
	return lvnctx->windowContext.createWindow(*window, createInfo);
}

void destroyWindow(LvnWindow* window)
{
	if (window == nullptr) { return; }
	LvnContext* lvnctx = lvn::getContext();
	lvnctx->windowContext.destroyWindow(window);
	lvn::destroyObject(lvnctx, window, Lvn_Stype_Window);
}

void windowUpdate(LvnWindow* window)
{
	lvn::getContext()->windowContext.updateWindow(window);
}

bool windowOpen(LvnWindow* window)
{
	return lvn::getContext()->windowContext.windowOpen(window);
}

LvnPair<int> windowGetDimensions(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getWindowSize(window);
}

int windowGetWidth(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getWindowWidth(window);
}

int windowGetHeight(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getWindowHeight(window);
}

void windowSetEventCallback(LvnWindow* window, void (*callback)(LvnEvent*), void* userData)
{
	window->data.eventCallBackFn = callback;
	window->data.userData = userData;
}

void windowSetVSync(LvnWindow* window, bool enable)
{
	lvn::getContext()->windowContext.setWindowVSync(window, enable);
}

bool windowGetVSync(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getWindowVSync(window);
}

void* windowGetNativeWindow(LvnWindow* window)
{
	return window->nativeWindow;
}

LvnRenderPass* windowGetRenderPass(LvnWindow* window)
{
	return &window->renderPass;
}

void windowSetContextCurrent(LvnWindow* window)
{
	lvn::getContext()->windowContext.setWindowContextCurrent(window);
}

LvnWindowCreateInfo windowCreateInfoGetConfig(int width, int height, const char* title)
{
	LvnWindowCreateInfo windowCreateInfo{};
	windowCreateInfo.width = width;
	windowCreateInfo.height = height;
	windowCreateInfo.title = title;

	 return windowCreateInfo;
}

// ------------------------------------------------------------
// [SECTION]: Input Functions
// ------------------------------------------------------------

bool keyPressed(LvnWindow* window, int keycode)
{
	return lvn::getContext()->windowContext.keyPressed(window, keycode);
}

bool keyReleased(LvnWindow* window, int keycode)
{
	return lvn::getContext()->windowContext.keyReleased(window, keycode);
}

bool mouseButtonPressed(LvnWindow* window, int button)
{
	return lvn::getContext()->windowContext.mouseButtonPressed(window, button);
}

bool mouseButtonReleased(LvnWindow* window, int button)
{
	return lvn::getContext()->windowContext.mouseButtonReleased(window, button);
}

LvnPair<float> mouseGetPos(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getMousePos(window);
}

void mouseGetPos(LvnWindow* window, float* xpos, float* ypos)
{
	lvn::getContext()->windowContext.getMousePosPtr(window, xpos, ypos);
}

float mouseGetX(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getMouseX(window);
}

float mouseGetY(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getMouseY(window);
}

void mouseSetCursor(LvnWindow* window, LvnMouseCursor cursor)
{
	lvn::getContext()->windowContext.setMouseCursor(window, cursor);
}

void mouseSetInputMode(LvnWindow* window, LvnMouseInputMode mode)
{
	lvn::getContext()->windowContext.SetMouseInputMode(window, mode);
}

LvnPair<int> windowGetPos(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getWindowPos(window);
}

void windowGetPos(LvnWindow* window, int* xpos, int* ypos)
{
	lvn::getContext()->windowContext.getWindowPosPtr(window, xpos, ypos);
}

LvnPair<int> windowGetSize(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getWindowSize(window);
}

void windowGetSize(LvnWindow* window, int* width, int* height)
{
	lvn::getContext()->windowContext.getWindowSizePtr(window, width, height);
}

// ------------------------------------------------------------
// [SECTION]: Graphics Functions
// ------------------------------------------------------------

LvnGraphicsApi getGraphicsApi()
{
	return lvn::getContext()->graphicsapi;
}

const char* getGraphicsApiName()
{
	switch (lvn::getContext()->graphicsapi)
	{
		case Lvn_GraphicsApi_None:   { return "None";   }
		case Lvn_GraphicsApi_vulkan: { return "vulkan"; }
		case Lvn_GraphicsApi_opengl: { return "opengl"; }
	}

	LVN_CORE_ERROR("Unknown Graphics API selected!");
	return LVN_EMPTY_STR;
}

void getPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* deviceCount)
{
	uint32_t getDeviceCount;
	lvn::getContext()->graphicsContext.getPhysicalDevices(nullptr, &getDeviceCount);

	if (pPhysicalDevices == nullptr)
	{
		*deviceCount = getDeviceCount;
		return;
	}

	lvn::getContext()->graphicsContext.getPhysicalDevices(pPhysicalDevices, &getDeviceCount);

	return;
}

LvnPhysicalDeviceInfo getPhysicalDeviceInfo(LvnPhysicalDevice* physicalDevice)
{
	return physicalDevice->info;
}

LvnResult checkPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice)
{
	return lvn::getContext()->graphicsContext.checkPhysicalDeviceSupport(physicalDevice);
}

LvnResult renderInit(LvnRenderInitInfo* renderInfo)
{
	return lvn::getContext()->graphicsContext.renderInit(renderInfo);
}

LvnClipRegion getRenderClipRegionEnum()
{
	return lvn::getContext()->matrixClipRegion;
}

void renderClearColor(LvnWindow* window, float r, float g, float b, float a)
{
	lvn::getContext()->graphicsContext.renderClearColor(window, r, g, b, a);
}

void renderCmdDraw(LvnWindow* window, uint32_t vertexCount)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdDraw(window, vertexCount);
}

void renderCmdDrawIndexed(LvnWindow* window, uint32_t indexCount)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdDrawIndexed(window, indexCount);
}

void renderCmdDrawInstanced(LvnWindow* window, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdDrawInstanced(window, vertexCount, instanceCount, firstInstance);
}

void renderCmdDrawIndexedInstanced(LvnWindow* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdDrawIndexedInstanced(window, indexCount, instanceCount, firstInstance);
}

void renderCmdSetStencilReference(uint32_t reference)
{

}

void renderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask)
{

}

void renderBeginNextFrame(LvnWindow* window)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderBeginNextFrame(window);
}

void renderDrawSubmit(LvnWindow* window)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderDrawSubmit(window);
}

void renderBeginCommandRecording(LvnWindow* window)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderBeginCommandRecording(window);
}

void renderEndCommandRecording(LvnWindow* window)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderEndCommandRecording(window);
}

void renderCmdBeginRenderPass(LvnWindow* window)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdBeginRenderPass(window);
}

void renderCmdEndRenderPass(LvnWindow* window)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdEndRenderPass(window);
}

void renderCmdBindPipeline(LvnWindow* window, LvnPipeline* pipeline)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdBindPipeline(window, pipeline);
}

void renderCmdBindVertexBuffer(LvnWindow* window, LvnBuffer* buffer)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdBindVertexBuffer(window, buffer);
}

void renderCmdBindIndexBuffer(LvnWindow* window, LvnBuffer* buffer)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdBindIndexBuffer(window, buffer);
}

void renderCmdBindDescriptorSets(LvnWindow* window, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdBindDescriptorSets(window, pipeline, firstSetIndex, descriptorSetCount, pDescriptorSets);
}

void renderCmdBeginFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdBeginFrameBuffer(window, frameBuffer);
}

void renderCmdEndFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdEndFrameBuffer(window, frameBuffer);
}

LvnResult createShaderFromSrc(LvnShader** shader, LvnShaderCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

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

	*shader = lvn::createObject<LvnShader>(lvnctx, Lvn_Stype_Shader);

	LVN_CORE_TRACE("created shader (from source): (%p)", *shader);
	return lvnctx->graphicsContext.createShaderFromSrc(*shader, createInfo);
}

LvnResult createShaderFromFileSrc(LvnShader** shader, LvnShaderCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

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

	*shader = lvn::createObject<LvnShader>(lvnctx, Lvn_Stype_Shader);

	LVN_CORE_TRACE("created shader (from source file): (%p), vertex file: %s, fragment file: %s", *shader, createInfo->vertexSrc.c_str(), createInfo->fragmentSrc.c_str());
	return lvnctx->graphicsContext.createShaderFromFileSrc(*shader, createInfo);
}

LvnResult createShaderFromFileBin(LvnShader** shader, LvnShaderCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

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

	*shader = lvn::createObject<LvnShader>(lvnctx, Lvn_Stype_Shader);

	LVN_CORE_TRACE("created shader (from binary file): (%p), vertex file: %s, fragment file: %s", *shader, createInfo->vertexSrc.c_str(), createInfo->fragmentSrc.c_str());
	return lvnctx->graphicsContext.createShaderFromFileBin(*shader, createInfo);
}

LvnResult createDescriptorLayout(LvnDescriptorLayout** descriptorLayout, LvnDescriptorLayoutCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

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

	*descriptorLayout = lvn::createObject<LvnDescriptorLayout>(lvnctx, Lvn_Stype_DescriptorLayout);

	LVN_CORE_TRACE("created descriptorLayout: (%p), descriptor binding count: %u", *descriptorLayout, createInfo->descriptorBindingCount);
	return lvnctx->graphicsContext.createDescriptorLayout(*descriptorLayout, createInfo);
}

LvnResult createDescriptorSet(LvnDescriptorSet** descriptorSet, LvnDescriptorLayout* descriptorLayout)
{
	LvnContext* lvnctx = lvn::getContext();

	*descriptorSet = lvn::createObject<LvnDescriptorSet>(lvnctx, Lvn_Stype_DescriptorSet);

	LVN_CORE_TRACE("created descriptorSet: (%p) from descriptorLayout: (%p)", *descriptorSet, descriptorLayout);
	return lvnctx->graphicsContext.createDescriptorSet(*descriptorSet, descriptorLayout);
}

LvnResult createPipeline(LvnPipeline** pipeline, LvnPipelineCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

	*pipeline = lvn::createObject<LvnPipeline>(lvnctx, Lvn_Stype_Pipeline);

	LVN_CORE_TRACE("created pipeline: (%p)", *pipeline);
	return lvnctx->graphicsContext.createPipeline(*pipeline, createInfo);
}

LvnResult createFrameBuffer(LvnFrameBuffer** frameBuffer, LvnFrameBufferCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

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

	*frameBuffer = lvn::createObject<LvnFrameBuffer>(lvnctx, Lvn_Stype_FrameBuffer);

	LVN_CORE_TRACE("created framebuffer: (%p)", *frameBuffer);
	return lvnctx->graphicsContext.createFrameBuffer(*frameBuffer, createInfo);
}

LvnResult createBuffer(LvnBuffer** buffer, LvnBufferCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

	// check valid buffer type
	if (createInfo->type == Lvn_BufferType_Unknown)
	{
		LVN_CORE_ERROR("createBuffer(LvnBuffer*, LvnBufferCreateInfo*) | createInfo->type is \'Lvn_BufferType_Unknown\'; cannot create vertex buffer without knowing the type of buffer usage");
		return Lvn_Result_Failure;
	}
	if (createInfo->type & (Lvn_BufferType_Uniform | Lvn_BufferType_Storage))
	{
		LVN_CORE_ERROR("createBuffer(LvnBuffer*, LvnBufferCreateInfo*) | createInfo->type does not have vertex or index buffer type (%u); cannot create vertexbuffer that does not have a vertex or index buffer type", createInfo->type);
		return Lvn_Result_Failure;
	}

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
		if (createInfo->pVertexAttributes[i].type == Lvn_VertexDataType_None)
		{
			LVN_CORE_ERROR("createBuffer(LvnBuffer*, LvnBufferCreateInfo*) | createInfo->pVertexAttributes[%d].type is Lvn_VertexDataType_None, cannot create vertex buffer without a vertex data type", i);
			return Lvn_Result_Failure;
		}
	}

	*buffer = lvn::createObject<LvnBuffer>(lvnctx, Lvn_Stype_Buffer);

	LVN_CORE_TRACE("created buffer: (%p)", *buffer);
	return lvnctx->graphicsContext.createBuffer(*buffer, createInfo);
}

LvnResult createUniformBuffer(LvnUniformBuffer** uniformBuffer, LvnUniformBufferCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

	// check valid buffer type
	if (createInfo->type & Lvn_BufferType_Unknown)
	{
		LVN_CORE_ERROR("createUniformBuffer(LvnUniformBuffer*, LvnUniformBufferCreateInfo*) | createInfo->type is \'Lvn_BufferType_Unknown\'; cannot create uniform buffer without knowing the type of buffer usage");
		return Lvn_Result_Failure;
	}
	if (createInfo->type & (Lvn_BufferType_Vertex | Lvn_BufferType_Index))
	{
		LVN_CORE_ERROR("createUniformBuffer(LvnUniformBuffer*, LvnUniformBufferCreateInfo*) | createInfo->type does not have uniform buffer type (%u); cannot create uniform buffer that does not have a uniform buffer type", createInfo->type);
		return Lvn_Result_Failure;
	}

	*uniformBuffer = lvn::createObject<LvnUniformBuffer>(lvnctx, Lvn_Stype_UniformBuffer);

	LVN_CORE_TRACE("created uniform buffer: (%p), binding: %u, size: %lu bytes", *uniformBuffer, createInfo->binding, createInfo->size);
	return lvnctx->graphicsContext.createUniformBuffer(*uniformBuffer, createInfo);
}

LvnResult createTexture(LvnTexture** texture, LvnTextureCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

	*texture = lvn::createObject<LvnTexture>(lvnctx, Lvn_Stype_Texture);

	LVN_CORE_TRACE("created texture: (%p) using image data: (%p), (w:%u,h:%u,ch:%u), total size: %u bytes", *texture, createInfo->imageData.pixels.data(), createInfo->imageData.width, createInfo->imageData.height, createInfo->imageData.channels, createInfo->imageData.pixels.memsize());
	return lvnctx->graphicsContext.createTexture(*texture, createInfo);
}

LvnResult createCubemap(LvnCubemap** cubemap, LvnCubemapCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

	if (createInfo->posx.pixels.data() == nullptr)
	{
		LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->posx.pixels does not point to a valid pointer array");
		return Lvn_Result_Failure;
	}
	if (createInfo->negx.pixels.data() == nullptr)
	{
		LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->negx.pixels does not point to a valid pointer array");
		return Lvn_Result_Failure;
	}
	if (createInfo->posy.pixels.data() == nullptr)
	{
		LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->posy.pixels does not point to a valid pointer array");
		return Lvn_Result_Failure;
	}
	if (createInfo->negy.pixels.data() == nullptr)
	{
		LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->negy.pixels does not point to a valid pointer array");
		return Lvn_Result_Failure;
	}
	if (createInfo->posz.pixels.data() == nullptr)
	{
		LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->posz.pixels does not point to a valid pointer array");
		return Lvn_Result_Failure;
	}
	if (createInfo->negz.pixels.data() == nullptr)
	{
		LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->negz.pixels does not point to a valid pointer array");
		return Lvn_Result_Failure;
	}

	// if(!(createInfo->posx.width * createInfo->posx.height ==
	// 	createInfo->negx.width * createInfo->negx.height ==
	// 	createInfo->posy.width * createInfo->posy.height ==
	// 	createInfo->negy.width * createInfo->negy.height ==
	// 	createInfo->posz.width * createInfo->posz.height ==
	// 	createInfo->negz.width * createInfo->negz.height))
	// {
	// 	LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | not all images have the same dimensions, all cubemap images must have the same width and height");
	// 	return Lvn_Result_Failure;
	// }

	*cubemap = lvn::createObject<LvnCubemap>(lvnctx, Lvn_Stype_Cubemap);

	LVN_CORE_TRACE("created cubemap: (%p)", *cubemap);
	return lvnctx->graphicsContext.createCubemap(*cubemap, createInfo);
}

LvnMesh createMesh(LvnMeshCreateInfo* createInfo)
{
	LvnMesh mesh{};

	lvn::createBuffer(&mesh.buffer, createInfo->bufferInfo);
	mesh.material = createInfo->material;
	mesh.modelMatrix = LvnMat4(1.0f);

	return mesh;
}

void destroyShader(LvnShader* shader)
{
	if (shader == nullptr) { return; }
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyShader(shader);
	lvn::destroyObject(lvnctx, shader, Lvn_Stype_Shader);
}

void destroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout)
{
	if (descriptorLayout == nullptr) { return; }
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyDescriptorLayout(descriptorLayout);
	lvn::destroyObject(lvnctx, descriptorLayout, Lvn_Stype_DescriptorLayout);
}

void destroyDescriptorSet(LvnDescriptorSet* descriptorSet)
{
	if (descriptorSet == nullptr) { return; }
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyDescriptorSet(descriptorSet);
	lvn::destroyObject(lvnctx, descriptorSet, Lvn_Stype_DescriptorSet);
}

void destroyPipeline(LvnPipeline* pipeline)
{
	if (pipeline == nullptr) { return; }
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyPipeline(pipeline);
	lvn::destroyObject(lvnctx, pipeline, Lvn_Stype_Pipeline);
}

void destroyFrameBuffer(LvnFrameBuffer* frameBuffer)
{
	if (frameBuffer == nullptr) { return; }
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyFrameBuffer(frameBuffer);
	lvn::destroyObject(lvnctx, frameBuffer, Lvn_Stype_FrameBuffer);
}

void destroyBuffer(LvnBuffer* buffer)
{
	if (buffer == nullptr) { return; }
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyBuffer(buffer);
	lvn::destroyObject(lvnctx, buffer, Lvn_Stype_Buffer);
}

void destroyUniformBuffer(LvnUniformBuffer* uniformBuffer)
{
	if (uniformBuffer == nullptr) { return; }
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyUniformBuffer(uniformBuffer);
	lvn::destroyObject(lvnctx, uniformBuffer, Lvn_Stype_UniformBuffer);
}

void destroyTexture(LvnTexture* texture)
{
	if (texture == nullptr) { return; }
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyTexture(texture);
	lvn::destroyObject(lvnctx, texture, Lvn_Stype_Texture);
}

void destroyCubemap(LvnCubemap* cubemap)
{
	if (cubemap == nullptr) { return; }
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyCubemap(cubemap);
	lvn::destroyObject(lvnctx, cubemap, Lvn_Stype_Cubemap);
}

void destroyMesh(LvnMesh* mesh)
{
	lvn::destroyBuffer(mesh->buffer);

	if (mesh->descriptorSet != nullptr)
		lvn::destroyDescriptorSet(mesh->descriptorSet);
}

void pipelineSpecificationSetConfig(LvnPipelineSpecification* pipelineSpecification)
{
	LVN_CORE_ASSERT(pipelineSpecification != nullptr, "pipeline specification points to nullptr when setting pipeline specification config");
	LvnContext* lvnctx = lvn::getContext();
	lvnctx->defaultPipelineSpecification = *pipelineSpecification;
}

LvnPipelineSpecification pipelineSpecificationGetConfig()
{
	LvnContext* lvnctx = lvn::getContext();
	return lvnctx->defaultPipelineSpecification;
}

void bufferUpdateVertexData(LvnBuffer* buffer, void* vertices, uint64_t size, uint64_t offset)
{
	lvn::getContext()->graphicsContext.bufferUpdateVertexData(buffer, vertices, size, offset);
}

void bufferUpdateIndexData(LvnBuffer* buffer, uint32_t* indices, uint64_t size, uint64_t offset)
{
	lvn::getContext()->graphicsContext.bufferUpdateIndexData(buffer, indices, size, offset);
}

void bufferResizeVertexBuffer(LvnBuffer* buffer, uint64_t size)
{
	lvn::getContext()->graphicsContext.bufferResizeVertexBuffer(buffer, size);
}

void bufferResizeIndexBuffer(LvnBuffer* buffer, uint64_t size)
{
	lvn::getContext()->graphicsContext.bufferResizeIndexBuffer(buffer, size);
}

LvnTexture* cubemapGetTextureData(LvnCubemap* cubemap)
{
	return &cubemap->textureData;
}

void updateUniformBufferData(LvnWindow* window, LvnUniformBuffer* uniformBuffer, void* data, uint64_t size)
{
	lvn::getContext()->graphicsContext.updateUniformBufferData(window, uniformBuffer, data, size);
}

void updateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count)
{
	lvn::getContext()->graphicsContext.updateDescriptorSetData(descriptorSet, pUpdateInfo, count);
}

LvnTexture* frameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex)
{
	return lvn::getContext()->graphicsContext.frameBufferGetImage(frameBuffer, attachmentIndex);
}

LvnRenderPass* frameBufferGetRenderPass(LvnFrameBuffer* frameBuffer)
{
	return lvn::getContext()->graphicsContext.frameBufferGetRenderPass(frameBuffer);
}

void frameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height)
{
	return lvn::getContext()->graphicsContext.framebufferResize(frameBuffer, width, height);
}

void frameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a)
{
	return lvn::getContext()->graphicsContext.frameBufferSetClearColor(frameBuffer, attachmentIndex, r, g, b, a);
}

LvnDepthImageFormat findSupportedDepthImageFormat(LvnDepthImageFormat* pDepthImageFormats, uint32_t count)
{
	if (pDepthImageFormats == nullptr)
	{
		LVN_CORE_ERROR("cannot find supported depth image format, no depth image candidates given");
		return (LvnDepthImageFormat)(0);
	}

	return lvn::getContext()->graphicsContext.findSupportedDepthImageFormat(pDepthImageFormats, count);
}

LvnBuffer* meshGetBuffer(LvnMesh* mesh)
{
	return mesh->buffer;
}

LvnMat4 meshGetMatrix(LvnMesh* mesh)
{
	return mesh->modelMatrix;
}

void meshSetMatrix(LvnMesh* mesh, const LvnMat4& matrix)
{
	mesh->modelMatrix = matrix;
}

LvnVertexBindingDescription meshVertexBindingDescroption =
{
	.binding = 0,
	.stride = sizeof(LvnVertex),
};

LvnVertexAttribute meshVertexAttributes[] = 
{
	{ 0, 0, Lvn_VertexDataType_Vec3f, 0 },                   // pos
	{ 0, 1, Lvn_VertexDataType_Vec4f, 3 * sizeof(float) },   // color
	{ 0, 2, Lvn_VertexDataType_Vec2f, 7 * sizeof(float) },   // texUV
	{ 0, 3, Lvn_VertexDataType_Vec3f, 9 * sizeof(float) },   // normal
	{ 0, 4, Lvn_VertexDataType_Vec3f, 12 * sizeof(float) },  // tangent
	{ 0, 5, Lvn_VertexDataType_Vec3f, 15 * sizeof(float) },  // bitangent
};

LvnBufferCreateInfo meshGetVertexBufferCreateInfoConfig(LvnVertex* pVertices, uint32_t vertexCount, uint32_t* pIndices, uint32_t indexCount)
{
	LvnBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.type = Lvn_BufferType_Vertex | Lvn_BufferType_Index;
	bufferCreateInfo.pVertexBindingDescriptions = &meshVertexBindingDescroption;
	bufferCreateInfo.vertexBindingDescriptionCount = 1;
	bufferCreateInfo.pVertexAttributes = meshVertexAttributes;
	bufferCreateInfo.vertexAttributeCount = sizeof(meshVertexAttributes) / sizeof(LvnVertexAttribute);
	bufferCreateInfo.pVertices = pVertices;
	bufferCreateInfo.vertexBufferSize = vertexCount * sizeof(LvnVertex);
	bufferCreateInfo.pIndices = pIndices;
	bufferCreateInfo.indexBufferSize = indexCount * sizeof(uint32_t);

	return bufferCreateInfo;
}

LvnImageData loadImageData(const char* filepath, int forceChannels, bool flipVertically)
{
	if (filepath == nullptr)
	{
		LVN_CORE_ERROR("loadImageData(const char*) | invalid filepath, filepath must not be nullptr");
		return {};
	}

	if (forceChannels < 0)
	{
		LVN_CORE_ERROR("loadImageData(const char*) | forceChannels < 0, channels cannot be negative");
		return {};
	}
	else if (forceChannels > 4)
	{
		LVN_CORE_ERROR("loadImageData(const char*) | forceChannels > 4, channels cannot be higher than 4 components (rgba)");
		return {};
	}

	stbi_set_flip_vertically_on_load(flipVertically);
	int imageWidth, imageHeight, imageChannels;
	stbi_uc* pixels = stbi_load(filepath, &imageWidth, &imageHeight, &imageChannels, forceChannels);

	if (!pixels)
	{
		LVN_CORE_ERROR("loadImageData(const char*) | failed to load image pixel data from file: %s", filepath);
		return {};
	}

	LvnImageData imageData{};
	imageData.width = imageWidth;
	imageData.height = imageHeight;
	imageData.channels = forceChannels ? forceChannels : imageChannels;
	imageData.size = imageData.width * imageData.height * imageData.channels;
	imageData.pixels = LvnData<uint8_t>(pixels, imageData.size);

	LVN_CORE_TRACE("loaded image data <unsigned char*> (%p), (w:%u,h:%u,ch:%u), total memory size: %u bytes, filepath: %s", pixels, imageData.width, imageData.height, imageData.channels, imageData.size, filepath);

	stbi_image_free(pixels);

	return imageData;
}

LvnImageData loadImageDataMemory(const uint8_t* data, int length, int forceChannels, bool flipVertically)
{
	if (!data)
	{
		LVN_CORE_ERROR("loadImageDataMemory(const unsigned char*, int, int, bool) | invalid filepath, filepath must not be nullptr");
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

	LvnImageData imageData{};
	imageData.width = imageWidth;
	imageData.height = imageHeight;
	imageData.channels = forceChannels ? forceChannels : imageChannels;
	imageData.size = imageData.width * imageData.height * imageData.channels;
	imageData.pixels = LvnData<uint8_t>(pixels, imageData.size);

	LVN_CORE_TRACE("loaded image data from memory <unsigned char*> (%p), (w:%u,h:%u,ch:%u), total memory size: %u bytes", pixels, imageData.width, imageData.height, imageData.channels, imageData.size);

	stbi_image_free(pixels);

	return imageData;
}

LvnModel loadModel(const char* filepath)
{
	std::string filepathstr(filepath);
	std::string extensionType = filepathstr.substr(filepathstr.find_last_of(".") + 1);

	if (extensionType == "gltf")
	{
		return lvn::loadGltfModel(filepath);
	}
	else if (extensionType == "glb")
	{
		return lvn::loadGlbModel(filepath);
	}

	LVN_CORE_WARN("loadModel(const char*) | could not load model, file extension type not recognized (%s), Filepath: %s", extensionType.c_str(), filepath);
	return {};
}

void freeModel(LvnModel* model)
{
	for (uint32_t i = 0; i < model->meshes.size(); i++)
	{
		lvn::destroyMesh(&model->meshes[i]);
	}
	
	for (uint32_t i = 0; i < model->textures.size(); i++)
	{
		lvn::destroyTexture(model->textures[i]);
	}
}

LvnCamera cameraConfigInit(LvnCameraCreateInfo* createInfo)
{
	LvnCamera camera{};
	camera.width = createInfo->width;
	camera.height = createInfo->height;
	camera.position = createInfo->position;
	camera.orientation = createInfo->orientation;
	camera.upVector = createInfo->upVector;
	camera.fov = createInfo->fovDeg;
	camera.nearPlane = createInfo->nearPlane;
	camera.farPlane = createInfo->farPlane;

	camera.viewMatrix = lvn::lookAt(camera.position, camera.position + camera.orientation, camera.upVector);
	camera.projectionMatrix = lvn::perspective(lvn::radians(camera.fov), static_cast<float>(camera.width) / static_cast<float>(camera.height), camera.nearPlane, camera.farPlane);
	camera.matrix = camera.projectionMatrix * camera.viewMatrix;

	return camera;
}

void cameraUpdateMatrix(LvnCamera* camera)
{
	camera->projectionMatrix = lvn::perspective(lvn::radians(camera->fov), static_cast<float>(camera->width) / static_cast<float>(camera->height), camera->nearPlane, camera->farPlane);
	camera->viewMatrix = lvn::lookAt(camera->position, camera->position + camera->orientation, camera->upVector);
	camera->matrix = camera->projectionMatrix * camera->viewMatrix;
}

void cameraSetFov(LvnCamera* camera, float fovDeg)
{
	camera->fov = fovDeg;
	lvn::cameraUpdateMatrix(camera);
}

void cameraSetPlane(LvnCamera* camera, float nearPlane, float farPlane)
{
	camera->nearPlane = nearPlane;
	camera->farPlane = farPlane;
	lvn::cameraUpdateMatrix(camera);
}

void cameraSetPos(LvnCamera* camera, const LvnVec3& position)
{
	camera->position = position;
	lvn::cameraUpdateMatrix(camera);
}

void cameraSetOrient(LvnCamera* camera, const LvnVec3& orientation)
{
	camera->orientation = orientation;
	lvn::cameraUpdateMatrix(camera);
}

void setCameraUpVec(LvnCamera* camera, const LvnVec3& upVector)
{
	camera->upVector = camera->upVector;
	lvn::cameraUpdateMatrix(camera);
}

float cameraGetFov(LvnCamera* camera)
{
	return camera->fov;
}

float cameraGetNearPlane(LvnCamera* camera)
{
	return camera->nearPlane;
}

float cameraGetFarPlane(LvnCamera* camera)
{
	return camera->farPlane;
}

LvnVec3 cameraGetPos(LvnCamera* camera)
{
	return camera->position;
}

LvnVec3 cameraGetOrient(LvnCamera* camera)
{
	return camera->orientation;
}

LvnVec3 cameraGetUpVec(LvnCamera* camera)
{
	return camera->upVector;
}


uint32_t getVertexDataTypeSize(LvnVertexDataType type)
{
	switch (type)
	{
		case Lvn_VertexDataType_None:        { return 0; }
		case Lvn_VertexDataType_Float:       { return sizeof(float); }
		case Lvn_VertexDataType_Double:      { return sizeof(double); }
		case Lvn_VertexDataType_Int:         { return sizeof(int); }
		case Lvn_VertexDataType_UnsignedInt: { return sizeof(uint32_t); }
		case Lvn_VertexDataType_Bool:        { return sizeof(bool); }
		case Lvn_VertexDataType_Vec2:        { return sizeof(float) * 2; }
		case Lvn_VertexDataType_Vec3:        { return sizeof(float) * 3; }
		case Lvn_VertexDataType_Vec4:        { return sizeof(float) * 4; }
		case Lvn_VertexDataType_Vec2i:       { return sizeof(int32_t) * 2; }
		case Lvn_VertexDataType_Vec3i:       { return sizeof(int32_t) * 3; }
		case Lvn_VertexDataType_Vec4i:       { return sizeof(int32_t) * 4; }
		case Lvn_VertexDataType_Vec2ui:      { return sizeof(uint32_t) * 2; }
		case Lvn_VertexDataType_Vec3ui:      { return sizeof(uint32_t) * 3; }
		case Lvn_VertexDataType_Vec4ui:      { return sizeof(uint32_t) * 4; }
		case Lvn_VertexDataType_Vec2d:       { return sizeof(double) * 2; }
		case Lvn_VertexDataType_Vec3d:       { return sizeof(double) * 3; }
		case Lvn_VertexDataType_Vec4d:       { return sizeof(double) * 4; }

		default:
		{
			LVN_CORE_WARN("unknown vertex data type enum: (%u)", type);
			return 0;
		}
	}
}


// ------------------------------------------------------------
// [SECTION]: Audio Functions
// ------------------------------------------------------------

LvnResult createSoundFromFile(LvnSound** sound, LvnSoundCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();
	ma_engine* pEngine = static_cast<ma_engine*>(lvnctx->audioEngineContextPtr);

	if (createInfo->filepath.empty())
	{
		LVN_CORE_ERROR("createSoundFromFile(LvnSound**, LvnSoundCreateInfo*) | createInfo->filepath is nullptr, cannot load sound data without a valid path to the sound file");
		return Lvn_Result_Failure;
	}

	*sound = lvn::createObject<LvnSound>(lvnctx, Lvn_Stype_Sound);

	LvnSound* soundPtr = *sound;
	soundPtr->volume = createInfo->volume;
	soundPtr->pan = createInfo->pan;
	soundPtr->pitch = createInfo->pitch;
	soundPtr->pos = createInfo->pos;
	soundPtr->looping = createInfo->looping;
	soundPtr->soundBoard = nullptr;

	if (ma_sound_init_from_file(pEngine, createInfo->filepath.c_str(), 0, NULL, NULL, &soundPtr->sound) != MA_SUCCESS)
	{
		LVN_CORE_ERROR("createSoundFromFile(LvnSound**, LvnSoundCreateInfo*) | failed to create sound object");
		return Lvn_Result_Failure;
	}

	ma_sound_set_volume(&soundPtr->sound, createInfo->volume);
	ma_sound_set_pan(&soundPtr->sound, createInfo->pan);
	ma_sound_set_pitch(&soundPtr->sound, createInfo->pitch);
	ma_sound_set_looping(&soundPtr->sound, createInfo->looping);

	LVN_CORE_TRACE("created sound: (%p), volume: %.2f, pan: %.2f, pitch: %.2f", *sound, createInfo->volume, createInfo->pan, createInfo->pitch);
	return Lvn_Result_Success;
}

void destroySound(LvnSound* sound)
{
	if (sound == nullptr) { return; }
	LvnContext* lvnctx = lvn::getContext();

	ma_sound_uninit(&sound->sound);

	lvn::destroyObject(lvnctx, sound, Lvn_Stype_Sound);
}

LvnSoundCreateInfo soundConfigInit(const char* filepath)
{
	LvnSoundCreateInfo soundInit{};
	soundInit.pos = { 0.0f, 0.0f, 0.0f };
	soundInit.volume = 1.0f;
	soundInit.pan = 0.0f;
	soundInit.pitch = 1.0f;
	soundInit.looping = false;
	soundInit.filepath = filepath;

	return soundInit;
}

void soundSetVolume(LvnSound* sound, float volume)
{
	if (sound->soundBoard != nullptr) { volume *= sound->soundBoard->masterVolume; }

	ma_sound_set_volume(&sound->sound, volume);
}

void soundSetPan(LvnSound* sound, float pan)
{
	if (sound->soundBoard != nullptr) { pan *= sound->soundBoard->masterPan; }

	ma_sound_set_pan(&sound->sound, pan);
}

void soundSetPitch(LvnSound* sound, float pitch)
{
	if (sound->soundBoard != nullptr) { pitch *= sound->soundBoard->masterPitch; }

	ma_sound_set_pitch(&sound->sound, pitch);
}

void soundSetLooping(LvnSound* sound, bool looping)
{
	ma_sound_set_looping(&sound->sound, looping);
}

void soundPlayStart(LvnSound* sound)
{
	ma_sound_start(&sound->sound);
}

void soundPlayStop(LvnSound* sound)
{
	ma_sound_stop(&sound->sound);
}

void soundTogglePause(LvnSound* sound)
{
	if (ma_sound_is_playing(&sound->sound)) { ma_sound_stop(&sound->sound); }
	else { ma_sound_start(&sound->sound); }
}

bool soundIsPlaying(LvnSound* sound)
{
	return ma_sound_is_playing(&sound->sound);
}

uint64_t soundGetTimeMiliseconds(LvnSound* sound)
{
	return ma_sound_get_time_in_milliseconds(&sound->sound);
}

float soundGetLengthSeconds(LvnSound* sound)
{
	float length;
	ma_sound_get_length_in_seconds(&sound->sound, &length);

	return length;
}

bool soundIsAttachedToSoundBoard(const LvnSound* sound)
{
	return sound->soundBoard != nullptr;
}

LvnResult createSoundBoard(LvnSoundBoard** soundBoard)
{
	LvnContext* lvnctx = lvn::getContext();

	*soundBoard = lvn::createObject<LvnSoundBoard>(lvnctx, Lvn_Stype_SoundBoard);

	LvnSoundBoard* soundBoardPtr = *soundBoard;
	soundBoardPtr->masterVolume = 1.0f;
	soundBoardPtr->masterPan = 0.0f;
	soundBoardPtr->masterPitch = 1.0f;

	LVN_CORE_TRACE("created sound board: (%p)", *soundBoard);
	return Lvn_Result_Success;
}

void destroySoundBoard(LvnSoundBoard* soundBoard)
{
	if (soundBoard == nullptr) { return; }
	LvnContext* lvnctx = lvn::getContext();

	for (auto& sound : soundBoard->sounds)
	{
		ma_sound_uninit(&sound.second.sound);
	}

	lvn::destroyObject(lvnctx, soundBoard, Lvn_Stype_SoundBoard);
}

LvnResult soundBoardAddSound(LvnSoundBoard* soundBoard, LvnSoundCreateInfo* soundInfo, uint32_t id)
{
	LvnContext* lvnctx = lvn::getContext();
	ma_engine* pEngine = static_cast<ma_engine*>(lvnctx->audioEngineContextPtr);

	if (soundInfo->filepath.empty())
	{
		LVN_CORE_ERROR("cannot add sound to sound board (%p), soundInfo->filepath is nullptr, cannot load sound data without a valid path to the sound file", soundBoard);
		return Lvn_Result_Failure;
	}

	if (soundBoard->sounds.find(id) != soundBoard->sounds.end())
	{
		LVN_CORE_ERROR("cannot add sound to sound board (%p), sound board already has a sound with that id: %u", soundBoard, id);
		return Lvn_Result_Failure;
	}

	LvnSound sound{};
	sound.pan = soundInfo->pan;
	sound.pitch = soundInfo->pitch;
	sound.pos = soundInfo->pos;
	sound.looping = soundInfo->looping;
	sound.soundBoard = soundBoard;

	if (ma_sound_init_from_file(pEngine, soundInfo->filepath.c_str(), 0, NULL, NULL, &sound.sound) != MA_SUCCESS)
	{
		LVN_CORE_ERROR("failed to create sound object when adding sound to sound board (%p)", soundBoard);
		return Lvn_Result_Failure;
	}

	ma_sound_set_volume(&sound.sound, soundInfo->volume);
	ma_sound_set_pan(&sound.sound, soundInfo->pan);
	ma_sound_set_pitch(&sound.sound, soundInfo->pitch);
	ma_sound_set_looping(&sound.sound, soundInfo->looping);

	soundBoard->sounds[id] = sound;
	return Lvn_Result_Success;
}

void soundBoardRemoveSound(LvnSoundBoard* soundBoard, uint32_t id)
{
	if (soundBoard->sounds.find(id) != soundBoard->sounds.end())
	{
		ma_sound_uninit(&soundBoard->sounds[id].sound);
		soundBoard->sounds.erase(id);
	}
}

const LvnSound* soundBoardGetSound(LvnSoundBoard* soundBoard, uint32_t id)
{
	if (soundBoard->sounds.find(id) == soundBoard->sounds.end()) { return nullptr; }

	return &soundBoard->sounds[id];
}

// ------------------------------------------------------------
// [SECTION]: Network Functions
// ------------------------------------------------------------

LvnResult createSocket(LvnSocket** socket, LvnSocketCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

	*socket = lvn::createObject<LvnSocket>(lvnctx, Lvn_Stype_Socket);
	LvnSocket* socketPtr = *socket;

	ENetAddress address;
	address.host = createInfo->address.host;
	address.port = createInfo->address.port;

	if (createInfo->type == Lvn_SocketType_Client)
	{
		socketPtr->socket = enet_host_create(nullptr, createInfo->connectionCount, createInfo->channelCount, createInfo->inBandWidth, createInfo->outBandWidth);
	}
	else if (createInfo->type == Lvn_SocketType_Server)
	{
		socketPtr->socket = enet_host_create(&address, createInfo->connectionCount, createInfo->channelCount, createInfo->inBandWidth, createInfo->outBandWidth);
	}

	if (socketPtr->socket == nullptr)
	{
		LVN_CORE_ERROR("createSocket(LvnSocket**, LvnSocketCreateInfo*) | an error occured while trying to create socket");
		return Lvn_Result_Failure;
	}

	socketPtr->connection = nullptr;
	socketPtr->type = createInfo->type;
	socketPtr->address = createInfo->address;
	socketPtr->channelCount = createInfo->channelCount;
	socketPtr->channelCount = createInfo->channelCount;
	socketPtr->inBandWidth = createInfo->inBandWidth;
	socketPtr->outBandWidth = createInfo->outBandWidth;

	LVN_CORE_TRACE("created socket: (%p), address: (%u:%u)", createInfo->address.host, createInfo->address.port);
	return Lvn_Result_Success;
}

void destroySocket(LvnSocket* socket)
{
	if (socket == nullptr) { return; }
	LvnContext* lvnctx = lvn::getContext();
	enet_host_destroy(socket->socket);
	lvn::destroyObject(lvnctx, socket, Lvn_Stype_Socket);
}

LvnSocketCreateInfo socketClientConfigInit(uint32_t connectionCount, uint32_t channelCount, uint32_t inBandwidth, uint32_t outBandWidth)
{
	LvnSocketCreateInfo createInfo{};
	createInfo.type = Lvn_SocketType_Client;
	createInfo.connectionCount = connectionCount;
	createInfo.channelCount = channelCount;
	createInfo.inBandWidth = inBandwidth;
	createInfo.outBandWidth = outBandWidth;

	return createInfo;
}

LvnSocketCreateInfo socketServerConfigInit(LvnAddress address, uint32_t connectionCount, uint32_t channelCount, uint32_t inBandwidth, uint32_t outBandWidth)
{
	LvnSocketCreateInfo createInfo{};
	createInfo.type = Lvn_SocketType_Client;
	createInfo.address = address;
	createInfo.connectionCount = connectionCount;
	createInfo.channelCount = channelCount;
	createInfo.inBandWidth = inBandwidth;
	createInfo.outBandWidth = outBandWidth;

	return createInfo;
}

uint32_t socketGetHostFromStr(const char* host)
{
	ENetAddress address;
	enet_address_set_host(&address, host);
	return address.host;
}

LvnResult socketConnect(LvnSocket* socket, LvnAddress* address, uint32_t channelCount, uint32_t milliseconds)
{
	if (socket->type != Lvn_SocketType_Client)
	{
		LVN_CORE_ERROR("cannot use socket (%p) with type that is not client to connect", socket->socket);
		return Lvn_Result_Failure;
	}

	ENetAddress enetAddress;
	enetAddress, socket->address.host;
	enetAddress.port = socket->address.port;

	socket->connection = enet_host_connect(socket->socket, &enetAddress, channelCount, 0);

	if (socket->connection == nullptr)
	{
		LVN_CORE_ERROR("no available peers for initiating a connection on socket (%p)", socket);
		return Lvn_Result_Failure;
	}

	ENetEvent event;
	if (enet_host_service(socket->socket, &event, milliseconds) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		return Lvn_Result_Success;
	}

	enet_peer_reset(socket->connection);
	return Lvn_Result_TimeOut;
}

LvnResult socketDisconnect(LvnSocket* socket, uint32_t milliseconds)
{
	if (socket->type != Lvn_SocketType_Client)
	{
		LVN_CORE_ERROR("cannot use socket (%p) with type that is not client to disconnect", socket->socket);
		return Lvn_Result_Failure;
	}
	enet_peer_disconnect(socket->connection, 0);

	ENetEvent event;
	if (enet_host_service(socket->socket, &event, milliseconds) > 0)
	{
		switch (event.type)
		{
			case ENET_EVENT_TYPE_RECEIVE:
			{
				enet_packet_destroy(event.packet);
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				return Lvn_Result_Success;
			}
		}
	}

	enet_peer_reset(socket->connection);
	return Lvn_Result_Success;
}

void socketSend(LvnSocket* socket, uint8_t channel, LvnPacket* packet)
{
	LVN_CORE_ASSERT(packet != nullptr, "packet is nullptr when trying to send packet through socket");

	ENetPacket* enetPacket = enet_packet_create(packet->data, packet->size, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(socket->connection, channel, enetPacket);
	enet_host_flush(socket->socket);
}

LvnResult socketReceive(LvnSocket* socket, LvnPacket* packet, uint32_t milliseconds)
{
	LVN_CORE_ASSERT(packet != nullptr, "packet is nullptr when trying to receive packet from socket");

	ENetEvent event;
	if (enet_host_service(socket->socket, &event, milliseconds) > 0 && event.type == ENET_EVENT_TYPE_RECEIVE)
	{
		packet->data = event.packet->data;
		packet->size = event.packet->dataLength;
		enet_packet_destroy(event.packet);
		return Lvn_Result_Success;
	}

	return Lvn_Result_TimeOut;
}

// ------------------------------------------------------------
// [SECTION]: Math Functions
// ------------------------------------------------------------

float radians(float deg)
{
	return deg * 0.0174532925199f; // deg * (PI / 180)
}

float degrees(float rad)
{
	return rad * 57.2957795131f; // rad * (180 / PI)
}

float clampAngle(float rad)
{
	float angle = fmod(rad, 2 * LVN_PI);
	if (angle < 0) { angle += 2 * LVN_PI; }
	return angle;
}

float clampAngleDeg(float deg)
{
	float angle = fmod(deg, 360.0f);
	if (angle < 0) { angle += 360.0f; }
	return angle;
}

float invSqrt(float num)
{
	union
	{
		float f;
		uint32_t i;
	} conv;

	float x2;
	const float threehalfs = 1.5f;

	x2 = num * 0.5f;
	conv.f = num;
	conv.i = 0x5f3759df - (conv.i >> 1);
	conv.f = conv.f * (threehalfs - (x2 * conv.f * conv.f));
	return conv.f;
}

LvnVec3f cross(LvnVec3f v1, LvnVec3f v2)
{
	float cx = v1.y * v2.z - v1.z * v2.y;
	float cy = v1.z * v2.x - v1.x * v2.z;
	float cz = v1.x * v2.y - v1.y * v2.x;
	return { cx, cy, cz };
}

LvnVec3d cross(LvnVec3d v1, LvnVec3d v2)
{
	double cx = v1.y * v2.z - v1.z * v2.y;
	double cy = v1.z * v2.x - v1.x * v2.z;
	double cz = v1.x * v2.y - v1.y * v2.x;
	return { cx, cy, cz };
}

} /* namespace lvn */
