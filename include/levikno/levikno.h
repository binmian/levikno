#ifndef HG_LEVIKNO_H
#define HG_LEVIKNO_H

// [LAYOUT]:
// ------------------------------------------------------------
//
// [SECTION]: Includes
// [SECTION]: Enums
// [SECTION]: Struct Declaration
// [SECTION]: Functions
// [SECTION]: Struct Implementation


// [SECTION]: Includes
// ------------------------------------------------------------

#include "lvn_config.h"

#include <cstdint> // uint8_t, uint16_t, uint32_t, uint64_t
#include <cstddef> // size_t
#include <new>     // operator new
#include <type_traits>


// [SECTION]: Enums
// ------------------------------------------------------------

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

enum LvnStructureType
{
    Lvn_Stype_Undefined = 0,
    Lvn_Stype_Logger,
    Lvn_Stype_Window,
    Lvn_Stype_FrameBuffer,
    Lvn_Stype_Shader,
    Lvn_Stype_DescriptorLayout,
    Lvn_Stype_Pipeline,
    Lvn_Stype_CommandPool,
    Lvn_Stype_Buffer,
    Lvn_Stype_Sampler,
    Lvn_Stype_Texture,
    Lvn_Stype_Cubemap,
    Lvn_Stype_Sound,
    Lvn_Stype_Socket,

    Lvn_Stype_Max_Value,
};

// [SECTION]: Struct Declaration
// ------------------------------------------------------------

struct LvnLogger;
struct LvnLoggerCreateInfo;
struct LvnContext;
struct LvnContextCreateInfo;
struct LvnLogMessage;
struct LvnLogPattern;
struct LvnSink;


// classes & custom data structures
template <typename T>
class LvnVector;

class LvnString;

template <typename T>
struct LvnLinkedIndexNode;

template <typename T>
using LvnINode = LvnLinkedIndexNode<T>;

template <typename T>
class LvnArenaList;

template <typename T, typename Container>
class LvnQueue;

struct LvnHash;
template <typename K, typename T>
struct LvnHashEntry;
template <typename K, typename T, typename Hash>
class LvnHashMap;

template <typename T>
class LvnUniquePtr;

template<typename T>
struct LvnPair;

template<typename T1, typename T2>
struct LvnDoublePair;

class LvnThread;
class LvnMutex;
class LvnLockGaurd;

// callbacks
typedef void* (*LvnMemAllocFunc)(size_t sz, void* userData);
typedef void  (*LvnMemFreeFunc)(void* ptr, void* userData);
typedef void* (*LvnMemReallocFunc)(void* ptr, size_t sz, void* userData);


// [SECTION]: Functions
// ------------------------------------------------------------

namespace lvn
{
    // -- memory allocation
    LVN_API void*                   memAlloc(size_t size);                              // custom memory allocation function that allocates memory given the size of memory, note that function is connected with the context and will keep track of allocation counts, will increment number of allocations per use
    LVN_API void                    memFree(void* ptr);                                 // custom memory free function, note that it keeps track of memory allocations remaining, decrements number of allocations per use with lvn::memAlloc
    LVN_API void*                   memRealloc(void* ptr, size_t size);                 // custom memory realloc function
    LVN_API void*                   memCopy(void* dst, const void* src, size_t size);   // custom memory copy function
    LVN_API void*                   memSet(void* ptr, int c, size_t size);              // custom memory set function

    LVN_API void                    setMemFuncs(LvnMemAllocFunc allocFunc, LvnMemFreeFunc freeFunc, LvnMemReallocFunc reallocFunc, void* userData);
    LVN_API LvnMemAllocFunc         getMemAllocFunc();
    LVN_API LvnMemFreeFunc          getMemFreeFunc();
    LVN_API LvnMemReallocFunc       getMemReallocFunc();
    LVN_API void*                   getMemUserData();
    LVN_API size_t                  getMemAllocCount();

    LVN_API LvnString               fileLoadSrc(const char* filepath);
    LVN_API LvnVector<uint8_t>      fileLoadBin(const char* filepath);

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

    template <typename T, typename... Args>
    LVN_API constexpr LvnUniquePtr<T> makeUniquePtr(Args&&... args)
    {
        T* ptr = lvn::memNew<T>(1, false);
        new (ptr) T(args...);
        return LvnUniquePtr<T>(ptr);
    }

    // context
    LVN_API LvnResult               initContext(LvnContextCreateInfo* createInfo = nullptr);
    LVN_API void                    terminateContext();
    LVN_API LvnContext*             getContext();


