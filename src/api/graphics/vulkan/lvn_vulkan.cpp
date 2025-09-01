#include "lvn_vulkan.h"

#ifdef LVN_INCLUDE_GLSLANG_SRC_COMPILE_SUPPORT
#include <glslang/Include/glslang_c_interface.h>
#endif

#ifdef LVN_ENABLE_ASSERTS
    #define VMA_ASSERT(expr) (static_cast<bool>(expr) ? void(0) : lvn::logCoreError("[VMA] " #expr))
#endif

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#ifdef LVN_ENABLE_ASSERTS
    #define LVN_CALL_ASSERT(x) LVN_ASSERT(x)
#else
    #define LVN_CALL_ASSERT(x) (x)
#endif

static const char* s_ValidationLayers[] =
{
    "VK_LAYER_KHRONOS_validation"
};

static const char* s_DeviceExtensions[] =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
};

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))

namespace lvn
{
static VulkanBackend* s_VkBackend = nullptr;

}
