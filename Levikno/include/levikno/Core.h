#ifndef HG_LEVIKNO_CORE_H
#define HG_LEVIKNO_CORE_H


#ifndef HG_LEVIKNO_DEFINE_CONFIG
#define HG_LEVIKNO_DEFINE_CONFIG

// Platform
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) 
	#ifndef LVN_PLATFORM_WINDOWS
		#define LVN_PLATFORM_WINDOWS
	#endif
	#ifdef LVN_PLATFORM_WINDOWS
		#ifdef LVN_SHARED_LIBRARY
			#define LVN_API __declspec(dllexport)
			#define LVN_API_IMPORT __declspec(dllimport)
		#else 
			#define LVN_API
		#endif
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

#ifdef LVN_ENABLE_ASSERTS
	#define LVN_ASSERT(x, ...) { if(!(x)) { LVN_ERROR(__VA_ARGS__); LVN_ASSERT_BREAK; } }
	#define LVN_CORE_ASSERT(x, ...) { if(!(x)) { LVN_CORE_ERROR(__VA_ARGS__); LVN_ASSERT_BREAK; } }
#elif LVN_DISABLE_ASSERTS
	#define LVN_ASSERT(x, ...) { if(!(x)) { LVN_ERROR("ERROR: {0}", __VA_ARGS__); } }
	#define LVN_CORE_ASSERT(x, ...) { if(!(x)) { LVN_CORE_ERROR("ERROR: {0}", __VA_ARGS__); } }
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
#define LVN_STRIGIFY(x) LVN_STR(x)


#endif // !HG_LVN_DEFINE_CONFIG

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* [Logging] */

/*
*  |Name           | FG | BG
* ---------------------------
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
*	TRACE			\33[1;37m
*	INFO			\33[0;32m
*	WARN			\33[1;33m
*	ERROR			\33[1;31m
*	CRITICAL		\33[1;37;41m
* 
*/

#define LVN_LOG_COLOR_TRACE 	  "\33[0;37m"
#define LVN_LOG_COLOR_INFO 	  "\33[0;32m"
#define LVN_LOG_COLOR_WARN 	  "\33[1;33m"
#define LVN_LOG_COLOR_ERROR 	  "\33[1;31m"
#define LVN_LOG_COLOR_CRITICAL "\33[1;37;41m"
#define LVN_LOG_COLOR_RESET	  "\33[0m"

#define LVN_PROPERTIES(prop, ...)				#prop, &prop, #__VA_ARGS__

// Core Log macros									
#define LVN_CORE_TRACE(...)						::lvn::logMessageTrace(lvn::getCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_INFO(...)						::lvn::logMessageInfo(lvn::getCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_WARN(...)						::lvn::logMessageWarn(lvn::getCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_ERROR(...)						::lvn::logMessageError(lvn::getCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_CRITICAL(...)					::lvn::logMessageCritical(lvn::getCoreLogger(), ##__VA_ARGS__)
													
#define LVN_CORE_LOG_TRACE(name, msg, ...)		::lvn::logMessageTrace(lvn::getCoreLogger(), #name "-trace: ", msg, ##__VA_ARGS__)
#define LVN_CORE_LOG_INFO(name, msg, ...)		::lvn::logMessageInfo(lvn::getCoreLogger(), #name "-info: ", msg, ##__VA_ARGS__)
#define LVN_CORE_LOG_WARN(name, msg, ...)		::lvn::logMessageWarn(lvn::getCoreLogger(), #name "-warning: ", msg, ##__VA_ARGS__)
#define LVN_CORE_LOG_ERROR(name, msg, ...)		::lvn::logMessageError(lvn::getCoreLogger(), #name "-error: ", msg, ##__VA_ARGS__)
#define LVN_CORE_LOG_CRITICAL(name, msg, ...)	::lvn::logMessageCritical(lvn::getCoreLogger(), #name "-critical: ", msg, ##__VA_ARGS__)
													
													
// Client Log macros								
#define LVN_TRACE(...)							::lvn::logMessageTrace(lvn::getClientLogger(), ##__VA_ARGS__)
#define LVN_INFO(...)							::lvn::logMessageInfo(lvn::getClientLogger(), ##__VA_ARGS__)
#define LVN_WARN(...)							::lvn::logMessageWarn(lvn::getClientLogger(), ##__VA_ARGS__)
#define LVN_ERROR(...)							::lvn::logMessageError(lvn::getClientLogger(), ##__VA_ARGS__)
#define LVN_CRITICAL(...)						::lvn::logMessageCritical(lvn::getClientLogger(), ##__VA_ARGS__)
													
