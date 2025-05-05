#ifndef HG_LEVIKNO_H
#define HG_LEVIKNO_H


// ------------------------------------------------------------
// Layout: levikno.h
// ------------------------------------------------------------
//
// [SECTION]: Config
// -- [SUBSECT]: Platform Defines
// -- [SUBSECT]: Debug Defines
// -- [SUBSECT]: Memory Alloc Defines
// -- [SUBSECT]: Misc Defines
// -- [SUBSECT]: Includes
// -- [SUBSECT]: Log Defines
// [SECTION]: Enums
// -- [SUBSECT]: Core Enums
// -- [SUBSECT]: Key Code Enums
// -- [SUBSECT]: Mouse Button Code Enums
// -- [SUBSECT]: Logging Enums
// -- [SUBSECT]: Graphics Enums
// -- [SUBSECT]: Networking Enums
// -- [SUBSECT]: Renderer Enums
// [SECTION]: Struct Definitions
// -- [SUBSECT]: Data Structure Definitions
// -- [SUBSECT]: ECS (Entity Component System) Definitions & Implementation
// -- [SUBSECT]: Vertices & Matrices
// [SECTION]: Functions
// -- [SUBSECT]: Core Functions
// -- [SUBSECT]: Logging Functions
// -- [SUBSECT]: Event Functions
// -- [SUBSECT]: Input Functions
// -- [SUBSECT]: Graphics Functions
// -- [SUBSECT]: Audio Functions
// -- [SUBSECT]: Networking Functions
// -- [SUBSECT]: Renderer Functions
// -- [SUBSECT]: ECS Functions
// -- [SUBSECT]: Math Functions
// [SECTION]: Struct Implementations
// -- [SUBSECT]: Data Structures
// -- [SUBSECT]: Vector Implementation
// -- [SUBSECT]: Matrix Implementation
// -- [SUBSECT]: Core Struct Implementation
// -- [SUBSECT]: Graphics Struct Implementation
// -- [SUBSECT]: Audio Struct Implementation
// -- [SUBSECT]: Networking Struct Implementation
// -- [SUBSECT]: Renderer Struct Implementation



// ------------------------------------------------------------
// [SECTION]: Config
// ------------------------------------------------------------

// -- [SUBSECT]: Platform Defines
// ------------------------------------------------------------

// platform
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
    #error "levikno does not support the current platform."
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

// compiler
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

// api

// glslang
#ifdef LVN_INCLUDE_GLSLANG_SUPPORTED
    #define LVN_INCLUDE_GLSLANG_SRC_COMPILE_SUPPORT
#endif

// -- [SUBSECT]: Debug Defines
// ------------------------------------------------------------

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
    #define LVN_ASSERT(x, ...) { if(!(x)) { printf(__VA_ARGS__); LVN_ASSERT_BREAK; } }
    #define LVN_CORE_ASSERT(x, ...) { if(!(x)) { printf(__VA_ARGS__); LVN_ASSERT_BREAK; } }
#else
    #define LVN_ASSERT(x, ...)
    #define LVN_CORE_ASSERT(x, ...)
#endif


// -- [SUBSECT]: Memory Alloc Defines
// ------------------------------------------------------------

#ifndef LVN_ALLOC
    #define LVN_MALLOC(sz) ::lvn::memAlloc(sz)
#endif

#ifndef LVN_FREE
    #define LVN_FREE(p) ::lvn::memFree(p)
#endif

#ifndef LVN_REALLOC
    #define LVN_REALLOC(p,sz) ::lvn::memRealloc(p,sz)
#endif


// -- [SUBSECT]: Misc Defines
// ------------------------------------------------------------

#define LVN_TRUE 1
#define LVN_FALSE 0
#define LVN_NULL_HANDLE nullptr

#define LVN_FILE_NAME __FILE__
#define LVN_LINE __LINE__
#define LVN_FUNC_NAME __func__

#define LVN_STR(x) #x
#define LVN_STRINGIFY(x) LVN_STR(x)

#ifndef M_PI
    #define M_PI 3.1415926535897932384626433832795
#endif

#define LVN_PI ((float)M_PI)
#define LVN_PI_EXACT (22.0/7.0) /* 3.1415... */


// -- [SUBSECT]: Includes
// ------------------------------------------------------------

#include <cstdlib> // malloc, free
#include <cstdint> // uint8_t, uint16_t, uint32_t, uint64_t
#include <cstring> // strlen
#include <cmath>

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <typeindex>


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


// -- [SUBSECT]: Log Defines
// ------------------------------------------------------------

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


// ------------------------------------------------------------
// [SECTION]: Enums
// ------------------------------------------------------------


// -- [SUBSECT]: Core Enums
// ------------------------------------------------------------

enum LvnResult : int
{
    Lvn_Result_Success              =  0,
    Lvn_Result_Failure              = -1,
    Lvn_Result_AlreadyCalled        = -2,
    Lvn_Result_MemAllocFailure      = -3,
    Lvn_Result_TimeOut              = -4,
};

enum LvnStructureType
{
    Lvn_Stype_Undefined = 0,
    Lvn_Stype_Window,
    Lvn_Stype_Logger,
    Lvn_Stype_FrameBuffer,
    Lvn_Stype_Shader,
    Lvn_Stype_DescriptorLayout,
    Lvn_Stype_Pipeline,
    Lvn_Stype_Buffer,
    Lvn_Stype_Sampler,
    Lvn_Stype_Texture,
    Lvn_Stype_Cubemap,
    Lvn_Stype_Sound,
    Lvn_Stype_Socket,

    Lvn_Stype_Max_Value,
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

enum LvnFileMode
{
    Lvn_FileMode_Write,
    Lvn_FileMode_Append,
};

enum LvnLoadFont
{
    Lvn_LoadFont_Default              = (0),
    Lvn_LoadFont_NoHinting            = (1U << 0),
    Lvn_LoadFont_AutoHinting          = (1U << 1),
    Lvn_LoadFont_TargetLight          = (1U << 2),
    Lvn_LoadFont_TargetMono           = (1U << 3),
};
typedef uint32_t LvnLoadFontFlagBits;


// -- [SUBSECT]: Key Code Enums
// ------------------------------------------------------------

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


// -- [SUBSECT]: Mouse Button Code Enums
// ------------------------------------------------------------

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

enum LvnMouseCursor
{
    Lvn_MouseCursor_Arrow,
    Lvn_MouseCursor_Ibeam,
    Lvn_MouseCursor_Crosshair,
    Lvn_MouseCursor_PointingHand,
    Lvn_MouseCursor_ResizeEW,
    Lvn_MouseCursor_ResizeNS,
    Lvn_MouseCursor_ResizeNWSE,
    Lvn_MouseCursor_ResizeNESW,
    Lvn_MouseCursor_ResizeAll,
    Lvn_MouseCursor_NotAllowed,
    Lvn_MouseCursor_HResize         = Lvn_MouseCursor_ResizeEW,
    Lvn_MouseCursor_VRrsize         = Lvn_MouseCursor_ResizeNS,
    Lvn_MouseCursor_Hand            = Lvn_MouseCursor_PointingHand,
};

enum LvnMouseInputMode
{
    Lvn_MouseInputMode_Normal,
    Lvn_MouseInputMode_Disable,
    Lvn_MouseInputMode_Hidden,
    Lvn_MouseInputMode_Captured,
};


// -- [SUBSECT]: Logging Enums
// ------------------------------------------------------------

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


// -- [SUBSECT]: Graphics Enums
// ------------------------------------------------------------

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

enum LvnBufferType
{
    Lvn_BufferType_Unknown  = 0,
    Lvn_BufferType_Vertex   = (1U << 0),
    Lvn_BufferType_Index    = (1U << 1),
    Lvn_BufferType_Uniform  = (1U << 2),
    Lvn_BufferType_Storage  = (1U << 3),
};
typedef uint32_t LvnBufferTypeFlagBits;

enum LvnBufferUsage
{
    Lvn_BufferUsage_Static,
    Lvn_BufferUsage_Dynamic,
    Lvn_BufferUsage_Resize,
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
    Lvn_ColorBlendOp_Add                 = 0,
    Lvn_ColorBlendOp_Subtract            = 1,
    Lvn_ColorBlendOp_ReverseSubtract     = 2,
    Lvn_ColorBlendOp_Min                 = 3,
    Lvn_ColorBlendOp_Max                 = 4,
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
    Lvn_CompareOp_Never          = 0,
    Lvn_CompareOp_Less           = 1,
    Lvn_CompareOp_Equal          = 2,
    Lvn_CompareOp_LessOrEqual    = 3,
    Lvn_CompareOp_Greater        = 4,
    Lvn_CompareOp_NotEqual       = 5,
    Lvn_CompareOp_GreaterOrEqual = 6,
    Lvn_CompareOp_Always         = 7,
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
    Lvn_DescriptorType_ImageSampler,
    Lvn_DescriptorType_ImageSamplerBindless,
    Lvn_DescriptorType_UniformBuffer,
    Lvn_DescriptorType_StorageBuffer,
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
    Lvn_StencilOp_Keep              = 0,
    Lvn_StencilOp_Zero              = 1,
    Lvn_StencilOp_Replace           = 2,
    Lvn_StencilOp_IncrementAndClamp = 3,
    Lvn_StencilOp_DecrementAndClamp = 4,
    Lvn_StencilOp_Invert            = 5,
    Lvn_StencilOp_IncrementAndWrap  = 6,
    Lvn_StencilOp_DecrementAndWrap  = 7,
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

enum LvnAttributeFormat
{
    Lvn_AttributeFormat_Undefined = 0,
    Lvn_AttributeFormat_Scalar_f32,
    Lvn_AttributeFormat_Scalar_f64,
    Lvn_AttributeFormat_Scalar_i32,
    Lvn_AttributeFormat_Scalar_ui32,
    Lvn_AttributeFormat_Scalar_i8,
    Lvn_AttributeFormat_Scalar_ui8,
    Lvn_AttributeFormat_Vec2_f32,
    Lvn_AttributeFormat_Vec3_f32,
    Lvn_AttributeFormat_Vec4_f32,
    Lvn_AttributeFormat_Vec2_f64,
    Lvn_AttributeFormat_Vec3_f64,
    Lvn_AttributeFormat_Vec4_f64,
    Lvn_AttributeFormat_Vec2_i32,
    Lvn_AttributeFormat_Vec3_i32,
    Lvn_AttributeFormat_Vec4_i32,
    Lvn_AttributeFormat_Vec2_ui32,
    Lvn_AttributeFormat_Vec3_ui32,
    Lvn_AttributeFormat_Vec4_ui32,
    Lvn_AttributeFormat_Vec2_i8,
    Lvn_AttributeFormat_Vec3_i8,
    Lvn_AttributeFormat_Vec4_i8,
    Lvn_AttributeFormat_Vec2_ui8,
    Lvn_AttributeFormat_Vec3_ui8,
    Lvn_AttributeFormat_Vec4_ui8,
    Lvn_AttributeFormat_Vec2_n8,
    Lvn_AttributeFormat_Vec3_n8,
    Lvn_AttributeFormat_Vec4_n8,
    Lvn_AttributeFormat_Vec2_un8,
    Lvn_AttributeFormat_Vec3_un8,
    Lvn_AttributeFormat_Vec4_un8,
    Lvn_AttributeFormat_2_10_10_10_ile,
    Lvn_AttributeFormat_2_10_10_10_uile,
    Lvn_AttributeFormat_2_10_10_10_nle,
    Lvn_AttributeFormat_2_10_10_10_unle,
};

enum LvnInterpolationMode
{
    Lvn_InterpolationMode_Step,
    Lvn_InterpolationMode_Linear,
};

enum LvnAnimationPath
{
    Lvn_AnimationPath_Translation,
    Lvn_AnimationPath_Rotation,
    Lvn_AnimationPath_Scale,
};


// -- [SUBSECT]: Networking Enums
// ------------------------------------------------------------

enum LvnSocketType
{
    Lvn_SocketType_Client,
    Lvn_SocketType_Server,
};


// -- [SUBSECT]: Renderer Enums
// ------------------------------------------------------------

enum LvnAttributeLocation
{
    Lvn_AttributeLocation_Position = 0,
    Lvn_AttributeLocation_Color,
    Lvn_AttributeLocation_TexCoords,
    // Lvn_AttributeLocation_Normal,
    // Lvn_AttributeLocation_Tangent,
    // Lvn_AttributeLocation_BoneIds,
    // Lvn_AttributeLocation_Weights,

    Lvn_AttributeLocation_Max_Value,
};

enum LvnRenderModeEnum
{
    Lvn_RenderMode_2d,
    Lvn_RenderMode_2dText,

    Lvn_RenderMode_Max_Value,
};


// ------------------------------------------------------------
// [SECTION]: Struct Definitions
// ------------------------------------------------------------

struct LvnAddress;
struct LvnAnimation;
struct LvnAnimationChannel;
struct LvnAppRenderEvent;
struct LvnAppTickEvent;
struct LvnBuffer;
struct LvnBufferCreateInfo;
struct LvnCamera;
struct LvnCircle;
struct LvnColor;
struct LvnContext;
struct LvnContextCreateInfo;
struct LvnCubemap;
struct LvnCubemapCreateInfo;
struct LvnCubemapHdrCreateInfo;
struct LvnDescriptorBinding;
struct LvnDescriptorLayout;
struct LvnDescriptorLayoutCreateInfo;
struct LvnDescriptorSet;
struct LvnDescriptorUpdateInfo;
struct LvnDrawCommand;
struct LvnEvent;
struct LvnFont;
struct LvnFontConfig;
struct LvnFontGlyph;
struct LvnFrameBuffer;
struct LvnFrameBufferColorAttachment;
struct LvnFrameBufferCreateInfo;
struct LvnFrameBufferDepthAttachment;
struct LvnGraphicsContext;
struct LvnImageData;
struct LvnImageHdrData;
struct LvnKeyHoldEvent;
struct LvnKeyPressedEvent;
struct LvnKeyReleasedEvent;
struct LvnKeyTypedEvent;
struct LvnLogFile;
struct LvnLogger;
struct LvnLoggerCreateInfo;
struct LvnLogMessage;
struct LvnLogPattern;
struct LvnMaterial;
struct LvnMemoryBindingInfo;
struct LvnMesh;
struct LvnMeshTextureBindings;
struct LvnModel;
struct LvnMouseButtonPressedEvent;
struct LvnMouseButtonReleasedEvent;
struct LvnMouseMovedEvent;
struct LvnMouseScrolledEvent;
struct LvnNode;
struct LvnOrthoCamera;
struct LvnPacket;
struct LvnPhysicalDevice;
struct LvnPhysicalDeviceFeatures;
struct LvnPhysicalDeviceProperties;
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
struct LvnPoly;
struct LvnPoint;
struct LvnRect;
struct LvnRenderer;
struct LvnRenderPass;
struct LvnSampler;
struct LvnSamplerCreateInfo;
struct LvnServer;
struct LvnShader;
struct LvnShaderCreateInfo;
struct LvnSkin;
struct LvnSocket;
struct LvnSocketCreateInfo;
struct LvnSound;
struct LvnSoundCreateInfo;
struct LvnSprite;
struct LvnTexture;
struct LvnTextureCreateInfo;
struct LvnTextureSamplerCreateInfo;
struct LvnTransform;
struct LvnTriangle;
struct LvnUniformBufferInfo;
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


// -- [SUBSECT]: Data Structure Definitions
// ------------------------------------------------------------

template <typename T>
struct LvnPair;

template <typename T1, typename T2>
struct LvnDoublePair;


template <typename T, size_t N>
class LvnArray;

template <typename T>
class LvnData;
typedef LvnData<uint8_t> LvnBin;

class LvnTimer;
class LvnThreadPool;
class LvnDrawList;

typedef LvnCamera LvnPerspectiveCamera;
typedef LvnOrthoCamera LvnOrthographicCamera;


// -- [SUBSECT]: ECS (Entity Component System) Definitions & Implementation
// ------------------------------------------------------------

template <typename T>
class LvnComponentArray;
class LvnIComponentArray;
class LvnComponentManager;
typedef uint64_t LvnEntity;


class LvnIComponentArray
{
public:
    virtual ~LvnIComponentArray() = default;
    virtual void entityDestroyed(LvnEntity entity) = 0;
};

template <typename T>
class LvnComponentArray : public LvnIComponentArray
{
    static_assert(!std::is_pointer_v<T>, "cannot have pointer type as template parameter in component array");

private:
    std::vector<T> m_Data;
    std::unordered_map<LvnEntity, uint64_t> m_EntityToIndex;
    std::queue<uint64_t> m_AvailableIndices;

public:
    void add_entity(LvnEntity entity, const T& comp)
    {
        LVN_CORE_ASSERT(m_EntityToIndex.find(entity) == m_EntityToIndex.end(), "entity already has component in component array");

        if (!m_AvailableIndices.empty())
        {
            const uint64_t index = m_AvailableIndices.front();
            m_AvailableIndices.pop();
            m_Data[index] = comp;
            m_EntityToIndex[entity] = index;
            return;
        }

        m_Data.push_back(comp);
        m_EntityToIndex[entity] = m_Data.size() - 1;
    }

    void remove_entity(LvnEntity entity)
    {
        LVN_CORE_ASSERT(m_EntityToIndex.find(entity) != m_EntityToIndex.end(), "entity not found within component array");

        const uint64_t index = m_EntityToIndex[entity];
        LVN_CORE_ASSERT(index < m_Data.size(), "index out of vector size range");

        m_AvailableIndices.push(index);
        m_EntityToIndex.erase(entity);
    }

    T& get_entity_component(LvnEntity entity)
    {
        LVN_CORE_ASSERT(m_EntityToIndex.find(entity) != m_EntityToIndex.end(), "entity not found within component array");

        const uint64_t index = m_EntityToIndex[entity];
        LVN_CORE_ASSERT(index < m_Data.size(), "index out of vector size range");

        return m_Data[index];
    }

    bool has_entity_with_component(LvnEntity entity)
    {
        return m_EntityToIndex.find(entity) != m_EntityToIndex.end();
    }

    virtual void entityDestroyed(LvnEntity entity) override
    {
        if (has_entity_with_component(entity))
            remove_entity(entity);
    }
};

class LvnComponentManager
{
private:
    std::unordered_map<std::type_index, std::shared_ptr<LvnIComponentArray>> m_Components;

public:

    template <typename T>
    void add_component()
    {
        m_Components[std::type_index(typeid(T))] = std::make_shared<LvnComponentArray<T>>();
    }

    template <typename T>
    void remove_component()
    {
        LVN_CORE_ASSERT(m_Components.find(std::type_index(typeid(T))) != m_Components.end(), "component not found within registry");

        m_Components.erase(std::type_index(typeid(T)));
    }

    template <typename T>
    LvnComponentArray<T>& get_component()
    {
        LVN_CORE_ASSERT(m_Components.find(std::type_index(typeid(T))) != m_Components.end(), "component not found within registry");
        auto index = std::type_index(typeid(T));
        return *static_cast<LvnComponentArray<T>*>(m_Components[index].get());
    }

    template <typename T>
    bool has_component()
    {
        return m_Components.find(std::type_index(typeid(T))) != m_Components.end();
    }

