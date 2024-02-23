#ifndef HG_LEVIKNO_H
#define HG_LEVIKNO_H


#ifndef HG_LEVIKNO_DEFINE_CONFIG
#define HG_LEVIKNO_DEFINE_CONFIG

// Platform
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) 
	#ifndef LVN_PLATFORM_WINDOWS
		#define LVN_PLATFORM_WINDOWS
	#endif

#elif __APPLE__
	#ifndef LVN_PLATFORM_APPLE
		#define LVN_PLATFORM_APPLE
	#endif

#elif __linux__
	#ifndef LVN_PLATFORM_LINUX
		#define LVN_PLATFORM_LINUX
	#endif
	#include <cassert> /* use assert() */

#else
	#error "Levikno does not support the current platform."
#endif

// dll
#ifdef LVN_PLATFORM_WINDOWS
  #ifdef LVN_SHARED_LIBRARY_EXPORT
	  #define LVN_API __declspec(dllexport)
  #elif LVN_SHARED_LIBRARY_IMPORT
	  #define LVN_API __declspec(dllimport)
  #else 
	  #define LVN_API
  #endif
#else
  #define LVN_API
#endif 

#define LVN_API
// Compiler
#ifdef _MSC_VER
	#define LVN_ASSERT_BREAK __debugbreak()
	#pragma warning (disable : 4267)
	#pragma warning (disable : 4244)
	#pragma warning (disable : 26495)

	#ifdef _DEBUG
		#ifndef LVN_DEBUG
			#define LVN_DEBUG
		#endif
	#endif
#else
	#ifndef NDEBUG
		#ifndef LVN_DEBUG
			#define LVN_DEBUG
		#endif
	#endif

	#define LVN_ASSERT_BREAK assert(false)
#endif

// Debug
#ifdef LVN_DEBUG
	#define LVN_ENABLE_ASSERTS
#endif

#ifdef LVN_DISABLE_ASSERTS_KEEP_ERROR_MESSAGES
	#define LVN_DISABLE_ASSERTS
#endif

#if defined (LVN_DISABLE_ASSERTS)
	#define LVN_ASSERT(x, ...) { if(!(x)) { LVN_ERROR(__VA_ARGS__); } }
	#define LVN_CORE_ASSERT(x, ...) { if(!(x)) { LVN_CORE_ERROR(__VA_ARGS__); } }
#elif defined(LVN_ENABLE_ASSERTS)
	#define LVN_ASSERT(x, ...) { if(!(x)) { LVN_ERROR(__VA_ARGS__); LVN_ASSERT_BREAK; } }
	#define LVN_CORE_ASSERT(x, ...) { if(!(x)) { LVN_CORE_ERROR(__VA_ARGS__); LVN_ASSERT_BREAK; } }
#else
	#define LVN_ASSERT(x, ...)
	#define LVN_CORE_ASSERT(x, ...)
#endif

// Warnings
#ifdef LVN_DEBUG

#endif


#define LVN_TRUE 1
#define LVN_FALSE 0
#define LVN_NULL_HANDLE nullptr

#define LVN_UINT8_MAX  0xff
#define LVN_UINT16_MAX 0xffff
#define LVN_UINT32_MAX 0xffffffff
#define LVN_UINT64_MAX 0xffffffffffffffff

#define LVN_ALIGN(x) alignas(x)
#define LVN_SCAST(type, x) static_cast<type>(x)

#define LVN_FILE_NAME __FILE__
#define LVN_LINE __LINE__
#define LVN_FUNC_NAME __func__

#define LVN_STR(x) #x
#define LVN_STRINGIFY(x) LVN_STR(x)

#define LVN_MALLOC(size) ::lvn::memAlloc(size)
#define LVN_FREE(ptr)    ::lvn::memFree(ptr)


#endif // !HG_LEVIKNO_DEFINE_CONFIG

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* [Logging] */

/*
*   Color          | FG | BG
* -----------------+----+----
*	Black          | 30 | 40
*	Red            | 31 | 41
*	Green          | 32 | 42
*	Yellow         | 33 | 43
*	Blue           | 34 | 44
*	Magenta        | 35 | 45
*	Cyan           | 36 | 46
*	White          | 37 | 47
*	Bright Black   | 90 | 100
*	Bright Red     | 91 | 101
*	Bright Green   | 92 | 102
*	Bright Yellow  | 93 | 103
*	Bright Blue    | 94 | 104
*	Bright Magenta | 95 | 105
*	Bright Cyan    | 96 | 106
*	Bright White   | 97 | 107
* 
* 
*	reset             0
*	bold/bright       1
*	underline         4
*	inverse           7
*	bold/bright off  21
*	underline off    24
*	inverse off      27
* 
*	
*	Log Colors:
*	TRACE			\x1b[1;37m
*	INFO			\x1b[0;32m
*	WARN			\x1b[1;33m
*	ERROR			\x1b[1;31m
*	CRITICAL		\x1b[1;37;41m
* 
*/

#define LVN_LOG_COLOR_TRACE 					"\x1b[0;37m"
#define LVN_LOG_COLOR_INFO 						"\x1b[0;32m"
#define LVN_LOG_COLOR_WARN 						"\x1b[1;33m"
#define LVN_LOG_COLOR_ERROR 					"\x1b[1;31m"
#define LVN_LOG_COLOR_CRITICAL					"\x1b[1;37;41m"
#define LVN_LOG_COLOR_RESET						"\x1b[0m"


// Core Log macros
#define LVN_CORE_TRACE(...)						::lvn::logMessageTrace(lvn::getCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_INFO(...)						::lvn::logMessageInfo(lvn::getCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_WARN(...)						::lvn::logMessageWarn(lvn::getCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_ERROR(...)						::lvn::logMessageError(lvn::getCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_CRITICAL(...)					::lvn::logMessageCritical(lvn::getCoreLogger(), ##__VA_ARGS__)

// Client Log macros
#define LVN_TRACE(...)		  					::lvn::logMessageTrace(lvn::getClientLogger(), ##__VA_ARGS__)
#define LVN_INFO(...)							::lvn::logMessageInfo(lvn::getClientLogger(), ##__VA_ARGS__)
#define LVN_WARN(...)							::lvn::logMessageWarn(lvn::getClientLogger(), ##__VA_ARGS__)
#define LVN_ERROR(...)							::lvn::logMessageError(lvn::getClientLogger(), ##__VA_ARGS__)
#define LVN_CRITICAL(...)						::lvn::logMessageCritical(lvn::getClientLogger(), ##__VA_ARGS__)


// Logging utils
#define LVN_PROPERTIES(prop)					#prop, &prop
#define LVN_LOG_FILE							LVN_FILE_NAME ":" LVN_STRINGIFY(LVN_LINE) " - "


/* [Core Enums] */

enum LvnResult
{
	Lvn_Result_Failure = 0,
	Lvn_Result_Success = 1,
	Lvn_Result_AlreadyCalled = 2
};

enum LvnStructureType
{
	Lvn_StructureType_Undefined = 0,
	Lvn_StructureType_Window,
	Lvn_StructureType_Logger,
	Lvn_StructureType_RenderPass,
	Lvn_StructureType_FrameBuffer,
	Lvn_StructureType_Shader,
	Lvn_StructureType_Pipeline,
	Lvn_StructureType_VertexArrayBuffer,
};

enum LvnMemoryBlockUsage
{
	Lvn_MemoryBlockUsage_Static,
	Lvn_MemoryBlockUsage_Dynamic,
};

/* Key Codes */
enum LvnKeyCodes
{
	Lvn_KeyCode_Space			= 32,
	Lvn_KeyCode_Apostrophe		= 39,		/* ' */
	Lvn_KeyCode_Comma			= 44,		/* , */
	Lvn_KeyCode_Minus			= 45,		/* - */
	Lvn_KeyCode_Period			= 46,		/* . */
	Lvn_KeyCode_Slash			= 47,		/* / */
	Lvn_KeyCode_0				= 48,
	Lvn_KeyCode_1				= 49,
	Lvn_KeyCode_2				= 50,
	Lvn_KeyCode_3				= 51,
	Lvn_KeyCode_4				= 52,
	Lvn_KeyCode_5				= 53,
	Lvn_KeyCode_6				= 54,
	Lvn_KeyCode_7				= 55,
	Lvn_KeyCode_8				= 56,
	Lvn_KeyCode_9				= 57,
	Lvn_KeyCode_Semicolon		= 59,		/* ; */
	Lvn_KeyCode_Equal			= 61,		/* = */
	Lvn_KeyCode_A				= 65,
	Lvn_KeyCode_B				= 66,
	Lvn_KeyCode_C				= 67,
	Lvn_KeyCode_D				= 68,
	Lvn_KeyCode_E				= 69,
	Lvn_KeyCode_F				= 70,
	Lvn_KeyCode_G				= 71,
	Lvn_KeyCode_H				= 72,
	Lvn_KeyCode_I				= 73,
	Lvn_KeyCode_J				= 74,
	Lvn_KeyCode_K				= 75,
	Lvn_KeyCode_L				= 76,
	Lvn_KeyCode_M				= 77,
	Lvn_KeyCode_N				= 78,
	Lvn_KeyCode_O				= 79,
	Lvn_KeyCode_P				= 80,
	Lvn_KeyCode_Q				= 81,
	Lvn_KeyCode_R				= 82,
	Lvn_KeyCode_S				= 83,
	Lvn_KeyCode_T				= 84,
	Lvn_KeyCode_U				= 85,
	Lvn_KeyCode_V				= 86,
	Lvn_KeyCode_W				= 87,
	Lvn_KeyCode_X				= 88,
	Lvn_KeyCode_Y				= 89,
	Lvn_KeyCode_Z				= 90,
	Lvn_KeyCode_LeftBracket		= 91,		/* [ */
	Lvn_KeyCode_Backslash		= 92,		/* \ */
	Lvn_KeyCode_RightBracket	= 93,		/* ] */
	Lvn_KeyCode_GraveAccent		= 96,		/* ` */
	Lvn_KeyCode_World1			= 161,		/* non-US #1 */
	Lvn_KeyCode_World2			= 162,		/* non-US #2 */

	/* Function keys */
	Lvn_KeyCode_Escape			= 256,
	Lvn_KeyCode_Enter			= 257,
	Lvn_KeyCode_Tab				= 258,
	Lvn_KeyCode_Backspace		= 259,
	Lvn_KeyCode_Insert			= 260,
	Lvn_KeyCode_Delete			= 261,
	Lvn_KeyCode_Right			= 262,
	Lvn_KeyCode_Left			= 263,
	Lvn_KeyCode_Down			= 264,
	Lvn_KeyCode_Up				= 265,
	Lvn_KeyCode_PageUp			= 266,
	Lvn_KeyCode_PageDown		= 267,
	Lvn_KeyCode_Home			= 268,
	Lvn_KeyCode_End				= 269,
	Lvn_KeyCode_CapsLock		= 280,
	Lvn_KeyCode_ScrollLock		= 281,
	Lvn_KeyCode_NumLock			= 282,
	Lvn_KeyCode_PrintScreen		= 283,
	Lvn_KeyCode_Pause			= 284,
	Lvn_KeyCode_F1				= 290,
	Lvn_KeyCode_F2				= 291,
	Lvn_KeyCode_F3				= 292,
	Lvn_KeyCode_F4				= 293,
	Lvn_KeyCode_F5				= 294,
	Lvn_KeyCode_F6				= 295,
	Lvn_KeyCode_F7				= 296,
	Lvn_KeyCode_F8				= 297,
	Lvn_KeyCode_F9				= 298,
	Lvn_KeyCode_F10				= 299,
	Lvn_KeyCode_F11				= 300,
	Lvn_KeyCode_F12				= 301,
	Lvn_KeyCode_F13				= 302,
	Lvn_KeyCode_F14				= 303,
	Lvn_KeyCode_F15				= 304,
	Lvn_KeyCode_F16				= 305,
	Lvn_KeyCode_F17				= 306,
	Lvn_KeyCode_F18				= 307,
	Lvn_KeyCode_F19				= 308,
	Lvn_KeyCode_F20				= 309,
	Lvn_KeyCode_F21				= 310,
	Lvn_KeyCode_F22				= 311,
	Lvn_KeyCode_F23				= 312,
	Lvn_KeyCode_F24				= 313,
	Lvn_KeyCode_F25				= 314,
	Lvn_KeyCode_KP_0			= 320,
	Lvn_KeyCode_KP_1			= 321,
	Lvn_KeyCode_KP_2			= 322,
	Lvn_KeyCode_KP_3			= 323,
	Lvn_KeyCode_KP_4			= 324,
	Lvn_KeyCode_KP_5			= 325,
	Lvn_KeyCode_KP_6			= 326,
	Lvn_KeyCode_KP_7			= 327,
	Lvn_KeyCode_KP_8			= 328,
	Lvn_KeyCode_KP_9			= 329,
	Lvn_KeyCode_KP_Decimal		= 330,
	Lvn_KeyCode_KP_Divide		= 331,
	Lvn_KeyCode_KP_Multiply		= 332,
	Lvn_KeyCode_KP_Subtract		= 333,
	Lvn_KeyCode_KP_Add			= 334,
	Lvn_KeyCode_KP_Enter		= 335,
	Lvn_KeyCode_KP_Equal		= 336,
	Lvn_KeyCode_LeftShift		= 340,
	Lvn_KeyCode_LeftControl		= 341,
	Lvn_KeyCode_LeftAlt			= 342,
	Lvn_KeyCode_LeftSuper		= 343,
	Lvn_KeyCode_RightShift		= 344,
	Lvn_KeyCode_RightControl	= 345,
	Lvn_KeyCode_RightAlt		= 346,
	Lvn_KeyCode_RightSuper		= 347,
	Lvn_KeyCode_Menu			= 348,
};

