#include "levikno.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <ctime>


#define LVN_DEFAULT_LOG_PATTERN "[%Y-%m-%d] [%T] [%#%l%^] %n: %v%$"

struct LvnLogger
{
    LvnString loggerName;
    LvnString logPatternFormat;
    LvnLogLevel logLevel;
    LvnVector<LvnLogPattern> logPatterns;

    LvnLogFile logfile;
};

struct LvnLoggingContext
{
    bool                                 logging;
    bool                                 enableCoreLogging;
    LvnLogger                            coreLogger;
    LvnLogger                            clientLogger;
    LvnVector<LvnLogPattern>             userLogPatterns;
    LvnString                            appName;
};

static LvnLoggingContext* s_LoggingContext = nullptr;
static size_t             s_MemAllocCount = 0;

namespace lvn
{

// memory allocation functions
static void*   mallocWrapper(size_t size, void* userData)               { (void)userData; return malloc(size); }
static void    freeWrapper(void* ptr, void* userData)                   { (void)userData; free(ptr); }
static void*   reallocWrapper(void* ptr, size_t size, void* userData)   { (void)userData; return realloc(ptr, size); }
static LvnMemAllocFunc    s_MemAllocFunc = mallocWrapper;
static LvnMemFreeFunc     s_MemFreeFunc = freeWrapper;
static LvnMemReallocFunc  s_MemReallocFunc = reallocWrapper;
static void*              s_MemAllocUserData = nullptr;

static const char*                  getLogLevelColor(LvnLogLevel level);
static const char*                  getLogLevelName(LvnLogLevel level);
static LvnVector<LvnLogPattern>     logParseFormat(const char* fmt);

static const char* getLogLevelColor(LvnLogLevel level)
{
    switch (level)
    {
        case Lvn_LogLevel_None:     { return LVN_LOG_COLOR_RESET; }
        case Lvn_LogLevel_Trace:    { return LVN_LOG_COLOR_TRACE; }
        case Lvn_LogLevel_Debug:    { return LVN_LOG_COLOR_DEBUG; }
        case Lvn_LogLevel_Info:     { return LVN_LOG_COLOR_INFO; }
        case Lvn_LogLevel_Warn:     { return LVN_LOG_COLOR_WARN; }
        case Lvn_LogLevel_Error:    { return LVN_LOG_COLOR_ERROR; }
        case Lvn_LogLevel_Fatal:    { return LVN_LOG_COLOR_FATAL; }
    }

    return nullptr;
}

static const char* getLogLevelName(LvnLogLevel level)
{
    switch (level)
    {
        case Lvn_LogLevel_None:     { return "none"; }
        case Lvn_LogLevel_Trace:    { return "trace"; }
        case Lvn_LogLevel_Debug:    { return "debug"; }
        case Lvn_LogLevel_Info:     { return "info"; }
        case Lvn_LogLevel_Warn:     { return "warn"; }
        case Lvn_LogLevel_Error:    { return "error"; }
        case Lvn_LogLevel_Fatal:    { return "fatal"; }
    }

    return nullptr;
}

const static LvnLogPattern s_LogPatterns[] =
{
    { '$', [](LvnLogMessage* msg) -> LvnString { return "\n"; } },
    { 'n', [](LvnLogMessage* msg) -> LvnString { return msg->loggerName; } },
    { 'l', [](LvnLogMessage* msg) -> LvnString { return getLogLevelName(msg->level); }},
    { '#', [](LvnLogMessage* msg) -> LvnString { return getLogLevelColor(msg->level); }},
    { '^', [](LvnLogMessage* msg) -> LvnString { return LVN_LOG_COLOR_RESET; }},
    { 'v', [](LvnLogMessage* msg) -> LvnString { return msg->msg; }},
    { '%', [](LvnLogMessage* msg) -> LvnString { return "%"; } },
    { 'T', [](LvnLogMessage* msg) -> LvnString { return dateGetTimeHHMMSS(); } },
    { 't', [](LvnLogMessage* msg) -> LvnString { return dateGetTime12HHMMSS(); } },
    { 'Y', [](LvnLogMessage* msg) -> LvnString { return dateGetYearStr(); }},
    { 'y', [](LvnLogMessage* msg) -> LvnString { return dateGetYear02dStr(); } },
    { 'm', [](LvnLogMessage* msg) -> LvnString { return dateGetMonthNumStr(); } },
    { 'B', [](LvnLogMessage* msg) -> LvnString { return dateGetMonthName(); } },
    { 'b', [](LvnLogMessage* msg) -> LvnString { return dateGetMonthNameShort(); } },
    { 'd', [](LvnLogMessage* msg) -> LvnString { return dateGetDayNumStr(); } },
    { 'A', [](LvnLogMessage* msg) -> LvnString { return dateGetWeekDayName(); } },
    { 'a', [](LvnLogMessage* msg) -> LvnString { return dateGetWeekDayNameShort(); } },
    { 'H', [](LvnLogMessage* msg) -> LvnString { return dateGetHourNumStr(); } },
    { 'h', [](LvnLogMessage* msg) -> LvnString { return dateGetHour12NumStr(); } },
    { 'M', [](LvnLogMessage* msg) -> LvnString { return dateGetMinuteNumStr(); } },
    { 'S', [](LvnLogMessage* msg) -> LvnString { return dateGetSecondNumStr(); } },
    { 'P', [](LvnLogMessage* msg) -> LvnString { return dateGetTimeMeridiem(); } },
    { 'p', [](LvnLogMessage* msg) -> LvnString { return dateGetTimeMeridiemLower(); }},
};

static LvnVector<LvnLogPattern> logParseFormat(const char* fmt)
{
    if (!fmt || !*fmt) { return {}; }

    LvnLoggingContext* logctx = lvn::getLoggingContex();

    LvnVector<LvnLogPattern> patterns;

    for (uint32_t i = 0; i < strlen(fmt) - 1; i++)
    {
        if (fmt[i] != '%') // Other characters in format
        {
            LvnLogPattern pattern = { /* .symbol = */ fmt[i], /* .func = */ nullptr };
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

        // find and add user defined patterns
        for (uint32_t j = 0; j < logctx->userLogPatterns.size(); j++)
        {
            if (fmt[i + 1] != logctx->userLogPatterns[j].symbol)
                continue;

            patterns.push_back(logctx->userLogPatterns[j]);
        }

        i++; // incramant past symbol on next character in format
    }

    return patterns;
}

void* memAlloc(size_t size)
{
    if (size == 0) { return nullptr; }
    void* allocmem = (*s_MemAllocFunc)(size, s_MemAllocUserData);
    if (!allocmem) { lvn::logCoreError("malloc failure, could not allocate memory!"); exit(-1); }
    memset(allocmem, 0, size);
    s_MemAllocCount++;
    return allocmem;
}

void memFree(void* ptr)
{
    if (ptr == nullptr) { return; }
    (*s_MemFreeFunc)(ptr, s_MemAllocUserData);
    s_MemAllocCount--;
}

void* memRealloc(void* ptr, size_t size)
{
    if (!ptr) { return lvn::memAlloc(size); }
    return (*s_MemReallocFunc)(ptr, size, s_MemAllocUserData);
}

void setMemFuncs(LvnMemAllocFunc allocFunc, LvnMemFreeFunc freeFunc, LvnMemReallocFunc reallocFunc, void* userData)
{
    s_MemAllocFunc = allocFunc;
    s_MemFreeFunc = freeFunc;
    s_MemReallocFunc = reallocFunc;
    s_MemAllocUserData = userData;
}

LvnMemAllocFunc getMemAllocFunc()
{
    return s_MemAllocFunc;
}

LvnMemFreeFunc getMemFreeFunc()
{
    return s_MemFreeFunc;
}

LvnMemReallocFunc getMemReallocFunc()
{
    return s_MemReallocFunc;
}

void* getMemUserData()
{
    return s_MemAllocUserData;
}

size_t getMemAllocCount()
{
    return s_MemAllocCount;
}

int dateGetYear()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return tm.tm_year + 1900;
}
int dateGetYear02d()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return (tm.tm_year + 1900) % 100;
}
int dateGetMonth()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return tm.tm_mon + 1;
}
int dateGetDay()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return tm.tm_mday;
}
int dateGetHour()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return tm.tm_hour;
}
int dateGetHour12()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return ((tm.tm_hour + 11) % 12) + 1;
}
int dateGetMinute()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return tm.tm_min;
}
int dateGetSecond()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return tm.tm_sec;
}

