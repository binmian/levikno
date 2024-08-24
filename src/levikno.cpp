#include "levikno.h"
#include "levikno_internal.h"

#include <cmath>
#include <cstdint>
#include <stdarg.h>
#include <time.h>

#include "stb_image.h"
#include "miniaudio.h"
#include "freetype/freetype.h"

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

#include "lvn_opengl.h"

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
static LvnResult                    setWindowContext(LvnContext* lvnctx, LvnWindowApi windowapi);
static void                         terminateWindowContext(LvnContext* lvnctx);
static LvnResult                    setGraphicsContext(LvnContext* lvnctx, LvnGraphicsApi graphicsapi);
static void                         terminateGraphicsContext(LvnContext* lvnctx);
static LvnResult                    initAudioContext(LvnContext* lvnctx);
static void                         terminateAudioContext(LvnContext* lvnctx);
static void                         initStandardPipelineSpecification(LvnContext* lvnctx);


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
	s_LvnContext->graphicsContext.enableValidationLayers = createInfo->enableVulkanValidationLayers;
	s_LvnContext->graphicsContext.frameBufferColorFormat = createInfo->frameBufferColorFormat;

	// logging
	if (createInfo->enableLogging) { logInit(); }

	s_LvnContext->coreLogger.logLevel = createInfo->coreLogLevel;

	// window context
	LvnResult result = setWindowContext(s_LvnContext, createInfo->windowapi);
	if (result != Lvn_Result_Success) { return result; }

	// graphics context
	result = setGraphicsContext(s_LvnContext, createInfo->graphicsapi);
	if (result != Lvn_Result_Success) { return result; }

	result = initAudioContext(s_LvnContext);
	if (result != Lvn_Result_Success) { return result; }

	// config
	initStandardPipelineSpecification(s_LvnContext);

	return Lvn_Result_Success;
}

void terminateContext()
{
	if (s_LvnContext == nullptr) { return; }

	terminateWindowContext(s_LvnContext);
	terminateGraphicsContext(s_LvnContext);
	terminateAudioContext(s_LvnContext);

	if (s_LvnContext->objectMemoryAllocations.windows > 0) { LVN_CORE_WARN("not all window objects have been destroyed, number of window objects remaining: %zu", s_LvnContext->objectMemoryAllocations.windows); }
	if (s_LvnContext->objectMemoryAllocations.loggers > 0) { LVN_CORE_WARN("not all logger objects have been destroyed, number of logger objects remaining: %zu", s_LvnContext->objectMemoryAllocations.loggers); }
	if (s_LvnContext->objectMemoryAllocations.frameBuffers > 0) { LVN_CORE_WARN("not all frameBuffers objects have been destroyed, number of frameBuffers objects remaining: %zu", s_LvnContext->objectMemoryAllocations.frameBuffers); }
	if (s_LvnContext->objectMemoryAllocations.shaders > 0) { LVN_CORE_WARN("not all shader objects have been destroyed, number of shader objects remaining: %zu", s_LvnContext->objectMemoryAllocations.shaders); }
	if (s_LvnContext->objectMemoryAllocations.descriptorLayouts > 0) { LVN_CORE_WARN("not all descriptor layout objects have been destroyed, number of descriptor layout objects remaining: %zu", s_LvnContext->objectMemoryAllocations.descriptorLayouts); }
	if (s_LvnContext->objectMemoryAllocations.descriptorSets > 0) { LVN_CORE_WARN("not all descriptor set objects have been destroyed, number of descriptor set objects remaining: %zu", s_LvnContext->objectMemoryAllocations.descriptorSets); }
	if (s_LvnContext->objectMemoryAllocations.pipelines > 0) { LVN_CORE_WARN("not all pipelines objects have been destroyed, number of pipelines objects remaining: %zu", s_LvnContext->objectMemoryAllocations.pipelines); }
	if (s_LvnContext->objectMemoryAllocations.buffers > 0) { LVN_CORE_WARN("not all buffer objects have been destroyed, number of buffer objects remaining: %zu", s_LvnContext->objectMemoryAllocations.buffers); }
	if (s_LvnContext->objectMemoryAllocations.uniformBuffers > 0) { LVN_CORE_WARN("not all uniform buffer objects have been destroyed, number of uniform buffer objects remaining: %zu", s_LvnContext->objectMemoryAllocations.uniformBuffers); }
	if (s_LvnContext->objectMemoryAllocations.textures > 0) { LVN_CORE_WARN("not all texture have been destroyed, number of texture objects remaining: %zu", s_LvnContext->objectMemoryAllocations.textures); }
	if (s_LvnContext->objectMemoryAllocations.cubemaps > 0) { LVN_CORE_WARN("not all cubemap objects have been destroyed, number of cubemap objects remaining: %zu", s_LvnContext->objectMemoryAllocations.cubemaps); }
	if (s_LvnContext->objectMemoryAllocations.sounds > 0) { LVN_CORE_WARN("not all sound objects have been destroyed, number of sound objects remaining: %zu", s_LvnContext->objectMemoryAllocations.sounds); }
	if (s_LvnContext->numMemoryAllocations > 0) { LVN_CORE_WARN("not all memory allocations have been freed, number of allocations remaining: %zu", s_LvnContext->numMemoryAllocations); }
	
	logTerminate();

	delete s_LvnContext;
	s_LvnContext = nullptr;
}

