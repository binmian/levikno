#ifndef HG_LEVIKNO_H
#define HG_LEVIKNO_H

// [LAYOUT]:
// ------------------------------------------------------------
//
// [SECTION]: Config
// [SECTION]: Includes
// [SECTION]: Enums
// [SECTION]: Struct Declaration
// [SECTION]: Functions
// [SECTION]: Struct Implementation


// [SECTION]: Config
// ------------------------------------------------------------

// platform
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #ifndef LVN_PLATFORM_WINDOWS
        #define LVN_PLATFORM_WINDOWS
    #endif

#elif __APPLE__
    #ifndef LVN_PLATFORM_APPLE
        #define LVN_PLATFORM_APPLE
    #endif

#elif __linux__
    #ifndef LVN_PLATFORM_LINUX
        #define LVN_PLATFORM_LINUX
    #endif
#else
    #error "levikno does not support the current platform."
#endif

// dll
#ifdef LVN_PLATFORM_WINDOWS
    #ifdef LVN_SHARED_LIBRARY_EXPORT
        #define LVN_API __declspec(dllexport)
    #elif LVN_SHARED_LIBRARY_IMPORT
        #define LVN_API __declspec(dllimport)
    #else
        #define LVN_API
    #endif
#else
  #define LVN_API
#endif


// compiler
#ifdef _MSC_VER
    #define LVN_ASSERT_BREAK __debugbreak()
    #pragma warning (disable : 4267)
    #pragma warning (disable : 4244)
    #pragma warning (disable : 26495)

    #ifdef _DEBUG
        #ifndef LVN_CONFIG_DEBUG
            #define LVN_CONFIG_DEBUG
        #endif
    #endif
#else
    #ifndef NDEBUG
        #ifndef LVN_CONFIG_DEBUG
            #define LVN_CONFIG_DEBUG
        #endif
    #endif
#endif


// debug
#ifdef LVN_CONFIG_DEBUG
    #define LVN_ENABLE_ASSERTS
#endif

#if defined (LVN_ENABLE_ASSERTS)
    #define LVN_ASSERT(x,msg) assert(x && msg)
#else
    #define LVN_ASSERT(x, ...)
#endif


// allocation
#ifndef LVN_MALLOC
    #define LVN_MALLOC(sz) ::lvn::memAlloc(sz)
#endif

#ifndef LVN_FREE
    #define LVN_FREE(p) ::lvn::memFree(p)
#endif

#ifndef LVN_REALLOC
    #define LVN_REALLOC(p,sz) ::lvn::memRealloc(p,sz)
#endif

// glslang
#ifdef LVN_INCLUDE_GLSLANG_SUPPORTED
    #define LVN_INCLUDE_GLSLANG_SRC_COMPILE_SUPPORT
#endif


// misc
#define LVN_TRUE 1
#define LVN_FALSE 0
#define LVN_NULL_HANDLE nullptr

#define LVN_FILE_NAME __FILE__
#define LVN_LINE __LINE__
#define LVN_FUNC_NAME __func__

#define LVN_STR(x) #x
#define LVN_STRINGIFY(x) LVN_STR(x)

#ifndef M_PI
    #define M_PI 3.1415926535897932384626433832795
#endif

#define LVN_PI ((float)M_PI)
#define LVN_PI_EXACT (22.0/7.0)


// logging

/*
*   Color          | FG | BG
* -----------------+----+----
*   Black          | 30 | 40
*   Red            | 31 | 41
*   Green          | 32 | 42
*   Yellow         | 33 | 43
*   Blue           | 34 | 44
*   Magenta        | 35 | 45
*   Cyan           | 36 | 46
*   White          | 37 | 47
*   Bright Black   | 90 | 100
*   Bright Red     | 91 | 101
*   Bright Green   | 92 | 102
*   Bright Yellow  | 93 | 103
*   Bright Blue    | 94 | 104
*   Bright Magenta | 95 | 105
*   Bright Cyan    | 96 | 106
*   Bright White   | 97 | 107
*
*
*   reset             0
*   bold/bright       1
*   underline         4
*   inverse           7
*   bold/bright off  21
*   underline off    24
*   inverse off      27
*
*
*   Log Colors:
*   TRACE           \x1b[0;37m
*   DEBUG           \x1b[0;34m
*   INFO            \x1b[0;32m
*   WARN            \x1b[1;33m
*   ERROR           \x1b[1;31m
*   FATAL           \x1b[1;37;41m
*
*/

