#include "levikno.h"
#include "levikno_internal.h"
#include "lvn_config.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <ctime>


#define LVN_DEFAULT_LOG_PATTERN "[%Y-%m-%d] [%T] [%#%l%^] %n: %v%$"

static LvnContext* s_LvnContext = nullptr;

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
static const char*                  getStypeEnumName(LvnStructureType stype);

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
    { 'l', [](LvnLogMessage* msg) -> LvnString { return lvn::getLogLevelName(msg->level); }},
    { '#', [](LvnLogMessage* msg) -> LvnString { return lvn::getLogLevelColor(msg->level); }},
    { '^', [](LvnLogMessage* msg) -> LvnString { return LVN_LOG_COLOR_RESET; }},
    { 'v', [](LvnLogMessage* msg) -> LvnString { return msg->msg; }},
    { '%', [](LvnLogMessage* msg) -> LvnString { return "%"; } },
    { 'T', [](LvnLogMessage* msg) -> LvnString { return lvn::dateGetTimeHHMMSS(); } },
    { 't', [](LvnLogMessage* msg) -> LvnString { return lvn::dateGetTime12HHMMSS(); } },
    { 'Y', [](LvnLogMessage* msg) -> LvnString { return lvn::dateGetYearStr(); }},
    { 'y', [](LvnLogMessage* msg) -> LvnString { return lvn::dateGetYear02dStr(); } },
    { 'm', [](LvnLogMessage* msg) -> LvnString { return lvn::dateGetMonthNumStr(); } },
    { 'B', [](LvnLogMessage* msg) -> LvnString { return lvn::dateGetMonthName(); } },
    { 'b', [](LvnLogMessage* msg) -> LvnString { return lvn::dateGetMonthNameShort(); } },
    { 'd', [](LvnLogMessage* msg) -> LvnString { return lvn::dateGetDayNumStr(); } },
    { 'A', [](LvnLogMessage* msg) -> LvnString { return lvn::dateGetWeekDayName(); } },
    { 'a', [](LvnLogMessage* msg) -> LvnString { return lvn::dateGetWeekDayNameShort(); } },
    { 'H', [](LvnLogMessage* msg) -> LvnString { return lvn::dateGetHourNumStr(); } },
    { 'h', [](LvnLogMessage* msg) -> LvnString { return lvn::dateGetHour12NumStr(); } },
    { 'M', [](LvnLogMessage* msg) -> LvnString { return lvn::dateGetMinuteNumStr(); } },
    { 'S', [](LvnLogMessage* msg) -> LvnString { return lvn::dateGetSecondNumStr(); } },
    { 'P', [](LvnLogMessage* msg) -> LvnString { return lvn::dateGetTimeMeridiem(); } },
    { 'p', [](LvnLogMessage* msg) -> LvnString { return lvn::dateGetTimeMeridiemLower(); }},
};

static LvnVector<LvnLogPattern> logParseFormat(const char* fmt)
{
    if (!fmt || !*fmt) { return {}; }

    LvnContext* lvnctx = lvn::getContext();

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
        for (uint32_t j = 0; j < lvnctx->userLogPatterns.size(); j++)
        {
            if (fmt[i + 1] != lvnctx->userLogPatterns[j].symbol)
                continue;

            patterns.push_back(lvnctx->userLogPatterns[j]);
        }

        i++; // incramant past symbol on next character in format
    }

    return patterns;
}

static const char* getStypeEnumName(LvnStructureType stype)
{
    switch (stype)
    {
        case Lvn_Stype_Undefined:          { return "Undefined"; }
        case Lvn_Stype_Window:             { return "LvnWindow"; }
        case Lvn_Stype_Logger:             { return "LvnLogger"; }
        case Lvn_Stype_FrameBuffer:        { return "LvnFrameBuffer"; }
        case Lvn_Stype_Shader:             { return "LvnShader"; }
        case Lvn_Stype_DescriptorLayout:   { return "LvnDescriptorLayout"; }
        case Lvn_Stype_Pipeline:           { return "LvnPipeline"; }
        case Lvn_Stype_Buffer:             { return "LvnBuffer"; }
        case Lvn_Stype_Sampler:            { return "LvnSampler"; }
        case Lvn_Stype_Texture:            { return "LvnTexture"; }
        case Lvn_Stype_Cubemap:            { return "LvnCubemap"; }
        case Lvn_Stype_Sound:              { return "LvnSound"; }
        case Lvn_Stype_Socket:             { return "LvnSocket"; }

        default:                           { return ""; }
    }
}