LvnContext* getContext()
{
	LVN_CORE_ASSERT(s_LvnContext != nullptr, "levikno context is nullptr, context was probably not created or initiated before using the library")
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

LvnFont loadFontFromFileTTF(const char* filepath, uint32_t fontSize, LvnCharset charset)
{
	LvnFont font{};

	LvnData<uint8_t> fontData = lvn::loadFileSrcBin(filepath);
	LvnVector<uint8_t> fontBuffer(fontData.data(), fontData.size());

	FT_Library ft;
	FT_Face face;

	if (FT_Init_FreeType(&ft))
	{
		LVN_CORE_ERROR("[freetype]: failed to load freetype library");
		LVN_CORE_ASSERT(false, "failed to load freetype");
		return font;
	}

	if (FT_New_Face(ft, filepath, 0, &face))
	{
		LVN_CORE_ERROR("[freetype]: failed to load font face!");
		LVN_CORE_ASSERT(false, "failed to load font face");
		return font;
	}

	FT_Set_Char_Size(face, 0, fontSize << 6, 96, 96);

	int maxDim = (1 + (face->size->metrics.height >> 6)) * ceilf(sqrtf(charset.last - charset.first));
	int width = 1;
	while (width < maxDim) width <<= 1;
	int height = width;

	// render glyphs to atlas
	LvnVector<uint8_t> pixels(width * height);
	int penx = 0, peny = 0;

	LvnVector<LvnFontGlyph> glyphs(charset.last - charset.first + 1);

	for (int8_t i = charset.first; i <= charset.last; i++)
	{
		FT_Load_Char(face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
		FT_Bitmap* bmp = &face->glyph->bitmap;

		if (penx + bmp->width >= width)
		{
			penx = 0;
			peny += ((face->size->metrics.height >> 6) + 1);
		}

		for (unsigned int row = 0; row < bmp->rows; row++)
		{
			for (unsigned int col = 0; col < bmp->width; col++)
			{
				int x = penx + col;
				int y = peny + row;
				pixels[y * width + x] = bmp->buffer[row * bmp->pitch + col];
			}
		}


		LvnFontGlyph glyph{};
		glyph.uv.x0 = (float)penx / (float)width;
		glyph.uv.y0 = (float)peny / (float)height;
		glyph.uv.x1 = (float)(penx + bmp->width) / (float)width;
		glyph.uv.y1 = (float)(peny + bmp->rows) / (float)height;

		glyph.size.x = bmp->width;
		glyph.size.y = bmp->rows;
		glyph.bearing.x = face->glyph->bitmap_left;
		glyph.bearing.y = face->glyph->bitmap_top;
		glyph.advance = face->glyph->advance.x >> 6;

		glyphs[i - charset.first] = glyph;

		penx += bmp->width + 1;
	}

	FT_Done_FreeType(ft);
	
	LvnImageData atlas{};
	atlas.width = width;
	atlas.height = height;
	atlas.channels = 1;
	atlas.size = width * height;
	atlas.pixels = LvnData<uint8_t>(pixels.data(), pixels.size());

	font.atlas = atlas;
	font.glyphs = LvnData<LvnFontGlyph>(glyphs.data(), glyphs.size());
	font.codepoints = charset;
	font.fontSize = fontSize;

	return font;
}

LvnFontGlyph fontGetGlyph(LvnFont* font, int8_t codepoint)
{
	LVN_CORE_ASSERT(codepoint >= font->codepoints.first && codepoint <= font->codepoints.last, "codepoint out of charset range");
	return font->glyphs[codepoint - font->codepoints.first];
}

void* memAlloc(size_t size)
{
	void* allocmem = calloc(1, size);
	if (!allocmem) { LVN_CORE_ERROR("malloc failure, could not allocate memory!"); LVN_ABORT; }
	if (s_LvnContext) { s_LvnContext->numMemoryAllocations++; }
	memset(allocmem, 0, size);
	return allocmem;
}

void memFree(void* ptr)
{
	free(ptr);
	ptr = nullptr;
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
	LvnContext* lvnctx = lvn::getContext();

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
	LvnContext* lvnctx = lvn::getContext();

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
	if (!lvn::getContext()->logging) { return; }

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
	if (!lvn::getContext()->logging) { return; }

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

LvnResult logAddPattern(LvnLogPattern* logPattern)
{
	if (!logPattern) { return Lvn_Result_Failure; }
	if (logPattern->symbol == '\0') { return Lvn_Result_Failure; }

	for (uint32_t i = 0; i < sizeof(s_LogPatterns) / sizeof(LvnLogPattern); i++)
	{
		if (s_LogPatterns[i].symbol == logPattern->symbol) { return Lvn_Result_Failure; }
	}

	lvn::getContext()->userLogPatterns.push_back(*logPattern);

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

static LvnResult setWindowContext(LvnContext* lvnctx, LvnWindowApi windowapi)
{
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

static void terminateWindowContext(LvnContext* lvnctx)
{
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
	return lvn::getContext()->windowapi;
}

const char* getWindowApiName()
{
	switch (lvn::getContext()->windowapi)
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
	LvnContext* lvnctx = lvn::getContext();

	if (createInfo->width < 0 || createInfo->height < 0)
	{
		LVN_CORE_ERROR("createWindow(LvnWindow**, LvnWindowCreateInfo*) | cannot create window with negative dimensions (w:%d,h:%d)", createInfo->width, createInfo->height);
		return Lvn_Result_Failure;
	}

	*window = new LvnWindow();
	lvnctx->objectMemoryAllocations.windows++;
	LVN_CORE_TRACE("created window: (%p), \"%s\" (w:%d,h:%d)", *window, createInfo->title, createInfo->width, createInfo->height);
	return lvnctx->windowContext.createWindow(*window, createInfo);
}

void destroyWindow(LvnWindow* window)
{
	LvnContext* lvnctx = lvn::getContext();
	lvnctx->windowContext.destroyWindow(window);
	delete window;
	window = nullptr;
	lvnctx->objectMemoryAllocations.windows--;
}

void windowUpdate(LvnWindow* window)
{
	lvn::getContext()->windowContext.updateWindow(window);
}

bool windowOpen(LvnWindow* window)
{
	return lvn::getContext()->windowContext.windowOpen(window);
}

LvnPair<int> windowGetDimensions(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getWindowSize(window);
}

int windowGetWidth(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getWindowWidth(window);
}

int windowGetHeight(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getWindowHeight(window);
}

void windowSetEventCallback(LvnWindow* window, void (*callback)(LvnEvent*), void* userData)
{
	window->data.eventCallBackFn = callback;
	window->data.userData = userData;
}

void windowSetVSync(LvnWindow* window, bool enable)
{
	lvn::getContext()->windowContext.setWindowVSync(window, enable);
}

bool windowGetVSync(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getWindowVSync(window);
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
	lvn::getContext()->windowContext.setWindowContextCurrent(window);
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
	return lvn::getContext()->windowContext.keyPressed(window, keycode);
}

bool keyReleased(LvnWindow* window, int keycode)
{
	return lvn::getContext()->windowContext.keyReleased(window, keycode);
}

bool mouseButtonPressed(LvnWindow* window, int button)
{
	return lvn::getContext()->windowContext.mouseButtonPressed(window, button);
}

bool mouseButtonReleased(LvnWindow* window, int button)
{
	return lvn::getContext()->windowContext.mouseButtonReleased(window, button);
}

void mouseSetPos(LvnWindow* window, float x, float y)
{
	lvn::getContext()->windowContext.setMousePos(window, x, y);
}

LvnPair<float> mouseGetPos(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getMousePos(window);
}

void mouseGetPos(LvnWindow* window, float* xpos, float* ypos)
{
	lvn::getContext()->windowContext.getMousePosPtr(window, xpos, ypos);
}

float mouseGetX(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getMouseX(window);
}

float mouseGetY(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getMouseY(window);
}

LvnPair<int> windowGetPos(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getWindowPos(window);
}

void windowGetPos(LvnWindow* window, int* xpos, int* ypos)
{
	lvn::getContext()->windowContext.getWindowPosPtr(window, xpos, ypos);
}

LvnPair<int> windowGetSize(LvnWindow* window)
{
	return lvn::getContext()->windowContext.getWindowSize(window);
}

void windowGetSize(LvnWindow* window, int* width, int* height)
{
	lvn::getContext()->windowContext.getWindowSizePtr(window, width, height);
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

static LvnResult setGraphicsContext(LvnContext* lvnctx, LvnGraphicsApi graphicsapi)
{
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
			result = vksImplCreateContext(&lvnctx->graphicsContext);
		#endif
			break;
		}
		case Lvn_GraphicsApi_opengl:
		{
			result = oglsImplCreateContext(&lvnctx->graphicsContext);
			break;
		}
	}

	if (result != Lvn_Result_Success)
		LVN_CORE_ERROR("could not create graphics context for: %s", getGraphicsApiNameEnum(graphicsapi));
	else
		LVN_CORE_TRACE("graphics context set: %s", getGraphicsApiNameEnum(graphicsapi));

	return result;
}

static void terminateGraphicsContext(LvnContext* lvnctx)
{
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
			oglsImplTerminateContext();
			break;
		}
		default:
		{
			LVN_CORE_ERROR("unknown Graphics API selected! Cannot terminate Graphics API!");
		}
	}

	LVN_CORE_TRACE("graphics context terminated: %s", getGraphicsApiNameEnum(lvnctx->graphicsapi));
}

static LvnResult initAudioContext(LvnContext* lvnctx)
{
	ma_engine* pEngine = (ma_engine*)lvn::memAlloc(sizeof(ma_engine));

	if (ma_engine_init(nullptr, pEngine) != MA_SUCCESS)
	{
		LVN_CORE_ERROR("failed to initialize audio engine context");
		return Lvn_Result_Failure;
	}

	lvnctx->audioEngineContextPtr = pEngine;

	return Lvn_Result_Success;
}

static void terminateAudioContext(LvnContext* lvnctx)
{
	if (lvnctx->audioEngineContextPtr != nullptr)
	{
		ma_engine_uninit(static_cast<ma_engine*>(lvnctx->audioEngineContextPtr));
		lvn::memFree(lvnctx->audioEngineContextPtr);
	}
}

static void initStandardPipelineSpecification(LvnContext* lvnctx)
{
	LvnPipelineSpecification pipelineSpecification{};

	// Input Assembly
	pipelineSpecification.inputAssembly.topology = Lvn_TopologyType_Triangle;
	pipelineSpecification.inputAssembly.primitiveRestartEnable = false;

	// Viewport
	pipelineSpecification.viewport.x = 0.0f;
	pipelineSpecification.viewport.y = 0.0f;
	pipelineSpecification.viewport.width = 800.0f;
	pipelineSpecification.viewport.height = 600.0f;
	pipelineSpecification.viewport.minDepth = 0.0f;
	pipelineSpecification.viewport.maxDepth = 1.0f;

	// Scissor
	pipelineSpecification.scissor.offset = { 0, 0 };
	pipelineSpecification.scissor.extent = { 800, 600 };

	// Rasterizer
	pipelineSpecification.rasterizer.depthClampEnable = false;
	pipelineSpecification.rasterizer.rasterizerDiscardEnable = false;
	pipelineSpecification.rasterizer.lineWidth = 1.0f;
	pipelineSpecification.rasterizer.cullMode = Lvn_CullFaceMode_Disable;
	pipelineSpecification.rasterizer.frontFace = Lvn_CullFrontFace_Clockwise;
	pipelineSpecification.rasterizer.depthBiasEnable = false;
	pipelineSpecification.rasterizer.depthBiasConstantFactor = 0.0f;
	pipelineSpecification.rasterizer.depthBiasClamp = 0.0f;
	pipelineSpecification.rasterizer.depthBiasSlopeFactor = 0.0f;

	// MultiSampling
	pipelineSpecification.multisampling.sampleShadingEnable = false;
	pipelineSpecification.multisampling.rasterizationSamples = Lvn_SampleCount_1_Bit;
	pipelineSpecification.multisampling.minSampleShading = 1.0f;
	pipelineSpecification.multisampling.sampleMask = nullptr;
	pipelineSpecification.multisampling.alphaToCoverageEnable = false;
	pipelineSpecification.multisampling.alphaToOneEnable = false;

	// Color Attachments
	pipelineSpecification.colorBlend.colorBlendAttachmentCount = 0; // If no attachments are provided, an attachment will automatically be created
	pipelineSpecification.colorBlend.pColorBlendAttachments = nullptr; 

	// Color Blend
	pipelineSpecification.colorBlend.logicOpEnable = false;
	pipelineSpecification.colorBlend.blendConstants[0] = 0.0f;
	pipelineSpecification.colorBlend.blendConstants[1] = 0.0f;
	pipelineSpecification.colorBlend.blendConstants[2] = 0.0f;
	pipelineSpecification.colorBlend.blendConstants[3] = 0.0f;

	// Depth Stencil
	pipelineSpecification.depthstencil.enableDepth = false;
	pipelineSpecification.depthstencil.depthOpCompare = Lvn_CompareOperation_Never;
	pipelineSpecification.depthstencil.enableStencil = false;
	pipelineSpecification.depthstencil.stencil.compareMask = 0x00;
	pipelineSpecification.depthstencil.stencil.writeMask = 0x00;
	pipelineSpecification.depthstencil.stencil.reference = 0;
	pipelineSpecification.depthstencil.stencil.compareOp = Lvn_CompareOperation_Never;
	pipelineSpecification.depthstencil.stencil.depthFailOp = Lvn_StencilOperation_Keep;
	pipelineSpecification.depthstencil.stencil.failOp = Lvn_StencilOperation_Keep;
	pipelineSpecification.depthstencil.stencil.passOp = Lvn_StencilOperation_Keep;

	lvnctx->defaultPipelineSpecification = pipelineSpecification;
}

LvnGraphicsApi getGraphicsApi()
{
	return lvn::getContext()->graphicsapi;
}

const char* getGraphicsApiName()
{
	switch (lvn::getContext()->graphicsapi)
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
	lvn::getContext()->graphicsContext.getPhysicalDevices(nullptr, &getDeviceCount);

	if (pPhysicalDevices == nullptr)
	{
		*deviceCount = getDeviceCount;
		return;
	}

	lvn::getContext()->graphicsContext.getPhysicalDevices(pPhysicalDevices, &getDeviceCount);

	return;
}

LvnPhysicalDeviceInfo getPhysicalDeviceInfo(LvnPhysicalDevice* physicalDevice)
{
	return physicalDevice->info;
}

LvnResult checkPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice)
{
	return lvn::getContext()->graphicsContext.checkPhysicalDeviceSupport(physicalDevice);
}

LvnResult renderInit(LvnRenderInitInfo* renderInfo)
{
	return lvn::getContext()->graphicsContext.renderInit(renderInfo);
}

void renderClearColor(LvnWindow* window, float r, float g, float b, float a)
{
	lvn::getContext()->graphicsContext.renderClearColor(window, r, g, b, a);
}

void renderCmdDraw(LvnWindow* window, uint32_t vertexCount)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdDraw(window, vertexCount);
}