/* Mouse Button Codes */
enum LvnMouseButtonCodes
{
	Lvn_MouseButton_1			= 0,
	Lvn_MouseButton_2			= 1,
	Lvn_MouseButton_3			= 2,
	Lvn_MouseButton_4			= 3,
	Lvn_MouseButton_5			= 4,
	Lvn_MouseButton_6			= 5,
	Lvn_MouseButton_7			= 6,
	Lvn_MouseButton_8			= 7,
	Lvn_MouseButton_Last		= Lvn_MouseButton_8,
	Lvn_MouseButton_Left		= Lvn_MouseButton_1,
	Lvn_MouseButton_Right		= Lvn_MouseButton_2,
	Lvn_MouseButton_Middle		= Lvn_MouseButton_3,
};

/* Mouse Mode Codes */
enum LvnMouseCursorModes
{
	Lvn_MouseCursor_Arrow			= 0,
	Lvn_MouseCursor_Ibeam			= 1,
	Lvn_MouseCursor_Crosshair		= 2,
	Lvn_MouseCursor_PointingHand	= 3,
	Lvn_MouseCursor_ResizeEW		= 4,
	Lvn_MouseCursor_ResizeNS		= 5,
	Lvn_MouseCursor_ResizeNWSE		= 6,
	Lvn_MouseCursor_ResizeNESW		= 7,
	Lvn_MouseCursor_ResizeAll		= 8,
	Lvn_MouseCursor_NotAllowed		= 9,
	Lvn_MouseCursor_HResize			= Lvn_MouseCursor_ResizeEW,
	Lvn_MouseCursor_VRrsize			= Lvn_MouseCursor_ResizeNS,
	Lvn_MouseCursor_Hand			= Lvn_MouseCursor_PointingHand,
	Lvn_MouseCursor_Max				= 10,
};

/* Logging Types */
enum LvnLogLevel
{
	Lvn_LogLevel_None		= 0,
	Lvn_LogLevel_Trace		= 1,
	Lvn_LogLevel_Info		= 2,
	Lvn_LogLevel_Warn		= 3,
	Lvn_LogLevel_Error		= 4,
	Lvn_LogLevel_Critical	= 5,
};

enum LvnEventType
{
	Lvn_EventType_None = 0,
	Lvn_EventType_AppTick,
	Lvn_EventType_AppRender,
	Lvn_EventType_KeyPressed,
	Lvn_EventType_KeyReleased,
	Lvn_EventType_KeyHold,
	Lvn_EventType_KeyTyped,
	Lvn_EventType_WindowClose,
	Lvn_EventType_WindowResize,
	Lvn_EventType_WindowFramebufferResize,
	Lvn_EventType_WindowFocus,
	Lvn_EventType_WindowLostFocus,
	Lvn_EventType_WindowMoved,
	Lvn_EventType_MouseButtonPressed,
	Lvn_EventType_MouseButtonReleased,
	Lvn_EventType_MouseMoved,
	Lvn_EventType_MouseScrolled,
};

enum LvnEventCategory
{
	Lvn_EventCategory_Application	= (1 << 0),
	Lvn_EventCategory_Input			= (1 << 1),
	Lvn_EventCategory_Keyboard		= (1 << 2),
	Lvn_EventCategory_Mouse			= (1 << 3),
	Lvn_EventCategory_MouseButton	= (1 << 4),
	Lvn_EventCategory_Window		= (1 << 5),
};

enum LvnWindowApi
{
	Lvn_WindowApi_None = 0,
	Lvn_WindowApi_Glfw,
	Lvn_WindowApi_Win32,

	Lvn_WindowApi_glfw  = Lvn_WindowApi_Glfw,
	Lvn_WindowApi_win32 = Lvn_WindowApi_Win32,
	Lvn_WindowApi_GLFW  = Lvn_WindowApi_Glfw,
	Lvn_WindowApi_WIN32 = Lvn_WindowApi_Win32,
};

/* [Graphics Enums] */
enum LvnAttachmentType
{
	Lvn_AttachmentType_Color,
	Lvn_AttachmentType_Depth,
	Lvn_AttachmentType_Resolve,
};

enum LvnAttachmentLoadOperation
{
	Lvn_AttachmentLoadOp_Load,
	Lvn_AttachmentLoadOp_Clear,
	Lvn_AttachmentLoadOp_DontCare,
};

enum LvnAttachmentStoreOperation
{
	Lvn_AttachmentStoreOp_Store,
	Lvn_AttachmentStoreOp_DontCare,
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
	Lvn_ColorBlendFactor_Zero					= 0,
	Lvn_ColorBlendFactor_One					= 1,
	Lvn_ColorBlendFactor_SrcColor				= 2,
	Lvn_ColorBlendFactor_OneMinusSrcColor		= 3,
	Lvn_ColorBlendFactor_DstColor				= 4,
	Lvn_ColorBlendFactor_OneMinusDstColor		= 5,
	Lvn_ColorBlendFactor_SrcAlpha				= 6,
	Lvn_ColorBlendFactor_OneMinusSrcAlpha		= 7,
	Lvn_ColorBlendFactor_DstAlpha				= 8,
	Lvn_ColorBlendFactor_OneMinusDstAlpha		= 9,
	Lvn_ColorBlendFactor_ConstantColor			= 10,
	Lvn_ColorBlendFactor_OneMinusConstantColor	= 11,
	Lvn_ColorBlendFactor_ConstantAlpha			= 12,
	Lvn_ColorBlendFactor_OneMinusConstantAlpha	= 13,
	Lvn_ColorBlendFactor_SrcAlphaSaturate		= 14,
	Lvn_ColorBlendFactor_Src1Color				= 15,
	Lvn_ColorBlendFactor_OneMinusSrc1Color		= 16,
	Lvn_ColorBlendFactor_Src1_Alpha				= 17,
	Lvn_ColorBlendFactor_OneMinusSrc1Alpha		= 18,
};

enum LvnColorBlendOperation
{
	Lvn_ColorBlendOperation_Add					= 0,
	Lvn_ColorBlendOperation_Subtract			= 1,
	Lvn_ColorBlendOperation_ReverseSubtract		= 2,
	Lvn_ColorBlendOperation_Min					= 3,
	Lvn_ColorBlendOperation_Max					= 4,
	Lvn_ColorBlendOperation_ZeroExt				= 1000148000,
	Lvn_ColorBlendOperation_SrcExt				= 1000148001,
	Lvn_ColorBlendOperation_DstExt				= 1000148002,
	Lvn_ColorBlendOperation_SrcOverExt			= 1000148003,
	Lvn_ColorBlendOperation_DstOverExt			= 1000148004,
	Lvn_ColorBlendOperation_SrcInExt			= 1000148005,
	Lvn_ColorBlendOperation_DstInExt			= 1000148006,
	Lvn_ColorBlendOperation_SrcOutExt			= 1000148007,
	Lvn_ColorBlendOperation_DstOutExt			= 1000148008,
	Lvn_ColorBlendOperation_SrcAtopExt			= 1000148009,
	Lvn_ColorBlendOperation_DstAtopExt			= 1000148010,
	Lvn_ColorBlendOperation_XorExt				= 1000148011,
	Lvn_ColorBlendOperation_MultiplyExt			= 1000148012,
	Lvn_ColorBlendOperation_ScreenExt			= 1000148013,
	Lvn_ColorBlendOperation_OverlayExt			= 1000148014,
	Lvn_ColorBlendOperation_DarkenExt			= 1000148015,
	Lvn_ColorBlendOperation_LightenExt			= 1000148016,
	Lvn_ColorBlendOperation_ColorDodgeExt		= 1000148017,
	Lvn_ColorBlendOperation_ColorBurnExt		= 1000148018,
	Lvn_ColorBlendOperation_HardLightExt		= 1000148019,
	Lvn_ColorBlendOperation_SoftLightExt		= 1000148020,
	Lvn_ColorBlendOperation_DifferenceExt		= 1000148021,
	Lvn_ColorBlendOperation_ExclusionExt		= 1000148022,
	Lvn_ColorBlendOperation_InvertExt			= 1000148023,
	Lvn_ColorBlendOperation_InvertRGBExt		= 1000148024,
	Lvn_ColorBlendOperation_LinearDodgeExt		= 1000148025,
	Lvn_ColorBlendOperation_LinearBurnExt		= 1000148026,
	Lvn_ColorBlendOperation_VividLightExt		= 1000148027,
	Lvn_ColorBlendOperation_LinearLightExt		= 1000148028,
	Lvn_ColorBlendOperation_PinLightExt			= 1000148029,
	Lvn_ColorBlendOperation_HardMixExt			= 1000148030,
	Lvn_ColorBlendOperation_HSLHueExt			= 1000148031,
	Lvn_ColorBlendOperation_HSLSaturationExt	= 1000148032,
	Lvn_ColorBlendOperation_HSLColorExt			= 1000148033,
	Lvn_ColorBlendOperation_HSLLuminosityExt	= 1000148034,
	Lvn_ColorBlendOperation_PlusExt				= 1000148035,
	Lvn_ColorBlendOperation_PlusClampedExt		= 1000148036,
	Lvn_ColorBlendOperation_PlusClampedAlphaExt	= 1000148037,
	Lvn_ColorBlendOperation_PlusDarkerExt		= 1000148038,
	Lvn_ColorBlendOperation_MinusExt			= 1000148039,
	Lvn_ColorBlendOperation_MinusClampedExt		= 1000148040,
	Lvn_ColorBlendOperation_ContrastExt			= 1000148041,
	Lvn_ColorBlendOperation_InvertOVGExt		= 1000148042,
	Lvn_ColorBlendOperation_RedExt				= 1000148043,
	Lvn_ColorBlendOperation_GreenExt			= 1000148044,
	Lvn_ColorBlendOperation_BlueExt				= 1000148045,
};

enum LvnCompareOperation
{
	Lvn_CompareOperation_Never			= 0,
	Lvn_CompareOperation_Less			= 1,
	Lvn_CompareOperation_Equal			= 2,
	Lvn_CompareOperation_LessOrEqual	= 3,
	Lvn_CompareOperation_Greater		= 4,
	Lvn_CompareOperation_NotEqual		= 5,
	Lvn_CompareOperation_GreaterOrEqual = 6,
	Lvn_CompareOperation_Always			= 7,
};

enum LvnImageFormat
{
	Lvn_ImageFormat_None = 0,
	Lvn_ImageFormat_RGB,
	Lvn_ImageFormat_RGBA,
	Lvn_ImageFormat_RGBA8,
	Lvn_ImageFormat_RGBA16F,
	Lvn_ImageFormat_RGBA32F,
	Lvn_ImageFormat_RedInt,
	Lvn_ImageFormat_DepthComponent,
	Lvn_ImageFormat_Depth24Stencil8,
};

enum LvnGraphicsApi
{
	Lvn_GraphicsApi_None = 0,
	Lvn_GraphicsApi_OpenGL,
	Lvn_GraphicsApi_Vulkan,

	Lvn_GraphicsApi_opengl = Lvn_GraphicsApi_OpenGL,
	Lvn_GraphicsApi_vulkan = Lvn_GraphicsApi_Vulkan,
};

enum LvnImageLayout
{
	Lvn_ImageLayout_Undefined,
	Lvn_ImageLayout_Present,
	Lvn_ImageLayout_ColorAttachment,
	Lvn_ImageLayout_DepthStencilAttachment,
};

