#include "core_internal.h"

#include "Platform/Window/glfw/lvn_glfw.h"

#include <time.h>

#ifdef LVN_PLATFORM_WINDOWS
	#include <windows.h>
#endif


namespace lvn
{
	namespace coreUtils
	{
		void enableLogANSIcodeColors()
		{
			DWORD consoleMode;
			HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			if (GetConsoleMode(outputHandle, &consoleMode))
			{
				SetConsoleMode(outputHandle, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
			}
		}

		const char* getLogLevelColor(LvnLogLevel level)
		{
			switch (level)
			{
				case LvnLogLevel_None:		{ return LVN_LOG_COLOR_RESET; }
				case LvnLogLevel_Trace:		{ return LVN_LOG_COLOR_TRACE; }
				case LvnLogLevel_Info:		{ return LVN_LOG_COLOR_INFO; }
				case LvnLogLevel_Warn:		{ return LVN_LOG_COLOR_WARN; }
				case LvnLogLevel_Error:		{ return LVN_LOG_COLOR_ERROR; }
				case LvnLogLevel_Critical:	{ return LVN_LOG_COLOR_CRITICAL; }
			}

			return nullptr;
		}

		const char* getLogLevelName(LvnLogLevel level)
		{
			switch (level)
			{
				case LvnLogLevel_None:		{ return "none"; }
				case LvnLogLevel_Trace:		{ return "trace"; }
				case LvnLogLevel_Info:		{ return "info"; }
				case LvnLogLevel_Warn:		{ return "warn"; }
				case LvnLogLevel_Error:		{ return "error"; }
				case LvnLogLevel_Critical:	{ return "critical"; }
			}

			return nullptr;
		}
	}


	/* [API] */
	static const char* s_MonthName[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
	static const char* s_MonthNameShort[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	static const char* s_WeekDayName[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
	static const char* s_WeekDayNameShort[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

	/* Date Time Functions */
	const char* getDateMonthName()
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		return s_MonthName[tm.tm_mon];
	}
	const char* getDateMonthNameShort()
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		return s_MonthNameShort[tm.tm_mon];
	}
	const char* getDateWeekDayName()
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		return s_WeekDayName[tm.tm_wday];
	}
	const char* getDateWeekDayNameShort()
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		return s_WeekDayNameShort[tm.tm_wday];
	}
	LvnString getDateTimeHHMMSS()
	{
		LvnString str(9);
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		snprintf(str, 9, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
		return str;
	}
	LvnString getDateTime12HHMMSS()
	{
		LvnString str(9);
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		snprintf(str, 9, "%02d:%02d:%02d", ((tm.tm_hour + 11) % 12) + 1, tm.tm_min, tm.tm_sec);
		return str;
	}
	const char* getDateTimeMeridiem()
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		if (tm.tm_hour < 12)
			return "AM";
		else
			return "PM";
	}
	const char* getDateTimeMeridiemLower()
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		if (tm.tm_hour < 12)
			return "am";
		else
			return "pm";
	}

	LvnString getDateYearStr()
	{
		LvnString str(5);
		snprintf(str, 5, "%d", getDateYear());
		return str;
	}
	LvnString getDateYear02dStr()
	{
		LvnString str(3);
		snprintf(str, 3, "%d", getDateYear02d());
		return str;
	}
	LvnString getDateMonthNumStr()
	{
		LvnString str(3);
		snprintf(str, 3, "%02d", getDateMonth());
		return str;
	}
	LvnString getDateDayNumStr()
	{
		LvnString str(3);
		snprintf(str, 3, "%02d", getDateDay());
		return str;
	}
	LvnString getDateHourNumStr()
	{
		LvnString str(3);
		snprintf(str, 3, "%02d", getDateHour());
		return str;
	}
	LvnString getDateHour12NumStr()
	{
		LvnString str(3);
		snprintf(str, 3, "%02d", getDateHour12());
		return str;
	}
	LvnString getDateMinuteNumStr()
	{
		LvnString str(3);
		snprintf(str, 3, "%02d", getDateMinute());
		return str;
	}
	LvnString getDateSecondNumStr()
	{
		LvnString str(3);
		snprintf(str, 3, "%02d", getDateSecond());
		return str;
	}