void renderCmdDrawIndexed(LvnWindow* window, uint32_t indexCount)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdDrawIndexed(window, indexCount);
}

void renderCmdDrawInstanced(LvnWindow* window, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdDrawInstanced(window, vertexCount, instanceCount, firstInstance);
}

void renderCmdDrawIndexedInstanced(LvnWindow* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdDrawIndexedInstanced(window, indexCount, instanceCount, firstInstance);
}

void renderCmdSetStencilReference(uint32_t reference)
{

}

void renderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask)
{

}

void renderBeginNextFrame(LvnWindow* window)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderBeginNextFrame(window);
}

void renderDrawSubmit(LvnWindow* window)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderDrawSubmit(window);
}

void renderBeginCommandRecording(LvnWindow* window)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderBeginCommandRecording(window);
}

void renderEndCommandRecording(LvnWindow* window)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderEndCommandRecording(window);
}

void renderCmdBeginRenderPass(LvnWindow* window)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdBeginRenderPass(window);
}

void renderCmdEndRenderPass(LvnWindow* window)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdEndRenderPass(window);
}

void renderCmdBindPipeline(LvnWindow* window, LvnPipeline* pipeline)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdBindPipeline(window, pipeline);
}

void renderCmdBindVertexBuffer(LvnWindow* window, LvnBuffer* buffer)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdBindVertexBuffer(window, buffer);
}