enum LvnPhysicalDeviceType
{
	Lvn_PhysicalDeviceType_Other = 0,
	Lvn_PhysicalDeviceType_Integrated_GPU = 1,
	Lvn_PhysicalDeviceType_Discrete_GPU = 2,
	Lvn_PhysicalDeviceType_Virtual_GPU = 3,
	Lvn_PhysicalDeviceType_CPU = 4,

	Lvn_PhysicalDeviceType_Unknown = Lvn_PhysicalDeviceType_Other,
};

enum LvnSampleCount
{
	Lvn_SampleCount_1_Bit = 0x00000001,
	Lvn_SampleCount_2_Bit = 0x00000002,
	Lvn_SampleCount_4_Bit = 0x00000004,
	Lvn_SampleCount_8_Bit = 0x00000008,
	Lvn_SampleCount_16_Bit = 0x00000010,
	Lvn_SampleCount_32_Bit = 0x00000020,
	Lvn_SampleCount_64_Bit = 0x00000040,
	Lvn_SampleCount_Max_Bit = 0x7FFFFFFF
};

enum LvnStencilOperation
{
	Lvn_StencilOperation_Keep = 0,
	Lvn_StencilOperation_Zero = 1,
	Lvn_StencilOperation_Replace = 2,
	Lvn_StencilOperation_IncrementAndClamp = 3,
	Lvn_StencilOperation_DecrementAndClamp = 4,
	Lvn_StencilOperation_Invert = 5,
	Lvn_StencilOperation_IncrementAndWrap = 6,
	Lvn_StencilOperation_DecrementAndWrap = 7,
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

enum LvnVertexDataType
{
	Lvn_VertexDataType_None = 0,
	Lvn_VertexDataType_Float,
	Lvn_VertexDataType_Double,
	Lvn_VertexDataType_Int,
	Lvn_VertexDataType_UnsignedInt,
	Lvn_VertexDataType_Bool,
	Lvn_VertexDataType_Vec2,
	Lvn_VertexDataType_Vec3,
	Lvn_VertexDataType_Vec4,
	Lvn_VertexDataType_Vec2i,
	Lvn_VertexDataType_Vec3i,
	Lvn_VertexDataType_Vec4i,
	Lvn_VertexDataType_Vec2ui,
	Lvn_VertexDataType_Vec3ui,
	Lvn_VertexDataType_Vec4ui,
	Lvn_VertexDataType_Vec2d,
	Lvn_VertexDataType_Vec3d,
	Lvn_VertexDataType_Vec4d,
	Lvn_VertexDataType_Vec2f = Lvn_VertexDataType_Vec2,
	Lvn_VertexDataType_Vec3f = Lvn_VertexDataType_Vec3,
	Lvn_VertexDataType_Vec4f = Lvn_VertexDataType_Vec4,
};

struct LvnAppRenderEvent;
struct LvnAppTickEvent;
struct LvnBatchCreateInfo;
struct LvnBatchRenderer;
struct LvnCharset;
struct LvnContext;
struct LvnContextCreateInfo;
struct LvnCubemap;
struct LvnCubemapCreateInfo;
struct LvnDrawCommand;
struct LvnDrawList;
struct LvnEvent;
struct LvnFont;
struct LvnFontGlyph;
struct LvnFontMetrics;
struct LvnFrameBuffer;
struct LvnFrameBufferCreateInfo;
struct LvnGraphicsContext;
struct LvnKeyHoldEvent;
struct LvnKeyPressedEvent;
struct LvnKeyReleasedEvent;
struct LvnKeyTypedEvent;
struct LvnLogMessage;
struct LvnLogPattern;
struct LvnLogger;
struct LvnMemoryBlock;
struct LvnMemoryPool;
struct LvnMemoryPoolCreateInfo;
struct LvnMouseButtonPressedEvent;
struct LvnMouseButtonReleasedEvent;
struct LvnMouseMovedEvent;
struct LvnMouseScrolledEvent;
struct LvnOrthographicCamera;
struct LvnPhysicalDevice;
struct LvnPhysicalDeviceInfo;
struct LvnPipeline;
struct LvnPipelineColorBlend;
struct LvnPipelineColorBlendAttachment;
struct LvnPipelineColorWriteMask;
struct LvnPipelineCreateInfo;
struct LvnPipelineDepthStencil;
struct LvnPipelineInputAssembly;
struct LvnPipelineMultiSampling;
struct LvnPipelineRasterizer;
struct LvnPipelineScissor;
struct LvnPipelineSpecification;
struct LvnPipelineStencilAttachment;
struct LvnPipelineViewport;
struct LvnRendererBackends;
struct LvnRenderPass;
struct LvnRenderPassAttachment;
struct LvnRenderPassCreateInfo;
struct LvnShader;
struct LvnShaderCreateInfo;
struct LvnVertexArrayBuffer;
struct LvnVertexArrayBufferCreateInfo;
struct LvnVertexAttribute;
struct LvnVertexBindingDescription;
struct LvnWindow;
struct LvnWindowCloseEvent;
struct LvnWindowContext;
struct LvnWindowCreateInfo;
struct LvnWindowData;
struct LvnWindowDimensions;
struct LvnWindowEvent;
struct LvnWindowFocusEvent;
struct LvnWindowFramebufferResizeEvent;
struct LvnWindowIconData;
struct LvnWindowLostFocusEvent;
struct LvnWindowMovedEvent;
struct LvnWindowResizeEvent;


/* [Vectors] */
template<typename T>
struct LvnVec2_t;
template<typename T>
struct LvnVec3_t;
template<typename T>
struct LvnVec4_t;

/* [Matricies] */
template<typename T>
struct LvnMat2x2_t;
template<typename T>
struct LvnMat3x3_t;
template<typename T>
struct LvnMat4x4_t;
template<typename T>
struct LvnMat2x3_t;
template<typename T>
struct LvnMat2x4_t;
template<typename T>
struct LvnMat3x2_t;
template<typename T>
struct LvnMat3x4_t;
template<typename T>
struct LvnMat4x2_t;
template<typename T>
struct LvnMat4x3_t;


/* Functions */
namespace lvn
{
	typedef LvnVec2_t<float>				vec2;
	typedef LvnVec3_t<float>				vec3;
	typedef LvnVec4_t<float>				vec4;
	typedef LvnVec2_t<int>					vec2i;
	typedef LvnVec3_t<int>					vec3i;
	typedef LvnVec4_t<int>					vec4i;
	typedef LvnVec2_t<unsigned int>			vec2ui;
	typedef LvnVec3_t<unsigned int>			vec3ui;
	typedef LvnVec4_t<unsigned int>			vec4ui;
	typedef LvnVec2_t<float>				vec2f;
	typedef LvnVec3_t<float>				vec3f;
	typedef LvnVec4_t<float>				vec4f;
	typedef LvnVec2_t<double>				vec2d;
	typedef LvnVec3_t<double>				vec3d;
	typedef LvnVec4_t<double>				vec4d;
	typedef LvnVec2_t<bool>					vec2b;
	typedef LvnVec3_t<bool>					vec3b;
	typedef LvnVec4_t<bool>					vec4b;

	typedef LvnMat2x2_t<float>				mat2;
	typedef LvnMat3x3_t<float>				mat3;
	typedef LvnMat4x4_t<float>				mat4;
	typedef LvnMat2x3_t<float>				mat2x3;
	typedef LvnMat2x4_t<float>				mat2x4;
	typedef LvnMat3x2_t<float>				mat3x2;
	typedef LvnMat3x4_t<float>				mat3x4;
	typedef LvnMat4x2_t<float>				mat4x2;
	typedef LvnMat4x3_t<float>				mat4x3;
	typedef LvnMat2x2_t<int>				mat2i;
	typedef LvnMat3x3_t<int>				mat3i;
	typedef LvnMat4x4_t<int>				mat4i;
	typedef LvnMat2x3_t<int>				mat2x3i;
	typedef LvnMat2x4_t<int>				mat2x4i;
	typedef LvnMat3x2_t<int>				mat3x2i;
	typedef LvnMat3x4_t<int>				mat3x4i;
	typedef LvnMat4x2_t<int>				mat4x2i;
	typedef LvnMat4x3_t<int>				mat4x3i;
	typedef LvnMat2x2_t<unsigned int>		mat2ui;
	typedef LvnMat3x3_t<unsigned int>		mat3ui;
	typedef LvnMat4x4_t<unsigned int>		mat4ui;
	typedef LvnMat2x3_t<unsigned int>		mat2x3ui;
	typedef LvnMat2x4_t<unsigned int>		mat2x4ui;
	typedef LvnMat3x2_t<unsigned int>		mat3x2ui;
	typedef LvnMat3x4_t<unsigned int>		mat3x4ui;
	typedef LvnMat4x2_t<unsigned int>		mat4x2ui;
	typedef LvnMat4x3_t<unsigned int>		mat4x3ui;
	typedef LvnMat2x2_t<float>				mat2f;
	typedef LvnMat3x3_t<float>				mat3f;
	typedef LvnMat4x4_t<float>				mat4f;
	typedef LvnMat2x3_t<float>				mat2x3f;
	typedef LvnMat2x4_t<float>				mat2x4f;
	typedef LvnMat3x2_t<float>				mat3x2f;
	typedef LvnMat3x4_t<float>				mat3x4f;
	typedef LvnMat4x2_t<float>				mat4x2f;
	typedef LvnMat4x3_t<float>				mat4x3f;
	typedef LvnMat2x2_t<double>				mat2d;
	typedef LvnMat3x3_t<double>				mat3d;
	typedef LvnMat4x4_t<double>				mat4d;
	typedef LvnMat2x3_t<double>				mat2x3d;
	typedef LvnMat2x4_t<double>				mat2x4d;
	typedef LvnMat3x2_t<double>				mat3x2d;
	typedef LvnMat3x4_t<double>				mat3x4d;
	typedef LvnMat4x2_t<double>				mat4x2d;
	typedef LvnMat4x3_t<double>				mat4x3d;


	/* [API] */
	LVN_API LvnResult				createContext(LvnContextCreateInfo* createInfo);
	LVN_API void					terminateContext();

	LVN_API int						getDateYear();
	LVN_API int						getDateYear02d();
	LVN_API int						getDateMonth();
	LVN_API int						getDateDay();
	LVN_API int						getDateHour();
	LVN_API int						getDateHour12();
	LVN_API int						getDateMinute();
	LVN_API int						getDateSecond();
	LVN_API long long				getSecondsSinceEpoch();

	LVN_API void*					memAlloc(size_t size);
	LVN_API void					memFree(void* ptr);

	/* [Logging] */

	// Logging is used to display info or error messages
	// The Levikno library has two loggers by default, core and client logger. 
	// The core logger is used to output information relevant to the library (eg. error messages, object creation)
	// The client logger is used for the implementation of the library for the application or engine (if the user wishes to use it)

	// Each Logger has a name, log level, and log patterns
	// - the log level tells the logger what type of messages to output, the logger will only output messages with the current log level set and higher
	// - log levels under the current level set in the logger will be ignored
	// - log patterns are used to format the message (eg. time, color)
	// - each log pattern starts with a '%' followed by a character
	// 
	// List of default log pattens:
	//  - '$' = end of log message; new line "\n"
	//  - 'n' = the name of the logger
	//  - 'l' = log level of message (eg. "info", "error")
	//  - '#' = start color range based on log level (ANSI code color)
	//  - '^' = end of color range (ANSI code reset)
	//  - 'v' = the actual message to display
	//  - '%' = display percent sign '%'; log pattern would be "%%"
	//  - 'T' = get the time in 24 hour HH:MM:SS format
	//  - 't' = get the time in 12 hour HH:MM:SS format
	//  - 'Y' = get the year in 4 digits (eg. 2025)
	//  - 'y' = get the year in 2 digits (eg. 25)
	//  - 'm' = get the month number from (1-12)
	//  - 'B' = get the name of the month (eg. April)
	//  - 'b' = get the name of the month shortened (eg. Apr)
	//  - 'd' = get the day number in month from (1-31)
	//  - 'A' = get the name of day in week (eg. Monday)
	//  - 'a' = get the name of day in week shortened (eg. Mon)
	//  - 'H' = get hour of day (0-23)
	//  - 'h' = get hour of day (1-12)
	//  - 'M' = get minute (0-59)
	//  - 'S' = get second (0-59)
	//  - 'P' = get the time meridiem (AM/PM)
	//  - 'p' = get the time meridiem in lower (am/pm)
	// 
	// Ex: The default log pattern is: "[%d-%m-%Y] [%T] [%#%l%^] %n: %v%$"
	//     Which could output: "[04-06-2025] [14:25:11] [\x1b[0;32minfo\x1b[0m] CORE: some informational message\n"