#define LVN_LOG_TRACE(name, msg, ...)			::lvn::logMessageTrace(lvn::getClientLogger(), #name "-trace: ", msg, ##__VA_ARGS__)
#define LVN_LOG_INFO(name, msg, ...)			::lvn::logMessageInfo(lvn::getClientLogger(), #name "-info: ", msg, ##__VA_ARGS__)
#define LVN_LOG_WARN(name, msg, ...)			::lvn::logMessageWarn(lvn::getClientLogger(), #name "-warning: ", msg, ##__VA_ARGS__)
#define LVN_LOG_ERROR(name, msg, ...)			::lvn::logMessageError(lvn::getClientLogger(), #name "-error: ", msg, ##__VA_ARGS__)
#define LVN_LOG_CRITICAL(name, msg, ...)		::lvn::logMessageCritical(lvn::getClientLogger(), #name "-critical: ", msg, ##__VA_ARGS__)


/* Key Codes */
enum LvnKeyCodes
{
	LvnKeyCode_Space			= 32,
	LvnKeyCode_Apostrophe		= 39,		/* ' */
	LvnKeyCode_Comma			= 44,		/* , */
	LvnKeyCode_Minus			= 45,		/* - */
	LvnKeyCode_Period			= 46,		/* . */
	LvnKeyCode_Slash			= 47,		/* / */
	LvnKeyCode_0				= 48,
	LvnKeyCode_1				= 49,
	LvnKeyCode_2				= 50,
	LvnKeyCode_3				= 51,
	LvnKeyCode_4				= 52,
	LvnKeyCode_5				= 53,
	LvnKeyCode_6				= 54,
	LvnKeyCode_7				= 55,
	LvnKeyCode_8				= 56,
	LvnKeyCode_9				= 57,
	LvnKeyCode_Semicolon		= 59,		/* ; */
	LvnKeyCode_Equal			= 61,		/* = */
	LvnKeyCode_A				= 65,
	LvnKeyCode_B				= 66,
	LvnKeyCode_C				= 67,
	LvnKeyCode_D				= 68,
	LvnKeyCode_E				= 69,
	LvnKeyCode_F				= 70,
	LvnKeyCode_G				= 71,
	LvnKeyCode_H				= 72,
	LvnKeyCode_I				= 73,
	LvnKeyCode_J				= 74,
	LvnKeyCode_K				= 75,
	LvnKeyCode_L				= 76,
	LvnKeyCode_M				= 77,
	LvnKeyCode_N				= 78,
	LvnKeyCode_O				= 79,
	LvnKeyCode_P				= 80,
	LvnKeyCode_Q				= 81,
	LvnKeyCode_R				= 82,
	LvnKeyCode_S				= 83,
	LvnKeyCode_T				= 84,
	LvnKeyCode_U				= 85,
	LvnKeyCode_V				= 86,
	LvnKeyCode_W				= 87,
	LvnKeyCode_X				= 88,
	LvnKeyCode_Y				= 89,
	LvnKeyCode_Z				= 90,
	LvnKeyCode_LeftBracket		= 91,		/* [ */
	LvnKeyCode_Backslash		= 92,		/* \ */
	LvnKeyCode_RightBracket		= 93,		/* ] */
	LvnKeyCode_GraveAccent		= 96,		/* ` */
	LvnKeyCode_World1			= 161,		/* non-US #1 */
	LvnKeyCode_World2			= 162,		/* non-US #2 */

