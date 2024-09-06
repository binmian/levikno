#ifndef HG_LEVIKNO_H
#define HG_LEVIKNO_H

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
	#include <cassert> /* assert() */

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

// Compiler
#ifdef _MSC_VER
	#define LVN_ASSERT_BREAK __debugbreak()
	#pragma warning (disable : 4267)
	#pragma warning (disable : 4244)
	#pragma warning (disable : 26495)

	#ifdef _DEBUG
		#ifndef LVN_CONFIG_DEBUG
			#define LVN_CONFIG_DEBUG
		#endif
	#endif
#else
	#ifndef NDEBUG
		#ifndef LVN_CONFIG_DEBUG
			#define LVN_CONFIG_DEBUG
		#endif
	#endif

	#define LVN_ASSERT_BREAK assert(false)
#endif

// debug
#ifdef LVN_CONFIG_DEBUG
	#define LVN_ENABLE_ASSERTS
#endif

#ifdef LVN_DISABLE_ASSERTS_KEEP_ERROR_MESSAGES
	#define LVN_DISABLE_ASSERTS
#endif

#if defined (LVN_DISABLE_ASSERTS)
	#define LVN_ASSERT(x, ...) { if(!(x)) { LVN_ERROR(__VA_ARGS__); } }
	#define LVN_CORE_ASSERT(x, ...) { if(!(x)) { LVN_CORE_ERROR(__VA_ARGS__); } }
#elif defined (LVN_ENABLE_ASSERTS)
	#define LVN_ASSERT(x, ...) { if(!(x)) { LVN_ERROR(__VA_ARGS__); LVN_ASSERT_BREAK; } }
	#define LVN_CORE_ASSERT(x, ...) { if(!(x)) { LVN_CORE_ERROR(__VA_ARGS__); LVN_ASSERT_BREAK; } }
#else
	#define LVN_ASSERT(x, ...)
	#define LVN_CORE_ASSERT(x, ...)
#endif


#define LVN_TRUE 1
#define LVN_FALSE 0
#define LVN_NULL_HANDLE nullptr

#define LVN_UINT8_MAX  0xff
#define LVN_UINT16_MAX 0xffff
#define LVN_UINT32_MAX 0xffffffff
#define LVN_UINT64_MAX 0xffffffffffffffff

#define LVN_FILE_NAME __FILE__
#define LVN_LINE __LINE__
#define LVN_FUNC_NAME __func__

#define LVN_STR(x) #x
#define LVN_STRINGIFY(x) LVN_STR(x)

#define LVN_PI 3.14159265f
#define LVN_PI_EXACT (static_cast<double>(22.0/7.0)) /* 3.1415... */


#include <cstdlib> // malloc, free
#include <cstdint> // uint8_t, uint16_t, uint32_t, uint64_t
#include <cstring> // strlen
#include <cmath>

#include <string>
#include <vector>
#include <chrono>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

using std::abs;
using std::acos;
using std::asin;
using std::atan;
using std::atan2;
using std::cos;
using std::sin;
using std::tan;
using std::cosh;
using std::sinh;
using std::tanh;
using std::exp;
using std::frexp;
using std::ldexp;
using std::log;
using std::log10;
using std::modf;
using std::pow;
using std::sqrt;
using std::ceil;
using std::fabs;
using std::floor;
using std::fmod;

/* [Vectors & Matrices] */

/*
 * NOTE: vector values can be retrieved by index or accessing member: vec[0] or vec.x
 * NOTE: vectors with multiple components will have different variable members but share the same memory through unions: vec.x is the same as vec.r and vec.s
 *
 * Matrices: used for graphics math and transformations
 * NOTE: Accessing Matrices by index start with column then row:
 *       mat4x4 = [ [0][0], [1][0], [2][0], [3][0] ]
 *                [ [0][1], [1][1], [2][1], [3][1] ]
 *                [ [0][2], [1][2], [2][2], [3][2] ]
 *                [ [0][3], [1][3], [2][3], [3][3] ]
 *
 */

/* [Logging] */

/*
*   Color          | FG | BG
* -----------------+----+----
*   Black          | 30 | 40
*   Red            | 31 | 41
*   Green          | 32 | 42
*   Yellow         | 33 | 43
*   Blue           | 34 | 44
*   Magenta        | 35 | 45
*   Cyan           | 36 | 46
*   White          | 37 | 47
*   Bright Black   | 90 | 100
*   Bright Red     | 91 | 101
*   Bright Green   | 92 | 102
*   Bright Yellow  | 93 | 103
*   Bright Blue    | 94 | 104
*   Bright Magenta | 95 | 105
*   Bright Cyan    | 96 | 106
*   Bright White   | 97 | 107
*
*
*   reset             0
*   bold/bright       1
*   underline         4
*   inverse           7
*   bold/bright off  21
*   underline off    24
*   inverse off      27
*
*
*   Log Colors:
*   TRACE           \x1b[0;37m
*   DEBUG           \x1b[0;34m
*   INFO            \x1b[0;32m
*   WARN            \x1b[1;33m
*   ERROR           \x1b[1;31m
*   FATAL           \x1b[1;37;41m
*
*/

#define LVN_LOG_COLOR_TRACE                     "\x1b[0;37m"
#define LVN_LOG_COLOR_DEBUG                     "\x1b[0;34m"
#define LVN_LOG_COLOR_INFO                      "\x1b[0;32m"
#define LVN_LOG_COLOR_WARN                      "\x1b[1;33m"
#define LVN_LOG_COLOR_ERROR                     "\x1b[1;31m"
#define LVN_LOG_COLOR_FATAL                     "\x1b[1;37;41m"
#define LVN_LOG_COLOR_RESET                     "\x1b[0m"


// Core Log macros
#define LVN_CORE_TRACE(...)                     ::lvn::logMessageTrace(lvn::logGetCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_DEBUG(...)                     ::lvn::logMessageDebug(lvn::logGetCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_INFO(...)                      ::lvn::logMessageInfo(lvn::logGetCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_WARN(...)                      ::lvn::logMessageWarn(lvn::logGetCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_ERROR(...)                     ::lvn::logMessageError(lvn::logGetCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_FATAL(...)                     ::lvn::logMessageFatal(lvn::logGetCoreLogger(), ##__VA_ARGS__)

// Client Log macros
#define LVN_TRACE(...)                          ::lvn::logMessageTrace(lvn::logGetClientLogger(), ##__VA_ARGS__)
#define LVN_DEBUG(...)                          ::lvn::logMessageDebug(lvn::logGetClientLogger(), ##__VA_ARGS__)
#define LVN_INFO(...)                           ::lvn::logMessageInfo(lvn::logGetClientLogger(), ##__VA_ARGS__)
#define LVN_WARN(...)                           ::lvn::logMessageWarn(lvn::logGetClientLogger(), ##__VA_ARGS__)
#define LVN_ERROR(...)                          ::lvn::logMessageError(lvn::logGetClientLogger(), ##__VA_ARGS__)
#define LVN_FATAL(...)                          ::lvn::logMessageFatal(lvn::logGetClientLogger(), ##__VA_ARGS__)


// Logging utils
#define LVN_PROPERTIES(prop)                    #prop, &prop
#define LVN_LOG_FILE                            LVN_FILE_NAME ":" LVN_STRINGIFY(LVN_LINE) " - "


// ---------------------------------------------
// [SECTION]: Enums
// ---------------------------------------------

/* [Core Enums] */

enum LvnResult
{
	Lvn_Result_Failure              = 0,
	Lvn_Result_Success              = 1,
	Lvn_Result_AlreadyCalled        = 2,
	Lvn_Result_MemAllocFailure      = 3,
};

enum LvnStructureType
{
	Lvn_Stype_Undefined = 0,
	Lvn_Stype_Window,
	Lvn_Stype_Logger,
	Lvn_Stype_FrameBuffer,
	Lvn_Stype_Shader,
	Lvn_Stype_DescriptorLayout,
	Lvn_Stype_DescriptorSet,
	Lvn_Stype_Pipeline,
	Lvn_Stype_Buffer,
	Lvn_Stype_UniformBuffer,
	Lvn_Stype_Texture,
	Lvn_Stype_Sound,
	Lvn_Stype_SoundBoard,

	Lvn_Stype_MaxType,
};

enum LvnMemAllocMode
{
	Lvn_MemAllocMode_Individual,
	Lvn_MemAllocMode_MemPool,
};

enum LvnClipRegion
{
	Lvn_ClipRegion_ApiSpecific,
	Lvn_ClipRegion_LeftHandZeroToOne,
	Lvn_ClipRegion_LeftHandNegOneToOne,
	Lvn_ClipRegion_RightHandZeroToOne,
	Lvn_ClipRegion_RightHandNegOneToOne,

	Lvn_ClipRegion_LHZO = Lvn_ClipRegion_LeftHandZeroToOne,
	Lvn_ClipRegion_LHNO = Lvn_ClipRegion_LeftHandNegOneToOne,
	Lvn_ClipRegion_RHZO = Lvn_ClipRegion_RightHandZeroToOne,
	Lvn_ClipRegion_RHNO = Lvn_ClipRegion_RightHandNegOneToOne,
};

/* Key Codes */
enum LvnKeyCodes
{
	Lvn_KeyCode_Space           = 32,
	Lvn_KeyCode_Apostrophe      = 39,       /* ' */
	Lvn_KeyCode_Comma           = 44,       /* , */
	Lvn_KeyCode_Minus           = 45,       /* - */
	Lvn_KeyCode_Period          = 46,       /* . */
	Lvn_KeyCode_Slash           = 47,       /* / */
	Lvn_KeyCode_0               = 48,
	Lvn_KeyCode_1               = 49,
	Lvn_KeyCode_2               = 50,
	Lvn_KeyCode_3               = 51,
	Lvn_KeyCode_4               = 52,
	Lvn_KeyCode_5               = 53,
	Lvn_KeyCode_6               = 54,
	Lvn_KeyCode_7               = 55,
	Lvn_KeyCode_8               = 56,
	Lvn_KeyCode_9               = 57,
	Lvn_KeyCode_Semicolon       = 59,       /* ; */
	Lvn_KeyCode_Equal           = 61,       /* = */
	Lvn_KeyCode_A               = 65,
	Lvn_KeyCode_B               = 66,
	Lvn_KeyCode_C               = 67,
	Lvn_KeyCode_D               = 68,
	Lvn_KeyCode_E               = 69,
	Lvn_KeyCode_F               = 70,
	Lvn_KeyCode_G               = 71,
	Lvn_KeyCode_H               = 72,
	Lvn_KeyCode_I               = 73,
	Lvn_KeyCode_J               = 74,
	Lvn_KeyCode_K               = 75,
	Lvn_KeyCode_L               = 76,
	Lvn_KeyCode_M               = 77,
	Lvn_KeyCode_N               = 78,
	Lvn_KeyCode_O               = 79,
	Lvn_KeyCode_P               = 80,
	Lvn_KeyCode_Q               = 81,
	Lvn_KeyCode_R               = 82,
	Lvn_KeyCode_S               = 83,
	Lvn_KeyCode_T               = 84,
	Lvn_KeyCode_U               = 85,
	Lvn_KeyCode_V               = 86,
	Lvn_KeyCode_W               = 87,
	Lvn_KeyCode_X               = 88,
	Lvn_KeyCode_Y               = 89,
	Lvn_KeyCode_Z               = 90,
	Lvn_KeyCode_LeftBracket     = 91,       /* [ */
	Lvn_KeyCode_Backslash       = 92,       /* \ */
	Lvn_KeyCode_RightBracket    = 93,       /* ] */
	Lvn_KeyCode_GraveAccent     = 96,       /* ` */
	Lvn_KeyCode_World1          = 161,      /* non-US #1 */
	Lvn_KeyCode_World2          = 162,      /* non-US #2 */