long long dateGetSecondsSinceEpoch()
{
    return time(NULL);
}

static const char* const s_MonthName[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
static const char* const s_MonthNameShort[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static const char* const s_WeekDayName[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
static const char* const s_WeekDayNameShort[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

const char* dateGetMonthName()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return s_MonthName[tm.tm_mon];
}
const char* dateGetMonthNameShort()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return s_MonthNameShort[tm.tm_mon];
}
const char* dateGetWeekDayName()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return s_WeekDayName[tm.tm_wday];
}
const char* dateGetWeekDayNameShort()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return s_WeekDayNameShort[tm.tm_wday];
}
const char* dateGetTimeMeridiem()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    if (tm.tm_hour < 12)
        return "AM";
    else
        return "PM";
}
const char* dateGetTimeMeridiemLower()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    if (tm.tm_hour < 12)
        return "am";
    else
        return "pm";
}

LvnString dateGetTimeHHMMSS()
{
    char buff[9];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buff, 9, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
    return LvnString(buff);
}
LvnString dateGetTime12HHMMSS()
{
    char buff[9];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buff, 9, "%02d:%02d:%02d", ((tm.tm_hour + 11) % 12) + 1, tm.tm_min, tm.tm_sec);
    return LvnString(buff);
}
LvnString dateGetYearStr()
{
    char buff[5];
    snprintf(buff, 5, "%d", dateGetYear());
    return LvnString(buff);
}
LvnString dateGetYear02dStr()
{
    char buff[3];
    snprintf(buff, 3, "%d", dateGetYear02d());
    return LvnString(buff);
}
LvnString dateGetMonthNumStr()
{
    char buff[3];
    snprintf(buff, 3, "%02d", dateGetMonth());
    return LvnString(buff);
}
LvnString dateGetDayNumStr()
{
    char buff[3];
    snprintf(buff, 3, "%02d", dateGetDay());
    return LvnString(buff);
}
LvnString dateGetHourNumStr()
{
    char buff[3];
    snprintf(buff, 3, "%02d", dateGetHour());
    return LvnString(buff);
}
LvnString dateGetHour12NumStr()
{
    char buff[3];
    snprintf(buff, 3, "%02d", dateGetHour12());
    return LvnString(buff);
}
LvnString dateGetMinuteNumStr()
{
    char buff[3];
    snprintf(buff, 3, "%02d", dateGetMinute());
    return LvnString(buff);
}
LvnString dateGetSecondNumStr()
{
    char buff[3];
    snprintf(buff, 3, "%02d", dateGetSecond());
    return LvnString(buff);
}

