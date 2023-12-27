#ifndef HG_LEVIKNO_H
#define HG_LEVIKNO_H

#ifndef HG_LVN_DEFINE_CONFIG
#define HG_LVN_DEFINE_CONFIG

// Platform
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) 
	#ifndef LVN_PLATFORM_WINDOWS
		#define LVN_PLATFORM_WINDOWS
	#endif
	#ifdef LVN_PLATFORM_WINDOWS
		#ifdef LVN_SHARED_LIBRARY
			#define LVN_API __declspec(dllexport)
			#define LVN_API_IMPORT __declspec(dllimport)
		#else 
			#define LVN_API
		#endif
	#endif

#elif __APPLE__
	#define LVN_PLATFORM_APPLE

#elif __linux__
	#define LVN_PLATFORM_LINUX

#else
	#error "lvn does not support the current platform."
#endif

// Compiler
#ifdef _MSC_VER
	#define LVN_ASSERT_BREAK __debugbreak()
	#pragma warning (disable : 4267)
	#pragma warning (disable : 4244)
	#pragma warning (disable : 26495)
#else
	#define LVN_ASSERT_BREAK assert(false);
#endif


// Debug
#ifdef LVN_DEBUG
	#define LVN_ENABLE_ASSERTS 
#endif

#ifdef LVN_DISABLE_ASSERTS_KEEP_ERROR_MESSAGES
	#define LVN_DISABLE_ASSERTS
#endif

#ifdef LVN_ENABLE_ASSERTS
	#define LVN_ASSERT(x, ...) { if(!(x)) { LVN_ERROR(__VA_ARGS__); LVN_ASSERT_BREAK; } }
	#define LVN_CORE_ASSERT(x, ...) { if(!(x)) { LVN_CORE_ERROR(__VA_ARGS__); LVN_ASSERT_BREAK; } }
#elif LVN_DISABLE_ASSERTS
	#define LVN_ASSERT(x, ...) { if(!(x)) { LVN_ERROR("ERROR: {0}", __VA_ARGS__); } }
	#define LVN_CORE_ASSERT(x, ...) { if(!(x)) { LVN_CORE_ERROR("ERROR: {0}", __VA_ARGS__); } }
#else
	#define LVN_ASSERT(x, ...)
	#define LVN_CORE_ASSERT(x, ...)
#endif

// Warnings
#ifdef LVN_DEBUG

#endif


#define LVN_TRUE 1
#define LVN_FALSE 0
#define LVN_NULL_HANDLE nullptr

#define LVN_UINT8_MAX  0xff
#define LVN_UINT16_MAX 0xffff
#define LVN_UINT32_MAX 0xffffffff
#define LVN_UINT64_MAX 0xffffffffffffffff

#define LVN_ALIGN(x) alignas(x)
#define LVN_SCAST(type, x) static_cast<type>(x)

#define LVN_MALLOC(type, size) static_cast<type>(malloc(size))
#define LVN_FREE(mem) free(mem)

#define LVN_FILE_NAME __FILE__
#define LVN_LINE __LINE__
#define LVN_FUNC_NAME __func__

#define LVN_STR(x) #x
#define LVN_STRIGIFY(x) LVN_STR(x)


#endif // !HG_LVN_DEFINE_CONFIG

#include <stdint.h>

#endif