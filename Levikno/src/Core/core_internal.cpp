#include "core_internal.h"

#include "Platform/Window/glfw/lvn_glfw.h"

#include <time.h>

namespace lvn
{
	namespace coreUtils
	{
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

	static LvnDateTimeStringContainer s_DateTimeStrings;
	static const char* s_MonthName[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
	static const char* s_MonthNameShort[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	static const char* s_WeekDayName[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
	static const char* s_WeekDayNameShort[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

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
	const char* getDateTimeHHMMSS()
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		snprintf(s_DateTimeStrings.time24HHMMSS, 9, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
		return s_DateTimeStrings.time24HHMMSS;
	}
	const char* getDateTime12HHMMSS()
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		snprintf(s_DateTimeStrings.time12HHMMSS, 9, "%02d:%02d:%02d", ((tm.tm_hour + 11) % 12) + 1, tm.tm_min, tm.tm_sec);
		return s_DateTimeStrings.time12HHMMSS;
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

	const char* getDateYearStr()
	{
		snprintf(s_DateTimeStrings.yearName, 5, "%d", getDateYear());
		return s_DateTimeStrings.yearName;
	}
	const char* getDateYear02dStr()
	{
		snprintf(s_DateTimeStrings.year02dName, 3, "%d", getDateYear02d());
		return s_DateTimeStrings.year02dName;
	}
	const char* getDateMonthNumStr()
	{
		snprintf(s_DateTimeStrings.monthNumberName, 3, "%02d", getDateMonth());
		return s_DateTimeStrings.monthNumberName;
	}
	const char* getDateDayNumStr()
	{
		snprintf(s_DateTimeStrings.dayNumberName, 3, "%02d", getDateDay());
		return s_DateTimeStrings.dayNumberName;
	}
	const char* getDateHourNumStr()
	{
		snprintf(s_DateTimeStrings.hourNumberName, 3, "%02d", getDateHour());
		return s_DateTimeStrings.hourNumberName;
	}
	const char* getDateHour12NumStr()
	{
		snprintf(s_DateTimeStrings.hour12NumberName, 3, "%02d", getDateHour12());
		return s_DateTimeStrings.hour12NumberName;
	}
	const char* getDateMinuteNumStr()
	{
		snprintf(s_DateTimeStrings.minuteNumberName, 3, "%02d", getDateMinute());
		return s_DateTimeStrings.minuteNumberName;
	}
	const char* getDateSecondNumStr()
	{
		snprintf(s_DateTimeStrings.secondNumberName, 3, "%02d", getDateSecond());
		return s_DateTimeStrings.secondNumberName;
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
		{ '$', [](LogMessage* msg) { return "\n"; } },
		{ 'n', [](LogMessage* msg) { return msg->loggerName; } },
		{ 'l', [](LogMessage* msg) { return coreUtils::getLogLevelName(msg->level); }},
		{ '#', [](LogMessage* msg) { return coreUtils::getLogLevelColor(msg->level); }},
		{ '^', [](LogMessage* msg) { return LVN_LOG_COLOR_RESET; }},
		{ 'v', [](LogMessage* msg) { return msg->msg; }},
		{ '%', [](LogMessage* msg) { return "%"; } },
		{ 'T', [](LogMessage* msg) { return getDateTimeHHMMSS(); } },
		{ 't', [](LogMessage* msg) { return getDateTime12HHMMSS(); } },
		{ 'Y', [](LogMessage* msg) { return getDateYearStr(); }},
		{ 'y', [](LogMessage* msg) { return getDateYear02dStr(); } },
		{ 'm', [](LogMessage* msg) { return getDateMonthNumStr(); } },
		{ 'B', [](LogMessage* msg) { return getDateMonthName(); } },
		{ 'b', [](LogMessage* msg) { return getDateMonthNameShort(); } },
		{ 'd', [](LogMessage* msg) { return getDateDayNumStr(); } },
		{ 'A', [](LogMessage* msg) { return getDateWeekDayName(); } },
		{ 'a', [](LogMessage* msg) { return getDateWeekDayNameShort(); } },
		{ 'H', [](LogMessage* msg) { return getDateHourNumStr(); } },
		{ 'h', [](LogMessage* msg) { return getDateHour12NumStr(); } },
		{ 'M', [](LogMessage* msg) { return getDateMinuteNumStr(); } },
		{ 'S', [](LogMessage* msg) { return getDateSecondNumStr(); } },
		{ 'P', [](LogMessage* msg) { return getDateTimeMeridiem(); } },
		{ 'p', [](LogMessage* msg) { return getDateTimeMeridiemLower(); }},
	};

	void logParseFormat(const char* fmt, LogPattern** pLogPatterns, uint32_t* logPatternCount)
	{
		if (!fmt || !strlen(fmt))
			return;

		LogPattern* patterns = (LogPattern*)malloc(0);
		uint32_t patternCount = 0;

		for (uint32_t i = 0; i < strlen(fmt) - 1; i++)
		{
			if (fmt[i] != '%') // Other characters in format
			{
				LogPattern pattern = { .symbol = fmt[i], .func = nullptr };
				LogPattern* newPattern = (LogPattern*)realloc(patterns, ++patternCount * sizeof(LogPattern));
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

				LogPattern* newPattern = (LogPattern*)realloc(patterns, ++patternCount * sizeof(LogPattern));
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
				const char* fmtstr = logger->pLogPatterns[i].func(msg);
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
		AppRenderEvent eventType{};
		if (event->type == eventType.info.type)
		{
			
		}

		return false;
	}
	bool dispatchAppTickEvent(Event* event, bool(*func)(AppTickEvent*))
	{
		return false;
	}
	bool dispatchKeyPressedEvent(Event* event, bool(*func)(KeyPressedEvent*))
	{
		return false;
	}
	bool dispatchKeyReleasedEvent(Event* event, bool(*func)(KeyReleasedEvent*))
	{
		return false;
	}
	bool dispatchKeyHoldEvent(Event* event, bool(*func)(KeyHoldEvent*))
	{
		return false;
	}
	bool dispatchWindowCloseEvent(Event* event, bool(*func)(WindowCloseEvent*))
	{
		return false;
	}
	bool dispatchWindowResizeEvent(Event* event, bool(*func)(WindowResizeEvent*))
	{
		return false;
	}
	bool dispatchWindowFramebufferResizeEvent(Event* event, bool(*func)(WindowFramebufferResizeEvent*))
	{
		return false;
	}
	bool dispatchWindowFocusEvent(Event* event, bool(*func)(WindowFocusEvent*))
	{
		return false;
	}
	bool dispatchWindowLostFocusEvent(Event* event, bool(*func)(WindowLostFocusEvent*))
	{
		return false;
	}
	bool dispatchWindowMovedEvent(Event* event, bool(*func)(WindowMovedEvent*))
	{
		return false;
	}
	bool dispatchMouseButtonPressedEvent(Event* event, bool(*func)(MouseButtonPressedEvent*))
	{
		return false;
	}
	bool dispatchMouseButtonReleasedEvent(Event* event, bool(*func)(MouseButtonReleasedEvent*))
	{
		return false;
	}
	bool dispatchMouseMovedEvent(Event* event, bool(*func)(MouseMovedEvent*))
	{
		return false;
	}
	bool dispatchMouseScrolledEvent(Event* event, bool(*func)(MouseScrolledEvent*))
	{
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
				glfws::glfwInitWindowContext(s_WindowContext);
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
				glfws::glfwTerminateWindowContext();
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

	WindowDimension getDimensions(Window* window)
	{
		return s_WindowContext->getDimensions(window);
	}

	unsigned int getWindowWidth(Window* window)
	{
		return s_WindowContext->getWindowWidth(window);
	}

	unsigned int getWindowHeight(Window* window)
	{
		return s_WindowContext->getWindowHeight(window);
	}

	void setWindowEventCallback(Window* window, void (*callback)(Event*))
	{
		window->info.eventCallBackFn = callback;
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

	void setWindowAPICurrent(Window* window)
	{
		s_WindowContext->setWindowContextCurrent(window);
	}

	void destroyWindow(Window* window)
	{
		s_WindowContext->destroyWindow(window);
	}
}