	/* Function keys */
	Lvn_KeyCode_Escape          = 256,
	Lvn_KeyCode_Enter           = 257,
	Lvn_KeyCode_Tab             = 258,
	Lvn_KeyCode_Backspace       = 259,
	Lvn_KeyCode_Insert          = 260,
	Lvn_KeyCode_Delete          = 261,
	Lvn_KeyCode_Right           = 262,
	Lvn_KeyCode_Left            = 263,
	Lvn_KeyCode_Down            = 264,
	Lvn_KeyCode_Up              = 265,
	Lvn_KeyCode_PageUp          = 266,
	Lvn_KeyCode_PageDown        = 267,
	Lvn_KeyCode_Home            = 268,
	Lvn_KeyCode_End             = 269,
	Lvn_KeyCode_CapsLock        = 280,
	Lvn_KeyCode_ScrollLock      = 281,
	Lvn_KeyCode_NumLock         = 282,
	Lvn_KeyCode_PrintScreen     = 283,
	Lvn_KeyCode_Pause           = 284,
	Lvn_KeyCode_F1              = 290,
	Lvn_KeyCode_F2              = 291,
	Lvn_KeyCode_F3              = 292,
	Lvn_KeyCode_F4              = 293,
	Lvn_KeyCode_F5              = 294,
	Lvn_KeyCode_F6              = 295,
	Lvn_KeyCode_F7              = 296,
	Lvn_KeyCode_F8              = 297,
	Lvn_KeyCode_F9              = 298,
	Lvn_KeyCode_F10             = 299,
	Lvn_KeyCode_F11             = 300,
	Lvn_KeyCode_F12             = 301,
	Lvn_KeyCode_F13             = 302,
	Lvn_KeyCode_F14             = 303,
	Lvn_KeyCode_F15             = 304,
	Lvn_KeyCode_F16             = 305,
	Lvn_KeyCode_F17             = 306,
	Lvn_KeyCode_F18             = 307,
	Lvn_KeyCode_F19             = 308,
	Lvn_KeyCode_F20             = 309,
	Lvn_KeyCode_F21             = 310,
	Lvn_KeyCode_F22             = 311,
	Lvn_KeyCode_F23             = 312,
	Lvn_KeyCode_F24             = 313,
	Lvn_KeyCode_F25             = 314,
	Lvn_KeyCode_KP_0            = 320,
	Lvn_KeyCode_KP_1            = 321,
	Lvn_KeyCode_KP_2            = 322,
	Lvn_KeyCode_KP_3            = 323,
	Lvn_KeyCode_KP_4            = 324,
	Lvn_KeyCode_KP_5            = 325,
	Lvn_KeyCode_KP_6            = 326,
	Lvn_KeyCode_KP_7            = 327,
	Lvn_KeyCode_KP_8            = 328,
	Lvn_KeyCode_KP_9            = 329,
	Lvn_KeyCode_KP_Decimal      = 330,
	Lvn_KeyCode_KP_Divide       = 331,
	Lvn_KeyCode_KP_Multiply     = 332,
	Lvn_KeyCode_KP_Subtract     = 333,
	Lvn_KeyCode_KP_Add          = 334,
	Lvn_KeyCode_KP_Enter        = 335,
	Lvn_KeyCode_KP_Equal        = 336,
	Lvn_KeyCode_LeftShift       = 340,
	Lvn_KeyCode_LeftControl     = 341,
	Lvn_KeyCode_LeftAlt         = 342,
	Lvn_KeyCode_LeftSuper       = 343,
	Lvn_KeyCode_RightShift      = 344,
	Lvn_KeyCode_RightControl    = 345,
	Lvn_KeyCode_RightAlt        = 346,
	Lvn_KeyCode_RightSuper      = 347,
	Lvn_KeyCode_Menu            = 348,
};

/* Mouse Button Codes */
enum LvnMouseButtonCodes
{
	Lvn_MouseButton_1           = 0,
	Lvn_MouseButton_2           = 1,
	Lvn_MouseButton_3           = 2,
	Lvn_MouseButton_4           = 3,
	Lvn_MouseButton_5           = 4,
	Lvn_MouseButton_6           = 5,
	Lvn_MouseButton_7           = 6,
	Lvn_MouseButton_8           = 7,
	Lvn_MouseButton_Last        = Lvn_MouseButton_8,
	Lvn_MouseButton_Left        = Lvn_MouseButton_1,
	Lvn_MouseButton_Right       = Lvn_MouseButton_2,
	Lvn_MouseButton_Middle      = Lvn_MouseButton_3,
};

/* Mouse Mode Codes */
enum LvnMouseCursorModes
{
	Lvn_MouseCursor_Arrow           = 0,
	Lvn_MouseCursor_Ibeam           = 1,
	Lvn_MouseCursor_Crosshair       = 2,
	Lvn_MouseCursor_PointingHand    = 3,
	Lvn_MouseCursor_ResizeEW        = 4,
	Lvn_MouseCursor_ResizeNS        = 5,
	Lvn_MouseCursor_ResizeNWSE      = 6,
	Lvn_MouseCursor_ResizeNESW      = 7,
	Lvn_MouseCursor_ResizeAll       = 8,
	Lvn_MouseCursor_NotAllowed      = 9,
	Lvn_MouseCursor_HResize         = Lvn_MouseCursor_ResizeEW,
	Lvn_MouseCursor_VRrsize         = Lvn_MouseCursor_ResizeNS,
	Lvn_MouseCursor_Hand            = Lvn_MouseCursor_PointingHand,
	Lvn_MouseCursor_Max             = 10,
};

/* Logging Types */
enum LvnLogLevel
{
	Lvn_LogLevel_None       = 0,
	Lvn_LogLevel_Trace      = 1,
	Lvn_LogLevel_Debug      = 2,
	Lvn_LogLevel_Info       = 3,
	Lvn_LogLevel_Warn       = 4,
	Lvn_LogLevel_Error      = 5,
	Lvn_LogLevel_Fatal      = 6,
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
	Lvn_EventCategory_Application   = (1U << 0),
	Lvn_EventCategory_Input         = (1U << 1),
	Lvn_EventCategory_Keyboard      = (1U << 2),
	Lvn_EventCategory_Mouse         = (1U << 3),
	Lvn_EventCategory_MouseButton   = (1U << 4),
	Lvn_EventCategory_Window        = (1U << 5),
};

enum LvnWindowApi
{
	Lvn_WindowApi_None = 0,
	Lvn_WindowApi_Glfw,
	// Lvn_WindowApi_Win32,

	Lvn_WindowApi_glfw  = Lvn_WindowApi_Glfw,
	Lvn_WindowApi_GLFW  = Lvn_WindowApi_Glfw,
	// Lvn_WindowApi_win32 = Lvn_WindowApi_Win32,
	// Lvn_WindowApi_WIN32 = Lvn_WindowApi_Win32,
};

/* [Graphics Enums] */
enum LvnBufferType
{
	Lvn_BufferType_Unknown         = 0,
	Lvn_BufferType_Vertex          = (1U << 0),
	Lvn_BufferType_Index           = (1U << 1),
	Lvn_BufferType_Uniform         = (1U << 2),
	Lvn_BufferType_Storage         = (1U << 3),
	Lvn_BufferType_DynamicVertex   = (1U << 4),
	Lvn_BufferType_DynamicIndex    = (1U << 5),
	Lvn_BufferType_DynamicUniform  = (1U << 6),
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
	Lvn_ColorBlendOperation_Add                 = 0,
	Lvn_ColorBlendOperation_Subtract            = 1,
	Lvn_ColorBlendOperation_ReverseSubtract     = 2,
	Lvn_ColorBlendOperation_Min                 = 3,
	Lvn_ColorBlendOperation_Max                 = 4,
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
	Lvn_CompareOperation_Never          = 0,
	Lvn_CompareOperation_Less           = 1,
	Lvn_CompareOperation_Equal          = 2,
	Lvn_CompareOperation_LessOrEqual    = 3,
	Lvn_CompareOperation_Greater        = 4,
	Lvn_CompareOperation_NotEqual       = 5,
	Lvn_CompareOperation_GreaterOrEqual = 6,
	Lvn_CompareOperation_Always         = 7,
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
	Lvn_DescriptorType_Sampler,
	Lvn_DescriptorType_CombinedImageSampler,
	Lvn_DescriptorType_SampledImage,
	Lvn_DescriptorType_UniformBuffer,
	Lvn_DescriptorType_StorageBuffer,
	// Lvn_DescriptorType_StorageImage,
	// Lvn_DescriptorType_UniformTexelBuffer,
	// Lvn_DescriptorType_StorageTexelBuffer,
	// Lvn_DescriptorType_UniformBufferDynamic,
	// Lvn_DescriptorType_StorageBufferDynamic,
	// Lvn_DescriptorType_InputAttachment,
};

enum LvnGraphicsApi
{
	Lvn_GraphicsApi_None = 0,
	Lvn_GraphicsApi_OpenGL,
	Lvn_GraphicsApi_Vulkan,

	Lvn_GraphicsApi_opengl = Lvn_GraphicsApi_OpenGL,
	Lvn_GraphicsApi_vulkan = Lvn_GraphicsApi_Vulkan,
};

enum LvnPhysicalDeviceType
{
	Lvn_PhysicalDeviceType_Other           = 0,
	Lvn_PhysicalDeviceType_Integrated_GPU  = 1,
	Lvn_PhysicalDeviceType_Discrete_GPU    = 2,
	Lvn_PhysicalDeviceType_Virtual_GPU     = 3,
	Lvn_PhysicalDeviceType_CPU             = 4,

	Lvn_PhysicalDeviceType_Unknown = Lvn_PhysicalDeviceType_Other,
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
	Lvn_StencilOperation_Keep              = 0,
	Lvn_StencilOperation_Zero              = 1,
	Lvn_StencilOperation_Replace           = 2,
	Lvn_StencilOperation_IncrementAndClamp = 3,
	Lvn_StencilOperation_DecrementAndClamp = 4,
	Lvn_StencilOperation_Invert            = 5,
	Lvn_StencilOperation_IncrementAndWrap  = 6,
	Lvn_StencilOperation_DecrementAndWrap  = 7,
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
struct LvnBuffer;
struct LvnBufferCreateInfo;
struct LvnCamera;
struct LvnCameraCreateInfo;
struct LvnCharset;
struct LvnContext;
struct LvnContextCreateInfo;
struct LvnCubemap;
struct LvnCubemapCreateInfo;
struct LvnDescriptorBinding;
struct LvnDescriptorLayout;
struct LvnDescriptorLayoutCreateInfo;
struct LvnDescriptorSet;
struct LvnDescriptorUpdateInfo;
struct LvnEvent;
struct LvnFont;
struct LvnFontGlyph;
struct LvnFrameBuffer;
struct LvnFrameBufferColorAttachment;
struct LvnFrameBufferCreateInfo;
struct LvnFrameBufferDepthAttachment;
struct LvnGraphicsContext;
struct LvnImageData;
struct LvnKeyHoldEvent;
struct LvnKeyPressedEvent;
struct LvnKeyReleasedEvent;
struct LvnKeyTypedEvent;
struct LvnLogger;
struct LvnLoggerCreateInfo;
struct LvnLogMessage;
struct LvnLogPattern;
struct LvnMaterial;
struct LvnMemAllocStructInfo;
struct LvnMesh;
struct LvnMeshCreateInfo;
struct LvnMeshTextureBindings;
struct LvnModel;
struct LvnMouseButtonPressedEvent;
struct LvnMouseButtonReleasedEvent;
struct LvnMouseMovedEvent;
struct LvnMouseScrolledEvent;
struct LvnNetworkMessage;
struct LvnNetworkMessageHeader;
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
struct LvnRenderInitInfo;
struct LvnRenderPass;
struct LvnShader;
struct LvnShaderCreateInfo;
struct LvnSound;
struct LvnSoundBoard;
struct LvnSoundCreateInfo;
struct LvnTexture;
struct LvnTextureCreateInfo;
struct LvnUniformBuffer;
struct LvnUniformBufferCreateInfo;
struct LvnVertex;
struct LvnVertexAttribute;
struct LvnVertexBindingDescription;
struct LvnWindow;
struct LvnWindowCloseEvent;
struct LvnWindowContext;
struct LvnWindowCreateInfo;
struct LvnWindowData;
struct LvnWindowEvent;
struct LvnWindowFocusEvent;
struct LvnWindowFramebufferResizeEvent;
struct LvnWindowIconData;
struct LvnWindowLostFocusEvent;
struct LvnWindowMovedEvent;
struct LvnWindowResizeEvent;


template<typename T>
struct LvnPair;

template<typename T1, typename T2>
struct LvnDoublePair;

template<typename T>
class LvnData;

class LvnTimer;
class LvnThreadPool;

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

template<typename T>
struct LvnQuat_t;

typedef LvnVec2_t<float>                LvnVec2;
typedef LvnVec3_t<float>                LvnVec3;
typedef LvnVec4_t<float>                LvnVec4;
typedef LvnVec2_t<int>                  LvnVec2i;
typedef LvnVec3_t<int>                  LvnVec3i;
typedef LvnVec4_t<int>                  LvnVec4i;
typedef LvnVec2_t<unsigned int>         LvnVec2ui;
typedef LvnVec3_t<unsigned int>         LvnVec3ui;
typedef LvnVec4_t<unsigned int>         LvnVec4ui;
typedef LvnVec2_t<float>                LvnVec2f;
typedef LvnVec3_t<float>                LvnVec3f;
typedef LvnVec4_t<float>                LvnVec4f;
typedef LvnVec2_t<double>               LvnVec2d;
typedef LvnVec3_t<double>               LvnVec3d;
typedef LvnVec4_t<double>               LvnVec4d;
typedef LvnVec2_t<bool>                 LvnVec2b;
typedef LvnVec3_t<bool>                 LvnVec3b;
typedef LvnVec4_t<bool>                 LvnVec4b;

typedef LvnMat2x2_t<float>              LvnMat2;
typedef LvnMat3x3_t<float>              LvnMat3;
typedef LvnMat4x4_t<float>              LvnMat4;
typedef LvnMat2x3_t<float>              LvnMat2x3;
typedef LvnMat2x4_t<float>              LvnMat2x4;
typedef LvnMat3x2_t<float>              LvnMat3x2;
typedef LvnMat3x4_t<float>              LvnMat3x4;
typedef LvnMat4x2_t<float>              LvnMat4x2;
typedef LvnMat4x3_t<float>              LvnMat4x3;
typedef LvnMat2x2_t<int>                LvnMat2i;
typedef LvnMat3x3_t<int>                LvnMat3i;
typedef LvnMat4x4_t<int>                LvnMat4i;
typedef LvnMat2x3_t<int>                LvnMat2x3i;
typedef LvnMat2x4_t<int>                LvnMat2x4i;
typedef LvnMat3x2_t<int>                LvnMat3x2i;
typedef LvnMat3x4_t<int>                LvnMat3x4i;
typedef LvnMat4x2_t<int>                LvnMat4x2i;
typedef LvnMat4x3_t<int>                LvnMat4x3i;
typedef LvnMat2x2_t<unsigned int>       LvnMat2ui;
typedef LvnMat3x3_t<unsigned int>       LvnMat3ui;
typedef LvnMat4x4_t<unsigned int>       LvnMat4ui;
typedef LvnMat2x3_t<unsigned int>       LvnMat2x3ui;
typedef LvnMat2x4_t<unsigned int>       LvnMat2x4ui;
typedef LvnMat3x2_t<unsigned int>       LvnMat3x2ui;
typedef LvnMat3x4_t<unsigned int>       LvnMat3x4ui;
typedef LvnMat4x2_t<unsigned int>       LvnMat4x2ui;
typedef LvnMat4x3_t<unsigned int>       LvnMat4x3ui;
typedef LvnMat2x2_t<float>              LvnMat2f;
typedef LvnMat3x3_t<float>              LvnMat3f;
typedef LvnMat4x4_t<float>              LvnMat4f;
typedef LvnMat2x3_t<float>              LvnMat2x3f;
typedef LvnMat2x4_t<float>              LvnMat2x4f;
typedef LvnMat3x2_t<float>              LvnMat3x2f;
typedef LvnMat3x4_t<float>              LvnMat3x4f;
typedef LvnMat4x2_t<float>              LvnMat4x2f;
typedef LvnMat4x3_t<float>              LvnMat4x3f;
typedef LvnMat2x2_t<double>             LvnMat2d;
typedef LvnMat3x3_t<double>             LvnMat3d;
typedef LvnMat4x4_t<double>             LvnMat4d;
typedef LvnMat2x3_t<double>             LvnMat2x3d;
typedef LvnMat2x4_t<double>             LvnMat2x4d;
typedef LvnMat3x2_t<double>             LvnMat3x2d;
typedef LvnMat3x4_t<double>             LvnMat3x4d;
typedef LvnMat4x2_t<double>             LvnMat4x2d;
typedef LvnMat4x3_t<double>             LvnMat4x3d;

typedef LvnQuat_t<float>                LvnQuat;
typedef LvnQuat_t<int>                  LvnQuati;
typedef LvnQuat_t<unsigned int>         LvnQuatui;
typedef LvnQuat_t<float>                LvnQuatf;
typedef LvnQuat_t<double>               LvnQuatd;

typedef LvnData<uint8_t>                LvnBin;

// ---------------------------------------------
// [SECTION]: Functions
// ---------------------------------------------
// - All functions are declared in the lvn namespace

namespace lvn
{
	typedef LvnVec2_t<float>                vec2;
	typedef LvnVec3_t<float>                vec3;
	typedef LvnVec4_t<float>                vec4;
	typedef LvnVec2_t<int>                  vec2i;
	typedef LvnVec3_t<int>                  vec3i;
	typedef LvnVec4_t<int>                  vec4i;
	typedef LvnVec2_t<unsigned int>         vec2ui;
	typedef LvnVec3_t<unsigned int>         vec3ui;
	typedef LvnVec4_t<unsigned int>         vec4ui;
	typedef LvnVec2_t<float>                vec2f;
	typedef LvnVec3_t<float>                vec3f;
	typedef LvnVec4_t<float>                vec4f;
	typedef LvnVec2_t<double>               vec2d;
	typedef LvnVec3_t<double>               vec3d;
	typedef LvnVec4_t<double>               vec4d;
	typedef LvnVec2_t<bool>                 vec2b;
	typedef LvnVec3_t<bool>                 vec3b;
	typedef LvnVec4_t<bool>                 vec4b;