    LVN_API int                     dateGetYear();                                      // get the year number (eg. 2025)
    LVN_API int                     dateGetYear02d();                                   // get the last two digits of the year number (eg. 25)
    LVN_API int                     dateGetMonth();                                     // get the month number (1...12)
    LVN_API int                     dateGetDay();                                       // get the date number (1...31)
    LVN_API int                     dateGetHour();                                      // get the hour of the current day in 24 hour format (0...24)
    LVN_API int                     dateGetHour12();                                    // get the hour of the current day in 12 hour format (0...12)
    LVN_API int                     dateGetMinute();                                    // get the minute of the current day (0...60)
    LVN_API int                     dateGetSecond();                                    // get the second of the current dat (0...60)
    LVN_API size_t                  dateGetSecondsSinceEpoch();                         // get the time in seconds since 00::00:00 UTC 1 January 1970

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
    LVN_API void                    logEnable(bool enable);                                                           // enable or disable logging
    LVN_API void                    logEnableCoreLogging(bool enable);                                                // enable or disable logging from the core logger
    LVN_API void                    logSetLevel(LvnLogger* logger, LvnLogLevel level);                                // sets the log level of logger, will only print messages with set log level and higher
    LVN_API bool                    logCheckLevel(LvnLogger* logger, LvnLogLevel level);                              // checks level with loger, returns true if level is the same or higher level than the level of the logger
    LVN_API void                    logRenameLogger(LvnLogger* logger, const char* name);                             // renames the name of the logger
    LVN_API LvnString               logGetMessage(LvnLogger* logger, LvnLogMessage* msg);                             // gets the message of the logger
    LVN_API LvnString               logFormatMessage(LvnLogger* logger, LvnLogLevel level, const char* msg, bool removeANSI = false); // formats the log message into the log pattern set by the logger
    LVN_API void                    logMessage(LvnLogger* logger, LvnLogLevel level, const char* msg);                // log message with given log level
    LVN_API void                    logMessageTrace(LvnLogger* logger, const char* fmt, ...);                         // log message with level trace; ANSI code "\x1b[0;37m"
    LVN_API void                    logMessageDebug(LvnLogger* logger, const char* fmt, ...);                         // log message with level debug; ANSI code "\x1b[0;34m"
    LVN_API void                    logMessageInfo(LvnLogger* logger, const char* fmt, ...);                          // log message with level info;  ANSI code "\x1b[0;32m"
    LVN_API void                    logMessageWarn(LvnLogger* logger, const char* fmt, ...);                          // log message with level warn;  ANSI code "\x1b[1;33m"
    LVN_API void                    logMessageError(LvnLogger* logger, const char* fmt, ...);                         // log message with level error; ANSI code "\x1b[1;31m"
    LVN_API void                    logMessageFatal(LvnLogger* logger, const char* fmt, ...);                         // log message with level fatal; ANSI code "\x1b[1;37;41m"
    LVN_API void                    logTrace(const char* fmt, ...);                                                   // log trace message on the clint logger
    LVN_API void                    logDebug(const char* fmt, ...);                                                   // log debug message on the clint logger
    LVN_API void                    logInfo(const char* fmt, ...);                                                    // log info message on the clint logger
    LVN_API void                    logWarn(const char* fmt, ...);                                                    // log warn message on the clint logger
    LVN_API void                    logError(const char* fmt, ...);                                                   // log error message on the clint logger
    LVN_API void                    logFatal(const char* fmt, ...);                                                   // log fatal message on the clint logger
    LVN_API void                    logCoreTrace(const char* fmt, ...);                                               // log trace message on the core logger
    LVN_API void                    logCoreDebug(const char* fmt, ...);                                               // log debug message on the core logger
    LVN_API void                    logCoreInfo(const char* fmt, ...);                                                // log info message on the core logger
    LVN_API void                    logCoreWarn(const char* fmt, ...);                                                // log warn message on the core logger
    LVN_API void                    logCoreError(const char* fmt, ...);                                               // log error message on the core logger
    LVN_API void                    logCoreFatal(const char* fmt, ...);                                               // log fatal message on the core logger
    LVN_API LvnLogger*              logGetCoreLogger();
    LVN_API LvnLogger*              logGetClientLogger();
    LVN_API const char*             logGetANSIcodeColor(LvnLogLevel level);                                           // get the ANSI color code of the log level in a string
    LVN_API int                     logOutputMessage(const char* logmsg);                                             // log messages to output
    LVN_API LvnResult               logSetPatternFormat(LvnLogger* logger, const char* patternfmt);                   // set the log pattern of the logger; messages outputed from that logger will be in this format
    LVN_API LvnResult               logAddPatterns(LvnLogPattern* pLogPatterns, uint32_t count);                      // add user defined log patterns to the library

    LVN_API LvnResult               createLogger(LvnLogger** logger, const LvnLoggerCreateInfo* loggerCreateInfo);
    LVN_API void                    destroyLogger(LvnLogger* logger);
    LVN_API LvnLoggerCreateInfo     configLoggerInit(const char* loggerName, const char* logFormat, LvnLogLevel logLevel, LvnSink* pSinks, uint32_t sinkCount);
    LVN_API void                    loggerAddSink(LvnLogger* logger, const LvnSink& sink);
    LVN_API void                    loggerRemoveSink(LvnLogger* logger, uint32_t id);
    LVN_API void                    loggerGetSinks(LvnLogger* logger, LvnSink** pSinks, uint32_t* sinkCount);
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
    LvnString(LvnString&& other);
    LvnString& operator=(LvnString&& other);

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

// -- LvnLinkedIndexNode, LvnArenaList
// ------------------------------------------------------------
// - simple and light weight replacement to std::list
// - arena list is designed to be more cache effecient by using indexed nodes to an allocated array instead of allocated memory per node
// - inserting into LvnArenaList has at worst O(n) linear time complexity

// LvnLinkedIndexNode
template <typename T>
struct LvnLinkedIndexNode
{
    size_t next;
    size_t prev;
    bool hasPrev, hasNext, taken;
    T value;