	/* Function keys */
	LvnKeyCode_Escape			= 256,
	LvnKeyCode_Enter			= 257,
	LvnKeyCode_Tab				= 258,
	LvnKeyCode_Backspace		= 259,
	LvnKeyCode_Insert			= 260,
	LvnKeyCode_Delete			= 261,
	LvnKeyCode_Right			= 262,
	LvnKeyCode_Left				= 263,
	LvnKeyCode_Down				= 264,
	LvnKeyCode_Up				= 265,
	LvnKeyCode_PageUp			= 266,
	LvnKeyCode_PageDown			= 267,
	LvnKeyCode_Home				= 268,
	LvnKeyCode_End				= 269,
	LvnKeyCode_CapsLock			= 280,
	LvnKeyCode_ScrollLock		= 281,
	LvnKeyCode_NumLock			= 282,
	LvnKeyCode_PrintScreen		= 283,
	LvnKeyCode_Pause			= 284,
	LvnKeyCode_F1				= 290,
	LvnKeyCode_F2				= 291,
	LvnKeyCode_F3				= 292,
	LvnKeyCode_F4				= 293,
	LvnKeyCode_F5				= 294,
	LvnKeyCode_F6				= 295,
	LvnKeyCode_F7				= 296,
	LvnKeyCode_F8				= 297,
	LvnKeyCode_F9				= 298,
	LvnKeyCode_F10				= 299,
	LvnKeyCode_F11				= 300,
	LvnKeyCode_F12				= 301,
	LvnKeyCode_F13				= 302,
	LvnKeyCode_F14				= 303,
	LvnKeyCode_F15				= 304,
	LvnKeyCode_F16				= 305,
	LvnKeyCode_F17				= 306,
	LvnKeyCode_F18				= 307,
	LvnKeyCode_F19				= 308,
	LvnKeyCode_F20				= 309,
	LvnKeyCode_F21				= 310,
	LvnKeyCode_F22				= 311,
	LvnKeyCode_F23				= 312,
	LvnKeyCode_F24				= 313,
	LvnKeyCode_F25				= 314,
	LvnKeyCode_KP_0				= 320,
	LvnKeyCode_KP_1				= 321,
	LvnKeyCode_KP_2				= 322,
	LvnKeyCode_KP_3				= 323,
	LvnKeyCode_KP_4				= 324,
	LvnKeyCode_KP_5				= 325,
	LvnKeyCode_KP_6				= 326,
	LvnKeyCode_KP_7				= 327,
	LvnKeyCode_KP_8				= 328,
	LvnKeyCode_KP_9				= 329,
	LvnKeyCode_KP_Decimal		= 330,
	LvnKeyCode_KP_Divide		= 331,
	LvnKeyCode_KP_Multiply		= 332,
	LvnKeyCode_KP_Subtract		= 333,
	LvnKeyCode_KP_Add			= 334,
	LvnKeyCode_KP_Enter			= 335,
	LvnKeyCode_KP_Equal			= 336,
	LvnKeyCode_LeftShift		= 340,
	LvnKeyCode_LeftControl		= 341,
	LvnKeyCode_LeftAlt			= 342,
	LvnKeyCode_LeftSuper		= 343,
	LvnKeyCode_RightShift		= 344,
	LvnKeyCode_RightControl		= 345,
	LvnKeyCode_RightAlt			= 346,
	LvnKeyCode_RightSuper		= 347,
	LvnKeyCode_Menu				= 348,
};

/* Mouse Button Codes */
enum LvnMouseButtonCodes
{
	LvnMouseButton_1			= 0,
	LvnMouseButton_2			= 1,
	LvnMouseButton_3			= 2,
	LvnMouseButton_4			= 3,
	LvnMouseButton_5			= 4,
	LvnMouseButton_6			= 5,
	LvnMouseButton_7			= 6,
	LvnMouseButton_8			= 7,
	LvnMouseButton_Last			= LvnMouseButton_8,
	LvnMouseButton_Left			= LvnMouseButton_1,
	LvnMouseButton_Right		= LvnMouseButton_2,
	LvnMouseButton_Middle		= LvnMouseButton_3,
};

/* Mouse Mode Codes */
enum LvnMouseModes
{
	LvnMouseCursor_Arrow			= 0,
	LvnMouseCursor_Ibeam			= 1,
	LvnMouseCursor_Crosshair		= 2,
	LvnMouseCursor_PointingHand		= 3,
	LvnMouseCursor_ResizeEW			= 4,
	LvnMouseCursor_ResizeNS			= 5,
	LvnMouseCursor_ResizeNWSE		= 6,
	LvnMouseCursor_ResizeNESW		= 7,
	LvnMouseCursor_ResizeAll		= 8,
	LvnMouseCursor_NotAllowed		= 9,
	LvnMouseCursor_HResize			= LvnMouseCursor_ResizeEW,
	LvnMouseCursor_VRrsize			= LvnMouseCursor_ResizeNS,
	LvnMouseCursor_Hand				= LvnMouseCursor_PointingHand,
	LvnMouseCursor_Max				= 10,
};