void renderCmdBindIndexBuffer(LvnWindow* window, LvnBuffer* buffer)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdBindIndexBuffer(window, buffer);
}

void renderCmdBindDescriptorSets(LvnWindow* window, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdBindDescriptorSets(window, pipeline, firstSetIndex, descriptorSetCount, pDescriptorSets);
}

void renderCmdBeginFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdBeginFrameBuffer(window, frameBuffer);
}

void renderCmdEndFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer)
{
	int width, height;
	lvn::windowGetSize(window, &width, &height);
	if (width * height <= 0) { return; }

	lvn::getContext()->graphicsContext.renderCmdEndFrameBuffer(window, frameBuffer);
}

LvnResult createShaderFromSrc(LvnShader** shader, LvnShaderCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

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
	lvnctx->objectMemoryAllocations.shaders++;
	LVN_CORE_TRACE("created shader (from source): (%p)", *shader);
	return lvnctx->graphicsContext.createShaderFromSrc(*shader, createInfo);
}

LvnResult createShaderFromFileSrc(LvnShader** shader, LvnShaderCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

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
	lvnctx->objectMemoryAllocations.shaders++;
	LVN_CORE_TRACE("created shader (from source file): (%p), vertex file: %s, fragment file: %s", *shader, createInfo->vertexSrc, createInfo->fragmentSrc);
	return lvnctx->graphicsContext.createShaderFromFileSrc(*shader, createInfo);
}

