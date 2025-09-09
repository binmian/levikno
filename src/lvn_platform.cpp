#include "levikno.h"
#include "levikno_internal.h"

#include <cstring>

#if defined(LVN_PLATFORM_LINUX) || defined(LVN_PLATFORM_WINDOWS) || defined(LVN_PLATFORM_MACOS)
#   include <cstdio>
#   include <cstdlib>
#endif


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

void* memCopy(void* dst, const void* src, size_t size)
{
    return memcpy(dst, src, size);
}

void* memSet(void* ptr, int c, size_t size)
{
    return memset(ptr, c, size);
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

// -- logging output
int logOutputMessage(const char* logmsg)
{
    return printf("%s", logmsg);
}

} /* namespace lvn */