// logging

LvnResult initLogging(LvnLoggingContextCreateInfo* createInfo)
{
    if (s_LoggingContext)
        return Lvn_Result_AlreadyCalled;

    s_LoggingContext = lvn::memNew<LvnLoggingContext>();

    LvnLoggingContext* logctx = lvn::getLoggingContex();
    logctx->logging = true;

    logctx->enableCoreLogging = !createInfo->core.disableCoreLogging;

    // core
    if (!createInfo->core.name.empty())
        logctx->coreLogger.loggerName = createInfo->core.name;
    else
        logctx->coreLogger.loggerName = "CORE";

    if (createInfo->core.level != Lvn_LogLevel_None)
        logctx->coreLogger.logLevel = createInfo->core.level;
    else
        logctx->coreLogger.logLevel = Lvn_LogLevel_None;

    if (!createInfo->core.logPattern.empty())
    {
        logctx->coreLogger.logPatternFormat = createInfo->core.logPattern;
        logctx->coreLogger.logPatterns = lvn::logParseFormat(createInfo->core.logPattern.c_str());
    }
    else
    {
        logctx->coreLogger.logPatternFormat = LVN_DEFAULT_LOG_PATTERN;
        logctx->coreLogger.logPatterns = lvn::logParseFormat(LVN_DEFAULT_LOG_PATTERN);
    }

    // client
    if (!createInfo->client.name.empty())
        logctx->clientLogger.loggerName = createInfo->client.name;
    else
        logctx->clientLogger.loggerName = "CLIENT";

    if (createInfo->client.level != Lvn_LogLevel_None)
        logctx->clientLogger.logLevel = createInfo->client.level;
    else
        logctx->clientLogger.logLevel = Lvn_LogLevel_None;

    if (!createInfo->client.logPattern.empty())
    {
        logctx->clientLogger.logPatternFormat = createInfo->client.logPattern;
        logctx->clientLogger.logPatterns = lvn::logParseFormat(createInfo->client.logPattern.c_str());
    }
    else
    {
        logctx->clientLogger.logPatternFormat = LVN_DEFAULT_LOG_PATTERN;
        logctx->clientLogger.logPatterns = lvn::logParseFormat(LVN_DEFAULT_LOG_PATTERN);
    }

    #ifdef LVN_PLATFORM_WINDOWS
    enableLogANSIcodeColors();
    #endif

    return Lvn_Result_Success;
}