    T* operator->() { return &value; }
};

// LvnArenaList
template <typename T>
class LvnArenaList
{
private:
    LvnINode<T>* m_Nodes;            /* pointer to an array of nodes */
    size_t m_Size;                   /* the number of the currently alive nodes in the list */
    size_t m_Capacity;               /* the number of nodes allocated for the m_Nodes array */
    size_t* m_FreeNodes;             /* pointer to an array of indices for nodes that are not taken in the array */
    size_t m_FreeSize;               /* the number of indices for nodes not taken */
    size_t m_FreeCapacity;           /* the number of indices allocated in the m_FreeNodes array; NOTE: m_FreeCapacity should always be the same value as m_Capacity */
    size_t m_Head;                   /* the index to the head of the list in the array */
    size_t m_Tail;                   /* the index to the tail of the list in the array */

    void destruct()
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            for (size_t i = 0; i < m_Capacity; i++)
            {
                if (m_Nodes[i].taken)
                    m_Nodes[i].value.~T();
            }
        }
    }

    void destruct_at(LvnINode<T>& node)
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
            node.value.~T();
        node.next = 0;
        node.prev = 0;
        node.hasPrev = false;
        node.hasNext = false;
        node.taken = false;
    }

public:
    LvnArenaList() : m_Nodes(nullptr), m_FreeNodes(nullptr), m_Size(0), m_Capacity(0), m_FreeSize(0), m_FreeCapacity(0), m_Head(0), m_Tail(0) {}
    ~LvnArenaList()
    {
        destruct();
        lvn::memDelete<LvnINode<T>>(m_Nodes, 0);
        lvn::memDelete<size_t>(m_FreeNodes, 0);
        m_Size = m_Capacity = m_FreeSize = m_FreeCapacity = m_Head = m_Tail = 0;
        m_Nodes = nullptr;
        m_FreeNodes = nullptr;
    }

    LvnArenaList(const LvnArenaList<T>& other)
    {
        m_Head = other.m_Head;
        m_Tail = other.m_Tail;
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;
        m_FreeSize = other.m_FreeSize;
        m_FreeCapacity = other.m_FreeCapacity;
        m_Nodes = lvn::memNew<LvnINode<T>>(other.m_Capacity, false);
        for (size_t i = 0; i < other.m_Capacity; i++)
            new (&m_Nodes[i]) LvnINode<T>(other.m_Nodes[i]);
        m_FreeNodes = lvn::memNew<size_t>(other.m_FreeSize, false);
        for (size_t i = 0; i < other.m_FreeSize; i++)
            new (&m_FreeNodes[i]) size_t(other.m_FreeNodes[i]);
    }
    LvnArenaList(LvnArenaList<T>&& other)
    {
        m_Nodes = other.m_Nodes;
        m_FreeNodes = other.m_FreeNodes;
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;
        m_FreeSize = other.m_FreeSize;
        m_FreeCapacity = other.m_FreeCapacity;
        m_Head = other.m_Head;
        m_Tail = other.m_Tail;
        other.m_Nodes = nullptr;
        other.m_FreeNodes = nullptr;
        other.m_Size = 0;
        other.m_Capacity = 0;
        other.m_FreeSize = 0;
        other.m_FreeCapacity = 0;
        other.m_Head = 0;
        other.m_Tail = 0;
    }
    LvnArenaList& operator=(const LvnArenaList<T>& other)
    {
        if (this == &other) return *this;
        destruct();
        lvn::memDelete<LvnINode<T>>(m_Nodes, 0);
        lvn::memDelete<size_t>(m_FreeNodes, 0);
        m_Head = other.m_Head;
        m_Tail = other.m_Tail;
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;
        m_FreeSize = other.m_FreeSize;
        m_FreeCapacity = other.m_FreeCapacity;
        m_Nodes = lvn::memNew<LvnINode<T>>(other.m_Capacity, false);
        for (size_t i = 0; i < other.m_Capacity; i++)
            new (&m_Nodes[i]) LvnINode<T>(other.m_Nodes[i]);
        m_FreeNodes = lvn::memNew<size_t>(other.m_FreeSize, false);
        for (size_t i = 0; i < other.m_FreeSize; i++)
            new (&m_FreeNodes[i]) size_t(other.m_FreeNodes[i]);
        return *this;
    }
    LvnArenaList& operator=(LvnArenaList<T>&& other)
    {
        destruct();
        lvn::memDelete<LvnINode<T>>(m_Nodes, 0);
        lvn::memDelete<size_t>(m_FreeNodes, 0);
        m_Nodes = other.m_Nodes;
        m_FreeNodes = other.m_FreeNodes;
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;
        m_FreeSize = other.m_FreeSize;
        m_FreeCapacity = other.m_FreeCapacity;
        m_Head = other.m_Head;
        m_Tail = other.m_Tail;
        other.m_Nodes = nullptr;
        other.m_FreeNodes = nullptr;
        other.m_Size = 0;
        other.m_Capacity = 0;
        other.m_FreeSize = 0;
        other.m_FreeCapacity = 0;
        other.m_Head = 0;
        other.m_Tail = 0;
        return *this;
    }

    T& operator [](size_t index)
    {
        return at_index(index);
    }
    const T& operator [](size_t index) const
    {
        return at_index(index);
    }

    T& at_index(size_t index)
    {
        LVN_ASSERT(index < m_Size, "list index out of range");
        size_t nodeIndex = m_Head;
        for (size_t i = 0; i < index; i++)
        {
            if (m_Nodes[nodeIndex].hasNext)
                nodeIndex = m_Nodes[nodeIndex].next;
        }
        return m_Nodes[nodeIndex].value;
    }
    const T& at_index(size_t index) const
    {
        LVN_ASSERT(index < m_Size, "list index out of range");
        size_t nodeIndex = m_Head;
        for (size_t i = 0; i < index; i++)
        {
            if (m_Nodes[nodeIndex].hasNext)
                nodeIndex = m_Nodes[nodeIndex].next;
        }
        return m_Nodes[nodeIndex].value;
    }

    LvnINode<T>& node_index(size_t index, size_t* pIndex = nullptr)
    {
        LVN_ASSERT(index < m_Size, "list index out of range");
        size_t nodeIndex = m_Head;
        for (size_t i = 0; i < index; i++)
        {
            if (m_Nodes[nodeIndex].hasNext)
                nodeIndex = m_Nodes[nodeIndex].next;
        }

        if (pIndex) *pIndex = nodeIndex;
        return m_Nodes[nodeIndex];
    }
    const LvnINode<T>& node_index(size_t index, size_t* pIndex = nullptr) const
    {
        LVN_ASSERT(index < m_Size, "list index out of range");
        size_t nodeIndex = m_Head;
        for (size_t i = 0; i < index; i++)
        {
            if (m_Nodes[nodeIndex].hasNext)
                nodeIndex = m_Nodes[nodeIndex].next;
        }

        if (pIndex) *pIndex = nodeIndex;
        return m_Nodes[nodeIndex];
    }

    void erase_index(const size_t index)
    {
        LVN_ASSERT(index < m_Size, "list index out of range");

        if (index == 0) { pop_front(); return; }
        else if (index == m_Size - 1) { pop_back(); return; }

        size_t nodeIndex;
        LvnINode<T>& node = node_index(index, &nodeIndex);
        if (node.hasNext)
        {
            LvnINode<T>& next = m_Nodes[node.next];
            next.prev = node.prev;
        }
        if (node.hasPrev)
        {
            LvnINode<T>& prev = m_Nodes[node.prev];
            prev.next = node.next;
        }

        /* push back free node */
        LVN_ASSERT(m_FreeSize < m_FreeCapacity, "free nodes array is full");
        m_FreeNodes[m_FreeSize] = nodeIndex;
        m_FreeSize++;

        destruct_at(node);
        m_Size--;
    }
    void insert_index(const size_t index, const T& value)
    {
        LVN_ASSERT(index <= m_Size, "list index out of range");

        if (index == 0) { push_front(value); return; }
        if (index == m_Size) { push_back(value); return; }

        if (m_Size >= m_Capacity)
            reserve(m_Size + 1);

        size_t currentNodeIndex;
        LvnINode<T>& node = node_index(index, &currentNodeIndex);

        /* check if there are indices to free nodes */
        if (m_FreeSize != 0)
        {
            size_t nodeIndex = m_FreeNodes[m_FreeSize - 1];
            LvnINode<T>& newNode = m_Nodes[nodeIndex];
            newNode = node;
            newNode.prev = currentNodeIndex;
            newNode.hasPrev = true;
            newNode.taken = true;

            if (index == m_Size - 1) /* if insert is on last index, move tail to next */
                m_Tail = nodeIndex;

            node.value = value;
            node.next = nodeIndex;
            node.hasNext = true;
            m_Size++;
            m_FreeSize--;
            return;
        }

        /* find an index linearly */
        for (size_t i = 0; i < m_Capacity; i++)
        {
            if (!m_Nodes[i].taken)
            {
                if (node.hasNext)
                    m_Nodes[node.next].prev = i;

                LvnINode<T>& newNode = m_Nodes[i];
                newNode = node;
                newNode.prev = currentNodeIndex;
                newNode.hasPrev = true;
                newNode.taken = true;

                if (index == m_Size - 1) /* if insert is on last index, move tail to next */
                    m_Tail = i;

                node.value = value;
                node.next = i;
                node.hasNext = true;
                m_Size++;
                return;
            }
        }

        LVN_ASSERT(false, "could not find empty node to insert index");
    }
    void push_back(const T& data)
    {
        if (!m_Size)
        {
            reserve(m_Size + 1);
            m_Head = m_Tail = 0;
            m_Nodes[m_Head].value = data;
            m_Nodes[m_Head].taken = true;
            m_Size++;
            return;
        }

        if (m_Size >= m_Capacity)
            reserve(m_Size + 1);

        LvnINode<T>& node = m_Nodes[m_Tail];

        /* check if there are indices to free nodes */
        if (m_FreeSize != 0)
        {
            size_t nodeIndex = m_FreeNodes[m_FreeSize - 1];
            LvnINode<T>& newNode = m_Nodes[nodeIndex];
            newNode.value = data;
            newNode.prev = m_Tail;
            newNode.hasPrev = true;
            newNode.taken = true;

            node.next = nodeIndex;
            node.hasNext = true;
            m_Tail = nodeIndex;
            m_Size++;
            m_FreeSize--;
            return;
        }

        /* find an index linearly */
        for (size_t i = 0; i < m_Capacity; i++)
        {
            if (!m_Nodes[i].taken)
            {
                LvnINode<T>& newNode = m_Nodes[i];
                newNode.value = data;
                newNode.prev = m_Tail;
                newNode.hasPrev = true;
                newNode.taken = true;

                node.next = i;
                node.hasNext = true;
                m_Tail = i;
                m_Size++;
                return;
            }
        }

        LVN_ASSERT(false, "could not find empty node to push back element");
    }
    void push_front(const T& data)
    {
        if (!m_Size)
        {
            reserve(m_Size + 1);
            m_Head = m_Tail = 0;
            m_Nodes[m_Head].value = data;
            m_Nodes[m_Head].taken = true;
            m_Size++;
            return;
        }

        if (m_Size >= m_Capacity)
            reserve(m_Size + 1);

        LvnINode<T>& node = m_Nodes[m_Head];

        /* check if there are indices to free nodes */
        if (m_FreeSize != 0)
        {
            size_t nodeIndex = m_FreeNodes[m_FreeSize - 1];
            LvnINode<T>& newNode = m_Nodes[nodeIndex];
            newNode.value = data;
            newNode.next = m_Head;
            newNode.hasNext = true;
            newNode.taken = true;

            node.prev = nodeIndex;
            node.hasPrev = true;
            m_Head = nodeIndex;
            m_Size++;
            m_FreeSize--;
            return;
        }

        /* find an index linearly */
        for (size_t i = 0; i < m_Capacity; i++)
        {
            if (!m_Nodes[i].taken)
            {
                LvnINode<T>& newNode = m_Nodes[i];
                newNode.value = data;
                newNode.next = m_Head;
                newNode.hasNext = true;
                newNode.taken = true;

                node.prev = i;
                node.hasPrev = true;
                m_Head = i;
                m_Size++;
                return;
            }
        }

        LVN_ASSERT(false, "could not find empty node to push front element");
    }
    void pop_back()
    {
        if (!m_Size) { return; }
        if (m_Size == 1) { destruct_at(m_Nodes[m_Head]); m_Tail = m_Head = 0; m_Size--; return; }

        /* push back free node */
        LVN_ASSERT(m_FreeSize < m_FreeCapacity, "free nodes array is full");
        m_FreeNodes[m_FreeSize] = m_Tail;
        m_FreeSize++;

        /* set prev node to tail */
        LvnINode<T>& node = m_Nodes[m_Tail];
        LvnINode<T>& prev = m_Nodes[node.prev];
        m_Tail = node.prev;
        prev.next = 0;
        prev.hasNext = false;

        destruct_at(node);
        m_Size--;
    }
    void pop_front()
    {
        if (!m_Size) { return; }
        if (m_Size == 1) { destruct_at(m_Nodes[m_Head]); m_Tail = m_Head = 0; m_Size--; return; }

        /* push back free node */
        LVN_ASSERT(m_FreeSize < m_FreeCapacity, "free nodes array is full");
        m_FreeNodes[m_FreeSize] = m_Head;
        m_FreeSize++;

        /* set next node to head */
        LvnINode<T>& node = m_Nodes[m_Head];
        LvnINode<T>& next = m_Nodes[node.next];
        m_Head = node.next;
        next.prev = 0;
        next.hasPrev = false;

        destruct_at(node);
        m_Size--;
    }

    /* FIXME: LvnArenaList only constructs nodes currently in list, but checks for uninitialized nodes on new push */
    /*        temp fix is to initialize allocations from memory callbacks to 0 */
    void reserve(size_t size)
    {
        if (size <= m_Capacity) { return; }
        LvnINode<T>* temp = lvn::memNew<LvnINode<T>>(size, false);
        for (size_t i = 0; i < m_Capacity; i++)
            new (&temp[i]) LvnINode<T>(m_Nodes[i]);
        destruct();
        lvn::memDelete<LvnINode<T>>(m_Nodes, 0);
        m_Nodes = temp;
        m_Capacity = size;
        size_t* freeTemp = lvn::memNew<size_t>(size, false);
        for (size_t i = 0; i < m_FreeSize; i++)
            new (&freeTemp[i]) size_t(m_FreeNodes[i]);
        lvn::memDelete<size_t>(m_FreeNodes, 0);
        m_FreeNodes = freeTemp;
        m_FreeCapacity = size;
    }

    size_t      size() const { return m_Size; }
    bool        empty() const { return m_Size == 0; }
    void        clear() { destruct(); lvn::memDelete<LvnINode<T>>(m_Nodes, 0); m_Size = 0; for (size_t i = 0; i < m_FreeCapacity; i++) m_FreeNodes[i] = i; m_FreeSize = m_FreeCapacity; m_Head = m_Tail = 0; }
    void        clear_free() { destruct(); lvn::memDelete<LvnINode<T>>(m_Nodes, 0); lvn::memDelete<size_t>(m_FreeNodes, 0); m_Nodes = nullptr; m_FreeNodes = nullptr; m_Head = m_Tail = m_Size = m_Capacity = m_FreeSize = m_FreeCapacity = 0; }

    T&          front() { LVN_ASSERT(m_Size, "cannot call front on empty list"); return m_Nodes[m_Head].value; }
    const T&    front() const { LVN_ASSERT(m_Size, "cannot call front on empty list"); return m_Nodes[m_Head].value; }

    T&          back() { LVN_ASSERT(m_Size, "cannot call back on empty list"); return m_Nodes[m_Tail].value; }
    const T&    back() const { LVN_ASSERT(m_Size, "cannot call back on empty list"); return m_Nodes[m_Tail].value; }
};