LvnResult createShaderFromFileBin(LvnShader** shader, LvnShaderCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

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
	lvnctx->objectMemoryAllocations.shaders++;
	LVN_CORE_TRACE("created shader (from binary file): (%p), vertex file: %s, fragment file: %s", *shader, createInfo->vertexSrc, createInfo->fragmentSrc);
	return lvnctx->graphicsContext.createShaderFromFileBin(*shader, createInfo);
}

LvnResult createDescriptorLayout(LvnDescriptorLayout** descriptorLayout, LvnDescriptorLayoutCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

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
	lvnctx->objectMemoryAllocations.descriptorLayouts++;
	LVN_CORE_TRACE("created descriptorLayout: (%p), descriptor binding count: %u", *descriptorLayout, createInfo->descriptorBindingCount);
	return lvnctx->graphicsContext.createDescriptorLayout(*descriptorLayout, createInfo);
}

LvnResult createDescriptorSet(LvnDescriptorSet** descriptorSet, LvnDescriptorLayout* descriptorLayout)
{
	LvnContext* lvnctx = lvn::getContext();

	*descriptorSet = new LvnDescriptorSet();
	lvnctx->objectMemoryAllocations.descriptorSets++;
	LVN_CORE_TRACE("created descriptorSet: (%p) from descriptorLayout: (%p)", *descriptorSet, descriptorLayout);
	return lvnctx->graphicsContext.createDescriptorSet(*descriptorSet, descriptorLayout);
}