void terminateLogging()
{
    if (!s_LoggingContext)
        return;

    if (s_MemAllocCount > 0)
    {
        lvn::logCoreError("not all memory allocations have been freed, number of allocations remaining: %zu", s_MemAllocCount);
    }

    if (s_LoggingContext->coreLogger.logfile.logToFile)
    {
        fclose(static_cast<FILE*>(s_LoggingContext->coreLogger.logfile.fileptr));
        s_LoggingContext->coreLogger.logfile.fileptr = nullptr;
    }
    if (s_LoggingContext->clientLogger.logfile.logToFile)
    {
        fclose(static_cast<FILE*>(s_LoggingContext->clientLogger.logfile.fileptr));
        s_LoggingContext->clientLogger.logfile.fileptr = nullptr;
    }

    lvn::memDelete<LvnLoggingContext>(s_LoggingContext);
}

LvnLoggingContext* getLoggingContex()
{
    LVN_ASSERT(s_LoggingContext, "cannot get logging context, logging context was not created");
    return s_LoggingContext;
}

void logEnable(bool enable)
{
    lvn::getLoggingContex()->logging = enable;
}

void logEnableCoreLogging(bool enable)
{
    lvn::getLoggingContex()->enableCoreLogging = enable;
}

void logSetLevel(LvnLogger* logger, LvnLogLevel level)
{
    logger->logLevel = level;
}

void logSetFileConfig(LvnLogger* logger, bool enable, const char* filename, LvnFileMode filemode)
{
    // if log to file was enabled before, fileptr needs to be closed
    if (logger->logfile.logToFile)
    {
        fclose(static_cast<FILE*>(logger->logfile.fileptr));
        logger->logfile.fileptr = nullptr;
    }

    logger->logfile.logToFile = enable;
    logger->logfile.filename = filename;
    logger->logfile.filemode = filemode;

    if (enable)
    {
        if (logger->logfile.filename.empty())
        {
            logger->logfile.filename = logger->loggerName + "_logs.txt";
            lvn::logCoreWarn("logSetFileConfig(LvnLogger*, bool enable, const char* filename, LvnFileMode filemode) | filename not set, setting file name to name of the logger: %s_logs.txt", logger->loggerName.c_str());
        }

        const char* filemode = "w";
        if (logger->logfile.filemode == Lvn_FileMode_Write) filemode = "w";
        else if (logger->logfile.filemode == Lvn_FileMode_Append) filemode = "a";

        logger->logfile.fileptr = fopen(logger->logfile.filename.c_str(), filemode);
    }
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
    if constexpr (!ic_LvnEnableLogging) { return; }
    if (!lvn::getLoggingContex()->logging) { return; }

    LvnString msgstr; msgstr.reserve(strlen(msg->msg) + 1);

    for (uint32_t i = 0; i < logger->logPatterns.size(); i++)
    {
        if (logger->logPatterns[i].func == nullptr) // no special format character '%' found
        {
            msgstr += logger->logPatterns[i].symbol;
        }
        else // call func of special format
        {
            msgstr += logger->logPatterns[i].func(msg);
        }
    }

    printf("%s", msgstr.c_str());
}