// -- LvnQueue
// ------------------------------------------------------------
// - simple and light weight replacement to std::queue
// - LvnQueue is a wrapper around LvnArenaList by default
// - designed to be more cache effecient based on indexed nodes from a singular allocated array

template <typename T, typename Container = LvnArenaList<T>>
class LvnQueue
{
private:
    Container m_Container;

public:
    LvnQueue() = default;
    LvnQueue(const T* data, size_t size)
    {
        m_Container.reserve(size);
        for (size_t i = 0; i < size; i++)
            m_Container.push_back(data[i]);
    }

    size_t      size() const { return m_Container.size(); }
    bool        empty() const { return m_Container.empty(); }
    void        push(const T& value) { m_Container.push_back(value); }
    void        pop() { m_Container.pop_front(); }
    T&          front() { return m_Container.front(); }
    const T&    front() const { return m_Container.front(); }
    T&          back() { return m_Container.back(); }
    const T&    back() const { return m_Container.back(); }
};

// -- LvnHash, LvnHashEntry, LvnHashMap
// ------------------------------------------------------------
// simple and light weight replacement to std::hash, std::unordered_map
// designed to be more cache effecient, all hash entries stored in a single allocated array
// note that LvnHashMap only takes in integral types for the key value

struct LvnHash
{
    /* splitmix64 */
    size_t operator()(size_t k) const
    {
        k += 0x9E3779B97F4A7C15;
        k = (k ^ (k >> 30)) * 0xBF58476D1CE4E5B9;
        k = (k ^ (k >> 27)) * 0x94D049BB133111EB;
        k = k ^ (k >> 31);
        return k;
    }
};