/* Logging Types */
enum LvnLogLevel
{
	LvnLogLevel_None		= 0,
	LvnLogLevel_Trace		= 1,
	LvnLogLevel_Info		= 2,
	LvnLogLevel_Warn		= 3,
	LvnLogLevel_Error		= 4,
	LvnLogLevel_Critical	= 5,
};

namespace lvn
{
	struct AppRenderEvent;
	struct AppTickEvent;
	struct Event;
	struct EventDispatcherEvent;
	struct KeyHoldEvent;
	struct KeyPressedEvent;
	struct KeyReleasedEvent;
	struct Logger;
	struct LogMessage;
	struct LogPattern;
	struct MouseButtonPressedEvent;
	struct MouseButtonReleasedEvent;
	struct MouseMovedEvent;
	struct MouseScrolledEvent;
	struct Window;
	struct WindowDimension;
	struct WindowEvent;
	struct WindowCloseEvent;
	struct WindowInfoEvent;
	struct WindowFocusEvent;
	struct WindowFramebufferResizeEvent;
	struct WindowIconDataEvent;
	struct WindowLostFocusEvent;
	struct WindowMovedEvent;
	struct WindowResizeEvent;

	enum class EventType
	{
		None = 0,
		AppTick,
		AppRender,
		KeyPressed,
		KeyReleased,
		KeyHold,
		WindowClose,
		WindowResize,
		WindowFramebufferResize,
		WindowFocus,
		WindowLostFocus,
		WindowMoved,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled,
	};

	enum EventCategory
	{
		None = 0,
		LvnEventCategory_Application	= (1 << 0),
		LvnEventCategory_Input			= (1 << 1),
		LvnEventCategory_Keyboard		= (1 << 2),
		LvnEventCategory_Mouse			= (1 << 3),
		LvnEventCategory_MouseButton	= (1 << 4),
		LvnEventCategory_Window			= (1 << 5),
	};

	enum class WindowContext
	{
		None = 0,
		glfw,
		win32,
	};

	/* Core API Decleration */

	/* [API] */

	const char* getDateMonthName();
	const char* getDateMonthNameShort();
	const char* getDateWeekDayName();
	const char* getDateWeekDayNameShort();
	const char* getDateTimeHHMMSS();
	const char* getDateTime12HHMMSS();
	const char* getDateTimeMeridiem();
	const char* getDateTimeMeridiemLower();
	
	const char* getDateYearStr();
	const char* getDateYear02dStr();
	const char* getDateMonthNumStr();
	const char* getDateDayNumStr();
	const char* getDateHourNumStr();
	const char* getDateHour12NumStr();
	const char* getDateMinuteNumStr();
	const char* getDateSecondNumStr();

	int getDateYear();
	int getDateYear02d();
	int getDateMonth();
	int getDateDay();
	int getDateHour();
	int getDateHour12();
	int getDateMinute();
	int getDateSecond();
	long long getSecondSinceEpoch();


	/* [Logging] */
	struct LogMessage
	{
		const char *msg, *loggerName;
		LvnLogLevel level;
		long long timeEpoch;
	};
	
	struct LogPattern
	{
		char symbol;
		const char*(*func)(LogMessage*);
	};

	bool logInit();
	bool logTerminate();
	void logSetLevel(Logger* logger, LvnLogLevel level);
	bool logCheckLevel(Logger* logger, LvnLogLevel level);
	void logOutputMessage(Logger* logger, LogMessage* msg);
	Logger* getCoreLogger();
	Logger* getClientLogger();
	const char* getLogANSIcodeColor(LvnLogLevel level);
	void logSetPattern(Logger* logger, const char* pattern);

