#include "levikno.h"
#include "levikno_internal.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#ifdef LVN_PLATFORM_WINDOWS
	#include <windows.h>
#endif

#define LVN_ABORT abort()
#define LVN_EMPTY_STR "\0"
#define LVN_DEFAULT_LOG_PATTERN "[%Y-%m-%d] [%T] [%#%l%^] %n: %v%$"

#include "lvn_glfw.h"
#include "lvn_vulkan.h"

static LvnContext* s_LvnContext = nullptr;

namespace lvn
{

static void         enableLogANSIcodeColors();
static void         logParseFormat(const char* fmt, LvnLogPattern** pLogPatterns, uint32_t* logPatternCount);
static const char*  getLogLevelColor(LvnLogLevel level);
static const char*  getLogLevelName(LvnLogLevel level);
static const char*  getGraphicsApiNameEnum(LvnGraphicsApi api);
static const char*  getWindowApiNameEnum(LvnWindowApi api);
static LvnResult    setWindowContext(LvnWindowApi windowapi);
static void         terminateWindowContext();
static LvnResult    setGraphicsContext(LvnGraphicsApi graphicsapi, bool enableValidation);
static void         terminateGraphicsContext();


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
		case Lvn_LogLevel_Info:     { return LVN_LOG_COLOR_INFO; }
		case Lvn_LogLevel_Warn:     { return LVN_LOG_COLOR_WARN; }
		case Lvn_LogLevel_Error:    { return LVN_LOG_COLOR_ERROR; }
		case Lvn_LogLevel_Critical: { return LVN_LOG_COLOR_CRITICAL; }
	}

	return nullptr;
}

static const char* getLogLevelName(LvnLogLevel level)
{
	switch (level)
	{
		case Lvn_LogLevel_None:     { return "none"; }
		case Lvn_LogLevel_Trace:    { return "trace"; }
		case Lvn_LogLevel_Info:     { return "info"; }
		case Lvn_LogLevel_Warn:     { return "warn"; }
		case Lvn_LogLevel_Error:    { return "error"; }
		case Lvn_LogLevel_Critical: { return "critical"; }
	}

	return nullptr;
}


/* [API] */
LvnResult createContext(LvnContextCreateInfo* createInfo)
{
	if (s_LvnContext != nullptr) { return Lvn_Result_AlreadyCalled; }
	s_LvnContext = new LvnContext();

	s_LvnContext->windowapi = createInfo->windowapi;
	s_LvnContext->graphicsapi = createInfo->graphicsapi;

	// logging
	if (createInfo->enableLogging) { logInit(); }

	// window context
	LvnResult result = setWindowContext(createInfo->windowapi);
	if (result != Lvn_Result_Success) { return result; }

	// graphics context
	result = setGraphicsContext(createInfo->graphicsapi, createInfo->enableVulkanValidationLayers);
	if (result != Lvn_Result_Success) { return result; }

	return Lvn_Result_Success;
}

void terminateContext()
{
	if (s_LvnContext == nullptr) { return; }

	terminateWindowContext();
	terminateGraphicsContext();

	if (s_LvnContext->numMemoryAllocations)
	{
		LVN_CORE_ERROR("not all memory allocations have been freed! Number of allocations remaining: %zu", s_LvnContext->numMemoryAllocations);
	}
	
	logTerminate();

	delete s_LvnContext;
	s_LvnContext = nullptr;
}

LvnContext* getContext()
{
	return s_LvnContext;
}