    auto begin() { return m_Components.begin(); }
    auto end() { return m_Components.end(); }
};


// -- [SUBSECT]: Vertices & Matrices
// ------------------------------------------------------------

typedef int length_t;

template<length_t L, typename T>
struct LvnVec;
template<typename T>
struct LvnVec<2, T>;
template<typename T>
struct LvnVec<3, T>;
template<typename T>
struct LvnVec<4, T>;

template<length_t R, length_t C, typename T>
struct LvnMat;
template<typename T>
struct LvnMat<2, 2, T>;
template<typename T>
struct LvnMat<3, 3, T>;
template<typename T>
struct LvnMat<4, 4, T>;
template<typename T>
struct LvnMat<2, 3, T>;
template<typename T>
struct LvnMat<2, 4, T>;
template<typename T>
struct LvnMat<3, 2, T>;
template<typename T>
struct LvnMat<3, 4, T>;
template<typename T>
struct LvnMat<4, 2, T>;
template<typename T>
struct LvnMat<4, 3, T>;

template<typename T>
struct LvnQuat_t;

typedef LvnVec<2, float>               LvnVec2;
typedef LvnVec<3, float>               LvnVec3;
typedef LvnVec<4, float>               LvnVec4;
typedef LvnVec<2, int>                 LvnVec2i;
typedef LvnVec<3, int>                 LvnVec3i;
typedef LvnVec<4, int>                 LvnVec4i;
typedef LvnVec<2, unsigned int>        LvnVec2ui;
typedef LvnVec<3, unsigned int>        LvnVec3ui;
typedef LvnVec<4, unsigned int>        LvnVec4ui;
typedef LvnVec<2, double>              LvnVec2d;
typedef LvnVec<3, double>              LvnVec3d;
typedef LvnVec<4, double>              LvnVec4d;
typedef LvnVec<2, float>               LvnVec2f;
typedef LvnVec<3, float>               LvnVec3f;
typedef LvnVec<4, float>               LvnVec4f;
typedef LvnVec<2, bool>                LvnVec2b;
typedef LvnVec<3, bool>                LvnVec3b;
typedef LvnVec<4, bool>                LvnVec4b;

typedef LvnMat<2, 2, float>            LvnMat2;
typedef LvnMat<3, 3, float>            LvnMat3;
typedef LvnMat<4, 4, float>            LvnMat4;
typedef LvnMat<2, 3, float>            LvnMat2x3;
typedef LvnMat<2, 4, float>            LvnMat2x4;
typedef LvnMat<3, 2, float>            LvnMat3x2;
typedef LvnMat<3, 4, float>            LvnMat3x4;
typedef LvnMat<4, 2, float>            LvnMat4x2;
typedef LvnMat<4, 3, float>            LvnMat4x3;
typedef LvnMat<2, 2, int>              LvnMat2i;
typedef LvnMat<3, 3, int>              LvnMat3i;
typedef LvnMat<4, 4, int>              LvnMat4i;
typedef LvnMat<2, 3, int>              LvnMat2x3i;
typedef LvnMat<2, 4, int>              LvnMat2x4i;
typedef LvnMat<3, 2, int>              LvnMat3x2i;
typedef LvnMat<3, 4, int>              LvnMat3x4i;
typedef LvnMat<4, 2, int>              LvnMat4x2i;
typedef LvnMat<4, 3, int>              LvnMat4x3i;
typedef LvnMat<2, 2, unsigned int>     LvnMat2ui;
typedef LvnMat<3, 3, unsigned int>     LvnMat3ui;
typedef LvnMat<4, 4, unsigned int>     LvnMat4ui;
typedef LvnMat<2, 3, unsigned int>     LvnMat2x3ui;
typedef LvnMat<2, 4, unsigned int>     LvnMat2x4ui;
typedef LvnMat<3, 2, unsigned int>     LvnMat3x2ui;
typedef LvnMat<3, 4, unsigned int>     LvnMat3x4ui;
typedef LvnMat<4, 2, unsigned int>     LvnMat4x2ui;
typedef LvnMat<4, 3, unsigned int>     LvnMat4x3ui;
typedef LvnMat<2, 2, double>           LvnMat2d;
typedef LvnMat<3, 3, double>           LvnMat3d;
typedef LvnMat<4, 4, double>           LvnMat4d;
typedef LvnMat<2, 3, double>           LvnMat2x3d;
typedef LvnMat<2, 4, double>           LvnMat2x4d;
typedef LvnMat<3, 2, double>           LvnMat3x2d;
typedef LvnMat<3, 4, double>           LvnMat3x4d;
typedef LvnMat<4, 2, double>           LvnMat4x2d;
typedef LvnMat<4, 3, double>           LvnMat4x3d;
typedef LvnMat<2, 2, float>            LvnMat2f;
typedef LvnMat<3, 3, float>            LvnMat3f;
typedef LvnMat<4, 4, float>            LvnMat4f;
typedef LvnMat<2, 3, float>            LvnMat2x3f;
typedef LvnMat<2, 4, float>            LvnMat2x4f;
typedef LvnMat<3, 2, float>            LvnMat3x2f;
typedef LvnMat<3, 4, float>            LvnMat3x4f;
typedef LvnMat<4, 2, float>            LvnMat4x2f;
typedef LvnMat<4, 3, float>            LvnMat4x3f;

typedef LvnQuat_t<float>               LvnQuat;
typedef LvnQuat_t<int>                 LvnQuati;
typedef LvnQuat_t<unsigned int>        LvnQuatui;
typedef LvnQuat_t<float>               LvnQuatf;
typedef LvnQuat_t<double>              LvnQuatd;


// ------------------------------------------------------------
// [SECTION]: Functions
// ------------------------------------------------------------
// - All functions are declared in the lvn namespace

namespace lvn
{
    typedef LvnVec<2, float>               vec2;
    typedef LvnVec<3, float>               vec3;
    typedef LvnVec<4, float>               vec4;
    typedef LvnVec<2, int>                 vec2i;
    typedef LvnVec<3, int>                 vec3i;
    typedef LvnVec<4, int>                 vec4i;
    typedef LvnVec<2, unsigned int>        vec2ui;
    typedef LvnVec<3, unsigned int>        vec3ui;
    typedef LvnVec<4, unsigned int>        vec4ui;
    typedef LvnVec<2, double>              vec2d;
    typedef LvnVec<3, double>              vec3d;
    typedef LvnVec<4, double>              vec4d;
    typedef LvnVec<2, float>               vec2f;
    typedef LvnVec<3, float>               vec3f;
    typedef LvnVec<4, float>               vec4f;
    typedef LvnVec<2, bool>                vec2b;
    typedef LvnVec<3, bool>                vec3b;
    typedef LvnVec<4, bool>                vec4b;

    typedef LvnMat<2, 2, float>            mat2;
    typedef LvnMat<3, 3, float>            mat3;
    typedef LvnMat<4, 4, float>            mat4;
    typedef LvnMat<2, 3, float>            mat2x3;
    typedef LvnMat<2, 4, float>            mat2x4;
    typedef LvnMat<3, 2, float>            mat3x2;
    typedef LvnMat<3, 4, float>            mat3x4;
    typedef LvnMat<4, 2, float>            mat4x2;
    typedef LvnMat<4, 3, float>            mat4x3;
    typedef LvnMat<2, 2, int>              mat2i;
    typedef LvnMat<3, 3, int>              mat3i;
    typedef LvnMat<4, 4, int>              mat4i;
    typedef LvnMat<2, 3, int>              mat2x3i;
    typedef LvnMat<2, 4, int>              mat2x4i;
    typedef LvnMat<3, 2, int>              mat3x2i;
    typedef LvnMat<3, 4, int>              mat3x4i;
    typedef LvnMat<4, 2, int>              mat4x2i;
    typedef LvnMat<4, 3, int>              mat4x3i;
    typedef LvnMat<2, 2, unsigned int>     mat2ui;
    typedef LvnMat<3, 3, unsigned int>     mat3ui;
    typedef LvnMat<4, 4, unsigned int>     mat4ui;
    typedef LvnMat<2, 3, unsigned int>     mat2x3ui;
    typedef LvnMat<2, 4, unsigned int>     mat2x4ui;
    typedef LvnMat<3, 2, unsigned int>     mat3x2ui;
    typedef LvnMat<3, 4, unsigned int>     mat3x4ui;
    typedef LvnMat<4, 2, unsigned int>     mat4x2ui;
    typedef LvnMat<4, 3, unsigned int>     mat4x3ui;
    typedef LvnMat<2, 2, double>           mat2d;
    typedef LvnMat<3, 3, double>           mat3d;
    typedef LvnMat<4, 4, double>           mat4d;
    typedef LvnMat<2, 3, double>           mat2x3d;
    typedef LvnMat<2, 4, double>           mat2x4d;
    typedef LvnMat<3, 2, double>           mat3x2d;
    typedef LvnMat<3, 4, double>           mat3x4d;
    typedef LvnMat<4, 2, double>           mat4x2d;
    typedef LvnMat<4, 3, double>           mat4x3d;
    typedef LvnMat<2, 2, float>            mat2f;
    typedef LvnMat<3, 3, float>            mat3f;
    typedef LvnMat<4, 4, float>            mat4f;
    typedef LvnMat<2, 3, float>            mat2x3f;
    typedef LvnMat<2, 4, float>            mat2x4f;
    typedef LvnMat<3, 2, float>            mat3x2f;
    typedef LvnMat<3, 4, float>            mat3x4f;
    typedef LvnMat<4, 2, float>            mat4x2f;
    typedef LvnMat<4, 3, float>            mat4x3f;

    typedef LvnQuat_t<float>               quat;
    typedef LvnQuat_t<int>                 quati;
    typedef LvnQuat_t<unsigned int>        quatui;
    typedef LvnQuat_t<float>               quatf;
    typedef LvnQuat_t<double>              quatd;

    typedef LvnVertex                       vertex;


    // -- [SUBSECT]: Core Functions
    // ------------------------------------------------------------

    LVN_API LvnContext*             getContext();                                       // pointer to the Levikno Context created from the library

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
    LVN_API void                    writeFileSrc(const char* filename, const char* src, LvnFileMode mode); // write to a file given the file name, the source content of the file and the mode to write to the file

    LVN_API LvnFont                 loadFontFromFileTTF(const char* filepath, uint32_t fontSize, const uint32_t* pCodepoints = nullptr, uint32_t codepointCount = 0, LvnLoadFontFlagBits flags = Lvn_LoadFont_Default);    // get the font data from a ttf font file, font data will be stored in a LvnImageData struct which is an atlas texture containing all the font glyphs and their UV positions
    LVN_API LvnFont                 loadFontFromFileTTFMemory(const uint8_t* fontData, uint64_t fontDataSize, uint32_t fontSize, const uint32_t* pCodepoints = nullptr, uint32_t codepointCount = 0, LvnLoadFontFlagBits flags = Lvn_LoadFont_Default);
    LVN_API LvnFontGlyph            fontGetGlyph(const LvnFont& font, uint32_t codepoint);
    LVN_API uint32_t                decodeCodepointUTF8(const char* str, uint32_t* next);
    LVN_API LvnData<uint32_t>       getDefaultSupportedCodepoints();

    LVN_API void*                   memAlloc(size_t size);                              // custom memory allocation function that allocates memory given the size of memory, note that function is connected with the context and will keep track of allocation counts, will increment number of allocations per use
    LVN_API void                    memFree(void* ptr);                                 // custom memory free function, note that it keeps track of memory allocations remaining, decrements number of allocations per use with lvn::memAlloc
    LVN_API void*                   memRealloc(void* ptr, size_t size);                 // custom memory realloc function


    // -- [SUBSECT]: Logging Functions
    // ------------------------------------------------------------

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

    LVN_API void                        logEnable(bool enable);                                                           // enable or disable logging
    LVN_API void                        logEnableCoreLogging(bool enable);                                                // enable or disable logging from the core logger
    LVN_API void                        logSetLevel(LvnLogger* logger, LvnLogLevel level);                                // sets the log level of logger, will only print messages with set log level and higher
    LVN_API void                        logSetFileConfig(LvnLogger* logger, bool enable, const char* filename = "", LvnFileMode filemode = Lvn_FileMode_Write);  // sets the log file config, whether to enable logging and the log file name and mode
    LVN_API bool                        logCheckLevel(LvnLogger* logger, LvnLogLevel level);                              // checks level with loger, returns true if level is the same or higher level than the level of the logger
    LVN_API void                        logRenameLogger(LvnLogger* logger, const char* name);                             // renames the name of the logger
    LVN_API void                        logOutputMessage(LvnLogger* logger, LvnLogMessage* msg);                          // prints the log message
    LVN_API std::string                 logFormatMessage(LvnLogger* logger, LvnLogLevel level, const char* msg, bool removeANSI = false); // formats the log message into the log pattern set by the logger
    LVN_API void                        logMessage(LvnLogger* logger, LvnLogLevel level, const char* msg);                // log message with given log level
    LVN_API void                        logMessageTrace(LvnLogger* logger, const char* fmt, ...);                         // log message with level trace; ANSI code "\x1b[0;37m"
    LVN_API void                        logMessageDebug(LvnLogger* logger, const char* fmt, ...);                         // log message with level debug; ANSI code "\x1b[0;34m"
    LVN_API void                        logMessageInfo(LvnLogger* logger, const char* fmt, ...);                          // log message with level info;  ANSI code "\x1b[0;32m"
    LVN_API void                        logMessageWarn(LvnLogger* logger, const char* fmt, ...);                          // log message with level warn;  ANSI code "\x1b[1;33m"
    LVN_API void                        logMessageError(LvnLogger* logger, const char* fmt, ...);                         // log message with level error; ANSI code "\x1b[1;31m"
    LVN_API void                        logMessageFatal(LvnLogger* logger, const char* fmt, ...);                         // log message with level fatal; ANSI code "\x1b[1;37;41m"
    LVN_API LvnLogger*                  logGetCoreLogger();
    LVN_API LvnLogger*                  logGetClientLogger();
    LVN_API const char*                 logGetANSIcodeColor(LvnLogLevel level);                                           // get the ANSI color code of the log level in a string
    LVN_API LvnResult                   logSetPatternFormat(LvnLogger* logger, const char* patternfmt);                   // set the log pattern of the logger; messages outputed from that logger will be in this format
    LVN_API LvnResult                   logAddPatterns(LvnLogPattern* pLogPatterns, uint32_t count);                      // add user defined log patterns to the library

    LVN_API LvnResult                   createLogger(LvnLogger** logger, const LvnLoggerCreateInfo* loggerCreateInfo);
    LVN_API void                        destroyLogger(LvnLogger* logger);


    // -- [SUBSECT]: Event Functions
    // ------------------------------------------------------------
    // - Use these function within the call back function of LvnWindow (if set)

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

    LVN_API LvnResult                   createWindow(LvnWindow** window, const LvnWindowCreateInfo* createInfo);
    LVN_API void                        destroyWindow(LvnWindow* window);
    LVN_API LvnWindowCreateInfo         configWindowInit(const char* title, int width, int height);

    LVN_API void                        windowUpdate(LvnWindow* window);
    LVN_API bool                        windowOpen(LvnWindow* window);
    LVN_API void                        windowPollEvents();
    LVN_API LvnPair<int>                windowGetDimensions(LvnWindow* window);
    LVN_API int                         windowGetWidth(LvnWindow* window);
    LVN_API int                         windowGetHeight(LvnWindow* window);
    LVN_API void                        windowSetEventCallback(LvnWindow* window, void (*callback)(LvnEvent*), void* userData);
    LVN_API void                        windowSetVSync(LvnWindow* window, bool enable);
    LVN_API bool                        windowGetVSync(LvnWindow* window);
    LVN_API void*                       windowGetNativeWindow(LvnWindow* window);
    LVN_API LvnRenderPass*              windowGetRenderPass(LvnWindow* window);
    LVN_API void                        windowSetContextCurrent(LvnWindow* window);


    // -- [SUBSECT]: Input Functions
    // ------------------------------------------------------------
    // - Use to get user input (eg. keyboard, mouse, window input)

    LVN_API bool                        keyPressed(LvnWindow* window, int keycode);
    LVN_API bool                        keyReleased(LvnWindow* window, int keycode);
    LVN_API bool                        mouseButtonPressed(LvnWindow* window, int button);
    LVN_API bool                        mouseButtonReleased(LvnWindow* window, int button);

    LVN_API LvnPair<float>              mouseGetPos(LvnWindow* window);
    LVN_API void                        mouseGetPos(LvnWindow* window, float* xpos, float* ypos);
    LVN_API float                       mouseGetX(LvnWindow* window);
    LVN_API float                       mouseGetY(LvnWindow* window);
    LVN_API void                        mouseSetCursor(LvnWindow* window, LvnMouseCursor);
    LVN_API void                        mouseSetInputMode(LvnWindow* window, LvnMouseInputMode mode);

    LVN_API LvnPair<int>                windowGetPos(LvnWindow* window);
    LVN_API void                        windowGetPos(LvnWindow* window, int* xpos, int* ypos);
    LVN_API LvnPair<int>                windowGetSize(LvnWindow* window);
    LVN_API void                        windowGetSize(LvnWindow* window, int* width, int* height);


    // -- [SUBSECT]: Graphics Functions
    // ------------------------------------------------------------
    // - Renderer functions with the prefix CmdDraw can only
    //     be used during command recording
    // - New graphics objects cannot be created or deleted
    //     during command recording

    LVN_API LvnGraphicsApi              getGraphicsApi();
    LVN_API const char*                 getGraphicsApiName();
    LVN_API void                        getPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* deviceCount);
    LVN_API LvnPhysicalDeviceProperties getPhysicalDeviceProperties(LvnPhysicalDevice* physicalDevice);
    LVN_API LvnPhysicalDeviceFeatures   getPhysicalDeviceFeatures(LvnPhysicalDevice* physicalDevice);
    LVN_API LvnResult                   checkPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice);
    LVN_API LvnResult                   setPhysicalDevice(LvnPhysicalDevice* physicalDevice);
    LVN_API LvnClipRegion               getRenderClipRegionEnum();