#define LVN_LOG_COLOR_TRACE                     "\x1b[0;37m"
#define LVN_LOG_COLOR_DEBUG                     "\x1b[0;34m"
#define LVN_LOG_COLOR_INFO                      "\x1b[0;32m"
#define LVN_LOG_COLOR_WARN                      "\x1b[1;33m"
#define LVN_LOG_COLOR_ERROR                     "\x1b[1;31m"
#define LVN_LOG_COLOR_FATAL                     "\x1b[1;37;41m"
#define LVN_LOG_COLOR_RESET                     "\x1b[0m"


// Core Log macros
#define LVN_CORE_TRACE(...)                     ::lvn::logMessageTrace(lvn::logGetCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_DEBUG(...)                     ::lvn::logMessageDebug(lvn::logGetCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_INFO(...)                      ::lvn::logMessageInfo(lvn::logGetCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_WARN(...)                      ::lvn::logMessageWarn(lvn::logGetCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_ERROR(...)                     ::lvn::logMessageError(lvn::logGetCoreLogger(), ##__VA_ARGS__)
#define LVN_CORE_FATAL(...)                     ::lvn::logMessageFatal(lvn::logGetCoreLogger(), ##__VA_ARGS__)

// Client Log macros
#define LVN_TRACE(...)                          ::lvn::logMessageTrace(lvn::logGetClientLogger(), ##__VA_ARGS__)
#define LVN_DEBUG(...)                          ::lvn::logMessageDebug(lvn::logGetClientLogger(), ##__VA_ARGS__)
#define LVN_INFO(...)                           ::lvn::logMessageInfo(lvn::logGetClientLogger(), ##__VA_ARGS__)
#define LVN_WARN(...)                           ::lvn::logMessageWarn(lvn::logGetClientLogger(), ##__VA_ARGS__)
#define LVN_ERROR(...)                          ::lvn::logMessageError(lvn::logGetClientLogger(), ##__VA_ARGS__)
#define LVN_FATAL(...)                          ::lvn::logMessageFatal(lvn::logGetClientLogger(), ##__VA_ARGS__)


// [SECTION]: Includes
// ------------------------------------------------------------

#include <cstdint> // uint8_t, uint16_t, uint32_t, uint64_t
#include <cstddef> // size_t
#include <cassert> // assert
#include <new>     // operator new
#include <type_traits>


// [SECTION]: Enums
// ------------------------------------------------------------

enum LvnResult : int
{
    Lvn_Result_Success              =  0,
    Lvn_Result_Failure              = -1,
    Lvn_Result_AlreadyCalled        = -2,
    Lvn_Result_MemAllocFailure      = -3,
    Lvn_Result_TimeOut              = -4,
};

enum LvnLogLevel
{
    Lvn_LogLevel_None       = 0,
    Lvn_LogLevel_Trace      = 1,
    Lvn_LogLevel_Debug      = 2,
    Lvn_LogLevel_Info       = 3,
    Lvn_LogLevel_Warn       = 4,
    Lvn_LogLevel_Error      = 5,
    Lvn_LogLevel_Fatal      = 6,
};

enum LvnFileMode
{
    Lvn_FileMode_Write,
    Lvn_FileMode_Append,
};


// [SECTION]: Struct Declaration
// ------------------------------------------------------------

struct LvnLogger;
struct LvnLoggerCreateInfo;
struct LvnLoggingContext;
struct LvnLoggingContextCreateInfo;
struct LvnLogMessage;
struct LvnLogPattern;


// classes
template <typename T>
class LvnVector;

class LvnString;


// callbacks
typedef void* (*LvnMemAllocFunc)(size_t sz, void* userData);
typedef void  (*LvnMemFreeFunc)(void* ptr, void* userData);
typedef void* (*LvnMemReallocFunc)(void* ptr, size_t sz, void* userData);