static const char* const s_MonthName[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
static const char* const s_MonthNameShort[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static const char* const s_WeekDayName[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
static const char* const s_WeekDayNameShort[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

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

long long getSecondsSinceEpoch()
{
	return time(NULL);
}

void* memAlloc(size_t size)
{
	void* allocmem = malloc(size);
	if (!allocmem) { LVN_CORE_ERROR("malloc failure, could not allocate memory!"); LVN_ABORT; }
	if (s_LvnContext) { s_LvnContext->numMemoryAllocations++; }
	return allocmem;
}

void memFree(void* ptr)
{
	free(ptr);
	if (s_LvnContext) s_LvnContext->numMemoryAllocations--;
}

LvnString getFileSrc(const char* filepath)
{
	FILE* fileptr;
	fileptr = fopen(filepath, "r");
	
	fseek(fileptr, 0, SEEK_END);
	long int size = ftell(fileptr);
	fseek(fileptr, 0, SEEK_SET);
	
	LvnString filesrc(size);
	fread(filesrc, sizeof(char), size, fileptr);
	fclose(fileptr);

	return filesrc;
}

LvnVector<uint8_t> getFileSrcBin(const char* filepath)
{
	FILE* fileptr;
	fileptr = fopen(filepath, "rb");

	fseek(fileptr, 0, SEEK_END);
	long int size = ftell(fileptr);
	fseek(fileptr, 0, SEEK_SET);

	LvnVector<uint8_t> bin(size);
	fread(bin.data(), sizeof(uint8_t), size, fileptr);

	return bin;
}

/* [Logging] */
const static LvnLogPattern s_LogPatterns[] =
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

	LvnVector<LvnLogPattern> patterns;

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

		i++; // incramant past symbol on next character in format
	}

	void* allocLogPatterns = malloc(patterns.memsize());
	if (!allocLogPatterns) { LVN_ABORT; }
	*logPatternCount = patterns.size();
	*pLogPatterns = (LvnLogPattern*)allocLogPatterns;
	memcpy(*pLogPatterns, patterns.data(), patterns.memsize());
}

LvnResult logInit()
{
	LvnContext* lvnctx = s_LvnContext;

	if (!lvnctx->logging)
	{
		lvnctx->logging = true;

		lvnctx->coreLogger.loggerName = "CORE";
		lvnctx->clientLogger.loggerName = "CLIENT";
		lvnctx->coreLogger.logLevel = Lvn_LogLevel_None;
		lvnctx->clientLogger.logLevel = Lvn_LogLevel_None;
		lvnctx->coreLogger.logPatternFormat = LVN_DEFAULT_LOG_PATTERN;
		lvnctx->clientLogger.logPatternFormat = LVN_DEFAULT_LOG_PATTERN;

		LvnLogPattern* coreLogPatterns = nullptr;
		uint32_t coreLogPatternCount = 0;
		logParseFormat(LVN_DEFAULT_LOG_PATTERN, &coreLogPatterns, &coreLogPatternCount);

		LvnLogPattern* clientLogPatterns = nullptr;
		uint32_t clientLogPatternCount = 0;
		logParseFormat(LVN_DEFAULT_LOG_PATTERN, &clientLogPatterns, &clientLogPatternCount);

		lvnctx->coreLogger.pLogPatterns = coreLogPatterns;
		lvnctx->coreLogger.logPatternCount = coreLogPatternCount;
		lvnctx->clientLogger.pLogPatterns = clientLogPatterns;
		lvnctx->clientLogger.logPatternCount = clientLogPatternCount;

		#ifdef LVN_PLATFORM_WINDOWS 
		enableLogANSIcodeColors();
		#endif

		return Lvn_Result_Success;
	}
		
	return Lvn_Result_AlreadyCalled;
}

void logTerminate()
{
	LvnContext* lvnctx = s_LvnContext;

	if (lvnctx->logging)
	{
		lvnctx->logging = false;

		if (lvnctx->coreLogger.pLogPatterns)
			free(lvnctx->coreLogger.pLogPatterns);
		if (lvnctx->clientLogger.pLogPatterns)
			free(lvnctx->clientLogger.pLogPatterns);
	}
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
	if (!s_LvnContext->logging) { return; }

	LvnVector<char> str;

	for (uint32_t i = 0; i < logger->logPatternCount; i++)
	{
		if (logger->pLogPatterns[i].func == nullptr) // no special format character '%' found
		{
			str.push_back(logger->pLogPatterns[i].symbol);
		}
		else // call func of special format
		{
			LvnString fmtstr = logger->pLogPatterns[i].func(msg);
			uint32_t fmtstrlen = fmtstr.size();
			str.copy_back(fmtstr, fmtstr.size() - 1);
		}
	}

	printf("%.*s",str.size(), str.data());
}

void logMessage(LvnLogger* logger, LvnLogLevel level, const char* msg)
{
	if (!s_LvnContext->logging) { return; }

	LvnLogMessage logMsg =
	{
		.msg = msg,
		.loggerName = logger->loggerName,
		.level = level,
		.timeEpoch = getSecondsSinceEpoch()
	};
	logOutputMessage(logger, &logMsg);
}

void logMessageTrace(LvnLogger* logger, const char* fmt, ...)
{
	if (!s_LvnContext || !s_LvnContext->logging) { return; }

	char buff[1024];

	va_list argptr;
	va_start(argptr, fmt);

	vsnprintf(buff, 1024, fmt, argptr);
	logMessage(logger, Lvn_LogLevel_Trace, buff);

	va_end(argptr);
}

void logMessageInfo(LvnLogger* logger, const char* fmt, ...)
{
	if (!s_LvnContext || !s_LvnContext->logging) { return; }

	char buff[1024];

	va_list argptr;
	va_start(argptr, fmt);

	vsnprintf(buff, 1024, fmt, argptr);
	logMessage(logger, Lvn_LogLevel_Info, buff);

	va_end(argptr);
}

void logMessageWarn(LvnLogger* logger, const char* fmt, ...)
{
	if (!s_LvnContext || !s_LvnContext->logging) { return; }

	char buff[1024];

	va_list argptr;
	va_start(argptr, fmt);

	vsnprintf(buff, 1024, fmt, argptr);
	logMessage(logger, Lvn_LogLevel_Warn, buff);

	va_end(argptr);
}

void logMessageError(LvnLogger* logger, const char* fmt, ...)
{
	if (!s_LvnContext || !s_LvnContext->logging) { return; }

	char buff[1024];

	va_list argptr;
	va_start(argptr, fmt);

	vsnprintf(buff, 1024, fmt, argptr);
	logMessage(logger, Lvn_LogLevel_Error, buff);

	va_end(argptr);
}

void logMessageCritical(LvnLogger* logger, const char* fmt, ...)
{
	if (!s_LvnContext || !s_LvnContext->logging) { return; }

	char buff[1024];

	va_list argptr;
	va_start(argptr, fmt);

	vsnprintf(buff, 1024, fmt, argptr);
	logMessage(logger, Lvn_LogLevel_Critical, buff);

	va_end(argptr);
}

LvnLogger* getCoreLogger()
{
	return &s_LvnContext->coreLogger;
}

LvnLogger* getClientLogger()
{
	return &s_LvnContext->clientLogger;
}

const char* getLogANSIcodeColor(LvnLogLevel level)
{
	switch (level)
	{
		case Lvn_LogLevel_None:     { return LVN_LOG_COLOR_RESET; }
		case Lvn_LogLevel_Trace:    { return LVN_LOG_COLOR_TRACE; }
		case Lvn_LogLevel_Info:     { return LVN_LOG_COLOR_INFO; }
		case Lvn_LogLevel_Warn:     { return LVN_LOG_COLOR_WARN; }
		case Lvn_LogLevel_Error:    { return LVN_LOG_COLOR_ERROR; }
		case Lvn_LogLevel_Critical: { return LVN_LOG_COLOR_CRITICAL; }
	}

	return nullptr;
}

void logSetPatternFormat(LvnLogger* logger, const char* patternfmt)
{
	if (!patternfmt || patternfmt[0] == '\0') return;

	free(logger->pLogPatterns);
	logger->logPatternFormat = patternfmt;

	logParseFormat(patternfmt, &logger->pLogPatterns, &logger->logPatternCount);
}

// [Window]

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

static const char* getWindowApiNameEnum(LvnWindowApi api)
{
	switch (api)
	{
		case Lvn_WindowApi_None:  { return "None";  }
		case Lvn_WindowApi_glfw:  { return "glfw";  }
		case Lvn_WindowApi_win32: { return "win32"; }
	}

	return LVN_EMPTY_STR;
}

static LvnResult setWindowContext(LvnWindowApi windowapi)
{
	LvnContext* lvnctx = s_LvnContext;

	LvnResult result = Lvn_Result_Failure;
	switch (windowapi)
	{
		case Lvn_WindowApi_None:
		{
			result = Lvn_Result_Failure;
			break;
		}
		case Lvn_WindowApi_glfw:
		{
			result = glfwImplInitWindowContext(&lvnctx->windowContext);
			break;
		}
		case Lvn_WindowApi_win32:
		{
			break;
		}
	}

	//windowInputInit();

	if (result != Lvn_Result_Success)
		LVN_CORE_ERROR("could not create window context for: %s", getWindowApiNameEnum(windowapi));
	else
		LVN_CORE_TRACE("window context set: %s", getWindowApiNameEnum(windowapi));

	return result;
}

static void terminateWindowContext()
{
	LvnContext* lvnctx = s_LvnContext;

	switch (lvnctx->windowapi)
	{
		case Lvn_WindowApi_None:
		{
			LVN_CORE_WARN("no window API Initialized! Cannot terminate window API!");
			return;
		}
		case Lvn_WindowApi_glfw:
		{
			glfwImplTerminateWindowContext();
			break;
		}
		case Lvn_WindowApi_win32:
		{
			break;
		}
		default:
		{
			LVN_CORE_ERROR("unknown Windows API selected! Cannot terminate window API!");
			return;
		}
	}

	LVN_CORE_TRACE("window context terminated: %s", getWindowApiNameEnum(lvnctx->windowapi));
}

LvnWindowApi getWindowApi()
{
	return s_LvnContext->windowapi;
}

const char* getWindowApiName()
{
	switch (s_LvnContext->windowapi)
	{
		case Lvn_WindowApi_None:  { return "None";  }
		case Lvn_WindowApi_glfw:  { return "glfw";  }
		case Lvn_WindowApi_win32: { return "win32"; }
	}

	LVN_CORE_ERROR("Unknown Windows API selected!");
	return LVN_EMPTY_STR;
}

LvnResult createWindow(LvnWindow** window, LvnWindowCreateInfo* createInfo)
{
	if (createInfo->width < 0 || createInfo->height < 0)
	{
		LVN_CORE_ERROR("cannot create window with negative dimensions (w:%d,h:%d)", createInfo->width, createInfo->height);
		return Lvn_Result_Failure;
	}

	*window = new LvnWindow();
	return s_LvnContext->windowContext.createWindow(*window, createInfo);

	LVN_CORE_TRACE("created window: (%p), \"%s\" (w:%d, h%d)", *window, createInfo->title, createInfo->width, createInfo->height);
}

void destroyWindow(LvnWindow* window)
{
	s_LvnContext->windowContext.destroyWindow(window);
	delete window;
	window = nullptr;
}

void updateWindow(LvnWindow* window)
{
	s_LvnContext->windowContext.updateWindow(window);
}

bool windowOpen(LvnWindow* window)
{
	return s_LvnContext->windowContext.windowOpen(window);
}

LvnWindowDimensions getWindowDimensions(LvnWindow* window)
{
	return s_LvnContext->windowContext.getDimensions(window);
}

int getWindowWidth(LvnWindow* window)
{
	return s_LvnContext->windowContext.getWindowWidth(window);
}

int getWindowHeight(LvnWindow* window)
{
	return s_LvnContext->windowContext.getWindowHeight(window);
}

void setWindowEventCallback(LvnWindow* window, void (*callback)(LvnEvent*))
{
	window->data.eventCallBackFn = callback;
}

void setWindowVSync(LvnWindow* window, bool enable)
{
	s_LvnContext->windowContext.setWindowVSync(window, enable);
}

bool getWindowVSync(LvnWindow* window)
{
	return s_LvnContext->windowContext.getWindowVSync(window);
}

void* getNativeWindow(LvnWindow* window)
{
	return window->nativeWindow;
}

void setWindowContextCurrent(LvnWindow* window)
{
	s_LvnContext->windowContext.setWindowContextCurrent(window);
}


// [Graphics]

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

static LvnResult setGraphicsContext(LvnGraphicsApi graphicsapi, bool enableValidation)
{
	LvnContext* lvnctx = s_LvnContext;

	LvnResult result = Lvn_Result_Failure;
	switch (graphicsapi)
	{
		case Lvn_GraphicsApi_None:
		{
			result = Lvn_Result_Failure;
			break;
		}
		case Lvn_GraphicsApi_vulkan:
		{
			result = vksImplCreateContext(&lvnctx->graphicsContext, enableValidation);
			break;
		}
		case Lvn_GraphicsApi_opengl:
		{
			break;
		}
	}

	if (result != Lvn_Result_Success)
		LVN_CORE_ERROR("could not create graphics context for: %s", getGraphicsApiNameEnum(graphicsapi));
	else
		LVN_CORE_TRACE("graphics context set: %s", getGraphicsApiNameEnum(graphicsapi));

	return result;
}

static void terminateGraphicsContext()
{
	LvnContext* lvnctx = s_LvnContext;

	switch (lvnctx->graphicsapi)
	{
		case Lvn_GraphicsApi_None:
		{
			LVN_CORE_WARN("no Graphics API Initialized! Cannot terminate Graphics API!");
			return;
		}
		case Lvn_GraphicsApi_vulkan:
		{
			vksImplTerminateContext();
			break;
		}
		case Lvn_GraphicsApi_opengl:
		{
			break;
		}
		default:
		{
			LVN_CORE_ERROR("unknown Graphics API selected! Cannot terminate Graphics API!");
		}
	}

	LVN_CORE_TRACE("graphics context terminated: %s", getGraphicsApiNameEnum(lvnctx->graphicsapi));
}

LvnGraphicsApi getGraphicsApi()
{
	return s_LvnContext->graphicsapi;
}

const char* getGraphicsApiName()
{
	switch (s_LvnContext->graphicsapi)
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
	s_LvnContext->graphicsContext.getPhysicalDevices(nullptr, &getDeviceCount);

	if (pPhysicalDevices == nullptr)
	{
		*deviceCount = getDeviceCount;
		return;
	}

	s_LvnContext->graphicsContext.getPhysicalDevices(pPhysicalDevices, &getDeviceCount);

	return;
}

LvnPhysicalDeviceInfo getPhysicalDeviceInfo(LvnPhysicalDevice* physicalDevice)
{
	return physicalDevice->info;
}

LvnResult renderInit(LvnRenderInitInfo* renderInfo)
{
	return s_LvnContext->graphicsContext.renderInit(renderInfo);
}

void renderCmdDraw(LvnWindow* window, uint32_t vertexCount)
{
	s_LvnContext->graphicsContext.renderCmdDraw(window, vertexCount);
}

void renderCmdDrawIndexed(LvnWindow* window, uint32_t indexCount)
{
	s_LvnContext->graphicsContext.renderCmdDrawIndexed(window, indexCount);
}

void renderCmdDrawInstanced(LvnWindow* window, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance)
{

}

void renderCmdDrawIndexedInstanced(LvnWindow* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance)
{

}

void renderCmdSetStencilReference(uint32_t reference)
{

}

void renderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask)
{

}

void renderBeginNextFrame(LvnWindow* window)
{
	s_LvnContext->graphicsContext.renderBeginNextFrame(window);
}

void renderDrawSubmit(LvnWindow* window)
{
	s_LvnContext->graphicsContext.renderDrawSubmit(window);
}

void renderBeginCommandRecording(LvnWindow* window)
{
	s_LvnContext->graphicsContext.renderBeginCommandRecording(window);
}

void renderEndCommandRecording(LvnWindow* window)
{
	s_LvnContext->graphicsContext.renderEndCommandRecording(window);
}

void renderCmdBeginRenderPass(LvnWindow* window)
{
	s_LvnContext->graphicsContext.renderCmdBeginRenderPass(window);
}

void renderCmdEndRenderPass(LvnWindow* window)
{
	s_LvnContext->graphicsContext.renderCmdEndRenderPass(window);
}

void renderCmdBindPipeline(LvnWindow* window, LvnPipeline* pipeline)
{
	s_LvnContext->graphicsContext.renderCmdBindPipeline(window, pipeline);
}

void renderCmdBindVertexBuffer(LvnWindow* window, LvnBuffer* buffer)
{
	s_LvnContext->graphicsContext.renderCmdBindVertexBuffer(window, buffer);
}

void renderCmdBindIndexBuffer(LvnWindow* window, LvnBuffer* buffer)
{
	s_LvnContext->graphicsContext.renderCmdBindIndexBuffer(window, buffer);
}

void renderCmdBindDescriptorLayout(LvnWindow* window, LvnPipeline* pipeline, LvnDescriptorLayout* descriptorLayout)
{
	s_LvnContext->graphicsContext.renderCmdBindDescriptorLayout(window, pipeline, descriptorLayout);
}

LvnResult createRenderPass(LvnRenderPass** renderPass, LvnRenderPassCreateInfo* createInfo)
{
	*renderPass = new LvnRenderPass();
	LVN_CORE_TRACE("created renderPass: (%p), attachment count: %u", *renderPass, createInfo->attachmentCount);
	return s_LvnContext->graphicsContext.createRenderPass(*renderPass, createInfo);
}

LvnResult createShaderFromSrc(LvnShader** shader, LvnShaderCreateInfo* createInfo)
{
	if (!createInfo->vertexSrc)
	{
		LVN_CORE_ERROR("createShaderFromSrc(LvnShader**, LvnShaderCreateInfo*) | createInfo->vertexSrc is nullptr, cannot create shader without the vertex shader source");
		return Lvn_Result_Failure;
	}

	if (!createInfo->fragmentSrc)
	{
		LVN_CORE_ERROR("createShaderFromSrc(LvnShader**, LvnShaderCreateInfo*) | createInfo->fragmentSrc is nullptr, cannot create shader without the fragment shader source");
		return Lvn_Result_Failure;
	}

	*shader = new LvnShader();
	LVN_CORE_TRACE("created shader: (%p)", *shader);
	return s_LvnContext->graphicsContext.createShaderFromSrc(*shader, createInfo);
}

LvnResult createShaderFromFileSrc(LvnShader** shader, LvnShaderCreateInfo* createInfo)
{
	if (!createInfo->vertexSrc)
	{
		LVN_CORE_ERROR("createShaderFromFileSrc(LvnShader**, LvnShaderCreateInfo*) | createInfo->vertexSrc is nullptr, cannot create shader without the vertex shader source");
		return Lvn_Result_Failure;
	}

	if (!createInfo->fragmentSrc)
	{
		LVN_CORE_ERROR("createShaderFromFileSrc(LvnShader**, LvnShaderCreateInfo*) | createInfo->fragmentSrc is nullptr, cannot create shader without the fragment shader source");
		return Lvn_Result_Failure;
	}

	*shader = new LvnShader();
	LVN_CORE_TRACE("created shader: (%p)", *shader);
	return s_LvnContext->graphicsContext.createShaderFromFileSrc(*shader, createInfo);
}

LvnResult createShaderFromFileBin(LvnShader** shader, LvnShaderCreateInfo* createInfo)
{
	if (!createInfo->vertexSrc)
	{
		LVN_CORE_ERROR("createShaderFileBin(LvnShader**, LvnShaderCreateInfo*) | createInfo->vertexSrc is nullptr, cannot create shader without the vertex shader source");
		return Lvn_Result_Failure;
	}

	if (!createInfo->fragmentSrc)
	{
		LVN_CORE_ERROR("createShaderFileBin(LvnShader**, LvnShaderCreateInfo*) | createInfo->fragmentSrc is nullptr, cannot create shader without the fragment shader source");
		return Lvn_Result_Failure;
	}

	*shader = new LvnShader();
	LVN_CORE_TRACE("created shader: (%p)", *shader);
	return s_LvnContext->graphicsContext.createShaderFromFileBin(*shader, createInfo);
}

LvnResult createDescriptorLayout(LvnDescriptorLayout** descriptorLayout, LvnDescriptorLayoutCreateInfo* createInfo)
{
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

	*descriptorLayout = new LvnDescriptorLayout();
	LVN_CORE_TRACE("created descriptorLayout: (%p), descriptor binding count: %u", *descriptorLayout, createInfo->descriptorBindingCount);
	return s_LvnContext->graphicsContext.createDescriptorLayout(*descriptorLayout, createInfo);
}

LvnResult createPipeline(LvnPipeline** pipeline, LvnPipelineCreateInfo* createInfo)
{
	*pipeline = new LvnPipeline();
	LVN_CORE_TRACE("created pipeline: (%p)", *pipeline);
	return s_LvnContext->graphicsContext.createPipeline(*pipeline, createInfo);
}

LvnResult createFrameBuffer(LvnFrameBuffer** frameBuffer, LvnFrameBufferCreateInfo* createInfo)
{
	*frameBuffer = new LvnFrameBuffer();
	LVN_CORE_TRACE("created framebuffer: (%p)", *frameBuffer);
	return s_LvnContext->graphicsContext.createFrameBuffer(*frameBuffer, createInfo);
}

LvnResult createBuffer(LvnBuffer** buffer, LvnBufferCreateInfo* createInfo)
{
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

	*buffer = new LvnBuffer();
	LVN_CORE_TRACE("created buffer: (%p)", *buffer);
	return s_LvnContext->graphicsContext.createBuffer(*buffer, createInfo);
}

LvnResult createUniformBuffer(LvnUniformBuffer** uniformBuffer, LvnUniformBufferCreateInfo* createInfo)
{
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

	*uniformBuffer = new LvnUniformBuffer();
	LVN_CORE_TRACE("created uniform buffer: (%p), binding: %u, size: %lu bytes", *uniformBuffer, createInfo->binding, createInfo->size);
	return s_LvnContext->graphicsContext.createUniformBuffer(*uniformBuffer, createInfo);
}

void destroyRenderPass(LvnRenderPass* renderPass)
{
	s_LvnContext->graphicsContext.destroyRenderPass(renderPass);
	delete renderPass;
}

void destroyShader(LvnShader* shader)
{
	s_LvnContext->graphicsContext.destroyShader(shader);
	delete shader;
}

void destroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout)
{
	s_LvnContext->graphicsContext.destroyDescriptorLayout(descriptorLayout);
	delete descriptorLayout;
}