	LVN_API LvnResult					logInit(); /* Initiates logging */
	LVN_API void						logTerminate();
	LVN_API void						logSetLevel(LvnLogger* logger, LvnLogLevel level);					/* sets the log level of logger, will only print messages with set log level and higher */
	LVN_API bool						logCheckLevel(LvnLogger* logger, LvnLogLevel level);				/* checks level with loger, returns true if level is the same or higher level than the level of the logger */
	LVN_API void						logRenameLogger(LvnLogger* logger, const char* name);				/* renames the name of the logger */
	LVN_API void						logOutputMessage(LvnLogger* logger, LvnLogMessage* msg);			/* prints the log message */
	LVN_API void						logMessage(LvnLogger* logger, LvnLogLevel level, const char* msg);	/* log message with given log level */
	LVN_API void						logMessageTrace(LvnLogger* logger, const char* fmt, ...);			/* log message with level trace; ANSI code "\x1b[1;37m" */
	LVN_API void						logMessageInfo(LvnLogger* logger, const char* fmt, ...);			/* log message with level info; ANSI code "\x1b[0;32m" */
	LVN_API void						logMessageWarn(LvnLogger* logger, const char* fmt, ...);			/* log message with level warn; ANSI code "\x1b[1;33m" */
	LVN_API void						logMessageError(LvnLogger* logger, const char* fmt, ...);			/* log message with level error; ANSI code "\x1b[1;31m" */
	LVN_API void						logMessageCritical(LvnLogger* logger, const char* fmt, ...);		/* log message with level critical; ANSI code "\x1b[1;37;41m" */
	LVN_API LvnLogger*					getCoreLogger();													
	LVN_API LvnLogger*					getClientLogger();													
	LVN_API const char*					getLogANSIcodeColor(LvnLogLevel level);								/* get the ANSI color code of the log level in a string */
	LVN_API void						logSetPatternFormat(LvnLogger* logger, const char* patternfmt);		/* set the log pattern of the logger; messages outputed from that logger will be in this format */
	LVN_API void						logAddPattern();


	/* [Events] */
	// Use these function within the call back function of LvnWindow (if it is set)
	LVN_API bool						dispatchLvnAppRenderEvent(LvnEvent* event, bool(*func)(LvnAppRenderEvent*));
	LVN_API bool						dispatchLvnAppTickEvent(LvnEvent* event, bool(*func)(LvnAppTickEvent*));
	LVN_API bool						dispatchKeyHoldEvent(LvnEvent* event, bool(*func)(LvnKeyHoldEvent*));
	LVN_API bool						dispatchKeyPressedEvent(LvnEvent* event, bool(*func)(LvnKeyPressedEvent*));
	LVN_API bool						dispatchKeyReleasedEvent(LvnEvent* event, bool(*func)(LvnKeyReleasedEvent*));
	LVN_API bool						dispatchKeyTypedEvent(LvnEvent* event, bool(*func)(LvnKeyTypedEvent*));
	LVN_API bool						dispatchMouseButtonPressedEvent(LvnEvent* event, bool(*func)(LvnMouseButtonPressedEvent*));
	LVN_API bool						dispatchMouseButtonReleasedEvent(LvnEvent* event, bool(*func)(LvnMouseButtonReleasedEvent*));
	LVN_API bool						dispatchMouseMovedEvent(LvnEvent* event, bool(*func)(LvnMouseMovedEvent*));
	LVN_API bool						dispatchMouseScrolledEvent(LvnEvent* event, bool(*func)(LvnMouseScrolledEvent*));
	LVN_API bool						dispatchWindowCloseEvent(LvnEvent* event, bool(*func)(LvnWindowCloseEvent*));
	LVN_API bool						dispatchWindowFramebufferResizeEvent(LvnEvent* event, bool(*func)(LvnWindowFramebufferResizeEvent*));
	LVN_API bool						dispatchWindowFocusEvent(LvnEvent* event, bool(*func)(LvnWindowFocusEvent*));
	LVN_API bool						dispatchWindowLostFocusEvent(LvnEvent* event, bool(*func)(LvnWindowLostFocusEvent*));
	LVN_API bool						dispatchWindowMovedEvent(LvnEvent* event, bool(*func)(LvnWindowMovedEvent*));
	LVN_API bool						dispatchWindowResizeEvent(LvnEvent* event, bool(*func)(LvnWindowResizeEvent*));

	/* [Window] */
	LVN_API LvnWindowApi				getWindowApi();
	LVN_API const char*					getWindowApiName();

	LVN_API LvnResult					createWindow(LvnWindow** window, LvnWindowCreateInfo* createInfo);
	LVN_API void						destroyWindow(LvnWindow* window);

	LVN_API void						updateWindow(LvnWindow* window);
	LVN_API bool						windowOpen(LvnWindow* window);
	LVN_API LvnWindowDimensions			getWindowDimensions(LvnWindow* window);
	LVN_API int							getWindowWidth(LvnWindow* window);
	LVN_API int							getWindowHeight(LvnWindow* window);
	LVN_API void						setWindowEventCallback(LvnWindow* window, void (*callback)(LvnEvent*));
	LVN_API void						setWindowVSync(LvnWindow* window, bool enable);
	LVN_API bool						getWindowVSync(LvnWindow* window);
	LVN_API void*						getNativeWindow(LvnWindow* window);
	LVN_API void						setWindowContextCurrent(LvnWindow* window);



	/* [Graphics API] */
	LVN_API LvnGraphicsApi				getGraphicsApi();
	LVN_API const char*					getGraphicsApiName();
	LVN_API void						getPhysicalDevices(LvnPhysicalDevice** ppPhysicalDevices, uint32_t* deviceCount);
	LVN_API LvnPhysicalDeviceInfo		getPhysicalDeviceInfo(LvnPhysicalDevice* physicalDevice);
	LVN_API LvnResult					renderInit(LvnRendererBackends* renderBackends);

	LVN_API void						renderCmdDraw(LvnWindow* window, uint32_t vertexCount);
	LVN_API void						renderCmdDrawIndexed(uint32_t indexCount);
	LVN_API void						renderCmdDrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
	LVN_API void						renderCmdDrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
	LVN_API void						renderCmdSetStencilReference(uint32_t reference);
	LVN_API void						renderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask);
	LVN_API void						renderBeginNextFrame(LvnWindow* window);																		// begins the next frame of the window
	LVN_API void						renderDrawSubmit(LvnWindow* window);																			// submits all draw commands recorded and presents to window
	LVN_API void						renderBeginCommandRecording(LvnWindow* window);																	// begins command buffer when recording draw commands start
	LVN_API void						renderEndCommandRecording(LvnWindow* window);																	// ends command buffer when finished recording draw commands
	LVN_API void						renderCmdBeginRenderPass(LvnWindow* window);																	// begins renderpass when rendering starts
	LVN_API void						renderCmdEndRenderPass(LvnWindow* window);																		// ends renderpass when rendering has finished
	LVN_API void						renderCmdBindPipeline(LvnWindow* window, LvnPipeline* pipeline);												// bind a pipeline to begin shading during rendering

	LVN_API LvnResult					createRenderPass(LvnRenderPass** renderPass, LvnRenderPassCreateInfo* createInfo);								// create renderpass for rendering draw commands
	LVN_API LvnResult					createShaderFromSrc(LvnShader** shader, LvnShaderCreateInfo* createInfo);										// create shader with the source code as input
	LVN_API LvnResult					createShaderFromFileSrc(LvnShader** shader, LvnShaderCreateInfo* createInfo);									// create shader with the file paths to the source files as input
	LVN_API LvnResult					createShaderFromFileBin(LvnShader** shader, LvnShaderCreateInfo* createInfo);									// create shader with the file paths to the binary files (.spv) as input
	LVN_API LvnResult					createPipeline(LvnPipeline** pipeline, LvnPipelineCreateInfo* createInfo);										// create pipeline to describe shading specifications
	LVN_API LvnResult					createFrameBuffer(LvnFrameBuffer** frameBuffer, LvnFrameBufferCreateInfo* createInfo);							// create framebuffer to render images to
	LVN_API LvnResult					createVertexArrayBuffer(LvnVertexArrayBuffer** vertexArrayBuffer, LvnVertexArrayBufferCreateInfo* createInfo);	// create a single buffer object that can hold both the vertex and index buffers

	LVN_API void						destroyRenderPass(LvnRenderPass* renderPass);																	// destroy renderpass object
	LVN_API void						destroyShader(LvnShader* shader);
	LVN_API void						destroyPipeline(LvnPipeline* pipeline);																			// destroy pipeline object
	LVN_API void						destroyFrameBuffer(LvnFrameBuffer* frameBuffer);																// destroy framebuffer object
	LVN_API void						destroyVertexArrayBuffer(LvnVertexArrayBuffer* vertexArrayBuffer);												// destory vertex buffers object

	LVN_API void						setDefaultPipelineSpecification(LvnPipelineSpecification* pipelineSpecification);
	LVN_API LvnPipelineSpecification	getDefaultPipelineSpecification();



	/* [Math] */
	template <typename T>
	LVN_API T						min(const T& n1, const T& n2) { return n1 < n2 ? n1 : n2; }

	template <typename T>
	LVN_API T						max(const T& n1, const T& n2) { return n1 > n2 ? n1 : n2; }

	template <typename T>
	LVN_API T						clamp(const T& val, const T& low, const T& high) { return lvn::max(lvn::min(val, high), low); }

}


/* [Core Struct Implementaion] */

struct LvnContextCreateInfo
{
	LvnWindowApi		windowapi;
	LvnGraphicsApi		graphicsapi;
	bool				enableLogging;
	bool				enableVulkanValidationLayers;
};

struct LvnMemoryBlock
{
	LvnStructureType sType;
	LvnMemoryBlockUsage usage;
	uint32_t numAllocations;
	uint32_t numAdvanceAllocations;
};

struct LvnMemoryPoolCreateInfo
{
	LvnMemoryBlock* pMemoryBlocks;
	uint32_t memoryBlockCount;
};

/* [Logging] */
struct LvnLogMessage
{
	const char *msg, *loggerName;
	LvnLogLevel level;
	long long timeEpoch;
};


/* [Events] */
struct LvnAppRenderEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;
};

struct LvnAppTickEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;
};

struct LvnKeyHoldEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;

	int keyCode;
	bool repeat;
};

struct LvnKeyPressedEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;

	int keyCode;
};

struct LvnKeyReleasedEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;

	int keyCode;
};

struct LvnKeyTypedEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;

	unsigned int key;
};

struct LvnMouseButtonPressedEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;

	int buttonCode;
};

struct LvnMouseButtonReleasedEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;

	int buttonCode;
};

struct LvnMouseMovedEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;

	int x, y;
};

struct LvnMouseScrolledEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;

	float x, y;
};

struct LvnWindowCloseEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;
};

struct LvnWindowFocusEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;
};

struct LvnWindowFramebufferResizeEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;

	unsigned int width, height;
};

struct LvnWindowLostFocusEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;
};

struct LvnWindowMovedEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;

	int x, y;
};

struct LvnWindowResizeEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;

	int width, height;
};

/* [Window] */
struct LvnWindowIconData
{
	unsigned char* image;
	int width, height;
};

struct LvnWindowCreateInfo
{
	int width, height;					/* width and height of window */
	const char* title;					/* title of window */
	int minWidth, minHeight;			/* minimum width and height of window (set to 0 if not specified) */
	int maxWidth, maxHeight;			/* maximum width and height of window (set to -1 if not specified) */
	bool fullscreen, resizable, vSync;	/* sets window to fullscreen if true; enables window resizing if true; vSync controls window framerate, sets framerate to 60fps if true */
	LvnWindowIconData* pIcons;			/* icon images used for window/app icon; pIcons can be stored in an array; pIcons will be ignored if set to null */
	uint32_t iconCount;					/* iconCount is the number of icons in pIcons; if using only one icon, set iconCount to 1; if using an array of icons, set to length of array */

	LvnWindowCreateInfo()
	{
		width = 0; height = 0; title = nullptr;
		minWidth = 0; minHeight = 0; maxWidth = -1; maxHeight = -1;
		resizable = true; vSync = false;
		pIcons = nullptr;
		iconCount = 0;
	}
};

struct LvnWindowDimensions
{
	int width, height;
};



/* [Graphics Struct Implementation] */


/* [Data Types] */

template<typename T>
struct LvnVec2_t
{
	union { T x, r, s; };
	union { T y, g, t; };