namespace lvn
{
    LVN_API void*                   memAlloc(size_t size);                              // custom memory allocation function that allocates memory given the size of memory, note that function is connected with the context and will keep track of allocation counts, will increment number of allocations per use
    LVN_API void                    memFree(void* ptr);                                 // custom memory free function, note that it keeps track of memory allocations remaining, decrements number of allocations per use with lvn::memAlloc
    LVN_API void*                   memRealloc(void* ptr, size_t size);                 // custom memory realloc function

    LVN_API void                    setMemFuncs(LvnMemAllocFunc allocFunc, LvnMemFreeFunc freeFunc, LvnMemReallocFunc reallocFunc, void* userData);
    LVN_API LvnMemAllocFunc         getMemAllocFunc();
    LVN_API LvnMemFreeFunc          getMemFreeFunc();
    LVN_API LvnMemReallocFunc       getMemReallocFunc();
    LVN_API void*                   getMemUserData();
    LVN_API size_t                  getMemAllocCount();

#ifdef LVN_CONFIG_DEBUG
    LVN_API inline size_t i_ObjectAllocationCount = 0;
    LVN_API inline size_t getObjectAllocationCount() { return i_ObjectAllocationCount; }
#endif

    template <typename T>
    LVN_API constexpr T* memNew(size_t size = 1, bool construct = true)
    {
        if (size == 0) { return nullptr; }
    #ifdef LVN_CONFIG_DEBUG
        i_ObjectAllocationCount++;
    #endif
        T* memalloc = (T*)(*lvn::getMemAllocFunc())(size * sizeof(T), lvn::getMemUserData());
        if (construct)
        {
            for (size_t i = 0; i < size; i++)
                new (memalloc + i) T();
        }
        return memalloc;
    }

    template <typename T>
    LVN_API constexpr void memDelete(T* ptr, size_t size = 1)
    {
        if (ptr == nullptr) { return; }
    #ifdef LVN_CONFIG_DEBUG
        i_ObjectAllocationCount--;
    #endif
        if (!std::is_trivially_destructible_v<T>)
        {
            for (size_t i = 0; i < size; i++)
                ptr[i].~T();
        }
        (*lvn::getMemFreeFunc())(ptr, lvn::getMemUserData());
    }


    LVN_API int                     dateGetYear();                                      // get the year number (eg. 2025)
    LVN_API int                     dateGetYear02d();                                   // get the last two digits of the year number (eg. 25)
    LVN_API int                     dateGetMonth();                                     // get the month number (1...12)
    LVN_API int                     dateGetDay();                                       // get the date number (1...31)
    LVN_API int                     dateGetHour();                                      // get the hour of the current day in 24 hour format (0...24)
    LVN_API int                     dateGetHour12();                                    // get the hour of the current day in 12 hour format (0...12)
    LVN_API int                     dateGetMinute();                                    // get the minute of the current day (0...60)
    LVN_API int                     dateGetSecond();                                    // get the second of the current dat (0...60)
    LVN_API long long               dateGetSecondsSinceEpoch();                         // get the time in seconds since 00::00:00 UTC 1 January 1970

    LVN_API const char*             dateGetMonthName();                                 // get the current month name (eg. January, April)
    LVN_API const char*             dateGetMonthNameShort();                            // get the current month shortened name (eg. Jan, Apr)
    LVN_API const char*             dateGetWeekDayName();                               // get the current day name in the week (eg. Monday, Friday)
    LVN_API const char*             dateGetWeekDayNameShort();                          // get the current day shortened name in the week (eg. Mon, Fri)
    LVN_API const char*             dateGetTimeMeridiem();                              // get the time meridiem of the current day (eg. AM, PM)
    LVN_API const char*             dateGetTimeMeridiemLower();                         // get the time meridiem of the current day in lower case (eg. am, pm)