	typedef LvnMat2x2_t<float>              mat2;
	typedef LvnMat3x3_t<float>              mat3;
	typedef LvnMat4x4_t<float>              mat4;
	typedef LvnMat2x3_t<float>              mat2x3;
	typedef LvnMat2x4_t<float>              mat2x4;
	typedef LvnMat3x2_t<float>              mat3x2;
	typedef LvnMat3x4_t<float>              mat3x4;
	typedef LvnMat4x2_t<float>              mat4x2;
	typedef LvnMat4x3_t<float>              mat4x3;
	typedef LvnMat2x2_t<int>                mat2i;
	typedef LvnMat3x3_t<int>                mat3i;
	typedef LvnMat4x4_t<int>                mat4i;
	typedef LvnMat2x3_t<int>                mat2x3i;
	typedef LvnMat2x4_t<int>                mat2x4i;
	typedef LvnMat3x2_t<int>                mat3x2i;
	typedef LvnMat3x4_t<int>                mat3x4i;
	typedef LvnMat4x2_t<int>                mat4x2i;
	typedef LvnMat4x3_t<int>                mat4x3i;
	typedef LvnMat2x2_t<unsigned int>       mat2ui;
	typedef LvnMat3x3_t<unsigned int>       mat3ui;
	typedef LvnMat4x4_t<unsigned int>       mat4ui;
	typedef LvnMat2x3_t<unsigned int>       mat2x3ui;
	typedef LvnMat2x4_t<unsigned int>       mat2x4ui;
	typedef LvnMat3x2_t<unsigned int>       mat3x2ui;
	typedef LvnMat3x4_t<unsigned int>       mat3x4ui;
	typedef LvnMat4x2_t<unsigned int>       mat4x2ui;
	typedef LvnMat4x3_t<unsigned int>       mat4x3ui;
	typedef LvnMat2x2_t<float>              mat2f;
	typedef LvnMat3x3_t<float>              mat3f;
	typedef LvnMat4x4_t<float>              mat4f;
	typedef LvnMat2x3_t<float>              mat2x3f;
	typedef LvnMat2x4_t<float>              mat2x4f;
	typedef LvnMat3x2_t<float>              mat3x2f;
	typedef LvnMat3x4_t<float>              mat3x4f;
	typedef LvnMat4x2_t<float>              mat4x2f;
	typedef LvnMat4x3_t<float>              mat4x3f;
	typedef LvnMat2x2_t<double>             mat2d;
	typedef LvnMat3x3_t<double>             mat3d;
	typedef LvnMat4x4_t<double>             mat4d;
	typedef LvnMat2x3_t<double>             mat2x3d;
	typedef LvnMat2x4_t<double>             mat2x4d;
	typedef LvnMat3x2_t<double>             mat3x2d;
	typedef LvnMat3x4_t<double>             mat3x4d;
	typedef LvnMat4x2_t<double>             mat4x2d;
	typedef LvnMat4x3_t<double>             mat4x3d;

	typedef LvnQuat_t<float>                quat;
	typedef LvnQuat_t<int>                  quati;
	typedef LvnQuat_t<unsigned int>         quatui;
	typedef LvnQuat_t<float>                quatf;
	typedef LvnQuat_t<double>               quatd;

	typedef LvnVertex                       vertex;


	/* [API] */
	LvnContext*                     getContext();                                       // pointer to the Levikno Context created from the library

	LVN_API LvnResult               createContext(LvnContextCreateInfo* createInfo);
	LVN_API void                    terminateContext();

	LVN_API int                     dateGetYear();                                      // get the year number (eg. 2025)
	LVN_API int                     dateGetYear02d();                                   // get the last two digits of the year number (eg. 25)
	LVN_API int                     dateGetMonth();                                     // get the month number (1...12)
	LVN_API int                     dateGetDay();                                       // get the date number (1...31)
	LVN_API int                     dateGetHour();                                      // get the hour of the current day in 24 hour format (0...24)
	LVN_API int                     dateGetHour12();                                    // get the hour of the current day in 12 hour format (0...12)
	LVN_API int                     dateGetMinute();                                    // get the minute of the current day (0...60)
	LVN_API int                     dateGetSecond();                                    // get the second of the current dat (0...60)
	LVN_API long long               dateGetSecondsSinceEpoch();                         // get the time in seconds since 00::00:00 UTC 1 January 1970

	LVN_API const char*             dateGetMonthName();                                 // get the current month name (eg. January, April)
	LVN_API const char*             dateGetMonthNameShort();                            // get the current month shortened name (eg. Jan, Apr)
	LVN_API const char*             dateGetWeekDayName();                               // get the current day name in the week (eg. Monday, Friday)
	LVN_API const char*             dateGetWeekDayNameShort();                          // get the current day shortened name in the week (eg. Mon, Fri)
	LVN_API const char*             dateGetTimeMeridiem();                              // get the time meridiem of the current day (eg. AM, PM)
	LVN_API const char*             dateGetTimeMeridiemLower();                         // get the time meridiem of the current day in lower case (eg. am, pm)

	LVN_API std::string             dateGetTimeHHMMSS();                                // get the time in HH:MM:SS format (eg. 14:34:54)
	LVN_API std::string             dateGetTime12HHMMSS();                              // get the time in HH:MM:SS 12 hour format (eg. 2:23:14)
	LVN_API std::string             dateGetYearStr();                                   // get the current year number as a string
	LVN_API std::string             dateGetYear02dStr();                                // get the last two digits of the current year number as a string
	LVN_API std::string             dateGetMonthNumStr();                               // get the current month number as a string
	LVN_API std::string             dateGetDayNumStr();                                 // get the current day number as a string
	LVN_API std::string             dateGetHourNumStr();                                // get the current hour number as a string
	LVN_API std::string             dateGetHour12NumStr();                              // get the current hour number in 12 hour format as a string
	LVN_API std::string             dateGetMinuteNumStr();                              // get the current minute as a string
	LVN_API std::string             dateGetSecondNumStr();                              // get the current second as a string

	LVN_API float                   getContextTime();                                   // get time in seconds since context creation

	LVN_API std::string             loadFileSrc(const char* filepath);                                     // get the src contents from a text file format, filepath must be a valid path to a text file
	LVN_API LvnBin                  loadFileSrcBin(const char* filepath);                                  // get the binary data contents (in unsigned char*) from a binary file (eg .spv), filepath must be a valid path to a binary file

	LVN_API LvnFont                 loadFontFromFileTTF(const char* filepath, uint32_t fontSize, LvnCharset charset);    // get the font data from a ttf font file, font data will be stored in a LvnImageData struct which is an atlas texture containing all the font glyphs and their UV positions
	LVN_API LvnFontGlyph            fontGetGlyph(LvnFont* font, int8_t codepoint);


	LVN_API void*                   memAlloc(size_t size);                              // custom memory allocation function that allocates memory given the size of memory, note that function is connected with the context and will keep track of allocation counts, will increment number of allocations per use
	LVN_API void                    memFree(void* ptr);                                 // custom memory free function, note that it keeps track of memory allocations remaining, decrements number of allocations per use with lvn::memAlloc
	LVN_API void*                   memRealloc(void* ptr, size_t size);                 // custom memory realloc function

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
	// Ex: The default log pattern is: "[%Y-%m-%d] [%T] [%#%l%^] %n: %v%$"
	//     Which could output: "[04-06-2025] [14:25:11] [\x1b[0;32minfo\x1b[0m] CORE: some informational message\n"

	LVN_API LvnResult                   logInit();                                                          // initiates logging
	LVN_API void                        logEnable(bool enable);                                             // enable or disable logging
	LVN_API void                        logEnableCoreLogging(bool enable);                                  // enable or disable logging from the core logger
	LVN_API void                        logSetLevel(LvnLogger* logger, LvnLogLevel level);                  // sets the log level of logger, will only print messages with set log level and higher
	LVN_API bool                        logCheckLevel(LvnLogger* logger, LvnLogLevel level);                // checks level with loger, returns true if level is the same or higher level than the level of the logger
	LVN_API void                        logRenameLogger(LvnLogger* logger, const char* name);               // renames the name of the logger
	LVN_API void                        logOutputMessage(LvnLogger* logger, LvnLogMessage* msg);            // prints the log message
	LVN_API void                        logMessage(LvnLogger* logger, LvnLogLevel level, const char* msg);  // log message with given log level
	LVN_API void                        logMessageTrace(LvnLogger* logger, const char* fmt, ...);           // log message with level trace; ANSI code "\x1b[0;37m"
	LVN_API void                        logMessageDebug(LvnLogger* logger, const char* fmt, ...);           // log message with level debug; ANSI code "\x1b[0;34m"
	LVN_API void                        logMessageInfo(LvnLogger* logger, const char* fmt, ...);            // log message with level info;  ANSI code "\x1b[0;32m"
	LVN_API void                        logMessageWarn(LvnLogger* logger, const char* fmt, ...);            // log message with level warn;  ANSI code "\x1b[1;33m"
	LVN_API void                        logMessageError(LvnLogger* logger, const char* fmt, ...);           // log message with level error; ANSI code "\x1b[1;31m"
	LVN_API void                        logMessageFatal(LvnLogger* logger, const char* fmt, ...);           // log message with level fatal; ANSI code "\x1b[1;37;41m"
	LVN_API LvnLogger*                  logGetCoreLogger();
	LVN_API LvnLogger*                  logGetClientLogger();
	LVN_API const char*                 logGetANSIcodeColor(LvnLogLevel level);                             // get the ANSI color code of the log level in a string
	LVN_API LvnResult                   logSetPatternFormat(LvnLogger* logger, const char* patternfmt);     // set the log pattern of the logger; messages outputed from that logger will be in this format
	LVN_API LvnResult                   logAddPatterns(LvnLogPattern* pLogPatterns, uint32_t count);        // add user defined log patterns to the library