    LVN_API void                        renderBeginNextFrame(LvnWindow* window);                                                                          // begins the next frame of the window
    LVN_API void                        renderDrawSubmit(LvnWindow* window);                                                                              // submits all draw commands recorded and presents to window
    LVN_API void                        renderBeginCommandRecording(LvnWindow* window);                                                                   // begins command buffer when recording draw commands start
    LVN_API void                        renderEndCommandRecording(LvnWindow* window);                                                                     // ends command buffer when finished recording draw commands
    LVN_API void                        renderCmdDraw(LvnWindow* window, uint32_t vertexCount);
    LVN_API void                        renderCmdDrawIndexed(LvnWindow* window, uint32_t indexCount);
    LVN_API void                        renderCmdDrawInstanced(LvnWindow* window, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
    LVN_API void                        renderCmdDrawIndexedInstanced(LvnWindow* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
    LVN_API void                        renderCmdSetStencilReference(uint32_t reference);
    LVN_API void                        renderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask);
    LVN_API void                        renderCmdBeginRenderPass(LvnWindow* window, float r, float g, float b, float a);                                  // begins renderpass when rendering starts
    LVN_API void                        renderCmdEndRenderPass(LvnWindow* window);                                                                        // ends renderpass when rendering has finished
    LVN_API void                        renderCmdBindPipeline(LvnWindow* window, LvnPipeline* pipeline);                                                  // bind a pipeline to begin shading during rendering
    LVN_API void                        renderCmdBindVertexBuffer(LvnWindow* window, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets); // binds the vertex buffer within an LvnBuffer object
    LVN_API void                        renderCmdBindIndexBuffer(LvnWindow* window, LvnBuffer* buffer, uint64_t offset);                                  // binds the index buffer within an LvnBuffer object
    LVN_API void                        renderCmdBindDescriptorSets(LvnWindow* window, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets); // bind multiple descriptor sets to the shader (if multiple sets are used), Note that descriptor sets must be in order to how the sets are ordered in the pipeline
    LVN_API void                        renderCmdBeginFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);                                        // begins the framebuffer for recording offscreen render calls, similar to beginning the render pass
    LVN_API void                        renderCmdEndFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);                                          // ends recording to the framebuffer

    LVN_API LvnResult                   createShaderFromSrc(LvnShader** shader, const LvnShaderCreateInfo* createInfo);                                   // create shader with the source code as input
    LVN_API LvnResult                   createShaderFromFileBin(LvnShader** shader, const LvnShaderCreateInfo* createInfo);                               // create shader with the file paths to the binary files (.spv) as input
    LVN_API LvnResult                   createShaderFromFileSrc(LvnShader** shader, const LvnShaderCreateInfo* createInfo);                               // create shader with the file paths to the source files as input
    LVN_API LvnResult                   createDescriptorLayout(LvnDescriptorLayout** descriptorLayout, const LvnDescriptorLayoutCreateInfo* createInfo);  // create descriptor layout for the pipeline
    LVN_API LvnResult                   createPipeline(LvnPipeline** pipeline, const LvnPipelineCreateInfo* createInfo);                                  // create pipeline to describe shading specifications
    LVN_API LvnResult                   createFrameBuffer(LvnFrameBuffer** frameBuffer, const LvnFrameBufferCreateInfo* createInfo);                      // create framebuffer to render images to
    LVN_API LvnResult                   createBuffer(LvnBuffer** buffer, const LvnBufferCreateInfo* createInfo);                                          // create a single buffer object that can hold both the vertex and index buffers
    LVN_API LvnResult                   createSampler(LvnSampler** sampler, const LvnSamplerCreateInfo* createInfo);                                      // create a sampler object to store texture sampler data
    LVN_API LvnResult                   createTexture(LvnTexture** texture, const LvnTextureCreateInfo* createInfo);                                      // create a texture object to store image data
    LVN_API LvnResult                   createTexture(LvnTexture** texture, const LvnTextureSamplerCreateInfo* createInfo);                               // create a texture object to store image data given a sampler object
    LVN_API LvnResult                   createCubemap(LvnCubemap** cubemap, const LvnCubemapCreateInfo* createInfo);                                      // create a cubemap texture object that holds the textures of the cubemap
    LVN_API LvnResult                   createCubemap(LvnCubemap** cubemap, const LvnCubemapHdrCreateInfo* createInfo);                                   // create a cubemap texture object that holds the hdr texture of the cubemap


    LVN_API void                        destroyShader(LvnShader* shader);                                                                                 // destroy shader module object
    LVN_API void                        destroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout);                                                   // destroy descriptor layout
    LVN_API void                        destroyPipeline(LvnPipeline* pipeline);                                                                           // destroy pipeline object
    LVN_API void                        destroyFrameBuffer(LvnFrameBuffer* frameBuffer);                                                                  // destroy framebuffer object
    LVN_API void                        destroyBuffer(LvnBuffer* buffer);                                                                                 // destory buffers object
    LVN_API void                        destroySampler(LvnSampler* sampler);                                                                              // destroy sampler object
    LVN_API void                        destroyTexture(LvnTexture* texture);                                                                              // destroy texture object
    LVN_API void                        destroyCubemap(LvnCubemap* cubemap);                                                                              // destroy cubemap object

    LVN_API uint32_t                    getAttributeFormatSize(LvnAttributeFormat format);
    LVN_API uint32_t                    getAttributeFormatComponentSize(LvnAttributeFormat format);
    LVN_API bool                        isAttributeFormatNormalizedType(LvnAttributeFormat format);
    LVN_API void                        pipelineSpecificationSetConfig(LvnPipelineSpecification* pipelineSpecification);
    LVN_API LvnPipelineSpecification    configPipelineSpecificationInit();
    LVN_API LvnResult                   allocateDescriptorSet(LvnDescriptorSet** descriptorSet, LvnDescriptorLayout* descriptorLayout);                   // create descriptor set to uplaod uniform data to pipeline

    LVN_API void                        bufferUpdateData(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset);
    LVN_API void                        bufferResize(LvnBuffer* buffer, uint64_t size);

    LVN_API LvnTexture*                 cubemapGetTextureData(LvnCubemap* cubemap);                                                                               // get the cubemap texture from the cubemap

    LVN_API void                        updateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count);           // update the descriptor content within a descroptor set

    LVN_API LvnTexture*                 frameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex);                                               // get the texture image data (render pass attachment) from the framebuffer via the attachment index
    LVN_API LvnRenderPass*              frameBufferGetRenderPass(LvnFrameBuffer* frameBuffer);                                                                    // get the render pass from the framebuffer
    LVN_API void                        frameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height);                                          // update the width and height of the new framebuffer (updates the image data dimensions), Note: call only when the image dimensions need to be changed
    LVN_API void                        frameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a);      // set the background color for the framebuffer for offscreen rendering
    LVN_API LvnDepthImageFormat         findSupportedDepthImageFormat(LvnDepthImageFormat* pDepthImageFormats, uint32_t count);

    LVN_API LvnImageData                loadImageData(const char* filepath, int forceChannels = 0, bool flipVertically = false);
    LVN_API LvnImageData                loadImageDataMemory(const uint8_t* data, int length, int forceChannels = 0, bool flipVertically = false);
    LVN_API LvnImageData                loadImageDataThread(const std::string filepath, int forceChannels = 0, bool flipVertically = false);
    LVN_API LvnImageData                loadImageDataMemoryThread(const uint8_t* data, int length, int forceChannels = 0, bool flipVertically = false);
    LVN_API LvnImageHdrData             loadHdrImageData(const char* filepath, int forceChannels = 0, bool flipVertically = false);

    LVN_API LvnResult                   writeImagePng(const LvnImageData& imageData, const char* filename);               // writes the image data into a png file with the filename/filepath
    LVN_API LvnResult                   writeImageJpg(const LvnImageData& imageData, const char* filename, int quality);  // writes the image data into a jpg file with the filename/filepath and the jpg quality (from 0...100)
    LVN_API LvnResult                   writeImageBmp(const LvnImageData& imageData, const char* filename);               // writes the image data into a bmp file with the filename/filepath

    LVN_API void                        imageFlipVertically(LvnImageData& imageData);                                     // flips the image vertically
    LVN_API void                        imageFlipHorizontally(LvnImageData& imageData);                                   // flips the image horizontally
    LVN_API void                        imageRotateCW(LvnImageData& imageData);                                           // rotates the image clockwise (right)
    LVN_API void                        imageRotateCCW(LvnImageData& imageData);                                          // rotates the image counter clockwise (left)

    LVN_API LvnImageData                imageGenWhiteNoise(uint32_t width, uint32_t height, uint32_t channels, uint32_t seed = 0);

    LVN_API LvnModel                    loadModel(const char* filepath);
    LVN_API void                        unloadModel(LvnModel* model);


    // -- [SUBSECT]: Audio Functions
    // ------------------------------------------------------------

    LVN_API LvnResult                   createSound(LvnSound** sound, const LvnSoundCreateInfo* createInfo);
    LVN_API void                        destroySound(LvnSound* sound);
    LVN_API LvnSoundCreateInfo          configSoundInit(const char* filepath);

    LVN_API void                        soundSetVolume(LvnSound* sound, float volume);
    LVN_API void                        soundSetPan(LvnSound* sound, float pan);
    LVN_API void                        soundSetPitch(LvnSound* sound, float pitch);
    LVN_API void                        soundSetLooping(LvnSound* sound, bool looping);
    LVN_API void                        soundPlayStart(LvnSound* sound);
    LVN_API void                        soundPlayStop(LvnSound* sound);
    LVN_API void                        soundTogglePause(LvnSound* sound);
    LVN_API bool                        soundIsPlaying(LvnSound* sound);
    LVN_API uint64_t                    soundGetTimeMiliseconds(LvnSound* sound);
    LVN_API float                       soundGetLengthSeconds(LvnSound* sound);


    // -- [SUBSECT]: Networking Functions
    // ------------------------------------------------------------

    LVN_API LvnResult                   createSocket(LvnSocket** socket, const LvnSocketCreateInfo* createInfo);
    LVN_API void                        destroySocket(LvnSocket* socket);
    LVN_API LvnSocketCreateInfo         configSocketClientInit(uint32_t connectionCount, uint32_t channelCount, uint32_t inBandwidth, uint32_t outBandWidth);
    LVN_API LvnSocketCreateInfo         configSocketServerInit(LvnAddress address, uint32_t connectionCount, uint32_t channelCount, uint32_t inBandwidth, uint32_t outBandWidth);

    LVN_API uint32_t                    socketGetHostFromStr(const char* host);
    LVN_API LvnResult                   socketConnect(LvnSocket* socket, LvnAddress* address, uint32_t channelCount, uint32_t milliseconds);
    LVN_API LvnResult                   socketDisconnect(LvnSocket* socket, uint32_t milliseconds);
    LVN_API void                        socketSend(LvnSocket* socket, uint8_t channel, LvnPacket* packet);
    LVN_API LvnResult                   socketReceive(LvnSocket* socket, LvnPacket* packet, uint32_t milliseconds);


    // -- [SUBSECT]: Renderer Functions
    // ------------------------------------------------------------
    // - high end api functions

    LVN_API LvnResult                   renderInit(const char* title, int width, int height);
    LVN_API LvnResult                   renderInit(const LvnWindowCreateInfo* createInfo);
    LVN_API void                        renderTerminate();
    LVN_API LvnWindow*                  getRendererWindow();
    LVN_API bool                        renderWindowOpen();
    LVN_API void                        drawBegin();
    LVN_API void                        drawEnd();
    LVN_API void                        drawClearColor(float r, float g, float b, float a);
    LVN_API void                        drawClearColor(const LvnColor& color);
    LVN_API void                        drawTriangle(const LvnVec2& v1, const LvnVec2& v2, const LvnVec2& v3, const LvnColor& color);
    LVN_API void                        drawRect(const LvnVec2& pos, const LvnVec2& size, const LvnColor& color);
    LVN_API void                        drawCircle(const LvnVec2& pos, float radius, const LvnColor& color);
    LVN_API void                        drawCircleSector(const LvnVec2& pos, float radius, float startAngle, float endAngle, const LvnColor& color);
    LVN_API void                        drawPolyNgon(const LvnVec2& pos, float radius, uint32_t nSides, const LvnColor& color);
    LVN_API void                        drawPolyNgonSector(const LvnVec2& pos, float radius, float startAngle, float endAngle, uint32_t nSides, const LvnColor& color);
    LVN_API void                        drawText(const char* text, const LvnVec2& pos, const LvnColor& color, float scale);
    LVN_API void                        drawTextEx(const char* text, const LvnVec2& pos, const LvnColor& color, float scale, float lineHeight, float textBoxWidth);


    // -- [SUBSECT]: ECS Functions
    // ------------------------------------------------------------

    LvnEntity createEntity();
    void destroyEntity(LvnEntity entity);
    LvnComponentManager* getComponentManager();

    template <typename T>
    void entityAddComponent(LvnEntity entity, const T& comp)
    {
        LvnComponentManager* compManager = lvn::getComponentManager();
        if (!compManager->has_component<T>())
            compManager->add_component<T>();

        LvnComponentArray<T>& compArray = compManager->get_component<T>();
        compArray.add_entity(entity, comp);
    }

    template <typename T, typename... Args>
    void entityAddComponent(LvnEntity entity, const T& comp, const Args&... args)
    {
        LvnComponentManager* compManager = lvn::getComponentManager();
        if (!compManager->has_component<T>())
            compManager->add_component<T>();

        LvnComponentArray<T>& compArray = compManager->get_component<T>();
        compArray.add_entity(entity, comp);

        entityAddComponent(entity, args...);
    }

    template <typename T>
    void entityRemoveComponent(LvnEntity entity)
    {
        LvnComponentArray<T>& compArray = lvn::getComponentManager()->get_component<T>();
        compArray.remove_entity(entity);
    }

    template <typename T, typename T2, typename... Args>
    void entityRemoveComponent(LvnEntity entity)
    {
        LvnComponentArray<T>& compArray = lvn::getComponentManager()->get_component<T>();
        compArray.remove_entity(entity);

        entityRemoveComponent<T2, Args...>(entity);
    }

    template <typename T>
    T& entityGetComponent(LvnEntity entity)
    {
        LvnComponentArray<T>& compArray = lvn::getComponentManager()->get_component<T>();
        return compArray.get_entity_component(entity);
    }

    template <typename... Ts>
    void entityUpdateSystem(LvnEntity* pEntities, uint64_t entityCount, void (*func)(Ts&...))
    {
        for (uint64_t i = 0; i < entityCount; i++)
        {
            func(entityGetComponent<Ts>(pEntities[i])...);
        }
    }



    // -- [SUBSECT]: Math Functions
    // ------------------------------------------------------------

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

    template <typename T>
    LVN_API bool                        within(T num, T within, T range) { return num <= (within + range) && num >= (within - range); }

    template <typename T>
    LVN_API bool                        within(T num, T within, T lowerRange, T upperRange) { return num <= (within + upperRange) && num >= (within - lowerRange); }

    LVN_API float radians(float deg);          // convert degrees to radians
    LVN_API float degrees(float rad);          // convert radians to degrees
    LVN_API float clampAngle(float rad);       // clamps the given angle in radians to the translated angle between 0 and 2 PI
    LVN_API float clampAngleDeg(float deg);    // clamps the given angle in degrees to the translated angle between 0 and 2 PI
    LVN_API float invSqrt(float num);
    LVN_API double derivative(double (*func)(double), double x, double delta = 0.001); // finds the instantaneous slope of the function given with a delta offset

    template <typename T>
    LVN_API LvnVec<2, T> normalize(const LvnVec<2, T>& v)
    {
        T u = static_cast<T>(1) / sqrt(v.x * v.x + v.y * v.y);
        return LvnVec<2, T>(v.x * u, v.y * u);
    }

    template <typename T>
    LVN_API LvnVec<3, T> normalize(const LvnVec<3, T>& v)
    {
        T u = static_cast<T>(1) / sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        return LvnVec<3, T>(v.x * u, v.y * u, v.z * u);
    }

    template <typename T>
    LVN_API LvnVec<4, T> normalize(const LvnVec<4, T>& v)
    {
        T u = static_cast<T>(1) / sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
        return LvnVec<4, T>(v.x * u, v.y * u, v.z * u, v.w * u);
    }

    template <typename T>
    LVN_API LvnQuat_t<T> normalize(const LvnQuat_t<T>& quat)
    {
        const T qw = quat.w;
        const T qx = quat.x;
        const T qy = quat.y;
        const T qz = quat.z;

        const float n = static_cast<T>(1) / sqrt(qx * qx + qy * qy + qz * qz + qw * qw);

        return LvnQuat_t<T>(qw * n, qx * n, qy * n, qz * n);
    }

    template <typename T>
    LVN_API T mag(LvnVec<2, T> v)
    {
        return sqrt(v.x * v.x + v.y * v.y);
    }

    template <typename T>
    LVN_API T mag(LvnVec<3, T> v)
    {
        return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

    template <typename T>
    LVN_API T mag(LvnVec<4, T> v)
    {
        return sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
    }

    template <typename T>
    LVN_API T mag2(LvnVec<2, T> v)
    {
        return v.x * v.x + v.y * v.y;
    }

    template <typename T>
    LVN_API T mag2(LvnVec<3, T> v)
    {
        return v.x * v.x + v.y * v.y + v.z * v.z;
    }

    template <typename T>
    LVN_API T mag2(LvnVec<4, T> v)
    {
        return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
    }

    template <typename T>
    LVN_API T mag2(const LvnQuat_t<T>& q)
    {
        return q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z;
    }

    template <typename T>
    LVN_API LvnVec<3, T> cross(const LvnVec<3, T>& v1, const LvnVec<3, T>& v2)
    {
        const T cx = v1.y * v2.z - v1.z * v2.y;
        const T cy = v1.z * v2.x - v1.x * v2.z;
        const T cz = v1.x * v2.y - v1.y * v2.x;
        return LvnVec<3, T>(cx, cy, cz);
    }

    template <typename T>
    LVN_API T dot(const LvnVec<2, T>& v1, const LvnVec<2, T>& v2)
    {
        return v1.x * v2.x + v1.y * v2.y;
    }

    template <typename T>
    LVN_API T dot(const LvnVec<3, T>& v1, const LvnVec<3, T>& v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    template <typename T>
    LVN_API T dot(const LvnVec<4, T>& v1, const LvnVec<4, T>& v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
    }

    template <typename T>
    LVN_API T dot(const LvnQuat_t<T>& q1, const LvnQuat_t<T>& q2)
    {
        return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
    }

    template <typename T>
    LVN_API T angle(const LvnVec<3, T>& v1, const LvnVec<3, T>& v2)
    {
        return acos(lvn::clamp(lvn::dot(v1, v2), T(-1), T(1)));
    }

    template <typename T>
    LVN_API LvnQuat_t<T> conjugate(const LvnQuat_t<T>& q)
    {
        return LvnQuat_t<T>(q.w, -q.x, -q.y, -q.z);
    }

    template <typename T>
    LVN_API LvnMat<2, 2, T> transpose(const LvnMat<2, 2, T>& m)
    {
        LvnMat<2, 2, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        return result;
    }

    template <typename T>
    LVN_API LvnMat<3, 3, T> transpose(const LvnMat<3, 3, T>& m)
    {
        LvnMat<3, 3, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[0][2] = m[2][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        result[1][2] = m[2][1];
        result[2][0] = m[0][2];
        result[2][1] = m[1][2];
        result[2][2] = m[2][2];
        return result;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> transpose(const LvnMat<4, 4, T>& m)
    {
        LvnMat<4, 4, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[0][2] = m[2][0];
        result[0][3] = m[3][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        result[1][2] = m[2][1];
        result[1][3] = m[3][1];
        result[2][0] = m[0][2];
        result[2][1] = m[1][2];
        result[2][2] = m[2][2];
        result[2][3] = m[3][2];
        result[3][0] = m[0][3];
        result[3][1] = m[1][3];
        result[3][2] = m[2][3];
        result[3][3] = m[3][3];
        return result;
    }

    template <typename T>
    LVN_API LvnMat<3, 2, T> transpose(const LvnMat<2, 3, T>& m)
    {
        LvnMat<3, 2, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        result[2][0] = m[0][2];
        result[2][1] = m[1][2];
        return result;
    }

    template <typename T>
    LVN_API LvnMat<4, 2, T> transpose(const LvnMat<2, 4, T>& m)
    {
        LvnMat<4, 2, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        result[2][0] = m[0][2];
        result[2][1] = m[1][2];
        result[3][0] = m[0][3];
        result[3][1] = m[1][3];
        return result;
    }

    template <typename T>
    LVN_API LvnMat<2, 3, T> transpose(const LvnMat<3, 2, T>& m)
    {
        LvnMat<2, 3, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[0][2] = m[2][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        result[1][2] = m[2][1];
        return result;
    }

    template <typename T>
    LVN_API LvnMat<4, 3, T> transpose(const LvnMat<3, 4, T>& m)
    {
        LvnMat<4, 3, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[0][2] = m[2][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        result[1][2] = m[2][1];
        result[2][0] = m[0][2];
        result[2][1] = m[1][2];
        result[2][2] = m[2][2];
        result[3][0] = m[0][3];
        result[3][1] = m[1][3];
        result[3][2] = m[2][3];
        return result;
    }

    template <typename T>
    LVN_API LvnMat<2, 4, T> transpose(const LvnMat<4, 2, T>& m)
    {
        LvnMat<2, 4, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[0][2] = m[2][0];
        result[0][3] = m[3][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        result[1][2] = m[2][1];
        result[1][3] = m[3][1];
        return result;
    }

    template <typename T>
    LVN_API LvnMat<3, 4, T> transpose(const LvnMat<4, 3, T>& m)
    {
        LvnMat<3, 4, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[0][2] = m[2][0];
        result[0][3] = m[3][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        result[1][2] = m[2][1];
        result[1][3] = m[3][1];
        result[2][0] = m[0][2];
        result[2][1] = m[1][2];
        result[2][2] = m[2][2];
        result[2][3] = m[3][2];
        return result;
    }

    template <typename T>
    LVN_API T determinant(const LvnMat<2, 2, T>& m)
    {
        return m[0][0] * m[1][1] - m[1][0] * m[0][1];
    }

    template <typename T>
    LVN_API T determinant(const LvnMat<3, 3, T>& m)
    {
        return + m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
               - m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
               + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);
    }

    template <typename T>
    LVN_API T determinant(const LvnMat<4, 4, T>& m)
    {
        T sub00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
        T sub01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
        T sub02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
        T sub03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
        T sub04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
        T sub05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

        LvnVec<4, T> detCof(
            + (m[1][1] * sub00 - m[1][2] * sub01 + m[1][3] * sub02),
            - (m[1][0] * sub00 - m[1][2] * sub03 + m[1][3] * sub04),
            + (m[1][0] * sub01 - m[1][1] * sub03 + m[1][3] * sub05),
            - (m[1][0] * sub02 - m[1][1] * sub04 + m[1][2] * sub05));

        return m[0][0] * detCof[0] + m[0][1] * detCof[1] +
               m[0][2] * detCof[2] + m[0][3] * detCof[3];
    }

    template <typename T>
    LVN_API LvnMat<2, 2, T> inverse(const LvnMat<2, 2, T>& m)
    {
        T oneOverDeterminant = static_cast<T>(1) / (
            + m[0][0] * m[1][1]
            - m[1][0] * m[0][1]);

        LvnMat<2, 2, T> inverse(
            + m[1][1] * oneOverDeterminant,
            - m[0][1] * oneOverDeterminant,
            - m[1][0] * oneOverDeterminant,
            + m[0][0] * oneOverDeterminant);

        return inverse;
    }

    template <typename T>
    LVN_API LvnMat<3, 3, T> inverse(const LvnMat<3, 3, T>& m)
    {
        T oneOverDeterminant = static_cast<T>(1) / (
            + m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
            - m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
            + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]));

        LvnMat<3, 3, T> inverse;
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
    LVN_API LvnMat<4, 4, T> inverse(const LvnMat<4, 4, T>& m)
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

        LvnVec<4, T> fac0(coef00, coef00, coef02, coef03);
        LvnVec<4, T> fac1(coef04, coef04, coef06, coef07);
        LvnVec<4, T> fac2(coef08, coef08, coef10, coef11);
        LvnVec<4, T> fac3(coef12, coef12, coef14, coef15);
        LvnVec<4, T> fac4(coef16, coef16, coef18, coef19);
        LvnVec<4, T> fac5(coef20, coef20, coef22, coef23);

        LvnVec<4, T> vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
        LvnVec<4, T> vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
        LvnVec<4, T> vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
        LvnVec<4, T> vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

        LvnVec<4, T> inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
        LvnVec<4, T> inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
        LvnVec<4, T> inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
        LvnVec<4, T> inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

        LvnVec<4, T> signA(+1, -1, +1, -1);
        LvnVec<4, T> signB(-1, +1, -1, +1);
        LvnMat<4, 4, T> inverse(inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB);

        LvnVec<4, T> row0(inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0]);

        LvnVec<4, T> dot0(m[0] * row0);
        T dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

        T oneOverDeterminant = static_cast<T>(1) / dot1;

        return inverse * oneOverDeterminant;
    }

    template <typename T>
    LVN_API LvnQuat_t<T> inverse(const LvnQuat_t<T>& q)
    {
        return lvn::conjugate(q) / lvn::dot(q, q);
    }

    template <typename T>
    LVN_API T lerp(const T& start, const T& end, float t)
    {
        return start + t * (end - start);
    }

    template <typename T>
    LVN_API LvnVec<2, T> lerp(const LvnVec<2, T>& start, const LvnVec<2, T>& end, float t)
    {
        return LvnVec<2, T>(lerp(start.x, end.x, t), lerp(start.y, end.y, t));
    }

    template <typename T>
    LVN_API LvnVec<3, T> lerp(const LvnVec<3, T>& start, const LvnVec<3, T>& end, float t)
    {
        return LvnVec<3, T>(lerp(start.x, end.x, t), lerp(start.y, end.y, t), lerp(start.z, end.z, t));
    }

    template <typename T>
    LVN_API LvnVec<4, T> lerp(const LvnVec<4, T>& start, const LvnVec<4, T>& end, float t)
    {
        return LvnVec<4, T>(lerp(start.x, end.x, t), lerp(start.y, end.y, t), lerp(start.z, end.z, t), lerp(start.w, end.w, t));
    }

    template <typename T>
    LVN_API LvnQuat_t<T> slerp(const LvnQuat_t<T>& q1, const LvnQuat_t<T>& q2, float t)
    {
        LvnQuat_t<T> q2s = q2;

        T cosTheta = dot(q1, q2);

        if (cosTheta < static_cast<T>(0))
        {
            q2s = -q2;
            cosTheta = -cosTheta;
        }

        if(cosTheta > static_cast<T>(1) - std::numeric_limits<T>::epsilon())
        {
            return lvn::normalize(LvnQuat_t<T>(
                lvn::lerp(q1.w, q2s.w, t),
                lvn::lerp(q1.x, q2s.x, t),
                lvn::lerp(q1.y, q2s.y, t),
                lvn::lerp(q1.z, q2s.z, t)));
        }
        else
        {
            T angle = acos(cosTheta);
            return (sin((static_cast<T>(1) - t) * angle) * q1 + sin(t * angle) * q2s) / sin(angle);
        }
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> orthoRHZO(T left, T right, T bottom, T top, T zNear, T zFar)
    {
        LvnMat<4, 4, T> matrix(static_cast<T>(1));
        matrix[0][0] = static_cast<T>(2) / (right - left);
        matrix[1][1] = static_cast<T>(2) / (top - bottom);
        matrix[2][2] = - static_cast<T>(2) / (zFar - zNear);
        matrix[3][0] = - (right + left)  / (right - left);
        matrix[3][1] = - (top + bottom)  / (top - bottom);
        matrix[3][2] = - (zFar + zNear) / (zFar - zNear);
        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> orthoRHNO(T left, T right, T bottom, T top, T zNear, T zFar)
    {
        LvnMat<4, 4, T> matrix(static_cast<T>(1));
        matrix[0][0] = static_cast<T>(2) / (right - left);
        matrix[1][1] = static_cast<T>(2) / (top - bottom);
        matrix[2][2] = - static_cast<T>(2) / (zFar - zNear);
        matrix[3][0] = - (right + left)  / (right - left);
        matrix[3][1] = - (top + bottom)  / (top - bottom);
        matrix[3][2] = - zNear / (zFar - zNear);
        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> orthoLHZO(T left, T right, T bottom, T top, T zNear, T zFar)
    {
        LvnMat<4, 4, T> matrix(static_cast<T>(1));
        matrix[0][0] = static_cast<T>(2) / (right - left);
        matrix[1][1] = static_cast<T>(2) / (top - bottom);
        matrix[2][2] = static_cast<T>(1) / (zFar - zNear);
        matrix[3][0] = - (right + left)  / (right - left);
        matrix[3][1] = - (top + bottom)  / (top - bottom);
        matrix[3][2] = - zNear / (zFar - zNear);
        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> orthoLHNO(T left, T right, T bottom, T top, T zNear, T zFar)
    {
        LvnMat<4, 4, T> matrix(static_cast<T>(1));
        matrix[0][0] = static_cast<T>(2) / (right - left);
        matrix[1][1] = static_cast<T>(2) / (top - bottom);
        matrix[2][2] = static_cast<T>(2) / (zFar - zNear);
        matrix[3][0] = - (right + left)  / (right - left);
        matrix[3][1] = - (top + bottom)  / (top - bottom);
        matrix[3][2] = - (zFar + zNear) / (zFar - zNear);
        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> ortho(T left, T right, T bottom, T top, T zNear, T zFar)
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
    LVN_API LvnMat<4, 4, T> perspectiveRHZO(const T& fovy, const T& aspect, const T& zNear, const T& zFar)
    {
        T tanHalfFov = static_cast<T>(tan(fovy / 2));

        LvnMat<4, 4, T> matrix(0);
        matrix[0][0] = static_cast<T>(1) / (aspect * tanHalfFov);
        matrix[1][1] = static_cast<T>(1) / (tanHalfFov);
        matrix[2][2] = zFar / (zNear - zFar);
        matrix[2][3] = static_cast<T>(1);
        matrix[3][2] = - (zFar * zNear) / (zFar - zNear);

        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> perspectiveRHNO(const T& fovy, const T& aspect, const T& zNear, const T& zFar)
    {
        T tanHalfFov = static_cast<T>(tan(fovy / 2));

        LvnMat<4, 4, T> matrix(0);
        matrix[0][0] = static_cast<T>(1) / (aspect * tanHalfFov);
        matrix[1][1] = static_cast<T>(1) / (tanHalfFov);
        matrix[2][2] = - (zFar + zNear) / (zFar - zNear);
        matrix[2][3] = - static_cast<T>(1);
        matrix[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);

        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> perspectiveLHZO(const T& fovy, const T& aspect, const T& zNear, const T& zFar)
    {
        T tanHalfFov = static_cast<T>(tan(fovy / 2));

        LvnMat<4, 4, T> matrix(0);
        matrix[0][0] = static_cast<T>(1) / (aspect * tanHalfFov);
        matrix[1][1] = static_cast<T>(1) / (tanHalfFov);
        matrix[2][2] = zFar / (zFar - zNear);
        matrix[2][3] = static_cast<T>(1);
        matrix[3][2] = - (zFar * zNear) / (zFar - zNear);

        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> perspectiveLHNO(const T& fovy, const T& aspect, const T& zNear, const T& zFar)
    {
        T tanHalfFov = static_cast<T>(tan(fovy / 2));

        LvnMat<4, 4, T> matrix(0);
        matrix[0][0] = static_cast<T>(1) / (aspect * tanHalfFov);
        matrix[1][1] = static_cast<T>(1) / (tanHalfFov);
        matrix[2][2] = (zFar + zNear) / (zFar - zNear);
        matrix[2][3] = static_cast<T>(1);
        matrix[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);

        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> perspective(const T& fovy, const T& aspect, const T& zNear, const T& zFar)
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
    LVN_API LvnMat<4, 4, T> lookAtRH(const LvnVec<3, T>& eye, const LvnVec<3, T>& center, const LvnVec<3, T>& up)
    {
        LvnVec<3, T> f(lvn::normalize(center - eye));
        LvnVec<3, T> s(lvn::normalize(lvn::cross(f, up)));
        LvnVec<3, T> u(lvn::cross(s, f));

        LvnMat<4, 4, T> matrix(static_cast<T>(1));
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
    LVN_API LvnMat<4, 4, T> lookAtLH(const LvnVec<3, T>& eye, const LvnVec<3, T>& center, const LvnVec<3, T>& up)
    {
        LvnVec<3, T> f(lvn::normalize(center - eye));
        LvnVec<3, T> s(lvn::normalize(lvn::cross(up, f)));
        LvnVec<3, T> u(lvn::cross(f, s));

        LvnMat<4, 4, T> matrix(static_cast<T>(1));
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
    LVN_API LvnMat<4, 4, T> lookAt(const LvnVec<3, T>& eye, const LvnVec<3, T>& center, const LvnVec<3, T>& up)
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
    LVN_API LvnMat<4, 4, T> translate(const LvnMat<4, 4, T>& mat, const LvnVec<3, T>& vec)
    {
        LvnMat<4, 4, T> translate(static_cast<T>(1));
        translate[3][0] = vec.x;
        translate[3][1] = vec.y;
        translate[3][2] = vec.z;

        return mat * translate;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> scale(const LvnMat<4, 4, T>& mat, const LvnVec<3, T>& vec)
    {
        LvnMat<4, 4, T> scale(static_cast<T>(1));
        scale[0][0] = vec.x;
        scale[1][1] = vec.y;
        scale[2][2] = vec.z;

        return mat * scale;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> rotate(const LvnMat<4, 4, T>& mat, T angle, const LvnVec<3, T>& axis)
    {
        const T c = cos(angle);
        const T s = sin(angle);
        const T nc = static_cast<T>(1) - cos(angle);

        LvnMat<4, 4, T> rotate(static_cast<T>(1));
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
    LvnVec<2, T> rotate(const LvnVec<2, T>& v, const T& angle)
    {
        LvnVec<2, T> result;
        const T rcos(cos(angle));
        const T rsin(sin(angle));

        result.x = v.x * rcos - v.y * rsin;
        result.y = v.x * rsin + v.y * rcos;
        return result;
    }

    template <typename T>
    LvnVec<3, T> rotate(const LvnVec<3, T>& v, const T& angle, const LvnVec<3, T>& axis)
    {
        return LvnMat<3, 3, T>(lvn::rotate(LvnMat<4, 4, T>(static_cast<T>(1)), angle, axis)) * v;
    }

    template <typename T>
    LVN_API LvnQuat_t<T> angleAxis(const T& angle, const LvnVec<3, T>& axis)
    {
        const T s = sin(angle / 2);
        return LvnQuat_t<T>(cos(angle / 2), axis.x * s, axis.y * s, axis.z * s);
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> quatToMat4(const LvnQuat_t<T> quat)
    {
        const T w = quat.w;
        const T x = quat.x;
        const T y = quat.y;
        const T z = quat.z;

        LvnMat<4, 4, T> matrix(static_cast<T>(1));
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


// ------------------------------------------------------------
// [SECTION]: Struct Implementations
// ------------------------------------------------------------


// -- [SUBSECT]: Data Structures
// ------------------------------------------------------------
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


template<typename T, size_t N>
class LvnArray
{
private:
    T m_Data[N];

public:
    LvnArray() {}
    LvnArray(T* data, size_t size)
    {
        memcpy(m_Data, data, size);
    }

    T& operator [](size_t i)
    {
        LVN_CORE_ASSERT(i < N, "%s, element index out of range", typeid(this).name());
        return m_Data[i];
    }
    const T& operator [](size_t i) const
    {
        LVN_CORE_ASSERT(i < N, "%s, element index out of range", typeid(this).name());
        return m_Data[i];
    }

    T* data()                { return m_Data; }
    const T* data() const    { return m_Data; }
    size_t size()            { return N; }
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
        delete [] m_Data;
    }

    LvnData(const T* data, size_t size)
    {
        m_Size = size;
        m_MemSize = size * sizeof(T);
        m_Data = new T[size];

        for (size_t i = 0; i < size; i++)
            m_Data[i] = data[i];
    }
    LvnData(const LvnData<T>& other)
    {
        m_Size = other.m_Size;
        m_MemSize = other.m_MemSize;
        m_Data = new T[other.m_Size];

        for (size_t i = 0; i < other.m_Size; i++)
            m_Data[i] = other.m_Data[i];
    }
    LvnData(LvnData<T>&& other)
    {
        m_Size = other.m_Size;
        m_MemSize = other.m_MemSize;
        m_Data = other.m_Data;
        other.m_Size = 0;
        other.m_MemSize = 0;
        other.m_Data = nullptr;
    }
    LvnData<T>& operator=(const LvnData<T>& other)
    {
        delete [] m_Data;
        m_Size = other.m_Size;
        m_MemSize = other.m_MemSize;
        m_Data = new T[other.m_Size];

        for (size_t i = 0; i < other.m_Size; i++)
            m_Data[i] = other.m_Data[i];

        return *this;
    }
    LvnData<T>& operator=(LvnData<T>&& other)
    {
        if (this != &other)
        {
            delete [] m_Data;

            m_Size = other.m_Size;
            m_MemSize = other.m_MemSize;
            m_Data = other.m_Data;

            other.m_Size = 0;
            other.m_MemSize = 0;
            other.m_Data = nullptr;
        }
        return *this;
    }

    T& operator[](size_t i)
    {
        LVN_CORE_ASSERT(i < m_Size, "%s, element index out of range", typeid(this).name());
        return m_Data[i];
    }
    const T& operator [](size_t i) const
    {
        LVN_CORE_ASSERT(i < m_Size, "%s, element index out of range", typeid(this).name());
        return m_Data[i];
    }

    size_t            size() { return m_Size; }
    size_t            memsize() { return m_MemSize; }
    const size_t      size() const { return m_Size; }
    const size_t      memsize() const { return m_MemSize; }

    T*                data() { return m_Data; }
    const T* const    data() const { return m_Data; }

    T*                begin() { return &m_Data[0]; }
    const T* const    begin() const { return &m_Data[0]; }
    T*                end() { return &m_Data[0] + m_Size; }
    const T* const    end() const { return &m_Data[0] + m_Size; }
    T*                front() { return &m_Data[0]; }
    const T* const    front() const { return &m_Data[0]; }
    T*                back() { return &m_Data[m_Size - 1]; }
    const T* const    back() const { return &m_Data[m_Size - 1]; }
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
        : m_Workers(1), m_Terminate(false)
    {
        for (uint32_t i = 0; i < m_Workers.size(); i++)
            m_Workers[i] = (std::thread(&LvnThreadPool::ThreadFunc, this));
    }

    LvnThreadPool(uint32_t workerCount)
        : m_Terminate(false)
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

struct LvnDrawCommand
{
    void* pVertices;
    uint32_t* pIndices;
    uint64_t vertexCount;
    uint64_t indexCount;
    uint64_t vertexStride;
};

class LvnDrawList
{
    std::vector<uint8_t> m_VerticesRaw;
    std::vector<uint32_t> m_Indices;
    std::vector<LvnDrawCommand> m_DrawCommands;
    size_t m_VertexCount;

public:
    void push_back(const LvnDrawCommand& drawCmd)
    {
        m_DrawCommands.push_back(drawCmd);
        
        m_Indices.insert(m_Indices.end(), drawCmd.pIndices, drawCmd.pIndices + drawCmd.indexCount);
        for (uint32_t i = m_Indices.size() - drawCmd.indexCount; i < m_Indices.size(); i++)
            m_Indices[i] += m_VertexCount;

        m_VerticesRaw.insert(m_VerticesRaw.end(), static_cast<uint8_t*>(drawCmd.pVertices), static_cast<uint8_t*>(drawCmd.pVertices) + drawCmd.vertexCount * drawCmd.vertexStride);
        m_VertexCount += drawCmd.vertexCount;
    }
    void clear()
    {
        m_DrawCommands.clear();
        m_VerticesRaw.clear();
        m_Indices.clear();
        m_VertexCount = 0;
    }
    bool empty()
    {
        return m_VerticesRaw.empty() && m_Indices.empty() && m_DrawCommands.empty();
    }

    void* vertices()                          { return m_VerticesRaw.data(); }
    const void* vertices() const              { return m_VerticesRaw.data(); }
    size_t vertex_count()                     { return m_VertexCount; }
    size_t vertex_size()                      { return m_VerticesRaw.size(); }

    uint32_t* indices()                       { return m_Indices.data(); }
    const uint32_t* indices() const           { return m_Indices.data(); }
    size_t index_count()                      { return m_Indices.size(); }
    size_t index_size()                       { return m_Indices.size() * sizeof(uint32_t); }

    LvnDrawCommand* drawcmds()                { return m_DrawCommands.data(); }
    const LvnDrawCommand* drawcmds() const    { return m_DrawCommands.data(); }
    size_t drawcmd_count()                    { return m_DrawCommands.size(); }
};



// -- [SUBSECT]: Vector Implementation
// ------------------------------------------------------------

template<typename T>
struct LvnVec<2, T>
{
    union { T x, r, s, i; };
    union { T y, g, t, j; };

    static length_t length() { return 2; }

    LvnVec() = default;
    LvnVec(const LvnVec<2, T>&) = default;
    LvnVec(const T& n)
        : x(n), y(n) {}
    LvnVec(const T& nx, const T& ny)
        : x(nx), y(ny) {}
    LvnVec(const LvnVec<3, T>& v)
        : x(v.x), y(v.y) {}
    LvnVec(const LvnVec<4, T>& v)
        : x(v.x), y(v.y) {}

    T& operator[](length_t i)
    {
        LVN_CORE_ASSERT(i >= 0 && i < this->length(), "vector index out of range");

        switch (i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        }
    }
    const T& operator[](length_t i) const
    {
        LVN_CORE_ASSERT(i >= 0 && i < this->length(), "vector index out of range");

        switch (i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        }
    }

    LvnVec<2, T>& operator+=(const LvnVec<2, T>& v)
    {
        this->x += v.x;
        this->y += v.y;
        return *this;
    }
    LvnVec<2, T>& operator-=(const LvnVec<2, T>& v)
    {
        this->x -= v.x;
        this->y -= v.y;
        return *this;
    }
    LvnVec<2, T>& operator*=(const LvnVec<2, T>& v)
    {
        this->x *= v.x;
        this->y *= v.y;
        return *this;
    }
    LvnVec<2, T>& operator/=(const LvnVec<2, T>& v)
    {
        this->x /= v.x;
        this->y /= v.y;
        return *this;
    }
    LvnVec<2, T>& operator++()
    {
        this->x++;
        this->y++;
        return *this;
    }
    LvnVec<2, T>& operator--()
    {
        this->x--;
        this->y--;
        return *this;
    }
    LvnVec<2, T> operator++(int)
    {
        LvnVec<2, T> vec(*this);
        ++*this;
        return vec;
    }
    LvnVec<2, T> operator--(int)
    {
        LvnVec<2, T> vec(*this);
        --*this;
        return vec;
    }
    LvnVec<2, T> operator+() const
    {
        return LvnVec<2, T>(this->x, this->y);
    }
    LvnVec<2, T> operator-() const
    {
        return LvnVec<2, T>(-this->x, -this->y);
    }
};

template <typename T>
LvnVec<2, T> operator+(const LvnVec<2, T>& v1, const LvnVec<2, T>& v2)
{
    return LvnVec<2, T>(v1.x + v2.x, v1.y + v2.y);
}
template <typename T>
LvnVec<2, T> operator-(const LvnVec<2, T>& v1, const LvnVec<2, T>& v2)
{
    return LvnVec<2, T>(v1.x - v2.x, v1.y - v2.y);
}
template <typename T>
LvnVec<2, T> operator*(const LvnVec<2, T>& v1, const LvnVec<2, T>& v2)
{
    return LvnVec<2, T>(v1.x * v2.x, v1.y * v2.y);
}
template <typename T>
LvnVec<2, T> operator/(const LvnVec<2, T>& v1, const LvnVec<2, T>& v2)
{
    return LvnVec<2, T>(v1.x / v2.x, v1.y / v2.y);
}
template <typename T>
LvnVec<2, T> operator+(const T& s, const LvnVec<2, T>& v)
{
    return LvnVec<2, T>(s + v.x, s + v.y);
}
template <typename T>
LvnVec<2, T> operator-(const T& s, const LvnVec<2, T>& v)
{
    return LvnVec<2, T>(s - v.x, s - v.y);
}
template <typename T>
LvnVec<2, T> operator*(const T& s, const LvnVec<2, T>& v)
{
    return LvnVec<2, T>(s * v.x, s * v.y);
}
template <typename T>
LvnVec<2, T> operator/(const T& s, const LvnVec<2, T>& v)
{
    return LvnVec<2, T>(s / v.x, s / v.y);
}
template <typename T>
LvnVec<2, T> operator+(const LvnVec<2, T>& v, const T& s)
{
    return LvnVec<2, T>(v.x + s, v.y + s);
}
template <typename T>
LvnVec<2, T> operator-(const LvnVec<2, T>& v, const T& s)
{
    return LvnVec<2, T>(v.x - s, v.y - s);
}
template <typename T>
LvnVec<2, T> operator*(const LvnVec<2, T>& v, const T& s)
{
    return LvnVec<2, T>(v.x * s, v.y * s);
}
template <typename T>
LvnVec<2, T> operator/(const LvnVec<2, T>& v, const T& s)
{
    return LvnVec<2, T>(v.x / s, v.y / s);
}


template<typename T>
struct LvnVec<3, T>
{
    union { T x, r, s, i; };
    union { T y, g, t, j; };
    union { T z, b, p, k; };

    static length_t length() { return 3; }

    LvnVec() = default;
    LvnVec(const LvnVec<3, T>&) = default;
    LvnVec(const T& n)
        : x(n), y(n), z(n) {}
    LvnVec(const T& nx, const T& ny, const T& nz)
        : x(nx), y(ny), z(nz) {}
    LvnVec(const LvnVec<4, T>& v)
        : x(v.x), y(v.y), z(v.z) {}
    LvnVec(const LvnVec<2, T>& v, const T& nz)
        : x(v.x), y(v.y), z(nz) {}
    LvnVec(const T& nx, const LvnVec<2, T>& v)
        : x(nx), y(v.x), z(v.y) {}

    T& operator[](length_t i)
    {
        LVN_CORE_ASSERT(i >= 0 && i < this->length(), "vector index out of range");

        switch (i)
        {
            default:
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }
    }
    const T& operator[](length_t i) const
    {
        LVN_CORE_ASSERT(i >= 0 && i < this->length(), "vector index out of range");

        switch (i)
        {
            default:
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }
    }

    LvnVec<3, T>& operator+=(const LvnVec<3, T>& v)
    {
        this->x += v.x;
        this->y += v.y;
        this->z += v.z;
        return *this;
    }
    LvnVec<3, T>& operator-=(const LvnVec<3, T>& v)
    {
        this->x -= v.x;
        this->y -= v.y;
        this->z -= v.z;
        return *this;
    }
    LvnVec<3, T>& operator*=(const LvnVec<3, T>& v)
    {
        this->x *= v.x;
        this->y *= v.y;
        this->z *= v.z;
        return *this;
    }
    LvnVec<3, T>& operator/=(const LvnVec<3, T>& v)
    {
        this->x /= v.x;
        this->y /= v.y;
        this->z /= v.z;
        return *this;
    }
    LvnVec<3, T>& operator++()
    {
        this->x++;
        this->y++;
        this->z++;
        return *this;
    }
    LvnVec<3, T>& operator--()
    {
        this->x--;
        this->y--;
        this->z--;
        return *this;
    }
    LvnVec<3, T> operator++(int)
    {
        LvnVec<3, T> vec(*this);
        ++*this;
        return vec;
    }
    LvnVec<3, T> operator--(int)
    {
        LvnVec<3, T> vec(*this);
        --*this;
        return vec;
    }
    LvnVec<3, T> operator+() const
    {
        return LvnVec<3, T>(this->x, this->y, this->z);
    }
    LvnVec<3, T> operator-() const
    {
        return LvnVec<3, T>(-this->x, -this->y, -this->z);
    }
};

template <typename T>
LvnVec<3, T> operator+(const LvnVec<3, T>& v1, const LvnVec<3, T>& v2)
{
    return LvnVec<3, T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
template <typename T>
LvnVec<3, T> operator-(const LvnVec<3, T>& v1, const LvnVec<3, T>& v2)
{
    return LvnVec<3, T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
template <typename T>
LvnVec<3, T> operator*(const LvnVec<3, T>& v1, const LvnVec<3, T>& v2)
{
    return LvnVec<3, T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}
template <typename T>
LvnVec<3, T> operator/(const LvnVec<3, T>& v1, const LvnVec<3, T>& v2)
{
    return LvnVec<3, T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}
template <typename T>
LvnVec<3, T> operator+(const T& s, const LvnVec<3, T>& v)
{
    return LvnVec<3, T>(s + v.x, s + v.y, s + v.z);
}
template <typename T>
LvnVec<3, T> operator-(const T& s, const LvnVec<3, T>& v)
{
    return LvnVec<3, T>(s - v.x, s - v.y, s - v.z);
}
template <typename T>
LvnVec<3, T> operator*(const T& s, const LvnVec<3, T>& v)
{
    return LvnVec<3, T>(s * v.x, s * v.y, s * v.z);
}
template <typename T>
LvnVec<3, T> operator/(const T& s, const LvnVec<3, T>& v)
{
    return LvnVec<3, T>(s / v.x, s / v.y, s / v.z);
}
template <typename T>
LvnVec<3, T> operator+(const LvnVec<3, T>& v, const T& s)
{
    return LvnVec<3, T>(v.x + s, v.y + s, v.z + s);
}
template <typename T>
LvnVec<3, T> operator-(const LvnVec<3, T>& v, const T& s)
{
    return LvnVec<3, T>(v.x - s, v.y - s, v.z - s);
}
template <typename T>
LvnVec<3, T> operator*(const LvnVec<3, T>& v, const T& s)
{
    return LvnVec<3, T>(v.x * s, v.y * s, v.z * s);
}
template <typename T>
LvnVec<3, T> operator/(const LvnVec<3, T>& v, const T& s)
{
    return LvnVec<3, T>(v.x / s, v.y / s, v.z / s);
}


template<typename T>
struct LvnVec<4, T>
{
    union { T x, r, s, i; };
    union { T y, g, t, j; };
    union { T z, b, p, k; };
    union { T w, a, q, l; };

    static length_t length() { return 4; }

    LvnVec() = default;
    LvnVec(const LvnVec<4, T>&) = default;
    LvnVec(const T& n)
        : x(n), y(n), z(n) {}
    LvnVec(const T& nx, const T& ny, const T& nz, const T& nw)
        : x(nx), y(ny), z(nz), w(nw) {}
    LvnVec(const LvnVec<2, T>& v1, LvnVec<2, T>& v2)
        : x(v1.x), y(v1.y), z(v2.x), w(v2.y) {}
    LvnVec(const LvnVec<2, T>& v, const T& nz, const T& nw)
        : x(v.x), y(v.y), z(nz), w(nw) {}
    LvnVec(const T& nx, const T& ny, const LvnVec<2, T>& v)
        : x(nx), y(ny), z(v.x), w(v.y) {}
    LvnVec(const T& nx, const LvnVec<2, T>& v, const T& nw)
        : x(nx), y(v.x), z(v.y), w(nw) {}
    LvnVec(const LvnVec<3, T>& v, const T& nw)
        : x(v.x), y(v.y), z(v.z), w(nw) {}
    LvnVec(const T& nx, const LvnVec<3, T>& v)
        : x(nx), y(v.x), z(v.y), w(v.z) {}

    T& operator[](length_t i)
    {
        LVN_CORE_ASSERT(i >= 0 && i < this->length(), "vector index out of range");

        switch (i)
        {
            default:
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
        }
    }
    const T& operator[](length_t i) const
    {
        LVN_CORE_ASSERT(i >= 0 && i < this->length(), "vector index out of range");

        switch (i)
        {
            default:
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
        }
    }

    LvnVec<4, T>& operator+=(const LvnVec<4, T>& v)
    {
        this->x += v.x;
        this->y += v.y;
        this->z += v.z;
        this->w += v.w;
        return *this;
    }
    LvnVec<4, T>& operator-=(const LvnVec<4, T>& v)
    {
        this->x -= v.x;
        this->y -= v.y;
        this->z -= v.z;
        this->w -= v.w;
        return *this;
    }
    LvnVec<4, T>& operator*=(const LvnVec<4, T>& v)
    {
        this->x *= v.x;
        this->y *= v.y;
        this->z *= v.z;
        this->w *= v.w;
        return *this;
    }
    LvnVec<4, T>& operator/=(const LvnVec<4, T>& v)
    {
        this->x /= v.x;
        this->y /= v.y;
        this->z /= v.z;
        this->w *= v.w;
        return *this;
    }
    LvnVec<4, T>& operator++()
    {
        this->x++;
        this->y++;
        this->z++;
        this->w++;
        return *this;
    }
    LvnVec<4, T>& operator--()
    {
        this->x--;
        this->y--;
        this->z--;
        this->w--;
        return *this;
    }
    LvnVec<4, T> operator++(int)
    {
        LvnVec<4, T> vec(*this);
        ++*this;
        return vec;
    }
    LvnVec<4, T> operator--(int)
    {
        LvnVec<4, T> vec(*this);
        --*this;
        return vec;
    }
    LvnVec<4, T> operator+() const
    {
        return LvnVec<4, T>(this->x, this->y, this->z, this->w);
    }
    LvnVec<4, T> operator-() const
    {
        return LvnVec<4, T>(-this->x, -this->y, -this->z, -this->w);
    }
};

template <typename T>
LvnVec<4, T> operator+(const LvnVec<4, T>& v1, const LvnVec<4, T>& v2)
{
    return LvnVec<4, T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}
template <typename T>
LvnVec<4, T> operator-(const LvnVec<4, T>& v1, const LvnVec<4, T>& v2)
{
    return LvnVec<4, T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}
template <typename T>
LvnVec<4, T> operator*(const LvnVec<4, T>& v1, const LvnVec<4, T>& v2)
{
    return LvnVec<4, T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}
template <typename T>
LvnVec<4, T> operator/(const LvnVec<4, T>& v1, const LvnVec<4, T>& v2)
{
    return LvnVec<4, T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
}
template <typename T>
LvnVec<4, T> operator+(const T& s, const LvnVec<4, T>& v)
{
    return LvnVec<4, T>(s + v.x, s + v.y, s + v.z, s + v.w);
}
template <typename T>
LvnVec<4, T> operator-(const T& s, const LvnVec<4, T>& v)
{
    return LvnVec<4, T>(s - v.x, s - v.y, s - v.z, s - v.w);
}
template <typename T>
LvnVec<4, T> operator*(const T& s, const LvnVec<4, T>& v)
{
    return LvnVec<4, T>(s * v.x, s * v.y, s * v.z, s * v.w);
}
template <typename T>
LvnVec<4, T> operator/(const T& s, const LvnVec<4, T>& v)
{
    return LvnVec<4, T>(s / v.x, s / v.y, s / v.z, s / v.w);
}
template <typename T>
LvnVec<4, T> operator+(const LvnVec<4, T>& v, const T& s)
{
    return LvnVec<4, T>(v.x + s, v.y + s, v.z + s, v.w + s);
}
template <typename T>
LvnVec<4, T> operator-(const LvnVec<4, T>& v, const T& s)
{
    return LvnVec<4, T>(v.x - s, v.y - s, v.z - s, v.w - s);
}
template <typename T>
LvnVec<4, T> operator*(const LvnVec<4, T>& v, const T& s)
{
    return LvnVec<4, T>(v.x * s, v.y * s, v.z * s, v.w * s);
}
template <typename T>
LvnVec<4, T> operator/(const LvnVec<4, T>& v, const T& s)
{
    return LvnVec<4, T>(v.x / s, v.y / s, v.z / s, v.w / s);
}


// -- [SUBSECT]: Matrix Implementation
// ------------------------------------------------------------

template<typename T>
struct LvnMat<2, 2, T>
{
    LvnVec<2, T> value[2];

    static length_t length() { return 2; }

    LvnMat() = default;
    LvnMat(const LvnMat<2, 2, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0 };
        this->value[1] = { 0, n };
    }
    LvnMat(const T& x0, const T& y0, const T& x1, const T& y1)
    {
        this->value[0] = { x0, y0 };
        this->value[1] = { x1, y1 };
    }
    LvnMat(const LvnVec<2, T>& v0, const LvnVec<2, T>& v1)
        : value{ v0, v1 } {}
    LvnMat(const LvnMat<3, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]) } {}
    LvnMat(const LvnMat<4, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]) } {}
    LvnMat(const LvnMat<2, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]) } {}
    LvnMat(const LvnMat<2, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]) } {}
    LvnMat(const LvnMat<3, 2, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]) } {}
    LvnMat(const LvnMat<3, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]) } {}
    LvnMat(const LvnMat<4, 2, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]) } {}
    LvnMat(const LvnMat<4, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]) } {}

    LvnVec<2, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<2, T>& operator[](length_t i) const
    {
        return this->value[i];
    }

    LvnMat<2, 2, T> operator+() const
    {
        return LvnMat<2, 2, T>(
            this->value[0],
            this->value[1]);
    }
    LvnMat<2, 2, T> operator-() const
    {
        return LvnMat<2, 2, T>(
            -this->value[0],
            -this->value[1]);
    }
    LvnMat<2, 2, T> operator*=(const T& s)
    {
        this->value[0] *= s;
        this->value[1] *= s;
        return *this;
    }
    LvnMat<2, 2, T> operator/=(const T& s)
    {
        this->value[0] /= s;
        this->value[1] /= s;
        return *this;
    }
    LvnMat<2, 2, T> operator+(const LvnMat<2, 2, T>& m)
    {
        return LvnMat<2, 2, T>(
            this->value[0] + m[0],
            this->value[1] + m[1]);
    }
    LvnMat<2, 2, T> operator-(const LvnMat<2, 2, T>& m)
    {
        return LvnMat<2, 2, T>(
            this->value[0] - m[0],
            this->value[1] - m[1]);
    }
};

template<typename T>
LvnMat<2, 2, T> operator*(const LvnMat<2, 2, T>& m, const T& s)
{
    return LvnMat<2, 2, T>(
        m[0] * s,
        m[1] * s);
}
template<typename T>
LvnMat<2, 2, T> operator/(const LvnMat<2, 2, T>& m, const T& s)
{
    return LvnMat<2, 2, T>(
        m[0] / s,
        m[1] / s);
}
template<typename T>
LvnMat<2, 2, T> operator*(const T& s, const LvnMat<2, 2, T>& m)
{
    return LvnMat<2, 2, T>(
        s * m[0],
        s * m[1]);
}
template<typename T>
LvnMat<2, 2, T> operator/(const T& s, const LvnMat<2, 2, T>& m)
{
    return LvnMat<2, 2, T>(
        s / m[0],
        s / m[1]);
}
template<typename T>
LvnMat<2, 2, T> operator*(const LvnMat<2, 2, T>& m1, const LvnMat<2, 2, T>& m2)
{
    return LvnMat<2, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1]);
}
template<typename T>
LvnMat<3, 2, T> operator*(const LvnMat<2, 2, T>& m1, const LvnMat<3, 2, T>& m2)
{
    return LvnMat<3, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1]);
}
template<typename T>
LvnMat<4, 2, T> operator*(const LvnMat<2, 2, T>& m1, const LvnMat<4, 2, T>& m2)
{
    return LvnMat<4, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1]);
}
template<typename T>
LvnVec<2, T> operator*(const LvnMat<2, 2, T>& m, const LvnVec<2, T>& v)
{
    return LvnVec<2, T>(
        m[0][0] * v.x + m[1][0] * v.y,
        m[0][1] * v.x + m[1][1] * v.y);
}
template<typename T>
LvnVec<2, T> operator*(const LvnVec<2, T>& v, const LvnMat<2, 2, T>& m)
{
    return LvnVec<2, T>(
        v.x * m[0][0] + v.y * m[0][1],
        v.x * m[1][0] + v.y * m[1][1]);
}