    LVN_API LvnString               dateGetTimeHHMMSS();                                // get the time in HH:MM:SS format (eg. 14:34:54)
    LVN_API LvnString               dateGetTime12HHMMSS();                              // get the time in HH:MM:SS 12 hour format (eg. 2:23:14)
    LVN_API LvnString               dateGetYearStr();                                   // get the current year number as a string
    LVN_API LvnString               dateGetYear02dStr();                                // get the last two digits of the current year number as a string
    LVN_API LvnString               dateGetMonthNumStr();                               // get the current month number as a string
    LVN_API LvnString               dateGetDayNumStr();                                 // get the current day number as a string
    LVN_API LvnString               dateGetHourNumStr();                                // get the current hour number as a string
    LVN_API LvnString               dateGetHour12NumStr();                              // get the current hour number in 12 hour format as a string
    LVN_API LvnString               dateGetMinuteNumStr();                              // get the current minute as a string
    LVN_API LvnString               dateGetSecondNumStr();                              // get the current second as a string


    // logging
    LVN_API LvnResult               initLogging(LvnLoggingContextCreateInfo* createInfo);
    LVN_API void                    terminateLogging();
    LVN_API LvnLoggingContext*      getLoggingContex();
    LVN_API void                    logEnable(bool enable);                                                           // enable or disable logging
    LVN_API void                    logEnableCoreLogging(bool enable);                                                // enable or disable logging from the core logger
    LVN_API void                    logSetLevel(LvnLogger* logger, LvnLogLevel level);                                // sets the log level of logger, will only print messages with set log level and higher
    LVN_API void                    logSetFileConfig(LvnLogger* logger, bool enable, const char* filename = "", LvnFileMode filemode = Lvn_FileMode_Write);  // sets the log file config, whether to enable logging and the log file name and mode
    LVN_API bool                    logCheckLevel(LvnLogger* logger, LvnLogLevel level);                              // checks level with loger, returns true if level is the same or higher level than the level of the logger
    LVN_API void                    logRenameLogger(LvnLogger* logger, const char* name);                             // renames the name of the logger
    LVN_API void                    logOutputMessage(LvnLogger* logger, LvnLogMessage* msg);                          // prints the log message
    LVN_API LvnString               logFormatMessage(LvnLogger* logger, LvnLogLevel level, const char* msg, bool removeANSI = false); // formats the log message into the log pattern set by the logger
    LVN_API void                    logMessage(LvnLogger* logger, LvnLogLevel level, const char* msg);                // log message with given log level
    LVN_API void                    logMessageTrace(LvnLogger* logger, const char* fmt, ...);                         // log message with level trace; ANSI code "\x1b[0;37m"
    LVN_API void                    logMessageDebug(LvnLogger* logger, const char* fmt, ...);                         // log message with level debug; ANSI code "\x1b[0;34m"
    LVN_API void                    logMessageInfo(LvnLogger* logger, const char* fmt, ...);                          // log message with level info;  ANSI code "\x1b[0;32m"
    LVN_API void                    logMessageWarn(LvnLogger* logger, const char* fmt, ...);                          // log message with level warn;  ANSI code "\x1b[1;33m"
    LVN_API void                    logMessageError(LvnLogger* logger, const char* fmt, ...);                         // log message with level error; ANSI code "\x1b[1;31m"
    LVN_API void                    logMessageFatal(LvnLogger* logger, const char* fmt, ...);                         // log message with level fatal; ANSI code "\x1b[1;37;41m"
    LVN_API LvnLogger*              logGetCoreLogger();
    LVN_API LvnLogger*              logGetClientLogger();
    LVN_API const char*             logGetANSIcodeColor(LvnLogLevel level);                                           // get the ANSI color code of the log level in a string
    LVN_API LvnResult               logSetPatternFormat(LvnLogger* logger, const char* patternfmt);                   // set the log pattern of the logger; messages outputed from that logger will be in this format
    LVN_API LvnResult               logAddPatterns(LvnLogPattern* pLogPatterns, uint32_t count);                      // add user defined log patterns to the library

    LVN_API LvnResult               createLogger(LvnLogger** logger, const LvnLoggerCreateInfo* loggerCreateInfo);
    LVN_API void                    destroyLogger(LvnLogger* logger);
} /* namespace lvn */