	LVN_API LvnResult                   createLogger(LvnLogger** logger, LvnLoggerCreateInfo* loggerCreateInfo);
	LVN_API void                        destroyLogger(LvnLogger* logger);


	/* [Events] */
	// Use these function within the call back function of LvnWindow (if set)
	LVN_API bool                        dispatchKeyHoldEvent(LvnEvent* event, bool(*func)(LvnKeyHoldEvent*, void*));
	LVN_API bool                        dispatchKeyPressedEvent(LvnEvent* event, bool(*func)(LvnKeyPressedEvent*, void*));
	LVN_API bool                        dispatchKeyReleasedEvent(LvnEvent* event, bool(*func)(LvnKeyReleasedEvent*, void*));
	LVN_API bool                        dispatchKeyTypedEvent(LvnEvent* event, bool(*func)(LvnKeyTypedEvent*, void*));
	LVN_API bool                        dispatchMouseButtonPressedEvent(LvnEvent* event, bool(*func)(LvnMouseButtonPressedEvent*, void*));
	LVN_API bool                        dispatchMouseButtonReleasedEvent(LvnEvent* event, bool(*func)(LvnMouseButtonReleasedEvent*, void*));
	LVN_API bool                        dispatchMouseMovedEvent(LvnEvent* event, bool(*func)(LvnMouseMovedEvent*, void*));
	LVN_API bool                        dispatchMouseScrolledEvent(LvnEvent* event, bool(*func)(LvnMouseScrolledEvent*, void*));
	LVN_API bool                        dispatchWindowCloseEvent(LvnEvent* event, bool(*func)(LvnWindowCloseEvent*, void*));
	LVN_API bool                        dispatchWindowFramebufferResizeEvent(LvnEvent* event, bool(*func)(LvnWindowFramebufferResizeEvent*, void*));
	LVN_API bool                        dispatchWindowFocusEvent(LvnEvent* event, bool(*func)(LvnWindowFocusEvent*, void*));
	LVN_API bool                        dispatchWindowLostFocusEvent(LvnEvent* event, bool(*func)(LvnWindowLostFocusEvent*, void*));
	LVN_API bool                        dispatchWindowMovedEvent(LvnEvent* event, bool(*func)(LvnWindowMovedEvent*, void*));
	LVN_API bool                        dispatchWindowResizeEvent(LvnEvent* event, bool(*func)(LvnWindowResizeEvent*, void*));

	/* [Window] */
	LVN_API LvnWindowApi                getWindowApi();
	LVN_API const char*                 getWindowApiName();

	LVN_API LvnResult                   createWindow(LvnWindow** window, LvnWindowCreateInfo* createInfo);
	LVN_API void                        destroyWindow(LvnWindow* window);

	LVN_API void                        windowUpdate(LvnWindow* window);
	LVN_API bool                        windowOpen(LvnWindow* window);
	LVN_API LvnPair<int>                windowGetDimensions(LvnWindow* window);
	LVN_API int                         windowGetWidth(LvnWindow* window);
	LVN_API int                         windowGetHeight(LvnWindow* window);
	LVN_API void                        windowSetEventCallback(LvnWindow* window, void (*callback)(LvnEvent*), void* userData);
	LVN_API void                        windowSetVSync(LvnWindow* window, bool enable);
	LVN_API bool                        windowGetVSync(LvnWindow* window);
	LVN_API void*                       windowGetNativeWindow(LvnWindow* window);
	LVN_API LvnRenderPass*              windowGetRenderPass(LvnWindow* window);
	LVN_API void                        windowSetContextCurrent(LvnWindow* window);
	LVN_API LvnWindowCreateInfo         windowCreateInfoGetConfig(int width, int height, const char* title);

	/* [Input] */
	LVN_API bool                        keyPressed(LvnWindow* window, int keycode);
	LVN_API bool                        keyReleased(LvnWindow* window, int keycode);
	LVN_API bool                        mouseButtonPressed(LvnWindow* window, int button);
	LVN_API bool                        mouseButtonReleased(LvnWindow* window, int button);

	LVN_API LvnPair<float>              mouseGetPos(LvnWindow* window);
	LVN_API void                        mouseGetPos(LvnWindow* window, float* xpos, float* ypos);
	LVN_API float                       mouseGetX(LvnWindow* window);
	LVN_API float                       mouseGetY(LvnWindow* window);

	LVN_API LvnPair<int>                windowGetPos(LvnWindow* window);
	LVN_API void                        windowGetPos(LvnWindow* window, int* xpos, int* ypos);
	LVN_API LvnPair<int>                windowGetSize(LvnWindow* window);
	LVN_API void                        windowGetSize(LvnWindow* window, int* width, int* height);


