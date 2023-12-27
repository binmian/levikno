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
	static WindowContext s_WindowContext = WindowContext::None;
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
		return s_DateTimeStrings.time12HHMMSS;
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
	const static LogPattern logPatterns[23] =
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
			s_CoreLogger.pLogPatterns = logPatterns;
			s_CoreLogger.logPatternCount = sizeof(logPatterns) / sizeof(LogPattern);
			s_ClientLogger.pLogPatterns = logPatterns;
			s_ClientLogger.logPatternCount = sizeof(logPatterns) / sizeof(LogPattern);

			return true;
		}
		
		return false;
	}

	bool logTerminate()
	{
		if (s_LoggingInit)
		{
			s_LoggingInit = false;
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
		printf(msg->msg);
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

	char* parseFormat(Logger* logger, const char* fmt)
	{
		/*uint32_t strsize = strlen(fmt) + 1;
		char* buff = static_cast<char*>(malloc(strsize));
		memcpy(buff, fmt, strsize);

		
		for (uint32_t i = 0; i < strlen(fmt); i++)
		{
			if (fmt[i] == '%')
			{
				
			}
			else continue;
		}
		*/

		return 0;
	}


	/* [Window] */
	bool setWindowContext(WindowContext windowContext)
	{
		s_WindowContext = windowContext;

		switch (getWindowContext())
		{
			case lvn::WindowContext::None:
			{
				LVN_CORE_WARN("setting Windows Context to none, no Windows Context selected!");
				break;
			}
			case lvn::WindowContext::glfw:
			{
				glfws::glfwInitWindowContext();
				break;
			}
			case lvn::WindowContext::win32:
			{

				break;
			}
			default:
			{
				LVN_CORE_ERROR("Unknown Windows Context selected! Cannot initilize window context!");
				break;
			}
		}

		//windowInputInit();

		LVN_CORE_INFO("window context set: %s", getWindowContextName());
		return true;
	}

	WindowContext getWindowContext()
	{
		return s_WindowContext;
	}

	const char* getWindowContextName()
	{
		switch (getWindowContext())
		{
			case lvn::WindowContext::None: { return "None"; }
			case lvn::WindowContext::glfw: { return "glfw"; }
			case lvn::WindowContext::win32: { return "win32"; }
		}

		LVN_CORE_ERROR("Unknown Windows Context selected!");
		return "";
	}

	bool terminateWindowContext()
	{
		switch (s_WindowContext)
		{
			case lvn::WindowContext::None:
			{
				LVN_CORE_WARN("No window context Initialized! Cannot terminate window context!");
				return false;
			}
			case lvn::WindowContext::glfw:
			{
				glfws::glfwTerminateWindowContext();
				break;
			}
			case lvn::WindowContext::win32:
			{

				break;
			}
			default:
			{
				LVN_CORE_ERROR("Unknown Windows Context selected! Cannot terminate window context!");
				return false;
			}
		}

		s_WindowContext = WindowContext::None;
		LVN_CORE_INFO("window context Terminated: %s", getWindowContextName());
		return true;
	}

	Window* createWindow(uint32_t width, uint32_t height, const char* title, bool fullscreen, bool resizable, int minWidth, int minHeight)
	{
		if (width * height < 0)
		{
			LVN_CORE_ERROR("Cannot create window with negative dimensions! (x:%d,y:%d)", width, height);
			return nullptr;
		}

		switch (s_WindowContext)
		{
			case lvn::WindowContext::None:
				break;
			case lvn::WindowContext::glfw:
			{
				return glfws::glfwImplCreateWindow(width, height, title, fullscreen, resizable, minWidth, minHeight);
			}
			case lvn::WindowContext::win32:
				break;
			default:
				break;
		}

		return nullptr;
	}

	Window* createWindow(WindowInfo winCreateInfo)
	{
		if (winCreateInfo.width * winCreateInfo.height < 0)
		{
			LVN_CORE_ERROR("Cannot create window with negative dimensions! (w:%d,h:%d)", winCreateInfo.width, winCreateInfo.height);
			return nullptr;
		}

		switch (s_WindowContext)
		{
			case lvn::WindowContext::None:
				break;
			case lvn::WindowContext::glfw:
			{
				return glfws::glfwImplCreateWindow(winCreateInfo);
			}
			case lvn::WindowContext::win32:
				break;
			default:
				break;
		}

		return nullptr;
	}

	void updateWindow(Window* window)
	{
		window->updateWindow(window);
	}

	bool windowOpen(Window* window)
	{
		return window->windowOpen(window);
	}

	WindowDimension getDimensions(Window* window)
	{
		return WindowDimension();
	}

	unsigned int getWindowWidth(Window* window)
	{
		return 0;
	}

	unsigned int getWindowHeight(Window* window)
	{
		return 0;
	}

	void setWindowEventCallback(Window* window, void (*callback)(Event*))
	{
		window->eventCallBackFn = callback;
	}

	void setWindowVSync(Window* window, bool enabled)
	{

	}

	bool getWindowVSync(Window* window)
	{
		return false;
	}

	void* getNativeWindow(Window* window)
	{
		return nullptr;
	}

	void setWindowContextDate(Window* window)
	{

	}

	void destroyWindow(Window* window)
	{

	}
}