// [SECTION]: Struct Implementation
// ------------------------------------------------------------

// -- LvnVector
// ------------------------------------------------------------
// - simple and light weight replacement to std::vector
// - this vector implementation is not intended to be used outside of the library, use std::vector instead

template <typename T>
class LvnVector
{
private:
    T* m_Data;            /* pointer array to data */
    size_t m_Size;      /* number of elements that are in this vector; size of vector */
    size_t m_Capacity;  /* max number of elements allocated/reserved for this vector; note that m_Size can be less than or equal to the capacity */

    void destruct() { if constexpr (!std::is_trivially_destructible_v<T>) { for (size_t i = 0; i < m_Size; i++) m_Data[i].~T(); } }
    void destruct_at(T* value) { if constexpr (!std::is_trivially_destructible_v<T>) value->~T(); }

public:
    LvnVector()
        : m_Data(nullptr), m_Size(0), m_Capacity(0) {}
    ~LvnVector()
    {
        lvn::memDelete<T>(m_Data, m_Size);
        m_Size = 0;
        m_Capacity = 0;
        m_Data = nullptr;
    }

    LvnVector(size_t size)
    {
        m_Size = size;
        m_Capacity = size;
        m_Data = lvn::memNew<T>(size);
    }
    LvnVector(const T* data, size_t size)
    {
        m_Size = size;
        m_Capacity = size;
        m_Data = lvn::memNew<T>(size, false);
        for (size_t i = 0; i < size; i++)
            new (&m_Data[i]) T(data[i]);
    }
    LvnVector(const T* begin, const T* end)
    {
        LVN_ASSERT(end > begin, "end element pointer must be after before element pointer");
        m_Size = end - begin;
        m_Capacity = m_Size;
        m_Data = lvn::memNew<T>(m_Size, false);
        for (size_t i = 0; i < m_Size; i++)
            new (&m_Data[i]) T(begin[i]);
    }
    LvnVector(size_t size, const T& value)
    {
        m_Size = size;
        m_Capacity = size;
        m_Data = lvn::memNew<T>(size, false);
        for (size_t i = 0; i < size; i++)
            new (&m_Data[i]) T(value);
    }
    LvnVector(const LvnVector& other)
    {
        m_Size = other.m_Size;
        m_Capacity = other.m_Size; /* NOTE: we are only allocating up to the size of the other vector, not the capacity */
        m_Data = lvn::memNew<T>(other.m_Size, false);
        for (size_t i = 0; i < other.m_Size; i++)
            new (&m_Data[i]) T(other.m_Data[i]);
    }
    LvnVector(LvnVector&& other)
    {
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;
        m_Data = other.m_Data;
        other.m_Size = 0;
        other.m_Capacity = 0;
        other.m_Data = nullptr;
    }
    LvnVector& operator=(const LvnVector& other)
    {
        if (this == &other) return *this;
        resize(other.m_Size);
        for (size_t i = 0; i < m_Size; i++)
            new (&m_Data[i]) T(other.m_Data[i]);
        return *this;
    }
    LvnVector& operator=(LvnVector&& other)
    {
        lvn::memDelete<T>(m_Data, m_Size);
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;
        m_Data = other.m_Data;
        other.m_Size = 0;
        other.m_Capacity = 0;
        other.m_Data = nullptr;
        return *this;
    }

    T& operator[](size_t i)
    {
        LVN_ASSERT(i < m_Size, "index out of vector size range");
        return m_Data[i];
    }
    const T& operator[](size_t i) const
    {
        LVN_ASSERT(i < m_Size, "index out of vector size range");
        return m_Data[i];
    }