template<typename T>
struct LvnMat<3, 3, T>
{
    LvnVec<3, T> value[3];

    static length_t length() { return 3; }

    LvnMat() = default;
    LvnMat(const LvnMat<3, 3, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0, 0 };
        this->value[1] = { 0, n, 0 };
        this->value[2] = { 0, 0, n };
    }
    LvnMat(const T& x0, const T& y0, const T& z0,
           const T& x1, const T& y1, const T& z1,
           const T& x2, const T& y2, const T& z2)
    {
        this->value[0] = { x0, y0, z0 };
        this->value[1] = { x1, y1, z1 };
        this->value[2] = { x2, y2, z2 };
    }

    LvnMat(const LvnVec<3, T>& v0, const LvnVec<3, T>& v1, const LvnVec<3, T>& v2)
        : value{ v0, v1, v2 } {}
    LvnMat(const LvnMat<2, 2, T>& m)
        : value{ LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0), LvnVec<3, T>(0, 0, 1) } {}
    LvnMat(const LvnMat<4, 4, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(m[2]) } {}
    LvnMat(const LvnMat<2, 3, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(0, 0, 1) } {}
    LvnMat(const LvnMat<2, 4, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(0, 0, 1) } {}
    LvnMat(const LvnMat<3, 2, T>& m)
        : value{ LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0), LvnVec<3, T>(m[2], 1) } {}
    LvnMat(const LvnMat<3, 4, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(m[2]) } {}
    LvnMat(const LvnMat<4, 2, T>& m)
        : value{ LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0), LvnVec<3, T>(m[2], 1) } {}
    LvnMat(const LvnMat<4, 3, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(m[2]) } {}

    LvnVec<3, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<3, T>& operator[](length_t i) const
    {
        return this->value[i];
    }

    LvnMat<3, 3, T> operator+() const
    {
        return LvnMat<3, 3, T>(
            this->value[0],
            this->value[1],
            this->value[2]);
    }
    LvnMat<3, 3, T> operator-() const
    {
        return LvnMat<3, 3, T>(
            -this->value[0],
            -this->value[1],
            -this->value[2]);
    }
    LvnMat<3, 3, T> operator*=(const T& s)
    {
        this->value[0] *= s;
        this->value[1] *= s;
        this->value[2] *= s;
        return *this;
    }
    LvnMat<3, 3, T> operator/=(const T& s)
    {
        this->value[0] /= s;
        this->value[1] /= s;
        this->value[2] /= s;
        return *this;
    }
    LvnMat<3, 3, T> operator+(const LvnMat<3, 3, T>& m)
    {
        return LvnMat<3, 3, T>(
            this->value[0] + m[0],
            this->value[1] + m[1],
            this->value[2] + m[2]);
    }
    LvnMat<3, 3, T> operator-(const LvnMat<3, 3, T>& m)
    {
        return LvnMat<3, 3, T>(
            this->value[0] - m[0],
            this->value[1] - m[1],
            this->value[2] - m[2]);
    }
};

