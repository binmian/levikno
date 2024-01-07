#include "core_internal.h"

#include "Platform/Window/glfw/lvn_glfw.h"

#include <time.h>

#ifdef LVN_PLATFORM_WINDOWS
	#include <windows.h>
#endif


namespace lvn
{
	static void enableLogANSIcodeColors();
	static const char* getLogLevelColor(LvnLogLevel level);
	static const char* getLogLevelName(LvnLogLevel level);
	static void logParseFormat(const char* fmt, LvnLogPattern** pLogPatterns, uint32_t* logPatternCount);


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
			case Lvn_LogLevel_None:		{ return LVN_LOG_COLOR_RESET; }
			case Lvn_LogLevel_Trace:	{ return LVN_LOG_COLOR_TRACE; }
			case Lvn_LogLevel_Info:		{ return LVN_LOG_COLOR_INFO; }
			case Lvn_LogLevel_Warn:		{ return LVN_LOG_COLOR_WARN; }
			case Lvn_LogLevel_Error:	{ return LVN_LOG_COLOR_ERROR; }
			case Lvn_LogLevel_Critical:	{ return LVN_LOG_COLOR_CRITICAL; }
		}

		return nullptr;
	}

	static const char* getLogLevelName(LvnLogLevel level)
	{
		switch (level)
		{
			case Lvn_LogLevel_None:		{ return "none"; }
			case Lvn_LogLevel_Trace:	{ return "trace"; }
			case Lvn_LogLevel_Info:		{ return "info"; }
			case Lvn_LogLevel_Warn:		{ return "warn"; }
			case Lvn_LogLevel_Error:	{ return "error"; }
			case Lvn_LogLevel_Critical:	{ return "critical"; }
		}

		return nullptr;
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
	static LvnLogger s_CoreLogger;
	static LvnLogger s_ClientLogger;
	const static LvnLogPattern s_LogPatterns[23] =
	{
		{ '$', [](LvnLogMessage* msg) -> LvnString { return "\n"; } },
		{ 'n', [](LvnLogMessage* msg) -> LvnString { return msg->loggerName; } },
		{ 'l', [](LvnLogMessage* msg) -> LvnString { return getLogLevelName(msg->level); }},
		{ '#', [](LvnLogMessage* msg) -> LvnString { return getLogLevelColor(msg->level); }},
		{ '^', [](LvnLogMessage* msg) -> LvnString { return LVN_LOG_COLOR_RESET; }},
		{ 'v', [](LvnLogMessage* msg) -> LvnString { return msg->msg; }},
		{ '%', [](LvnLogMessage* msg) -> LvnString { return "%"; } },
		{ 'T', [](LvnLogMessage* msg) -> LvnString { return getDateTimeHHMMSS(); } },
		{ 't', [](LvnLogMessage* msg) -> LvnString { return getDateTime12HHMMSS(); } },
		{ 'Y', [](LvnLogMessage* msg) -> LvnString { return getDateYearStr(); }},
		{ 'y', [](LvnLogMessage* msg) -> LvnString { return getDateYear02dStr(); } },
		{ 'm', [](LvnLogMessage* msg) -> LvnString { return getDateMonthNumStr(); } },
		{ 'B', [](LvnLogMessage* msg) -> LvnString { return getDateMonthName(); } },
		{ 'b', [](LvnLogMessage* msg) -> LvnString { return getDateMonthNameShort(); } },
		{ 'd', [](LvnLogMessage* msg) -> LvnString { return getDateDayNumStr(); } },
		{ 'A', [](LvnLogMessage* msg) -> LvnString { return getDateWeekDayName(); } },
		{ 'a', [](LvnLogMessage* msg) -> LvnString { return getDateWeekDayNameShort(); } },
		{ 'H', [](LvnLogMessage* msg) -> LvnString { return getDateHourNumStr(); } },
		{ 'h', [](LvnLogMessage* msg) -> LvnString { return getDateHour12NumStr(); } },
		{ 'M', [](LvnLogMessage* msg) -> LvnString { return getDateMinuteNumStr(); } },
		{ 'S', [](LvnLogMessage* msg) -> LvnString { return getDateSecondNumStr(); } },
		{ 'P', [](LvnLogMessage* msg) -> LvnString { return getDateTimeMeridiem(); } },
		{ 'p', [](LvnLogMessage* msg) -> LvnString { return getDateTimeMeridiemLower(); }},
	};

	static void logParseFormat(const char* fmt, LvnLogPattern** pLogPatterns, uint32_t* logPatternCount)
	{
		if (!fmt || !strlen(fmt))
			return;

		LvnLogPattern* patterns = static_cast<LvnLogPattern*>(malloc(0));
		uint32_t patternCount = 0;

		for (uint32_t i = 0; i < strlen(fmt) - 1; i++)
		{
			if (fmt[i] != '%') // Other characters in format
			{
				LvnLogPattern pattern = { .symbol = fmt[i], .func = nullptr };
				LvnLogPattern* newPattern = static_cast<LvnLogPattern*>(realloc(patterns, ++patternCount * sizeof(LvnLogPattern)));
				if (!newPattern) return;
				memcpy(&newPattern[patternCount - 1], &pattern, sizeof(LvnLogPattern));
				patterns = newPattern;

				continue;
			}

			// find pattern with matching symbol
			for (uint32_t j = 0; j < sizeof(s_LogPatterns) / sizeof(LvnLogPattern); j++)
			{
				if (fmt[i + 1] != s_LogPatterns[j].symbol)
					continue;

				LvnLogPattern* newPattern = static_cast<LvnLogPattern*>(realloc(patterns, ++patternCount * sizeof(LvnLogPattern)));
				if (!newPattern) return;
				memcpy(&newPattern[patternCount - 1], &s_LogPatterns[j], sizeof(LvnLogPattern));
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
			s_CoreLogger.logLevel = Lvn_LogLevel_None;
			s_ClientLogger.logLevel = Lvn_LogLevel_None;
			s_CoreLogger.logPatternFormat = "%#[%T] [%l] %n: %v%^%$";
			s_ClientLogger.logPatternFormat = "%#[%T] [%l] %n: %v%^%$";

			LvnLogPattern* logPatterns = nullptr;
			uint32_t logPatternCount = 0;
			logParseFormat("%#[%T] [%l] %n: %v%^%$", &logPatterns, &logPatternCount);

			s_CoreLogger.pLogPatterns = logPatterns;
			s_CoreLogger.logPatternCount = logPatternCount;
			s_ClientLogger.pLogPatterns = logPatterns;
			s_ClientLogger.logPatternCount = logPatternCount;

			#ifdef LVN_PLATFORM_WINDOWS 
			enableLogANSIcodeColors();
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

	void logSetLevel(LvnLogger* logger, LvnLogLevel level)
	{
		logger->logLevel = level;
	}

	bool logCheckLevel(LvnLogger* logger, LvnLogLevel level)
	{
		return (level >= logger->logLevel);
	}

	void logOutputMessage(LvnLogger* logger, LvnLogMessage* msg)
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

	LvnLogger* getCoreLogger()
	{
		return &s_CoreLogger;
	}

	LvnLogger* getClientLogger()
	{
		return &s_ClientLogger;
	}

	const char* getLogANSIcodeColor(LvnLogLevel level)
	{
		switch (level)
		{
			case Lvn_LogLevel_None:		{ return LVN_LOG_COLOR_RESET;	}
			case Lvn_LogLevel_Trace:	{ return LVN_LOG_COLOR_TRACE;	}
			case Lvn_LogLevel_Info:		{ return LVN_LOG_COLOR_INFO;	}
			case Lvn_LogLevel_Warn:		{ return LVN_LOG_COLOR_WARN;	}
			case Lvn_LogLevel_Error:	{ return LVN_LOG_COLOR_ERROR;	}
			case Lvn_LogLevel_Critical:	{ return LVN_LOG_COLOR_CRITICAL;}
		}

		return nullptr;
	}

	void logSetPattern(LvnLogger* logger, const char* pattern)
	{
		
	}


	/* [Events] */
	bool dispatchLvnAppRenderEvent(LvnEvent* event, bool(*func)(LvnAppRenderEvent*))
	{
		if (event->type == Lvn_EventType_AppRender)
		{
			LvnAppRenderEvent eventType{};
			eventType.type = Lvn_EventType_AppRender;
			eventType.category = Lvn_EventCategory_Application;
			eventType.name = "LvnAppRenderEvent";
			eventType.handled = false;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchLvnAppTickEvent(LvnEvent* event, bool(*func)(LvnAppTickEvent*))
	{
		if (event->type == Lvn_EventType_AppTick)
		{
			LvnAppTickEvent eventType{};
			eventType.type = Lvn_EventType_AppTick;
			eventType.category = Lvn_EventCategory_Application;
			eventType.name = "LvnAppTickEvent";
			eventType.handled = false;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchKeyHoldEvent(LvnEvent* event, bool(*func)(LvnKeyHoldEvent*))
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

			return func(&eventType);
		}

		return false;
	}
	bool dispatchKeyPressedEvent(LvnEvent* event, bool(*func)(LvnKeyPressedEvent*))
	{
		if (event->type == Lvn_EventType_KeyPressed)
		{
			LvnKeyPressedEvent eventType{};
			eventType.type = Lvn_EventType_KeyPressed;
			eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
			eventType.name = "LvnKeyPressedEvent";
			eventType.handled = false;
			eventType.keyCode = event->data.code;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchKeyReleasedEvent(LvnEvent* event, bool(*func)(LvnKeyReleasedEvent*))
	{
		if (event->type == Lvn_EventType_KeyReleased)
		{
			LvnKeyReleasedEvent eventType{};
			eventType.type = Lvn_EventType_KeyReleased;
			eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
			eventType.name = "LvnKeyReleasedEvent";
			eventType.handled = false;
			eventType.keyCode = event->data.code;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchKeyTypedEvent(LvnEvent* event, bool(*func)(LvnKeyTypedEvent*))
	{
		if (event->type == Lvn_EventType_KeyTyped)
		{
			LvnKeyTypedEvent eventType{};
			eventType.type = Lvn_EventType_KeyTyped;
			eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
			eventType.name = "LvnKeyTypedEvent";
			eventType.handled = false;
			eventType.key = event->data.ucode;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchMouseButtonPressedEvent(LvnEvent* event, bool(*func)(LvnMouseButtonPressedEvent*))
	{
		if (event->type == Lvn_EventType_MouseButtonPressed)
		{
			LvnMouseButtonPressedEvent eventType{};
			eventType.type = Lvn_EventType_MouseButtonPressed;
			eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_MouseButton | Lvn_EventCategory_Mouse;
			eventType.name = "LvnMouseButtonPressedEvent";
			eventType.handled = false;
			eventType.buttonCode = event->data.code;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchMouseButtonReleasedEvent(LvnEvent* event, bool(*func)(LvnMouseButtonReleasedEvent*))
	{
		if (event->type == Lvn_EventType_MouseButtonReleased)
		{
			LvnMouseButtonReleasedEvent eventType{};
			eventType.type = Lvn_EventType_MouseButtonReleased;
			eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_MouseButton | Lvn_EventCategory_Mouse;
			eventType.name = "LvnMouseButtonReleasedEvent";
			eventType.handled = false;
			eventType.buttonCode = event->data.code;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchMouseMovedEvent(LvnEvent* event, bool(*func)(LvnMouseMovedEvent*))
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

			return func(&eventType);
		}

		return false;
	}
	bool dispatchMouseScrolledEvent(LvnEvent* event, bool(*func)(LvnMouseScrolledEvent*))
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

			return func(&eventType);
		}

		return false;
	}
	bool dispatchWindowCloseEvent(LvnEvent* event, bool(*func)(LvnWindowCloseEvent*))
	{
		if (event->type == Lvn_EventType_WindowClose)
		{
			LvnWindowCloseEvent eventType{};
			eventType.type = Lvn_EventType_WindowClose;
			eventType.category = Lvn_EventCategory_Window;
			eventType.name = "LvnWindowCloseEvent";
			eventType.handled = false;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchWindowFocusEvent(LvnEvent* event, bool(*func)(LvnWindowFocusEvent*))
	{
		if (event->type == Lvn_EventType_WindowFocus)
		{
			LvnWindowFocusEvent eventType{};
			eventType.type = Lvn_EventType_WindowFocus;
			eventType.category = Lvn_EventCategory_Window;
			eventType.name = "LvnWindowFocusEvent";
			eventType.handled = false;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchWindowFramebufferResizeEvent(LvnEvent* event, bool(*func)(LvnWindowFramebufferResizeEvent*))
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

			return func(&eventType);
		}

		return false;
	}
	bool dispatchWindowLostFocusEvent(LvnEvent* event, bool(*func)(LvnWindowLostFocusEvent*))
	{
		if (event->type == Lvn_EventType_WindowLostFocus)
		{
			LvnWindowLostFocusEvent eventType{};
			eventType.type = Lvn_EventType_WindowLostFocus;
			eventType.category = Lvn_EventCategory_Window;
			eventType.name = "LvnWindowLostFocusEvent";
			eventType.handled = false;

			return func(&eventType);
		}

		return false;
	}
	bool dispatchWindowMovedEvent(LvnEvent* event, bool(*func)(LvnWindowMovedEvent*))
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

			return func(&eventType);
		}

		return false;
	}
	bool dispatchWindowResizeEvent(LvnEvent* event, bool(*func)(LvnWindowResizeEvent*))
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

			return func(&eventType);
		}

		return false;
	}


	/* [Window] */
	static LvnWindowContext* s_WindowContext = nullptr;

	bool createWindowContext(LvnWindowApi windowapi)
	{
		switch (windowapi)
		{
			case Lvn_WindowApi_None:
			{
				LVN_CORE_WARN("setting Window API to none, no Windows API selected!");
				return false;
			}
			case Lvn_WindowApi_glfw:
			{
				s_WindowContext = new LvnWindowContext();
				s_WindowContext->windowapi = Lvn_WindowApi_glfw;
				glfwImplInitWindowContext(s_WindowContext);
				break;
			}
			case Lvn_WindowApi_win32:
			{

				break;
			}
		}

		//windowInputInit();

		LVN_CORE_INFO("window context set: %s", getWindowApiName());
		return true;
	}

	bool terminateWindowContext()
	{
		switch (s_WindowContext->windowapi)
		{
			case Lvn_WindowApi_None:
			{
				LVN_CORE_WARN("no window API Initialized! Cannot terminate window API!");
				return false;
			}
			case Lvn_WindowApi_glfw:
			{
				glfwImplTerminateWindowContext();
				delete s_WindowContext;
				break;
			}
			case Lvn_WindowApi_win32:
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

	LvnWindowApi getWindowApi()
	{
		return s_WindowContext->windowapi;
	}

	const char* getWindowApiName()
	{
		switch (s_WindowContext->windowapi)
		{
			case Lvn_WindowApi_None: { return "None"; }
			case Lvn_WindowApi_glfw: { return "glfw"; }
			case Lvn_WindowApi_win32: { return "win32"; }
		}

		LVN_CORE_ERROR("Unknown Windows API selected!");
		return nullptr;
	}

	LvnWindow* createWindow(int width, int height, const char* title, bool fullscreen, bool resizable, int minWidth, int minHeight)
	{
		if (width * height < 0)
		{
			LVN_CORE_ERROR("Cannot create window with negative dimensions! (x:%d,y:%d)", width, height);
			return nullptr;
		}

		return s_WindowContext->createWindow(width, height, title, fullscreen, resizable, minWidth, minHeight);
	}

	LvnWindow* createWindow(LvnWindowCreateInfo* winCreateInfo)
	{
		if (winCreateInfo->width * winCreateInfo->height < 0)
		{
			LVN_CORE_ERROR("Cannot create window with negative dimensions! (w:%d,h:%d)", winCreateInfo->width, winCreateInfo->height);
			return nullptr;
		}

		return s_WindowContext->createWindowInfo(winCreateInfo);
	}

	void updateWindow(LvnWindow* window)
	{
		s_WindowContext->updateWindow(window);
	}

	bool windowOpen(LvnWindow* window)
	{
		return s_WindowContext->windowOpen(window);
	}

	LvnWindowDimension getWindowDimensions(LvnWindow* window)
	{
		return s_WindowContext->getDimensions(window);
	}

	int getWindowWidth(LvnWindow* window)
	{
		return s_WindowContext->getWindowWidth(window);
	}

	int getWindowHeight(LvnWindow* window)
	{
		return s_WindowContext->getWindowHeight(window);
	}

	void setWindowEventCallback(LvnWindow* window, void (*callback)(LvnEvent*))
	{
		window->data.eventCallBackFn = callback;
	}

	void setWindowVSync(LvnWindow* window, bool enable)
	{
		s_WindowContext->setWindowVSync(window, enable);
	}

	bool getWindowVSync(LvnWindow* window)
	{
		return s_WindowContext->getWindowVSync(window);
	}

	void* getNativeWindow(LvnWindow* window)
	{
		return window->nativeWindow;
	}

	void setWindowContextCurrent(LvnWindow* window)
	{
		s_WindowContext->setWindowContextCurrent(window);
	}

	void destroyWindow(LvnWindow* window)
	{
		s_WindowContext->destroyWindow(window);
	}
}