    void insert(const T* it, const T& value)
    {
        LVN_ASSERT(it >= m_Data && it <= m_Data + m_Size, "insert element not within vector bounds");
        size_t index = it - m_Data;
        insert_index(index, value);
    }
    void insert(const T* it, const T* begin, const T* end)
    {
        LVN_ASSERT(it >= m_Data && it <= m_Data + m_Size, "insert element not within vector bounds");
        LVN_ASSERT(end >= begin, "end insert element cannot be less than begin insert element");
        size_t index = it - m_Data;
        insert_index(index, begin, end);
    }
    void insert(const T* it, const T* data, size_t size)
    {
        if (size == 0) return;
        LVN_ASSERT(it >= m_Data && it <= m_Data + m_Size, "insert element not within vector bounds");
        size_t index = it - m_Data;
        insert_index(index, data, size);
    }
    void insert_index(size_t index, const T& value)
    {
        LVN_ASSERT(index <= m_Size, "insert index not within vector bounds");
        insert_index(index, &value, 1);
    }
    void insert_index(size_t index, const T* begin, const T* end)
    {
        LVN_ASSERT(index <= m_Size, "insert index not within vector bounds");
        LVN_ASSERT(end >= begin, "end insert element cannot be less than begin insert element");
        size_t count = end - begin;
        if (count == 0) return;
        insert_index(index, begin, count);
    }
    void insert_index(size_t index, const T* data, size_t size)
    {
        if (size == 0) return;
        LVN_ASSERT(index <= m_Size, "insert index not within vector bounds");
        reserve(m_Size + size);

        /* shift elements to the right */
        for (int64_t i = m_Size - 1; i >= (int64_t)index; --i)
        {
            new (m_Data + i + size) T(static_cast<std::remove_reference_t<T>&&>(m_Data[i])); /* NOTE: cast to rvalue for move constructor */
            destruct_at(m_Data + i);
        }

        /* construct new elements in place at index */
        for (size_t i = 0; i < size; ++i)
            new (m_Data + index + i) T(data[i]);

        m_Size += size;
    }

    T*          begin() { return m_Data; }
    const T*    begin() const { return m_Data; }
    T*          end() { return m_Data + m_Size; }
    const T*    end() const { return m_Data + m_Size; }
    T&          front() { LVN_ASSERT(m_Size > 0, "cannot access index of empty vector"); return m_Data[0]; }
    const T&    front() const { LVN_ASSERT(m_Size > 0, "cannot access index of empty vector"); return m_Data[0]; }
    T&          back() { LVN_ASSERT(m_Size > 0, "cannot access index of empty vector"); return m_Data[m_Size - 1]; }
    const T&    back() const { LVN_ASSERT(m_Size > 0, "cannot access index of empty vector"); return m_Data[m_Size - 1]; }

    bool        empty() const { return m_Size == 0; }
    void        clear() { destruct(); m_Size = 0; }
    void        clear_free() { if (m_Data) { lvn::memDelete<T>(m_Data, m_Size); m_Size = m_Capacity = 0; m_Data = nullptr; } }
    void        erase(const T* it) { LVN_ASSERT(it >= m_Data && it < m_Data + m_Size, "erase element not within vector bounds"); size_t index = it - m_Data; erase_index(index); }
    void        erase_index(size_t index) { LVN_ASSERT(index < m_Size, "index out of vector size range"); size_t aftIndex = m_Size - index - 1; if (aftIndex != 0) { for (size_t i = index + 1; i < m_Size; i++) { m_Data[i - 1] = m_Data[i]; } destruct_at(&m_Data[m_Size - 1]); } --m_Size; }
    T*          data() { return m_Data; }
    const T*    data() const { return m_Data; }
    size_t      size() const { return m_Size; }
    size_t      capacity() const { return m_Capacity; }
    size_t      memsize() const { return m_Size * sizeof(T); }
    size_t      memcap() const { return m_Capacity * sizeof(T); }
    void        resize(size_t size) { if (size > m_Size) { reserve(size); for (size_t i = m_Size; i < size; i++) { new (m_Data + i) T(); } } else { for (size_t i = size; i < m_Size; i++) destruct_at(&m_Data[i]); }  m_Size = size; }
    void        resize(size_t size, const T& value) { if (size > m_Size) { reserve(size); for (size_t i = m_Size; i < size; i++) { new (m_Data + i) T(value); } } else { for (size_t i = size; i < m_Size; i++) destruct_at(&m_Data[i]); }  m_Size = size; }
    void        reserve(size_t size) { if (size <= m_Capacity) return; T* temp = lvn::memNew<T>(size, false); for (size_t i = 0; i < m_Size; i++) { new (temp + i) T(m_Data[i]); } lvn::memDelete<T>(m_Data, m_Size); m_Data = temp; m_Capacity = size; }
    void        shrink_to_fit() { if (m_Size >= m_Capacity) { return; } T* temp = lvn::memNew<T>(m_Size, false); for (size_t i = 0; i < m_Size; i++) { new (temp + i) T(m_Data[i]); } lvn::memDelete<T>(m_Data, m_Size); m_Data = temp; m_Capacity = m_Size; }