template<typename T>
LvnMat<3, 3, T> operator*(LvnMat<3, 3, T>& m, const T& s)
{
    return LvnMat<3, 3, T>(
        m[0] * s,
        m[1] * s,
        m[2] * s);
}
template<typename T>
LvnMat<3, 3, T> operator/(LvnMat<3, 3, T>& m, const T& s)
{
    return LvnMat<3, 3, T>(
        m[0] / s,
        m[1] / s,
        m[2] / s);
}
template<typename T>
LvnMat<3, 3, T> operator*(const T& s, LvnMat<3, 3, T>& m)
{
    return LvnMat<3, 3, T>(
        s * m[0],
        s * m[1],
        s * m[2]);
}
template<typename T>
LvnMat<3, 3, T> operator/(const T& s, LvnMat<3, 3, T>& m)
{
    return LvnMat<3, 3, T>(
        s / m[0],
        s / m[1],
        s / m[2]);
}
template<typename T>
LvnMat<3, 3, T> operator*(const LvnMat<3, 3, T>& m1, const LvnMat<3, 3, T>& m2)
{
    return LvnMat<3, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2]);
}
template<typename T>
LvnMat<2, 3, T> operator*(const LvnMat<3, 3, T>& m1, const LvnMat<2, 3, T>& m2)
{
    return LvnMat<2, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2]);
}
template<typename T>
LvnMat<4, 3, T> operator*(const LvnMat<3, 3, T>& m1, const LvnMat<4, 3, T>& m2)
{
    return LvnMat<4, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2],
        m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1] + m1[2][2] * m2[3][2]);
}
template<typename T>
LvnVec<3, T> operator*(const LvnMat<3, 3, T>& m, const LvnVec<3, T>& v)
{
    return LvnVec<3, T>(
        m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
        m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
        m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z);
}
template<typename T>
LvnVec<3, T> operator*(const LvnVec<3, T>& v, const LvnMat<3, 3, T>& m)
{
    return LvnVec<3, T>(
        v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2],
        v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2],
        v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2]);
}