LvnResult createPipeline(LvnPipeline** pipeline, LvnPipelineCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

	*pipeline = new LvnPipeline();
	lvnctx->objectMemoryAllocations.pipelines++;
	LVN_CORE_TRACE("created pipeline: (%p)", *pipeline);
	return lvnctx->graphicsContext.createPipeline(*pipeline, createInfo);
}

LvnResult createFrameBuffer(LvnFrameBuffer** frameBuffer, LvnFrameBufferCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

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
	lvnctx->objectMemoryAllocations.frameBuffers++;
	LVN_CORE_TRACE("created framebuffer: (%p)", *frameBuffer);
	return lvnctx->graphicsContext.createFrameBuffer(*frameBuffer, createInfo);
}

LvnResult createBuffer(LvnBuffer** buffer, LvnBufferCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

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
	lvnctx->objectMemoryAllocations.buffers++;
	LVN_CORE_TRACE("created buffer: (%p)", *buffer);
	return lvnctx->graphicsContext.createBuffer(*buffer, createInfo);
}

LvnResult createUniformBuffer(LvnUniformBuffer** uniformBuffer, LvnUniformBufferCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

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
	lvnctx->objectMemoryAllocations.uniformBuffers++;
	LVN_CORE_TRACE("created uniform buffer: (%p), binding: %u, size: %lu bytes", *uniformBuffer, createInfo->binding, createInfo->size);
	return lvnctx->graphicsContext.createUniformBuffer(*uniformBuffer, createInfo);
}