    void        push_back(const T& value) { resize(m_Size + 1); m_Data[m_Size - 1] = value; }
    void        push_range(const T* data, size_t size) { resize(m_Size + size); for (size_t i = 0; i < size; i++) { m_Data[i + m_Size - size] = data[i]; } }
    void        push_range(size_t size, const T& value) { resize(m_Size + size); for (size_t i = 0; i < size; i++) { m_Data[i + m_Size - size] = value; } }
    void        pop_back() { if (m_Size == 0) return; destruct_at(&m_Data[m_Size - 1]); resize(m_Size - 1); }

    T*          find(const T& e) { T* begin = m_Data; const T* end = m_Data + m_Size; while (begin < end) { if (*begin == e) break; begin++; } return begin; }
    const T*    find(const T& e) const { T* begin = m_Data; const T* end = m_Data + m_Size; while (begin < end) { if (*begin == e) break; begin++; } return begin; }
    size_t      find_index(const T& e) const { T* begin = m_Data; const T* end = m_Data + m_Size; size_t i = 0; while (begin < end) { if (*begin == e) break; begin++; i++; } return i; }
    bool        contains(const T& e) const { T* begin = m_Data; const T* end = m_Data + m_Size; while (begin < end) { if (*begin == e) return true; begin++; } return false; }
};

// -- LvnString
// ------------------------------------------------------------
// - simple and light weight replacement to std::string
// - used for functions or struct data types that need to use or return stored string types
// - this is meant to be a temporary object on client side, convert LvnString to std::string when possible

class LvnString
{
private:
    char* m_Data;
    size_t m_Size;
    size_t m_Capacity;

public:
    static const size_t npos = -1;

    LvnString();
    ~LvnString();
    LvnString(const char* str);
    LvnString(const char* data, size_t size);
    LvnString(const LvnString& other);
    LvnString& operator=(const LvnString& other);

    char& operator [](size_t index);
    const char& operator [](size_t index) const;

    bool operator ==(const LvnString& other);
    bool operator !=(const LvnString& other);
    bool operator ==(const char* str);
    bool operator !=(const char* str);

    LvnString operator+(const LvnString& other);
    LvnString operator+(const char* str);
    void operator+=(const LvnString& other);
    void operator+=(const char* str);
    void operator+=(const char& ch);

    void append(const char* str);
    void append(const char& ch);
    LvnString substr(size_t index);
    const LvnString substr(size_t index) const;
    LvnString substr(size_t index, size_t len);
    const LvnString substr(size_t index, size_t len) const;

    void insert(const char* it, const char& ch);
    void insert(const char* it, const char* begin, const char* end);
    void insert(const char* it, const char* data, size_t size);
    void insert_index(size_t index, const char& ch);
    void insert_index(size_t index, const char* begin, const char* end);
    void insert_index(size_t index, const char* data, size_t size);

    bool           empty() const { return m_Size == 0; }
    size_t         length() const { return m_Size; }
    size_t         size() const { return m_Size; }
    size_t         memsize() const { return m_Size * sizeof(char); }
    size_t         memcap() const { return m_Capacity * sizeof(char); }
    const char*    c_str() const { return m_Data; }
    char*          data() { return m_Data; }
    const char*    data() const { return m_Data; }