void destroyPipeline(LvnPipeline* pipeline)
{
	s_LvnContext->graphicsContext.destroyPipeline(pipeline);
	delete pipeline;
}

void destroyFrameBuffer(LvnFrameBuffer* frameBuffer)
{
	s_LvnContext->graphicsContext.destroyFrameBuffer(frameBuffer);
	delete frameBuffer;
}

void destroyBuffer(LvnBuffer* buffer)
{
	s_LvnContext->graphicsContext.destroyBuffer(buffer);
	delete buffer;
}

void destroyUniformBuffer(LvnUniformBuffer* uniformBuffer)
{
	s_LvnContext->graphicsContext.destroyUniformBuffer(uniformBuffer);
	delete uniformBuffer;
}

void setDefaultPipelineSpecification(LvnPipelineSpecification* pipelineSpecification)
{
	s_LvnContext->graphicsContext.setDefaultPipelineSpecification(pipelineSpecification);
}

LvnPipelineSpecification getDefaultPipelineSpecification()
{
	return s_LvnContext->graphicsContext.getDefaultPipelineSpecification();
}

void updateUniformBufferData(LvnWindow* window, LvnUniformBuffer* uniformBuffer, void* data, uint64_t size)
{
	s_LvnContext->graphicsContext.updateUniformBufferData(window, uniformBuffer, data, size);
}