LvnResult createTexture(LvnTexture** texture, LvnTextureCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

	*texture = new LvnTexture();
	lvnctx->objectMemoryAllocations.textures++;
	LVN_CORE_TRACE("created texture: (%p) using image data: (%p), (w:%u,h:%u,ch:%u), total size: %u bytes", *texture, createInfo->imageData.pixels.data(), createInfo->imageData.width, createInfo->imageData.height, createInfo->imageData.channels, createInfo->imageData.pixels.memsize());
	return lvnctx->graphicsContext.createTexture(*texture, createInfo);
}

LvnResult createCubemap(LvnCubemap** cubemap, LvnCubemapCreateInfo* createInfo)
{
	LvnContext* lvnctx = lvn::getContext();

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
	lvnctx->objectMemoryAllocations.cubemaps++;
	LVN_CORE_TRACE("created cubemap: (%p)", *cubemap);
	return lvnctx->graphicsContext.createCubemap(*cubemap, createInfo);
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
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyShader(shader);
	delete shader;
	shader = nullptr;
	lvnctx->objectMemoryAllocations.shaders--;
}

void destroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout)
{
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyDescriptorLayout(descriptorLayout);
	delete descriptorLayout;
	descriptorLayout = nullptr;
	lvnctx->objectMemoryAllocations.descriptorLayouts--;
}

void destroyDescriptorSet(LvnDescriptorSet* descriptorSet)
{
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyDescriptorSet(descriptorSet);
	delete descriptorSet;
	descriptorSet = nullptr;
	lvnctx->objectMemoryAllocations.descriptorSets--;
}

void destroyPipeline(LvnPipeline* pipeline)
{
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyPipeline(pipeline);
	delete pipeline;
	pipeline = nullptr;
	lvnctx->objectMemoryAllocations.pipelines--;
}

void destroyFrameBuffer(LvnFrameBuffer* frameBuffer)
{
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyFrameBuffer(frameBuffer);
	delete frameBuffer;
	frameBuffer = nullptr;
	lvnctx->objectMemoryAllocations.frameBuffers--;
}

void destroyBuffer(LvnBuffer* buffer)
{
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyBuffer(buffer);
	delete buffer;
	buffer = nullptr;
	lvnctx->objectMemoryAllocations.buffers--;
}

void destroyUniformBuffer(LvnUniformBuffer* uniformBuffer)
{
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyUniformBuffer(uniformBuffer);
	delete uniformBuffer;
	uniformBuffer = nullptr;
	lvnctx->objectMemoryAllocations.uniformBuffers--;
}

void destroyTexture(LvnTexture* texture)
{
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyTexture(texture);
	delete texture;
	texture = nullptr;
	lvnctx->objectMemoryAllocations.textures--;
}

void destroyCubemap(LvnCubemap* cubemap)
{
	LvnContext* lvnctx = lvn::getContext();

	lvnctx->graphicsContext.destroyCubemap(cubemap);
	delete cubemap;
	cubemap = nullptr;
	lvnctx->objectMemoryAllocations.cubemaps--;
}

void destroyMesh(LvnMesh* mesh)
{
	lvn::destroyBuffer(mesh->buffer);
}

void pipelineSpecificationSetConfig(LvnPipelineSpecification* pipelineSpecification)
{
	LVN_CORE_ASSERT(pipelineSpecification != nullptr, "pipeline specification points to nullptr when setting pipeline specification config");
	LvnContext* lvnctx = lvn::getContext();
	lvnctx->defaultPipelineSpecification = *pipelineSpecification;
}

LvnPipelineSpecification pipelineSpecificationGetConfig()
{
	LvnContext* lvnctx = lvn::getContext();
	return lvnctx->defaultPipelineSpecification;
}

void bufferUpdateVertexData(LvnBuffer* buffer, void* vertices, uint32_t size, uint32_t offset)
{
	lvn::getContext()->graphicsContext.bufferUpdateVertexData(buffer, vertices, size, offset);
}