template <typename K, typename T>
struct LvnHashEntry
{
    T data;
    K key;
    size_t nextIndex;
    bool taken, hasNext;
};

template <typename K, typename T, typename Hash = LvnHash>
class LvnHashMap
{
    static_assert(std::is_integral_v<K>, "cannot have non integral type as key");
    using MoveRef = std::remove_reference_t<T>&&;
private:
    LvnHashEntry<K, T>* m_HashEntries;
    size_t m_Size;
    size_t m_Capacity;
    Hash m_Hasher;

    void destruct()
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            for (size_t i = 0; i < m_Capacity; i++)
            {
                if (m_HashEntries[i].taken)
                    m_HashEntries[i].data.~T();
            }
        }
    }
    bool erase_recursive(size_t index)
    {
        if (m_HashEntries[index].hasNext)
        {
            size_t nextIndex = m_HashEntries[index].nextIndex;
            m_HashEntries[index].key = m_HashEntries[nextIndex].key;
            m_HashEntries[index].nextIndex = m_HashEntries[nextIndex].nextIndex;
            m_HashEntries[index].taken = m_HashEntries[nextIndex].taken;
            m_HashEntries[index].hasNext = m_HashEntries[nextIndex].hasNext;
            m_HashEntries[index].data = static_cast<MoveRef>(m_HashEntries[nextIndex].data);
            if (erase_recursive(nextIndex))
            {
                m_HashEntries[index].nextIndex = 0;
                m_HashEntries[index].hasNext = false;
            }
        }
        else /* last entry in chain */
        {
            if (m_HashEntries[index].taken && !std::is_trivially_destructible_v<T>)
                m_HashEntries[index].data.~T();
            m_HashEntries[index].key = 0;
            m_HashEntries[index].nextIndex = 0;
            m_HashEntries[index].taken = false;
            m_HashEntries[index].hasNext = false;
            return true;
        }

        return false;
    }

