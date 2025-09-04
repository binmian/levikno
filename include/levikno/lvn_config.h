#ifndef HG_LVN_CONFIG_H
#define HG_LVN_CONFIG_H

#include <cassert> // assert


enum LvnPlatform
{
    Lvn_Platform_Linux,
    Lvn_Platform_Windows,
    Lvn_Platform_MacOS,
};

enum LvnResult : int
{
    Lvn_Result_Success              =  0,
    Lvn_Result_Failure              = -1,
    Lvn_Result_AlreadyCalled        = -2,
    Lvn_Result_MemAllocFailure      = -3,
    Lvn_Result_TimeOut              = -4,
};


// platform
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #ifndef LVN_PLATFORM_WINDOWS
        #define LVN_PLATFORM_WINDOWS
    #endif
    inline constexpr LvnPlatform ic_LvnPlatform = Lvn_Platform_Windows;

#elif __APPLE__
    #ifndef LVN_PLATFORM_MACOS
        #define LVN_PLATFORM_MACOS
    #endif
    inline constexpr LvnPlatform ic_LvnPlatform = Lvn_Platform_MacOS;

#elif __linux__
    #ifndef LVN_PLATFORM_LINUX
        #define LVN_PLATFORM_LINUX
    #endif
    inline constexpr LvnPlatform ic_LvnPlatform = Lvn_Platform_Linux;
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

// debug
#if defined(_DEBUG)
    #ifndef LVN_CONFIG_DEBUG
        #define LVN_CONFIG_DEBUG
    #endif
    inline constexpr bool ic_LvnDebug = true;
#elif !defined(NDEBUG)
    #ifndef LVN_CONFIG_DEBUG
        #define LVN_CONFIG_DEBUG
    #endif
    inline constexpr bool ic_LvnDebug = true;
#else
    inline constexpr bool ic_LvnDebug = false;
#endif

// assert
#ifdef LVN_CONFIG_DEBUG
    #define LVN_ENABLE_ASSERTS
#endif

#ifdef LVN_ENABLE_ASSERTS
    #define LVN_ASSERT(x,msg) assert(x && msg)
    inline constexpr bool ic_LvnEnableAsserts = true;
#else
    #define LVN_ASSERT(x, ...)
    inline constexpr bool ic_LvnEnableAsserts = false;
#endif


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

#define LVN_LOG_COLOR_TRACE "\x1b[0;37m";
#define LVN_LOG_COLOR_DEBUG "\x1b[0;34m";
#define LVN_LOG_COLOR_INFO  "\x1b[0;32m";
#define LVN_LOG_COLOR_WARN  "\x1b[1;33m";
#define LVN_LOG_COLOR_ERROR "\x1b[1;31m";
#define LVN_LOG_COLOR_FATAL "\x1b[1;37;41m";
#define LVN_LOG_COLOR_RESET "\x1b[0m";

#ifndef LVN_DISABLE_LOGGING
    inline constexpr bool ic_LvnEnableLogging = true;
    #define LVN_CORE_TRACE(...)                     ::lvn::logMessageTrace(lvn::logGetCoreLogger(), ##__VA_ARGS__)
    #define LVN_CORE_DEBUG(...)                     ::lvn::logMessageDebug(lvn::logGetCoreLogger(), ##__VA_ARGS__)
    #define LVN_CORE_INFO(...)                      ::lvn::logMessageInfo(lvn::logGetCoreLogger(), ##__VA_ARGS__)
    #define LVN_CORE_WARN(...)                      ::lvn::logMessageWarn(lvn::logGetCoreLogger(), ##__VA_ARGS__)
    #define LVN_CORE_ERROR(...)                     ::lvn::logMessageError(lvn::logGetCoreLogger(), ##__VA_ARGS__)
    #define LVN_CORE_FATAL(...)                     ::lvn::logMessageFatal(lvn::logGetCoreLogger(), ##__VA_ARGS__)
    #define LVN_TRACE(...)                          ::lvn::logMessageTrace(lvn::logGetClientLogger(), ##__VA_ARGS__)
    #define LVN_DEBUG(...)                          ::lvn::logMessageDebug(lvn::logGetClientLogger(), ##__VA_ARGS__)
    #define LVN_INFO(...)                           ::lvn::logMessageInfo(lvn::logGetClientLogger(), ##__VA_ARGS__)
    #define LVN_WARN(...)                           ::lvn::logMessageWarn(lvn::logGetClientLogger(), ##__VA_ARGS__)
    #define LVN_ERROR(...)                          ::lvn::logMessageError(lvn::logGetClientLogger(), ##__VA_ARGS__)
    #define LVN_FATAL(...)                          ::lvn::logMessageFatal(lvn::logGetClientLogger(), ##__VA_ARGS__)
#else
    inline constexpr bool ic_LvnEnableLogging = false;
    #define LVN_CORE_TRACE(...)
    #define LVN_CORE_DEBUG(...)
    #define LVN_CORE_INFO(...)
    #define LVN_CORE_WARN(...)
    #define LVN_CORE_ERROR(...)
    #define LVN_CORE_FATAL(...)
    #define LVN_TRACE(...)
    #define LVN_DEBUG(...)
    #define LVN_INFO(...)
    #define LVN_WARN(...)
    #define LVN_ERROR(...)
    #define LVN_FATAL(...)
#endif


#endif /* !HG_LVN_CONFIG_H */