void bufferUpdateIndexData(LvnBuffer* buffer, uint32_t* indices, uint32_t size, uint32_t offset)
{
	lvn::getContext()->graphicsContext.bufferUpdateIndexData(buffer, indices, size, offset);
}

void bufferResizeVertexBuffer(LvnBuffer* buffer, uint32_t size)
{
	lvn::getContext()->graphicsContext.bufferResizeVertexBuffer(buffer, size);
}

void bufferResizeIndexBuffer(LvnBuffer* buffer, uint32_t size)
{
	lvn::getContext()->graphicsContext.bufferResizeIndexBuffer(buffer, size);
}

LvnTexture* cubemapGetTextureData(LvnCubemap* cubemap)
{
	return &cubemap->textureData;
}

void updateUniformBufferData(LvnWindow* window, LvnUniformBuffer* uniformBuffer, void* data, uint64_t size)
{
	lvn::getContext()->graphicsContext.updateUniformBufferData(window, uniformBuffer, data, size);
}

void updateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count)
{
	lvn::getContext()->graphicsContext.updateDescriptorSetData(descriptorSet, pUpdateInfo, count);
}

LvnTexture* frameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex)
{
	return lvn::getContext()->graphicsContext.frameBufferGetImage(frameBuffer, attachmentIndex);
}

LvnRenderPass* frameBufferGetRenderPass(LvnFrameBuffer* frameBuffer)
{
	return lvn::getContext()->graphicsContext.frameBufferGetRenderPass(frameBuffer);
}

void frameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height)
{
	return lvn::getContext()->graphicsContext.framebufferResize(frameBuffer, width, height);
}

void frameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a)
{
	return lvn::getContext()->graphicsContext.frameBufferSetClearColor(frameBuffer, attachmentIndex, r, g, b, a);
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

LvnImageData loadImageData(const char* filepath, int forceChannels, bool flipVertically)
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

	stbi_set_flip_vertically_on_load(flipVertically);
	int imageWidth, imageHeight, imageChannels;
	stbi_uc* pixels = stbi_load(filepath, &imageWidth, &imageHeight, &imageChannels, forceChannels);

	if (!pixels)
	{
		LVN_CORE_ERROR("loadImageData(LvnImageData*, const char*) | failed to load image pixel data from file: %s", filepath);
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
	LvnContext* lvnctx = lvn::getContext();

	if (createInfo->filepath == nullptr)
	{
		LVN_CORE_ERROR("createSoundFromFile(LvnSound**, LvnSoundCreateInfo*) | createInfo->filepath is nullptr, cannot load sound data without a valid path to the sound file");
		return Lvn_Result_Failure;
	}

	ma_engine* pEngine = static_cast<ma_engine*>(lvnctx->audioEngineContextPtr);

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

	lvnctx->objectMemoryAllocations.sounds++;
	return Lvn_Result_Success;
}

void destroySound(LvnSound* sound)
{
	LvnContext* lvnctx = lvn::getContext();

	ma_sound* pSound = static_cast<ma_sound*>(sound->soundPtr);

	ma_sound_uninit(pSound);
	lvn::memFree(pSound);

	delete sound;
	lvnctx->objectMemoryAllocations.sounds--;
}

LvnSoundCreateInfo soundConfigInit(const char* filepath)
{
	LvnSoundCreateInfo soundInit{};
	soundInit.pos = { 0.0f, 0.0f, 0.0f };
	soundInit.volume = 1.0f;
	soundInit.pan = 0.0f;
	soundInit.pitch = 1.0f;
	soundInit.looping = false;
	soundInit.filepath = filepath;

	return soundInit;
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

void soundPlayStart(LvnSound* sound)
{
	ma_sound* pSound = static_cast<ma_sound*>(sound->soundPtr);
	ma_sound_start(pSound);
}

void soundPlayStop(LvnSound* sound)
{
	ma_sound* pSound = static_cast<ma_sound*>(sound->soundPtr);
	ma_sound_stop(pSound);
}

void soundTogglePause(LvnSound* sound)
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

float clampAngle(float rad)
{
	float angle = fmod(rad, 2 * LVN_PI);
	if (angle < 0) { angle += 2 * LVN_PI; }
	return angle;
}

float clampAngleDeg(float deg)
{
	float angle = fmod(deg, 360.0f);
	if (angle < 0) { angle += 360.0f; }
	return angle;
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