public:
    LvnHashMap()
        : m_HashEntries(nullptr), m_Size(0), m_Capacity(0) {}
    ~LvnHashMap()
    {
        destruct();
        lvn::memDelete(m_HashEntries, 0);
        m_Size = m_Capacity = 0;
        m_HashEntries = nullptr;
    }

    LvnHashMap(size_t size)
        : m_Size(0)
    {
        reserve(size);
    }

    LvnHashMap(const LvnHashMap& other)
    {
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;
        m_HashEntries = lvn::memNew<LvnHashEntry<K, T>>(m_Capacity, false);
        for (size_t i = 0; i < other.m_Capacity; i++)
        {
            if (other.m_HashEntries[i].taken)
                new (&m_HashEntries[i]) LvnHashEntry<K, T>(other.m_HashEntries[i]);
        }
    }
    LvnHashMap(LvnHashMap&& other)
    {
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;
        m_HashEntries = other.m_HashEntries;
        other.m_Size = 0;
        other.m_Capacity = 0;
        other.m_HashEntries = nullptr;
    }
    LvnHashMap& operator=(const LvnHashMap& other)
    {
        if (this == &other) return *this;
        destruct();
        lvn::memDelete<LvnHashEntry<K, T>>(m_HashEntries, 0);
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;
        m_HashEntries = lvn::memNew<LvnHashEntry<K, T>>(other.m_Capacity, false);
        for (size_t i = 0; i < other.m_Capacity; i++)
        {
            if (other.m_HashEntries[i].taken)
                new (&m_HashEntries[i]) LvnHashEntry<K, T>(other.m_HashEntries[i]);
        }
        return *this;
    }
    LvnHashMap& operator=(LvnHashMap&& other)
    {
        destruct();
        lvn::memDelete<LvnHashEntry<K, T>>(m_HashEntries, 0);
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;
        m_HashEntries = other.m_HashEntries;
        other.m_Size = 0;
        other.m_Capacity = 0;
        other.m_HashEntries = nullptr;
        return *this;
    }

    T& operator[](const K& key)
    {
        return at(key);
    }
    const T& operator[](K key) const
    {
        return at(key);
    }

    /* reserves new memory space and rehashes entries */
    void reserve(size_t size)
    {
        /* step 1: reserve/allocate memory */
        if (size <= m_Size) return;
        LvnHashEntry<K, T>* temp = m_HashEntries;
        size_t tempSize = m_Capacity;
        m_HashEntries = lvn::memNew<LvnHashEntry<K, T>>(size);
        m_Capacity = size;

        /* step 2: rehash and insert entries into new table */
        m_Size = 0;
        for (size_t i = 0; i < tempSize; i++)
        {
            if (temp[i].taken)
                insert(temp[i].key, static_cast<MoveRef>(temp[i].data));
        }
        destruct();
        lvn::memDelete<LvnHashEntry<K, T>>(temp, 0);
    }
    void insert(const K& key, const T& value)
    {
        /* resize/rehash when size exceeds 70% capacity (0.7 load factor) */
        if (m_Size * 10 >= m_Capacity * 7)
            reserve(m_Capacity ? m_Capacity * 2 : 8);

        size_t index = m_Hasher.operator()(key) % m_Capacity;
        if (m_HashEntries[index].taken && m_HashEntries[index].key == key)
        {
            m_HashEntries[index].data = value;
            return;
        }

        /* iterate through entries if key not found */
        LvnHashEntry<K, T>* entry = &m_HashEntries[index];
        while (entry->hasNext)
        {
            index = entry->nextIndex;
            entry = &m_HashEntries[entry->nextIndex];

            if (entry->key == key)
            {
                entry->data = value;
                return;
            }
        }

        /* add a new entry if key still not found */
        LvnHashEntry<K, T>* findEntry = &m_HashEntries[index];
        while (findEntry->taken)
        {
            index = (index + 1) % m_Capacity;
            findEntry = &m_HashEntries[index];
        }

        findEntry->key = key;
        findEntry->data = value;
        findEntry->taken = true;
        m_Size++;

        if (entry->key != findEntry->key)
        {
            entry->nextIndex = index;
            entry->hasNext = true;
        }
    }
    void insert(const K& key, T&& value)
    {
        /* resize/rehash when size exceeds 70% capacity (0.7 load factor) */
        if (m_Size * 10 >= m_Capacity * 7)
            reserve(m_Capacity ? m_Capacity * 2 : 8);

        size_t index = m_Hasher.operator()(key) % m_Capacity;
        if (m_HashEntries[index].taken && m_HashEntries[index].key == key)
        {
            m_HashEntries[index].data = static_cast<MoveRef>(value);
            return;
        }

        /* iterate through entries if key not found */
        LvnHashEntry<K, T>* entry = &m_HashEntries[index];
        while (entry->hasNext)
        {
            index = entry->nextIndex;
            entry = &m_HashEntries[entry->nextIndex];

            if (entry->key == key)
            {
                entry->data = static_cast<MoveRef>(value);
                return;
            }
        }

        /* add a new entry if key still not found */
        LvnHashEntry<K, T>* findEntry = &m_HashEntries[index];
        while (findEntry->taken)
        {
            index = (index + 1) % m_Capacity;
            findEntry = &m_HashEntries[index];
        }

        findEntry->key = key;
        findEntry->data = static_cast<MoveRef>(value);
        findEntry->taken = true;
        m_Size++;

        if (entry->key != findEntry->key)
        {
            entry->nextIndex = index;
            entry->hasNext = true;
        }
    }
    void erase(const K& key)
    {
        if (m_Size == 0) return;

        size_t index = m_Hasher.operator()(key) % m_Capacity;
        if (m_HashEntries[index].key == key)
        {
            erase_recursive(index);
            return;
        }

        LvnHashEntry<K, T>* entry = &m_HashEntries[index];
        while (entry->hasNext)
        {
            index = entry->nextIndex;
            entry = &m_HashEntries[entry->nextIndex];

            if (entry->key == key)
            {
                erase_recursive(index);
                return;
            }
        }
    }
    T& at(const K& key)
    {
        if (m_Size == 0)
            insert(key, T{});

        size_t index = m_Hasher.operator()(key) % m_Capacity;
        if (m_HashEntries[index].taken && m_HashEntries[index].key == key)
            return m_HashEntries[index].data;

        LvnHashEntry<K, T>* entry = &m_HashEntries[index];
        while (entry->hasNext)
        {
            index = entry->nextIndex;
            entry = &m_HashEntries[entry->nextIndex];

            if (entry->key == key)
                return entry->data;
        }

        /* if key not found, create new entry */
        insert(key, T{});
        return at(key);
    }
    const T& at(const K& key) const
    {
        if (m_Size == 0)
            insert(key, T{});

        size_t index = m_Hasher.operator()(key) % m_Capacity;
        if (m_HashEntries[index].taken && m_HashEntries[index].key == key)
            return m_HashEntries[index].data;

        LvnHashEntry<K, T>* entry = &m_HashEntries[index];
        while (entry->hasNext)
        {
            index = entry->nextIndex;
            entry = &m_HashEntries[entry->nextIndex];

            if (entry->key == key)
                return entry->data;
        }

        /* if key not found, create new entry */
        insert(key, T{});
        return at(key);
    }

    bool contains(const K& key)
    {
        if (m_Size == 0) return false;

        size_t index = m_Hasher.operator()(key) % m_Capacity;
        if (key == m_HashEntries[index].key)
            return true;

        LvnHashEntry<K, T>* entry = &m_HashEntries[index];
        while (entry->hasNext)
        {
            index = entry->nextIndex;
            entry = &m_HashEntries[entry->nextIndex];

            if (entry->key == key)
                return true;
        }

        return false;
    }

    bool                   empty() { return m_Size == 0; }
    void                   clear() { if (m_Size) { destruct(); } m_Size = 0; }
    void                   clear_free() { lvn::memDelete<LvnHashEntry<K, T>>(m_HashEntries, m_Capacity); m_Size = m_Capacity = 0; m_HashEntries = nullptr; }
    size_t                 size() { return m_Size; }
    size_t                 capacity() { return m_Capacity; }
    size_t                 memcap() { return m_Capacity * sizeof(LvnHashEntry<K, T>); }
    LvnHashEntry<K, T>*    data() { return m_HashEntries; }
};

