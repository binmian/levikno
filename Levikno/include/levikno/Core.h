#ifndef HG_LEVIKNO_CORE_H
#define HG_LEVIKNO_CORE_H


#ifndef HG_LEVIKNO_DEFINE_CONFIG
#define HG_LEVIKNO_DEFINE_CONFIG

// Platform
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) 
	#ifndef LVN_PLATFORM_WINDOWS
		#define LVN_PLATFORM_WINDOWS
	#endif
	#ifdef LVN_SHARED_LIBRARY_EXPORT
			#define LVN_API __declspec(dllexport)
	#elif LVN_SHARED_LIBRARY_IMPORT
			#define LVN_API __declspec(dllimport)
	#else 
		#define LVN_API
	#endif

#elif __APPLE__
	#define LVN_PLATFORM_APPLE

#elif __linux__
	#define LVN_PLATFORM_LINUX

#else
	#error "lvn does not support the current platform."
#endif

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
	#define LVN_ASSERT_BREAK assert(false);
#endif

// Debug
#ifdef LVN_DEBUG
	#define LVN_ENABLE_ASSERTS 
#endif

#ifdef LVN_DISABLE_ASSERTS_KEEP_ERROR_MESSAGES
	#define LVN_DISABLE_ASSERTS
#endif

#if defined (LVN_DISABLE_ASSERTS)
	#define LVN_ASSERT(x, ...) { if(!(x)) { LVN_ERROR(LVN_LOG_FILE##__VA_ARGS__); } }
	#define LVN_CORE_ASSERT(x, ...) { if(!(x)) { LVN_CORE_ERROR(LVN_LOG_FILE##__VA_ARGS__); } }
#elif defined(LVN_ENABLE_ASSERTS)
	#define LVN_ASSERT(x, ...) { if(!(x)) { LVN_ERROR(LVN_LOG_FILE##__VA_ARGS__); LVN_ASSERT_BREAK; } }
	#define LVN_CORE_ASSERT(x, ...) { if(!(x)) { LVN_CORE_ERROR(LVN_LOG_FILE##__VA_ARGS__); LVN_ASSERT_BREAK; } }
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

#define LVN_MALLOC(type, size) static_cast<type>(malloc(size))
#define LVN_FREE(mem) free(mem)

#define LVN_FILE_NAME __FILE__
#define LVN_LINE __LINE__
#define LVN_FUNC_NAME __func__

#define LVN_STR(x) #x
#define LVN_STRINGIFY(x) LVN_STR(x)


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
#define LVN_TRACE(...)							::lvn::logMessageTrace(lvn::getClientLogger(), ##__VA_ARGS__)
#define LVN_INFO(...)							::lvn::logMessageInfo(lvn::getClientLogger(), ##__VA_ARGS__)
#define LVN_WARN(...)							::lvn::logMessageWarn(lvn::getClientLogger(), ##__VA_ARGS__)
#define LVN_ERROR(...)							::lvn::logMessageError(lvn::getClientLogger(), ##__VA_ARGS__)
#define LVN_CRITICAL(...)						::lvn::logMessageCritical(lvn::getClientLogger(), ##__VA_ARGS__)


// Logging utils
#define LVN_PROPERTIES(prop)					#prop, &prop
#define LVN_LOG_FILE							LVN_FILE_NAME ":" LVN_STRINGIFY(LVN_LINE) " - "
#define LVN_MALLOC_FAILURE(prop)				LVN_CORE_ERROR("malloc failure, could not allocate memory for: \"%s\" at %p", LVN_PROPERTIES(prop))
#define LVN_MALLOC_ASSERT(prop)					LVN_CORE_ASSERT(prop, "malloc failure, could not allocate memory for: \"%s\" at %p", LVN_PROPERTIES(prop));

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

struct LvnAppRenderEvent;
struct LvnAppTickEvent;
struct LvnEvent;
struct LvnKeyTypedEvent;
struct LvnKeyHoldEvent;
struct LvnKeyPressedEvent;
struct LvnKeyReleasedEvent;
struct LvnLogger;
struct LvnLogMessage;
struct LvnLogPattern;
struct LvnString;
struct LvnMouseButtonPressedEvent;
struct LvnMouseButtonReleasedEvent;
struct LvnMouseMovedEvent;
struct LvnMouseScrolledEvent;
struct LvnWindow;
struct LvnWindowCloseEvent;
struct LvnWindowContext;
struct LvnWindowCreateInfo;
struct LvnWindowData;
struct LvnWindowDimension;
struct LvnWindowEvent;
struct LvnWindowFocusEvent;
struct LvnWindowFramebufferResizeEvent;
struct LvnWindowIconData;
struct LvnWindowLostFocusEvent;
struct LvnWindowMovedEvent;
struct LvnWindowResizeEvent;