	/* [Graphics API] */
	LVN_API LvnGraphicsApi              getGraphicsApi();
	LVN_API const char*                 getGraphicsApiName();
	LVN_API void                        getPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* deviceCount);
	LVN_API LvnPhysicalDeviceInfo       getPhysicalDeviceInfo(LvnPhysicalDevice* physicalDevice);
	LVN_API LvnResult                   checkPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice);
	LVN_API LvnResult                   renderInit(LvnRenderInitInfo* renderInfo);
	LVN_API LvnClipRegion               getRenderClipRegionEnum();

	LVN_API void                        renderClearColor(LvnWindow* window, float r, float g, float b, float a);
	LVN_API void                        renderCmdDraw(LvnWindow* window, uint32_t vertexCount);
	LVN_API void                        renderCmdDrawIndexed(LvnWindow* window, uint32_t indexCount);
	LVN_API void                        renderCmdDrawInstanced(LvnWindow* window, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
	LVN_API void                        renderCmdDrawIndexedInstanced(LvnWindow* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
	LVN_API void                        renderCmdSetStencilReference(uint32_t reference);
	LVN_API void                        renderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask);
	LVN_API void                        renderBeginNextFrame(LvnWindow* window);                                                                          // begins the next frame of the window
	LVN_API void                        renderDrawSubmit(LvnWindow* window);                                                                              // submits all draw commands recorded and presents to window
	LVN_API void                        renderBeginCommandRecording(LvnWindow* window);                                                                   // begins command buffer when recording draw commands start
	LVN_API void                        renderEndCommandRecording(LvnWindow* window);                                                                     // ends command buffer when finished recording draw commands
	LVN_API void                        renderCmdBeginRenderPass(LvnWindow* window);                                                                      // begins renderpass when rendering starts
	LVN_API void                        renderCmdEndRenderPass(LvnWindow* window);                                                                        // ends renderpass when rendering has finished
	LVN_API void                        renderCmdBindPipeline(LvnWindow* window, LvnPipeline* pipeline);                                                  // bind a pipeline to begin shading during rendering
	LVN_API void                        renderCmdBindVertexBuffer(LvnWindow* window, LvnBuffer* buffer);                                                  // binds the vertex buffer within an LvnBuffer object
	LVN_API void                        renderCmdBindIndexBuffer(LvnWindow* window, LvnBuffer* buffer);                                                   // binds the index buffer within an LvnBuffer object
	LVN_API void                        renderCmdBindDescriptorSets(LvnWindow* window, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets); // bind multiple descriptor sets to the shader (if multiple sets are used), Note that descriptor sets must be in order to how the sets are ordered in the pipeline
	LVN_API void                        renderCmdBeginFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);                                        // begins the framebuffer for recording offscreen render calls, similar to beginning the render pass
	LVN_API void                        renderCmdEndFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);                                          // ends recording to the framebuffer

	LVN_API LvnResult                   createShaderFromSrc(LvnShader** shader, LvnShaderCreateInfo* createInfo);                                         // create shader with the source code as input
	LVN_API LvnResult                   createShaderFromFileBin(LvnShader** shader, LvnShaderCreateInfo* createInfo);                                     // create shader with the file paths to the binary files (.spv) as input
	LVN_API LvnResult                   createShaderFromFileSrc(LvnShader** shader, LvnShaderCreateInfo* createInfo);                                     // create shader with the file paths to the source files as input
	LVN_API LvnResult                   createDescriptorLayout(LvnDescriptorLayout** descriptorLayout, LvnDescriptorLayoutCreateInfo* createInfo);        // create descriptor layout for the pipeline
	LVN_API LvnResult                   createDescriptorSet(LvnDescriptorSet** descriptorSet, LvnDescriptorLayout* descriptorLayout);                     // create descriptor set to uplaod uniform data to pipeline
	LVN_API LvnResult                   createPipeline(LvnPipeline** pipeline, LvnPipelineCreateInfo* createInfo);                                        // create pipeline to describe shading specifications
	LVN_API LvnResult                   createFrameBuffer(LvnFrameBuffer** frameBuffer, LvnFrameBufferCreateInfo* createInfo);                            // create framebuffer to render images to
	LVN_API LvnResult                   createBuffer(LvnBuffer** buffer, LvnBufferCreateInfo* createInfo);                                                // create a single buffer object that can hold both the vertex and index buffers
	LVN_API LvnResult                   createUniformBuffer(LvnUniformBuffer** uniformBuffer, LvnUniformBufferCreateInfo* createInfo);                    // create a uniform buffer object to send changing data to the shader pipeline
	LVN_API LvnResult                   createTexture(LvnTexture** texture, LvnTextureCreateInfo* createInfo);                                            // create a texture object to hold image pixel data and sampler
	LVN_API LvnResult                   createCubemap(LvnCubemap** cubemap, LvnCubemapCreateInfo* createInfo);                                            // create a cubemap texture object that holds the textures of the cubemap


	LVN_API void                        destroyShader(LvnShader* shader);                                                                                 // destroy shader module object
	LVN_API void                        destroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout);                                                   // destroy descriptor layout
	LVN_API void                        destroyDescriptorSet(LvnDescriptorSet* descriptorSet);                                                            // destroy descriptor set object
	LVN_API void                        destroyPipeline(LvnPipeline* pipeline);                                                                           // destroy pipeline object
	LVN_API void                        destroyFrameBuffer(LvnFrameBuffer* frameBuffer);                                                                  // destroy framebuffer object
	LVN_API void                        destroyBuffer(LvnBuffer* buffer);                                                                                 // destory buffers object
	LVN_API void                        destroyUniformBuffer(LvnUniformBuffer* uniformBuffer);                                                            // destroy uniform buffer object
	LVN_API void                        destroyTexture(LvnTexture* texture);                                                                              // destroy texture object
	LVN_API void                        destroyCubemap(LvnCubemap* cubemap);                                                                              // destroy cubemap object

	LVN_API void                        pipelineSpecificationSetConfig(LvnPipelineSpecification* pipelineSpecification);
	LVN_API LvnPipelineSpecification    pipelineSpecificationGetConfig();

	LVN_API void                        bufferUpdateVertexData(LvnBuffer* buffer, void* vertices, uint32_t size, uint32_t offset);
	LVN_API void                        bufferUpdateIndexData(LvnBuffer* buffer, uint32_t* indices, uint32_t size, uint32_t offset);
	LVN_API void                        bufferResizeVertexBuffer(LvnBuffer* buffer, uint32_t size);
	LVN_API void                        bufferResizeIndexBuffer(LvnBuffer* buffer, uint32_t size);

	LVN_API uint32_t                    getVertexDataTypeSize(LvnVertexDataType type);

	LVN_API LvnTexture*                 cubemapGetTextureData(LvnCubemap* cubemap);                                                                               // get the cubemap texture from the cubemap

	LVN_API void                        updateUniformBufferData(LvnWindow* window, LvnUniformBuffer* uniformBuffer, void* data, uint64_t size);                   // update the data stored in a uniform or storage buffer
	LVN_API void                        updateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count);           // update the descriptor content within a descroptor set

	LVN_API LvnTexture*                 frameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex);                                               // get the texture image data (render pass attachment) from the framebuffer via the attachment index
	LVN_API LvnRenderPass*              frameBufferGetRenderPass(LvnFrameBuffer* frameBuffer);                                                                    // get the render pass from the framebuffer
	LVN_API void                        frameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height);                                          // update the width and height of the new framebuffer (updates the image data dimensions), Note: call only when the image dimensions need to be changed
	LVN_API void                        frameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a);      // set the background color for the framebuffer for offscreen rendering

	LVN_API LvnDepthImageFormat         findSupportedDepthImageFormat(LvnDepthImageFormat* pDepthImageFormats, uint32_t count);

	LVN_API LvnBuffer*                  meshGetBuffer(LvnMesh* mesh);
	LVN_API LvnMat4                     meshGetMatrix(LvnMesh* mesh);
	LVN_API void                        meshSetMatrix(LvnMesh* mesh, const LvnMat4& matrix);
	LVN_API LvnBufferCreateInfo         meshGetVertexBufferCreateInfoConfig(LvnVertex* pVertices, uint32_t vertexCount, uint32_t* pIndices, uint32_t indexCount);

	LVN_API LvnImageData                loadImageData(const char* filepath, int forceChannels = 0, bool flipVertically = false);
	LVN_API LvnImageData                loadImageDataMemory(const uint8_t* data, int length, int forceChannels = 0, bool flipVertically = false);

	LVN_API LvnMesh                     createMesh(LvnMeshCreateInfo* createInfo);                                        // create a mesh object containing the vertex/index buffer, model matrix, and material
	LVN_API void                        destroyMesh(LvnMesh* mesh);                                                       // destroy mesh object

	LVN_API LvnModel                    loadModel(const char* filepath);
	LVN_API void                        freeModel(LvnModel* model);

	LVN_API LvnCamera                   cameraConfigInit(LvnCameraCreateInfo* createInfo);                                // initialize the config of the camera struct given the create info parameters
	LVN_API void                        cameraUpdateMatrix(LvnCamera* camera);                                            // updates the camera matrix given the camera position, orientation, aspect ratio, fov, near plane, and far plane are set, note that it will also update the projection and view matrix 
	LVN_API void                        cameraSetFov(LvnCamera* camera, float fovDeg);                                    // set the fov of the camera in degrees
	LVN_API void                        cameraSetPlane(LvnCamera* camera, float nearPlane, float farPlane);               // set the near and far plane of the camera
	LVN_API void                        cameraSetPos(LvnCamera* camera, const LvnVec3& position);                         // set the position of the camera in coord space
	LVN_API void                        cameraSetOrient(LvnCamera* camera, const LvnVec3& orientation);                   // set the orientation of the camera
	LVN_API void                        cameraSetUpVec(LvnCamera* camera, const LvnVec3& upVector);                       // set the up vector of the camera
	LVN_API float                       cameraGetFov(LvnCamera* camera);                                                  // get the fov of the camera in degrees
	LVN_API float                       cameraGetNearPlane(LvnCamera* camera);                                            // get the near plane of the camera
	LVN_API float                       cameraGetFarPlane(LvnCamera* camera);                                             // get the far plane of the camera
	LVN_API LvnVec3                     cameraGetPos(LvnCamera* camera);                                                  // get the position of the camera in coord space
	LVN_API LvnVec3                     cameraGetOrient(LvnCamera* camera);                                               // get the orientation of the camera
	LVN_API LvnVec3                     cameraGetUpVec(LvnCamera* camera);                                                // get the up vector of the camera

	/* [Audio] */
	LVN_API LvnResult                   createSoundFromFile(LvnSound** sound, LvnSoundCreateInfo* createInfo);
	LVN_API void                        destroySound(LvnSound* sound);
	LVN_API LvnSoundCreateInfo          soundConfigInit(const char* filepath);

	LVN_API void                        soundSetVolume(const LvnSound* sound, float volume);
	LVN_API void                        soundSetPan(const LvnSound* sound, float pan);
	LVN_API void                        soundSetPitch(const LvnSound* sound, float pitch);
	LVN_API void                        soundSetLooping(const LvnSound* sound, bool looping);
	LVN_API void                        soundPlayStart(const LvnSound* sound);
	LVN_API void                        soundPlayStop(const LvnSound* sound);
	LVN_API void                        soundTogglePause(const LvnSound* sound);
	LVN_API bool                        soundIsPlaying(const LvnSound* sound);
	LVN_API uint64_t                    soundGetTimeMiliseconds(const LvnSound* sound);
	LVN_API float                       soundGetLengthSeconds(const LvnSound* sound);
	LVN_API bool                        soundIsAttachedToSoundBoard(const LvnSound* sound);

	LVN_API LvnResult                   createSoundBoard(LvnSoundBoard** soundBoard);
	LVN_API void                        destroySoundBoard(LvnSoundBoard* soundBoard);
	LVN_API LvnResult                   soundBoardAddSound(LvnSoundBoard* soundBoard, LvnSoundCreateInfo* soundInfo, uint32_t id);
	LVN_API void                        soundBoardRemoveSound(LvnSoundBoard* soundBoard, uint32_t id);
	LVN_API const LvnSound*             soundBoardGetSound(LvnSoundBoard* soundBoard, uint32_t id);


	/* [Networking] */
	LVN_API LvnResult                   createSocket();


	/* [Math] */
	template <typename T>
	LVN_API T                           min(const T& n1, const T& n2) { return n1 < n2 ? n1 : n2; }

	template <typename T>
	LVN_API T                           max(const T& n1, const T& n2) { return n1 > n2 ? n1 : n2; }

	template <typename T>
	LVN_API T                           clamp(const T& val, const T& low, const T& high) { return lvn::max(lvn::min(val, high), low); }

	template <typename T>
	LVN_API LvnPair<T>                  midpoint(const T& x1, const T& y1, const T& x2, const T& y2) { return { (x1 + x2) / static_cast<T>(2), (y1 + y2) / static_cast<T>(2) }; }

	template <typename T>
	LVN_API T                           distance(const T& x1, const T& y1, const T& x2, const T& y2) { return sqrt(pow((x1 - x2), static_cast<T>(2)) + pow((y1 - y2), static_cast<T>(2))); }

	template<typename T>
	LVN_API bool                        within(T num, T within, T range) { return num <= (within + range) && num >= (within - range); }
	
	template<typename T>
	LVN_API bool                        within(T num, T within, T lowerRange, T upperRange) { return num <= (within + upperRange) && num >= (within - lowerRange); }

	LVN_API float radians(float deg);          // convert degrees to radians
	LVN_API float degrees(float rad);          // convert radians to degrees
	LVN_API float clampAngle(float rad);       // clamps the given angle in radians to the translated angle between 0 and 2 PI
	LVN_API float clampAngleDeg(float deg);    // clamps the given angle in degrees to the translated angle between 0 and 2 PI
	LVN_API float invSqrt(float num);

	LVN_API LvnVec2f normalize(LvnVec2f v);
	LVN_API LvnVec2d normalize(LvnVec2d v);
	LVN_API LvnVec3f normalize(LvnVec3f v);
	LVN_API LvnVec3d normalize(LvnVec3d v);
	LVN_API LvnVec4f normalize(LvnVec4f v);
	LVN_API LvnVec4d normalize(LvnVec4d v);

	LVN_API LvnVec3f cross(LvnVec3f v1, LvnVec3f v2);
	LVN_API LvnVec3d cross(LvnVec3d v1, LvnVec3d v2);

	template <typename T>
	LVN_API T dot(const LvnVec2_t<T>& v1, const LvnVec2_t<T>& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}

	template <typename T>
	LVN_API T dot(const LvnVec3_t<T>& v1, const LvnVec3_t<T>& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	template <typename T>
	LVN_API T dot(const LvnVec4_t<T>& v1, const LvnVec4_t<T>& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
	}

	template <typename T>
	LVN_API T angle(const LvnVec3_t<T>& v1, const LvnVec3_t<T>& v2)
	{
		return acos(lvn::clamp(lvn::dot(v1, v2), T(-1), T(1)));
	}

	template <typename T>
	LVN_API LvnMat2x2_t<T> inverse(const LvnMat2x2_t<T>& m)
	{
		T oneOverDeterminant = static_cast<T>(1) / (
			+ m[0][0] * m[1][1]
			- m[1][0] * m[0][1]);

		LvnMat2x2_t<T> inverse(
			+ m[1][1] * oneOverDeterminant,
			- m[0][1] * oneOverDeterminant,
			- m[1][0] * oneOverDeterminant,
			+ m[0][0] * oneOverDeterminant);

		return inverse;
	}

	template <typename T>
	LVN_API LvnMat3x3_t<T> inverse(const LvnMat3x3_t<T>& m)
	{
		T oneOverDeterminant = static_cast<T>(1) / (
			+ m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
			- m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
			+ m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]));

		LvnMat3x3_t<T> inverse;
		inverse[0][0] = + (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * oneOverDeterminant;
		inverse[1][0] = - (m[1][0] * m[2][2] - m[2][0] * m[1][2]) * oneOverDeterminant;
		inverse[2][0] = + (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * oneOverDeterminant;
		inverse[0][1] = - (m[0][1] * m[2][2] - m[2][1] * m[0][2]) * oneOverDeterminant;
		inverse[1][1] = + (m[0][0] * m[2][2] - m[2][0] * m[0][2]) * oneOverDeterminant;
		inverse[2][1] = - (m[0][0] * m[2][1] - m[2][0] * m[0][1]) * oneOverDeterminant;
		inverse[0][2] = + (m[0][1] * m[1][2] - m[1][1] * m[0][2]) * oneOverDeterminant;
		inverse[1][2] = - (m[0][0] * m[1][2] - m[1][0] * m[0][2]) * oneOverDeterminant;
		inverse[2][2] = + (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * oneOverDeterminant;

		return inverse;
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> inverse(const LvnMat4x4_t<T>& m)
	{
		T coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
		T coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
		T coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

		T coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
		T coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
		T coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

		T coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
		T coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
		T coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

		T coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
		T coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
		T coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

		T coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
		T coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
		T coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

		T coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
		T coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
		T coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

		LvnVec4_t<T> fac0(coef00, coef00, coef02, coef03);
		LvnVec4_t<T> fac1(coef04, coef04, coef06, coef07);
		LvnVec4_t<T> fac2(coef08, coef08, coef10, coef11);
		LvnVec4_t<T> fac3(coef12, coef12, coef14, coef15);
		LvnVec4_t<T> fac4(coef16, coef16, coef18, coef19);
		LvnVec4_t<T> fac5(coef20, coef20, coef22, coef23);

		LvnVec4_t<T> vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
		LvnVec4_t<T> vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
		LvnVec4_t<T> vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
		LvnVec4_t<T> vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

		LvnVec4_t<T> inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
		LvnVec4_t<T> inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
		LvnVec4_t<T> inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
		LvnVec4_t<T> inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

		LvnVec4_t<T> signA(+1, -1, +1, -1);
		LvnVec4_t<T> signB(-1, +1, -1, +1);
		LvnMat4x4_t<T> inverse(inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB);

		LvnVec4_t<T> row0(inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0]);

		LvnVec4_t<T> dot0(m[0] * row0);
		T dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

		T oneOverDeterminant = static_cast<T>(1) / dot1;

		return inverse * oneOverDeterminant;
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> orthoRHZO(T left, T right, T bottom, T top, T zNear, T zFar)
	{
		LvnMat4x4_t<T> matrix(static_cast<T>(1));
		matrix[0][0] = static_cast<T>(2) / (right - left);
		matrix[1][1] = static_cast<T>(2) / (top - bottom);
		matrix[2][2] = - static_cast<T>(2) / (zFar - zNear);
		matrix[3][0] = - (right + left)  / (right - left);
		matrix[3][1] = - (top + bottom)  / (top - bottom);
		matrix[3][2] = - (zFar + zNear) / (zFar - zNear);
		return matrix;
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> orthoRHNO(T left, T right, T bottom, T top, T zNear, T zFar)
	{
		LvnMat4x4_t<T> matrix(static_cast<T>(1));
		matrix[0][0] = static_cast<T>(2) / (right - left);
		matrix[1][1] = static_cast<T>(2) / (top - bottom);
		matrix[2][2] = - static_cast<T>(2) / (zFar - zNear);
		matrix[3][0] = - (right + left)  / (right - left);
		matrix[3][1] = - (top + bottom)  / (top - bottom);
		matrix[3][2] = - zNear / (zFar - zNear);
		return matrix;
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> orthoLHZO(T left, T right, T bottom, T top, T zNear, T zFar)
	{
		LvnMat4x4_t<T> matrix(static_cast<T>(1));
		matrix[0][0] = static_cast<T>(2) / (right - left);
		matrix[1][1] = static_cast<T>(2) / (top - bottom);
		matrix[2][2] = static_cast<T>(1) / (zFar - zNear);
		matrix[3][0] = - (right + left)  / (right - left);
		matrix[3][1] = - (top + bottom)  / (top - bottom);
		matrix[3][2] = - zNear / (zFar - zNear);
		return matrix;
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> orthoLHNO(T left, T right, T bottom, T top, T zNear, T zFar)
	{
		LvnMat4x4_t<T> matrix(static_cast<T>(1));
		matrix[0][0] = static_cast<T>(2) / (right - left);
		matrix[1][1] = static_cast<T>(2) / (top - bottom);
		matrix[2][2] = static_cast<T>(2) / (zFar - zNear);
		matrix[3][0] = - (right + left)  / (right - left);
		matrix[3][1] = - (top + bottom)  / (top - bottom);
		matrix[3][2] = - (zFar + zNear) / (zFar - zNear);
		return matrix;
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> ortho(T left, T right, T bottom, T top, T zNear, T zFar)
	{
		switch (lvn::getRenderClipRegionEnum())
		{
			case Lvn_ClipRegion_RHZO: { return lvn::orthoRHZO(left, right, bottom, top, zNear, zFar); }
			case Lvn_ClipRegion_RHNO: { return lvn::orthoRHNO(left, right, bottom, top, zNear, zFar); }
			case Lvn_ClipRegion_LHZO: { return lvn::orthoLHZO(left, right, bottom, top, zNear, zFar); }
			case Lvn_ClipRegion_LHNO: { return lvn::orthoLHNO(left, right, bottom, top, zNear, zFar); }

			default: { return lvn::orthoRHNO(left, right, bottom, top, zNear, zFar); } // opengl default
		}
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> perspectiveRHZO(const T& fovy, const T& aspect, const T& zNear, const T& zFar)
	{
		LVN_CORE_ASSERT(aspect >= 0, "aspect ratio cannot be negative");

		T tanHalfFov = static_cast<T>(tan(fovy / 2));

		LvnMat4x4_t<T> matrix(0);
		matrix[0][0] = static_cast<T>(1) / (aspect * tanHalfFov);
		matrix[1][1] = static_cast<T>(1) / (tanHalfFov);
		matrix[2][2] = zFar / (zNear - zFar);
		matrix[2][3] = static_cast<T>(1);
		matrix[3][2] = - (zFar * zNear) / (zFar - zNear);

		return matrix;
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> perspectiveRHNO(const T& fovy, const T& aspect, const T& zNear, const T& zFar)
	{
		LVN_CORE_ASSERT(aspect >= 0, "aspect ratio cannot be negative");

		T tanHalfFov = static_cast<T>(tan(fovy / 2));

		LvnMat4x4_t<T> matrix(0);
		matrix[0][0] = static_cast<T>(1) / (aspect * tanHalfFov);
		matrix[1][1] = static_cast<T>(1) / (tanHalfFov);
		matrix[2][2] = - (zFar + zNear) / (zFar - zNear);
		matrix[2][3] = - static_cast<T>(1);
		matrix[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);

		return matrix;
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> perspectiveLHZO(const T& fovy, const T& aspect, const T& zNear, const T& zFar)
	{
		LVN_CORE_ASSERT(aspect >= 0, "aspect ratio cannot be negative");

		T tanHalfFov = static_cast<T>(tan(fovy / 2));

		LvnMat4x4_t<T> matrix(0);
		matrix[0][0] = static_cast<T>(1) / (aspect * tanHalfFov);
		matrix[1][1] = static_cast<T>(1) / (tanHalfFov);
		matrix[2][2] = zFar / (zFar - zNear);
		matrix[2][3] = static_cast<T>(1);
		matrix[3][2] = - (zFar * zNear) / (zFar - zNear);

		return matrix;
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> perspectiveLHNO(const T& fovy, const T& aspect, const T& zNear, const T& zFar)
	{
		LVN_CORE_ASSERT(aspect >= 0, "aspect ratio cannot be negative");

		T tanHalfFov = static_cast<T>(tan(fovy / 2));

		LvnMat4x4_t<T> matrix(0);
		matrix[0][0] = static_cast<T>(1) / (aspect * tanHalfFov);
		matrix[1][1] = static_cast<T>(1) / (tanHalfFov);
		matrix[2][2] = (zFar + zNear) / (zFar - zNear);
		matrix[2][3] = static_cast<T>(1);
		matrix[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);

		return matrix;
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> perspective(const T& fovy, const T& aspect, const T& zNear, const T& zFar)
	{
		switch (lvn::getRenderClipRegionEnum())
		{
			case Lvn_ClipRegion_RHZO: { return lvn::perspectiveRHZO(fovy, aspect, zNear, zFar); }
			case Lvn_ClipRegion_RHNO: { return lvn::perspectiveRHNO(fovy, aspect, zNear, zFar); }
			case Lvn_ClipRegion_LHZO: { return lvn::perspectiveLHZO(fovy, aspect, zNear, zFar); }
			case Lvn_ClipRegion_LHNO: { return lvn::perspectiveLHNO(fovy, aspect, zNear, zFar); }

			default: { return lvn::perspectiveRHNO(fovy, aspect, zNear, zFar); } // opengl default
		}
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> lookAtRH(const LvnVec3_t<T>& eye, const LvnVec3_t<T>& center, const LvnVec3_t<T>& up)
	{
		LvnVec3_t<T> f(lvn::normalize(center - eye));
		LvnVec3_t<T> s(lvn::normalize(lvn::cross(f, up)));
		LvnVec3_t<T> u(lvn::cross(s, f));

		LvnMat4x4_t<T> matrix(static_cast<T>(1));
		matrix[0][0] =  s.x;
		matrix[1][0] =  s.y;
		matrix[2][0] =  s.z;
		matrix[0][1] =  u.x;
		matrix[1][1] =  u.y;
		matrix[2][1] =  u.z;
		matrix[0][2] = -f.x;
		matrix[1][2] = -f.y;
		matrix[2][2] = -f.z;
		matrix[3][0] = -lvn::dot(s, eye);
		matrix[3][1] = -lvn::dot(u, eye);
		matrix[3][2] =  lvn::dot(f, eye);
		return matrix;
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> lookAtLH(const LvnVec3_t<T>& eye, const LvnVec3_t<T>& center, const LvnVec3_t<T>& up)
	{
		LvnVec3_t<T> f(lvn::normalize(center - eye));
		LvnVec3_t<T> s(lvn::normalize(lvn::cross(up, f)));
		LvnVec3_t<T> u(lvn::cross(f, s));

		LvnMat4x4_t<T> matrix(static_cast<T>(1));
		matrix[0][0] = s.x;
		matrix[1][0] = s.y;
		matrix[2][0] = s.z;
		matrix[0][1] = u.x;
		matrix[1][1] = u.y;
		matrix[2][1] = u.z;
		matrix[0][2] = f.x;
		matrix[1][2] = f.y;
		matrix[2][2] = f.z;
		matrix[3][0] = -lvn::dot(s, eye);
		matrix[3][1] = -lvn::dot(u, eye);
		matrix[3][2] = -lvn::dot(f, eye);
		return matrix;
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> lookAt(const LvnVec3_t<T>& eye, const LvnVec3_t<T>& center, const LvnVec3_t<T>& up)
	{
		switch (lvn::getRenderClipRegionEnum())
		{
			case Lvn_ClipRegion_RHZO: { return lvn::lookAtRH(eye, center, up); }
			case Lvn_ClipRegion_RHNO: { return lvn::lookAtRH(eye, center, up); }
			case Lvn_ClipRegion_LHZO: { return lvn::lookAtLH(eye, center, up); }
			case Lvn_ClipRegion_LHNO: { return lvn::lookAtLH(eye, center, up); }

			default: { return lvn::lookAtRH(eye, center, up); } // opengl default
		}
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> translate(const LvnMat4x4_t<T>& mat, const LvnVec3_t<T>& vec)
	{
		LvnMat4x4_t<T> translate(static_cast<T>(1));
		translate[3][0] = vec.x;
		translate[3][1] = vec.y;
		translate[3][2] = vec.z;

		return mat * translate;
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> scale(const LvnMat4x4_t<T>& mat, const LvnVec3_t<T>& vec)
	{
		LvnMat4x4_t<T> scale(static_cast<T>(1));
		scale[0][0] = vec.x;
		scale[1][1] = vec.y;
		scale[2][2] = vec.z;

		return mat * scale;
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> rotate(const LvnMat4x4_t<T>& mat, T angle, const LvnVec3_t<T>& axis)
	{
		const T c = cos(angle);
		const T s = sin(angle);
		const T nc = static_cast<T>(1) - cos(angle);

		LvnMat4x4_t<T> rotate(static_cast<T>(1));
		rotate[0][0] = c + axis.x * axis.x * nc;
		rotate[0][1] = axis.x * axis.y * nc + axis.z * s;
		rotate[0][2] = axis.x * axis.z * nc - axis.y * s;

		rotate[1][0] = axis.x * axis.y * nc - axis.z * s;
		rotate[1][1] = c + axis.y * axis.y * nc;
		rotate[1][2] = axis.y * axis.z * nc + axis.x * s;

		rotate[2][0] = axis.x * axis.z * nc + axis.y * s;
		rotate[2][1] = axis.y * axis.z * nc - axis.x * s;
		rotate[2][2] = c + axis.z * axis.z * nc;

		return mat * rotate;
	}

	template <typename T>
	LVN_API LvnMat4x4_t<T> quatToMat4(const LvnQuat_t<T> quat)
	{
		const T w = quat.w;
		const T x = quat.x;
		const T y = quat.y;
		const T z = quat.z;

		LvnMat4x4_t<T> matrix(static_cast<T>(1));
		matrix[0][0] = static_cast<T>(1) - 2 * (y * y + z * z);
		matrix[0][1] = 2 * (x * y + w * z);
		matrix[0][2] = 2 * (x * z - w * y);
		matrix[1][0] = 2 * (x * y - w * z);
		matrix[1][1] = static_cast<T>(1) - 2 * (x * x + z * z);
		matrix[1][2] = 2 * (y * z + w * x);
		matrix[2][0] = 2 * (x * z + w * y);
		matrix[2][1] = 2 * (y * z - w * x);
		matrix[2][2] = static_cast<T>(1) - 2 * (x * x + y * y);

		return matrix;
	}
}


/* [Core Struct Implementaion] */

struct LvnMemAllocStructInfo
{
	LvnStructureType sType;
	uint64_t count;
};

struct LvnContextCreateInfo
{
	const char*               applicationName;               // name of application or program
	LvnWindowApi              windowapi;                     // window api to use when creating windows
	LvnGraphicsApi            graphicsapi;                   // graphics api to use when rendering (eg. vulkan, opengl)

	struct
	{
		bool                      enableLogging;                 // enable or diable logging
		bool                      disableCoreLogging;            // whether to disable core logging in the library
		bool                      enableVulkanValidationLayers;  // enable vulkan validation layer messages when using vulkan
	}                         logging;

	LvnTextureFormat          frameBufferColorFormat;        // set the color image format of the window framebuffer when rendering
	LvnClipRegion             matrixClipRegion;              // set the clip region to the correct coordinate system depending on the api
	
	struct
	{
		LvnMemAllocMode           memAllocMode;                  // memory allocation mode, how memory should be allocated when creating new object
		LvnMemAllocStructInfo*    memAllocStructInfos;           // array of object alloc info structs to tell how many objects of each type to allocate if using memory pool
		uint32_t                  memAllocStructInfoCount;       // number of object alloc inso structs;
		LvnMemAllocStructInfo*    blockMemAllocStructInfos;      // array of objects alloc info structs of each type to allocate for further memory blocks in case if the first block is full
		uint32_t                  blockMemAllocStructInfoCount;  // number of block object alloc info structs
	}                          memoryInfo;
};

/* [Logging] */
struct LvnLoggerCreateInfo
{
	const char* loggerName;
	const char* logPatternFormat;
	LvnLogLevel logLevel;
};

struct LvnLogMessage
{
	const char *msg, *loggerName;
	LvnLogLevel level;
	long long timeEpoch;
};

struct LvnLogPattern
{
	char symbol;
	std::string (*func)(LvnLogMessage*);
};

/* [Events] */
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
	int width, height;                  // width and height of window
	const char* title;                  // title of window
	int minWidth, minHeight;            // minimum width and height of window (set to 0 if not specified)
	int maxWidth, maxHeight;            // maximum width and height of window (set to -1 if not specified)
	bool fullscreen, resizable, vSync;  // sets window to fullscreen if true; enables window resizing if true; vSync controls window framerate, sets framerate to 60fps if true
	LvnWindowIconData* pIcons;          // icon images used for window/app icon; pIcons can be stored in an array; pIcons will be ignored if set to null
	uint32_t iconCount;                 // iconCount is the number of icons in pIcons; if using only one icon, set iconCount to 1; if using an array of icons, set to length of array

	void (*eventCallBack)(LvnEvent*);   // set function ptr used as a callback to get events from this window
	void* userData;                     // pass a ptr of a variable or struct to use and get data during window callbacks

	LvnWindowCreateInfo()
	{
		width = 0;
		height = 0;
		title = nullptr;
		minWidth = 0, minHeight = 0;
		maxWidth = -1, maxHeight = -1;
		fullscreen = false, resizable = true, vSync = false;
		pIcons = nullptr;
		iconCount = 0;
		eventCallBack = nullptr;
		userData = nullptr;
	}
};


// ---------------------------------------------
// [SECTION]: Data Structures
// ---------------------------------------------
// - Basic data structures for use of allocating or handling data
template<typename T>
struct LvnPair
{
	union { T p1, x, width; };
	union { T p2, y, height; };
};

template<typename T1, typename T2>
struct LvnDoublePair
{
	union { T1 p1, x, width; };
	union { T2 p2, y, height; };
};


template<typename T>
class LvnData
{
private:
	T* m_Data;
	size_t m_Size, m_MemSize;

public:
	LvnData()
		: m_Data(0), m_Size(0), m_MemSize(0) {}

	~LvnData()
	{
		free(m_Data);
	}

	LvnData(const T* data, size_t size)
	{
		void* buff = malloc(size * sizeof(T));
		if (!buff) { LVN_CORE_ASSERT(false, "malloc failure, failed to allocate memory"); LVN_ASSERT_BREAK; }
		memcpy(buff, data, size * sizeof(T));
		m_Data = (T*)buff;
		m_Size = size;
		m_MemSize = size * sizeof(T);
	}
	LvnData(const LvnData<T>& data)
	{
		void* buff = malloc(data.m_MemSize);
		if (!buff) { LVN_CORE_ASSERT(false, "malloc failure, failed to allocate memory"); LVN_ASSERT_BREAK; }
		memcpy(buff, data.m_Data, data.m_MemSize);
		this->m_Data = (T*)buff;
		this->m_Size = data.m_Size;
		this->m_MemSize = data.m_MemSize;
	}
	LvnData<T>& operator=(const LvnData<T>& data)
	{
		free(m_Data);
		void* buff = malloc(data.m_MemSize);
		if (!buff) { LVN_CORE_ASSERT(false, "malloc failure, failed to allocate memory"); LVN_ASSERT_BREAK; }
		memcpy(buff, data.m_Data, data.m_MemSize);
		this->m_Data = (T*)buff;
		this->m_Size = data.m_Size;
		this->m_MemSize = data.m_MemSize;

		return *this;
	}

	T& operator[](size_t i)
	{
		LVN_CORE_ASSERT(i < m_Size, "element index out of range");
		return m_Data[i];
	}

	const T* const data() const { return m_Data; }
	const size_t size() const { return m_Size; }
	const size_t memsize() const { return m_MemSize; }

	const T* const begin() const { return &m_Data[0]; }
	const T* const end() const { return &m_Data[m_Size - 1]; }
};

class LvnTimer
{
public:
	LvnTimer() : m_Start(std::chrono::high_resolution_clock::now()), m_Now(std::chrono::high_resolution_clock::now()), m_Pause(false) {}

	void     begin() { m_Start = std::chrono::high_resolution_clock::now(); }
	void     reset() { m_Start = std::chrono::high_resolution_clock::now(); m_Pause = false; }
	void     pause(bool pause) { m_Pause = pause; }

	float    elapsed() { if (!m_Pause) { m_Now = std::chrono::high_resolution_clock::now(); } return std::chrono::duration_cast<std::chrono::nanoseconds>(m_Now - m_Start).count() * 0.001f * 0.001f * 0.001f; }
	float    elapsedms() { return elapsed() * 1000.0f; }

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_Start, m_Now;
	bool m_Pause;
};

class LvnThreadPool
{
private:
	using LvnTaskFnPtr = void (*)();

	std::vector<std::thread> m_Workers;
	std::queue<LvnTaskFnPtr> m_Tasks;
	std::mutex m_QueueMutex;
	std::condition_variable m_QueueCondition;
	bool m_Terminate;

	void ThreadFunc()
	{
		while (true)
		{
			LvnTaskFnPtr fnPtr;

			std::unique_lock<std::mutex> lock(m_QueueMutex);
			m_QueueCondition.wait(lock, [this]() { return !m_Tasks.empty() || m_Terminate; });

			if (m_Terminate) { return; }

			if (!m_Tasks.empty())
			{
				fnPtr = m_Tasks.front();
				m_Tasks.pop();

				lock.unlock();
				fnPtr();
			}
		}
	}

public:
	LvnThreadPool()
		: m_Workers(1), m_Tasks(), m_QueueMutex(), m_QueueCondition(), m_Terminate(false)
	{
		for (uint32_t i = 0; i < m_Workers.size(); i++)
			m_Workers[i] = (std::thread(&LvnThreadPool::ThreadFunc, this));
	}

	LvnThreadPool(uint32_t workerCount)
		: m_Tasks(), m_QueueMutex(), m_QueueCondition(), m_Terminate(false)
	{
		m_Workers.resize(workerCount > 0 ? workerCount : 1);

		for (uint32_t i = 0; i < m_Workers.size(); i++)
			m_Workers[i] = (std::thread(&LvnThreadPool::ThreadFunc, this));
	}

	~LvnThreadPool()
	{
		m_QueueMutex.lock();
		m_Terminate = true;
		m_QueueMutex.unlock();

		m_QueueCondition.notify_all();
		for (uint32_t i = 0; i < m_Workers.size(); i++)
			m_Workers[i].join();

		m_Workers.clear();
	}

	void add_task(LvnTaskFnPtr fnPtr)
	{
		std::lock_guard<std::mutex> lock(m_QueueMutex);
		m_Tasks.push(fnPtr);
		m_QueueCondition.notify_one();
	}

	bool busy()
	{
		std::lock_guard<std::mutex> lock(m_QueueMutex);
		return !m_Tasks.empty();
	}

	void wait()
	{
		while (LvnThreadPool::busy()) {}
	}
};


// ---------------------------------------------
// [SECTION]: Vectors & Matrices
// ---------------------------------------------

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
	LvnVec2_t<T> operator+(LvnVec2_t<T>& v)
	{
		return { this->x + v.x, this->y + v.y };
	}
	LvnVec2_t<T> operator-(LvnVec2_t<T>& v)
	{
		return { this->x - v.x, this->y - v.y };
	}
	LvnVec2_t<T> operator*(LvnVec2_t<T>& v)
	{
		return { this->x * v.x, this->y * v.y };
	}
	LvnVec2_t<T> operator/(LvnVec2_t<T>& v)
	{
		return { this->x / v.x, this->y / v.y };
	}
	LvnVec2_t<T> operator+(const LvnVec2_t<T>& v) const
	{
		return { this->x + v.x, this->y + v.y };
	}
	LvnVec2_t<T> operator-(const LvnVec2_t<T>& v) const
	{
		return { this->x - v.x, this->y - v.y };
	}
	LvnVec2_t<T> operator*(const LvnVec2_t<T>& v) const
	{
		return { this->x * v.x, this->y * v.y };
	}
	LvnVec2_t<T> operator/(const LvnVec2_t<T>& v) const
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
	T& operator[](int i) const
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
	LvnVec3_t(const LvnVec2_t<T>& n_xy)
		: x(n_xy.x), y(n_xy.y), z(T(0)) {}
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
	LvnVec3_t<T> operator+(LvnVec3_t<T>& v)
	{
		return { this->x + v.x, this->y + v.y, this->z + v.z };
	}
	LvnVec3_t<T> operator-(LvnVec3_t<T>& v)
	{
		return { this->x - v.x, this->y - v.y, this->z - v.z };
	}
	LvnVec3_t<T> operator*(LvnVec3_t<T>& v)
	{
		return { this->x * v.x, this->y * v.y, this->z * v.z };
	}
	LvnVec3_t<T> operator/(LvnVec3_t<T>& v)
	{
		return { this->x / v.x, this->y / v.y, this->z / v.z };
	}
	LvnVec3_t<T> operator+(const LvnVec3_t<T>& v) const
	{
		return { this->x + v.x, this->y + v.y, this->z + v.z };
	}
	LvnVec3_t<T> operator-(const LvnVec3_t<T>& v) const
	{
		return { this->x - v.x, this->y - v.y, this->z - v.z };
	}
	LvnVec3_t<T> operator*(const LvnVec3_t<T>& v) const
	{
		return { this->x * v.x, this->y * v.y, this->z * v.z };
	}
	LvnVec3_t<T> operator/(const LvnVec3_t<T>& v) const
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
			return z;
		}
	}
	T& operator[](int i) const
	{
		switch (i)
		{
		default:
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
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
	LvnVec4_t(const LvnVec2_t<T>& n_xy)
		: x(n_xy.x), y(n_xy.y), z(0), w(T(0)) {}
	LvnVec4_t(const LvnVec3_t<T>& n_xyz)
		: x(n_xyz.x), y(n_xyz.y), z(n_xyz.z), w(T(0)) {}

	LvnVec4_t<T> operator+()
	{
		return { this->x, this->y, this->z, this->w };
	}
	LvnVec4_t<T> operator-()
	{
		return { -this->x, -this->y, -this->z, -this->w };
	}
	LvnVec4_t<T> operator+(LvnVec4_t<T>& v)
	{
		return { this->x + v.x, this->y + v.y, this->z + v.z, this->w + v.w };
	}
	LvnVec4_t<T> operator-(LvnVec4_t<T>& v)
	{
		return { this->x - v.x, this->y - v.y, this->z - v.z, this->w - v.w };
	}
	LvnVec4_t<T> operator*(LvnVec4_t<T>& v)
	{
		return { this->x * v.x, this->y * v.y, this->z * v.z, this->w * v.w };
	}
	LvnVec4_t<T> operator/(LvnVec4_t<T>& v)
	{
		return { this->x / v.x, this->y / v.y, this->z / v.z, this->w / v.w };
	}
	LvnVec4_t<T> operator+(const LvnVec4_t<T>& v) const
	{
		return { this->x + v.x, this->y + v.y, this->z + v.z, this->w + v.w };
	}
	LvnVec4_t<T> operator-(const LvnVec4_t<T>& v) const
	{
		return { this->x - v.x, this->y - v.y, this->z - v.z, this->w - v.w };
	}
	LvnVec4_t<T> operator*(const LvnVec4_t<T>& v) const
	{
		return { this->x * v.x, this->y * v.y, this->z * v.z, this->w * v.w };
	}
	LvnVec4_t<T> operator/(const LvnVec4_t<T>& v) const
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
			return z;
		case 3:
			return w;
		}
	}
	T& operator[](int i) const
	{
		switch (i)
		{
		default:
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
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
	LvnVec2_t<T> operator[](int i) const
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
	LvnMat2x2_t<T> operator*(const T& s)
	{
		return LvnMat2x2_t<T>(
			this->value[0] * s,
			this->value[1] * s);
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

	LvnMat2x2_t<T> operator+() const
	{
		return LvnMat2x2_t<T>(
			this->value[0],
			this->value[1]);
	}
	LvnMat2x2_t<T> operator-() const
	{
		return LvnMat2x2_t<T>(
			-this->value[0],
			-this->value[1]);
	}
	LvnMat2x2_t<T> operator+(const LvnMat2x2_t<T>& m) const
	{
		return LvnMat2x2_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1]);
	}
	LvnMat2x2_t<T> operator-(const LvnMat2x2_t<T>& m) const
	{
		return LvnMat2x2_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1]);
	}
	LvnMat2x2_t<T> operator*(const T& n) const
	{
		return LvnMat2x2_t<T>(
			this->value[0] * n,
			this->value[1] * n);
	}
	LvnMat2x2_t<T> operator*(const LvnMat2x2_t<T>& m) const
	{
		return LvnMat2x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y);
	}
	LvnMat3x2_t<T> operator*(const LvnMat3x2_t<T>& m) const
	{
		return LvnMat3x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y);
	}
	LvnMat4x2_t<T> operator*(const LvnMat4x2_t<T>& m) const
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
	LvnVec3_t<T> operator[](int i) const
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
	LvnMat3x3_t<T> operator*(const T& s)
	{
		return LvnMat3x3_t<T>(
			this->value[0] * s,
			this->value[1] * s,
			this->value[2] * s);
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

	LvnMat3x3_t<T> operator+() const
	{
		return LvnMat3x3_t<T>(
			this->value[0],
			this->value[1],
			this->value[2]);
	}
	LvnMat3x3_t<T> operator-() const
	{
		return LvnMat3x3_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2]);
	}
	LvnMat3x3_t<T> operator+(const LvnMat3x3_t<T>& m) const
	{
		return LvnMat3x3_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2]);
	}
	LvnMat3x3_t<T> operator-(const LvnMat3x3_t<T>& m) const
	{
		return LvnMat3x3_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2]);
	}
	LvnMat3x3_t<T> operator*(const LvnMat3x3_t<T>& m) const
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
	LvnMat2x3_t<T> operator*(const LvnMat2x3_t<T>& m) const
	{
		return LvnMat2x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z);
	}
	LvnMat4x3_t<T> operator*(const LvnMat4x3_t<T>& m) const
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
	LvnVec4_t<T> operator[](int i) const
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
	LvnMat4x4_t<T> operator*(const T& s)
	{
		return LvnMat4x4_t<T>(
			this->value[0] * s,
			this->value[1] * s,
			this->value[2] * s,
			this->value[3] * s);
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

	LvnMat4x4_t<T> operator+() const
	{
		return LvnMat4x4_t<T>(
			this->value[0],
			this->value[1],
			this->value[2],
			this->value[3]);
	}
	LvnMat4x4_t<T> operator-() const
	{
		return LvnMat4x4_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2],
			-this->value[3]);
	}
	LvnMat4x4_t<T> operator+(const LvnMat4x4_t<T>& m) const
	{
		return LvnMat4x4_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2],
			this->value[3] + m.value[3]);
	}
	LvnMat4x4_t<T> operator-(const LvnMat4x4_t<T>& m) const
	{
		return LvnMat4x4_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2],
			this->value[3] - m.value[3]);
	}
	LvnMat4x4_t<T> operator*(const LvnMat4x4_t<T>& m) const
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
	LvnMat2x4_t<T> operator*(const LvnMat2x4_t<T>& m) const
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
	LvnMat3x4_t<T> operator*(const LvnMat3x4_t<T>& m) const
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
	LvnVec3_t<T> operator[](int i) const
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
	LvnMat2x3_t<T> operator*(const T& s)
	{
		return LvnMat2x3_t<T>(
			this->value[0] * s,
			this->value[1] * s);
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

	LvnMat2x3_t<T> operator+() const
	{
		return LvnMat2x3_t<T>(
			this->value[0],
			this->value[1]);
	}
	LvnMat2x3_t<T> operator-() const
	{
		return LvnMat2x3_t<T>(
			-this->value[0],
			-this->value[1]);
	}
	LvnMat2x3_t<T> operator+(const LvnMat2x3_t<T>& m) const
	{
		return LvnMat2x3_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1]);
	}
	LvnMat2x3_t<T> operator-(const LvnMat2x3_t<T>& m) const
	{
		return LvnMat2x3_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1]);
	}
	LvnMat2x3_t<T> operator*(const LvnMat2x2_t<T>& m) const
	{
		return LvnMat2x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y);
	}
	LvnMat3x3_t<T> operator*(const LvnMat3x2_t<T>& m) const
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
	LvnMat4x3_t<T> operator*(const LvnMat4x2_t<T>& m) const
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
	LvnVec4_t<T> operator[](int i) const
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
	LvnMat2x4_t<T> operator*(const T& s)
	{
		return LvnMat2x4_t<T>(
			this->value[0] * s,
			this->value[1] * s);
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

	LvnMat2x4_t<T> operator+() const
	{
		return LvnMat2x4_t<T>(
			this->value[0],
			this->value[1]);
	}
	LvnMat2x4_t<T> operator-() const
	{
		return LvnMat2x4_t<T>(
			-this->value[0],
			-this->value[1]);
	}
	LvnMat2x4_t<T> operator+(const LvnMat2x4_t<T>& m) const
	{
		return LvnMat2x4_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1]);
	}
	LvnMat2x4_t<T> operator-(const LvnMat2x4_t<T>& m) const
	{
		return LvnMat2x4_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1]);
	}
	LvnMat4x4_t<T> operator*(const LvnMat4x2_t<T>& m) const
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
	LvnMat2x4_t<T> operator*(const LvnMat2x2_t<T>& m) const
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
	LvnMat3x4_t<T> operator*(const LvnMat3x2_t<T>& m) const
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
	LvnVec2_t<T> operator[](int i) const
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
	LvnMat3x2_t<T> operator*(const T& s)
	{
		return LvnMat3x2_t<T>(
			this->value[0] * s,
			this->value[1] * s,
			this->value[2] * s);
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

	LvnMat3x2_t<T> operator+() const
	{
		return LvnMat3x2_t<T>(
			this->value[0],
			this->value[1],
			this->value[2]);
	}
	LvnMat3x2_t<T> operator-() const
	{
		return LvnMat3x2_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2]);
	}
	LvnMat3x2_t<T> operator+(const LvnMat3x2_t<T>& m) const
	{
		return LvnMat3x2_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2]);
	}
	LvnMat3x2_t<T> operator-(const LvnMat3x2_t<T>& m) const
	{
		return LvnMat3x2_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2]);
	}
	LvnMat3x2_t<T> operator*(const LvnMat3x3_t<T>& m) const
	{
		return LvnMat3x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z);
	}
	LvnMat4x2_t<T> operator*(const LvnMat4x3_t<T>& m) const
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
	LvnVec4_t<T> operator[](int i) const
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
	LvnMat3x4_t<T> operator*(const T& s)
	{
		return LvnMat3x4_t<T>(
			this->value[0] * s,
			this->value[1] * s,
			this->value[2] * s);
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

	LvnMat3x4_t<T> operator+() const
	{
		return LvnMat3x4_t<T>(
			this->value[0],
			this->value[1],
			this->value[2]);
	}
	LvnMat3x4_t<T> operator-() const
	{
		return LvnMat3x4_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2]);
	}
	LvnMat3x4_t<T> operator+(const LvnMat3x4_t<T>& m) const
	{
		return LvnMat3x4_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2]);
	}
	LvnMat3x4_t<T> operator-(const LvnMat3x4_t<T>& m) const
	{
		return LvnMat3x4_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2]);
	}
	LvnMat4x4_t<T> operator*(const LvnMat4x3_t<T>& m) const
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
	LvnMat2x4_t<T> operator*(const LvnMat2x3_t<T>& m) const
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
	LvnMat3x4_t<T> operator*(const LvnMat3x3_t<T>& m) const
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
	LvnVec2_t<T> operator[](int i) const
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
	LvnMat4x2_t<T> operator*(const T& s)
	{
		return LvnMat4x2_t<T>(
			this->value[0] * s,
			this->value[1] * s,
			this->value[2] * s,
			this->value[3] * s);
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

	LvnMat4x2_t<T> operator+() const
	{
		return LvnMat4x2_t<T>(
			this->value[0],
			this->value[1],
			this->value[2],
			this->value[3]);
	}
	LvnMat4x2_t<T> operator-() const
	{
		return LvnMat4x2_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2],
			-this->value[3]);
	}
	LvnMat4x2_t<T> operator+(const LvnMat4x2_t<T>& m) const
	{
		return LvnMat4x2_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2],
			this->value[3] + m.value[3]);
	}
	LvnMat4x2_t<T> operator-(const LvnMat4x2_t<T>& m) const
	{
		return LvnMat4x2_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2],
			this->value[3] - m.value[3]);
	}
	LvnMat2x2_t<T> operator*(const LvnMat2x4_t<T>& m) const
	{
		return LvnMat2x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w);
	}
	LvnMat3x2_t<T> operator*(const LvnMat3x4_t<T>& m) const
	{
		return LvnMat3x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z + this->value[3].x * m.value[2].w,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z + this->value[3].y * m.value[2].w);
	}
	LvnMat4x2_t<T> operator*(const LvnMat4x4_t<T>& m) const
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
	LvnVec3_t<T> operator[](int i) const
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
	LvnMat4x3_t<T> operator*(const T& s)
	{
		return LvnMat4x3_t<T>(
			this->value[0] * s,
			this->value[1] * s,
			this->value[2] * s,
			this->value[3] * s);
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

	LvnMat4x3_t<T> operator+() const
	{
		return LvnMat4x3_t<T>(
			this->value[0],
			this->value[1],
			this->value[2],
			this->value[3]);
	}
	LvnMat4x3_t<T> operator-() const
	{
		return LvnMat4x3_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2],
			-this->value[3]);
	}
	LvnMat4x3_t<T> operator+(const LvnMat4x3_t<T>& m) const
	{
		return LvnMat4x3_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2],
			this->value[3] + m.value[3]);
	}
	LvnMat4x3_t<T> operator-(const LvnMat4x3_t<T>& m) const
	{
		return LvnMat4x3_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2],
			this->value[3] - m.value[3]);
	}
	LvnMat2x3_t<T> operator*(const LvnMat2x4_t<T>& m) const
	{
		return LvnMat2x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z + this->value[3].z * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z + this->value[3].z * m.value[1].w);
	}
	LvnMat3x3_t<T> operator*(const LvnMat3x4_t<T>& m) const
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
	LvnMat4x3_t<T> operator*(const LvnMat4x4_t<T>& m) const
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