void* memAlloc(size_t size)
{
    if (size == 0) { return nullptr; }
    void* allocmem = (*s_MemAllocFunc)(size, s_MemAllocUserData);
    if (!allocmem) { LVN_CORE_ERROR("malloc failure, could not allocate memory!"); exit(-1); }
    memset(allocmem, 0, size);
    lvn::getContext()->memAllocCount++;
    return allocmem;
}

void memFree(void* ptr)
{
    if (ptr == nullptr) { return; }
    (*s_MemFreeFunc)(ptr, s_MemAllocUserData);
    lvn::getContext()->memAllocCount--;
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
    return lvn::getContext()->memAllocCount;
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

size_t dateGetSecondsSinceEpoch()
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

LvnResult initContext(LvnContextCreateInfo* createInfo)
{
    if (s_LvnContext)
        return Lvn_Result_AlreadyCalled;

    s_LvnContext = lvn::memNew<LvnContext>();

    LvnContext* lvnctx = lvn::getContext();
    lvnctx->logging = true;

    lvnctx->enableCoreLogging = createInfo == nullptr ? true : !createInfo->logging.core.disableCoreLogging;

    // core
    if (createInfo != nullptr && !createInfo->logging.core.name.empty())
        lvnctx->coreLogger.loggerName = createInfo->logging.core.name;
    else
        lvnctx->coreLogger.loggerName = "CORE";

    if (createInfo != nullptr && createInfo->logging.core.level != Lvn_LogLevel_None)
        lvnctx->coreLogger.logLevel = createInfo->logging.core.level;
    else
        lvnctx->coreLogger.logLevel = Lvn_LogLevel_None;

    if (createInfo != nullptr && !createInfo->logging.core.logPattern.empty())
    {
        lvnctx->coreLogger.logPatternFormat = createInfo->logging.core.logPattern;
        lvnctx->coreLogger.logPatterns = lvn::logParseFormat(createInfo->logging.core.logPattern.c_str());
    }
    else
    {
        lvnctx->coreLogger.logPatternFormat = LVN_DEFAULT_LOG_PATTERN;
        lvnctx->coreLogger.logPatterns = lvn::logParseFormat(LVN_DEFAULT_LOG_PATTERN);
    }

    // client
    if (createInfo != nullptr && !createInfo->logging.client.name.empty())
        lvnctx->clientLogger.loggerName = createInfo->logging.client.name;
    else
        lvnctx->clientLogger.loggerName = "CLIENT";

    if (createInfo != nullptr && createInfo->logging.client.level != Lvn_LogLevel_None)
        lvnctx->clientLogger.logLevel = createInfo->logging.client.level;
    else
        lvnctx->clientLogger.logLevel = Lvn_LogLevel_None;

    if (createInfo != nullptr && !createInfo->logging.client.logPattern.empty())
    {
        lvnctx->clientLogger.logPatternFormat = createInfo->logging.client.logPattern;
        lvnctx->clientLogger.logPatterns = lvn::logParseFormat(createInfo->logging.client.logPattern.c_str());
    }
    else
    {
        lvnctx->clientLogger.logPatternFormat = LVN_DEFAULT_LOG_PATTERN;
        lvnctx->clientLogger.logPatterns = lvn::logParseFormat(LVN_DEFAULT_LOG_PATTERN);
    }

    #ifdef LVN_PLATFORM_WINDOWS
    enableLogANSIcodeColors();
    #endif

    return Lvn_Result_Success;
}

void terminateContext()
{
    if (!s_LvnContext)
        return;

    for (uint32_t i = 0; i < Lvn_Stype_Max_Value; i++)
    {
        if (s_LvnContext->sTypeMemoryAllocationCounts[i] > 0)
            LVN_CORE_ERROR("<createObject>: not all %s objects have been destroyed, sType id: (%u), number of objects remaining: %zu", lvn::getStypeEnumName(static_cast<LvnStructureType>(i)), i, s_LvnContext->sTypeMemoryAllocationCounts[i]);
    }

    if (s_LvnContext->memAllocCount > 0)
        LVN_CORE_ERROR("<memAlloc>: not all memory allocations have been freed, number of allocations remaining: %zu", s_LvnContext->memAllocCount);

    if (s_LvnContext->coreLogger.logfile.logToFile)
    {
        fclose(static_cast<FILE*>(s_LvnContext->coreLogger.logfile.fileptr));
        s_LvnContext->coreLogger.logfile.fileptr = nullptr;
    }
    if (s_LvnContext->clientLogger.logfile.logToFile)
    {
        fclose(static_cast<FILE*>(s_LvnContext->clientLogger.logfile.fileptr));
        s_LvnContext->clientLogger.logfile.fileptr = nullptr;
    }

    lvn::memDelete<LvnContext>(s_LvnContext);
}

LvnContext* getContext()
{
    LVN_ASSERT(s_LvnContext != nullptr, "cannot get context, context was not created");
    return s_LvnContext;
}

void logEnable(bool enable)
{
    lvn::getContext()->logging = enable;
}

void logEnableCoreLogging(bool enable)
{
    lvn::getContext()->enableCoreLogging = enable;
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
            LVN_CORE_WARN("logSetFileConfig(LvnLogger*, bool enable, const char* filename, LvnFileMode filemode) | filename not set, setting file name to name of the logger: %s_logs.txt", logger->loggerName.c_str());
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
    if (!lvn::getContext()->logging) { return; }

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
    if (!lvn::getContext()->logging) { return; }

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
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging) { return; }
    if (!lvnctx->enableCoreLogging && logger == &lvnctx->coreLogger) { return; }
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
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging) { return; }
    if (!lvnctx->enableCoreLogging && logger == &lvnctx->coreLogger) { return; }
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
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging) { return; }
    if (!lvnctx->enableCoreLogging && logger == &lvnctx->coreLogger) { return; }
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
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging) { return; }
    if (!lvnctx->enableCoreLogging && logger == &lvnctx->coreLogger) { return; }
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
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging) { return; }
    if (!lvnctx->enableCoreLogging && logger == &lvnctx->coreLogger) { return; }
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
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging) { return; }
    if (!lvnctx->enableCoreLogging && logger == &lvnctx->coreLogger) { return; }
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
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging) { return; }
    if (!lvn::logCheckLevel(&lvnctx->clientLogger, Lvn_LogLevel_Trace)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(&lvnctx->clientLogger, Lvn_LogLevel_Trace, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

void logDebug(const char* fmt, ...)
{
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging) { return; }
    if (!lvn::logCheckLevel(&lvnctx->clientLogger, Lvn_LogLevel_Debug)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(&lvnctx->clientLogger, Lvn_LogLevel_Debug, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

void logInfo(const char* fmt, ...)
{
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging) { return; }
    if (!lvn::logCheckLevel(&lvnctx->clientLogger, Lvn_LogLevel_Info)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(&lvnctx->clientLogger, Lvn_LogLevel_Info, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

void logWarn(const char* fmt, ...)
{
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging) { return; }
    if (!lvn::logCheckLevel(&lvnctx->clientLogger, Lvn_LogLevel_Warn)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(&lvnctx->clientLogger, Lvn_LogLevel_Warn, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

void logError(const char* fmt, ...)
{
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging) { return; }
    if (!lvn::logCheckLevel(&lvnctx->clientLogger, Lvn_LogLevel_Error)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(&lvnctx->clientLogger, Lvn_LogLevel_Error, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

void logFatal(const char* fmt, ...)
{
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging) { return; }
    if (!lvn::logCheckLevel(&lvnctx->clientLogger, Lvn_LogLevel_Fatal)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(&lvnctx->clientLogger, Lvn_LogLevel_Fatal, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

void logCoreTrace(const char* fmt, ...)
{
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging || !lvnctx->enableCoreLogging) { return; }
    if (!lvn::logCheckLevel(&lvnctx->coreLogger, Lvn_LogLevel_Trace)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(&lvnctx->coreLogger, Lvn_LogLevel_Trace, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

void logCoreDebug(const char* fmt, ...)
{
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging || !lvnctx->enableCoreLogging) { return; }
    if (!lvn::logCheckLevel(&lvnctx->coreLogger, Lvn_LogLevel_Debug)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(&lvnctx->coreLogger, Lvn_LogLevel_Debug, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

void logCoreInfo(const char* fmt, ...)
{
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging || !lvnctx->enableCoreLogging) { return; }
    if (!lvn::logCheckLevel(&lvnctx->coreLogger, Lvn_LogLevel_Info)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(&lvnctx->coreLogger, Lvn_LogLevel_Info, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

void logCoreWarn(const char* fmt, ...)
{
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging || !lvnctx->enableCoreLogging) { return; }
    if (!lvn::logCheckLevel(&lvnctx->coreLogger, Lvn_LogLevel_Warn)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(&lvnctx->coreLogger, Lvn_LogLevel_Warn, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

void logCoreError(const char* fmt, ...)
{
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging || !lvnctx->enableCoreLogging) { return; }
    if (!lvn::logCheckLevel(&lvnctx->coreLogger, Lvn_LogLevel_Error)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(&lvnctx->coreLogger, Lvn_LogLevel_Error, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

void logCoreFatal(const char* fmt, ...)
{
    LvnContext* lvnctx = lvn::getContext();
    if (!lvnctx || !lvnctx->logging || !lvnctx->enableCoreLogging) { return; }
    if (!lvn::logCheckLevel(&lvnctx->coreLogger, Lvn_LogLevel_Fatal)) { return; }

    LvnVector<char> buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(nullptr, 0, fmt, argptr);
    buff.resize(len + 1);
    vsnprintf(&buff[0], len + 1, fmt, argcopy);
    lvn::logMessage(&lvnctx->coreLogger, Lvn_LogLevel_Fatal, buff.data());

    va_end(argcopy);
    va_end(argptr);
}

LvnLogger* logGetCoreLogger()
{
    return &lvn::getContext()->coreLogger;
}

LvnLogger* logGetClientLogger()
{
    return &lvn::getContext()->clientLogger;
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

    LvnContext* lvnctx = lvn::getContext();
    lvnctx->userLogPatterns.insert(lvnctx->userLogPatterns.end(), pLogPatterns, pLogPatterns + count);

    return Lvn_Result_Success;
}

LvnResult createLogger(LvnLogger** logger, const LvnLoggerCreateInfo* loggerCreateInfo)
{
    LvnContext* lvnctx = lvn::getContext();

    *logger = lvn::createObject<LvnLogger>(Lvn_Stype_Logger);
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
            LVN_CORE_ERROR("createLogger(LvnLogger**, LvnLoggerCreateInfo*) | loggerCreateInfo->fileConfig.filename is empty, cannot log to a file without a valid file path/name");
            return Lvn_Result_Failure;
        }

        const char* filemode = "w";
        if (loggerPtr->logfile.filemode == Lvn_FileMode_Write) filemode = "w";
        else if (loggerPtr->logfile.filemode == Lvn_FileMode_Append) filemode = "a";

        loggerPtr->logfile.fileptr = fopen(loggerPtr->logfile.filename.c_str(), filemode);
    }

    loggerPtr->logPatterns = lvn::logParseFormat(loggerCreateInfo->format.c_str());

    LVN_CORE_TRACE("created logger: (%p), name: \"%s\"", *logger, loggerCreateInfo->loggerName.c_str());
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

    LvnContext* lvnctx = lvn::getContext();
    lvn::destroyObject  <LvnLogger>(logger, Lvn_Stype_Logger);
}

LvnLoggerCreateInfo configLoggerInit(const char* loggerName, const char* logFormat, LvnLogLevel logLevel)
{
    LvnLoggerCreateInfo createInfo{};
    createInfo.loggerName = loggerName;
    createInfo.format = logFormat;
    createInfo.level = logLevel;

    createInfo.fileConfig.enableLogToFile = false;
    createInfo.fileConfig.filemode = Lvn_FileMode_Write;
    createInfo.fileConfig.filename = loggerName;

    return createInfo;
}

} /* namespace lvn */

// -- LvnString
LvnString::LvnString()
{
    m_Data = lvn::memNew<char>();
    m_Data[0] = '\0';
    m_Size = 0;
    m_Capacity = 1;
}
LvnString::~LvnString()
{
    lvn::memDelete<char>(m_Data);
    m_Size = m_Capacity = 0;
    m_Data = nullptr;
}
LvnString::LvnString(const char* str)
{
    m_Size = strlen(str);
    m_Capacity = m_Size + 1;
    m_Data = lvn::memNew<char>(m_Capacity);
    memcpy(m_Data, str, m_Capacity);
}
LvnString::LvnString(const char* data, size_t size)
{
    m_Size = size;
    m_Capacity = m_Size + 1;
    m_Data = lvn::memNew<char>(m_Capacity);
    memcpy(m_Data, data, m_Capacity);
    m_Data[m_Size] = '\0';
}
LvnString::LvnString(const LvnString& other)
{
    m_Size = other.m_Size;
    m_Capacity = other.m_Capacity;
    m_Data = lvn::memNew<char>(other.m_Capacity);
    memcpy(m_Data, other.m_Data, other.m_Capacity);
}
LvnString& LvnString::operator=(const LvnString& other)
{
    if (this == &other) return *this;
    lvn::memDelete<char>(m_Data);
    m_Size = other.m_Size;
    m_Capacity = other.m_Capacity;
    m_Data = lvn::memNew<char>(other.m_Capacity);
    memcpy(m_Data, other.m_Data, other.m_Capacity);
    return *this;
}

char& LvnString::operator [](size_t index)
{
    LVN_ASSERT(index < length(), "string index out of range");
    return m_Data[index];
}
const char& LvnString::operator [](size_t index) const
{
    LVN_ASSERT(index < length(), "string index out of range");
    return m_Data[index];
}

bool LvnString::operator ==(const LvnString& other)
{
    if (this->length() != other.length())
        return false;
    return memcmp(m_Data, other.m_Data, this->length()) == 0;
}
bool LvnString::operator !=(const LvnString& other)
{
    if (this->length() != other.length())
        return true;
    return memcmp(m_Data, other.m_Data, this->length()) != 0;
}
bool LvnString::operator ==(const char* str)
{
    if (!str) { return false; }
    if (this->length() != strlen(str)) { return false; }
    return memcmp(m_Data, str, length()) == 0;
}
bool LvnString::operator !=(const char* str)
{
    if (!str) { return true; }
    if (this->length() != strlen(str)) { return true; }
    return memcmp(m_Data, str, length()) != 0;
}
LvnString LvnString::operator+(const LvnString& other)
{
    return operator+(other.m_Data);
}
LvnString LvnString::operator+(const char* str)
{
    LvnString s;
    size_t strsize = length();
    s.resize(strsize + strlen(str));
    memcpy(s.data(), m_Data, strsize);
    memcpy(s.data() + strsize, str, strlen(str));
    return s;
}
void LvnString::operator+=(const LvnString& other)
{
    append(other.c_str());
}
void LvnString::operator+=(const char* str)
{
    append(str);
}
void LvnString::operator+=(const char& ch)
{
    append(ch);
}
void LvnString::append(const char* str)
{
    size_t strsize = strlen(str);
    resize(m_Size + strsize);
    memcpy(&m_Data[m_Size - strsize], str, strsize * sizeof(char));
}
void LvnString::append(const char& ch)
{
    resize(m_Size + 1);
    memcpy(&m_Data[m_Size - 1], &ch, sizeof(char));
}
LvnString LvnString::substr(size_t index)
{
    LVN_ASSERT(index < m_Size, "string index out of string bounds");
    LvnString s;
    s.resize(m_Size - index);
    memcpy(s.m_Data, &this->m_Data[index], m_Size - index);
    return s;
}
const LvnString LvnString::substr(size_t index) const
{
    LVN_ASSERT(index < m_Size, "string index out of string bounds");
    LvnString s;
    s.resize(m_Size - index);
    memcpy(s.m_Data, &this->m_Data[index], m_Size - index);
    return s;
}
LvnString LvnString::substr(size_t index, size_t len)
{
    LVN_ASSERT(index + len <= m_Size, "string index out of string bounds");
    LvnString s;
    s.resize(len);
    memcpy(s.m_Data, &this->m_Data[index], len);
    return s;
}
const LvnString LvnString::substr(size_t index, size_t len) const
{
    LVN_ASSERT(index + len <= m_Size, "string index out of string bounds");
    LvnString s;
    s.resize(len);
    memcpy(s.m_Data, &this->m_Data[index], len);
    return s;
}
void LvnString::insert(const char* it, const char& ch)
{
    LVN_ASSERT(it >= m_Data && it <= m_Data + m_Size, "insert element not within string bounds");
    size_t index = it - m_Data;
    insert_index(index, ch);
}
void LvnString::insert(const char* it, const char* begin, const char* end)
{
    LVN_ASSERT(it >= m_Data && it <= m_Data + m_Size, "insert element not within string bounds");
    LVN_ASSERT(end >= begin, "end insert element cannot be less than begin insert element");
    size_t index = it - m_Data;
    insert_index(index, begin, end);
}
void LvnString::insert(const char* it, const char* data, size_t size)
{
    if (size == 0) return;
    LVN_ASSERT(it >= m_Data && it <= m_Data + m_Size, "insert element not within string bounds");
    size_t index = it - m_Data;
    insert_index(index, data, size);
}
void LvnString::insert_index(size_t index, const char& ch)
{
    LVN_ASSERT(index <= m_Size, "insert index not within string bounds");
    resize(m_Size + 1);
    memmove(&m_Data[index + 1], &m_Data[index], (m_Size - index - 1) * sizeof(char));
    memcpy(&m_Data[index], &ch, sizeof(char));
}
void LvnString::insert_index(size_t index, const char* begin, const char* end)
{
    LVN_ASSERT(index <= m_Size, "insert index not within string bounds");
    LVN_ASSERT(end >= begin, "end insert element cannot be less than begin insert element");
    size_t count = end - begin;
    if (count == 0) return;
    resize(m_Size + count);
    memmove(&m_Data[index + count], &m_Data[index], (m_Size - index - count) * sizeof(char));
    memcpy(&m_Data[index], begin, count * sizeof(char));
}
void LvnString::insert_index(size_t index, const char* data, size_t size)
{
    if (size == 0) return;
    LVN_ASSERT(index <= m_Size, "insert index not within string bounds");
    resize(m_Size + size);
    memmove(&m_Data[index + size], &m_Data[index], (m_Size - index - size) * sizeof(char));
    memcpy(&m_Data[index], data, size * sizeof(char));
}
void LvnString::reserve(size_t size)
{
    if (size <= m_Capacity) { return; }
    char* temp = lvn::memNew<char>(size);
    memcpy(temp, m_Data, m_Size * sizeof(char));
    lvn::memDelete<char>(m_Data);
    m_Data = temp;
    m_Capacity = size;
}
void LvnString::resize(size_t size)
{
    reserve(size + 1);
    m_Size = size;
    m_Data[m_Size] = '\0';
}
void LvnString::clear()
{
    m_Size = 0;
    m_Data[m_Size] = '\0';
}
void LvnString::clear_free()
{
    lvn::memDelete<char>(m_Data);
    m_Data = nullptr;
    m_Size = m_Capacity = 0;
}
void LvnString::erase(const char* it)
{
    LVN_ASSERT(it >= m_Data && it < m_Data + m_Size, "erase element not within string bounds");
    size_t index = it - m_Data;
    erase_index(index);
}
void LvnString::erase_index(size_t index)
{
    LVN_ASSERT(index < m_Size, "index out of vector size range");
    size_t aftIndex = m_Size - index - 1;
    if (aftIndex != 0)
        memcpy(&m_Data[index], &m_Data[index + 1], aftIndex * sizeof(char));
    --m_Size;
}
void LvnString::push_back(const char& ch)
{
    resize(m_Size + 1);
    memcpy(&m_Data[m_Size - 1], &ch, sizeof(char));
}
void LvnString::push_range(const char* ch, size_t size)
{
    resize(m_Size + size);
    memcpy(&m_Data[m_Size - size], ch, size * sizeof(char));
}
void LvnString::pop_back()
{
    if (m_Size == 0) { return; }
    resize(m_Size - 1);
}
size_t LvnString::find(const LvnString& other) const
{
    return find(other.c_str());
}
size_t LvnString::rfind(const LvnString& other) const
{
    return rfind(other.c_str());
}
size_t LvnString::find(const char& ch) const
{
    if (m_Size == 0) { return LvnString::npos; }
    for (size_t i = 0; i < m_Size; i++)
    {
        if (m_Data[i] == ch)
            return i;
    }

    return LvnString::npos;
}
size_t LvnString::rfind(const char& ch) const
{
    if (m_Size == 0) { return LvnString::npos; }
    for (size_t i = m_Size - 1; i >= 0; i--)
    {
        if (m_Data[i] == ch)
            return i;

        if (i == 0) break;
    }

    return LvnString::npos;
}
size_t LvnString::find(const char* str) const
{
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str);
    if (strsize > m_Size) { return LvnString::npos; }

    for (size_t i = 0; i <= m_Size - strsize; i++)
    {
        bool match = true;
        for (size_t j = 0; j < strsize; j++)
        {
            if (m_Data[i + j] != str[j])
            {
                match = false;
                break;
            }
        }

        if (match) { return i; }
    }

    return LvnString::npos;
}
size_t LvnString::rfind(const char* str) const
{
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str);
    if (strsize > m_Size) { return LvnString::npos; }

    for (size_t i = m_Size - strsize; i >= 0; i--)
    {
        bool match = true;
        for (size_t j = 0; j < strsize; j++)
        {
            if (m_Data[i + j] != str[j])
            {
                match = false;
                break;
            }
        }

        if (match) { return i; }
        if (i == 0) break;
    }

    return LvnString::npos;
}
size_t LvnString::find_first_of(const LvnString& other, size_t index) const
{
    return find_first_of(other.c_str(), index);
}
size_t LvnString::find_first_of(const LvnString& other, size_t index, size_t length) const
{
    return find_first_of(other.c_str(), index, length);
}
size_t LvnString::find_first_of(const char& ch, size_t index) const
{
    LVN_ASSERT(index < m_Size, "index not within string bounds");
    for (size_t i = index; i < m_Size; i++)
        if (m_Data[i] == ch) { return i; }
    return LvnString::npos;
}
size_t LvnString::find_first_of(const char* str, size_t index) const
{
    LVN_ASSERT(index < m_Size, "index not within string bounds");
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str) + 1;
    return find_first_of(str, index, strsize);
}
size_t LvnString::find_first_of(const char* str, size_t index, size_t length) const
{
    LVN_ASSERT(index < m_Size, "index not within string bounds");
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str) + 1;
    LVN_ASSERT(length <= strsize, "length not within str size");

    for (size_t i = index; i < m_Size; i++)
    {
        for (size_t j = 0; j < length; j++)
        {
            if (m_Data[i] == str[j])
                return i;
        }
    }

    return LvnString::npos;
}
size_t LvnString::find_first_not_of(const LvnString& other, size_t index) const
{
    return find_first_not_of(other.c_str(), index);
}
size_t LvnString::find_first_not_of(const LvnString& other, size_t index, size_t length) const
{
    return find_first_not_of(other.c_str(), index, length);
}
size_t LvnString::find_first_not_of(const char& ch, size_t index) const
{
    LVN_ASSERT(index < m_Size, "index not within string bounds");
    for (size_t i = index; i < m_Size; i++)
        if (m_Data[i] != ch) { return i; }
    return LvnString::npos;
}
size_t LvnString::find_first_not_of(const char* str, size_t index) const
{
    LVN_ASSERT(index < m_Size, "index not within string bounds");
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str) + 1;
    return find_first_not_of(str, index, strsize);
}
size_t LvnString::find_first_not_of(const char* str, size_t index, size_t length) const
{
    LVN_ASSERT(index < m_Size, "index not within string bounds");
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str) + 1;
    LVN_ASSERT(length <= strsize, "length not within str size");

    for (size_t i = index; i < m_Size; i++)
    {
        bool found = false;
        for (size_t j = 0; j < length; j++)
        {
            if (m_Data[i] == str[j])
            {
                found = true;
                break;
            }
        }

        if (!found) { return i; }
    }

    return LvnString::npos;
}
size_t LvnString::find_last_of(const LvnString& other, size_t index) const
{
    return find_last_of(other.c_str(), index);
}
size_t LvnString::find_last_of(const LvnString& other, size_t index, size_t length) const
{
    return find_last_of(other.c_str(), index, length);
}
size_t LvnString::find_last_of(const char& ch, size_t index) const
{
    LVN_ASSERT(index < m_Size || index == LvnString::npos, "index not within string bounds");
    for (size_t i = m_Size - 1; i != LvnString::npos; i--)
        if (m_Data[i] == ch) { return i; }
    return LvnString::npos;
}
size_t LvnString::find_last_of(const char* str, size_t index) const
{
    LVN_ASSERT(index < m_Size || index == LvnString::npos, "index not within string bounds");
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str) + 1;
    return find_last_of(str, index, strsize);
}
size_t LvnString::find_last_of(const char* str, size_t index, size_t length) const
{
    LVN_ASSERT(index < m_Size || index == LvnString::npos, "index not within string bounds");
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str) + 1;
    LVN_ASSERT(length <= strsize, "length not within str size");

    for (size_t i = m_Size - 1; i != LvnString::npos; i--)
    {
        for (size_t j = 0; j < length; j++)
        {
            if (m_Data[i] == str[j])
                return i;
        }
    }

    return LvnString::npos;
}
size_t LvnString::find_last_not_of(const LvnString& other, size_t index) const
{
    return find_last_not_of(other.c_str(), index);
}
size_t LvnString::find_last_not_of(const LvnString& other, size_t index, size_t length) const
{
    return find_last_not_of(other.c_str(), index, length);
}
size_t LvnString::find_last_not_of(const char& ch, size_t index) const
{
    LVN_ASSERT(index < m_Size || index == LvnString::npos, "index not within string bounds");
    for (size_t i = m_Size - 1; i != LvnString::npos; i--)
        if (m_Data[i] != ch) { return i; }
    return LvnString::npos;
}
size_t LvnString::find_last_not_of(const char* str, size_t index) const
{
    LVN_ASSERT(index < m_Size || index == LvnString::npos, "index not within string bounds");
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str) + 1;
    return find_last_not_of(str, index, strsize);
}
size_t LvnString::find_last_not_of(const char* str, size_t index, size_t length) const
{
    LVN_ASSERT(index < m_Size || index == LvnString::npos, "index not within string bounds");
    if (!str || !*str || m_Size == 0) { return LvnString::npos; }
    size_t strsize = strlen(str) + 1;
    LVN_ASSERT(length <= strsize, "length not within str size");

    for (size_t i = m_Size - 1; i != LvnString::npos; i--)
    {
        bool found = false;
        for (size_t j = 0; j < length; j++)
        {
            if (m_Data[i] == str[j])
            {
                found = true;
                break;
            }
        }

        if (!found) { return i; }
    }

    return LvnString::npos;
}
bool LvnString::starts_with(const char& ch) const
{
    if (!m_Size) { return false; }
    return m_Data[0] == ch;
}
bool LvnString::ends_with(const char& ch) const
{
    if (!m_Size) { return false; }
    return m_Data[m_Size - 1] == ch;
}
bool LvnString::contains(const char& ch) const
{
    char* begin = m_Data;
    const char* end = m_Data + m_Size;
    while (begin < end)
    {
        if (*begin == ch) { return true; }
        begin++;
    }
    return false;
}
LvnString operator+(const char* str, const LvnString& other)
{
    LvnString s;
    size_t strsize = strlen(str);
    s.resize(strsize + other.length());
    memcpy(s.data(), str, strsize);
    memcpy(s.data() + strsize, other.data(), other.length());
    return s;
}