/* [Core Struct Implementaion] */
/* [Logging] */
struct LvnLogMessage
{
	const char* msg, * loggerName;
	LvnLogLevel level;
	long long timeEpoch;
};

struct LvnLogPattern
{
	char symbol;
	LvnString(*func)(LvnLogMessage*);
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
	int width, height;
	const char* title;
	int minWidth, minHeight;
	int maxWidth, maxHeight;
	bool fullscreen, resizable, vSync;
	LvnWindowIconData* pIcons;
	uint32_t iconCount;

	LvnWindowCreateInfo()
	{
		width = 0; height = 0; title = nullptr;
		minWidth = 0; minHeight = 0; maxWidth = -1; maxHeight = -1;
		resizable = true; vSync = false;
		pIcons = nullptr;
		iconCount = 0;
	}
};

struct LvnWindowDimension
{
	int width, height;
};

struct LvnString
{
	char* m_Str;
	uint32_t m_Size;

	LvnString()
		: m_Str(0), m_Size(0) {}

	LvnString(const char* strsrc)
	{
		m_Size = strlen(strsrc) + 1;
		m_Str = (char*)malloc(m_Size);
		if (!m_Str) { return; }
		memcpy(m_Str, strsrc, m_Size);
	}

	LvnString(uint32_t strsize)
	{
		m_Size = strsize;
		m_Str = (char*)malloc(m_Size);
		if (!m_Str) { return; }
	}

	LvnString(char* strsrc, uint32_t strsize)
	{
		m_Size = strsize;
		m_Str = (char*)malloc(strsize);
		if (!m_Str) { return; }
		memcpy(m_Str, strsrc, strsize);
	}

	LvnString(const LvnString& lvnstr)
	{
		m_Size = lvnstr.m_Size;
		m_Str = (char*)malloc(m_Size);
		if (!m_Str) { return; }
		memcpy(m_Str, lvnstr.m_Str, m_Size);
	}

	LvnString& operator=(const LvnString& lvnstr)
	{
		m_Size = lvnstr.m_Size;
		m_Str = (char*)malloc(m_Size);
		if (!m_Str) { return *this; }
		memcpy(m_Str, lvnstr.m_Str, m_Size);
		return *this;
	}

	const char* c_str() { return m_Str; }
	uint32_t size() { return m_Size; }

	operator const char* () { return this->m_Str; }
	operator char* () { return this->m_Str; }
};

namespace lvn
{
	/* Core API Decleration */


	/* [API] */
	LVN_API const char*			getDateMonthName();
	LVN_API const char*			getDateMonthNameShort();
	LVN_API const char*			getDateWeekDayName();
	LVN_API const char*			getDateWeekDayNameShort();
	LVN_API const char*			getDateTimeMeridiem();
	LVN_API const char*			getDateTimeMeridiemLower();

	LVN_API LvnString			getDateTimeHHMMSS();
	LVN_API LvnString			getDateTime12HHMMSS();
	LVN_API LvnString			getDateYearStr();
	LVN_API LvnString			getDateYear02dStr();
	LVN_API LvnString			getDateMonthNumStr();
	LVN_API LvnString			getDateDayNumStr();
	LVN_API LvnString			getDateHourNumStr();
	LVN_API LvnString			getDateHour12NumStr();
	LVN_API LvnString			getDateMinuteNumStr();
	LVN_API LvnString			getDateSecondNumStr();

	LVN_API int					getDateYear();
	LVN_API int					getDateYear02d();
	LVN_API int					getDateMonth();
	LVN_API int					getDateDay();
	LVN_API int					getDateHour();
	LVN_API int					getDateHour12();
	LVN_API int					getDateMinute();
	LVN_API int					getDateSecond();
	LVN_API long long			getSecondSinceEpoch();


	/* [Logging] */
	LVN_API bool				logInit();
	LVN_API bool				logTerminate();
	LVN_API void				logSetLevel(LvnLogger* logger, LvnLogLevel level);
	LVN_API bool				logCheckLevel(LvnLogger* logger, LvnLogLevel level);
	LVN_API void				logOutputMessage(LvnLogger* logger, LvnLogMessage* msg);
	LVN_API LvnLogger*			getCoreLogger();
	LVN_API LvnLogger*			getClientLogger();
	LVN_API const char*			getLogANSIcodeColor(LvnLogLevel level);
	LVN_API void				logSetPattern(LvnLogger* logger, const char* pattern);