template<typename T>
struct LvnQuat_t
{
	union
	{
		struct { T w, x, y, z; };
		struct { T r, i, j, k; };
	};

	LvnQuat_t()
		: w(0), x(0), y(0), z(0) {}

	LvnQuat_t(const T& nw, const T& nx, const T& ny, const T& nz)
		: w(nw), x(nx), y(ny), z(nz) {}


	T& operator[](int i)
	{
		switch (i)
		{
		default:
		case 0:
			return w;
		case 1:
			return x;
		case 2:
			return y;
		case 3:
			return z;
		}
	}
};

// ---------------------------------------------
// [SECTION]: Struct Implementaion
// ---------------------------------------------

struct LvnPhysicalDeviceInfo
{
	char name[256];
	LvnPhysicalDeviceType type;
	uint32_t apiVersion;
	uint32_t driverVersion;
};

struct LvnRenderInitInfo
{
	LvnPhysicalDevice*  physicalDevice;
	bool                gammaCorrection;
	uint32_t            maxFramesInFlight;
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
	uint32_t binding;
	uint32_t layout;
	LvnVertexDataType type;
	uint32_t offset;
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
	uint32_t descriptorCount;
	const LvnUniformBuffer* bufferInfo;
	const LvnTexture* textureInfo;
};

struct LvnPipelineCreateInfo
{
	LvnPipelineSpecification* pipelineSpecification;
	LvnVertexBindingDescription* pVertexBindingDescriptions;
	uint32_t vertexBindingDescriptionCount;
	LvnVertexAttribute* pVertexAttributes;
	uint32_t vertexAttributeCount;
	LvnDescriptorLayout** pDescriptorLayouts;
	uint32_t descriptorLayoutCount;
	LvnShader* shader;
	LvnRenderPass* renderPass;
};