// -- LvnUniquePtr
// ------------------------------------------------------------
// - simple and light weight replacement to std::unique_ptr
// - note that pointers given to LvnUniquePtr must be allocated from lvn::memNew()
// - a makeUniquePtr() function is given to properly allocate pointers for LvnUniquePtr

template <typename T>
class LvnUniquePtr
{
private:
    T* m_Ptr;

public:
    LvnUniquePtr() : m_Ptr(nullptr) {}
    ~LvnUniquePtr() { lvn::memDelete<T>(m_Ptr); }

    explicit LvnUniquePtr(T* ptr) : m_Ptr(ptr) {}

    template <typename U, typename = typename std::enable_if_t<std::is_convertible_v<U*, T*>>>
    LvnUniquePtr(LvnUniquePtr<U>&& other) : m_Ptr(other.release()) {}

    LvnUniquePtr(LvnUniquePtr&& other)
    {
        m_Ptr = other.m_Ptr;
        other.m_Ptr = nullptr;
    }
    LvnUniquePtr& operator=(LvnUniquePtr&& other)
    {
        lvn::memDelete<T>(m_Ptr);
        m_Ptr = other.m_Ptr;
        other.m_Ptr = nullptr;
        return *this;
    }

    LvnUniquePtr(const LvnUniquePtr& other) = delete;
    LvnUniquePtr& operator=(const LvnUniquePtr& other) = delete;