// [Section]: Math

float radians(float deg)
{
	return deg * 0.0174532925199f; // deg * (PI / 180)
}

float degrees(float rad)
{
	return rad * 57.2957795131f; // rad * (180 / PI)
}

float inverseSqrt(float num)
{
	union
	{
		float f;
		uint32_t i;
	} conv;

	float x2;
	const float threehalfs = 1.5f;

	x2 = num * 0.5f;
	conv.f  = num;
	conv.i  = 0x5f3759df - (conv.i >> 1);
	conv.f  = conv.f * (threehalfs - (x2 * conv.f * conv.f));
	return conv.f;
}

LvnVec2f normalize(LvnVec2f v)
{
	float u = inverseSqrt(v.x * v.x + v.y * v.y);
	return { v.x * u, v.y * u };
}

LvnVec2d normalize(LvnVec2d v)
{
	double u = inverseSqrt(static_cast<float>(v.x * v.x + v.y * v.y));
	return { v.x * u, v.y * u };
}

LvnVec3f normalize(LvnVec3f v)
{
	float u = inverseSqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	return { v.x * u, v.y * u, v.z * u };
}

LvnVec3d normalize(LvnVec3d v)
{
	double u = inverseSqrt(static_cast<float>(v.x * v.x + v.y * v.y + v.z * v.z));
	return { v.x * u, v.y * u, v.z * u };
}

LvnVec4f normalize(LvnVec4f v)
{
	float u = inverseSqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	return { v.x * u, v.y * u, v.z * u, v.w * u };
}

LvnVec4d normalize(LvnVec4d v)
{
	double u = inverseSqrt(static_cast<float>(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w));
	return { v.x * u, v.y * u, v.z * u, v.w * u };
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