    char&          front() { LVN_ASSERT(m_Size, "cannot call front on empty string"); return m_Data[0]; }
    const char&    front() const { LVN_ASSERT(m_Size, "cannot call front on empty string"); return m_Data[0]; }
    char&          back() { LVN_ASSERT(m_Size, "cannot call back on empty string"); return m_Data[m_Size - 1]; }
    const char&    back() const { LVN_ASSERT(m_Size, "cannot call back on empty string"); return m_Data[m_Size - 1]; }
    char*          begin() { return m_Data; }
    const char*    begin() const { return m_Data; }
    char*          end() { return m_Data + m_Size; }
    const char*    end() const { return m_Data + m_Size; }

    void           reserve(size_t size);
    void           resize(size_t size);
    void           clear();
    void           clear_free();
    void           erase(const char* it);
    void           erase_index(size_t index);
    void           push_back(const char& ch);
    void           push_range(const char* ch, size_t size);
    void           pop_back();
    size_t         find(const LvnString& other) const;
    size_t         rfind(const LvnString& other) const;
    size_t         find(const char& ch) const;
    size_t         rfind(const char& ch) const;
    size_t         find(const char* str) const;
    size_t         rfind(const char* str) const;
    size_t         find_first_of(const LvnString& other, size_t index = 0) const;
    size_t         find_first_of(const LvnString& other, size_t index, size_t length) const;
    size_t         find_first_of(const char& ch, size_t index = 0) const;
    size_t         find_first_of(const char* str, size_t index = 0) const;
    size_t         find_first_of(const char* str, size_t index, size_t length) const;
    size_t         find_first_not_of(const LvnString& other, size_t index = 0) const;
    size_t         find_first_not_of(const LvnString& other, size_t index, size_t length) const;
    size_t         find_first_not_of(const char& ch, size_t index = 0) const;
    size_t         find_first_not_of(const char* str, size_t index = 0) const;
    size_t         find_first_not_of(const char* str, size_t index, size_t length) const;
    size_t         find_last_of(const LvnString& other, size_t index = LvnString::npos) const;
    size_t         find_last_of(const LvnString& other, size_t index, size_t length) const;
    size_t         find_last_of(const char& ch, size_t index = LvnString::npos) const;
    size_t         find_last_of(const char* str, size_t index = LvnString::npos) const;
    size_t         find_last_of(const char* str, size_t index, size_t length) const;
    size_t         find_last_not_of(const LvnString& other, size_t index = LvnString::npos) const;
    size_t         find_last_not_of(const LvnString& other, size_t index, size_t length) const;
    size_t         find_last_not_of(const char& ch, size_t index = LvnString::npos) const;
    size_t         find_last_not_of(const char* str, size_t index = LvnString::npos) const;
    size_t         find_last_not_of(const char* str, size_t index, size_t length) const;
    bool           starts_with(const char& ch) const;
    bool           ends_with(const char& ch) const;
    bool           contains(const char& ch) const;
};
LvnString operator+(const char* str, const LvnString& other);


// logging
struct LvnLoggerCreateInfo
{
    LvnString loggerName;
    LvnString format;
    LvnLogLevel level;

    struct
    {
        bool enableLogToFile;
        LvnString filename;
        LvnFileMode filemode;
    } fileConfig;
};

struct LvnLogMessage
{
    const char *msg, *loggerName;
    LvnLogLevel level;
    long long timeEpoch;
};

struct LvnLogPattern
{
    char symbol;
    LvnString (*func)(LvnLogMessage*);
};

struct LvnLogFile
{
    LvnString filename;
    LvnFileMode filemode;
    void* fileptr;
    bool logToFile;
};

struct LvnLoggingContextCreateInfo
{
    bool enableGraphicsApiDebugLogs;        // enable debug output for graphics api calls (eg. vulkan validation layer, opengl debug callbacks)

    struct
    {
        bool disableCoreLogging;            // whether to disable core logging in the library
        LvnLogLevel level;                  // set the log level of the core logger
        LvnString logPattern;               // set the log pattern of the core logger
        LvnString name;                     // set the name of the core logger
    } core;

    struct
    {
        LvnLogLevel level;                  // set the log level of the client logger
        LvnString logPattern;               // set the log pattern of the client logger
        LvnString name;                     // set the name of the client logger
    } client;
};

#endif /* !HG_LEVIKNO_H */