	LvnVec2_t() {}
	LvnVec2_t(const T& n)
		: x(n), y(n) {}
	LvnVec2_t(const T& nx, const T& ny)
		: x(nx), y(ny) {}
	LvnVec2_t(const LvnVec3_t<T>& v)
	{
		this->x = v.x;
		this->y = v.y;
	}
	LvnVec2_t(const LvnVec4_t<T>& v)
	{
		this->x = v.x;
		this->y = v.y;
	}

	LvnVec2_t<T> operator+()
	{
		return { this->x, this->y };
	}
	LvnVec2_t<T> operator-()
	{
		return { -this->x, -this->y };
	}
	LvnVec2_t<T> operator+(const LvnVec2_t<T>& v)
	{
		return { this->x + v.x, this->y + v.y };
	}
	LvnVec2_t<T> operator-(const LvnVec2_t<T>& v)
	{
		return { this->x - v.x, this->y - v.y };
	}
	LvnVec2_t<T> operator*(const LvnVec2_t<T>& v)
	{
		return { this->x * v.x, this->y * v.y };
	}
	LvnVec2_t<T> operator/(const LvnVec2_t<T>& v)
	{
		return { this->x / v.x, this->y / v.y };
	}
	LvnVec2_t<T>& operator+=(const LvnVec2_t<T>& v)
	{
		this->x += v.x;
		this->y += v.y;
		return *this;
	}
	LvnVec2_t<T>& operator-=(const LvnVec2_t<T>& v)
	{
		this->x -= v.x;
		this->y -= v.y;
		return *this;
	}
	LvnVec2_t<T>& operator*=(const LvnVec2_t<T>& v)
	{
		this->x *= v.x;
		this->y *= v.y;
		return *this;
	}
	LvnVec2_t<T>& operator/=(const LvnVec2_t<T>& v)
	{
		this->x /= v.x;
		this->y /= v.y;
		return *this;
	}
	T& operator[](int i)
	{
		switch (i)
		{
		default:
		case 0:
			return x;
		case 1:
			return y;
		}
	}
};

template<typename T>
struct LvnVec3_t
{
	union { T x, r, s; };
	union { T y, g, t; };
	union { T z, b, p; };

	LvnVec3_t() {}
	LvnVec3_t(const T& n)
		: x(n), y(n), z(n) {}
	LvnVec3_t(const T& nx, const T& ny, const T& nz)
		: x(nx), y(ny), z(nz) {}
	LvnVec3_t(const T& n_x, const LvnVec2_t<T>& n_yz)
		: x(n_x), y(n_yz.y), z(n_yz.z) {}
	LvnVec3_t(const LvnVec2_t<T>& n_xy, const T& n_z)
		: x(n_xy.x), y(n_xy.y), z(n_z) {}
	LvnVec3_t(const LvnVec4_t<T>& v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
	}

	LvnVec3_t<T> operator+()
	{
		return { this->x, this->y, this->z };
	}
	LvnVec3_t<T> operator-()
	{
		return { -this->x, -this->y, -this->z };
	}
	LvnVec3_t<T> operator+(const LvnVec3_t<T>& v)
	{
		return { this->x + v.x, this->y + v.y, this->z + v.z };
	}
	LvnVec3_t<T> operator-(const LvnVec3_t<T>& v)
	{
		return { this->x - v.x, this->y - v.y, this->z - v.z };
	}
	LvnVec3_t<T> operator*(const LvnVec3_t<T>& v)
	{
		return { this->x * v.x, this->y * v.y, this->z * v.z };
	}
	LvnVec3_t<T> operator/(const LvnVec3_t<T>& v)
	{
		return { this->x / v.x, this->y / v.y, this->z / v.z };
	}
	LvnVec3_t<T>& operator+=(const LvnVec3_t<T>& v)
	{
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		return *this;
	}
	LvnVec3_t<T>& operator-=(const LvnVec3_t<T>& v)
	{
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
		return *this;
	}
	LvnVec3_t<T>& operator*=(const LvnVec3_t<T>& v)
	{
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;
		return *this;
	}
	LvnVec3_t<T>& operator/=(const LvnVec3_t<T>& v)
	{
		this->x /= v.x;
		this->y /= v.y;
		this->z /= v.z;
		return *this;
	}
	T& operator[](int i)
	{
		switch (i)
		{
		default:
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return y;
		}
	}
};

template<typename T>
struct LvnVec4_t
{
	union { T x, r, s; };
	union { T y, g, t; };
	union { T z, b, p; };
	union { T w, a, q; };

	LvnVec4_t() {}
	LvnVec4_t(const T& n)
		: x(n), y(n), z(n), w(n) {}
	LvnVec4_t(const T& nx, const T& ny, const T& nz, const T& nw)
		: x(nx), y(ny), z(nz), w(nw) {}
	LvnVec4_t(const T& n_x, const T& n_y, const LvnVec2_t<T>& n_zw)
		: x(n_x), y(n_y), z(n_zw.z), w(n_zw.w) {}
	LvnVec4_t(const LvnVec2_t<T>& n_xy, const T& n_z, const T& n_w)
		: x(n_xy.x), y(n_xy.y), z(n_z), w(n_w) {}
	LvnVec4_t(const T& n_x, const LvnVec2_t<T>& n_yz, const T& n_w)
		: x(n_x), y(n_yz.y), z(n_yz.z), w(n_w) {}
	LvnVec4_t(const LvnVec3_t<T>& n_xyz, const T& n_w)
		: x(n_xyz.x), y(n_xyz.y), z(n_xyz.z), w(n_w) {}
	LvnVec4_t(const T& n_x, const LvnVec3_t<T>& n_yzw)
		: x(n_x), y(n_yzw.y), z(n_yzw.z), w(n_yzw.w) {}

	LvnVec4_t<T> operator+()
	{
		return { this->x, this->y, this->z, this->w };
	}
	LvnVec4_t<T> operator-()
	{
		return { -this->x, -this->y, -this->z, -this->w };
	}
	LvnVec4_t<T> operator+(const LvnVec4_t<T>& v)
	{
		return { this->x + v.x, this->y + v.y, this->z + v.z, this->w + v.w };
	}
	LvnVec4_t<T> operator-(const LvnVec4_t<T>& v)
	{
		return { this->x - v.x, this->y - v.y, this->z - v.z, this->w - v.w };
	}
	LvnVec4_t<T> operator*(const LvnVec4_t<T>& v)
	{
		return { this->x * v.x, this->y * v.y, this->z * v.z, this->w * v.w };
	}
	LvnVec4_t<T> operator/(const LvnVec4_t<T>& v)
	{
		return { this->x / v.x, this->y / v.y, this->z / v.z, this->w / v.w };
	}
	LvnVec4_t<T>& operator+=(const LvnVec4_t<T>& v)
	{
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		this->w += v.w;
		return *this;
	}
	LvnVec4_t<T>& operator-=(const LvnVec4_t<T>& v)
	{
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
		this->w -= v.w;
		return *this;
	}
	LvnVec4_t<T>& operator*=(const LvnVec4_t<T>& v)
	{
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;
		this->w *= v.w;
		return *this;
	}
	LvnVec4_t<T>& operator/=(const LvnVec4_t<T>& v)
	{
		this->x /= v.x;
		this->y /= v.y;
		this->z /= v.z;
		this->w /= v.w;
		return *this;
	}
	T& operator[](int i)
	{
		switch (i)
		{
		default:
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return y;
		case 3:
			return w;
		}
	}
};


template<typename T>
struct LvnMat2x2_t
{
	LvnVec2_t<T> value[2];

	static int length() { return 2; }

	LvnMat2x2_t()
	{
		this->value[0] = { 1, 0 };
		this->value[1] = { 0, 1 };
	}
	LvnMat2x2_t(const T& n)
	{
		this->value[0] = { n, 0 };
		this->value[1] = { 0, n };
	}
	LvnMat2x2_t
	(
		const T& x0, const T& y0,
		const T& x1, const T& y1
	)
	{
		this->value[0] = { x0, y0 };
		this->value[1] = { x1, y1 };
	}
	LvnMat2x2_t(const LvnVec2_t<T>& v0, const LvnVec2_t<T>& v1)
		: value{ v0, v1 } {}
	LvnMat2x2_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}
	LvnMat2x2_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}
	LvnMat2x2_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}
	LvnMat2x2_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}
	LvnMat2x2_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}
	LvnMat2x2_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}
	LvnMat2x2_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}
	LvnMat2x2_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}
	LvnMat2x2_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}

	LvnVec2_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat2x2_t<T> operator+()
	{
		return LvnMat2x2_t<T>(
			this->value[0],
			this->value[1]);
	}
	LvnMat2x2_t<T> operator-()
	{
		return LvnMat2x2_t<T>(
			-this->value[0],
			-this->value[1]);
	}
	LvnMat2x2_t<T> operator+(const LvnMat2x2_t<T>& m)
	{
		return LvnMat2x2_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1]);
	}
	LvnMat2x2_t<T> operator-(const LvnMat2x2_t<T>& m)
	{
		return LvnMat2x2_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1]);
	}
	LvnMat2x2_t<T> operator*(const T& n)
	{
		return LvnMat2x2_t<T>(
			this->value[0] * n,
			this->value[1] * n);
	}
	LvnMat2x2_t<T> operator*(const LvnMat2x2_t<T>& m)
	{
		return LvnMat2x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y);
	}
	LvnMat3x2_t<T> operator*(const LvnMat3x2_t<T>& m)
	{
		return LvnMat3x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y);
	}
	LvnMat4x2_t<T> operator*(const LvnMat4x2_t<T>& m)
	{
		return LvnMat4x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y);
	}
};

template<typename T>
LvnVec2_t<T> operator*(const LvnMat2x2_t<T>& m, const LvnVec2_t<T>& v)
{
	return LvnVec2_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y,
		m.value[0].y * v.x + m.value[1].y * v.y);
}
template<typename T>
LvnVec2_t<T> operator*(const LvnVec2_t<T>& v, const LvnMat2x2_t<T>& m)
{
	return LvnVec2_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y,
		v.x * m.value[1].x + v.y * m.value[1].y);
}

template<typename T>
struct LvnMat3x3_t
{
	LvnVec3_t<T> value[3];

	static int length() { return 3; }

	LvnMat3x3_t()
	{
		this->value[0] = { 1, 0, 0 };
		this->value[1] = { 0, 1, 0 };
		this->value[2] = { 0, 0, 1 };
	}
	LvnMat3x3_t(const T& n)
	{
		this->value[0] = { n, 0, 0 };
		this->value[1] = { 0, n, 0 };
		this->value[2] = { 0, 0, n };
	}
	LvnMat3x3_t
	(
		const T& x0, const T& y0, const T& z0,
		const T& x1, const T& y1, const T& z1,
		const T& x2, const T& y2, const T& z2
	)
	{
		this->value[0] = { x0, y0, z0 };
		this->value[1] = { x1, y1, z1 };
		this->value[2] = { x2, y2, z2 };
	}
	LvnMat3x3_t(const LvnVec3_t<T>& v0, const LvnVec3_t<T>& v1, const LvnVec3_t<T>& v2)
		: value{ v0, v1, v2 } {}
	LvnMat3x3_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0), LvnVec3_t<T>(0, 0, 1) } {}
	LvnMat3x3_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(m.value[2]) } {}
	LvnMat3x3_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(m.value[2]) } {}
	LvnMat3x3_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(0, 0, 1) } {}
	LvnMat3x3_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(0, 0, 1) } {}
	LvnMat3x3_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0), LvnVec3_t<T>(m.value[2], 1) } {}
	LvnMat3x3_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(m.value[2]) } {}
	LvnMat3x3_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0), LvnVec3_t<T>(m.value[2], 1) } {}
	LvnMat3x3_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(m.value[2]) } {}

	LvnVec3_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat3x3_t<T> operator+()
	{
		return LvnMat3x3_t<T>(
			this->value[0],
			this->value[1],
			this->value[2]);
	}
	LvnMat3x3_t<T> operator-()
	{
		return LvnMat3x3_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2]);
	}
	LvnMat3x3_t<T> operator+(const LvnMat3x3_t<T>& m)
	{
		return LvnMat3x3_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2]);
	}
	LvnMat3x3_t<T> operator-(const LvnMat3x3_t<T>& m)
	{
		return LvnMat3x3_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2]);
	}
	LvnMat3x3_t<T> operator*(const LvnMat3x3_t<T>& m)
	{
		return LvnMat3x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y + this->value[2].z * m.value[2].z);
	}
	LvnMat2x3_t<T> operator*(const LvnMat2x3_t<T>& m)
	{
		return LvnMat2x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z);
	}
	LvnMat4x3_t<T> operator*(const LvnMat4x3_t<T>& m)
	{
		return LvnMat4x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y + this->value[2].z * m.value[2].z,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y + this->value[2].x * m.value[3].z,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y + this->value[2].y * m.value[3].z,
			this->value[0].z * m.value[3].x + this->value[1].z * m.value[3].y + this->value[2].z * m.value[3].z);
	}
};