struct LvnShaderCreateInfo
{
	const char* vertexSrc;
	const char* fragmentSrc;
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
	uint32_t type;
	LvnVertexBindingDescription* pVertexBindingDescriptions;
	uint32_t vertexBindingDescriptionCount;
	LvnVertexAttribute* pVertexAttributes;
	uint32_t vertexAttributeCount;
	const void* pVertices;
	uint64_t vertexBufferSize;
	const uint32_t* pIndices;
	uint64_t indexBufferSize;
};

struct LvnUniformBufferCreateInfo
{
	LvnBufferType type;
	uint32_t binding;
	uint64_t size;
};

struct LvnImageData
{
	LvnData<uint8_t> pixels;
	uint32_t width, height, channels;
	uint64_t size;
};

struct LvnTextureCreateInfo
{
	LvnImageData imageData;
	LvnTextureFilter minFilter, magFilter;
	LvnTextureMode wrapMode;
	LvnTextureFormat format;

};

struct LvnVertex
{
	LvnVec3 pos;
	LvnVec4 color;
	LvnVec2 texUV;
	LvnVec3 normal;

	LvnVec3 tangent;
	LvnVec3 bitangent;
};

struct LvnMaterial
{
	LvnVec3 vBaseColor;
	float fMetallic;
	float fRoughness;
	float fNormal;
	float fOcclusion;
	float fEmissiveStrength;

