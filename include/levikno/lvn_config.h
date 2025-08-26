#ifndef HG_LVN_CONFIG_H
#define HG_LVN_CONFIG_H

#include <cassert> // assert


enum LvnPlatform
{
    Lvn_Platform_Linux,
    Lvn_Platform_Windows,
    Lvn_Platform_MacOS,
};

// platform
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    inline constexpr LvnPlatform ic_LvnPlatform = Lvn_Platform_Windows;

#elif __APPLE__
    inline constexpr LvnPlatform ic_LvnPlatform = Lvn_Platform_MacOS;

#elif __linux__
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
    inline constexpr bool ic_LvnDebug = true;
#elif !defined(NDEBUG)
    inline constexpr bool ic_LvnDebug = true;
#else
    inline constexpr bool ic_LvnDebug = false;
#endif

// assert
#ifdef LVN_CONFIG_DEBUG
    #define LVN_ENABLE_ASSERTS
#endif

#if defined (LVN_ENABLE_ASSERTS)
    #define LVN_ASSERT(x,msg) assert(x && msg)
#else
    #define LVN_ASSERT(x, ...)
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

inline constexpr const char* ic_LvnLogColorTrace = "\x1b[0;37m";
inline constexpr const char* ic_LvnLogColorDebug = "\x1b[0;34m";
inline constexpr const char* ic_LvnLogColorInfo  = "\x1b[0;32m";
inline constexpr const char* ic_LvnLogColorWarn  = "\x1b[1;33m";
inline constexpr const char* ic_LvnLogColorError = "\x1b[1;31m";
inline constexpr const char* ic_LvnLogColorFatal = "\x1b[1;37;41m";
inline constexpr const char* ic_LvnLogColorReset = "\x1b[0m";

#ifndef LVN_DISABLE_LOGGING
    inline constexpr bool ic_LvnEnableLogging = true;
#else
    inline constexpr bool ic_LvnEnableLogging = false;
#endif


#endif /* !HG_LVN_CONFIG_H */
