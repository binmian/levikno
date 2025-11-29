#include "lvn_platform.h"
#include "levikno.h"
#include "levikno_internal.h"

#include <cstring>

#if defined(LVN_PLATFORM_LINUX) || defined(LVN_PLATFORM_WINDOWS) || defined(LVN_PLATFORM_MACOS)
#   include <cstdio>
#   include <cstdlib>
#endif

#ifdef LVN_PLATFORM_LINUX
#   include <dlfcn.h>
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

void* memAlloc(LvnContext* ctx, size_t size)
{
    if (size == 0) { return nullptr; }
    void* allocmem = (*s_MemAllocFunc)(size, s_MemAllocUserData);
    if (!allocmem) { LVN_CORE_ERROR(ctx, "malloc failure, could not allocate memory!"); exit(-1); }
    memset(allocmem, 0, size);
    ctx->memAllocCount++;
    return allocmem;
}

void memFree(LvnContext* ctx, void* ptr)
{
    if (ptr == nullptr) { return; }
    (*s_MemFreeFunc)(ptr, s_MemAllocUserData);
    ctx->memAllocCount--;
}

void* memRealloc(LvnContext* ctx, void* ptr, size_t size)
{
    if (!ptr) { return lvn::memAlloc(ctx, size); }
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

LvnString fileLoadSrc(const char* filepath, LvnResult* result)
{
    FILE* fileptr = fopen(filepath, "r");

    if (!fileptr)
    {
        if (result) { *result = Lvn_Result_Failure; }
        return {};
    }

    fseek(fileptr, 0, SEEK_END);
    long int size = ftell(fileptr);
    fseek(fileptr, 0, SEEK_SET);

    LvnVector<char> src(size);
    fread(src.data(), sizeof(char), size, fileptr);
    fclose(fileptr);

    if (result) { *result = Lvn_Result_Success; }
    return LvnString(src.data(), src.size());
}

LvnVector<uint8_t> fileLoadBin(const char* filepath, LvnResult* result)
{
    FILE* fileptr = fopen(filepath, "rb");

    if (!fileptr)
    {
        if (result) { *result = Lvn_Result_Failure; }
        return {};
    }

    fseek(fileptr, 0, SEEK_END);
    long int size = ftell(fileptr);
    fseek(fileptr, 0, SEEK_SET);

    LvnVector<uint8_t> bin(size);
    fread(bin.data(), sizeof(uint8_t), size, fileptr);
    fclose(fileptr);

    if (result) { *result = Lvn_Result_Success; }
    return lvn::move(bin);
}

// -- logging output
int logOutputMessage(const char* logmsg)
{
    return printf("%s", logmsg);
}


void* platformLoadModule(const char* path)
{
    return dlopen(path, RTLD_LAZY | RTLD_LOCAL);
}

void platformUnloadModule(void* module)
{
    dlclose(module);
}

} /* namespace lvn */