	LvnTexture* albedo;
	LvnTexture* metallicRoughnessOcclusion;
	LvnTexture* normal;
	LvnTexture* emissive;
};

struct LvnMesh
{
	LvnBuffer* buffer;    // single buffer that contains both vertex and index buffer
	LvnMaterial material; // material to hold shader and texture data
	LvnMat4 modelMatrix;  // model matrix of mesh

	uint32_t vertexCount; // number of vertices in this mesh
	uint32_t indexCount;  // number of indices in this mesh
};

struct LvnMeshCreateInfo
{
	LvnBufferCreateInfo* bufferInfo;
	LvnMaterial material;
};

struct LvnModel
{
	LvnData<LvnMesh> meshes;
	LvnData<LvnTexture*> textures;
	LvnMat4 modelMatrix;
};

struct LvnCamera
{
	LvnMat4 projectionMatrix;    // projection matrix
	LvnMat4 viewMatrix;          // view matrix
	LvnMat4 matrix;              // combined projection view matrix

	LvnVec3 position;            // position of camera in space
	LvnVec3 orientation;         // orientation/direction camera is looking
	LvnVec3 upVector;            // up vector to differentiate direction in space

	float fov;                   // field of view
	float nearPlane;             // near plane
	float farPlane;              // far plane

	uint32_t width, height;      // dimensions of camera
};

struct LvnCameraCreateInfo
{
	uint32_t width, height;

	LvnVec3 position;
	LvnVec3 orientation;
	LvnVec3 upVector;

	float fovDeg;
	float nearPlane;
	float farPlane;
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

	int8_t unicode;
	int advance;
};

struct LvnCharset
{
	int8_t first, last;        // first and last codepoints to iterate through when loading glyph data, the codepoints relate to ASCII digits
};

struct LvnFont
{
	LvnImageData atlas;
	float fontSize;

	LvnCharset codepoints;
	LvnData<LvnFontGlyph> glyphs;
};

struct LvnSoundCreateInfo
{
	const char* filepath;      // the filepath to the sound file (.wav .mp3)

	float volume;              // volume of sound source, (default: 1.0, min/mute: 0.0), 1.0 is the upper limit however volume can be set higher than 1.0 at your own risk
	float pan;                 // pan of the sound source if using 2 channel sterio output (center: 0.0, left: -1.0, right 1.0)
	float pitch;               // pitch of the sound source, (default: 1.0, min: 0.0, no upper limit)
	bool looping;              // sound source loops when reaches end of track

	LvnVec3 pos;
};

struct LvnNetworkMessageHeader
{
	int id;
	uint64_t size;
};

struct LvnNetworkMessage
{
	LvnNetworkMessageHeader header;
	uint8_t* body;
};

#endif