template<typename T>
struct LvnMat<4, 4, T>
{
    LvnVec<4, T> value[4];

    static length_t length() { return 4; }

    LvnMat() = default;
    LvnMat(const LvnMat<4, 4, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0, 0, 0 };
        this->value[1] = { 0, n, 0, 0 };
        this->value[2] = { 0, 0, n, 0 };
        this->value[3] = { 0, 0, 0, n };
    }
    LvnMat(const T& x0, const T& y0, const T& z0, const T& w0,
           const T& x1, const T& y1, const T& z1, const T& w1,
           const T& x2, const T& y2, const T& z2, const T& w2,
           const T& x3, const T& y3, const T& z3, const T& w3)
    {
        this->value[0] = { x0, y0, z0, w0 };
        this->value[1] = { x1, y1, z1, w1 };
        this->value[2] = { x2, y2, z2, w2 };
        this->value[3] = { x3, y3, z3, w3 };
    }

    LvnMat(const LvnVec<4, T>& v0, const LvnVec<4, T>& v1, const LvnVec<4, T>& v2, const LvnVec<4, T>& v3)
        : value{ v0, v1, v2, v3 } {}
    LvnMat(const LvnMat<2, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0), LvnVec<4, T>(0, 0, 1, 0), LvnVec<4, T>(0, 0, 0, 1) } {}
    LvnMat(const LvnMat<3, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0), LvnVec<4, T>(m[2], 0), LvnVec<4, T>(0, 0, 0, 1) } {}
    LvnMat(const LvnMat<2, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0), LvnVec<4, T>(0, 0, 1, 0), LvnVec<4, T>(0, 0, 0, 1) } {}
    LvnMat(const LvnMat<2, 4, T>& m)
        : value{ LvnVec<4, T>(m[0]), LvnVec<4, T>(m[1]), LvnVec<4, T>(0, 0, 1, 0 ), LvnVec<4, T>(0, 0, 0, 1) } {}
    LvnMat(const LvnMat<3, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0), LvnVec<4, T>(m[2], 1, 0), LvnVec<4, T>(0, 0, 0, 1) } {}
    LvnMat(const LvnMat<3, 4, T>& m)
        : value{ LvnVec<4, T>(m[0]), LvnVec<4, T>(m[1]), LvnVec<4, T>(m[2]), LvnVec<4, T>(0, 0, 0, 1) } {}
    LvnMat(const LvnMat<4, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0), LvnVec<4, T>(m[2], 1, 0), LvnVec<4, T>(m[3], 0, 1) } {}
    LvnMat(const LvnMat<4, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0), LvnVec<4, T>(m[2], 0), LvnVec<4, T>(m[3], 1) } {}

    LvnVec<4, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<4, T>& operator[](length_t i) const
    {
        return this->value[i];
    }

    LvnMat<4, 4, T> operator+() const
    {
        return LvnMat<4, 4, T>(
            this->value[0],
            this->value[1],
            this->value[2],
            this->value[3]);
    }
    LvnMat<4, 4, T> operator-() const
    {
        return LvnMat<4, 4, T>(
            -this->value[0],
            -this->value[1],
            -this->value[2],
            -this->value[3]);
    }
    LvnMat<4, 4, T> operator+(const LvnMat<4, 4, T>& m)
    {
        return LvnMat<4, 4, T>(
            this->value[0] + m[0],
            this->value[1] + m[1],
            this->value[4] + m[2],
            this->value[3] + m[3]);
    }
    LvnMat<4, 4, T> operator-(const LvnMat<4, 4, T>& m)
    {
        return LvnMat<4, 4, T>(
            this->value[0] - m[0],
            this->value[1] - m[1],
            this->value[2] - m[2],
            this->value[3] - m[3]);
    }
    LvnMat<4, 4, T> operator*=(const T& s)
    {
        this->value[0] *= s;
        this->value[1] *= s;
        this->value[2] *= s;
        this->value[3] *= s;
        return *this;
    }
    LvnMat<4, 4, T> operator/=(const T& s)
    {
        this->value[0] /= s;
        this->value[1] /= s;
        this->value[2] /= s;
        this->value[3] /= s;
        return *this;
    }
};

template<typename T>
LvnMat<4, 4, T> operator*(const LvnMat<4, 4, T>& m, const T& s)
{
    return LvnMat<4, 4, T>(
        m[0] * s,
        m[1] * s,
        m[2] * s,
        m[3] * s);
}
template<typename T>
LvnMat<4, 4, T> operator/(const LvnMat<4, 4, T>& m, const T& s)
{
    return LvnMat<4, 4, T>(
        m[0] / s,
        m[1] / s,
        m[2] / s,
        m[3] / s);
}
template<typename T>
LvnMat<4, 4, T> operator*(const T& s, const LvnMat<4, 4, T>& m)
{
    return LvnMat<4, 4, T>(
        s * m[0],
        s * m[1],
        s * m[2],
        s * m[3]);
}
template<typename T>
LvnMat<4, 4, T> operator/(const T& s, const LvnMat<4, 4, T>& m)
{
    return LvnMat<4, 4, T>(
        s / m[0],
        s / m[1],
        s / m[2],
        s / m[3]);
}
template<typename T>
LvnMat<4, 4, T> operator*(const LvnMat<4, 4, T>& m1, const LvnMat<4, 4, T>& m2)
{
    return LvnMat<4, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3],
        m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1] + m1[2][3] * m2[0][2] + m1[3][3] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3],
        m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1] + m1[2][3] * m2[1][2] + m1[3][3] * m2[1][3],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2] + m1[3][2] * m2[2][3],
        m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1] + m1[2][3] * m2[2][2] + m1[3][3] * m2[2][3],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2] + m1[3][0] * m2[3][3],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2] + m1[3][1] * m2[3][3],
        m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1] + m1[2][2] * m2[3][2] + m1[3][2] * m2[3][3],
        m1[0][3] * m2[3][0] + m1[1][3] * m2[3][1] + m1[2][3] * m2[3][2] + m1[3][3] * m2[3][3]);
}
template<typename T>
LvnMat<2, 4, T> operator*(const LvnMat<4, 4, T>& m1, const LvnMat<2, 4, T>& m2)
{
    return LvnMat<2, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3],
        m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1] + m1[2][3] * m2[0][2] + m1[3][3] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3],
        m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1] + m1[2][3] * m2[1][2] + m1[3][3] * m2[1][3]);
}
template<typename T>
LvnMat<3, 4, T> operator*(const LvnMat<4, 4, T>& m1, const LvnMat<3, 4, T>& m2)
{
    return LvnMat<3, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3],
        m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1] + m1[2][3] * m2[0][2] + m1[3][3] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3],
        m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1] + m1[2][3] * m2[1][2] + m1[3][3] * m2[1][3],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2] + m1[3][2] * m2[2][3],
        m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1] + m1[2][3] * m2[2][2] + m1[3][3] * m2[2][3]);
}
template<typename T>
LvnVec<4, T> operator*(const LvnMat<4, 4, T>& m, const LvnVec<4, T>& v)
{
    return LvnVec<4, T>(
        m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
        m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
        m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w,
        m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w);
}
template<typename T>
LvnVec<4, T> operator*(const LvnVec<4, T>& v, const LvnMat<4, 4, T>& m)
{
    return LvnVec<4, T>(
        v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + v.w * m[0][3],
        v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + v.w * m[1][3],
        v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + v.w * m[2][3],
        v.x * m[3][0] + v.y * m[3][1] + v.z * m[3][2] + v.w * m[3][3]);
}


template<typename T>
struct LvnMat<2, 3, T>
{
    LvnVec<3, T> value[2];

    static length_t length() { return 2; }

    LvnMat() = default;
    LvnMat(const LvnMat<2, 3, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0, 0 };
        this->value[1] = { 0, n, 0 };
    }
    LvnMat(const T& x0, const T& y0, const T& z0,
           const T& x1, const T& y1, const T& z1)
    {
        this->value[0] = { x0, y0, z0 };
        this->value[1] = { x1, y1, z1 };
    }
    LvnMat(const LvnVec<3, T>& v0, const LvnVec<3, T>& v1)
        : value{  v0, v1  } {}
    LvnMat(const LvnMat<2, 2, T>& m)
        : value{  LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0)  } {}
    LvnMat(const LvnMat<3, 3, T>& m)
        : value{  LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1])  } {}
    LvnMat(const LvnMat<4, 4, T>& m)
        : value{  LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1])  } {}
    LvnMat(const LvnMat<2, 4, T>& m)
        : value{  LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1])  } {}
    LvnMat(const LvnMat<3, 2, T>& m)
        : value{  LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0)  } {}
    LvnMat(const LvnMat<3, 4, T>& m)
        : value{  LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1])  } {}
    LvnMat(const LvnMat<4, 2, T>& m)
        : value{  LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0)  } {}
    LvnMat(const LvnMat<4, 3, T>& m)
        : value{  LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1])  } {}

    LvnVec<3, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<3, T>& operator[](int i) const
    {
        return this->value[i];
    }

    LvnMat<2, 3, T> operator+() const
    {
        return LvnMat<2, 3, T>(
            this->value[0],
            this->value[1]);
    }
    LvnMat<2, 3, T> operator-() const
    {
        return LvnMat<2, 3, T>(
            -this->value[0],
            -this->value[1]);
    }
    LvnMat<2, 3, T> operator+(const LvnMat<2, 3, T>& m)
    {
        return LvnMat<2, 3, T>(
            this->value[0] + m[0],
            this->value[1] + m[1]);
    }
    LvnMat<2, 3, T> operator-(const LvnMat<2, 3, T>& m)
    {
        return LvnMat<2, 3, T>(
            this->value[0] - m[0],
            this->value[1] - m[1]);
    }
};

template<typename T>
LvnMat<2, 3, T> operator*(const LvnMat<2, 3, T>& m, const T& s)
{
    return LvnMat<2, 3, T>(
        m[0] * s,
        m[1] * s);
}
template<typename T>
LvnMat<2, 3, T> operator/(const LvnMat<2, 3, T>& m, const T& s)
{
    return LvnMat<2, 3, T>(
        m[0] / s,
        m[1] / s);
}
template<typename T>
LvnMat<2, 3, T> operator*(const T& s, const LvnMat<2, 3, T>& m)
{
    return LvnMat<2, 3, T>(
        s * m[0],
        s * m[1]);
}
template<typename T>
LvnMat<2, 3, T> operator/(const T& s, const LvnMat<2, 3, T>& m)
{
    return LvnMat<2, 3, T>(
        s / m[0],
        s / m[1]);
}
template<typename T>
LvnMat<2, 3, T> operator*(const LvnMat<2, 3, T>& m1, const LvnMat<2, 2, T>& m2)
{
    return LvnMat<2, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1]);
}
template<typename T>
LvnMat<3, 3, T> operator*(const LvnMat<2, 3, T>& m1, const LvnMat<3, 2, T>& m2)
{
    return LvnMat<3, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1]);
}
template<typename T>
LvnMat<4, 3, T> operator*(const LvnMat<2, 3, T>& m1, const LvnMat<4, 2, T>& m2)
{
    return LvnMat<4, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1],
        m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1]);
}
template<typename T>
LvnVec<3, T> operator*(const LvnMat<2, 3, T>& m, const LvnVec<2, T>& v)
{
    return LvnVec<3, T>(
        m[0][0] * v.x + m[1][0] * v.y,
        m[0][1] * v.x + m[1][1] * v.y,
        m[0][2] * v.x + m[1][2] * v.y);
}
template<typename T>
LvnVec<2, T> operator*(const LvnVec<3, T>& v, const LvnMat<2, 3, T>& m)
{
    return LvnVec<2, T>(
        v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2],
        v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2]);
}

template<typename T>
struct LvnMat<2, 4, T>
{
    LvnVec<4, T> value[2];

    static length_t length() { return 2; }