template<typename T>
LvnVec3_t<T> operator*(const LvnMat3x3_t<T>& m, const LvnVec3_t<T>& v)
{
	return LvnVec3_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z,
		m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z,
		m.value[0].z * v.x + m.value[1].z * v.y + m.value[2].z * v.z);
}
template<typename T>
LvnVec3_t<T> operator*(const LvnVec3_t<T>& v, const LvnMat3x3_t<T>& m)
{
	return LvnVec3_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z,
		v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z,
		v.x * m.value[2].x + v.y * m.value[2].y + v.z * m.value[2].z);
}

template<typename T>
struct LvnMat4x4_t
{
	LvnVec4_t<T> value[4];

	static int length() { return 4; }

	LvnMat4x4_t()
	{
		this->value[0] = { 1, 0, 0, 0 };
		this->value[1] = { 0, 1, 0, 0 };
		this->value[2] = { 0, 0, 1, 0 };
		this->value[3] = { 0, 0, 0, 1 };
	}
	LvnMat4x4_t(const T& n)
	{
		this->value[0] = { n, 0, 0, 0 };
		this->value[1] = { 0, n, 0, 0 };
		this->value[2] = { 0, 0, n, 0 };
		this->value[3] = { 0, 0, 0, n };
	}
	LvnMat4x4_t
	(
		const T& x0, const T& y0, const T& z0, const T& w0,
		const T& x1, const T& y1, const T& z1, const T& w1,
		const T& x2, const T& y2, const T& z2, const T& w2,
		const T& x3, const T& y3, const T& z3, const T& w3
	)
	{
		this->value[0] = { x0, y0, z0, w0 };
		this->value[1] = { x1, y1, z1, w1 };
		this->value[2] = { x2, y2, z2, w2 };
		this->value[3] = { x3, y3, z3, w3 };
	}
	LvnMat4x4_t(const LvnVec4_t<T>& v0, const LvnVec4_t<T>& v1, const LvnVec4_t<T>& v2, const LvnVec4_t<T>& v3)
		: value{ v0, v1, v2, v3 } {}
	LvnMat4x4_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0, 0), LvnVec4_t<T>(m.value[1], 0, 0), LvnVec4_t<T>(0, 0, 1, 0), LvnVec4_t<T>(0, 0, 0, 1) } {}
	LvnMat4x4_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0), LvnVec4_t<T>(m.value[2], 0), LvnVec4_t<T>(0, 0, 0, 1) } {}
	LvnMat4x4_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]), LvnVec4_t<T>(m.value[2]), LvnVec4_t<T>(m.value[3]) } {}
	LvnMat4x4_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0), LvnVec4_t<T>(0, 0, 1, 0), LvnVec4_t<T>(0, 0, 0, 1) } {}
	LvnMat4x4_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]), LvnVec4_t<T>(0, 0, 1, 0), LvnVec4_t<T>(0, 0, 0, 1) } {}
	LvnMat4x4_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0, 0), LvnVec4_t<T>(m.value[1], 0, 0), LvnVec4_t<T>(m.value[2], 0, 0), LvnVec4_t<T>(0, 0, 0, 1) } {}
	LvnMat4x4_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]), LvnVec4_t<T>(m.value[2]), LvnVec4_t<T>(0, 0, 0, 1) } {}
	LvnMat4x4_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0, 0), LvnVec4_t<T>(m.value[1], 0, 0), LvnVec4_t<T>(m.value[2], 1, 0), LvnVec4_t<T>(m.value[3], 0, 1) } {}
	LvnMat4x4_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0), LvnVec4_t<T>(m.value[2], 0), LvnVec4_t<T>(m.value[3], 1) } {}

	LvnVec4_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat4x4_t<T> operator+()
	{
		return LvnMat4x4_t<T>(
			this->value[0],
			this->value[1],
			this->value[2],
			this->value[3]);
	}
	LvnMat4x4_t<T> operator-()
	{
		return LvnMat4x4_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2],
			-this->value[3]);
	}
	LvnMat4x4_t<T> operator+(const LvnMat4x4_t<T>& m)
	{
		return LvnMat4x4_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2],
			this->value[3] + m.value[3]);
	}
	LvnMat4x4_t<T> operator-(const LvnMat4x4_t<T>& m)
	{
		return LvnMat4x4_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2],
			this->value[3] - m.value[3]);
	}
	LvnMat4x4_t<T> operator*(const LvnMat4x4_t<T>& m)
	{
		return LvnMat4x4_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z + this->value[3].z * m.value[0].w,
			this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y + this->value[2].w * m.value[0].z + this->value[3].w * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z + this->value[3].z * m.value[1].w,
			this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y + this->value[2].w * m.value[1].z + this->value[3].w * m.value[1].w,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z + this->value[3].x * m.value[2].w,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z + this->value[3].y * m.value[2].w,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y + this->value[2].z * m.value[2].z + this->value[3].z * m.value[2].w,
			this->value[0].w * m.value[2].x + this->value[1].w * m.value[2].y + this->value[2].w * m.value[2].z + this->value[3].w * m.value[2].w,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y + this->value[2].x * m.value[3].z + this->value[3].x * m.value[3].w,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y + this->value[2].y * m.value[3].z + this->value[3].y * m.value[3].w,
			this->value[0].z * m.value[3].x + this->value[1].z * m.value[3].y + this->value[2].z * m.value[3].z + this->value[3].z * m.value[3].w,
			this->value[0].w * m.value[3].x + this->value[1].w * m.value[3].y + this->value[2].w * m.value[3].z + this->value[3].w * m.value[3].w);
	}
	LvnMat2x4_t<T> operator*(const LvnMat2x4_t<T>& m)
	{
		return LvnMat2x4_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z + this->value[3].z * m.value[0].w,
			this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y + this->value[2].w * m.value[0].z + this->value[3].w * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z + this->value[3].z * m.value[1].w,
			this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y + this->value[2].w * m.value[1].z + this->value[3].w * m.value[1].w);
	}
	LvnMat3x4_t<T> operator*(const LvnMat3x4_t<T>& m)
	{
		return LvnMat3x4_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z + this->value[3].z * m.value[0].w,
			this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y + this->value[2].w * m.value[0].z + this->value[3].w * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z + this->value[3].z * m.value[1].w,
			this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y + this->value[2].w * m.value[1].z + this->value[3].w * m.value[1].w,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z + this->value[3].x * m.value[2].w,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z + this->value[3].y * m.value[2].w,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y + this->value[2].z * m.value[2].z + this->value[3].z * m.value[2].w,
			this->value[0].w * m.value[2].x + this->value[1].w * m.value[2].y + this->value[2].w * m.value[2].z + this->value[3].w * m.value[2].w);
	}
};

template<typename T>
LvnVec4_t<T> operator*(const LvnMat4x4_t<T>& m, const LvnVec4_t<T>& v)
{
	return LvnVec4_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z + m.value[3].x * v.w,
		m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z + m.value[3].y * v.w,
		m.value[0].z * v.x + m.value[1].z * v.y + m.value[2].z * v.z + m.value[3].z * v.w,
		m.value[0].w * v.x + m.value[1].w * v.y + m.value[2].w * v.z + m.value[3].w * v.w);
}
template<typename T>
LvnVec4_t<T> operator*(const LvnVec4_t<T>& v, const LvnMat4x4_t<T>& m)
{
	return LvnVec4_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z + v.w * m.value[0].w,
		v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z + v.w * m.value[1].w,
		v.x * m.value[2].x + v.y * m.value[2].y + v.z * m.value[2].z + v.w * m.value[2].w,
		v.x * m.value[3].x + v.y * m.value[3].y + v.z * m.value[3].z + v.w * m.value[3].w);
}

template<typename T>
struct LvnMat2x3_t
{
	LvnVec3_t<T> value[2];

	static int length() { return 2; }

	LvnMat2x3_t()
	{
		this->value[0] = { 1, 0, 0 };
		this->value[1] = { 0, 1, 0 };
	}
	LvnMat2x3_t(const T& n)
	{
		this->value[0] = { n, 0, 0 };
		this->value[1] = { 0, n, 0 };
	}
	LvnMat2x3_t
	(
		const T& x0, const T& y0, const T& z0,
		const T& x1, const T& y1, const T& z1
	)
	{
		this->value[0] = { x0, y0, z0 };
		this->value[1] = { x1, y1, z1 };
	}
	LvnMat2x3_t(const LvnVec3_t<T>& v0, const LvnVec3_t<T>& v1)
		: value{ v0, v1 } {}
	LvnMat2x3_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0) } {}
	LvnMat2x3_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]) } {}
	LvnMat2x3_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]) } {}
	LvnMat2x3_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]) } {}
	LvnMat2x3_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]) } {}
	LvnMat2x3_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0) } {}
	LvnMat2x3_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]) } {}
	LvnMat2x3_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0) } {}
	LvnMat2x3_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]) } {}

	LvnVec3_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat2x3_t<T> operator+()
	{
		return LvnMat2x3_t<T>(
			this->value[0],
			this->value[1]);
	}
	LvnMat2x3_t<T> operator-()
	{
		return LvnMat2x3_t<T>(
			-this->value[0],
			-this->value[1]);
	}
	LvnMat2x3_t<T> operator+(const LvnMat2x3_t<T>& m)
	{
		return LvnMat2x3_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1]);
	}
	LvnMat2x3_t<T> operator-(const LvnMat2x3_t<T>& m)
	{
		return LvnMat2x3_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1]);
	}
	LvnMat2x3_t<T> operator*(const LvnMat2x2_t<T>& m)
	{
		return LvnMat2x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y);
	}
	LvnMat3x3_t<T> operator*(const LvnMat3x2_t<T>& m)
	{
		return LvnMat3x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y);
	}
	LvnMat4x3_t<T> operator*(const LvnMat4x2_t<T>& m)
	{
		return LvnMat4x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y,
			this->value[0].z * m.value[3].x + this->value[1].z * m.value[3].y);
	}
};

template<typename T>
LvnVec3_t<T> operator*(const LvnMat2x3_t<T>& m, const LvnVec2_t<T>& v)
{
	return LvnVec3_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y,
		m.value[0].y * v.x + m.value[1].y * v.y,
		m.value[0].z * v.x + m.value[1].z * v.y);
}
template<typename T>
LvnVec2_t<T> operator*(const LvnVec3_t<T>& v, const LvnMat2x3_t<T>& m)
{
	return LvnVec2_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z,
		v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z);
}

template<typename T>
struct LvnMat2x4_t
{
	LvnVec4_t<T> value[2];

	static int length() { return 2; }

	LvnMat2x4_t()
	{
		this->value[0] = { 1, 0, 0, 0 };
		this->value[1] = { 0, 1, 0, 0 };
	}
	LvnMat2x4_t(const T& n)
	{
		this->value[0] = { n, 0, 0, 0 };
		this->value[1] = { 0, n, 0, 0 };
	}
	LvnMat2x4_t
	(
		const T& x0, const T& y0, const T& z0, const T& w0,
		const T& x1, const T& y1, const T& z1, const T& w1
	)
	{
		this->value[0] = { x0, y0, z0, w0 };
		this->value[1] = { x1, y1, z1, w1 };
	}
	LvnMat2x4_t(const LvnVec4_t<T>& v0, const LvnVec4_t<T>& v1)
		: value{ v0, v1 } {}
	LvnMat2x4_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]) } {}
	LvnMat2x4_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0) } {}
	LvnMat2x4_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]) } {}
	LvnMat2x4_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0) } {}
	LvnMat2x4_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]) } {}
	LvnMat2x4_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0, 0), LvnVec4_t<T>(m.value[1], 0, 0) } {}
	LvnMat2x4_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]) } {}
	LvnMat2x4_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0, 0), LvnVec4_t<T>(m.value[1], 0, 0) } {}
	LvnMat2x4_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0) } {}

	LvnVec4_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat2x4_t<T> operator+()
	{
		return LvnMat2x4_t<T>(
			this->value[0],
			this->value[1]);
	}
	LvnMat2x4_t<T> operator-()
	{
		return LvnMat2x4_t<T>(
			-this->value[0],
			-this->value[1]);
	}
	LvnMat2x4_t<T> operator+(const LvnMat2x4_t<T>& m)
	{
		return LvnMat2x4_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1]);
	}
	LvnMat2x4_t<T> operator-(const LvnMat2x4_t<T>& m)
	{
		return LvnMat2x4_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1]);
	}
	LvnMat4x4_t<T> operator*(const LvnMat4x2_t<T>& m)
	{
		return LvnMat4x4_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y,
			this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y,
			this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y,
			this->value[0].w * m.value[2].x + this->value[1].w * m.value[2].y,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y,
			this->value[0].z * m.value[3].x + this->value[1].z * m.value[3].y,
			this->value[0].w * m.value[3].x + this->value[1].w * m.value[3].y);
	}
	LvnMat2x4_t<T> operator*(const LvnMat2x2_t<T>& m)
	{
		return LvnMat2x4_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y,
			this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y,
			this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y);
	}
	LvnMat3x4_t<T> operator*(const LvnMat3x2_t<T>& m)
	{
		return LvnMat3x4_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y,
			this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y,
			this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y,
			this->value[0].w * m.value[2].x + this->value[1].w * m.value[2].y);
	}
};