	template<typename... Args>
	void logMessage(Logger* logger, LvnLogLevel level, const char* fmt, Args... args)
	{
		if (!logCheckLevel(logger, level))
			return;

		int64_t epoch = getSecondSinceEpoch();

		int strsize = snprintf(nullptr, 0, fmt, args...) + 1;
		if (strsize <= 0)
			return;
		char* buff = (char*)malloc(strsize);
		snprintf(buff, strsize, fmt, args...);

		LogMessage logMsg = { .msg = buff, .level = level, .timeEpoch = epoch };
		logOutputMessage(logger, &logMsg);
		free(buff);
	}
	template<typename... Args>
	void logMessageTrace(Logger* logger, const char* fmt, Args... args)
	{
		logMessage(logger, LvnLogLevel_Trace, fmt, args...);
	}
	template<typename... Args>
	void logMessageInfo(Logger* logger, const char* fmt, Args... args)
	{
		logMessage(logger, LvnLogLevel_Info, fmt, args...);
	}
	template<typename... Args>
	void logMessageWarn(Logger* logger, const char* fmt, Args... args)
	{
		logMessage(logger, LvnLogLevel_Warn, fmt, args...);
	}
	template<typename... Args>
	void logMessageError(Logger* logger, const char* fmt, Args... args)
	{
		logMessage(logger, LvnLogLevel_Error, fmt, args...);
	}
	template<typename... Args>
	void logMessageCritical(Logger* logger, const char* fmt, Args... args)
	{
		logMessage(logger, LvnLogLevel_Critical, fmt, args...);
	}


	/* [Events] */
	struct Event
	{
		EventType type;
		int category;
		bool handled;
	};

	struct EventDispatcher
	{
		EventType event;
	};

	bool dispatchAppRenderEvent(Event* event, bool (*func)(AppRenderEvent*));
	bool dispatchAppTickEvent(Event* event, bool (*func)(AppTickEvent*));
	bool dispatchKeyPressedEvent(Event* event, bool (*func)(KeyPressedEvent*));
	bool dispatchKeyReleasedEvent(Event* event, bool (*func)(KeyReleasedEvent*));
	bool dispatchKeyHoldEvent(Event* event, bool (*func)(KeyHoldEvent*));
	bool dispatchWindowCloseEvent(Event* event, bool (*func)(WindowCloseEvent*));
	bool dispatchWindowResizeEvent(Event* event, bool (*func)(WindowResizeEvent*));
	bool dispatchWindowFramebufferResizeEvent(Event* event, bool (*func)(WindowFramebufferResizeEvent*));
	bool dispatchWindowFocusEvent(Event* event, bool (*func)(WindowFocusEvent*));
	bool dispatchWindowLostFocusEvent(Event* event, bool (*func)(WindowLostFocusEvent*));
	bool dispatchWindowMovedEvent(Event* event, bool (*func)(WindowMovedEvent*));
	bool dispatchMouseButtonPressedEvent(Event* event, bool (*func)(MouseButtonPressedEvent*));
	bool dispatchMouseButtonReleasedEvent(Event* event, bool (*func)(MouseButtonReleasedEvent*));
	bool dispatchMouseMovedEvent(Event* event, bool (*func)(MouseMovedEvent*));
	bool dispatchMouseScrolledEvent(Event* event, bool (*func)(MouseScrolledEvent*));


	/* [Window] */
	struct WindowIconData
	{
		unsigned char* image;
		int width, height;
	};

	struct WindowInfo
	{
		int width, height;
		const char* title;
		int minWidth, minHeight;
		int maxWidth, maxHeight;
		bool fullscreen, resizable, vSync;
		WindowIconData* pIcons;
		int iconCount;

		WindowInfo()
			: maxWidth(-1), maxHeight(-1), resizable(true) {}
	};
	typedef WindowInfo WindowCreateInfo;

	struct WindowDimension
	{
		int width, height;
	};

	bool setWindowContext(WindowContext windowContext);
	WindowContext getWindowContext();
	const char* getWindowContextName();
	bool terminateWindowContext();

	Window* createWindow(int width, int height, const char* title, bool fullscreen = false, bool resizable = true, int minWidth = 0, int minHeight = 0);
	Window* createWindow(WindowInfo winCreateInfo);

	void updateWindow(Window* window);
	bool windowOpen(Window* window);

	WindowDimension getDimensions(Window* window);
	unsigned int getWindowWidth(Window* window);
	unsigned int getWindowHeight(Window* window);

	void setWindowEventCallback(Window* window, void (*callback)(Event*));

	void setWindowVSync(Window* window, bool enabled);
	bool getWindowVSync(Window* window);

	void* getNativeWindow(Window* window);
	void setWindowContextCurrent(Window* window);

	void destroyWindow(Window* window);

	/* [API] */

}

#endif