LvnString logFormatMessage(LvnLogger* logger, LvnLogLevel level, const char* msg, bool removeANSI)
{
    LvnLogMessage logMsg{};
    logMsg.msg = msg;
    logMsg.loggerName = logger->loggerName.c_str();
    logMsg.level = level;
    logMsg.timeEpoch = lvn::dateGetSecondsSinceEpoch();

    LvnString msgstr; msgstr.reserve(strlen(msg) + 1);

    for (uint32_t i = 0; i < logger->logPatterns.size(); i++)
    {
        if (removeANSI && (logger->logPatterns[i].symbol == '#' || logger->logPatterns[i].symbol == '^'))
            continue;

        if (logger->logPatterns[i].func == nullptr) // no special format character '%' found
        {
            msgstr += logger->logPatterns[i].symbol;
        }
        else // call func of special format
        {
            msgstr += logger->logPatterns[i].func(&logMsg);
        }
    }

    return msgstr;
}

void logMessage(LvnLogger* logger, LvnLogLevel level, const char* msg)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    if (!lvn::getLoggingContex()->logging) { return; }

    LvnLogMessage logMsg{};
    logMsg.msg = msg;
    logMsg.loggerName = logger->loggerName.c_str();
    logMsg.level = level;
    logMsg.timeEpoch = lvn::dateGetSecondsSinceEpoch();

    lvn::logOutputMessage(logger, &logMsg);

    if (logger->logfile.logToFile)
    {
        LvnString msgstr; msgstr.reserve(strlen(msg) + 1);

        for (uint32_t i = 0; i < logger->logPatterns.size(); i++)
        {
            if (logger->logPatterns[i].symbol == '#' || logger->logPatterns[i].symbol == '^')
                continue;

            if (logger->logPatterns[i].func == nullptr) // no special format character '%' found
            {
                msgstr += logger->logPatterns[i].symbol;
            }
            else // call func of special format
            {
                msgstr += logger->logPatterns[i].func(&logMsg);
            }
        }

        fprintf(static_cast<FILE*>(logger->logfile.fileptr), "%s", msgstr.c_str());
    }
}