template<typename T>
LvnVec4_t<T> operator*(const LvnMat2x4_t<T>& m, const LvnVec2_t<T>& v)
{
	return LvnVec4_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y,
		m.value[0].y * v.x + m.value[1].y * v.y,
		m.value[0].z * v.x + m.value[1].z * v.y,
		m.value[0].w * v.x + m.value[1].w * v.y);
}
template<typename T>
LvnVec2_t<T> operator*(const LvnVec4_t<T>& v, const LvnMat2x4_t<T>& m)
{
	return LvnVec2_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z + v.w * m.value[0].w,
		v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z + v.w * m.value[1].w);
}

template<typename T>
struct LvnMat3x2_t
{
	LvnVec2_t<T> value[3];

	static int length() { return 3; }

	LvnMat3x2_t()
	{
		this->value[0] = { 1, 0 };
		this->value[1] = { 0, 1 };
		this->value[2] = { 0, 0 };
	}
	LvnMat3x2_t(const T& n)
	{
		this->value[0] = { n, 0 };
		this->value[1] = { 0, n };
		this->value[2] = { 0, 0 };
	}
	LvnMat3x2_t
	(
		const T& x0, const T& y0,
		const T& x1, const T& y1,
		const T& x2, const T& y2
	)
	{
		this->value[0] = { x0, y0 };
		this->value[1] = { x1, y1 };
		this->value[2] = { x2, y2 };
	}
	LvnMat3x2_t(const LvnVec2_t<T>& v0, const LvnVec2_t<T>& v1, const LvnVec2_t<T>& v2)
		: value{ v0, v1, v2 } {}
	LvnMat3x2_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(0, 0) } {}
	LvnMat3x2_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]) } {}
	LvnMat3x2_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]) } {}
	LvnMat3x2_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(0, 0) } {}
	LvnMat3x2_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(0, 0) } {}
	LvnMat3x2_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]) } {}
	LvnMat3x2_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]) } {}
	LvnMat3x2_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]) } {}
	LvnMat3x2_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]) } {}

	LvnVec2_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat3x2_t<T> operator+()
	{
		return LvnMat3x2_t<T>(
			this->value[0],
			this->value[1],
			this->value[2]);
	}
	LvnMat3x2_t<T> operator-()
	{
		return LvnMat3x2_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2]);
	}
	LvnMat3x2_t<T> operator+(const LvnMat3x2_t<T>& m)
	{
		return LvnMat3x2_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2]);
	}
	LvnMat3x2_t<T> operator-(const LvnMat3x2_t<T>& m)
	{
		return LvnMat3x2_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2]);
	}
	LvnMat3x2_t<T> operator*(const LvnMat3x3_t<T>& m)
	{
		return LvnMat3x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z);
	}
	LvnMat4x2_t<T> operator*(const LvnMat4x3_t<T>& m)
	{
		return LvnMat4x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y + this->value[2].x * m.value[3].z,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y + this->value[2].y * m.value[3].z);
	}
};

template<typename T>
LvnVec2_t<T> operator*(const LvnMat3x2_t<T>& m, const LvnVec3_t<T>& v)
{
	return LvnVec2_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z,
		m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z);
}
template<typename T>
LvnVec3_t<T> operator*(const LvnVec2_t<T>& v, const LvnMat3x2_t<T>& m)
{
	return LvnVec3_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y,
		v.x * m.value[1].x + v.y * m.value[1].y,
		v.x * m.value[2].x + v.y * m.value[2].y);
}

template<typename T>
struct LvnMat3x4_t
{
	LvnVec4_t<T> value[3];

	static int length() { return 3; }

	LvnMat3x4_t()
	{
		this->value[0] = { 1, 0, 0, 0 };
		this->value[1] = { 0, 1, 0, 0 };
		this->value[2] = { 0, 0, 1, 0 };
	}
	LvnMat3x4_t(const T& n)
	{
		this->value[0] = { n, 0, 0, 0 };
		this->value[1] = { 0, n, 0, 0 };
		this->value[2] = { 0, 0, n, 0 };
	}
	LvnMat3x4_t
	(
		const T& x0, const T& y0, const T& z0,
		const T& x1, const T& y1, const T& z1,
		const T& x2, const T& y2, const T& z2
	)
	{
		this->value[0] = { x0, y0, z0 };
		this->value[1] = { x1, y1, z1 };
		this->value[2] = { x2, y2, z2 };
	}
	LvnMat3x4_t(const LvnVec4_t<T>& v0, const LvnVec4_t<T>& v1, const LvnVec4_t<T>& v2)
		: value{ v0, v1, v2 } {}
	LvnMat3x4_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0, 0), LvnVec4_t<T>(m.value[1], 0, 0), LvnVec4_t<T>(0, 0, 1, 0) } {}
	LvnMat3x4_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0), LvnVec4_t<T>(m.value[2], 0) } {}
	LvnMat3x4_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]), LvnVec4_t<T>(m.value[2]) } {}
	LvnMat3x4_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0), LvnVec4_t<T>(0, 0, 1, 0) } {}
	LvnMat3x4_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]), LvnVec4_t<T>(0, 0, 1, 0) } {}
	LvnMat3x4_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0, 0), LvnVec4_t<T>(m.value[1], 0, 0), LvnVec4_t<T>(m.value[2], 1, 0) } {}
	LvnMat3x4_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]), LvnVec4_t<T>(m.value[2]) } {}
	LvnMat3x4_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0, 0), LvnVec4_t<T>(m.value[1], 0, 0), LvnVec4_t<T>(m.value[2], 1, 0) } {}
	LvnMat3x4_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0), LvnVec4_t<T>(m.value[2], 0) } {}

	LvnVec4_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat3x4_t<T> operator+()
	{
		return LvnMat3x4_t<T>(
			this->value[0],
			this->value[1],
			this->value[2]);
	}
	LvnMat3x4_t<T> operator-()
	{
		return LvnMat3x4_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2]);
	}
	LvnMat3x4_t<T> operator+(const LvnMat3x4_t<T>& m)
	{
		return LvnMat3x4_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2]);
	}
	LvnMat3x4_t<T> operator-(const LvnMat3x4_t<T>& m)
	{
		return LvnMat3x4_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2]);
	}
	LvnMat4x4_t<T> operator*(const LvnMat4x3_t<T>& m)
	{
		return LvnMat4x4_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z,
			this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y + this->value[2].w * m.value[0].z,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z,
			this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y + this->value[2].w * m.value[1].z,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y + this->value[2].z * m.value[2].z,
			this->value[0].w * m.value[2].x + this->value[1].w * m.value[2].y + this->value[2].w * m.value[2].z,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y + this->value[2].x * m.value[3].z,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y + this->value[2].y * m.value[3].z,
			this->value[0].z * m.value[3].x + this->value[1].z * m.value[3].y + this->value[2].z * m.value[3].z,
			this->value[0].w * m.value[3].x + this->value[1].w * m.value[3].y + this->value[2].w * m.value[3].z);
	}
	LvnMat2x4_t<T> operator*(const LvnMat2x3_t<T>& m)
	{
		return LvnMat2x4_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].x + this->value[2].x * m.value[0].x,
			this->value[0].y * m.value[0].y + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].y,
			this->value[0].z * m.value[0].z + this->value[1].z * m.value[0].z + this->value[2].z * m.value[0].z,
			this->value[0].w * m.value[0].w + this->value[1].w * m.value[0].w + this->value[2].w * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].x + this->value[2].x * m.value[1].x,
			this->value[0].y * m.value[1].y + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].y,
			this->value[0].z * m.value[1].z + this->value[1].z * m.value[1].z + this->value[2].z * m.value[1].z,
			this->value[0].w * m.value[1].w + this->value[1].w * m.value[1].w + this->value[2].w * m.value[1].w);
	}
	LvnMat3x4_t<T> operator*(const LvnMat3x3_t<T>& m)
	{
		return LvnMat3x4_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z,
			this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y + this->value[2].w * m.value[0].z,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z,
			this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y + this->value[2].w * m.value[1].z,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y + this->value[2].z * m.value[2].z,
			this->value[0].w * m.value[2].x + this->value[1].w * m.value[2].y + this->value[2].w * m.value[2].z);
	}
};

template<typename T>
LvnVec4_t<T> operator*(const LvnMat3x4_t<T>& m, const LvnVec3_t<T>& v)
{
	return LvnVec4_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z,
		m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z,
		m.value[0].z * v.x + m.value[1].z * v.y + m.value[2].z * v.z,
		m.value[0].w * v.x + m.value[1].w * v.y + m.value[2].w * v.z);
}
template<typename T>
LvnVec3_t<T> operator*(const LvnVec4_t<T>& v, const LvnMat3x4_t<T>& m)
{
	return LvnVec3_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z + v.w * m.value[0].w,
		v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z + v.w * m.value[1].w,
		v.x * m.value[2].x + v.y * m.value[2].y + v.z * m.value[2].z + v.w * m.value[2].w);
}

template<typename T>
struct LvnMat4x2_t
{
	LvnVec2_t<T> value[4];

	static int length() { return 4; }

	LvnMat4x2_t()
	{
		this->value[0] = { 1, 0 };
		this->value[1] = { 0, 1 };
		this->value[2] = { 0, 0 };
		this->value[3] = { 0, 0 };
	}
	LvnMat4x2_t(const T& n)
	{
		this->value[0] = { n, 0 };
		this->value[1] = { 0, n };
		this->value[2] = { 0, 0 };
		this->value[3] = { 0, 0 };
	}
	LvnMat4x2_t
	(
		const T& x0, const T& y0,
		const T& x1, const T& y1,
		const T& x2, const T& y2,
		const T& x3, const T& y3
	)
	{
		this->value[0] = { x0, y0 };
		this->value[1] = { x1, y1 };
		this->value[2] = { x2, y2 };
		this->value[3] = { x3, y3 };
	}
	LvnMat4x2_t(const LvnVec2_t<T>& v0, const LvnVec2_t<T>& v1, const LvnVec2_t<T>& v2, const LvnVec2_t<T>& v3)
		: value{ v0, v1, v2, v3 } {}
	LvnMat4x2_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(0, 0), LvnVec2_t<T>(0, 0) } {}
	LvnMat4x2_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]), LvnVec2_t<T>(0, 0) } {}
	LvnMat4x2_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]), LvnVec2_t<T>(m.value[3]) } {}
	LvnMat4x2_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(0, 0), LvnVec2_t<T>(0, 0) } {}
	LvnMat4x2_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(0, 0), LvnVec2_t<T>(0, 0) } {}
	LvnMat4x2_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]), LvnVec2_t<T>(0, 0) } {}
	LvnMat4x2_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]), LvnVec2_t<T>(0, 0) } {}
	LvnMat4x2_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]), LvnVec2_t<T>(m.value[3]) } {}
	LvnMat4x2_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]), LvnVec2_t<T>(m.value[3]) } {}

	LvnVec2_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat4x2_t<T> operator+()
	{
		return LvnMat4x2_t<T>(
			this->value[0],
			this->value[1],
			this->value[2],
			this->value[3]);
	}
	LvnMat4x2_t<T> operator-()
	{
		return LvnMat4x2_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2],
			-this->value[3]);
	}
	LvnMat4x2_t<T> operator+(const LvnMat4x2_t<T>& m)
	{
		return LvnMat4x2_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2],
			this->value[3] + m.value[3]);
	}
	LvnMat4x2_t<T> operator-(const LvnMat4x2_t<T>& m)
	{
		return LvnMat4x2_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2],
			this->value[3] - m.value[3]);
	}
	LvnMat2x2_t<T> operator*(const LvnMat2x4_t<T>& m)
	{
		return LvnMat2x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w);
	}
	LvnMat3x2_t<T> operator*(const LvnMat3x4_t<T>& m)
	{
		return LvnMat3x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z + this->value[3].x * m.value[2].w,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z + this->value[3].y * m.value[2].w);
	}
	LvnMat4x2_t<T> operator*(const LvnMat4x4_t<T>& m)
	{
		return LvnMat4x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z + this->value[3].x * m.value[2].w,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z + this->value[3].y * m.value[2].w,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y + this->value[2].x * m.value[3].z + this->value[3].x * m.value[3].w,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y + this->value[2].y * m.value[3].z + this->value[3].y * m.value[3].w);
	}
};