	int getDateYear()
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		return tm.tm_year + 1900;
	}
	int getDateYear02d()
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		return (tm.tm_year + 1900) % 100;
	}
	int getDateMonth()
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		return tm.tm_mon + 1;
	}
	int getDateDay()
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		return tm.tm_mday;
	}
	int getDateHour()
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		return tm.tm_hour;
	}
	int getDateHour12()
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		return ((tm.tm_hour + 11) % 12) + 1;
	}
	int getDateMinute()
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		return tm.tm_min;
	}
	int getDateSecond()
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		return tm.tm_sec;
	}
	long long getSecondSinceEpoch()
	{
		return time(NULL);
	}

	/* [Logging] */
	static bool s_LoggingInit = false;
	static Logger s_CoreLogger;
	static Logger s_ClientLogger;
	const static LogPattern s_LogPatterns[23] =
	{
		{ '$', [](LogMessage* msg) -> LvnString { return "\n"; } },
		{ 'n', [](LogMessage* msg) -> LvnString { return msg->loggerName; } },
		{ 'l', [](LogMessage* msg) -> LvnString { return coreUtils::getLogLevelName(msg->level); }},
		{ '#', [](LogMessage* msg) -> LvnString { return coreUtils::getLogLevelColor(msg->level); }},
		{ '^', [](LogMessage* msg) -> LvnString { return LVN_LOG_COLOR_RESET; }},
		{ 'v', [](LogMessage* msg) -> LvnString { return msg->msg; }},
		{ '%', [](LogMessage* msg) -> LvnString { return "%"; } },
		{ 'T', [](LogMessage* msg) -> LvnString { return getDateTimeHHMMSS(); } },
		{ 't', [](LogMessage* msg) -> LvnString { return getDateTime12HHMMSS(); } },
		{ 'Y', [](LogMessage* msg) -> LvnString { return getDateYearStr(); }},
		{ 'y', [](LogMessage* msg) -> LvnString { return getDateYear02dStr(); } },
		{ 'm', [](LogMessage* msg) -> LvnString { return getDateMonthNumStr(); } },
		{ 'B', [](LogMessage* msg) -> LvnString { return getDateMonthName(); } },
		{ 'b', [](LogMessage* msg) -> LvnString { return getDateMonthNameShort(); } },
		{ 'd', [](LogMessage* msg) -> LvnString { return getDateDayNumStr(); } },
		{ 'A', [](LogMessage* msg) -> LvnString { return getDateWeekDayName(); } },
		{ 'a', [](LogMessage* msg) -> LvnString { return getDateWeekDayNameShort(); } },
		{ 'H', [](LogMessage* msg) -> LvnString { return getDateHourNumStr(); } },
		{ 'h', [](LogMessage* msg) -> LvnString { return getDateHour12NumStr(); } },
		{ 'M', [](LogMessage* msg) -> LvnString { return getDateMinuteNumStr(); } },
		{ 'S', [](LogMessage* msg) -> LvnString { return getDateSecondNumStr(); } },
		{ 'P', [](LogMessage* msg) -> LvnString { return getDateTimeMeridiem(); } },
		{ 'p', [](LogMessage* msg) -> LvnString { return getDateTimeMeridiemLower(); }},
	};

	void logParseFormat(const char* fmt, LogPattern** pLogPatterns, uint32_t* logPatternCount)
	{
		if (!fmt || !strlen(fmt))
			return;

		LogPattern* patterns = static_cast<LogPattern*>(malloc(0));
		uint32_t patternCount = 0;

		for (uint32_t i = 0; i < strlen(fmt) - 1; i++)
		{
			if (fmt[i] != '%') // Other characters in format
			{
				LogPattern pattern = { .symbol = fmt[i], .func = nullptr };
				LogPattern* newPattern = static_cast<LogPattern*>(realloc(patterns, ++patternCount * sizeof(LogPattern)));
				if (!newPattern) return;
				memcpy(&newPattern[patternCount - 1], &pattern, sizeof(LogPattern));
				patterns = newPattern;

				continue;
			}
			
			// find pattern with matching symbol
			for (uint32_t j = 0; j < sizeof(s_LogPatterns) / sizeof(LogPattern); j++)
			{
				if (fmt[i + 1] != s_LogPatterns[j].symbol)
					continue;

				LogPattern* newPattern = static_cast<LogPattern*>(realloc(patterns, ++patternCount * sizeof(LogPattern)));
				if (!newPattern) return;
				memcpy(&newPattern[patternCount - 1], &s_LogPatterns[j], sizeof(LogPattern));
				patterns = newPattern;
			}

			i++; // incramant past symbol on next character in format
		}

		*pLogPatterns = patterns;
		*logPatternCount = patternCount;
	}

	bool logInit()
	{
		if (!s_LoggingInit)
		{
			s_LoggingInit = true;

			s_CoreLogger.loggerName = "CORE";
			s_ClientLogger.loggerName = "APP";
			s_CoreLogger.logLevel = LvnLogLevel_None;
			s_ClientLogger.logLevel = LvnLogLevel_None;
			s_CoreLogger.logPatternFormat = "%#[%T] [%l] %n: %v%^%$";
			s_ClientLogger.logPatternFormat = "%#[%T] [%l] %n: %v%^%$";

			LogPattern* logPatterns = nullptr;
			uint32_t logPatternCount = 0;
			logParseFormat("%#[%T] [%l] %n: %v%^%$", &logPatterns, &logPatternCount);

			s_CoreLogger.pLogPatterns = logPatterns;
			s_CoreLogger.logPatternCount = logPatternCount;
			s_ClientLogger.pLogPatterns = logPatterns;
			s_ClientLogger.logPatternCount = logPatternCount;

			#ifdef LVN_PLATFORM_WINDOWS 
			coreUtils::enableLogANSIcodeColors();
			#endif

			return true;
		}
		
		return false;
	}

	bool logTerminate()
	{
		if (s_LoggingInit)
		{
			s_LoggingInit = false;

			// freed once since both Core and Client loggers share the same memory to pLogPatterns
			if (s_CoreLogger.pLogPatterns)
				free(s_CoreLogger.pLogPatterns);

			return true;
		}

		return false;
	}

	void logSetLevel(Logger* logger, LvnLogLevel level)
	{
		logger->logLevel = level;
	}

	bool logCheckLevel(Logger* logger, LvnLogLevel level)
	{
		return (level >= logger->logLevel);
	}

	void logOutputMessage(Logger* logger, LogMessage* msg)
	{
		if (!s_LoggingInit) return;

		msg->loggerName = logger->loggerName;
		char* dst = (char*)malloc(0); if (!dst) return;
		uint32_t dstsize = 0;

		for (uint32_t i = 0; i < logger->logPatternCount; i++)
		{
			if (logger->pLogPatterns[i].func == nullptr) // no special format character '%' found
			{
				char* strnew = (char*)realloc(dst, ++dstsize); if (!strnew) return;
				strnew[dstsize - 1] = logger->pLogPatterns[i].symbol; /* msvc throws warning for buffer overflow because we replace the */
				dst = strnew;										  /* last index of string with our char symbol instead of null terminator */
			}
			else // call func of special format
			{
				LvnString fmtstr = logger->pLogPatterns[i].func(msg);
				uint32_t fmtstrlen = strlen(fmtstr);
				dstsize += fmtstrlen;
				char* strnew = (char*)realloc(dst, dstsize); if (!strnew) return;
				memcpy(&strnew[dstsize - fmtstrlen], fmtstr, fmtstrlen);
				dst = strnew;
			}
		}

		printf("%.*s", dstsize, dst);
		free(dst);
	}

	Logger* getCoreLogger()
	{
		return &s_CoreLogger;
	}

	Logger* getClientLogger()
	{
		return &s_ClientLogger;
	}

	const char* getLogANSIcodeColor(LvnLogLevel level)
	{
		switch (level)
		{
			case LvnLogLevel_None:		{ return LVN_LOG_COLOR_RESET;	}
			case LvnLogLevel_Trace:		{ return LVN_LOG_COLOR_TRACE;	}
			case LvnLogLevel_Info:		{ return LVN_LOG_COLOR_INFO;	}
			case LvnLogLevel_Warn:		{ return LVN_LOG_COLOR_WARN;	}
			case LvnLogLevel_Error:		{ return LVN_LOG_COLOR_ERROR;	}
			case LvnLogLevel_Critical:	{ return LVN_LOG_COLOR_CRITICAL;}
		}

		return nullptr;
	}

	void logSetPattern(Logger* logger, const char* pattern)
	{
		
	}


	/* [Events] */
	bool dispatchAppRenderEvent(Event* event, bool(*func)(AppRenderEvent*))
	{
		if (event->type == EventType::AppRender)
		{
			AppRenderEvent eventType{};
			eventType.type = EventType::AppRender;
			eventType.category = LvnEventCategory_Application;
			eventType.name = "AppRenderEvent";
			eventType.handled = false;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchAppTickEvent(Event* event, bool(*func)(AppTickEvent*))
	{
		if (event->type == EventType::AppTick)
		{
			AppTickEvent eventType{};
			eventType.type = EventType::AppTick;
			eventType.category = LvnEventCategory_Application;
			eventType.name = "AppTickEvent";
			eventType.handled = false;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchKeyHoldEvent(Event* event, bool(*func)(KeyHoldEvent*))
	{
		if (event->type == EventType::KeyHold)
		{
			KeyHoldEvent eventType{};
			eventType.type = EventType::KeyHold;
			eventType.category = LvnEventCategory_Input | LvnEventCategory_Keyboard;
			eventType.name = "KeyHoldEvent";
			eventType.handled = false;
			eventType.keyCode = event->data.code;
			eventType.repeat = event->data.repeat;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchKeyPressedEvent(Event* event, bool(*func)(KeyPressedEvent*))
	{
		if (event->type == EventType::KeyPressed)
		{
			KeyPressedEvent eventType{};
			eventType.type = EventType::KeyPressed;
			eventType.category = LvnEventCategory_Input | LvnEventCategory_Keyboard;
			eventType.name = "KeyPressedEvent";
			eventType.handled = false;
			eventType.keyCode = event->data.code;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchKeyReleasedEvent(Event* event, bool(*func)(KeyReleasedEvent*))
	{
		if (event->type == EventType::KeyReleased)
		{
			KeyReleasedEvent eventType{};
			eventType.type = EventType::KeyReleased;
			eventType.category = LvnEventCategory_Input | LvnEventCategory_Keyboard;
			eventType.name = "KeyReleasedEvent";
			eventType.handled = false;
			eventType.keyCode = event->data.code;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchKeyTypedEvent(Event* event, bool(*func)(KeyTypedEvent*))
	{
		if (event->type == EventType::KeyTyped)
		{
			KeyTypedEvent eventType{};
			eventType.type = EventType::KeyTyped;
			eventType.category = LvnEventCategory_Input | LvnEventCategory_Keyboard;
			eventType.name = "KeyTypedEvent";
			eventType.handled = false;
			eventType.key = event->data.ucode;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchMouseButtonPressedEvent(Event* event, bool(*func)(MouseButtonPressedEvent*))
	{
		if (event->type == EventType::MouseButtonPressed)
		{
			MouseButtonPressedEvent eventType{};
			eventType.type = EventType::MouseButtonPressed;
			eventType.category = LvnEventCategory_Input | LvnEventCategory_MouseButton | LvnEventCategory_Mouse;
			eventType.name = "MouseButtonPressedEvent";
			eventType.handled = false;
			eventType.buttonCode = event->data.code;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchMouseButtonReleasedEvent(Event* event, bool(*func)(MouseButtonReleasedEvent*))
	{
		if (event->type == EventType::MouseButtonReleased)
		{
			MouseButtonReleasedEvent eventType{};
			eventType.type = EventType::MouseButtonReleased;
			eventType.category = LvnEventCategory_Input | LvnEventCategory_MouseButton | LvnEventCategory_Mouse;
			eventType.name = "MouseButtonReleasedEvent";
			eventType.handled = false;
			eventType.buttonCode = event->data.code;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchMouseMovedEvent(Event* event, bool(*func)(MouseMovedEvent*))
	{
		if (event->type == EventType::MouseMoved)
		{
			MouseMovedEvent eventType{};
			eventType.type = EventType::MouseMoved;
			eventType.category = LvnEventCategory_Input | LvnEventCategory_Mouse;
			eventType.name = "MouseMovedEvent";
			eventType.handled = false;
			eventType.x = event->data.xd;
			eventType.y = event->data.yd;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchMouseScrolledEvent(Event* event, bool(*func)(MouseScrolledEvent*))
	{
		if (event->type == EventType::MouseScrolled)
		{
			MouseScrolledEvent eventType{};
			eventType.type = EventType::MouseScrolled;
			eventType.category = LvnEventCategory_Input | LvnEventCategory_MouseButton | LvnEventCategory_Mouse;
			eventType.name = "MouseScrolledEvent";
			eventType.handled = false;
			eventType.x = static_cast<float>(event->data.xd);
			eventType.y = static_cast<float>(event->data.yd);

			return func(&eventType);
		}

		return false;
	}
	bool dispatchWindowCloseEvent(Event* event, bool(*func)(WindowCloseEvent*))
	{
		if (event->type == EventType::WindowClose)
		{
			WindowCloseEvent eventType{};
			eventType.type = EventType::WindowClose;
			eventType.category = LvnEventCategory_Window;
			eventType.name = "WindowCloseEvent";
			eventType.handled = false;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchWindowFocusEvent(Event* event, bool(*func)(WindowFocusEvent*))
	{
		if (event->type == EventType::WindowFocus)
		{
			WindowFocusEvent eventType{};
			eventType.type = EventType::WindowFocus;
			eventType.category = LvnEventCategory_Window;
			eventType.name = "WindowFocusEvent";
			eventType.handled = false;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchWindowFramebufferResizeEvent(Event* event, bool(*func)(WindowFramebufferResizeEvent*))
	{
		if (event->type == EventType::WindowFramebufferResize)
		{
			WindowFramebufferResizeEvent eventType{};
			eventType.type = EventType::WindowFramebufferResize;
			eventType.category = LvnEventCategory_Window;
			eventType.name = "WindowFramebufferResizeEvent";
			eventType.handled = false;
			eventType.width = event->data.x;
			eventType.height = event->data.y;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchWindowLostFocusEvent(Event* event, bool(*func)(WindowLostFocusEvent*))
	{
		if (event->type == EventType::WindowLostFocus)
		{
			WindowLostFocusEvent eventType{};
			eventType.type = EventType::WindowLostFocus;
			eventType.category = LvnEventCategory_Window;
			eventType.name = "WindowLostFocusEvent";
			eventType.handled = false;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchWindowMovedEvent(Event* event, bool(*func)(WindowMovedEvent*))
	{
		if (event->type == lvn::EventType::WindowMoved)
		{
			WindowMovedEvent eventType{};
			eventType.type = EventType::WindowMoved;
			eventType.category = LvnEventCategory_Window;
			eventType.name = "WindowMovedEvent";
			eventType.handled = false;
			eventType.x = event->data.x;
			eventType.y = event->data.y;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchWindowResizeEvent(Event* event, bool(*func)(WindowResizeEvent*))
	{

		if (event->type == lvn::EventType::WindowResize)
		{
			WindowResizeEvent eventType{};
			eventType.type = EventType::WindowResize;
			eventType.category = LvnEventCategory_Window;
			eventType.name = "WindowResizeEvent";
			eventType.handled = false;
			eventType.width = event->data.x;
			eventType.height = event->data.y;

			return func(&eventType);
		}

		return false;
	}


	/* [Window] */
	static WindowContext* s_WindowContext = nullptr;

	bool createWindowContext(WindowAPI windowapi)
	{
		switch (windowapi)
		{
			case WindowAPI::None:
			{
				LVN_CORE_WARN("setting Window API to none, no Windows API selected!");
				return false;
			}
			case WindowAPI::glfw:
			{
				s_WindowContext = new WindowContext();
				s_WindowContext->windowapi = WindowAPI::glfw;
				glfwImplInitWindowContext(s_WindowContext);
				break;
			}
			case WindowAPI::win32:
			{

				break;
			}
		}

		//windowInputInit();

		LVN_CORE_INFO("window context set: %s", getWindowAPIName());
		return true;
	}

	bool terminateWindowContext()
	{
		switch (s_WindowContext->windowapi)
		{
			case WindowAPI::None:
			{
				LVN_CORE_WARN("no window API Initialized! Cannot terminate window API!");
				return false;
			}
			case WindowAPI::glfw:
			{
				glfwImplTerminateWindowContext();
				delete s_WindowContext;
				break;
			}
			case WindowAPI::win32:
			{

				break;
			}
			default:
			{
				LVN_CORE_ERROR("unknown Windows API selected! Cannot terminate window API!");
				return false;
			}
		}

		LVN_CORE_INFO("window context terminated");
		return true;
	}

	WindowAPI getWindowAPI()
	{
		return s_WindowContext->windowapi;
	}

	const char* getWindowAPIName()
	{
		switch (s_WindowContext->windowapi)
		{
			case WindowAPI::None: { return "None"; }
			case WindowAPI::glfw: { return "glfw"; }
			case WindowAPI::win32: { return "win32"; }
		}

		LVN_CORE_ERROR("Unknown Windows API selected!");
		return nullptr;
	}

	Window* createWindow(int width, int height, const char* title, bool fullscreen, bool resizable, int minWidth, int minHeight)
	{
		if (width * height < 0)
		{
			LVN_CORE_ERROR("Cannot create window with negative dimensions! (x:%d,y:%d)", width, height);
			return nullptr;
		}

		return s_WindowContext->createWindow(width, height, title, fullscreen, resizable, minWidth, minHeight);
	}

	Window* createWindow(WindowCreateInfo* winCreateInfo)
	{
		if (winCreateInfo->width * winCreateInfo->height < 0)
		{
			LVN_CORE_ERROR("Cannot create window with negative dimensions! (w:%d,h:%d)", winCreateInfo->width, winCreateInfo->height);
			return nullptr;
		}

		return s_WindowContext->createWindowInfo(winCreateInfo);
	}

	void updateWindow(Window* window)
	{
		s_WindowContext->updateWindow(window);
	}

	bool windowOpen(Window* window)
	{
		return s_WindowContext->windowOpen(window);
	}

	WindowDimension getWindowDimensions(Window* window)
	{
		return s_WindowContext->getDimensions(window);
	}

	int getWindowWidth(Window* window)
	{
		return s_WindowContext->getWindowWidth(window);
	}

	int getWindowHeight(Window* window)
	{
		return s_WindowContext->getWindowHeight(window);
	}

	void setWindowEventCallback(Window* window, void (*callback)(Event*))
	{
		window->data.eventCallBackFn = callback;
	}

	void setWindowVSync(Window* window, bool enable)
	{
		s_WindowContext->setWindowVSync(window, enable);
	}

	bool getWindowVSync(Window* window)
	{
		return s_WindowContext->getWindowVSync(window);
	}

	void* getNativeWindow(Window* window)
	{
		return window->nativeWindow;
	}

	void setWindowContextCurrent(Window* window)
	{
		s_WindowContext->setWindowContextCurrent(window);
	}

	void destroyWindow(Window* window)
	{
		s_WindowContext->destroyWindow(window);
	}
}