void logMessageTrace(LvnLogger* logger, const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    LvnLoggingContext* logctx = lvn::getLoggingContex();
    if (!logctx || !logctx->logging) { return; }
    if (!logctx->enableCoreLogging && logger == &logctx->coreLogger) { return; }
    if (!lvn::logCheckLevel(logger, Lvn_LogLevel_Trace)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(logger, Lvn_LogLevel_Trace, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

void logMessageDebug(LvnLogger* logger, const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    LvnLoggingContext* logctx = lvn::getLoggingContex();
    if (!logctx || !logctx->logging) { return; }
    if (!logctx->enableCoreLogging && logger == &logctx->coreLogger) { return; }
    if (!lvn::logCheckLevel(logger, Lvn_LogLevel_Debug)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(logger, Lvn_LogLevel_Debug, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

void logMessageInfo(LvnLogger* logger, const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    LvnLoggingContext* logctx = lvn::getLoggingContex();
    if (!logctx || !logctx->logging) { return; }
    if (!logctx->enableCoreLogging && logger == &logctx->coreLogger) { return; }
    if (!lvn::logCheckLevel(logger, Lvn_LogLevel_Info)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(logger, Lvn_LogLevel_Info, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

void logMessageWarn(LvnLogger* logger, const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    LvnLoggingContext* logctx = lvn::getLoggingContex();
    if (!logctx || !logctx->logging) { return; }
    if (!logctx->enableCoreLogging && logger == &logctx->coreLogger) { return; }
    if (!lvn::logCheckLevel(logger, Lvn_LogLevel_Warn)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(logger, Lvn_LogLevel_Warn, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

void logMessageError(LvnLogger* logger, const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    LvnLoggingContext* logctx = lvn::getLoggingContex();
    if (!logctx || !logctx->logging) { return; }
    if (!logctx->enableCoreLogging && logger == &logctx->coreLogger) { return; }
    if (!lvn::logCheckLevel(logger, Lvn_LogLevel_Error)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(logger, Lvn_LogLevel_Error, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

void logMessageFatal(LvnLogger* logger, const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    LvnLoggingContext* logctx = lvn::getLoggingContex();
    if (!logctx || !logctx->logging) { return; }
    if (!logctx->enableCoreLogging && logger == &logctx->coreLogger) { return; }
    if (!lvn::logCheckLevel(logger, Lvn_LogLevel_Fatal)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(logger, Lvn_LogLevel_Fatal, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

void logTrace(const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);
    lvn::logMessageTrace(&lvn::getLoggingContex()->clientLogger, fmt, argptr);
    va_end(argcopy);
    va_end(argptr);
}

void logDebug(const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);
    lvn::logMessageDebug(&lvn::getLoggingContex()->clientLogger, fmt, argptr);
    va_end(argcopy);
    va_end(argptr);
}

void logInfo(const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);
    lvn::logMessageInfo(&lvn::getLoggingContex()->clientLogger, fmt, argptr);
    va_end(argcopy);
    va_end(argptr);
}

void logWarn(const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);
    lvn::logMessageWarn(&lvn::getLoggingContex()->clientLogger, fmt, argptr);
    va_end(argcopy);
    va_end(argptr);
}

void logError(const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);
    lvn::logMessageError(&lvn::getLoggingContex()->clientLogger, fmt, argptr);
    va_end(argcopy);
    va_end(argptr);
}

void logFatal(const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);
    lvn::logMessageFatal(&lvn::getLoggingContex()->clientLogger, fmt, argptr);
    va_end(argcopy);
    va_end(argptr);
}

void logCoreTrace(const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);
    lvn::logMessageTrace(&lvn::getLoggingContex()->coreLogger, fmt, argptr);
    va_end(argcopy);
    va_end(argptr);
}

void logCoreDebug(const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);
    lvn::logMessageDebug(&lvn::getLoggingContex()->coreLogger, fmt, argptr);
    va_end(argcopy);
    va_end(argptr);
}

void logCoreInfo(const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);
    lvn::logMessageInfo(&lvn::getLoggingContex()->coreLogger, fmt, argptr);
    va_end(argcopy);
    va_end(argptr);

}

void logCoreWarn(const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);
    lvn::logMessageWarn(&lvn::getLoggingContex()->coreLogger, fmt, argptr);
    va_end(argcopy);
    va_end(argptr);

}

void logCoreError(const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);
    lvn::logMessageError(&lvn::getLoggingContex()->coreLogger, fmt, argptr);
    va_end(argcopy);
    va_end(argptr);

}

void logCoreFatal(const char* fmt, ...)
{
    if constexpr (!ic_LvnEnableLogging) { return; }
    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);
    lvn::logMessageFatal(&lvn::getLoggingContex()->coreLogger, fmt, argptr);
    va_end(argcopy);
    va_end(argptr);
}

LvnLogger* logGetCoreLogger()
{
    return &lvn::getLoggingContex()->coreLogger;
}

LvnLogger* logGetClientLogger()
{
    return &lvn::getLoggingContex()->clientLogger;
}

const char* logGetANSIcodeColor(LvnLogLevel level)
{
    switch (level)
    {
        case Lvn_LogLevel_None:     { return LVN_LOG_COLOR_RESET; }
        case Lvn_LogLevel_Trace:    { return LVN_LOG_COLOR_TRACE; }
        case Lvn_LogLevel_Debug:    { return LVN_LOG_COLOR_DEBUG; }
        case Lvn_LogLevel_Info:     { return LVN_LOG_COLOR_INFO; }
        case Lvn_LogLevel_Warn:     { return LVN_LOG_COLOR_WARN; }
        case Lvn_LogLevel_Error:    { return LVN_LOG_COLOR_ERROR; }
        case Lvn_LogLevel_Fatal:    { return LVN_LOG_COLOR_FATAL; }
    }

    return nullptr;
}

LvnResult logSetPatternFormat(LvnLogger* logger, const char* patternfmt)
{
    if (!logger) { return Lvn_Result_Failure; }
    if (!patternfmt || patternfmt[0] == '\0') { return Lvn_Result_Failure; }

    logger->logPatternFormat = patternfmt;

    logger->logPatterns = lvn::logParseFormat(patternfmt);

    return Lvn_Result_Success;
}

LvnResult logAddPatterns(LvnLogPattern* pLogPatterns, uint32_t count)
{
    if (!pLogPatterns) { return Lvn_Result_Failure; }
    if (pLogPatterns->symbol == '\0') { return Lvn_Result_Failure; }

    for (uint32_t i = 0; i < sizeof(s_LogPatterns) / sizeof(LvnLogPattern); i++)
    {
        for (uint32_t j = 0; j < count; j++)
        {
            if (pLogPatterns[j].symbol == s_LogPatterns[i].symbol) { return Lvn_Result_Failure; }
        }
    }

    LvnLoggingContext* logctx = lvn::getLoggingContex();
    logctx->userLogPatterns.insert(logctx->userLogPatterns.end(), pLogPatterns, pLogPatterns + count);

    return Lvn_Result_Success;
}

LvnResult createLogger(LvnLogger** logger, const LvnLoggerCreateInfo* loggerCreateInfo)
{
    LvnLoggingContext* logctx = lvn::getLoggingContex();

    *logger = lvn::memNew<LvnLogger>();
    LvnLogger* loggerPtr = *logger;

    loggerPtr->loggerName = loggerCreateInfo->loggerName;
    loggerPtr->logPatternFormat = loggerCreateInfo->format;
    loggerPtr->logLevel = loggerCreateInfo->level;

    loggerPtr->logfile.logToFile = loggerCreateInfo->fileConfig.enableLogToFile;
    loggerPtr->logfile.filename = loggerCreateInfo->fileConfig.filename;
    loggerPtr->logfile.filemode = loggerCreateInfo->fileConfig.filemode;

    if (loggerPtr->logfile.logToFile)
    {
        if (loggerPtr->logfile.filename.empty())
        {
            lvn::logCoreError("createLogger(LvnLogger**, LvnLoggerCreateInfo*) | loggerCreateInfo->fileConfig.filename is empty, cannot log to a file without a valid file path/name");
            return Lvn_Result_Failure;
        }

        const char* filemode = "w";
        if (loggerPtr->logfile.filemode == Lvn_FileMode_Write) filemode = "w";
        else if (loggerPtr->logfile.filemode == Lvn_FileMode_Append) filemode = "a";

        loggerPtr->logfile.fileptr = fopen(loggerPtr->logfile.filename.c_str(), filemode);
    }

    loggerPtr->logPatterns = lvn::logParseFormat(loggerCreateInfo->format.c_str());

    lvn::logCoreTrace("created logger: (%p), name: \"%s\"", *logger, loggerCreateInfo->loggerName.c_str());
    return Lvn_Result_Success;
}

void destroyLogger(LvnLogger* logger)
{
    if (logger == nullptr) { return; }

    if (logger->logfile.logToFile)
    {
        fclose(static_cast<FILE*>(logger->logfile.fileptr));
        logger->logfile.fileptr = nullptr;
    }

    LvnLoggingContext* logctx = lvn::getLoggingContex();
    lvn::memDelete<LvnLogger>(logger);
}

} /* namespace lvn */
