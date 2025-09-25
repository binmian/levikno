#include "lvn_platform.h"
#include "levikno.h"
#include "levikno_internal.h"
#include "lvn_graphics_internal.h"

#include <cstring>

#if defined(LVN_PLATFORM_LINUX) || defined(LVN_PLATFORM_WINDOWS) || defined(LVN_PLATFORM_MACOS)
#   include <cstdio>
#   include <cstdlib>
#endif

#ifdef LVN_INCLUDE_GLFW
#   include "lvn_impl_glfw.h"
#endif

#ifdef LVN_INCLUDE_VULKAN
#   include "lvn_impl_vulkan.h"
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

LvnResult initWindowApiFuncs(LvnGraphicsContext* ctx)
{
    LvnResult result = Lvn_Result_Failure;
    switch (ctx->windowapi)
    {
        case Lvn_WindowApi_None:
        {
            LVN_CORE_TRACE("no window api selected, window related functions will not be set");
            return Lvn_Result_Success;
        }
        case Lvn_WindowApi_Glfw:
        {
#ifdef LVN_INCLUDE_GLFW
            result = lvn::implGlfwInitWindowContext(ctx);
#endif
            break;
        }

        default:
        {
            LVN_CORE_ERROR("unrecognized window api: (%d), cannot create window api related functions", ctx->windowapi);
            return Lvn_Result_Failure;
        }
    }

    if (result != Lvn_Result_Success)
    {
        LVN_CORE_ERROR("could not create window api related functions for: %s", lvn::getWindowApiNameEnum(ctx->windowapi));
        return Lvn_Result_Failure;
    }

    LVN_CORE_TRACE("window api set: %s", lvn::getWindowApiNameEnum(ctx->windowapi));
    return result;
}

void terminateWindowApiFuncs(LvnGraphicsContext* ctx)
{
    switch (ctx->windowapi)
    {
        case Lvn_WindowApi_None: { break; }
        case Lvn_WindowApi_Glfw:
        {
#ifdef LVN_INCLUDE_GLFW
            lvn::implGlfwTerminateWindowContext();
#endif
            break;
        }

        default:
        {
            LVN_CORE_ERROR("unrecognized window api: (%d), cannot terminate window api related functions", ctx->windowapi);
            return;
        }
    }

    LVN_CORE_TRACE("window api terminated: %s", lvn::getWindowApiNameEnum(ctx->windowapi));
}

LvnResult initGraphicsApiFuncs(LvnGraphicsContext* ctx)
{
    LvnResult result = Lvn_Result_Failure;
    switch (ctx->graphicsapi)
    {
        case Lvn_GraphicsApi_None:
        {
            LVN_CORE_TRACE("no graphics api selected, graphics related functions will not be set");
            return Lvn_Result_Success;
        }
        case Lvn_GraphicsApi_opengl:
        {
            // TODO: add opengl impl
            result = Lvn_Result_Success;
            break;
        }
        case Lvn_GraphicsApi_vulkan:
        {
#ifdef LVN_INCLUDE_VULKAN
            result = lvn::implVkInitGraphicsContext(ctx);
#endif
            break;
        }

        default:
        {
            LVN_CORE_ERROR("unrecognized graphics api: (%d), cannot create graphics api related functions", ctx->graphicsapi);
            return Lvn_Result_Failure;
        }
    }

    if (result != Lvn_Result_Success)
    {
        LVN_CORE_ERROR("could not create graphics api related functions for: %s", lvn::getGraphicsApiNameEnum(ctx->graphicsapi));
        return Lvn_Result_Failure;
    }

    LVN_CORE_TRACE("graphics api set: %s", lvn::getGraphicsApiNameEnum(ctx->graphicsapi));
    return result;
}

void terminateGraphicsApiFuncs(LvnGraphicsContext* ctx)
{
    switch (ctx->graphicsapi)
    {
        case Lvn_GraphicsApi_None: { break; }
        case Lvn_GraphicsApi_opengl:
        {
            break;
        }
        case Lvn_GraphicsApi_vulkan:
        {
#ifdef LVN_INCLUDE_VULKAN
            lvn::implVkTerminateGraphicsContext();
#endif
            break;
        }

        default:
        {
            LVN_CORE_ERROR("unrecognized graphics api: (%d), cannot terminate graphics api related functions", ctx->graphicsapi);
            return;
        }
    }

    LVN_CORE_TRACE("graphics api terminated: %s", lvn::getGraphicsApiNameEnum(ctx->graphicsapi));
}

} /* namespace lvn */