template<typename T>
LvnVec2_t<T> operator*(const LvnMat4x2_t<T>& m, const LvnVec4_t<T>& v)
{
	return LvnVec2_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z + m.value[3].x * v.w,
		m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z + m.value[3].y * v.w);
}
template<typename T>
LvnVec4_t<T> operator*(const LvnVec2_t<T>& v, const LvnMat4x2_t<T>& m)
{
	return LvnVec4_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y,
		v.x * m.value[1].x + v.y * m.value[1].y,
		v.x * m.value[2].x + v.y * m.value[2].y,
		v.x * m.value[3].x + v.y * m.value[3].y);
}

template<typename T>
struct LvnMat4x3_t
{
	LvnVec3_t<T> value[4];

	static int length() { return 4; }

	LvnMat4x3_t()
	{
		this->value[0] = { 1, 0, 0 };
		this->value[1] = { 0, 1, 0 };
		this->value[2] = { 0, 0, 1 };
		this->value[3] = { 0, 0, 0 };
	}
	LvnMat4x3_t(const T& n)
	{
		this->value[0] = { n, 0, 0 };
		this->value[1] = { 0, n, 0 };
		this->value[2] = { 0, 0, n };
		this->value[3] = { 0, 0, 0 };
	}
	LvnMat4x3_t
	(
		const T& x0, const T& y0, const T& z0,
		const T& x1, const T& y1, const T& z1,
		const T& x2, const T& y2, const T& z2,
		const T& x3, const T& y3, const T& z3
	)
	{
		this->value[0] = { x0, y0, z0 };
		this->value[1] = { x1, y1, z1 };
		this->value[2] = { x2, y2, z2 };
		this->value[3] = { x3, y3, z3 };
	}
	LvnMat4x3_t(const LvnVec3_t<T>& v0, const LvnVec3_t<T>& v1, const LvnVec3_t<T>& v2, const LvnVec3_t<T>& v3)
		: value{ v0, v1, v2, v3 } {}
	LvnMat4x3_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0), LvnVec3_t<T>(0, 0, 1), LvnVec3_t<T>(0, 0, 0) } {}
	LvnMat4x3_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(m.value[2]), LvnVec3_t<T>(0, 0, 0) } {}
	LvnMat4x3_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(m.value[2]), LvnVec3_t<T>(m.value[3]) } {}
	LvnMat4x3_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(0, 0, 1), LvnVec3_t<T>(0, 0, 0) } {}
	LvnMat4x3_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(0, 0, 1), LvnVec3_t<T>(0, 0, 0) } {}
	LvnMat4x3_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0), LvnVec3_t<T>(m.value[2], 1), LvnVec3_t<T>(0, 0, 0) } {}
	LvnMat4x3_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(m.value[2]), LvnVec3_t<T>(0, 0, 0) } {}
	LvnMat4x3_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0), LvnVec3_t<T>(m.value[2], 1), LvnVec3_t<T>(m.value[3], 0) } {}
	LvnMat4x3_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(m.value[2]), LvnVec3_t<T>(m.value[3]) } {}

	LvnVec3_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat4x3_t<T> operator+()
	{
		return LvnMat4x3_t<T>(
			this->value[0],
			this->value[1],
			this->value[2],
			this->value[3]);
	}
	LvnMat4x3_t<T> operator-()
	{
		return LvnMat4x3_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2],
			-this->value[3]);
	}
	LvnMat4x3_t<T> operator+(const LvnMat4x3_t<T>& m)
	{
		return LvnMat4x3_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2],
			this->value[3] + m.value[3]);
	}
	LvnMat4x3_t<T> operator-(const LvnMat4x3_t<T>& m)
	{
		return LvnMat4x3_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2],
			this->value[3] - m.value[3]);
	}
	LvnMat2x3_t<T> operator*(const LvnMat2x4_t<T>& m)
	{
		return LvnMat2x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z + this->value[3].z * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z + this->value[3].z * m.value[1].w);
	}
	LvnMat3x3_t<T> operator*(const LvnMat3x4_t<T>& m)
	{
		return LvnMat3x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z + this->value[3].z * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z + this->value[3].z * m.value[1].w,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z + this->value[3].x * m.value[2].w,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z + this->value[3].y * m.value[2].w,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y + this->value[2].z * m.value[2].z + this->value[3].z * m.value[2].w);
	}
	LvnMat4x3_t<T> operator*(const LvnMat4x4_t<T>& m)
	{
		return LvnMat4x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z + this->value[3].z * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z + this->value[3].z * m.value[1].w,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z + this->value[3].x * m.value[2].w,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z + this->value[3].y * m.value[2].w,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y + this->value[2].z * m.value[2].z + this->value[3].z * m.value[2].w,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y + this->value[2].x * m.value[3].z + this->value[3].x * m.value[3].w,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y + this->value[2].y * m.value[3].z + this->value[3].y * m.value[3].w,
			this->value[0].z * m.value[3].x + this->value[1].z * m.value[3].y + this->value[2].z * m.value[3].z + this->value[3].z * m.value[3].w);
	}
};

template<typename T>
LvnVec3_t<T> operator*(const LvnMat4x3_t<T>& m, const LvnVec4_t<T>& v)
{
	return LvnVec3_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z + m.value[3].x * v.w,
		m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z + m.value[3].y * v.w,
		m.value[0].z * v.x + m.value[1].z * v.y + m.value[2].z * v.z + m.value[3].z * v.w);
}
template<typename T>
LvnVec4_t<T> operator*(const LvnVec3_t<T>& v, const LvnMat4x3_t<T>& m)
{
	return LvnVec4_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z,
		v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z,
		v.x * m.value[2].x + v.y * m.value[2].y + v.z * m.value[2].z,
		v.x * m.value[3].x + v.y * m.value[3].y + v.z * m.value[3].z);
}

typedef LvnVec2_t<float>				LvnVec2;
typedef LvnVec3_t<float>				LvnVec3;
typedef LvnVec4_t<float>				LvnVec4;
typedef LvnVec2_t<int>					LvnVec2i;
typedef LvnVec3_t<int>					LvnVec3i;
typedef LvnVec4_t<int>					LvnVec4i;
typedef LvnVec2_t<unsigned int>			LvnVec2ui;
typedef LvnVec3_t<unsigned int>			LvnVec3ui;
typedef LvnVec4_t<unsigned int>			LvnVec4ui;
typedef LvnVec2_t<float>				LvnVec2f;
typedef LvnVec3_t<float>				LvnVec3f;
typedef LvnVec4_t<float>				LvnVec4f;
typedef LvnVec2_t<double>				LvnVec2d;
typedef LvnVec3_t<double>				LvnVec3d;
typedef LvnVec4_t<double>				LvnVec4d;
typedef LvnVec2_t<bool>					LvnVec2b;
typedef LvnVec3_t<bool>					LvnVec3b;
typedef LvnVec4_t<bool>					LvnVec4b;

typedef LvnMat2x2_t<float>				LvnMat2;
typedef LvnMat3x3_t<float>				LvnMat3;
typedef LvnMat4x4_t<float>				LvnMat4;
typedef LvnMat2x3_t<float>				LvnMat2x3;
typedef LvnMat2x4_t<float>				LvnMat2x4;
typedef LvnMat3x2_t<float>				LvnMat3x2;
typedef LvnMat3x4_t<float>				LvnMat3x4;
typedef LvnMat4x2_t<float>				LvnMat4x2;
typedef LvnMat4x3_t<float>				LvnMat4x3;
typedef LvnMat2x2_t<int>				LvnMat2i;
typedef LvnMat3x3_t<int>				LvnMat3i;
typedef LvnMat4x4_t<int>				LvnMat4i;
typedef LvnMat2x3_t<int>				LvnMat2x3i;
typedef LvnMat2x4_t<int>				LvnMat2x4i;
typedef LvnMat3x2_t<int>				LvnMat3x2i;
typedef LvnMat3x4_t<int>				LvnMat3x4i;
typedef LvnMat4x2_t<int>				LvnMat4x2i;
typedef LvnMat4x3_t<int>				LvnMat4x3i;
typedef LvnMat2x2_t<unsigned int>		LvnMat2ui;
typedef LvnMat3x3_t<unsigned int>		LvnMat3ui;
typedef LvnMat4x4_t<unsigned int>		LvnMat4ui;
typedef LvnMat2x3_t<unsigned int>		LvnMat2x3ui;
typedef LvnMat2x4_t<unsigned int>		LvnMat2x4ui;
typedef LvnMat3x2_t<unsigned int>		LvnMat3x2ui;
typedef LvnMat3x4_t<unsigned int>		LvnMat3x4ui;
typedef LvnMat4x2_t<unsigned int>		LvnMat4x2ui;
typedef LvnMat4x3_t<unsigned int>		LvnMat4x3ui;
typedef LvnMat2x2_t<float>				LvnMat2f;
typedef LvnMat3x3_t<float>				LvnMat3f;
typedef LvnMat4x4_t<float>				LvnMat4f;
typedef LvnMat2x3_t<float>				LvnMat2x3f;
typedef LvnMat2x4_t<float>				LvnMat2x4f;
typedef LvnMat3x2_t<float>				LvnMat3x2f;
typedef LvnMat3x4_t<float>				LvnMat3x4f;
typedef LvnMat4x2_t<float>				LvnMat4x2f;
typedef LvnMat4x3_t<float>				LvnMat4x3f;
typedef LvnMat2x2_t<double>				LvnMat2d;
typedef LvnMat3x3_t<double>				LvnMat3d;
typedef LvnMat4x4_t<double>				LvnMat4d;
typedef LvnMat2x3_t<double>				LvnMat2x3d;
typedef LvnMat2x4_t<double>				LvnMat2x4d;
typedef LvnMat3x2_t<double>				LvnMat3x2d;
typedef LvnMat3x4_t<double>				LvnMat3x4d;
typedef LvnMat4x2_t<double>				LvnMat4x2d;
typedef LvnMat4x3_t<double>				LvnMat4x3d;

struct LvnPhysicalDeviceInfo
{
	char name[256];
	LvnPhysicalDeviceType type;
	uint32_t apiVersion;
	uint32_t driverVersion;
};

struct LvnRendererBackends
{
	LvnPhysicalDevice*	physicalDevice;
	LvnWindow**			pWindows;
	uint32_t			windowCount;
	bool				gammaCorrection;
	uint32_t			maxFramesInFlight;
};

struct LvnRenderPassAttachment
{
	LvnAttachmentType type;
	LvnImageFormat format;
	LvnSampleCount samples;
	LvnAttachmentLoadOperation loadOp, stencilLoadOp;
	LvnAttachmentStoreOperation storeOp, stencilStoreOp;
	LvnImageLayout initialLayout, finalLayout;
};

struct LvnRenderPassCreateInfo
{
	LvnRenderPassAttachment* pAttachments;
	uint32_t attachmentCount;
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

struct LvnPipelineSpecification
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
	uint32_t layout, binding;
	LvnVertexDataType type;
	uint32_t offset;
};

struct LvnPipelineCreateInfo
{
	LvnPipelineSpecification* pipelineSpecification;
	LvnShader* shader;
	LvnWindow* window;
	LvnRenderPass* renderPass;
	LvnVertexBindingDescription* pVertexBindingDescriptions;
	LvnVertexAttribute* pVertexAttributes;

	uint32_t vertexBindingDescriptionCount, vertexAttributeCount;
};

struct LvnShaderCreateInfo
{
	const char* vertexSrc;
	const char* fragmentSrc;
};

struct LvnFrameBufferCreateInfo
{
	uint32_t width, height;
	LvnWindow* window;
	LvnRenderPass* renderPass;
};

struct LvnVertexArrayBufferCreateInfo
{
	float* pVertices;
	uint32_t* pIndices;
	LvnVertexBindingDescription* pVertexBindingDescriptions;
	LvnVertexAttribute* pVertexAttributes;

	uint32_t vertexCount, indexCount, vertexBindingDescriptionCount, vertexAttributeCount;
};


#endif