    LvnMat() = default;
    LvnMat(const LvnMat<2, 4, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0, 0, 0 };
        this->value[1] = { 0, n, 0, 0 };
    }
    LvnMat
    (
        const T& x0, const T& y0, const T& z0, const T& w0,
        const T& x1, const T& y1, const T& z1, const T& w1
    )
    {
        this->value[0] = { x0, y0, z0, w0 };
        this->value[1] = { x1, y1, z1, w1 };
    }
    LvnMat(const LvnVec<4, T>& v0, const LvnVec<4, T>& v1)
        : value{ v0, v1 } {}
    LvnMat(const LvnMat<2, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0) } {}
    LvnMat(const LvnMat<3, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0) } {}
    LvnMat(const LvnMat<4, 4, T>& m)
        : value{ LvnVec<4, T>(m[0]), LvnVec<4, T>(m[1]) } {}
    LvnMat(const LvnMat<2, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0) } {}
    LvnMat(const LvnMat<3, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0) } {}
    LvnMat(const LvnMat<3, 4, T>& m)
        : value{ LvnVec<4, T>(m[0]), LvnVec<4, T>(m[1]) } {}
    LvnMat(const LvnMat<4, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0) } {}
    LvnMat(const LvnMat<4, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0) } {}

    LvnVec<4, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<4, T>& operator[](length_t i) const
    {
        return this->value[i];
    }

    LvnMat<2, 4, T> operator+() const
    {
        return LvnMat<2, 4, T>(
            this->value[0],
            this->value[1]);
    }
    LvnMat<2, 4, T> operator-() const
    {
        return LvnMat<2, 4, T>(
            -this->value[0],
            -this->value[1]);
    }
    LvnMat<2, 4, T> operator+(const LvnMat<2, 4, T>& m)
    {
        return LvnMat<2, 4, T>(
            this->value[0] + m[0],
            this->value[1] + m[1]);
    }
    LvnMat<2, 4, T> operator-(const LvnMat<2, 4, T>& m)
    {
        return LvnMat<2, 4, T>(
            this->value[0] - m[0],
            this->value[1] - m[1]);
    }
};

template<typename T>
LvnMat<2, 4, T> operator*(const LvnMat<2, 4, T>& m, const T& s)
{
    return LvnMat<2, 4, T>(
        m[0] * s,
        m[1] * s);
}
template<typename T>
LvnMat<2, 4, T> operator/(const LvnMat<2, 4, T>& m, const T& s)
{
    return LvnMat<2, 4, T>(
        m[0] / s,
        m[1] / s);
}
template<typename T>
LvnMat<2, 4, T> operator*(const T& s, const LvnMat<2, 4, T>& m)
{
    return LvnMat<2, 4, T>(
        s * m[0],
        s * m[1]);
}
template<typename T>
LvnMat<2, 4, T> operator/(const T& s, const LvnMat<2, 4, T>& m)
{
    return LvnMat<2, 4, T>(
        s / m[0],
        s / m[1]);
}
template<typename T>
LvnMat<4, 4, T> operator*(const LvnMat<2, 4, T>& m1, const LvnMat<4, 2, T>& m2)
{
    return LvnMat<4, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1],
        m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1],
        m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1],
        m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1],
        m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1],
        m1[0][3] * m2[3][0] + m1[1][3] * m2[3][1]);
}
template<typename T>
LvnMat<2, 4, T> operator*(const LvnMat<2, 4, T>& m1, const LvnMat<2, 2, T>& m2)
{
    return LvnMat<2, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1],
        m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1],
        m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1]);
}
template<typename T>
LvnMat<3, 4, T> operator*(const LvnMat<2, 4, T>& m1, const LvnMat<3, 2, T>& m2)
{
    return LvnMat<3, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1],
        m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1],
        m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1],
        m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1]);
}
template<typename T>
LvnVec<4, T> operator*(const LvnMat<2, 4, T>& m, const LvnVec<2, T>& v)
{
    return LvnVec<4, T>(
        m[0][0] * v.x + m[1][0] * v.y,
        m[0][1] * v.x + m[1][1] * v.y,
        m[0][2] * v.x + m[1][2] * v.y,
        m[0][3] * v.x + m[1][3] * v.y);
}
template<typename T>
LvnVec<2, T> operator*(const LvnVec<4, T>& v, const LvnMat<2, 4, T>& m)
{
    return LvnVec<2, T>(
        v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + v.w * m[0][3],
        v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + v.w * m[1][3]);
}


template<typename T>
struct LvnMat<3, 2, T>
{
    LvnVec<2, T> value[3];

    static length_t length() { return 3; }

    LvnMat() = default;
    LvnMat(const LvnMat<3, 2, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0 };
        this->value[1] = { 0, n };
        this->value[2] = { 0, 0 };
    }
    LvnMat
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
    LvnMat(const LvnVec<2, T>& v0, const LvnVec<2, T>& v1, const LvnVec<2, T>& v2)
        : value{ v0, v1, v2 } {}
    LvnMat(const LvnMat<2, 2, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<3, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]) } {}
    LvnMat(const LvnMat<4, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]) } {}
    LvnMat(const LvnMat<2, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<2, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<3, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]) } {}
    LvnMat(const LvnMat<4, 2, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]) } {}
    LvnMat(const LvnMat<4, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]) } {}

    LvnVec<2, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<2, T>& operator[](length_t i) const
    {
        return this->value[i];
    }

    LvnMat<3, 2, T> operator+() const
    {
        return LvnMat<3, 2, T>(
            this->value[0],
            this->value[1],
            this->value[2]);
    }
    LvnMat<3, 2, T> operator-() const
    {
        return LvnMat<3, 2, T>(
            -this->value[0],
            -this->value[1],
            -this->value[2]);
    }
    LvnMat<3, 2, T> operator+(const LvnMat<3, 2, T>& m)
    {
        return LvnMat<3, 2, T>(
            this->value[0] + m[0],
            this->value[1] + m[1],
            this->value[2] + m[2]);
    }
    LvnMat<3, 2, T> operator-(const LvnMat<3, 2, T>& m)
    {
        return LvnMat<3, 2, T>(
            this->value[0] - m[0],
            this->value[1] - m[1],
            this->value[2] - m[2]);
    }
};

template<typename T>
LvnMat<3, 2, T> operator*(const LvnMat<3, 2, T>& m, const T& s)
{
    return LvnMat<3, 2, T>(
        m[0] * s,
        m[1] * s,
        m[2] * s);
}
template<typename T>
LvnMat<3, 2, T> operator/(const LvnMat<3, 2, T>& m, const T& s)
{
    return LvnMat<3, 2, T>(
        m[0] / s,
        m[1] / s,
        m[2] / s);
}
template<typename T>
LvnMat<3, 2, T> operator*(const T& s, const LvnMat<3, 2, T>& m)
{
    return LvnMat<3, 2, T>(
        s * m[0],
        s * m[1],
        s * m[2]);
}
template<typename T>
LvnMat<3, 2, T> operator/(const T& s, const LvnMat<3, 2, T>& m)
{
    return LvnMat<3, 2, T>(
        s / m[0],
        s / m[1],
        s / m[2]);
}
template<typename T>
LvnMat<3, 2, T> operator*(const LvnMat<3, 2, T>& m1, const LvnMat<3, 3, T>& m2)
{
    return LvnMat<3, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2]);
}
template<typename T>
LvnMat<4, 2, T> operator*(const LvnMat<3, 2, T>& m1, const LvnMat<4, 3, T>& m2)
{
    return LvnMat<4, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2]);
}
template<typename T>
LvnMat<2, 2, T> operator*(const LvnMat<3, 2, T>& m1, const LvnMat<2, 3, T>& m2)
{
    return LvnMat<2, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2]);
}
template<typename T>
LvnVec<2, T> operator*(const LvnMat<3, 2, T>& m, const LvnVec<3, T>& v)
{
    return LvnVec<2, T>(
        m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
        m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z);
}
template<typename T>
LvnVec<3, T> operator*(const LvnVec<2, T>& v, const LvnMat<3, 2, T>& m)
{
    return LvnVec<3, T>(
        v.x * m[0][0] + v.y * m[0][1],
        v.x * m[1][0] + v.y * m[1][1],
        v.x * m[2][0] + v.y * m[2][1]);
}


template<typename T>
struct LvnMat<3, 4, T>
{
    LvnVec<4, T> value[3];

    static length_t length() { return 3; }

    LvnMat() = default;
    LvnMat(const LvnMat<3, 4, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0, 0, 0 };
        this->value[1] = { 0, n, 0, 0 };
        this->value[2] = { 0, 0, n, 0 };
    }
    LvnMat
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
    LvnMat(const LvnVec<4, T>& v0, const LvnVec<4, T>& v1, const LvnVec<4, T>& v2)
        : value{ v0, v1, v2 } {}
    LvnMat(const LvnMat<2, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0), LvnVec<4, T>(0, 0, 1, 0) } {}
    LvnMat(const LvnMat<3, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0), LvnVec<4, T>(m[2], 0) } {}
    LvnMat(const LvnMat<4, 4, T>& m)
        : value{ LvnVec<4, T>(m[0]), LvnVec<4, T>(m[1]), LvnVec<4, T>(m[2]) } {}
    LvnMat(const LvnMat<2, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0), LvnVec<4, T>(0, 0, 1, 0) } {}
    LvnMat(const LvnMat<2, 4, T>& m)
        : value{ LvnVec<4, T>(m[0]), LvnVec<4, T>(m[1]), LvnVec<4, T>(0, 0, 1, 0) } {}
    LvnMat(const LvnMat<3, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0), LvnVec<4, T>(m[2], 1, 0) } {}
    LvnMat(const LvnMat<4, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0), LvnVec<4, T>(m[2], 1, 0) } {}
    LvnMat(const LvnMat<4, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0), LvnVec<4, T>(m[2], 0) } {}

    LvnVec<4, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<4, T>& operator[](length_t i) const
    {
        return this->value[i];
    }

    LvnMat<3, 4, T> operator+() const
    {
        return LvnMat<3, 4, T>(
            this->value[0],
            this->value[1],
            this->value[2]);
    }
    LvnMat<3, 4, T> operator-() const
    {
        return LvnMat<3, 4, T>(
            -this->value[0],
            -this->value[1],
            -this->value[2]);
    }
    LvnMat<3, 4, T> operator+(const LvnMat<3, 4, T>& m)
    {
        return LvnMat<3, 4, T>(
            this->value[0] + m[0],
            this->value[1] + m[1],
            this->value[2] + m[2]);
    }
    LvnMat<3, 4, T> operator-(const LvnMat<3, 4, T>& m)
    {
        return LvnMat<3, 4, T>(
            this->value[0] - m[0],
            this->value[1] - m[1],
            this->value[2] - m[2]);
    }
};

template<typename T>
LvnMat<3, 4, T> operator*(const LvnMat<3, 4, T>& m, const T& s)
{
    return LvnMat<3, 4, T>(
        m[0] * s,
        m[1] * s,
        m[2] * s);
}
template<typename T>
LvnMat<3, 4, T> operator/(const LvnMat<3, 4, T>& m, const T& s)
{
    return LvnMat<3, 4, T>(
        m[0] / s,
        m[1] / s,
        m[2] / s);
}
template<typename T>
LvnMat<3, 4, T> operator*(const T& s, const LvnMat<3, 4, T>& m)
{
    return LvnMat<3, 4, T>(
        s * m[0],
        s * m[1],
        s * m[2]);
}
template<typename T>
LvnMat<3, 4, T> operator/(const T& s, const LvnMat<3, 4, T>& m)
{
    return LvnMat<3, 4, T>(
        s / m[0],
        s / m[1],
        s / m[2]);
}
template<typename T>
LvnMat<4, 4, T> operator*(const LvnMat<3, 4, T>& m1, const LvnMat<4, 3, T>& m2)
{
    return LvnMat<4, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2],
        m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1] + m1[2][3] * m2[0][2],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2],
        m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1] + m1[2][3] * m2[1][2],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2],
        m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1] + m1[2][3] * m2[2][2],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2],
        m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1] + m1[2][2] * m2[3][2],
        m1[0][3] * m2[3][0] + m1[1][3] * m2[3][1] + m1[2][3] * m2[3][2]);
}
template<typename T>
LvnMat<2, 4, T> operator*(const LvnMat<3, 4, T>& m1, const LvnMat<2, 3, T>& m2)
{
    return LvnMat<2, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][0] + m1[2][0] * m2[0][0],
        m1[0][1] * m2[0][1] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][1],
        m1[0][2] * m2[0][2] + m1[1][2] * m2[0][2] + m1[2][2] * m2[0][2],
        m1[0][3] * m2[0][3] + m1[1][3] * m2[0][3] + m1[2][3] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][0] + m1[2][0] * m2[1][0],
        m1[0][1] * m2[1][1] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][1],
        m1[0][2] * m2[1][2] + m1[1][2] * m2[1][2] + m1[2][2] * m2[1][2],
        m1[0][3] * m2[1][3] + m1[1][3] * m2[1][3] + m1[2][3] * m2[1][3]);
}
template<typename T>
LvnMat<3, 4, T> operator*(const LvnMat<3, 4, T>& m1, const LvnMat<3, 3, T>& m2)
{
    return LvnMat<3, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2],
        m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1] + m1[2][3] * m2[0][2],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2],
        m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1] + m1[2][3] * m2[1][2],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2],
        m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1] + m1[2][3] * m2[2][2]);
}
template<typename T>
LvnVec<4, T> operator*(const LvnMat<3, 4, T>& m, const LvnVec<3, T>& v)
{
    return LvnVec<3, T>(
        m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
        m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
        m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z,
        m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z);
}
template<typename T>
LvnVec<3, T> operator*(const LvnVec<4, T>& v, const LvnMat<3, 4, T>& m)
{
    return LvnVec<3, T>(
        v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + v.w * m[0][3],
        v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + v.w * m[1][3],
        v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + v.w * m[2][3]);
}


template<typename T>
struct LvnMat<4, 2, T>
{
    LvnVec<2, T> value[4];

    static length_t length() { return 4; }

    LvnMat() = default;
    LvnMat(const LvnMat<4, 2, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0 };
        this->value[1] = { 0, n };
        this->value[2] = { 0, 0 };
        this->value[3] = { 0, 0 };
    }
    LvnMat
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
    LvnMat(const LvnVec<2, T>& v0, const LvnVec<2, T>& v1, const LvnVec<2, T>& v2, const LvnVec<2, T>& v3)
        : value{ v0, v1, v2, v3 } {}
    LvnMat(const LvnMat<2, 2, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(0, 0), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<3, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<4, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]), LvnVec<2, T>(m[3]) } {}
    LvnMat(const LvnMat<2, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(0, 0), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<2, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(0, 0), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<3, 2, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<3, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<4, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]), LvnVec<2, T>(m[3]) } {}

    LvnVec<2, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<2, T>& operator[](length_t i) const
    {
        return this->value[i];
    }

    LvnMat<4, 2, T> operator+() const
    {
        return LvnMat<4, 2, T>(
            this->value[0],
            this->value[1],
            this->value[2],
            this->value[3]);
    }
    LvnMat<4, 2, T> operator-() const
    {
        return LvnMat<4, 2, T>(
            -this->value[0],
            -this->value[1],
            -this->value[2],
            -this->value[3]);
    }
    LvnMat<4, 2, T> operator+(const LvnMat<4, 2, T>& m)
    {
        return LvnMat<4, 2, T>(
            this->value[0] + m[0],
            this->value[1] + m[1],
            this->value[2] + m[2],
            this->value[3] + m[3]);
    }
    LvnMat<4, 2, T> operator-(const LvnMat<4, 2, T>& m)
    {
        return LvnMat<4, 2, T>(
            this->value[0] - m[0],
            this->value[1] - m[1],
            this->value[2] - m[2],
            this->value[3] - m[3]);
    }
};

template<typename T>
LvnMat<4, 2, T> operator*(const LvnMat<4, 2, T>& m, const T& s)
{
    return LvnMat<4, 2, T>(
        m[0] * s,
        m[1] * s,
        m[2] * s,
        m[3] * s);
}
template<typename T>
LvnMat<4, 2, T> operator/(const LvnMat<4, 2, T>& m, const T& s)
{
    return LvnMat<4, 2, T>(
        m[0] / s,
        m[1] / s,
        m[2] / s,
        m[3] / s);
}
template<typename T>
LvnMat<4, 2, T> operator*(const T& s, const LvnMat<4, 2, T>& m)
{
    return LvnMat<4, 2, T>(
        s * m[0],
        s * m[1],
        s * m[2],
        s * m[3]);
}
template<typename T>
LvnMat<4, 2, T> operator/(const T& s, const LvnMat<4, 2, T>& m)
{
    return LvnMat<4, 2, T>(
        s / m[0],
        s / m[1],
        s / m[2],
        s / m[3]);
}
template<typename T>
LvnMat<2, 2, T> operator*(const LvnMat<4, 2, T>& m1, const LvnMat<2, 4, T>& m2)
{
    return LvnMat<2, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3]);
}
template<typename T>
LvnMat<3, 2, T> operator*(const LvnMat<4, 2, T>& m1, const LvnMat<3, 4, T>& m2)
{
    return LvnMat<3, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3]);
}
template<typename T>
LvnMat<4, 2, T> operator*(const LvnMat<4, 2, T>& m1, const LvnMat<2, 2, T>& m2)
{
    return LvnMat<4, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2] + m1[3][0] * m2[3][3],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2] + m1[3][1] * m2[3][3]);
}
template<typename T>
LvnVec<2, T> operator*(const LvnMat<4, 2, T>& m, const LvnVec<4, T>& v)
{
    return LvnVec<2, T>(
        m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
        m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w);
}
template<typename T>
LvnVec<4, T> operator*(const LvnVec<2, T>& v, const LvnMat<4, 2, T>& m)
{
    return LvnVec<4, T>(
        v.x * m[0][0] + v.y * m[0][1],
        v.x * m[1][0] + v.y * m[1][1],
        v.x * m[2][0] + v.y * m[2][1],
        v.x * m[3][0] + v.y * m[3][1]);
}


template<typename T>
struct LvnMat<4, 3, T>
{
    LvnVec<3, T> value[4];

    static length_t length() { return 4; }

    LvnMat() = default;
    LvnMat(const LvnMat<4, 3, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0, 0 };
        this->value[1] = { 0, n, 0 };
        this->value[2] = { 0, 0, n };
        this->value[3] = { 0, 0, 0 };
    }
    LvnMat
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
    LvnMat(const LvnVec<3, T>& v0, const LvnVec<3, T>& v1, const LvnVec<3, T>& v2, const LvnVec<3, T>& v3)
        : value{ v0, v1, v2, v3 } {}
    LvnMat(const LvnMat<2, 2, T>& m)
        : value{ LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0), LvnVec<3, T>(0, 0, 1), LvnVec<3, T>(0, 0, 0) } {}
    LvnMat(const LvnMat<3, 3, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(m[2]), LvnVec<3, T>(0, 0, 0) } {}
    LvnMat(const LvnMat<4, 4, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(m[2]), LvnVec<3, T>(m[3]) } {}
    LvnMat(const LvnMat<2, 3, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(0, 0, 1), LvnVec<3, T>(0, 0, 0) } {}
    LvnMat(const LvnMat<2, 4, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(0, 0, 1), LvnVec<3, T>(0, 0, 0) } {}
    LvnMat(const LvnMat<3, 2, T>& m)
        : value{ LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0), LvnVec<3, T>(m[2], 1), LvnVec<3, T>(0, 0, 0) } {}
    LvnMat(const LvnMat<3, 4, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(m[2]), LvnVec<3, T>(0, 0, 0) } {}
    LvnMat(const LvnMat<4, 2, T>& m)
        : value{ LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0), LvnVec<3, T>(m[2], 1), LvnVec<3, T>(m[3], 0) } {}

    LvnVec<3, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<3, T>& operator[](length_t i) const
    {
        return this->value[i];
    }

    LvnMat<4, 3, T> operator+() const
    {
        return LvnMat<4, 3, T>(
            this->value[0],
            this->value[1],
            this->value[2],
            this->value[3]);
    }
    LvnMat<4, 3, T> operator-() const
    {
        return LvnMat<4, 3, T>(
            -this->value[0],
            -this->value[1],
            -this->value[2],
            -this->value[3]);
    }
    LvnMat<4, 3, T> operator+(const LvnMat<4, 3, T>& m)
    {
        return LvnMat<4, 3, T>(
            this->value[0] + m[0],
            this->value[1] + m[1],
            this->value[2] + m[2],
            this->value[3] + m[3]);
    }
    LvnMat<4, 3, T> operator-(const LvnMat<4, 3, T>& m)
    {
        return LvnMat<4, 3, T>(
            this->value[0] - m[0],
            this->value[1] - m[1],
            this->value[2] - m[2],
            this->value[3] - m[3]);
    }
};

