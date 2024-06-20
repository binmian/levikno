#include "levikno.h"
#include "levikno_internal.h"

#include <cmath>
#include <cstdint>
#include <stdarg.h>
#include <time.h>

#include "stb_image.h"
#include "stb_truetype.h"
#include "stb_image_write.h"

#include "miniaudio.h"

#ifdef LVN_PLATFORM_WINDOWS
	#include <windows.h>
#endif

#define LVN_ABORT abort()
#define LVN_EMPTY_STR "\0"
#define LVN_DEFAULT_LOG_PATTERN "[%Y-%m-%d] [%T] [%#%l%^] %n: %v%$"

#include "lvn_glfw.h"

#if defined(LVN_GRAPHICS_API_INCLUDE_VULKAN)
	#include "lvn_vulkan.h"
#endif

#include "lvn_loadModel.h"

static LvnContext* s_LvnContext = nullptr;


namespace lvn
{

static void                         enableLogANSIcodeColors();
static LvnVector<LvnLogPattern>     logParseFormat(const char* fmt);
static const char*                  getLogLevelColor(LvnLogLevel level);
static const char*                  getLogLevelName(LvnLogLevel level);
static const char*                  getGraphicsApiNameEnum(LvnGraphicsApi api);
static const char*                  getWindowApiNameEnum(LvnWindowApi api);
static LvnResult                    setWindowContext(LvnWindowApi windowapi);
static void                         terminateWindowContext();
static LvnResult                    setGraphicsContext(LvnGraphicsApi graphicsapi, bool enableValidation);
static void                         terminateGraphicsContext();
static LvnResult                    initAudioContext();
static void                         terminateAudioContext();


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


/* [SECTION]: Core */
LvnResult createContext(LvnContextCreateInfo* createInfo)
{
	if (s_LvnContext != nullptr) { return Lvn_Result_AlreadyCalled; }
	s_LvnContext = new LvnContext();

	s_LvnContext->windowapi = createInfo->windowapi;
	s_LvnContext->graphicsapi = createInfo->graphicsapi;

	// logging
	if (createInfo->enableLogging) { logInit(); }

	s_LvnContext->coreLogger.logLevel = createInfo->coreLogLevel;

	// window context
	LvnResult result = setWindowContext(createInfo->windowapi);
	if (result != Lvn_Result_Success) { return result; }

	// graphics context
	result = setGraphicsContext(createInfo->graphicsapi, createInfo->enableVulkanValidationLayers);
	if (result != Lvn_Result_Success) { return result; }

	result = initAudioContext();
	if (result != Lvn_Result_Success) { return result; }

	return Lvn_Result_Success;
}

void terminateContext()
{
	if (s_LvnContext == nullptr) { return; }

	terminateWindowContext();
	terminateGraphicsContext();
	terminateAudioContext();

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
std::string dateGetTimeHHMMSS()
{
	char buff[9];
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	snprintf(buff, 9, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
	return std::string(buff);
}
std::string dateGetTime12HHMMSS()
{
	char buff[9];
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	snprintf(buff, 9, "%02d:%02d:%02d", ((tm.tm_hour + 11) % 12) + 1, tm.tm_min, tm.tm_sec);
	return std::string(buff);
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

std::string dateGetYearStr()
{
	char buff[5];
	snprintf(buff, 5, "%d", dateGetYear());
	return std::string(buff);
}
std::string dateGetYear02dStr()
{
	char buff[3];
	snprintf(buff, 3, "%d", dateGetYear02d());
	return std::string(buff);
}
std::string dateGetMonthNumStr()
{
	char buff[3];
	snprintf(buff, 3, "%02d", dateGetMonth());
	return std::string(buff);
}
std::string dateGetDayNumStr()
{
	char buff[3];
	snprintf(buff, 3, "%02d", dateGetDay());
	return std::string(buff);
}
std::string dateGetHourNumStr()
{
	char buff[3];
	snprintf(buff, 3, "%02d", dateGetHour());
	return std::string(buff);
}
std::string dateGetHour12NumStr()
{
	char buff[3];
	snprintf(buff, 3, "%02d", dateGetHour12());
	return std::string(buff);
}
std::string dateGetMinuteNumStr()
{
	char buff[3];
	snprintf(buff, 3, "%02d", dateGetMinute());
	return std::string(buff);
}
std::string dateGetSecondNumStr()
{
	char buff[3];
	snprintf(buff, 3, "%02d", dateGetSecond());
	return std::string(buff);
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

std::string loadFileSrc(const char* filepath)
{
	FILE* fileptr;
	fileptr = fopen(filepath, "r");
	
	fseek(fileptr, 0, SEEK_END);
	long int size = ftell(fileptr);
	fseek(fileptr, 0, SEEK_SET);
	
	LvnVector<char> src(size);
	fread(src.data(), sizeof(char), size, fileptr);
	fclose(fileptr);

	return std::string(src.data(), src.data() + src.size());
}

LvnData<uint8_t> loadFileSrcBin(const char* filepath)
{
	FILE* fileptr;
	fileptr = fopen(filepath, "rb");

	fseek(fileptr, 0, SEEK_END);
	long int size = ftell(fileptr);
	fseek(fileptr, 0, SEEK_SET);

	LvnVector<uint8_t> bin(size);
	fread(bin.data(), sizeof(uint8_t), size, fileptr);
	fclose(fileptr);

	return LvnData<uint8_t>(bin.data(), bin.size());
}

LvnFont loadFontFromFileTTF(const char* filepath, float fontSize, LvnCharset charset)
{
	LvnFont font{};

	LvnData<uint8_t> fontData = lvn::loadFileSrcBin(filepath);
	LvnVector<uint8_t> fontBuffer(fontData.data(), fontData.size());

	/* prepare font */
	stbtt_fontinfo info;
	if (!stbtt_InitFont(&info, fontBuffer.data(), 0))
	{
		LVN_CORE_ERROR("failed to load ttf font from file: %s", filepath);
		return font;
	}

	/* calculate font scaling */
	float scale = stbtt_ScaleForPixelHeight(&info, fontSize);

	int width = 0;   // bitmap width
	int height = fontSize; // bitmap height

	for (int8_t i = charset.first; i <= charset.last; i++)
	{
		int advance, lsb;
		stbtt_GetCodepointHMetrics(&info, i, &advance, &lsb);

		width += roundf(scale * advance);
	}

	LvnVector<uint8_t> bitmap(width * height * sizeof(uint8_t));

	int x = 0;
	int ascent, descent, lineGap;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
	
	ascent = roundf(ascent * scale);
	descent = roundf(descent * scale);

	font.fontSize = fontSize;
	font.codepoints = charset;

	LvnVector<LvnFontGlyph> glyphs(charset.last - charset.first + 1);

	uint8_t firstChar = charset.first;

	for (uint8_t i = charset.first; i <= charset.last; i++)
	{
		int x0 = x;

		int advance, lsb;
		stbtt_GetCodepointHMetrics(&info, i, &advance, &lsb);
		advance = roundf(advance * scale);
		lsb = roundf(lsb * scale);

		// get bounding box for character (may be offset to account for chars that dip above or below the line)
		int c_x1, c_y1, c_x2, c_y2;
		stbtt_GetCodepointBitmapBox(&info, i, scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
		
		// compute y (different characters have different heights)
		int y = ascent + c_y1;
		
		// render character (stride and offset is important here)
		int byteOffset = x + lsb + (y * width);
		stbtt_MakeCodepointBitmap(&info, bitmap.data() + byteOffset, c_x2 - c_x1, c_y2 - c_y1, width, scale, scale, i);

		// advance x
		x += advance;

		LvnFontGlyph glyph{};
		glyph.unicode = i;
		glyph.advance = advance;
		glyph.size.x = c_x2 - c_x1;
		glyph.size.x = c_y2 - c_y1;
		glyph.bearing.x = lsb;
		glyph.bearing.y = y;
		glyph.uv.x0 = (float)x0 / (float)width;
		glyph.uv.x1 = (float)x / (float)width;
		glyph.uv.y0 = 0.0f;
		glyph.uv.y1 = 1.0f;

		glyphs[i - firstChar] = glyph;
	}
	
	/* save out a 1 channel image */
	stbi_write_png("out.png", width, height, 1, bitmap.data(), width);
	
	font.glyphs = LvnData<LvnFontGlyph>(glyphs.data(), glyphs.size());

	LvnImageData atlas{};
	atlas.width = width;
	atlas.height = height;
	atlas.channels = 1;
	atlas.size = width * height;
	atlas.pixels = LvnData<uint8_t>(bitmap.data(), bitmap.size());

	font.atlas = atlas;

	return font;
}



void* memAlloc(size_t size)
{
	void* allocmem = calloc(1, size);
	if (!allocmem) { LVN_CORE_ERROR("malloc failure, could not allocate memory!"); LVN_ABORT; }
	if (s_LvnContext) { s_LvnContext->numMemoryAllocations++; }
	return allocmem;
}

void memFree(void* ptr)
{
	free(ptr);
	if (s_LvnContext) s_LvnContext->numMemoryAllocations--;
}

/* [Logging] */
const static LvnLogPattern s_LogPatterns[] =
{
	{ '$', [](LvnLogMessage* msg) -> std::string { return "\n"; } },
	{ 'n', [](LvnLogMessage* msg) -> std::string { return msg->loggerName; } },
	{ 'l', [](LvnLogMessage* msg) -> std::string { return getLogLevelName(msg->level); }},
	{ '#', [](LvnLogMessage* msg) -> std::string { return getLogLevelColor(msg->level); }},
	{ '^', [](LvnLogMessage* msg) -> std::string { return LVN_LOG_COLOR_RESET; }},
	{ 'v', [](LvnLogMessage* msg) -> std::string { return msg->msg; }},
	{ '%', [](LvnLogMessage* msg) -> std::string { return "%"; } },
	{ 'T', [](LvnLogMessage* msg) -> std::string { return dateGetTimeHHMMSS(); } },
	{ 't', [](LvnLogMessage* msg) -> std::string { return dateGetTime12HHMMSS(); } },
	{ 'Y', [](LvnLogMessage* msg) -> std::string { return dateGetYearStr(); }},
	{ 'y', [](LvnLogMessage* msg) -> std::string { return dateGetYear02dStr(); } },
	{ 'm', [](LvnLogMessage* msg) -> std::string { return dateGetMonthNumStr(); } },
	{ 'B', [](LvnLogMessage* msg) -> std::string { return dateGetMonthName(); } },
	{ 'b', [](LvnLogMessage* msg) -> std::string { return dateGetMonthNameShort(); } },
	{ 'd', [](LvnLogMessage* msg) -> std::string { return dateGetDayNumStr(); } },
	{ 'A', [](LvnLogMessage* msg) -> std::string { return dateGetWeekDayName(); } },
	{ 'a', [](LvnLogMessage* msg) -> std::string { return dateGetWeekDayNameShort(); } },
	{ 'H', [](LvnLogMessage* msg) -> std::string { return dateGetHourNumStr(); } },
	{ 'h', [](LvnLogMessage* msg) -> std::string { return dateGetHour12NumStr(); } },
	{ 'M', [](LvnLogMessage* msg) -> std::string { return dateGetMinuteNumStr(); } },
	{ 'S', [](LvnLogMessage* msg) -> std::string { return dateGetSecondNumStr(); } },
	{ 'P', [](LvnLogMessage* msg) -> std::string { return dateGetTimeMeridiem(); } },
	{ 'p', [](LvnLogMessage* msg) -> std::string { return dateGetTimeMeridiemLower(); }},
};

static LvnVector<LvnLogPattern> logParseFormat(const char* fmt)
{
	if (!fmt || fmt[0] == '\0') { return {}; }

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

		// find and add user defined patterns
		for (uint32_t j = 0; j < s_LvnContext->userLogPatterns.size(); j++)
		{
			if (fmt[i + 1] != s_LvnContext->userLogPatterns[j].symbol)
				continue;

			patterns.push_back(s_LvnContext->userLogPatterns[j]);
		}

		i++; // incramant past symbol on next character in format
	}

	return patterns;
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

		lvnctx->coreLogger.logPatterns = lvnctx->clientLogger.logPatterns = lvn::logParseFormat(LVN_DEFAULT_LOG_PATTERN);

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

	std::string msgstr;

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

void logMessage(LvnLogger* logger, LvnLogLevel level, const char* msg)
{
	if (!s_LvnContext->logging) { return; }

	LvnLogMessage logMsg =
	{
		.msg = msg,
		.loggerName = logger->loggerName,
		.level = level,
		.timeEpoch = dateGetSecondsSinceEpoch()
	};
	logOutputMessage(logger, &logMsg);
}

void logMessageTrace(LvnLogger* logger, const char* fmt, ...)
{
	if (!s_LvnContext || !s_LvnContext->logging) { return; }
	if (!logCheckLevel(logger, Lvn_LogLevel_Trace)) { return; }

	LvnVector<char> buff;

	va_list argptr, argcopy;
	va_start(argptr, fmt);
	va_copy(argcopy, argptr);

	int len = vsnprintf(nullptr, 0, fmt, argptr);
	buff.resize(len + 1);
	vsnprintf(&buff[0], len + 1, fmt, argcopy);
	logMessage(logger, Lvn_LogLevel_Trace, buff.data());

	va_end(argcopy);
	va_end(argptr);
}

void logMessageDebug(LvnLogger* logger, const char* fmt, ...)
{
	if (!s_LvnContext || !s_LvnContext->logging) { return; }
	if (!logCheckLevel(logger, Lvn_LogLevel_Debug)) { return; }

	LvnVector<char> buff;

	va_list argptr, argcopy;
	va_start(argptr, fmt);
	va_copy(argcopy, argptr);

	int len = vsnprintf(nullptr, 0, fmt, argptr);
	buff.resize(len + 1);
	vsnprintf(&buff[0], len + 1, fmt, argcopy);
	logMessage(logger, Lvn_LogLevel_Debug, buff.data());

	va_end(argcopy);
	va_end(argptr);
}

void logMessageInfo(LvnLogger* logger, const char* fmt, ...)
{
	if (!s_LvnContext || !s_LvnContext->logging) { return; }
	if (!logCheckLevel(logger, Lvn_LogLevel_Info)) { return; }

	LvnVector<char> buff;

	va_list argptr, argcopy;
	va_start(argptr, fmt);
	va_copy(argcopy, argptr);

	int len = vsnprintf(nullptr, 0, fmt, argptr);
	buff.resize(len + 1);
	vsnprintf(&buff[0], len + 1, fmt, argcopy);
	logMessage(logger, Lvn_LogLevel_Info, buff.data());

	va_end(argcopy);
	va_end(argptr);
}

void logMessageWarn(LvnLogger* logger, const char* fmt, ...)
{
	if (!s_LvnContext || !s_LvnContext->logging) { return; }
	if (!logCheckLevel(logger, Lvn_LogLevel_Warn)) { return; }

	LvnVector<char> buff;

	va_list argptr, argcopy;
	va_start(argptr, fmt);
	va_copy(argcopy, argptr);

	int len = vsnprintf(nullptr, 0, fmt, argptr);
	buff.resize(len + 1);
	vsnprintf(&buff[0], len + 1, fmt, argcopy);
	logMessage(logger, Lvn_LogLevel_Warn, buff.data());

	va_end(argcopy);
	va_end(argptr);
}

void logMessageError(LvnLogger* logger, const char* fmt, ...)
{
	if (!s_LvnContext || !s_LvnContext->logging) { return; }
	if (!logCheckLevel(logger, Lvn_LogLevel_Error)) { return; }

	LvnVector<char> buff;

	va_list argptr, argcopy;
	va_start(argptr, fmt);
	va_copy(argcopy, argptr);

	int len = vsnprintf(nullptr, 0, fmt, argptr);
	buff.resize(len + 1);
	vsnprintf(&buff[0], len + 1, fmt, argcopy);
	logMessage(logger, Lvn_LogLevel_Error, buff.data());

	va_end(argcopy);
	va_end(argptr);
}

void logMessageFatal(LvnLogger* logger, const char* fmt, ...)
{
	if (!s_LvnContext || !s_LvnContext->logging) { return; }
	if (!logCheckLevel(logger, Lvn_LogLevel_Fatal)) { return; }

	LvnVector<char> buff;

	va_list argptr, argcopy;
	va_start(argptr, fmt);
	va_copy(argcopy, argptr);

	int len = vsnprintf(nullptr, 0, fmt, argptr);
	buff.resize(len + 1);
	vsnprintf(&buff[0], len + 1, fmt, argcopy);
	logMessage(logger, Lvn_LogLevel_Fatal, buff.data());

	va_end(argcopy);
	va_end(argptr);
}

LvnLogger* logGetCoreLogger()
{
	return &s_LvnContext->coreLogger;
}

LvnLogger* logGetClientLogger()
{
	return &s_LvnContext->clientLogger;
}

const char* logGetANSIcodeColor(LvnLogLevel level)
{
	switch (level)
	{
		case Lvn_LogLevel_None:     { return LVN_LOG_COLOR_RESET; }
		case Lvn_LogLevel_Trace:    { return LVN_LOG_COLOR_TRACE; }
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

LvnResult logAddPattern(LvnLogPattern* logPattern)
{
	if (!logPattern) { return Lvn_Result_Failure; }
	if (logPattern->symbol == '\0') { return Lvn_Result_Failure; }

	for (uint32_t i = 0; i < sizeof(s_LogPatterns) / sizeof(LvnLogPattern); i++)
	{
		if (s_LogPatterns[i].symbol == logPattern->symbol) { return Lvn_Result_Failure; }
	}

	s_LvnContext->userLogPatterns.push_back(*logPattern);

	return Lvn_Result_Success;
}

// [SECTION]: Events
bool dispatchLvnAppRenderEvent(LvnEvent* event, bool(*func)(LvnAppRenderEvent*, void*))
{
	if (event->type == Lvn_EventType_AppRender)
	{
		LvnAppRenderEvent eventType{};
		eventType.type = Lvn_EventType_AppRender;
		eventType.category = Lvn_EventCategory_Application;
		eventType.name = "LvnAppRenderEvent";
		eventType.handled = false;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchLvnAppTickEvent(LvnEvent* event, bool(*func)(LvnAppTickEvent*, void*))
{
	if (event->type == Lvn_EventType_AppTick)
	{
		LvnAppTickEvent eventType{};
		eventType.type = Lvn_EventType_AppTick;
		eventType.category = Lvn_EventCategory_Application;
		eventType.name = "LvnAppTickEvent";
		eventType.handled = false;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchKeyHoldEvent(LvnEvent* event, bool(*func)(LvnKeyHoldEvent*, void*))
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

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchKeyPressedEvent(LvnEvent* event, bool(*func)(LvnKeyPressedEvent*, void*))
{
	if (event->type == Lvn_EventType_KeyPressed)
	{
		LvnKeyPressedEvent eventType{};
		eventType.type = Lvn_EventType_KeyPressed;
		eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
		eventType.name = "LvnKeyPressedEvent";
		eventType.handled = false;
		eventType.keyCode = event->data.code;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchKeyReleasedEvent(LvnEvent* event, bool(*func)(LvnKeyReleasedEvent*, void*))
{
	if (event->type == Lvn_EventType_KeyReleased)
	{
		LvnKeyReleasedEvent eventType{};
		eventType.type = Lvn_EventType_KeyReleased;
		eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
		eventType.name = "LvnKeyReleasedEvent";
		eventType.handled = false;
		eventType.keyCode = event->data.code;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchKeyTypedEvent(LvnEvent* event, bool(*func)(LvnKeyTypedEvent*, void*))
{
	if (event->type == Lvn_EventType_KeyTyped)
	{
		LvnKeyTypedEvent eventType{};
		eventType.type = Lvn_EventType_KeyTyped;
		eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
		eventType.name = "LvnKeyTypedEvent";
		eventType.handled = false;
		eventType.key = event->data.ucode;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchMouseButtonPressedEvent(LvnEvent* event, bool(*func)(LvnMouseButtonPressedEvent*, void*))
{
	if (event->type == Lvn_EventType_MouseButtonPressed)
	{
		LvnMouseButtonPressedEvent eventType{};
		eventType.type = Lvn_EventType_MouseButtonPressed;
		eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_MouseButton | Lvn_EventCategory_Mouse;
		eventType.name = "LvnMouseButtonPressedEvent";
		eventType.handled = false;
		eventType.buttonCode = event->data.code;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchMouseButtonReleasedEvent(LvnEvent* event, bool(*func)(LvnMouseButtonReleasedEvent*, void*))
{
	if (event->type == Lvn_EventType_MouseButtonReleased)
	{
		LvnMouseButtonReleasedEvent eventType{};
		eventType.type = Lvn_EventType_MouseButtonReleased;
		eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_MouseButton | Lvn_EventCategory_Mouse;
		eventType.name = "LvnMouseButtonReleasedEvent";
		eventType.handled = false;
		eventType.buttonCode = event->data.code;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchMouseMovedEvent(LvnEvent* event, bool(*func)(LvnMouseMovedEvent*, void*))
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

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchMouseScrolledEvent(LvnEvent* event, bool(*func)(LvnMouseScrolledEvent*, void*))
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

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchWindowCloseEvent(LvnEvent* event, bool(*func)(LvnWindowCloseEvent*, void*))
{
	if (event->type == Lvn_EventType_WindowClose)
	{
		LvnWindowCloseEvent eventType{};
		eventType.type = Lvn_EventType_WindowClose;
		eventType.category = Lvn_EventCategory_Window;
		eventType.name = "LvnWindowCloseEvent";
		eventType.handled = false;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchWindowFocusEvent(LvnEvent* event, bool(*func)(LvnWindowFocusEvent*, void*))
{
	if (event->type == Lvn_EventType_WindowFocus)
	{
		LvnWindowFocusEvent eventType{};
		eventType.type = Lvn_EventType_WindowFocus;
		eventType.category = Lvn_EventCategory_Window;
		eventType.name = "LvnWindowFocusEvent";
		eventType.handled = false;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchWindowFramebufferResizeEvent(LvnEvent* event, bool(*func)(LvnWindowFramebufferResizeEvent*, void*))
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

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchWindowLostFocusEvent(LvnEvent* event, bool(*func)(LvnWindowLostFocusEvent*, void*))
{
	if (event->type == Lvn_EventType_WindowLostFocus)
	{
		LvnWindowLostFocusEvent eventType{};
		eventType.type = Lvn_EventType_WindowLostFocus;
		eventType.category = Lvn_EventCategory_Window;
		eventType.name = "LvnWindowLostFocusEvent";
		eventType.handled = false;

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchWindowMovedEvent(LvnEvent* event, bool(*func)(LvnWindowMovedEvent*, void*))
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

		return func(&eventType, event->userData);
	}

	return false;
}
bool dispatchWindowResizeEvent(LvnEvent* event, bool(*func)(LvnWindowResizeEvent*, void*))
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

		return func(&eventType, event->userData);
	}

	return false;
}


// [SECTION]: Window

static const char* getWindowApiNameEnum(LvnWindowApi api)
{
	switch (api)
	{
		case Lvn_WindowApi_None:  { return "None";  }
		case Lvn_WindowApi_glfw:  { return "glfw";  }
		// case Lvn_WindowApi_win32: { return "win32"; }
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
		// case Lvn_WindowApi_win32:
		// {
		// 	break;
		// }
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
		// case Lvn_WindowApi_win32:
		// {
		// 	break;
		// }
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
		// case Lvn_WindowApi_win32: { return "win32"; }
	}

	LVN_CORE_ERROR("Unknown Windows API selected!");
	return LVN_EMPTY_STR;
}

LvnResult createWindow(LvnWindow** window, LvnWindowCreateInfo* createInfo)
{
	if (createInfo->width < 0 || createInfo->height < 0)
	{
		LVN_CORE_ERROR("createWindow(LvnWindow**, LvnWindowCreateInfo*) | cannot create window with negative dimensions (w:%d,h:%d)", createInfo->width, createInfo->height);
		return Lvn_Result_Failure;
	}

	*window = new LvnWindow();
	LVN_CORE_TRACE("created window: (%p), \"%s\" (w:%d,h:%d)", *window, createInfo->title, createInfo->width, createInfo->height);
	return s_LvnContext->windowContext.createWindow(*window, createInfo);
}

void destroyWindow(LvnWindow* window)
{
	s_LvnContext->windowContext.destroyWindow(window);
	delete window;
	window = nullptr;
}

void windowUpdate(LvnWindow* window)
{
	s_LvnContext->windowContext.updateWindow(window);
}

bool windowOpen(LvnWindow* window)
{
	return s_LvnContext->windowContext.windowOpen(window);
}

LvnPair<int> windowGetDimensions(LvnWindow* window)
{
	return s_LvnContext->windowContext.getWindowSize(window);
}

int windowGetWidth(LvnWindow* window)
{
	return s_LvnContext->windowContext.getWindowWidth(window);
}

int windowGetHeight(LvnWindow* window)
{
	return s_LvnContext->windowContext.getWindowHeight(window);
}

void windowSetEventCallback(LvnWindow* window, void (*callback)(LvnEvent*), void* userData)
{
	window->data.eventCallBackFn = callback;
	window->data.userData = userData;
}

void windowSetVSync(LvnWindow* window, bool enable)
{
	s_LvnContext->windowContext.setWindowVSync(window, enable);
}

bool windowGetVSync(LvnWindow* window)
{
	return s_LvnContext->windowContext.getWindowVSync(window);
}

void* windowGetNativeWindow(LvnWindow* window)
{
	return window->nativeWindow;
}

LvnRenderPass* windowGetRenderPass(LvnWindow* window)
{
	return &window->renderPass;
}

void windowSetContextCurrent(LvnWindow* window)
{
	s_LvnContext->windowContext.setWindowContextCurrent(window);
}

LvnWindowCreateInfo windowCreateInfoGetConfig(int width, int height, const char* title)
{
	LvnWindowCreateInfo windowCreateInfo{};
	windowCreateInfo.width = width;
	windowCreateInfo.height = height;
	windowCreateInfo.title = title;

	 return windowCreateInfo;
}

// [SECTION]: Input
bool keyPressed(LvnWindow* window, int keycode)
{
	return s_LvnContext->windowContext.keyPressed(window, keycode);
}

bool keyReleased(LvnWindow* window, int keycode)
{
	return s_LvnContext->windowContext.keyReleased(window, keycode);
}

bool mouseButtonPressed(LvnWindow* window, int button)
{
	return s_LvnContext->windowContext.mouseButtonPressed(window, button);
}

bool mouseButtonReleased(LvnWindow* window, int button)
{
	return s_LvnContext->windowContext.mouseButtonReleased(window, button);
}

void mouseSetPos(LvnWindow* window, float x, float y)
{
	s_LvnContext->windowContext.setMousePos(window, x, y);
}

LvnPair<float> mouseGetPos(LvnWindow* window)
{
	return s_LvnContext->windowContext.getMousePos(window);
}

void mouseGetPos(LvnWindow* window, float* xpos, float* ypos)
{
	s_LvnContext->windowContext.getMousePosPtr(window, xpos, ypos);
}

float mouseGetX(LvnWindow* window)
{
	return s_LvnContext->windowContext.getMouseX(window);
}

float mouseGetY(LvnWindow* window)
{
	return s_LvnContext->windowContext.getMouseY(window);
}

LvnPair<int> windowGetPos(LvnWindow* window)
{
	return s_LvnContext->windowContext.getWindowPos(window);
}

void windowGetPos(LvnWindow* window, int* xpos, int* ypos)
{
	s_LvnContext->windowContext.getWindowPosPtr(window, xpos, ypos);
}

LvnPair<int> windowGetSize(LvnWindow* window)
{
	return s_LvnContext->windowContext.getWindowSize(window);
}

void windowGetSize(LvnWindow* window, int* width, int* height)
{
	s_LvnContext->windowContext.getWindowSizePtr(window, width, height);
}

// [SECTION]: Graphics

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
		#if defined(LVN_GRAPHICS_API_INCLUDE_VULKAN)
			result = vksImplCreateContext(&lvnctx->graphicsContext, enableValidation);
		#endif
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
		#if defined(LVN_GRAPHICS_API_INCLUDE_VULKAN)
			vksImplTerminateContext();
		#endif
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

static LvnResult initAudioContext()
{
	ma_engine* pEngine = (ma_engine*)lvn::memAlloc(sizeof(ma_engine));

	if (ma_engine_init(nullptr, pEngine) != MA_SUCCESS)
	{
		LVN_CORE_ERROR("failed to initialize audio engine context");
		return Lvn_Result_Failure;
	}

	s_LvnContext->audioEngineContextPtr = pEngine;

	return Lvn_Result_Success;
}

static void terminateAudioContext()
{
	if (s_LvnContext->audioEngineContextPtr != nullptr)
	{
		ma_engine_uninit(static_cast<ma_engine*>(s_LvnContext->audioEngineContextPtr));
		lvn::memFree(s_LvnContext->audioEngineContextPtr);
	}
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

LvnResult checkPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice)
{
	return s_LvnContext->graphicsContext.checkPhysicalDeviceSupport(physicalDevice);
}

LvnResult renderInit(LvnRenderInitInfo* renderInfo)
{
	return s_LvnContext->graphicsContext.renderInit(renderInfo);
}

void renderClearColor(LvnWindow* window, float r, float g, float b, float a)
{
	s_LvnContext->graphicsContext.renderClearColor(window, r, g, b, a);
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
	s_LvnContext->graphicsContext.renderCmdDrawInstanced(window, vertexCount, instanceCount, firstInstance);
}

void renderCmdDrawIndexedInstanced(LvnWindow* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance)
{
	s_LvnContext->graphicsContext.renderCmdDrawIndexedInstanced(window, indexCount, instanceCount, firstInstance);
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

void renderCmdBindDescriptorSets(LvnWindow* window, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets)
{
	s_LvnContext->graphicsContext.renderCmdBindDescriptorSets(window, pipeline, firstSetIndex, descriptorSetCount, pDescriptorSets);
}

void renderCmdBeginFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer)
{
	s_LvnContext->graphicsContext.renderCmdBeginFrameBuffer(window, frameBuffer);
}

void renderCmdEndFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer)
{
	s_LvnContext->graphicsContext.renderCmdEndFrameBuffer(window, frameBuffer);
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
	LVN_CORE_TRACE("created shader (from source): (%p)", *shader);
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
	LVN_CORE_TRACE("created shader (from source file): (%p), vertex file: %s, fragment file: %s", *shader, createInfo->vertexSrc, createInfo->fragmentSrc);
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
	LVN_CORE_TRACE("created shader (from binary file): (%p), vertex file: %s, fragment file: %s", *shader, createInfo->vertexSrc, createInfo->fragmentSrc);
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

LvnResult createDescriptorSet(LvnDescriptorSet** descriptorSet, LvnDescriptorLayout* descriptorLayout)
{
	*descriptorSet = new LvnDescriptorSet();
	LVN_CORE_TRACE("created descriptorSet: (%p) from descriptorLayout: (%p)", *descriptorSet, descriptorLayout);
	return s_LvnContext->graphicsContext.createDescriptorSet(*descriptorSet, descriptorLayout);
}

LvnResult createPipeline(LvnPipeline** pipeline, LvnPipelineCreateInfo* createInfo)
{
	*pipeline = new LvnPipeline();
	LVN_CORE_TRACE("created pipeline: (%p)", *pipeline);
	return s_LvnContext->graphicsContext.createPipeline(*pipeline, createInfo);
}

LvnResult createFrameBuffer(LvnFrameBuffer** frameBuffer, LvnFrameBufferCreateInfo* createInfo)
{
	if (createInfo->pColorAttachments == nullptr)
	{
		LVN_CORE_ERROR("createFrameBuffer(LvnFrameBuffer**, LvnFrameBufferCreateInfo*) | createInfo->pColorAttachments is nullptr, cannot create framebuffer without one or more color attachments");
		return Lvn_Result_Failure;
	}

	uint32_t totalAttachments = createInfo->colorAttachmentCount + (createInfo->depthAttachment != nullptr ? 1 : 0);

	for (uint32_t i = 0; i < createInfo->colorAttachmentCount; i++)
	{
		if (createInfo->pColorAttachments[i].index >= totalAttachments)
		{
			LVN_CORE_ERROR("createFrameBuffer(LvnFrameBuffer**, LvnFrameBufferCreateInfo*) | createInfo->pColorAttachments[%u].index is greater than or equal to total attachments, color attachment index must be less than the total number of attachments", i);
			return Lvn_Result_Failure;
		}
		if (createInfo->depthAttachment != nullptr && createInfo->pColorAttachments[i].index == createInfo->depthAttachment->index)
		{
			LVN_CORE_ERROR("createFrameBuffer(LvnFrameBuffer**, LvnFrameBufferCreateInfo*) | createInfo->pColorAttachments[%u].index has the same value as createInfo->depthAttachment->index, color attachment index must not be the same as the depth attachment index", i);
			return Lvn_Result_Failure;
		}
	}

	if (createInfo->depthAttachment != nullptr)
	{
		if (createInfo->depthAttachment->index >= totalAttachments)
		{
			LVN_CORE_ERROR("createFrameBuffer(LvnFrameBuffer**, LvnFrameBufferCreateInfo*) | createInfo->pColorAttachments[%u].index is greater than or equal to total attachments, depth attachment index must be less than the total number of attachments");
			return Lvn_Result_Failure;
		}
	}

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

LvnResult createTexture(LvnTexture** texture, LvnTextureCreateInfo* createInfo)
{
	*texture = new LvnTexture();
	LVN_CORE_TRACE("created texture: (%p) using image data: (%p), (w:%u,h:%u,ch:%u), total size: %u bytes", *texture, createInfo->imageData.pixels.data(), createInfo->imageData.width, createInfo->imageData.height, createInfo->imageData.channels, createInfo->imageData.pixels.memsize());
	return s_LvnContext->graphicsContext.createTexture(*texture, createInfo);
}

LvnResult createCubemap(LvnCubemap** cubemap, LvnCubemapCreateInfo* createInfo)
{
	if (createInfo->posx.pixels.data() == nullptr)
	{
		LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->posx.pixels does not point to a valid pointer array");
		return Lvn_Result_Failure;
	}
	if (createInfo->negx.pixels.data() == nullptr)
	{
		LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->negx.pixels does not point to a valid pointer array");
		return Lvn_Result_Failure;
	}
	if (createInfo->posy.pixels.data() == nullptr)
	{
		LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->posy.pixels does not point to a valid pointer array");
		return Lvn_Result_Failure;
	}
	if (createInfo->negy.pixels.data() == nullptr)
	{
		LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->negy.pixels does not point to a valid pointer array");
		return Lvn_Result_Failure;
	}
	if (createInfo->posz.pixels.data() == nullptr)
	{
		LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->posz.pixels does not point to a valid pointer array");
		return Lvn_Result_Failure;
	}
	if (createInfo->negz.pixels.data() == nullptr)
	{
		LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | createInfo->negz.pixels does not point to a valid pointer array");
		return Lvn_Result_Failure;
	}

	// if(!(createInfo->posx.width * createInfo->posx.height ==
	// 	createInfo->negx.width * createInfo->negx.height ==
	// 	createInfo->posy.width * createInfo->posy.height ==
	// 	createInfo->negy.width * createInfo->negy.height ==
	// 	createInfo->posz.width * createInfo->posz.height ==
	// 	createInfo->negz.width * createInfo->negz.height))
	// {
	// 	LVN_CORE_ERROR("createCubemap(LvnCubemap**, LvnCubemapCreateInfo*) | not all images have the same dimensions, all cubemap images must have the same width and height");
	// 	return Lvn_Result_Failure;
	// }

	*cubemap = new LvnCubemap();
	LVN_CORE_TRACE("created cubemap: (%p)", *cubemap);
	return s_LvnContext->graphicsContext.createCubemap(*cubemap, createInfo);
}

LvnMesh createMesh(LvnMeshCreateInfo* createInfo)
{
	LvnMesh mesh{};

	lvn::createBuffer(&mesh.buffer, createInfo->bufferInfo);
	mesh.material = createInfo->material;
	mesh.modelMatrix = LvnMat4(1.0f);

	return mesh;
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

void destroyDescriptorSet(LvnDescriptorSet* descriptorSet)
{
	s_LvnContext->graphicsContext.destroyDescriptorSet(descriptorSet);
	delete descriptorSet;
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

void destroyTexture(LvnTexture* texture)
{
	s_LvnContext->graphicsContext.destroyTexture(texture);
	delete texture;
}

void destroyCubemap(LvnCubemap* cubemap)
{
	s_LvnContext->graphicsContext.destroyCubemap(cubemap);
	delete cubemap;
}

void destroyMesh(LvnMesh* mesh)
{
	lvn::destroyBuffer(mesh->buffer);
}

void pipelineSpecificationSetConfig(LvnPipelineSpecification* pipelineSpecification)
{
	s_LvnContext->graphicsContext.setDefaultPipelineSpecification(pipelineSpecification);
}

LvnPipelineSpecification pipelineSpecificationGetConfig()
{
	return s_LvnContext->graphicsContext.getDefaultPipelineSpecification();
}

void bufferUpdateVertexData(LvnBuffer* buffer, void* vertices, uint32_t size, uint32_t offset)
{
	s_LvnContext->graphicsContext.bufferUpdateVertexData(buffer, vertices, size, offset);
}

void bufferUpdateIndexData(LvnBuffer* buffer, uint32_t* indices, uint32_t size, uint32_t offset)
{
	s_LvnContext->graphicsContext.bufferUpdateIndexData(buffer, indices, size, offset);
}

void bufferResizeVertexBuffer(LvnBuffer* buffer, uint32_t size)
{
	s_LvnContext->graphicsContext.bufferResizeVertexBuffer(buffer, size);
}

void bufferResizeIndexBuffer(LvnBuffer* buffer, uint32_t size)
{
	s_LvnContext->graphicsContext.bufferResizeIndexBuffer(buffer, size);
}

const LvnTexture* cubemapGetTextureData(LvnCubemap* cubemap)
{
	return &cubemap->textureData;
}

void updateUniformBufferData(LvnWindow* window, LvnUniformBuffer* uniformBuffer, void* data, uint64_t size)
{
	s_LvnContext->graphicsContext.updateUniformBufferData(window, uniformBuffer, data, size);
}

void updateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count)
{
	s_LvnContext->graphicsContext.updateDescriptorSetData(descriptorSet, pUpdateInfo, count);
}

LvnTexture* frameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex)
{
	return s_LvnContext->graphicsContext.getFrameBufferImage(frameBuffer, attachmentIndex);
}

LvnRenderPass* frameBufferGetRenderPass(LvnFrameBuffer* frameBuffer)
{
	return s_LvnContext->graphicsContext.getFrameBufferRenderPass(frameBuffer);
}

void frameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height)
{
	return s_LvnContext->graphicsContext.updateFrameBuffer(frameBuffer, width, height);
}

void frameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a)
{
	return s_LvnContext->graphicsContext.setFrameBufferClearColor(frameBuffer, attachmentIndex, r, g, b, a);
}

LvnBuffer* meshGetBuffer(LvnMesh* mesh)
{
	return mesh->buffer;
}

LvnMat4 meshGetMatrix(LvnMesh* mesh)
{
	return mesh->modelMatrix;
}

void meshSetMatrix(LvnMesh* mesh, const LvnMat4& matrix)
{
	mesh->modelMatrix = matrix;
}

LvnBufferCreateInfo meshGetVertexBufferCreateInfoConfig(LvnVertex* pVertices, uint32_t vertexCount, uint32_t* pIndices, uint32_t indexCount)
{
	LvnVertexBindingDescription meshVertexBindingDescroption{};
	meshVertexBindingDescroption.stride = sizeof(LvnVertex);
	meshVertexBindingDescroption.binding = 0;

	LvnVertexAttribute meshVertexAttributes[] = 
	{
		{ 0, 0, Lvn_VertexDataType_Vec3f, 0 },                   // pos
		{ 0, 1, Lvn_VertexDataType_Vec4f, 3 * sizeof(float) },   // color
		{ 0, 2, Lvn_VertexDataType_Vec2f, 7 * sizeof(float) },   // texUV
		{ 0, 3, Lvn_VertexDataType_Vec3f, 9 * sizeof(float) },   // normal
		{ 0, 4, Lvn_VertexDataType_Vec3f, 12 * sizeof(float) },  // tangent
		{ 0, 5, Lvn_VertexDataType_Vec3f, 15 * sizeof(float) },  // bitangent
	};

	LvnBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.type = Lvn_BufferType_Vertex | Lvn_BufferType_Index;
	bufferCreateInfo.pVertexBindingDescriptions = &meshVertexBindingDescroption;
	bufferCreateInfo.vertexBindingDescriptionCount = 1;
	bufferCreateInfo.pVertexAttributes = meshVertexAttributes;
	bufferCreateInfo.vertexAttributeCount = sizeof(meshVertexAttributes) / sizeof(LvnVertexAttribute);
	bufferCreateInfo.pVertices = pVertices;
	bufferCreateInfo.vertexBufferSize = vertexCount * sizeof(LvnVertex);
	bufferCreateInfo.pIndices = pIndices;
	bufferCreateInfo.indexBufferSize = indexCount * sizeof(uint32_t);

	return bufferCreateInfo;
}

LvnImageData loadImageData(const char* filepath, int forceChannels)
{
	if (filepath == nullptr)
	{
		LVN_CORE_ERROR("loadImageData(LvnImageData*, const char*, int) | invalid filepath, filepath must not be nullptr");
		return {};
	}

	if (forceChannels < 0)
	{
		LVN_CORE_ERROR("loadImageData(LvnImageData*, const char*, int) | forceChannels < 0, channels cannot be negative");
		return {};
	}
	else if (forceChannels > 4)
	{
		LVN_CORE_ERROR("loadImageData(LvnImageData*, const char*, int) | forceChannels > 4, channels cannot be higher than 4 components (rgba)");
		return {};
	}

	int imageWidth, imageHeight, imageChannels;
	stbi_uc* pixels = stbi_load(filepath, &imageWidth, &imageHeight, &imageChannels, forceChannels);

	if (!pixels)
	{
		LVN_CORE_ERROR("loadImageData(LvnImageData*, const char*) | failed to load image pixel data to (%p) from file: %s", pixels, filepath);
		return {};
	}

	LvnImageData imageData{};
	imageData.width = imageWidth;
	imageData.height = imageHeight;
	imageData.channels = forceChannels ? forceChannels : imageChannels;
	imageData.size = imageData.width * imageData.height * imageData.channels;
	imageData.pixels = LvnData<uint8_t>(pixels, imageData.size);

	LVN_CORE_TRACE("loaded image data <unsigned char*> (%p), (w:%u,h:%u,ch:%u), total memory size: %u bytes, filepath: %s", pixels, imageData.width, imageData.height, imageData.channels, imageData.size, filepath);

	stbi_image_free(pixels);

	return imageData;
}

LvnModel loadModel(const char* filepath)
{
	std::string filepathstr(filepath);
	std::string extensionType = filepathstr.substr(filepathstr.find_last_of(".") + 1);

	if (extensionType == "gltf")
	{
		return lvn::loadGltfModel(filepath);
	}

	LVN_CORE_WARN("loadModel(const char*) | could not load model, file extension type not recognized (%s), Filepath: %s", extensionType.c_str(), filepath);
	return {};
}

void freeModel(LvnModel* model)
{
	for (uint32_t i = 0; i < model->meshes.size(); i++)
	{
		lvn::destroyMesh(&model->meshes[i]);
	}
	
	for (uint32_t i = 0; i < model->textures.size(); i++)
	{
		lvn::destroyTexture(model->textures[i]);
	}
}

LvnCamera cameraConfigInit(LvnCameraCreateInfo* createInfo)
{
	LvnCamera camera{};
	camera.width = createInfo->width;
	camera.height = createInfo->height;
	camera.position = createInfo->position;
	camera.orientation = createInfo->orientation;
	camera.upVector = createInfo->upVector;
	camera.fov = createInfo->fovDeg;
	camera.nearPlane = createInfo->nearPlane;
	camera.farPlane = createInfo->farPlane;

	camera.viewMatrix = lvn::lookAt(camera.position, camera.position + camera.orientation, camera.upVector);
	camera.projectionMatrix = lvn::perspective(lvn::radians(camera.fov), static_cast<float>(camera.width) / static_cast<float>(camera.height), camera.nearPlane, camera.farPlane);
	camera.matrix = camera.projectionMatrix * camera.viewMatrix;

	return camera;
}

void cameraUpdateMatrix(LvnCamera* camera)
{
	camera->projectionMatrix = lvn::perspective(lvn::radians(camera->fov), static_cast<float>(camera->width) / static_cast<float>(camera->height), camera->nearPlane, camera->farPlane);
	camera->viewMatrix = lvn::lookAt(camera->position, camera->position + camera->orientation, camera->upVector);
	camera->matrix = camera->projectionMatrix * camera->viewMatrix;
}

void cameraSetFov(LvnCamera* camera, float fovDeg)
{
	camera->fov = fovDeg;
	lvn::cameraUpdateMatrix(camera);
}

void cameraSetPlane(LvnCamera* camera, float nearPlane, float farPlane)
{
	camera->nearPlane = nearPlane;
	camera->farPlane = farPlane;
	lvn::cameraUpdateMatrix(camera);
}

void cameraSetPos(LvnCamera* camera, const LvnVec3& position)
{
	camera->position = position;
	lvn::cameraUpdateMatrix(camera);
}

void cameraSetOrient(LvnCamera* camera, const LvnVec3& orientation)
{
	camera->orientation = orientation;
	lvn::cameraUpdateMatrix(camera);
}

void setCameraUpVec(LvnCamera* camera, const LvnVec3& upVector)
{
	camera->upVector = camera->upVector;
	lvn::cameraUpdateMatrix(camera);
}

float cameraGetFov(LvnCamera* camera)
{
	return camera->fov;
}

float cameraGetNearPlane(LvnCamera* camera)
{
	return camera->nearPlane;
}

float cameraGetFarPlane(LvnCamera* camera)
{
	return camera->farPlane;
}

LvnVec3 cameraGetPos(LvnCamera* camera)
{
	return camera->position;
}

LvnVec3 cameraGetOrient(LvnCamera* camera)
{
	return camera->orientation;
}

LvnVec3 cameraGetUpVec(LvnCamera* camera)
{
	return camera->upVector;
}


uint32_t getVertexDataTypeSize(LvnVertexDataType type)
{
	switch (type)
	{
		case Lvn_VertexDataType_None:        { return 0; }
		case Lvn_VertexDataType_Float:       { return sizeof(float); }
		case Lvn_VertexDataType_Double:      { return sizeof(double); }
		case Lvn_VertexDataType_Int:         { return sizeof(int); }
		case Lvn_VertexDataType_UnsignedInt: { return sizeof(uint32_t); }
		case Lvn_VertexDataType_Bool:        { return sizeof(bool); }
		case Lvn_VertexDataType_Vec2:        { return sizeof(float) * 2; }
		case Lvn_VertexDataType_Vec3:        { return sizeof(float) * 3; }
		case Lvn_VertexDataType_Vec4:        { return sizeof(float) * 4; }
		case Lvn_VertexDataType_Vec2i:       { return sizeof(int32_t) * 2; }
		case Lvn_VertexDataType_Vec3i:       { return sizeof(int32_t) * 3; }
		case Lvn_VertexDataType_Vec4i:       { return sizeof(int32_t) * 4; }
		case Lvn_VertexDataType_Vec2ui:      { return sizeof(uint32_t) * 2; }
		case Lvn_VertexDataType_Vec3ui:      { return sizeof(uint32_t) * 3; }
		case Lvn_VertexDataType_Vec4ui:      { return sizeof(uint32_t) * 4; }
		case Lvn_VertexDataType_Vec2d:       { return sizeof(double) * 2; }
		case Lvn_VertexDataType_Vec3d:       { return sizeof(double) * 3; }
		case Lvn_VertexDataType_Vec4d:       { return sizeof(double) * 4; }

		default:
		{
			LVN_CORE_WARN("unknown vertex data type enum: (%u)", type);
			return 0;
		}
	}
}


// [SECTION]: Audio

LvnResult createSoundFromFile(LvnSound** sound, LvnSoundCreateInfo* createInfo)
{
	if (createInfo->filepath == nullptr)
	{
		LVN_CORE_ERROR("createSoundFromFile(LvnSound**, LvnSoundCreateInfo*) | createInfo->filepath is nullptr, cannot load sound data without a valid path to the sound file");
		return Lvn_Result_Failure;
	}

	ma_engine* pEngine = static_cast<ma_engine*>(s_LvnContext->audioEngineContextPtr);

	ma_sound* pSound = (ma_sound*)lvn::memAlloc(sizeof(ma_sound));
	if (ma_sound_init_from_file(pEngine, createInfo->filepath, 0, NULL, NULL, pSound) != MA_SUCCESS)
	{
		LVN_CORE_ERROR("createSoundFromFile(LvnSound**, LvnSoundCreateInfo*) | failed to create sound object");
		return Lvn_Result_Failure;
	}

	ma_sound_set_volume(pSound, createInfo->volume);
	ma_sound_set_pan(pSound, createInfo->pan);
	ma_sound_set_pitch(pSound, createInfo->pitch);
	ma_sound_set_looping(pSound, createInfo->looping);

	*sound = new LvnSound();
	LvnSound* soundPtr = *sound;
	soundPtr->volume = createInfo->volume;
	soundPtr->pan = createInfo->pan;
	soundPtr->pitch = createInfo->pitch;
	soundPtr->pos = createInfo->pos;
	soundPtr->looping = createInfo->looping;
	soundPtr->soundPtr = pSound;

	return Lvn_Result_Success;
}

void destroySound(LvnSound* sound)
{
	ma_sound* pSound = static_cast<ma_sound*>(sound->soundPtr);

	ma_sound_uninit(pSound);
	lvn::memFree(pSound);

	delete sound;
}

void soundSetVolume(LvnSound* sound, float volume)
{
	ma_sound* pSound = static_cast<ma_sound*>(sound->soundPtr);
	ma_sound_set_volume(pSound, volume);
}

void soundSetPan(LvnSound* sound, float pan)
{
	ma_sound* pSound = static_cast<ma_sound*>(sound->soundPtr);
	ma_sound_set_pan(pSound, pan);
}

void soundSetPitch(LvnSound* sound, float pitch)
{
	ma_sound* pSound = static_cast<ma_sound*>(sound->soundPtr);
	ma_sound_set_pitch(pSound, pitch);
}

void soundSetLooping(LvnSound* sound, bool looping)
{
	ma_sound* pSound = static_cast<ma_sound*>(sound->soundPtr);
	ma_sound_set_looping(pSound, looping);
}

void soundSetPlayStart(LvnSound* sound)
{
	ma_sound* pSound = static_cast<ma_sound*>(sound->soundPtr);
	ma_sound_start(pSound);
}

void soundSetPlayStop(LvnSound* sound)
{
	ma_sound* pSound = static_cast<ma_sound*>(sound->soundPtr);
	ma_sound_stop(pSound);
}

void soundSetPlayPause(LvnSound* sound)
{
	ma_sound* pSound = static_cast<ma_sound*>(sound->soundPtr);
	if (ma_sound_is_playing(pSound)) { ma_sound_stop(pSound); }
	else { ma_sound_start(pSound); }
}


// [SECTION]: Math

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
	conv.f = num;
	conv.i = 0x5f3759df - (conv.i >> 1);
	conv.f = conv.f * (threehalfs - (x2 * conv.f * conv.f));
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