	/* [Events] */
	LVN_API bool				dispatchLvnAppRenderEvent(LvnEvent* event, bool(*func)(LvnAppRenderEvent*));
	LVN_API bool				dispatchLvnAppTickEvent(LvnEvent* event, bool(*func)(LvnAppTickEvent*));
	LVN_API bool				dispatchKeyHoldEvent(LvnEvent* event, bool(*func)(LvnKeyHoldEvent*));
	LVN_API bool				dispatchKeyPressedEvent(LvnEvent* event, bool(*func)(LvnKeyPressedEvent*));
	LVN_API bool				dispatchKeyReleasedEvent(LvnEvent* event, bool(*func)(LvnKeyReleasedEvent*));
	LVN_API bool				dispatchKeyTypedEvent(LvnEvent* event, bool(*func)(LvnKeyTypedEvent*));
	LVN_API bool				dispatchMouseButtonPressedEvent(LvnEvent* event, bool(*func)(LvnMouseButtonPressedEvent*));
	LVN_API bool				dispatchMouseButtonReleasedEvent(LvnEvent* event, bool(*func)(LvnMouseButtonReleasedEvent*));
	LVN_API bool				dispatchMouseMovedEvent(LvnEvent* event, bool(*func)(LvnMouseMovedEvent*));
	LVN_API bool				dispatchMouseScrolledEvent(LvnEvent* event, bool(*func)(LvnMouseScrolledEvent*));
	LVN_API bool				dispatchWindowCloseEvent(LvnEvent* event, bool(*func)(LvnWindowCloseEvent*));
	LVN_API bool				dispatchWindowFramebufferResizeEvent(LvnEvent* event, bool(*func)(LvnWindowFramebufferResizeEvent*));
	LVN_API bool				dispatchWindowFocusEvent(LvnEvent* event, bool(*func)(LvnWindowFocusEvent*));
	LVN_API bool				dispatchWindowLostFocusEvent(LvnEvent* event, bool(*func)(LvnWindowLostFocusEvent*));
	LVN_API bool				dispatchWindowMovedEvent(LvnEvent* event, bool(*func)(LvnWindowMovedEvent*));
	LVN_API bool				dispatchWindowResizeEvent(LvnEvent* event, bool(*func)(LvnWindowResizeEvent*));

	/* [Window] */
	LVN_API bool				createWindowContext(LvnWindowApi windowapi);
	LVN_API bool				terminateWindowContext();
	LVN_API LvnWindowApi		getWindowApi();
	LVN_API const char*			getWindowApiName();

	LVN_API LvnWindow*			createWindow(int width, int height, const char* title, bool fullscreen = false, bool resizable = true, int minWidth = 0, int minHeight = 0);
	LVN_API LvnWindow*			createWindow(LvnWindowCreateInfo* winCreateInfo);

	LVN_API void				updateWindow(LvnWindow* window);
	LVN_API bool				windowOpen(LvnWindow* window);
	LVN_API LvnWindowDimension	getWindowDimensions(LvnWindow* window);
	LVN_API int					getWindowWidth(LvnWindow* window);
	LVN_API int					getWindowHeight(LvnWindow* window);
	LVN_API void				setWindowEventCallback(LvnWindow* window, void (*callback)(LvnEvent*));
	LVN_API void				setWindowVSync(LvnWindow* window, bool enable);
	LVN_API bool				getWindowVSync(LvnWindow* window);
	LVN_API void*				getNativeWindow(LvnWindow* window);
	LVN_API void				setWindowContextCurrent(LvnWindow* window);
	LVN_API void				destroyWindow(LvnWindow* window);



	/* [Implementation] */
	template<typename... Args>
	void logMessage(LvnLogger* logger, LvnLogLevel level, const char* fmt, Args... args)
	{
		if (!logCheckLevel(logger, level))
			return;

		int64_t epoch = getSecondSinceEpoch();

		int strsize = snprintf(nullptr, 0, fmt, args...) + 1;
		if (strsize <= 0)
			return;
		char* buff = (char*)malloc(strsize);
		snprintf(buff, strsize, fmt, args...);

		LvnLogMessage logMsg = { .msg = buff, .level = level, .timeEpoch = epoch };
		logOutputMessage(logger, &logMsg);
		free(buff);
	}

	template<typename... Args>
	void logMessageTrace(LvnLogger* logger, const char* fmt, Args... args)
	{
		logMessage(logger, Lvn_LogLevel_Trace, fmt, args...);
	}
	template<typename... Args>
	void logMessageInfo(LvnLogger* logger, const char* fmt, Args... args)
	{
		logMessage(logger, Lvn_LogLevel_Info, fmt, args...);
	}
	template<typename... Args>
	void logMessageWarn(LvnLogger* logger, const char* fmt, Args... args)
	{
		logMessage(logger, Lvn_LogLevel_Warn, fmt, args...);
	}
	template<typename... Args>
	void logMessageError(LvnLogger* logger, const char* fmt, Args... args)
	{
		logMessage(logger, Lvn_LogLevel_Error, fmt, args...);
	}
	template<typename... Args>
	void logMessageCritical(LvnLogger* logger, const char* fmt, Args... args)
	{
		logMessage(logger, Lvn_LogLevel_Critical, fmt, args...);
	}
}

#endif