template<typename T>
LvnMat<4, 3, T> operator*(const LvnMat<4, 3, T>& m, const T& s)
{
    return LvnMat<4, 3, T>(
        m[0] * s,
        m[1] * s,
        m[2] * s,
        m[3] * s);
}
template<typename T>
LvnMat<4, 3, T> operator/(const LvnMat<4, 3, T>& m, const T& s)
{
    return LvnMat<4, 3, T>(
        m[0] / s,
        m[1] / s,
        m[2] / s,
        m[3] / s);
}
template<typename T>
LvnMat<4, 3, T> operator*(const T& s, const LvnMat<4, 3, T>& m)
{
    return LvnMat<4, 3, T>(
        s * m[0],
        s * m[1],
        s * m[2],
        s * m[3]);
}
template<typename T>
LvnMat<4, 3, T> operator/(const T& s, const LvnMat<4, 3, T>& m)
{
    return LvnMat<4, 3, T>(
        s / m[0],
        s / m[1],
        s / m[2],
        s / m[3]);
}
template<typename T>
LvnMat<2, 3, T> operator*(const LvnMat<4, 3, T>& m1, const LvnMat<2, 4, T>& m2)
{
    return LvnMat<2, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3]);
}
template<typename T>
LvnMat<3, 3, T> operator*(const LvnMat<4, 3, T>& m1, const LvnMat<3, 4, T>& m2)
{
    return LvnMat<3, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2] + m1[3][2] * m2[2][3]);
}
template<typename T>
LvnMat<4, 3, T> operator*(const LvnMat<4, 3, T>& m1, const LvnMat<4, 4, T>& m2)
{
    return LvnMat<4, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2] + m1[3][2] * m2[2][3],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2] + m1[3][0] * m2[3][3],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2] + m1[3][1] * m2[3][3],
        m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1] + m1[2][2] * m2[3][2] + m1[3][2] * m2[3][3]);
}
template<typename T>
LvnVec<3, T> operator*(const LvnMat<4, 3, T>& m, const LvnVec<4, T>& v)
{
    return LvnVec<3, T>(
        m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
        m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
        m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w);
}
template<typename T>
LvnVec<4, T> operator*(const LvnVec<3, T>& v, const LvnMat<4, 3, T>& m)
{
    return LvnVec<4, T>(
        v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + v.w * m[0][3],
        v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + v.w * m[1][3],
        v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + v.w * m[2][3],
        v.x * m[3][0] + v.y * m[3][1] + v.z * m[3][2] + v.w * m[3][3]);
}

template<typename T>
struct LvnQuat_t
{
    union { T w, r; };
    union { T x, i; };
    union { T y, j; };
    union { T z, k; };

    static int length() { return 4; }

    LvnQuat_t() = default;
    LvnQuat_t(const LvnQuat_t<T>&) = default;

    LvnQuat_t(const T& nw, const T& nx, const T& ny, const T& nz)
        : w(nw), x(nx), y(ny), z(nz) {}

    T& operator[](int i)
    {
        LVN_CORE_ASSERT(i >= 0 && i < length(), "vector index out of range");

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
    const T& operator[](int i) const
    {
        LVN_CORE_ASSERT(i >= 0 && i < length(), "vector index out of range");

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

    LvnQuat_t<T>& operator++()
    {
        this->w++;
        this->x++;
        this->y++;
        this->z++;
        return *this;
    }
    LvnQuat_t<T>& operator--()
    {
        this->w--;
        this->x--;
        this->y--;
        this->z--;
        return *this;
    }
    LvnQuat_t<T> operator++(int)
    {
        LvnQuat_t<T> q(*this);
        ++*this;
        return q;
    }
    LvnQuat_t<T> operator--(int)
    {
        LvnQuat_t<T> q(*this);
        --*this;
        return q;
    }
    LvnQuat_t<T> operator+() const
    {
        return LvnQuat_t<T>(w, x, y, z);
    }
    LvnQuat_t<T> operator-() const
    {
        return LvnQuat_t<T>(-w, -x, -y, -z);
    }
    LvnQuat_t<T> operator+(const LvnQuat_t<T>& q)
    {
        return LvnQuat_t<T>(
            this->w + q.w,
            this->x + q.x,
            this->y + q.y,
            this->z + q.z);
    }
    LvnQuat_t<T> operator-(const LvnQuat_t<T>& q)
    {
        return LvnQuat_t<T>(
            this->w - q.w,
            this->x - q.x,
            this->y - q.y,
            this->z - q.z);
    }
    LvnQuat_t<T> operator*(const LvnQuat_t<T>& q)
    {
        return LvnQuat_t<T>(
            this->w * q.w - this->x * q.x - this->y * q.y - this->z * q.z,
            this->w * q.x + this->x * q.w + this->y * q.z - this->z * q.y,
            this->w * q.y + this->y * q.w + this->z * q.x - this->x * q.z,
            this->w * q.z + this->z * q.w + this->x * q.y - this->y * q.x);
    }
};

template <typename T>
LvnQuat_t<T> operator+(const LvnQuat_t<T>& q1, const LvnQuat_t<T>& q2)
{
    return LvnQuat_t<T>(q1.w + q2.w, q1.x + q2.x, q1.y + q2.y, q1.z + q2.z);
}
template <typename T>
LvnQuat_t<T> operator-(const LvnQuat_t<T>& q1, const LvnQuat_t<T>& q2)
{
    return LvnQuat_t<T>(q1.w - q2.w, q1.x - q2.x, q1.y - q2.y, q1.z - q2.z);
}
template <typename T>
LvnQuat_t<T> operator*(const LvnQuat_t<T>& q1, const LvnQuat_t<T>& q2)
{
    return LvnQuat_t<T>(q1.w * q2.w, q1.x * q2.x, q1.y * q2.y, q1.z * q2.z);
}
template <typename T>
LvnQuat_t<T> operator/(const LvnQuat_t<T>& q1, const LvnQuat_t<T>& q2)
{
    return LvnQuat_t<T>(q1.w / q2.w, q1.x / q2.x, q1.y / q2.y, q1.z / q2.z);
}
template <typename T>
LvnQuat_t<T> operator+(const T& s, const LvnQuat_t<T>& q)
{
    return LvnQuat_t<T>(s + q.w, s + q.x, s + q.y, s + q.z);
}
template <typename T>
LvnQuat_t<T> operator-(const T& s, const LvnQuat_t<T>& q)
{
    return LvnQuat_t<T>(s - q.w, s - q.x, s - q.y, s - q.z);
}
template <typename T>
LvnQuat_t<T> operator*(const T& s, const LvnQuat_t<T>& q)
{
    return LvnQuat_t<T>(s * q.w, s * q.x, s * q.y, s * q.z);
}
template <typename T>
LvnQuat_t<T> operator/(const T& s, const LvnQuat_t<T>& q)
{
    return LvnQuat_t<T>(s / q.w, s / q.x, s / q.y, s / q.z);
}
template <typename T>
LvnQuat_t<T> operator+(const LvnQuat_t<T>& q, const T& s)
{
    return LvnQuat_t<T>(q.w + s, q.x + s, q.y + s, q.z + s);
}
template <typename T>
LvnQuat_t<T> operator-(const LvnQuat_t<T>& q, const T& s)
{
    return LvnQuat_t<T>(q.w - s, q.x - s, q.y - s, q.z - s);
}
template <typename T>
LvnQuat_t<T> operator*(const LvnQuat_t<T>& q, const T& s)
{
    return LvnQuat_t<T>(q.w * s, q.x * s, q.y * s, q.z * s);
}
template <typename T>
LvnQuat_t<T> operator/(const LvnQuat_t<T>& q, const T& s)
{
    return LvnQuat_t<T>(q.w / s, q.x / s, q.y / s, q.z / s);
}

// -- [SUBSECT]: Core Struct Implementation
// ------------------------------------------------------------

struct LvnMemoryBindingInfo
{
    LvnStructureType sType;
    uint64_t count;
};

struct LvnContextCreateInfo
{
    std::string                   applicationName;               // name of application or program
    LvnWindowApi                  windowapi;                     // window api to use when creating windows
    LvnGraphicsApi                graphicsapi;                   // graphics api to use when rendering (eg. vulkan, opengl)
    bool                          enableMultithreading;          // enables the use of multithreading within the context

    struct
    {
        bool                      enableLogging;                 // enable or diable logging
        bool                      disableCoreLogging;            // whether to disable core logging in the library
        bool                      enableGraphicsApiDebugLogs;    // enable debug output for graphics api calls (eg. vulkan validation layer, opengl debug callbacks)
    } logging;

    struct
    {
        LvnTextureFormat              frameBufferColorFormat;        // set the color image format of the window framebuffer when rendering
        LvnClipRegion                 matrixClipRegion;              // set the clip region to the correct coordinate system depending on the api
        uint32_t                      maxFramesInFlight;             // set the max frames in flight (vulkan only)
    } rendering;
    
    struct
    {
        LvnMemAllocMode           memAllocMode;                  // memory allocation mode, how memory should be allocated when creating new object
        LvnMemoryBindingInfo*     pMemoryBindings;               // array of object alloc info structs to tell how many objects of each type to allocate if using memory pool
        uint32_t                  memoryBindingCount;            // number of object alloc inso structs;
        LvnMemoryBindingInfo*     pBlockMemoryBindings;          // array of objects alloc info structs of each type to allocate for further memory blocks in case if the first block is full
        uint32_t                  blockMemoryBindingCount;       // number of block object alloc info structs
    } memoryInfo;
};

/* [Logging] */
struct LvnLoggerCreateInfo
{
    std::string loggerName;
    std::string format;
    LvnLogLevel level;

    struct
    {
        bool enableLogToFile;
        std::string filename;
        LvnFileMode filemode;
    } fileConfig;
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

struct LvnLogFile
{
    std::string filename;
    LvnFileMode filemode;
    FILE* fileptr;
    bool logToFile;
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
    std::string title;                  // title of window
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
        minWidth = 0, minHeight = 0;
        maxWidth = -1, maxHeight = -1;
        fullscreen = false, resizable = true, vSync = false;
        pIcons = nullptr;
        iconCount = 0;
        eventCallBack = nullptr;
        userData = nullptr;
    }
};


// -- [SUBSECT]: Graphics Struct Implementation
// ------------------------------------------------------------

struct LvnPhysicalDeviceProperties
{
    std::string name;
    LvnPhysicalDeviceType type;
    uint32_t apiVersion;
    uint32_t driverVersion;
    uint32_t vendorID;
};

struct LvnPhysicalDeviceFeatures
{
    bool robustBufferAccess;
    bool fullDrawIndexUint32;
    bool imageCubeArray;
    bool independentBlend;
    bool geometryShader;
    bool tessellationShader;
    bool sampleRateShading;
    bool dualSrcBlend;
    bool logicOp;
    bool multiDrawIndirect;
    bool drawIndirectFirstInstance;
    bool depthClamp;
    bool depthBiasClamp;
    bool fillModeNonSolid;
    bool depthBounds;
    bool wideLines;
    bool largePoints;
    bool alphaToOne;
    bool multiViewport;
    bool samplerAnisotropy;
    bool textureCompressionETC2;
    bool textureCompressionASTC_LDR;
    bool textureCompressionBC;
    bool occlusionQueryPrecise;
    bool pipelineStatisticsQuery;
    bool vertexPipelineStoresAndAtomics;
    bool fragmentStoresAndAtomics;
    bool shaderTessellationAndGeometryPointSize;
    bool shaderImageGatherExtended;
    bool shaderStorageImageExtendedFormats;
    bool shaderStorageImageMultisample;
    bool shaderStorageImageReadWithoutFormat;
    bool shaderStorageImageWriteWithoutFormat;
    bool shaderUniformBufferArrayDynamicIndexing;
    bool shaderSampledImageArrayDynamicIndexing;
    bool shaderStorageBufferArrayDynamicIndexing;
    bool shaderStorageImageArrayDynamicIndexing;
    bool shaderClipDistance;
    bool shaderCullDistance;
    bool shaderFloat64;
    bool shaderInt64;
    bool shaderInt16;
    bool shaderResourceResidency;
    bool shaderResourceMinLod;
    bool sparseBinding;
    bool sparseResidencyBuffer;
    bool sparseResidencyImage2D;
    bool sparseResidencyImage3D;
    bool sparseResidency2Samples;
    bool sparseResidency4Samples;
    bool sparseResidency8Samples;
    bool sparseResidency16Samples;
    bool sparseResidencyAliased;
    bool variableMultisampleRate;
    bool inheritedQueries;
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
    LvnAttributeFormat format;
    uint64_t offset;
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
    const LvnUniformBufferInfo* bufferInfo;
    const LvnTexture* const* pTextureInfos;
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
    std::string vertexSrc;
    std::string fragmentSrc;
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
    LvnBufferTypeFlagBits type;
    LvnBufferUsage usage;
    uint64_t size;
    const void* data;
};

struct LvnUniformBufferInfo
{
    LvnBuffer* buffer;
    uint64_t range;
    uint64_t offset;
};

struct LvnImageData
{
    LvnData<uint8_t> pixels;
    uint32_t width, height, channels;
    uint64_t size;
};

struct LvnImageHdrData
{
    LvnData<float> pixels;
    uint32_t width, height, channels;
    uint64_t size;
};

struct LvnSamplerCreateInfo
{
    LvnTextureFilter minFilter, magFilter;
    LvnTextureMode wrapS, wrapT;
};

struct LvnTextureCreateInfo
{
    LvnImageData imageData;
    LvnTextureFormat format;
    LvnTextureFilter minFilter, magFilter;
    LvnTextureMode wrapS, wrapT;
};

struct LvnTextureSamplerCreateInfo
{
    LvnImageData imageData;
    LvnTextureFormat format;
    LvnSampler* sampler;
};

struct LvnVertex
{
    LvnVec3 pos;
    LvnVec4 color;
    LvnVec2 texUV;
    LvnVec3 normal;
    LvnVec3 tangent;
    LvnVec3 bitangent;
    LvnVec4 joints;
    LvnVec4 weights;
};

struct LvnTransform
{
    LvnVec3 translation;
    LvnQuat rotation;
    LvnVec3 scale;
};

struct LvnMaterial
{
    LvnVec3 baseColorFactor;
    LvnVec3 emissiveFactor;
    float metallicFactor;
    float roughnessFactor;

    LvnTexture* albedo;
    LvnTexture* metallicRoughnessOcclusion;
    LvnTexture* normal;
    LvnTexture* emissive;

    bool doubleSided;
};

struct LvnPrimitive
{
    LvnTopologyType topology;
    LvnMaterial material;
    uint32_t vertexCount;
    uint32_t indexCount;

    LvnBuffer* buffer;
    LvnDescriptorSet* descriptorSet;
};

struct LvnMesh
{
    std::vector<LvnPrimitive> primitives;
};

struct LvnNode
{
    LvnNode* parent;
    uint32_t index;
    std::vector<std::shared_ptr<LvnNode>> children;

    LvnMesh mesh;
    LvnTransform transform;
    int32_t skin;
    LvnMat4 matrix;
};

struct LvnSkin
{
    std::string name;
    std::vector<LvnMat4> inverseBindMatrices;
    std::vector<LvnNode*> joints;
    LvnBuffer* ssbo;
};

struct LvnAnimationChannel
{
    LvnAnimationPath path;
    LvnInterpolationMode interpolation;
    std::vector<float> keyFrames;
    std::vector<LvnVec4> outputs;
    LvnNode* node;
};

struct LvnAnimation
{
    std::vector<LvnAnimationChannel> channels;
    float start;
    float end;
    float currentTime;
};

struct LvnModel
{
    std::vector<std::shared_ptr<LvnNode>> nodes;
    std::vector<LvnAnimation> animations;
    std::vector<LvnSkin> skins;
    LvnMat4 matrix;

    std::vector<LvnBuffer*> buffers;
    std::vector<LvnSampler*> samplers;
    std::vector<LvnTexture*> textures;
};

struct LvnCamera
{
    float aspectRatio;           // aspect ratio (width / height)
    float fov;                   // field of view
    float zNear;                 // near plane
    float zFar;                  // far plane
};

struct LvnOrthoCamera
{
    float right;                 // posx bound
    float left;                  // negx bound
    float top;                   // posy bound
    float bottom;                // negy bound
    float zNear;                 // far plane
    float zFar;                  // near plane
};

struct LvnCubemapCreateInfo
{
    LvnImageData posx, negx, posy, negy, posz, negz;
};

struct LvnCubemapHdrCreateInfo
{
    LvnImageHdrData hdr;
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

    uint32_t unicode;
    int advance;
};

struct LvnFont
{
    LvnImageData atlas;
    float fontSize;

    LvnData<uint32_t> codepoints;
    LvnData<LvnFontGlyph> glyphs;
};


// -- [SUBSECT]: Audio Struct Implementation
// ------------------------------------------------------------

struct LvnSoundCreateInfo
{
    std::string filepath;      // the filepath to the sound file (.wav .mp3)

    float volume;              // volume of sound source, (default: 1.0, min/mute: 0.0), 1.0 is the upper limit however volume can be set higher than 1.0 at your own risk
    float pan;                 // pan of the sound source if using 2 channel sterio output (center: 0.0, left: -1.0, right 1.0)
    float pitch;               // pitch of the sound source, (default: 1.0, min: 0.0, no upper limit)
    bool looping;              // sound source loops when reaches end of track

    LvnVec3 pos;
};


// -- [SUBSECT]: Networking Struct Implementation
// ------------------------------------------------------------

struct LvnAddress
{
    uint32_t host;
    uint16_t port;
};

struct LvnSocketCreateInfo
{
    LvnSocketType type;
    LvnAddress address;
    uint32_t channelCount;
    uint32_t connectionCount;
    uint32_t inBandWidth;
    uint32_t outBandWidth;
};

struct LvnPacket
{
    void* data;
    size_t size;
};


// -- [SUBSECT]: Renderer Struct Implementation
// ------------------------------------------------------------

struct LvnColor
{
    uint8_t r, g, b, a;
};

struct LvnPoint
{
    float x, y;
};

struct LvnTriangle
{
    LvnVec2 v1;
    LvnVec2 v2;
    LvnVec2 v3;
};

struct LvnRect
{
    LvnVec2 pos;
    LvnVec2 size;
};

struct LvnCircle
{
    LvnVec2 pos;
    float radius;
};

struct LvnPoly
{

};

#endif