    T*          operator->() { return m_Ptr; }
    const T*    operator->() const { return m_Ptr; }

    T&          operator*() { return *m_Ptr; }
    const T&    operator*() const { return *m_Ptr; }

    operator    bool() const { return m_Ptr; }

    T*          get() { return m_Ptr; }
    const T*    get() const { return m_Ptr; }

    T*          release() { T* temp = m_Ptr; m_Ptr = nullptr; return temp; }

    void        reset(T* ptr = nullptr) { if (m_Ptr) { lvn::memDelete<T>(m_Ptr); } m_Ptr = ptr; }
};

template<typename T>
struct LvnPair
{
    T first;
    T second;
};

template<typename T1, typename T2>
struct LvnDoublePair
{
    union { T1 p1, x, width, first; };
    union { T2 p2, y, height, second; };
};

class LvnThread
{
private:
    void* m_Thread;

public:
    LvnThread() = default;
    ~LvnThread();
    LvnThread(void (*funcptr)(void*), void* arg);

    LvnThread(const LvnThread&) = delete;
    LvnThread& operator=(const LvnThread&) = delete;

    LvnThread(LvnThread&& other);
    LvnThread& operator=(LvnThread&& other);

    void join();
    bool joinable();
    uint64_t id();
};

class LvnMutex
{
private:
    void* m_Mutex;

public:
    LvnMutex();
    ~LvnMutex();

    LvnMutex(const LvnMutex&) = delete;
    LvnMutex& operator=(const LvnMutex&) = delete;

    LvnMutex(LvnMutex&& other);
    LvnMutex& operator=(LvnMutex&& other);

    void lock();
    void unlock();
};

class LvnLockGaurd
{
private:
    LvnMutex& m_Mutex;

public:
    LvnLockGaurd(LvnMutex& mutex) : m_Mutex(mutex) { m_Mutex.lock(); }
    ~LvnLockGaurd() { m_Mutex.unlock(); }

    LvnLockGaurd(const LvnMutex&) = delete;
    LvnLockGaurd& operator=(const LvnMutex&) = delete;

    void lock() { m_Mutex.lock(); }
    void unlock() { m_Mutex.unlock(); }
};

// logging
struct LvnSink
{
    int (*logFunc)(const char*);
    uint32_t id;
};

struct LvnLoggerCreateInfo
{
    LvnString loggerName;
    LvnString format;
    LvnLogLevel level;
    LvnSink* pSinks;
    uint32_t sinkCount;
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

struct LvnContextCreateInfo
{
    struct
    {
        struct
        {
            bool disableCoreLogging;            // whether to disable core logging in the library
            bool noOutputSink;                  // dont add a sink for log output for this logger
            LvnLogLevel level;                  // set the log level of the core logger
            LvnString logPattern;               // set the log pattern of the core logger
            LvnString name;                     // set the name of the core logger
        } core;

        struct
        {
            bool noOutputSink;                  // dont add a sink for log output for this logger
            LvnLogLevel level;                  // set the log level of the client logger
            LvnString logPattern;               // set the log pattern of the client logger
            LvnString name;                     // set the name of the client logger
        } client;
    } logging;
};

#endif /* !HG_LEVIKNO_H */
