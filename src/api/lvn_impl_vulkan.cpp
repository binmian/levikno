#include "lvn_impl_vulkan.h"
#include "lvn_impl_vulkan_backends.h"
#include "lvn_graphics_internal.h"

#include <vulkan/vulkan.h>

#ifdef LVN_ENABLE_ASSERTS
    #define VMA_ASSERT(expr) (static_cast<bool>(expr) ? void(0) : LVN_CORE_ERROR("[VMA] " #expr))
#endif

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#ifdef LVN_INCLUDE_WIN32
#   include <vulkan/vulkan_win32.h>
#endif
#ifdef LVN_INCLUDE_COCOA
#   include <vulkan/vulkan_macos.h>
#endif
#ifdef LVN_INCLUDE_WAYLAND
#   include <vulkan/vulkan_wayland.h>
#endif
#ifdef LVN_INCLUDE_X11_OWO
#   include <vulkan/vulkan_xcb.h>
#   include <vulkan/vulkan_xlib.h>
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

static VulkanBackends* s_VkBackends = nullptr;

namespace lvn
{

namespace vks
{
    static VulkanBackends*                      getVulkanBackends();
    static LvnResult                            createVulkanInstace(VulkanBackends* vkBackends, bool enableValidationLayers);
    static VKAPI_ATTR VkBool32 VKAPI_CALL       debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
    static LvnVector<const char*>               getRequiredExtensions(VulkanBackends* vkBackends);
    static VkResult                             createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    static void                                 destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    static void                                 fillVulkanDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo);
    static LvnPhysicalDeviceType                getPhysicalDeviceTypeEnum(VkPhysicalDeviceType type);
    static bool                                 checkValidationLayerSupport();
    static LvnResult                            setupDebugMessenger(VulkanBackends* vkBackends);
    static LvnVector<VkPhysicalDevice>          getPhysicalDevices(VkInstance instance);
    static VkPhysicalDevice                     getBestPhysicalDevice(VkInstance instance, const LvnVector<VkPhysicalDevice>& physicalDevices, VkSurfaceKHR surface = VK_NULL_HANDLE);
    static VulkanQueueFamilyIndices             findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
    static bool                                 checkDeviceExtensionSupport(VkPhysicalDevice device);
    static LvnResult                            createVulkanSurface(VkInstance instance, LvnWindow* window, VkSurfaceKHR* surface);
    static VulkanSwapChainSupportDetails        querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice device);
    static LvnResult                            createLogicalDevice(VulkanBackends* vkBackends, VkSurfaceKHR surface = VK_NULL_HANDLE);
    static LvnResult                            setupRenderInit(VulkanBackends* vkBackends, VkPhysicalDevice physicalDevice = VK_NULL_HANDLE, VkSurfaceKHR surface = VK_NULL_HANDLE);
    static LvnResult                            createVulkanWindowSurfaceData(LvnWindow* window, VkSurfaceKHR surface);
    static void                                 destroyVulkanWindowSurfaceData(LvnWindow* window);
    static VkFormat                             findSupportedFormat(VkPhysicalDevice physicalDevice, const VkFormat* candidates, uint32_t count, VkImageTiling tiling, VkFormatFeatureFlags features);
    static VkFormat                             findDepthFormat(VkPhysicalDevice physicalDevice);
    static bool                                 hasStencilComponent(VkFormat format);
    static LvnResult                            createRenderPass(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData, VkFormat format);
    static VkSurfaceFormatKHR                   chooseSwapSurfaceFormat(VulkanBackends* vkBackends, const VkSurfaceFormatKHR* pAvailableFormats, uint32_t count);
    static VkPresentModeKHR                     chooseSwapPresentMode(const VkPresentModeKHR* pAvailablePresentModes, uint32_t count, bool vSync);
    static VkExtent2D                           chooseSwapExtent(const LvnWindow* window, const VkSurfaceCapabilitiesKHR* capabilities);
    static LvnResult                            createSwapChain(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData, VulkanSwapChainSupportDetails swapChainSupport, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode, VkExtent2D extent);
    static VkImageView                          createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    static void                                 createImageViews(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
    static void                                 createDepthResources(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
    static LvnResult                            createFrameBuffers(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
    static LvnResult                            createSyncObjects(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
    static void                                 cleanSwapChain(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
    static void                                 recreateSwapChain(VulkanBackends* vkBackends, LvnWindow* window);
    static LvnResult                            createBuffer(VulkanBackends* vkBackends, VkBuffer* buffer, VmaAllocation* bufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memUsage);
    static VulkanPipeline                       createVulkanPipeline(VulkanBackends* vkBackends, VulkanPipelineCreateData* createData);
    static LvnResult                            createImage(VulkanBackends* vkBackends, VkImage* image, VmaAllocation* imageMemory, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkSampleCountFlagBits samples, VmaMemoryUsage memUsage);
    static VkShaderModule                       createShaderModule(VulkanBackends* vkBackends, const uint8_t* code, uint32_t size);
    static void                                 transitionImageLayout(VulkanBackends* vkBackends, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount);
    static void                                 copyBuffer(VulkanBackends* vkBackends, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset);
    static void                                 copyBufferToImage(VulkanBackends* vkBackends, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);
    static VkPrimitiveTopology                  getVulkanTopologyTypeEnum(LvnTopologyType topologyType);
    static VkCullModeFlags                      getVulkanCullModeFlagEnum(LvnCullFaceMode cullFaceMode);
    static VkFrontFace                          getVulkanCullFrontFaceEnum(LvnCullFrontFace cullFrontFace);
    static VkFormat                             getVulkanColorFormatEnum(LvnColorImageFormat format);
    static VkFormat                             getVulkanDepthFormatEnum(LvnDepthImageFormat format);
    static VkColorComponentFlags                getColorComponents(LvnPipelineColorWriteMask colorMask);
    static VkBlendFactor                        getBlendFactorEnum(LvnColorBlendFactor blendFactor);
    static VkBlendOp                            getBlendOperationEnum(LvnColorBlendOperation blendOp);
    static VkCompareOp                          getCompareOpEnum(LvnCompareOperation compare);
    static VkStencilOp                          getStencilOpEnum(LvnStencilOperation stencilOp);
    static VkFormat                             getVertexAttributeFormatEnum(LvnAttributeFormat format);
    static VkSampleCountFlagBits                getMaxUsableSampleCount(VulkanBackends* vkBackends);
    static VkSampleCountFlagBits                getSampleCountFlagEnum(LvnSampleCount samples);
    static uint32_t                             getSampleCountValue(VkSampleCountFlagBits samples);
    static VkSampleCountFlagBits                getSupportedSampleCount(VulkanBackends* vkBackends, LvnSampleCount samples);
    static VkDescriptorType                     getDescriptorTypeEnum(LvnDescriptorType type);
    static VkBufferUsageFlags                   getUniformBufferTypeEnum(LvnBufferType type);
    static VkShaderStageFlags                   getShaderStageFlagEnum(LvnShaderStage stage);
    static VkFilter                             getTextureFilterEnum(LvnTextureFilter filter);
    static VkSamplerAddressMode                 getTextureWrapModeEnum(LvnTextureMode mode);
    static VkPipelineColorBlendAttachmentState  createColorAttachment();

    static VulkanBackends* getVulkanBackends()
    {
        LVN_ASSERT(s_VkBackends != nullptr, "cannot get vulkan backends, vulkan backends was not created");
        return s_VkBackends;
    }

    static LvnResult createVulkanInstace(VulkanBackends* vkBackends, bool enableValidationLayers)
    {
        // create vulkan instance
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "levikno";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "levikno";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;


        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        LvnVector<const char*> extensions = vks::getRequiredExtensions(vkBackends);
        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = ARRAY_LEN(s_ValidationLayers);
            createInfo.ppEnabledLayerNames = s_ValidationLayers;

            vks::fillVulkanDebugMessengerCreateInfo(&debugCreateInfo);
            createInfo.pNext = &debugCreateInfo;
        }

        // create instance
        if (vkCreateInstance(&createInfo, nullptr, &vkBackends->instance) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] failed to create instance at (%p)", vkBackends->instance);
            return Lvn_Result_Failure;
        }

        return Lvn_Result_Success;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            {
                LVN_CORE_INFO("vulkan validation Layer: %s", pCallbackData->pMessage);
                return VK_TRUE;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            {
                LVN_CORE_WARN("vulkan validation Layer: %s", pCallbackData->pMessage);
                return VK_TRUE;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            {
                LVN_CORE_ERROR("vulkan validation Layer: %s", pCallbackData->pMessage);
                return VK_TRUE;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            {
                LVN_CORE_FATAL("vulkan validation Layer: %s", pCallbackData->pMessage);
                return VK_TRUE;
            }
            default:
            {
                return VK_FALSE;
            }
        }

        return VK_FALSE;
    }

    static LvnVector<const char*> getRequiredExtensions(VulkanBackends* vkBackends)
    {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        LvnVector<VkExtensionProperties> extensionsProps(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionsProps.data());

        for (uint32_t i = 0; i < extensionCount; i++)
        {
            if (strcmp(extensionsProps[i].extensionName, "VK_KHR_surface") == 0)
                vkBackends->vkplatform.extensions.surfaceSupport = true;

            if (strcmp(extensionsProps[i].extensionName, "VK_KHR_win32_surface") == 0)
                vkBackends->vkplatform.extensions.win32Support = true;

            if (strcmp(extensionsProps[i].extensionName, "VK_EXT_metal_surface") == 0)
                vkBackends->vkplatform.extensions.cocoaSupport = true;

            if (strcmp(extensionsProps[i].extensionName, "VK_KHR_wayland_surface") == 0)
                vkBackends->vkplatform.extensions.waylandSupport = true;

            if (strcmp(extensionsProps[i].extensionName, "VK_KHR_xcb_surface") == 0)
                vkBackends->vkplatform.extensions.x11xcbSupport = true;
            else if (strcmp(extensionsProps[i].extensionName, "VK_KHR_xlib_surface") == 0)
                vkBackends->vkplatform.extensions.x11libSupport = true;

            if (vkBackends->enableValidationLayers && strcmp(extensionsProps[i].extensionName, "VK_EXT_debug_utils") == 0)
                vkBackends->vkplatform.extensions.debugSupport = true;
        }

        LvnVector<const char*> extensions;

        if (vkBackends->vkplatform.extensions.surfaceSupport)
            extensions.push_back("VK_KHR_surface");

        if (vkBackends->vkplatform.extensions.win32Support)
            extensions.push_back("VK_KHR_win32_surface");

        if (vkBackends->vkplatform.extensions.cocoaSupport)
            extensions.push_back("VK_EXT_metal_surface");

        if (vkBackends->vkplatform.extensions.waylandSupport)
            extensions.push_back("VK_KHR_wayland_surface");

        if (vkBackends->vkplatform.extensions.x11xcbSupport)
            extensions.push_back("VK_KHR_xcb_surface");
        else if (vkBackends->vkplatform.extensions.x11xcbSupport)
            extensions.push_back("VK_KHR_xlib_surface");

        if (vkBackends->vkplatform.extensions.surfaceSupport)
            extensions.push_back("VK_EXT_debug_utils");

        LVN_ASSERT(!extensions.empty(), "cannot retrieve vulkan instance extensions");

        return extensions;
    }

    static VkResult createDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        else
            return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    static void destroyDebugUtilsMessengerEXT(
        VkInstance instance, 
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
            func(instance, debugMessenger, pAllocator);
    }

    static void fillVulkanDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo)
    {
        createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo->messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo->messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo->pfnUserCallback = debugCallback;
    }

    static LvnPhysicalDeviceType getPhysicalDeviceTypeEnum(VkPhysicalDeviceType type)
    {
        switch (type)
        {
            case VK_PHYSICAL_DEVICE_TYPE_OTHER:          { return Lvn_PhysicalDeviceType_Other; }
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: { return Lvn_PhysicalDeviceType_Integrated_GPU; }
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   { return Lvn_PhysicalDeviceType_Discrete_GPU; }
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    { return Lvn_PhysicalDeviceType_Virtual_GPU; }
            case VK_PHYSICAL_DEVICE_TYPE_CPU:            { return Lvn_PhysicalDeviceType_CPU; }
            default: { break; }
        }

        LVN_CORE_WARN("[vulkan] No physical device matches type");
        return Lvn_PhysicalDeviceType_Unknown;
    }

    static bool checkValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        LvnVector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : s_ValidationLayers)
        {
            for (uint32_t i = 0; i < layerCount; i++)
            {
                if (strcmp(layerName, availableLayers[i].layerName) == 0)
                    return true;
            }
        }
        return false;
    }

    static LvnResult setupDebugMessenger(VulkanBackends* vkBackends)
    {
        if (!vkBackends->enableValidationLayers)
            return Lvn_Result_Failure;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        vks::fillVulkanDebugMessengerCreateInfo(&createInfo);

        if (vks::createDebugUtilsMessengerEXT(vkBackends->instance, &createInfo, nullptr, &vkBackends->debugMessenger) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] failed to set up debug messenger!");
            return Lvn_Result_Failure;
        }

        return Lvn_Result_Success;
    }

    static LvnVector<VkPhysicalDevice> getPhysicalDevices(VkInstance instance)
    {
        // get physical devices
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        // create vulkan physical devices
        LvnVector<VkPhysicalDevice> physicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());
        return physicalDevices;
    }

    static VkPhysicalDevice getBestPhysicalDevice(VkInstance instance, const LvnVector<VkPhysicalDevice>& physicalDevices, VkSurfaceKHR surface)
    {
        size_t bestScore = 0;
        VkPhysicalDevice bestDevice = VK_NULL_HANDLE;

        for (const auto& physicalDevice : physicalDevices)
        {
            // check queue families
            if (surface)
            {
                VulkanQueueFamilyIndices queueIndices = vks::findQueueFamilies(physicalDevice, surface);
                if (!queueIndices.has_graphics || !queueIndices.has_present)
                    continue;
            }

            // check device extension support
            if (!vks::checkDeviceExtensionSupport(physicalDevice))
                continue;

            VkPhysicalDeviceProperties deviceProperties{};
            vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

            VkPhysicalDeviceFeatures deviceFeatures{};
            vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

            size_t score = 0;

            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                score += 1000;
            else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
                score += 500;
            else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
                score += 100;
            else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)
                score += 10;
            else
                score += 1;

            if (score > bestScore)
            {
                bestScore = score;
                bestDevice = physicalDevice;
            }
        }

        return bestDevice;
    }

    static VulkanQueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        VulkanQueueFamilyIndices indices{};

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        LvnVector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsIndex = i;
                indices.has_graphics = true;
            }

            VkBool32 presentSupport = false;
            if (surface)
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport)
            {
                indices.presentIndex = i;
                indices.has_present = true;
            }

            if (surface && indices.has_graphics && indices.has_present) // with surface, screen rendering
                break;
            if (!surface && indices.has_graphics) // no surface, offscreen rendering
                break;

            i++;
        }

        return indices;
    }

    static bool checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        LvnVector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        LvnVector<const char*> requiredExtensions(s_DeviceExtensions, s_DeviceExtensions + ARRAY_LEN(s_DeviceExtensions));

        for (uint32_t i = 0; i < requiredExtensions.size(); i++)
        {
            bool extensionFound = false;
            for (uint32_t j = 0; j < extensionCount; j++)
            {
                if (!strcmp(requiredExtensions[i], availableExtensions[j].extensionName))
                {
                    extensionFound = true;
                    break;
                }
            }

            if (!extensionFound)
            {
                LVN_CORE_ERROR("[vulkan] required vulkan extension not found: %s", requiredExtensions[i]);
                return false;
            }
        }

        return true;
    }

    static LvnResult createVulkanSurface(VkInstance instance, LvnWindow* window, VkSurfaceKHR* surface)
    {
        if (!surface)
            return Lvn_Result_Failure;

        LvnGraphicsContext* graphicsctx = lvn::getGraphicsContext();

#ifdef LVN_INCLUDE_WAYLAND
        VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo{};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.display = static_cast<wl_display*>(window->nativeWindowData.wl.display);
        surfaceCreateInfo.surface = static_cast<wl_surface*>(window->nativeWindowData.wl.surface);
        if (vkCreateWaylandSurfaceKHR(instance, &surfaceCreateInfo, nullptr, surface) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] failed to create vulkan surface under wayland");
            return Lvn_Result_Failure;
        }
#endif

        return Lvn_Result_Success;
    }

    static VulkanSwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice device)
    {
        VulkanSwapChainSupportDetails details{};

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    static LvnResult createLogicalDevice(VulkanBackends* vkBackends, VkSurfaceKHR surface)
    {
        // find queue families
        VulkanQueueFamilyIndices queueIndices = vks::findQueueFamilies(vkBackends->physicalDevice, surface);
        vkBackends->deviceIndices = queueIndices;

        // check queue families
        if (!queueIndices.has_graphics)
        {
            LVN_CORE_ERROR("[vulkan] failed to create logical device, physical device does not support graphics queue family");
            return Lvn_Result_Failure;
        }
        if (surface && !queueIndices.has_present)
        {
            LVN_CORE_ERROR("[vulkan] failed to create logical device, physical device does not support present queue family");
            return Lvn_Result_Failure;
        }

        // check device extension support
        if (!vks::checkDeviceExtensionSupport(vkBackends->physicalDevice))
        {
            LVN_CORE_ERROR("[vulkan] failed to create logical device, physical device does not support required extensions");
            return Lvn_Result_Failure;
        }

        float queuePriority = 1.0f;
        LvnVector<VkDeviceQueueCreateInfo> queueCreateInfos;
        VkDeviceQueueCreateInfo queuePresentCreateInfo{};
        queuePresentCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queuePresentCreateInfo.queueFamilyIndex = queueIndices.presentIndex;
        queuePresentCreateInfo.queueCount = 1;
        queuePresentCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queuePresentCreateInfo);

        if (queueIndices.presentIndex != queueIndices.graphicsIndex)
        {
            VkDeviceQueueCreateInfo queueGraphicsCreateInfo{};
            queueGraphicsCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueGraphicsCreateInfo.queueFamilyIndex = queueIndices.graphicsIndex;
            queueGraphicsCreateInfo.queueCount = 1;
            queueGraphicsCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueGraphicsCreateInfo);
        }

        // create logical device
        VkPhysicalDeviceFeatures deviceFeatures{};

        if (vkBackends->deviceSupportedFeatures.samplerAnisotropy)
            deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = queueCreateInfos.size();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.ppEnabledExtensionNames = s_DeviceExtensions;
        createInfo.enabledExtensionCount = ARRAY_LEN(s_DeviceExtensions);

        if (vkBackends->enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(ARRAY_LEN(s_ValidationLayers));
            createInfo.ppEnabledLayerNames = s_ValidationLayers;
        }
        else
            createInfo.enabledLayerCount = 0;

        if (vkCreateDevice(vkBackends->physicalDevice, &createInfo, nullptr, &vkBackends->device) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] failed to create logical device <VkDevice> (%p)", vkBackends->device);
            return Lvn_Result_Failure;
        }

        // get device queues
        vkGetDeviceQueue(vkBackends->device, queueIndices.graphicsIndex, 0, &vkBackends->graphicsQueue);
        if (surface)
            vkGetDeviceQueue(vkBackends->device, queueIndices.presentIndex, 0, &vkBackends->presentQueue);

        return Lvn_Result_Success;
    }

    static LvnResult setupRenderInit(VulkanBackends* vkBackends, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
    {
        if (vkBackends->device != VK_NULL_HANDLE)
            vkDeviceWaitIdle(vkBackends->device);

        if (vkBackends->commandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(vkBackends->device, vkBackends->commandPool, nullptr);
            vkBackends->commandPool = VK_NULL_HANDLE;
        }
        if (vkBackends->vmaAllocator != VK_NULL_HANDLE)
        {
            vmaDestroyAllocator(vkBackends->vmaAllocator);
            vkBackends->vmaAllocator = VK_NULL_HANDLE;
        }
        if (vkBackends->device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(vkBackends->device, nullptr);
            vkBackends->device = VK_NULL_HANDLE;
        }

        if (!physicalDevice)
        {
            LvnVector<VkPhysicalDevice> physicalDevices = vks::getPhysicalDevices(vkBackends->instance);
            physicalDevice = vks::getBestPhysicalDevice(vkBackends->instance, physicalDevices, surface);
        }

        vkBackends->physicalDevice = physicalDevice;
        VkPhysicalDeviceProperties physicalDeviceProperties{};
        vkGetPhysicalDeviceProperties(vkBackends->physicalDevice, &physicalDeviceProperties);
        vkBackends->deviceProperties = physicalDeviceProperties;

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(vkBackends->physicalDevice, &supportedFeatures);
        vkBackends->deviceSupportedFeatures = supportedFeatures;

        // create logical device once
        if (vks::createLogicalDevice(vkBackends, surface) != Lvn_Result_Success)
        {
            vkDestroyDevice(vkBackends->device, nullptr);
            return Lvn_Result_Failure;
        }

        // check swap chain present support
        if (surface)
        {
            VulkanSwapChainSupportDetails swapChainSupport = vks::querySwapChainSupport(surface, vkBackends->physicalDevice);
            if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty())
            {
                LVN_CORE_ERROR("[vulkan] selected physical device does not have supported swap chain formats or present modes");
                vkDestroyDevice(vkBackends->device, nullptr);
                return Lvn_Result_Failure;
            }
        }

        // create general command pool
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = vkBackends->deviceIndices.graphicsIndex;

        if (vkCreateCommandPool(vkBackends->device, &poolInfo, nullptr, &vkBackends->commandPool) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] failed to create command pool!");
            vkDestroyDevice(vkBackends->device, nullptr);
            return Lvn_Result_Failure;
        }

        // create VmaAllocator
        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.device = vkBackends->device;
        allocatorInfo.physicalDevice = vkBackends->physicalDevice;
        allocatorInfo.instance = vkBackends->instance;

        if (vmaCreateAllocator(&allocatorInfo, &vkBackends->vmaAllocator) != VK_SUCCESS )
        {
            LVN_CORE_ERROR("[vma] failed to create vma memory allocator for vulkan");
            vkDestroyCommandPool(vkBackends->device, vkBackends->commandPool, nullptr);
            vkDestroyDevice(vkBackends->device, nullptr);
            return Lvn_Result_Failure;
        }

        return Lvn_Result_Success;
    }

    static LvnResult createVulkanWindowSurfaceData(LvnWindow* window, VkSurfaceKHR surface)
    {
        VulkanBackends* vkBackends = vks::getVulkanBackends();

        window->apiData = lvn::memNew<VulkanWindowSurfaceData>();
        VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
        surfaceData->surface = surface;

        bool vSync = window->vSync;

        // get and check swap chain specs
        VulkanSwapChainSupportDetails swapChainSupport = vks::querySwapChainSupport(surfaceData->surface, vkBackends->physicalDevice);
        LVN_ASSERT(!swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty(), "[vulkan] device does not have supported swap chain formats or present modes");

        VkSurfaceFormatKHR surfaceFormat = vks::chooseSwapSurfaceFormat(vkBackends, swapChainSupport.formats.data(), swapChainSupport.formats.size());
        VkPresentModeKHR presentMode = vks::chooseSwapPresentMode(swapChainSupport.presentModes.data(), swapChainSupport.presentModes.size(), vSync);
        VkExtent2D extent = vks::chooseSwapExtent(window, &swapChainSupport.capabilities);

        vks::createSwapChain(vkBackends, surfaceData, swapChainSupport, surfaceFormat, presentMode, extent);
        vks::createImageViews(vkBackends, surfaceData);
        vks::createDepthResources(vkBackends, surfaceData);
        vks::createRenderPass(vkBackends, surfaceData, surfaceFormat.format);
        vks::createFrameBuffers(vkBackends, surfaceData);
        vks::createSyncObjects(vkBackends, surfaceData);

        window->renderPass.nativeRenderPass = surfaceData->renderPass;

        return Lvn_Result_Success;
    }

    static void destroyVulkanWindowSurfaceData(LvnWindow* window)
    {
        if (!window->apiData) { return; }

        VulkanBackends* vkBackends = vks::getVulkanBackends();
        VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);

        vkDeviceWaitIdle(vkBackends->device);

        // sync objects
        for (uint32_t i = 0; i < surfaceData->renderFinishedSemaphores.size(); i++)
        {
            vkDestroySemaphore(vkBackends->device, surfaceData->renderFinishedSemaphores[i], nullptr);
        }

        // swap chain images
        for (uint32_t i = 0; i < surfaceData->swapChainImageViews.size(); i++)
        {
            vkDestroyImageView(vkBackends->device, surfaceData->swapChainImageViews[i], nullptr);
        }

        vkDestroyImageView(vkBackends->device, surfaceData->depthImageView, nullptr);
        vkDestroyImage(vkBackends->device, surfaceData->depthImage, nullptr);
        vmaFreeMemory(vkBackends->vmaAllocator, surfaceData->depthImageMemory);

        // frame buffers
        for (uint32_t i = 0; i < surfaceData->frameBuffers.size(); i++)
        {
            vkDestroyFramebuffer(vkBackends->device, surfaceData->frameBuffers[i], nullptr);
        }

        // swap chain
        vkDestroySwapchainKHR(vkBackends->device, surfaceData->swapChain, nullptr);

        // render pass
        vkDestroyRenderPass(vkBackends->device, surfaceData->renderPass, nullptr);

        // window surface
        vkDestroySurfaceKHR(vkBackends->instance, surfaceData->surface, nullptr);

        lvn::memDelete<VulkanWindowSurfaceData>(surfaceData);
    }

    static VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const VkFormat* candidates, uint32_t count, VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, candidates[i], &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
                return candidates[i];
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
                return candidates[i];
        }

        LVN_ASSERT(false, "[vulkan] failed to find supported format type for physical device!");
        return VK_FORMAT_UNDEFINED;
    }

    static VkFormat findDepthFormat(VkPhysicalDevice physicalDevice)
    {
        VkFormat formats[] = { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT };
        return vks::findSupportedFormat(physicalDevice, formats, ARRAY_LEN(formats), VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

    static bool hasStencilComponent(VkFormat format)
    {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    static LvnResult createRenderPass(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData, VkFormat format)
    {
        // color attachment
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // depth attachment
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = vks::findDepthFormat(vkBackends->physicalDevice);
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcAccessMask = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


        VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = ARRAY_LEN(attachments);
        renderPassInfo.pAttachments = attachments;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(vkBackends->device, &renderPassInfo, nullptr, &surfaceData->renderPass) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] failed to create render pass");
            return Lvn_Result_Failure;
        }

        return Lvn_Result_Success;
    }

    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(VulkanBackends* vkBackends, const VkSurfaceFormatKHR* pAvailableFormats, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            if (pAvailableFormats[i].format == vkBackends->frameBufferColorFormat && pAvailableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return pAvailableFormats[i];
            }
        }

        return pAvailableFormats[0];
    }

    static VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR* pAvailablePresentModes, uint32_t count, bool vSync)
    {
        VkPresentModeKHR presentMode = vSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR;

        for (uint32_t i = 0; i < count; i++)
        {
            if (pAvailablePresentModes[i] == presentMode)
                return pAvailablePresentModes[i];
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    static VkExtent2D chooseSwapExtent(const LvnWindow* window, const VkSurfaceCapabilitiesKHR* capabilities)
    {
        if (capabilities->currentExtent.width != UINT32_MAX)
            return capabilities->currentExtent;

        VkExtent2D actualExtent = { static_cast<uint32_t>(window->width), static_cast<uint32_t>(window->height) };

        actualExtent.width = lvn::clamp(actualExtent.width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
        actualExtent.height = lvn::clamp(actualExtent.height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);

        return actualExtent;
    }

    static LvnResult createSwapChain(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData, VulkanSwapChainSupportDetails swapChainSupport, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode, VkExtent2D extent)
    {
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
            imageCount = swapChainSupport.capabilities.maxImageCount;

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surfaceData->surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        VulkanQueueFamilyIndices indices = vks::findQueueFamilies(vkBackends->physicalDevice, surfaceData->surface);
        uint32_t queueFamilyIndices[] = { indices.graphicsIndex, indices.presentIndex };

        if (indices.graphicsIndex != indices.presentIndex)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(vkBackends->device, &createInfo, nullptr, &surfaceData->swapChain) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] failed to create swap chain");
            return Lvn_Result_Failure;
        }

        if (vkGetSwapchainImagesKHR(vkBackends->device, surfaceData->swapChain, &imageCount, nullptr) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] failed to get swap chain image count");
            return Lvn_Result_Failure;
        }

        surfaceData->swapChainImages.resize(imageCount);
        if (vkGetSwapchainImagesKHR(vkBackends->device, surfaceData->swapChain, &imageCount, surfaceData->swapChainImages.data()) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] failed to get swap chain image count");
            return Lvn_Result_Failure;
        }

        surfaceData->swapChainImageFormat = surfaceFormat.format;
        surfaceData->swapChainExtent = extent;

        return Lvn_Result_Success;
    }

    static VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] failed to create image view for swap chain");
            return VK_NULL_HANDLE;
        };

        return imageView;
    }

    static void createImageViews(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData)
    {
        surfaceData->swapChainImageViews.resize(surfaceData->swapChainImages.size());

        for (size_t i = 0; i < surfaceData->swapChainImageViews.size(); i++)
            surfaceData->swapChainImageViews[i] = vks::createImageView(vkBackends->device, surfaceData->swapChainImages[i], surfaceData->swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    static void createDepthResources(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData)
    {
        VkFormat depthFormat = vks::findDepthFormat(vkBackends->physicalDevice);

        vks::createImage(vkBackends, &surfaceData->depthImage, &surfaceData->depthImageMemory, surfaceData->swapChainExtent.width, surfaceData->swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_SAMPLE_COUNT_1_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
        surfaceData->depthImageView = vks::createImageView(vkBackends->device, surfaceData->depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

        vks::transitionImageLayout(vkBackends, surfaceData->depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
    }

    static LvnResult createFrameBuffers(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData)
    {
        surfaceData->frameBuffers.resize(surfaceData->swapChainImages.size());

        for (size_t i = 0; i < surfaceData->frameBuffers.size(); i++)
        {
            VkImageView attachments[] =
            {
                surfaceData->swapChainImageViews[i],
                surfaceData->depthImageView
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = surfaceData->renderPass;
            framebufferInfo.attachmentCount = ARRAY_LEN(attachments);
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = surfaceData->swapChainExtent.width;
            framebufferInfo.height = surfaceData->swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(vkBackends->device, &framebufferInfo, nullptr, &surfaceData->frameBuffers[i]) != VK_SUCCESS)
            {
                LVN_CORE_ERROR("[vulkan] failed to create framebuffer for swap chain");
                return Lvn_Result_Failure;
            }
        }

        return Lvn_Result_Success;
    }

    static LvnResult createSyncObjects(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData)
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        surfaceData->renderFinishedSemaphores.resize(surfaceData->swapChainImages.size());
        for (uint32_t i = 0; i < surfaceData->renderFinishedSemaphores.size(); i++)
        {
            if (vkCreateSemaphore(vkBackends->device, &semaphoreInfo, nullptr, &surfaceData->renderFinishedSemaphores[i]) != VK_SUCCESS)
            {
                LVN_CORE_ERROR("[vulkan] failed to create semaphore for swap chain");
                return Lvn_Result_Failure;
            }
        }

        return Lvn_Result_Success;
    }

    static void cleanSwapChain(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData)
    {
        // swap chain images
        for (uint32_t i = 0; i < surfaceData->swapChainImageViews.size(); i++)
        {
            vkDestroyImageView(vkBackends->device, surfaceData->swapChainImageViews[i], nullptr);
        }

        vkDestroyImageView(vkBackends->device, surfaceData->depthImageView, nullptr);
        vkDestroyImage(vkBackends->device, surfaceData->depthImage, nullptr);
        vmaFreeMemory(vkBackends->vmaAllocator, surfaceData->depthImageMemory);

        // frame buffers
        for (uint32_t i = 0; i < surfaceData->frameBuffers.size(); i++)
            vkDestroyFramebuffer(vkBackends->device, surfaceData->frameBuffers[i], nullptr);

        // swap chain
        vkDestroySwapchainKHR(vkBackends->device, surfaceData->swapChain, nullptr);
    }

    static void recreateSwapChain(VulkanBackends* vkBackends, LvnWindow* window)
    {
        vkDeviceWaitIdle(vkBackends->device);

        VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
        bool vSync = window->vSync;

        vks::cleanSwapChain(vkBackends, surfaceData);

        VulkanSwapChainSupportDetails swapChainSupport = vks::querySwapChainSupport(surfaceData->surface, vkBackends->physicalDevice);
        LVN_ASSERT(!swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty(), "[vulkan] physical device does not have swap chain support formats or present modes!");

        VkSurfaceFormatKHR surfaceFormat = vks::chooseSwapSurfaceFormat(vkBackends, swapChainSupport.formats.data(), swapChainSupport.formats.size());
        VkPresentModeKHR presentMode = vks::chooseSwapPresentMode(swapChainSupport.presentModes.data(), swapChainSupport.presentModes.size(), vSync);
        VkExtent2D extent = vks::chooseSwapExtent(window, &swapChainSupport.capabilities);

        vks::createSwapChain(vkBackends, surfaceData, swapChainSupport, surfaceFormat, presentMode, extent);
        vks::createImageViews(vkBackends, surfaceData);
        vks::createDepthResources(vkBackends, surfaceData);
        vks::createFrameBuffers(vkBackends, surfaceData);
    }

    static LvnResult createBuffer(VulkanBackends* vkBackends, VkBuffer* buffer, VmaAllocation* bufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memUsage)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memUsage;

        if (vmaCreateBuffer(vkBackends->vmaAllocator, &bufferInfo, &allocInfo, buffer, bufferMemory, nullptr) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] failed to create buffer <VkBuffer> (%p), buffer memory: (%p), buffer size: %zu bytes", *buffer, *bufferMemory, size);
            return Lvn_Result_Failure;
        }

        return Lvn_Result_Success;
    }

    static VulkanPipeline createVulkanPipeline(VulkanBackends* vkBackends, VulkanPipelineCreateData* createData)
    {
        VulkanPipeline pipeline{};

        LvnPipelineFixedFunctions* pipelineSpecification = createData->pipelineFixedFuncs;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

        inputAssembly.topology = vks::getVulkanTopologyTypeEnum(pipelineSpecification->inputAssembly.topology);
        inputAssembly.primitiveRestartEnable = pipelineSpecification->inputAssembly.primitiveRestartEnable;

        LvnVector<VkDynamicState> dynamicStates;
        dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
        dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

        if (pipelineSpecification->depthstencil.enableStencil)
        {
            dynamicStates.push_back(VK_DYNAMIC_STATE_STENCIL_REFERENCE);
            dynamicStates.push_back(VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK);
            dynamicStates.push_back(VK_DYNAMIC_STATE_STENCIL_WRITE_MASK);
        }

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = pipelineSpecification->rasterizer.depthClampEnable;
        rasterizer.rasterizerDiscardEnable = pipelineSpecification->rasterizer.rasterizerDiscardEnable;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = pipelineSpecification->rasterizer.lineWidth;
        rasterizer.cullMode = vks::getVulkanCullModeFlagEnum(pipelineSpecification->rasterizer.cullMode);
        rasterizer.frontFace = vks::getVulkanCullFrontFaceEnum(pipelineSpecification->rasterizer.frontFace);
        rasterizer.depthBiasEnable = pipelineSpecification->rasterizer.depthBiasEnable;
        rasterizer.depthBiasConstantFactor = pipelineSpecification->rasterizer.depthBiasConstantFactor; // Optional
        rasterizer.depthBiasClamp = pipelineSpecification->rasterizer.depthBiasClamp; // Optional
        rasterizer.depthBiasSlopeFactor = pipelineSpecification->rasterizer.depthBiasSlopeFactor; // Optional

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = pipelineSpecification->multisampling.sampleShadingEnable;
        multisampling.rasterizationSamples = vks::getSupportedSampleCount(vkBackends, pipelineSpecification->multisampling.rasterizationSamples);
        multisampling.minSampleShading = pipelineSpecification->multisampling.minSampleShading; // Optional
        multisampling.pSampleMask = pipelineSpecification->multisampling.sampleMask; // Optional
        multisampling.alphaToCoverageEnable = pipelineSpecification->multisampling.alphaToCoverageEnable; // Optional
        multisampling.alphaToOneEnable = pipelineSpecification->multisampling.alphaToOneEnable; // Optional

        LvnVector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;

        if (pipelineSpecification->colorBlend.colorBlendAttachmentCount == 0 || pipelineSpecification->colorBlend.pColorBlendAttachments == nullptr)
        {
            colorBlendAttachments.push_back(vks::createColorAttachment());
        }
        else
        {
            for (uint32_t i = 0; i < pipelineSpecification->colorBlend.colorBlendAttachmentCount; i++)
            {
                LvnPipelineColorBlendAttachment attachment = pipelineSpecification->colorBlend.pColorBlendAttachments[i];

                VkPipelineColorBlendAttachmentState colorBlendAttachment{};
                colorBlendAttachment.colorWriteMask = vks::getColorComponents(attachment.colorWriteMask);
                colorBlendAttachment.blendEnable = attachment.blendEnable;
                colorBlendAttachment.srcColorBlendFactor = vks::getBlendFactorEnum(attachment.srcColorBlendFactor);
                colorBlendAttachment.dstColorBlendFactor = vks::getBlendFactorEnum(attachment.dstColorBlendFactor);
                colorBlendAttachment.colorBlendOp = vks::getBlendOperationEnum(attachment.colorBlendOp);
                colorBlendAttachment.srcAlphaBlendFactor = vks::getBlendFactorEnum(attachment.srcAlphaBlendFactor);
                colorBlendAttachment.dstAlphaBlendFactor = vks::getBlendFactorEnum(attachment.dstAlphaBlendFactor);
                colorBlendAttachment.alphaBlendOp = vks::getBlendOperationEnum(attachment.alphaBlendOp);

                colorBlendAttachments.push_back(colorBlendAttachment);
            }
        }

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = pipelineSpecification->colorBlend.logicOpEnable;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
        colorBlending.pAttachments = colorBlendAttachments.data();
        colorBlending.blendConstants[0] = pipelineSpecification->colorBlend.blendConstants[0]; // Optional
        colorBlending.blendConstants[1] = pipelineSpecification->colorBlend.blendConstants[1]; // Optional
        colorBlending.blendConstants[2] = pipelineSpecification->colorBlend.blendConstants[2]; // Optional
        colorBlending.blendConstants[3] = pipelineSpecification->colorBlend.blendConstants[3]; // Optional

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = pipelineSpecification->depthstencil.enableDepth ? VK_TRUE : VK_FALSE;
        depthStencil.depthWriteEnable = pipelineSpecification->depthstencil.enableDepth ? VK_TRUE : VK_FALSE;
        depthStencil.depthCompareOp = vks::getCompareOpEnum(pipelineSpecification->depthstencil.depthOpCompare);
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f; // Optional
        depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = pipelineSpecification->depthstencil.enableStencil ? VK_TRUE : VK_FALSE;
        depthStencil.back.compareMask = pipelineSpecification->depthstencil.stencil.compareMask;
        depthStencil.back.writeMask = pipelineSpecification->depthstencil.stencil.writeMask;
        depthStencil.back.reference = pipelineSpecification->depthstencil.stencil.reference;
        depthStencil.back.compareOp = vks::getCompareOpEnum(pipelineSpecification->depthstencil.stencil.compareOp);
        depthStencil.back.depthFailOp = vks::getStencilOpEnum(pipelineSpecification->depthstencil.stencil.depthFailOp);
        depthStencil.back.failOp = vks::getStencilOpEnum(pipelineSpecification->depthstencil.stencil.failOp);
        depthStencil.back.passOp = vks::getStencilOpEnum(pipelineSpecification->depthstencil.stencil.passOp);
        depthStencil.front = depthStencil.back;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        if (createData->descriptorSetLayoutCount != 0)
        {
            pipelineLayoutInfo.setLayoutCount = createData->descriptorSetLayoutCount;
            pipelineLayoutInfo.pSetLayouts = createData->pDescrptorSetLayouts;
        }
        else
        {
            pipelineLayoutInfo.setLayoutCount = 0;
            pipelineLayoutInfo.pSetLayouts = nullptr;
        }

        if (createData->pushConstantCount != 0)
        {
            pipelineLayoutInfo.pushConstantRangeCount = createData->pushConstantCount;
            pipelineLayoutInfo.pPushConstantRanges = createData->pPushConstants;
        }
        else
        {
            pipelineLayoutInfo.pushConstantRangeCount = 0;
            pipelineLayoutInfo.pPushConstantRanges = nullptr;
        }

        VkResult result = vkCreatePipelineLayout(vkBackends->device, &pipelineLayoutInfo, nullptr, &pipeline.pipelineLayout);
        LVN_ASSERT(result == VK_SUCCESS, "[vulkan] failed to create pipeline layout!");

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.renderPass = createData->renderPass;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = createData->shaderStages;
        pipelineInfo.pVertexInputState = &createData->vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = pipeline.pipelineLayout;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        result = vkCreateGraphicsPipelines(vkBackends->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.pipeline);
        LVN_ASSERT(result == VK_SUCCESS, "[vulkan] failed to create graphics pipeline!");

        return pipeline;
    }

    static LvnResult createImage(VulkanBackends* vkBackends, VkImage* image, VmaAllocation* imageMemory, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkSampleCountFlagBits samples, VmaMemoryUsage memUsage)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = samples;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        if (vmaCreateImage(vkBackends->vmaAllocator, &imageInfo, &allocInfo, image, imageMemory, nullptr) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] failed to create image <VkImage>, image size: (w:%u, h:%u)", width, height);
            return Lvn_Result_Failure;
        }

        return Lvn_Result_Success;
    }

    static VkShaderModule createShaderModule(VulkanBackends* vkBackends, const uint8_t* code, uint32_t size)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = size;
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code);

        VkShaderModule shaderModule;
        VkResult result = vkCreateShaderModule(vkBackends->device, &createInfo, nullptr, &shaderModule);
        LVN_ASSERT(result == VK_SUCCESS, "[vulkan] failed to create shader module!");

        return shaderModule;
    }

    static void transitionImageLayout(VulkanBackends* vkBackends, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = vkBackends->commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(vkBackends->device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);


        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = layerCount;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (vks::hasStencilComponent(format))
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        else
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } 
        else
        {
            LVN_CORE_ERROR("[vulkan] unsupported layout transition during image layout transition");
            LVN_ASSERT(false,"");
            return;
        }

        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);


        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(vkBackends->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(vkBackends->graphicsQueue);

        vkFreeCommandBuffers(vkBackends->device, vkBackends->commandPool, 1, &commandBuffer);
    }

    void copyBuffer(VulkanBackends* vkBackends, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = vkBackends->commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(vkBackends->device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        copyRegion.srcOffset = srcOffset;
        copyRegion.dstOffset = dstOffset;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(vkBackends->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(vkBackends->graphicsQueue);

        vkFreeCommandBuffers(vkBackends->device, vkBackends->commandPool, 1, &commandBuffer);
    }


    static void copyBufferToImage(VulkanBackends* vkBackends, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = vkBackends->commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(vkBackends->device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);


        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = layerCount;

        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { width, height, 1 };

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(vkBackends->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(vkBackends->graphicsQueue);

        vkFreeCommandBuffers(vkBackends->device, vkBackends->commandPool, 1, &commandBuffer);
    }

    static VkPrimitiveTopology getVulkanTopologyTypeEnum(LvnTopologyType topologyType)
    {
        switch (topologyType)
        {
            case Lvn_TopologyType_Point: { return VK_PRIMITIVE_TOPOLOGY_POINT_LIST; }
            case Lvn_TopologyType_Line: { return VK_PRIMITIVE_TOPOLOGY_LINE_LIST; }
            case Lvn_TopologyType_LineStrip: { return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; }
            case Lvn_TopologyType_Triangle: { return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; }
            case Lvn_TopologyType_TriangleStrip: { return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; }
            default:
            {
                LVN_CORE_WARN("unknown topology type enum (%d), setting to triangle topology type (default)", topologyType);
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            }
        }
    }

    static VkCullModeFlags getVulkanCullModeFlagEnum(LvnCullFaceMode cullFaceMode)
    {
        switch (cullFaceMode)
        {
            case Lvn_CullFaceMode_Disable: { return VK_CULL_MODE_NONE; }
            case Lvn_CullFaceMode_Front: { return VK_CULL_MODE_FRONT_BIT; }
            case Lvn_CullFaceMode_Back: { return VK_CULL_MODE_BACK_BIT; }
            case Lvn_CullFaceMode_Both: { return VK_CULL_MODE_FRONT_AND_BACK; }
            default:
            {
                LVN_CORE_WARN("unknown cull face mode enum (%d), setting to cull face mode none (default)", cullFaceMode);
                return VK_CULL_MODE_NONE;
            }
        }
    }

    static VkFrontFace getVulkanCullFrontFaceEnum(LvnCullFrontFace cullFrontFace)
    {
        switch (cullFrontFace)
        {
            case Lvn_CullFrontFace_Clockwise: { return VK_FRONT_FACE_CLOCKWISE; }
            case Lvn_CullFrontFace_CounterClockwise: { return VK_FRONT_FACE_COUNTER_CLOCKWISE; }
            default:
            {
                LVN_CORE_WARN("unknown cull front face enum (%d), setting to cull front face clockwise (default)", cullFrontFace);
                return VK_FRONT_FACE_CLOCKWISE;
            }
        }

    }

    static VkFormat getVulkanColorFormatEnum(LvnColorImageFormat format)
    {
        switch (format)
        {
            case Lvn_ColorImageFormat_None: { return VK_FORMAT_UNDEFINED; }
            case Lvn_ColorImageFormat_RGB: { return VK_FORMAT_R8G8B8_UNORM; }
            case Lvn_ColorImageFormat_RGBA: { return VK_FORMAT_R8G8B8A8_UNORM; }
            case Lvn_ColorImageFormat_RGBA8: { return VK_FORMAT_R8G8B8A8_UNORM; }
            case Lvn_ColorImageFormat_RGBA16F: { return VK_FORMAT_R16G16B16A16_SFLOAT; }
            case Lvn_ColorImageFormat_RGBA32F: { return VK_FORMAT_R32G32B32A32_SFLOAT; }
            case Lvn_ColorImageFormat_SRGB: { return VK_FORMAT_R8G8B8_SRGB; }
            case Lvn_ColorImageFormat_SRGBA: { return VK_FORMAT_R8G8B8A8_SRGB; }
            case Lvn_ColorImageFormat_SRGBA8: { return VK_FORMAT_R8G8B8A8_SRGB; }
            case Lvn_ColorImageFormat_SRGBA16F: { return VK_FORMAT_R16G16B16A16_SFLOAT; }
            case Lvn_ColorImageFormat_SRGBA32F: { return VK_FORMAT_R32G32B32A32_SFLOAT; }
            case Lvn_ColorImageFormat_RedInt: { return VK_FORMAT_R8_SINT; }

            default:
            {
                LVN_CORE_WARN("unknown image format enum (%d), setting image format to undefined", format);
                return VK_FORMAT_UNDEFINED;
            }
        }
    }

    static VkFormat getVulkanDepthFormatEnum(LvnDepthImageFormat format)
    {
        switch (format)
        {
            case Lvn_DepthImageFormat_Depth16: { return VK_FORMAT_D16_UNORM; }
            case Lvn_DepthImageFormat_Depth32: { return VK_FORMAT_D32_SFLOAT; }
            case Lvn_DepthImageFormat_Depth24Stencil8: { return VK_FORMAT_D24_UNORM_S8_UINT; }
            case Lvn_DepthImageFormat_Depth32Stencil8: { return VK_FORMAT_D32_SFLOAT_S8_UINT; }

            default:
            {
                LVN_CORE_WARN("unknown image format enum (%u), image format must be a depth component format", format);
                return VK_FORMAT_UNDEFINED;
            }
        }
    }

    static VkColorComponentFlags getColorComponents(LvnPipelineColorWriteMask colorMask)
    {
        VkColorComponentFlags colorComponentsFlag = 0;

        if (colorMask.colorComponentR) colorComponentsFlag |= VK_COLOR_COMPONENT_R_BIT;
        if (colorMask.colorComponentG) colorComponentsFlag |= VK_COLOR_COMPONENT_G_BIT;
        if (colorMask.colorComponentB) colorComponentsFlag |= VK_COLOR_COMPONENT_B_BIT;
        if (colorMask.colorComponentA) colorComponentsFlag |= VK_COLOR_COMPONENT_A_BIT;

        return colorComponentsFlag;
    }

    static VkBlendFactor getBlendFactorEnum(LvnColorBlendFactor blendFactor)
    {
        switch (blendFactor)
        {
            case Lvn_ColorBlendFactor_Zero: { return VK_BLEND_FACTOR_ZERO; }
            case Lvn_ColorBlendFactor_One: { return VK_BLEND_FACTOR_ONE; }
            case Lvn_ColorBlendFactor_SrcColor: { return VK_BLEND_FACTOR_SRC_COLOR; }
            case Lvn_ColorBlendFactor_OneMinusSrcColor: { return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR; }
            case Lvn_ColorBlendFactor_DstColor: { return VK_BLEND_FACTOR_DST_COLOR; }
            case Lvn_ColorBlendFactor_OneMinusDstColor: { return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR; }
            case Lvn_ColorBlendFactor_SrcAlpha: { return VK_BLEND_FACTOR_SRC_ALPHA; }
            case Lvn_ColorBlendFactor_OneMinusSrcAlpha: { return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; }
            case Lvn_ColorBlendFactor_DstAlpha: { return VK_BLEND_FACTOR_DST_ALPHA; }
            case Lvn_ColorBlendFactor_OneMinusDstAlpha: { return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA; }
            case Lvn_ColorBlendFactor_ConstantColor: { return VK_BLEND_FACTOR_CONSTANT_COLOR; }
            case Lvn_ColorBlendFactor_OneMinusConstantColor: { return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR; }
            case Lvn_ColorBlendFactor_ConstantAlpha: { return VK_BLEND_FACTOR_CONSTANT_ALPHA; }
            case Lvn_ColorBlendFactor_OneMinusConstantAlpha: { return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA; }
            case Lvn_ColorBlendFactor_SrcAlphaSaturate: { return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE; }
            case Lvn_ColorBlendFactor_Src1Color: { return VK_BLEND_FACTOR_SRC1_COLOR; }
            case Lvn_ColorBlendFactor_OneMinusSrc1Color: { return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR; }
            case Lvn_ColorBlendFactor_Src1_Alpha: { return VK_BLEND_FACTOR_SRC1_ALPHA; }
            case Lvn_ColorBlendFactor_OneMinusSrc1Alpha: { return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA; }
            default:
            {
                LVN_CORE_WARN("unknown blend factor enum (%d), setting to blend factor zero (default)", blendFactor);
                return VK_BLEND_FACTOR_ZERO;
            }
        }
    }

    static VkBlendOp getBlendOperationEnum(LvnColorBlendOperation blendOp)
    {
        switch (blendOp)
        {
            case Lvn_ColorBlendOp_Add: { return VK_BLEND_OP_ADD; }
            case Lvn_ColorBlendOp_Subtract: { return VK_BLEND_OP_SUBTRACT; }
            case Lvn_ColorBlendOp_ReverseSubtract: { return VK_BLEND_OP_REVERSE_SUBTRACT; }
            case Lvn_ColorBlendOp_Min: { return VK_BLEND_OP_MIN; }
            case Lvn_ColorBlendOp_Max: { return VK_BLEND_OP_MAX; }

            default:
            {
                LVN_CORE_WARN("unknown blend operation enum (%d), setting to blend operation enum add (default)", blendOp);
                return VK_BLEND_OP_ADD;
            }
        }
    }

    static VkCompareOp getCompareOpEnum(LvnCompareOperation compare)
    {
        switch (compare)
        {
            case Lvn_CompareOp_Never: { return VK_COMPARE_OP_NEVER; }
            case Lvn_CompareOp_Less: { return VK_COMPARE_OP_LESS; }
            case Lvn_CompareOp_Equal: { return VK_COMPARE_OP_EQUAL; }
            case Lvn_CompareOp_LessOrEqual: { return VK_COMPARE_OP_LESS_OR_EQUAL; }
            case Lvn_CompareOp_Greater: { return VK_COMPARE_OP_GREATER; }
            case Lvn_CompareOp_NotEqual: { return VK_COMPARE_OP_NOT_EQUAL; }
            case Lvn_CompareOp_GreaterOrEqual: { return VK_COMPARE_OP_GREATER_OR_EQUAL; }
            case Lvn_CompareOp_Always: { return VK_COMPARE_OP_ALWAYS; }
            default:
            {
                LVN_CORE_WARN("unknown compare enum (%d), setting to compare enum never", compare);
                return VK_COMPARE_OP_NEVER;
            }
        }
    }

    static VkStencilOp getStencilOpEnum(LvnStencilOperation stencilOp)
    {
        switch (stencilOp)
        {
            case Lvn_StencilOp_Keep: { return VK_STENCIL_OP_KEEP; }
            case Lvn_StencilOp_Zero: { return VK_STENCIL_OP_ZERO; }
            case Lvn_StencilOp_Replace: { return VK_STENCIL_OP_REPLACE; }
            case Lvn_StencilOp_IncrementAndClamp: { return VK_STENCIL_OP_INCREMENT_AND_CLAMP; }
            case Lvn_StencilOp_DecrementAndClamp: { return VK_STENCIL_OP_DECREMENT_AND_CLAMP; }
            case Lvn_StencilOp_Invert: { return VK_STENCIL_OP_INVERT; }
            case Lvn_StencilOp_IncrementAndWrap: { return VK_STENCIL_OP_INCREMENT_AND_WRAP; }
            case Lvn_StencilOp_DecrementAndWrap: { return VK_STENCIL_OP_DECREMENT_AND_WRAP; }
            default:
            {
                LVN_CORE_WARN("unknown stencil operation enum (%d), setting to stencil operation enum keep (default)", stencilOp);
                return VK_STENCIL_OP_KEEP;
            }
        }
    }

    static VkFormat getVertexAttributeFormatEnum(LvnAttributeFormat format)
    {
        switch (format)
        {
            case Lvn_AttributeFormat_Undefined:        { return VK_FORMAT_UNDEFINED; }
            case Lvn_AttributeFormat_Scalar_f32:       { return VK_FORMAT_R32_SFLOAT; }
            case Lvn_AttributeFormat_Scalar_f64:       { return VK_FORMAT_R64_SFLOAT; }
            case Lvn_AttributeFormat_Scalar_i32:       { return VK_FORMAT_R32_SINT; }
            case Lvn_AttributeFormat_Scalar_ui32:      { return VK_FORMAT_R32_UINT; }
            case Lvn_AttributeFormat_Scalar_i8:        { return VK_FORMAT_R8_SINT; }
            case Lvn_AttributeFormat_Scalar_ui8:       { return VK_FORMAT_R8_UINT; }
            case Lvn_AttributeFormat_Vec2_f32:         { return VK_FORMAT_R32G32_SFLOAT; }
            case Lvn_AttributeFormat_Vec3_f32:         { return VK_FORMAT_R32G32B32_SFLOAT; }
            case Lvn_AttributeFormat_Vec4_f32:         { return VK_FORMAT_R32G32B32A32_SFLOAT; }
            case Lvn_AttributeFormat_Vec2_f64:         { return VK_FORMAT_R64G64_SFLOAT; }
            case Lvn_AttributeFormat_Vec3_f64:         { return VK_FORMAT_R64G64B64_SFLOAT; }
            case Lvn_AttributeFormat_Vec4_f64:         { return VK_FORMAT_R64G64B64A64_SFLOAT; }
            case Lvn_AttributeFormat_Vec2_i32:         { return VK_FORMAT_R32G32_SINT; }
            case Lvn_AttributeFormat_Vec3_i32:         { return VK_FORMAT_R32G32B32_SINT; }
            case Lvn_AttributeFormat_Vec4_i32:         { return VK_FORMAT_R32G32B32A32_SINT; }
            case Lvn_AttributeFormat_Vec2_ui32:        { return VK_FORMAT_R32G32_UINT; }
            case Lvn_AttributeFormat_Vec3_ui32:        { return VK_FORMAT_R32G32B32_UINT; }
            case Lvn_AttributeFormat_Vec4_ui32:        { return VK_FORMAT_R32G32B32A32_UINT; }
            case Lvn_AttributeFormat_Vec2_i8:          { return VK_FORMAT_R8G8_SINT; }
            case Lvn_AttributeFormat_Vec3_i8:          { return VK_FORMAT_R8G8B8_SINT; }
            case Lvn_AttributeFormat_Vec4_i8:          { return VK_FORMAT_R8G8B8A8_SINT; }
            case Lvn_AttributeFormat_Vec2_ui8:         { return VK_FORMAT_R8G8_UINT; }
            case Lvn_AttributeFormat_Vec3_ui8:         { return VK_FORMAT_R8G8B8_UINT; }
            case Lvn_AttributeFormat_Vec4_ui8:         { return VK_FORMAT_R8G8B8A8_UINT; }
            case Lvn_AttributeFormat_Vec2_n8:          { return VK_FORMAT_R8G8_SNORM; }
            case Lvn_AttributeFormat_Vec3_n8:          { return VK_FORMAT_R8G8B8_SNORM; }
            case Lvn_AttributeFormat_Vec4_n8:          { return VK_FORMAT_R8G8B8A8_SNORM; }
            case Lvn_AttributeFormat_Vec2_un8:         { return VK_FORMAT_R8G8_UNORM; }
            case Lvn_AttributeFormat_Vec3_un8:         { return VK_FORMAT_R8G8B8_UNORM; }
            case Lvn_AttributeFormat_Vec4_un8:         { return VK_FORMAT_R8G8B8A8_UNORM; }
            case Lvn_AttributeFormat_2_10_10_10_ile:   { return VK_FORMAT_A2B10G10R10_SINT_PACK32; }
            case Lvn_AttributeFormat_2_10_10_10_uile:  { return VK_FORMAT_A2B10G10R10_UINT_PACK32; }
            case Lvn_AttributeFormat_2_10_10_10_nle:   { return VK_FORMAT_A2B10G10R10_SNORM_PACK32; }
            case Lvn_AttributeFormat_2_10_10_10_unle:  { return VK_FORMAT_A2B10G10R10_UNORM_PACK32; }

            default:
            {
                LVN_CORE_WARN("uknown vertex attribute format type enum (%d), setting to format type undefined", format);
                return VK_FORMAT_UNDEFINED;
            }
        }
    }

    static VkSampleCountFlagBits getMaxUsableSampleCount(VulkanBackends* vkBackends)
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(vkBackends->physicalDevice, &physicalDeviceProperties);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
        if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
        if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
        if (counts & VK_SAMPLE_COUNT_8_BIT)  { return VK_SAMPLE_COUNT_8_BIT; }
        if (counts & VK_SAMPLE_COUNT_4_BIT)  { return VK_SAMPLE_COUNT_4_BIT; }
        if (counts & VK_SAMPLE_COUNT_2_BIT)  { return VK_SAMPLE_COUNT_2_BIT; }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    static VkSampleCountFlagBits getSampleCountFlagEnum(LvnSampleCount samples)
    {
        switch (samples)
        {
            case Lvn_SampleCount_1_Bit: { return VK_SAMPLE_COUNT_1_BIT; }
            case Lvn_SampleCount_2_Bit: { return VK_SAMPLE_COUNT_2_BIT; }
            case Lvn_SampleCount_4_Bit: { return VK_SAMPLE_COUNT_4_BIT; }
            case Lvn_SampleCount_8_Bit: { return VK_SAMPLE_COUNT_8_BIT; }
            case Lvn_SampleCount_16_Bit: { return VK_SAMPLE_COUNT_16_BIT; }
            case Lvn_SampleCount_32_Bit: { return VK_SAMPLE_COUNT_32_BIT; }
            case Lvn_SampleCount_64_Bit: { return VK_SAMPLE_COUNT_64_BIT; }
            default:
            {
                LVN_CORE_WARN("unknown sampler count enum (%d), setting to sample count enum 1 bit (default)", samples);
                return VK_SAMPLE_COUNT_1_BIT;
            }
        }
    }

    static uint32_t getSampleCountValue(VkSampleCountFlagBits samples)
    {
        switch (samples)
        {
            case VK_SAMPLE_COUNT_1_BIT: { return 1; }
            case VK_SAMPLE_COUNT_2_BIT: { return 2; }
            case VK_SAMPLE_COUNT_4_BIT: { return 4; }
            case VK_SAMPLE_COUNT_8_BIT: { return 8; }
            case VK_SAMPLE_COUNT_16_BIT: { return 16; }
            case VK_SAMPLE_COUNT_32_BIT: { return 32; }
            case VK_SAMPLE_COUNT_64_BIT: { return 64; }
            case VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM: { return 0x7FFFFFFF; }
            default:
            {
                LVN_CORE_WARN("could not find vulkan sample enum match!");
                return 1;
            }
        }
    }

    static VkSampleCountFlagBits getSupportedSampleCount(VulkanBackends* vkBackends, LvnSampleCount samples)
    {
        VkSampleCountFlagBits fbSampleCount = vks::getSampleCountFlagEnum(samples);
        VkSampleCountFlagBits maxSampleCount = vks::getMaxUsableSampleCount(vkBackends);

        uint32_t fbCount = getSampleCountValue(fbSampleCount);
        uint32_t maxCount = getSampleCountValue(maxSampleCount);

        if (fbCount > maxCount)
        {
            LVN_CORE_WARN("specified sample count (%u) is higher than the max sample count that is supported by the device (%u); using supported sample count instead!", fbCount, maxCount);
            return maxSampleCount;
        }

        return fbSampleCount;
    }

    static VkDescriptorType getDescriptorTypeEnum(LvnDescriptorType type)
    {
        switch (type)
        {
            case Lvn_DescriptorType_ImageSampler: { return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; }
            case Lvn_DescriptorType_ImageSamplerBindless: { return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; }
            case Lvn_DescriptorType_UniformBuffer: { return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; }
            case Lvn_DescriptorType_StorageBuffer: { return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; }

            default:
            {
                LVN_CORE_WARN("unknown descriptor type enum (%d), setting to descriptor type sampler (defualt)", type);
                return VK_DESCRIPTOR_TYPE_SAMPLER;
            }
        }
    }

    static VkBufferUsageFlags getUniformBufferTypeEnum(LvnBufferType type)
    {
        switch (type)
        {
            case Lvn_BufferType_Uniform: { return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; }
            case Lvn_BufferType_Storage: { return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT; }

            default:
            {
                LVN_CORE_WARN("unknown buffer enum type (%u), setting to buffer type uniform buffer (defualt)", type);
                return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            }
        }
    }

    static VkShaderStageFlags getShaderStageFlagEnum(LvnShaderStage stage)
    {
        switch (stage)
        {
            case Lvn_ShaderStage_All: { return VK_SHADER_STAGE_ALL; }
            case Lvn_ShaderStage_Vertex: { return VK_SHADER_STAGE_VERTEX_BIT; }
            case Lvn_ShaderStage_Fragment: { return VK_SHADER_STAGE_FRAGMENT_BIT; }

            default:
            {
                LVN_CORE_WARN("unknown shader stage enum type (%u), setting stage to \'VK_SHADER_STAGE_ALL\' as default", stage);
                return VK_SHADER_STAGE_ALL;
            }
        }
    }

    static VkFilter getTextureFilterEnum(LvnTextureFilter filter)
    {
        switch (filter)
        {
            case Lvn_TextureFilter_Nearest: { return VK_FILTER_NEAREST; }
            case Lvn_TextureFilter_Linear: { return VK_FILTER_LINEAR; }

            default:
            {
                LVN_CORE_WARN("unknown sampler filter enum type (%u), setting filter to \'VK_FILTER_NEAREST\' as default", filter);
                return VK_FILTER_NEAREST;
            }
        }
    }

    static VkSamplerAddressMode getTextureWrapModeEnum(LvnTextureMode mode)
    {
        switch (mode)
        {
            case Lvn_TextureMode_Repeat: { return VK_SAMPLER_ADDRESS_MODE_REPEAT; }
            case Lvn_TextureMode_MirrorRepeat: { return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT; }
            case Lvn_TextureMode_ClampToEdge: { return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; }
            case Lvn_TextureMode_ClampToBorder: { return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER; }

            default:
            {
                LVN_CORE_WARN("unknown sampler address mode enum type (%u), setting mode to \'VK_SAMPLER_ADDRESS_MODE_REPEAT\' as default", mode);
                return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            }
        }
    }

    static VkPipelineColorBlendAttachmentState createColorAttachment()
    {
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        return colorBlendAttachment;
    }

} /* namespace vks */

LvnResult implVkInitGraphicsContext(LvnGraphicsContext* graphicsContext)
{
    if (s_VkBackends)
        return Lvn_Result_AlreadyCalled;

    s_VkBackends = lvn::memNew<VulkanBackends>();
    VulkanBackends* vkBackends = s_VkBackends;

    vkBackends->enableValidationLayers = graphicsContext->enableGraphicsApiDebugLogs;
    vkBackends->defaultPipelineFixedFuncs = lvn::configPipelineFixedFuncInit();
    vkBackends->maxFramesInFlight = graphicsContext->maxFramesInFlight > 0 ? graphicsContext->maxFramesInFlight : 1;

    switch (graphicsContext->frameBufferColorFormat)
    {
        case Lvn_TextureFormat_Unorm: { vkBackends->frameBufferColorFormat = VK_FORMAT_B8G8R8A8_UNORM; break; }
        case Lvn_TextureFormat_Srgb: { vkBackends->frameBufferColorFormat = VK_FORMAT_B8G8R8A8_SRGB; break; }

        default:
        {
            LVN_CORE_ERROR("[vulkan]: invalid framebuffer color format for swapchain surface creation: (%u)", graphicsContext->frameBufferColorFormat);
            return Lvn_Result_Failure;
        }
    }

    // create vulkan instance
    if (vks::createVulkanInstace(vkBackends, graphicsContext->enableGraphicsApiDebugLogs) != Lvn_Result_Success)
    {
        LVN_CORE_ERROR("[vulkan] failed to create vulkan instance when creating graphics context");
        return Lvn_Result_Failure;
    }

    if (vkBackends->enableValidationLayers && !vks::checkValidationLayerSupport())
        LVN_CORE_WARN("vulkan validation layers enabled but are not available");
    else if (vkBackends->enableValidationLayers && vks::setupDebugMessenger(vkBackends) != Lvn_Result_Success)
    {
        vkDestroyInstance(vkBackends->instance, nullptr);
        return Lvn_Result_Failure;
    }

    if (vks::setupRenderInit(vkBackends) != Lvn_Result_Success)
    {
        if (vkBackends->enableValidationLayers)
            vks::destroyDebugUtilsMessengerEXT(vkBackends->instance, vkBackends->debugMessenger, nullptr);
        vkDestroyInstance(vkBackends->instance, nullptr);
        return Lvn_Result_Failure;
    }

    // bind function pointers
    graphicsContext->graphicsapi = Lvn_GraphicsApi_vulkan;
    graphicsContext->getPhysicalDevices = vksImplGetPhysicalDevices;
    graphicsContext->checkPhysicalDeviceSupport = vksImplCheckPhysicalDeviceSupport;
    graphicsContext->setPhysicalDevice = vksImplSetPhysicalDevice;
    graphicsContext->createShaderFromSrc = vksImplCreateShaderFromSrc;
    graphicsContext->createShaderFromBin = vksImplCreateShaderFromBin;
    graphicsContext->createShaderFromFileSrc = vksImplCreateShaderFromFileSrc;
    graphicsContext->createShaderFromFileBin = vksImplCreateShaderFromFileBin;
    graphicsContext->createDescriptorLayout = vksImplCreateDescriptorLayout;
    graphicsContext->createPipeline = vksImplCreatePipeline;
    graphicsContext->createCommandPool = vksImplCreateCommandPool;
    graphicsContext->createFrameBuffer = vksImplCreateFrameBuffer;
    graphicsContext->createBuffer = vksImplCreateBuffer;
    graphicsContext->createSampler = vksImplCreateSampler;
    graphicsContext->createTexture = vksImplCreateTexture;
    graphicsContext->createCubemap = vksImplCreateCubemap;

    graphicsContext->destroyShader = vksImplDestroyShader;
    graphicsContext->destroyDescriptorLayout = vksImplDestroyDescriptorLayout;
    graphicsContext->destroyPipeline = vksImplDestroyPipeline;
    graphicsContext->destroyCommandPool = vksImplDestroyCommandPool;
    graphicsContext->destroyFrameBuffer = vksImplDestroyFrameBuffer;
    graphicsContext->destroyBuffer = vksImplDestroyBuffer;
    graphicsContext->destroySampler = vksImplDestroySampler;
    graphicsContext->destroyTexture = vksImplDestroyTexture;
    graphicsContext->destroyCubemap = vksImplDestroyCubemap;

    graphicsContext->renderBeginNextFrame = vksImplRenderBeginNextFrame;
    graphicsContext->renderDrawSubmit = vksImplRenderDrawSubmit;
    graphicsContext->renderBeginCommandRecording = vksImplRenderBeginCommandRecording;
    graphicsContext->renderEndCommandRecording = vksImplRenderEndCommandRecording;
    graphicsContext->renderCmdDraw = vksImplRenderCmdDraw;
    graphicsContext->renderCmdDrawIndexed = vksImplRenderCmdDrawIndexed;
    graphicsContext->renderCmdDrawInstanced = vksImplRenderCmdDrawInstanced;
    graphicsContext->renderCmdDrawIndexedInstanced = vksImplRenderCmdDrawIndexedInstanced;
    graphicsContext->renderCmdSetStencilReference = vksImplRenderCmdSetStencilReference;
    graphicsContext->renderCmdSetStencilMask = vksImplRenderCmdSetStencilMask;
    graphicsContext->renderCmdBeginRenderPass = vksImplRenderCmdBeginRenderPass;
    graphicsContext->renderCmdEndRenderPass = vksImplRenderCmdEndRenderPass;
    graphicsContext->renderCmdBindPipeline = vksImplRenderCmdBindPipeline;
    graphicsContext->renderCmdBindVertexBuffer = vksImplRenderCmdBindVertexBuffer;
    graphicsContext->renderCmdBindIndexBuffer = vksImplRenderCmdBindIndexBuffer;
    graphicsContext->renderCmdBindDescriptorSets = vksImplRenderCmdBindDescriptorSets;
    graphicsContext->renderCmdBeginFrameBuffer = vksImplRenderCmdBeginFrameBuffer;
    graphicsContext->renderCmdEndFrameBuffer = vksImplRenderCmdEndFrameBuffer;

    graphicsContext->bufferUpdateData = vksImplBufferUpdateData;
    graphicsContext->bufferResize = vksImplBufferResize;
    graphicsContext->allocateCommandBuffers = vksImplAllocateCommandBuffers;
    graphicsContext->allocateDescriptorSet = vksImplAllocateDescriptorSet;
    graphicsContext->updateDescriptorSetData = vksImplUpdateDescriptorSetData;
    graphicsContext->frameBufferGetImage = vksImplFrameBufferGetImage;
    graphicsContext->frameBufferGetRenderPass = vksImplFrameBufferGetRenderPass;
    graphicsContext->framebufferResize = vksImplFrameBufferResize;
    graphicsContext->frameBufferSetClearColor = vksImplFrameBufferSetClearColor;
    graphicsContext->findSupportedDepthImageFormat = vksImplFindSupportedDepthImageFormat;
    graphicsContext->internalWindowListenEventFn = vksImplInternalWindowListenEventFn;

    return Lvn_Result_Success;
}

void implVkTerminateGraphicsContext()
{
    VulkanBackends* vkBackends = s_VkBackends;
    vkDeviceWaitIdle(vkBackends->device);

    // command pool
    vkDestroyCommandPool(vkBackends->device, vkBackends->commandPool, nullptr);

    // VmaAllocator
    vmaDestroyAllocator(vkBackends->vmaAllocator);

    // logical device
    vkDestroyDevice(vkBackends->device, nullptr);

    // debug validation layers
    if (vkBackends->enableValidationLayers)
        vks::destroyDebugUtilsMessengerEXT(vkBackends->instance, vkBackends->debugMessenger, nullptr);

    // instance
    vkDestroyInstance(vkBackends->instance, nullptr);

    lvn::memDelete<VulkanBackends>(s_VkBackends);
    s_VkBackends = nullptr;
}

void vksImplGetPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* physicalDeviceCount)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();

    // get physical devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vkBackends->instance, &deviceCount, nullptr);

    if (physicalDeviceCount != nullptr)
        *physicalDeviceCount = deviceCount;

    if (pPhysicalDevices == nullptr)
        return;

    // create vulkan physical devices
    LvnVector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(vkBackends->instance, &deviceCount, physicalDevices.data());

    vkBackends->lvnPhysicalDevices.resize(deviceCount);
    for (uint32_t i = 0; i < physicalDevices.size(); i++)
    {
        VkPhysicalDeviceProperties deviceProperties{};
        vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures{};
        vkGetPhysicalDeviceFeatures(physicalDevices[i], &deviceFeatures);

        LvnPhysicalDeviceProperties props{};
        props.type = vks::getPhysicalDeviceTypeEnum(deviceProperties.deviceType);
        props.name = LvnString(deviceProperties.deviceName);
        props.apiVersion = deviceProperties.apiVersion;
        props.driverVersion = deviceProperties.driverVersion;
        props.vendorID = deviceProperties.vendorID;

        vkBackends->lvnPhysicalDevices[i].properties = props;
        vkBackends->lvnPhysicalDevices[i].features = *reinterpret_cast<LvnPhysicalDeviceFeatures*>(&deviceFeatures);
        vkBackends->lvnPhysicalDevices[i].physicalDevice = physicalDevices[i];
    }

    *pPhysicalDevices = vkBackends->lvnPhysicalDevices.data();
}

LvnResult vksImplCheckPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();
    VkPhysicalDevice vkDevice = static_cast<VkPhysicalDevice>(physicalDevice->physicalDevice);

    if (vkDevice == VK_NULL_HANDLE)
    {
        LVN_CORE_ERROR("[vulkan]: physical device <VkPhysicalDevice> is nullptr, cannot check for physical device support");
        return Lvn_Result_Failure;
    }

    // check device extension support
    if (!vks::checkDeviceExtensionSupport(vkDevice))
    {
        LVN_CORE_ERROR("[vulkan] check physical device support, physical device does not support required extensions");
        return Lvn_Result_Failure;
    }

    return Lvn_Result_Success;
}

LvnResult vksImplSetPhysicalDevice(LvnPhysicalDevice* physicalDevice)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();
    VkPhysicalDevice vkPhysicalDevice = static_cast<VkPhysicalDevice>(physicalDevice->physicalDevice);
    return vks::setupRenderInit(vkBackends, vkPhysicalDevice);
}


LvnResult vksImplCreateShaderFromSrc(LvnShader* shader, const LvnShaderCreateInfo* createInfo)
{
    return Lvn_Result_Failure;
}

LvnResult vksImplCreateShaderFromBin(LvnShader* shader, const LvnShaderBinCreateInfo* createInfo)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();

    VkShaderModule vertShaderModule = vks::createShaderModule(vkBackends, createInfo->vertexBin, createInfo->vertexSize);
    VkShaderModule fragShaderModule = vks::createShaderModule(vkBackends, createInfo->fragmentBin, createInfo->fragmentSize);

    shader->nativeVertexShaderModule = vertShaderModule;
    shader->nativeFragmentShaderModule = fragShaderModule;

    return Lvn_Result_Success;
}

LvnResult vksImplCreateShaderFromFileSrc(LvnShader* shader, const LvnShaderCreateInfo* createInfo)
{
    return Lvn_Result_Failure;
}

LvnResult vksImplCreateShaderFromFileBin(LvnShader* shader, const LvnShaderCreateInfo* createInfo)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();

    LvnVector<uint8_t> vertbin = lvn::fileLoadBin(createInfo->vertexSrc.c_str());
    LvnVector<uint8_t> fragbin = lvn::fileLoadBin(createInfo->fragmentSrc.c_str());

    VkShaderModule vertShaderModule = vks::createShaderModule(vkBackends, vertbin.data(), vertbin.size());
    VkShaderModule fragShaderModule = vks::createShaderModule(vkBackends, fragbin.data(), fragbin.size());

    shader->nativeVertexShaderModule = vertShaderModule;
    shader->nativeFragmentShaderModule = fragShaderModule;

    return Lvn_Result_Success;
}

LvnResult vksImplCreateDescriptorLayout(LvnDescriptorLayout* descriptorLayout, const LvnDescriptorLayoutCreateInfo* createInfo)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();

    LvnVector<VkDescriptorSetLayoutBinding> layoutBindings(createInfo->descriptorBindingCount);
    LvnVector<VkDescriptorPoolSize> poolSizes(createInfo->descriptorBindingCount);

    for (uint32_t i = 0; i < createInfo->descriptorBindingCount; i++)
    {
        VkDescriptorType descriptorType = vks::getDescriptorTypeEnum(createInfo->pDescriptorBindings[i].descriptorType);

        layoutBindings[i].binding = createInfo->pDescriptorBindings[i].binding;
        layoutBindings[i].descriptorType = descriptorType;
        layoutBindings[i].descriptorCount = createInfo->pDescriptorBindings[i].descriptorCount;
        layoutBindings[i].pImmutableSamplers = nullptr;
        layoutBindings[i].stageFlags = vks::getShaderStageFlagEnum(createInfo->pDescriptorBindings[i].shaderStage);

        poolSizes[i].type = descriptorType;
        poolSizes[i].descriptorCount = createInfo->pDescriptorBindings[i].descriptorCount * createInfo->pDescriptorBindings[i].maxAllocations;
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = layoutBindings.size();
    layoutInfo.pBindings = layoutBindings.data();

    VkDescriptorSetLayout vkDescriptorLayout;
    if (vkCreateDescriptorSetLayout(vkBackends->device, &layoutInfo, nullptr, &vkDescriptorLayout) != VK_SUCCESS)
    {
        LVN_CORE_ERROR("[vulkan] failed to create descriptor set layout <VkDescriptorSetLayout> at (%p)", vkDescriptorLayout);
        return Lvn_Result_Failure;
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = createInfo->maxSets;

    VkDescriptorPool descriptorPool;

    if (vkCreateDescriptorPool(vkBackends->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
    {
        LVN_CORE_ERROR("[vulkan] failed to create descriptor pool at (%p)", descriptorPool);
        vkDestroyDescriptorSetLayout(vkBackends->device, vkDescriptorLayout, nullptr);
        return Lvn_Result_Failure;
    }

    descriptorLayout->descriptorLayout = vkDescriptorLayout;
    descriptorLayout->descriptorPool = descriptorPool;

    return Lvn_Result_Success;
}

LvnResult vksImplAllocateDescriptorSet(LvnDescriptorLayout* descriptorLayout, LvnDescriptorSet** pDescriptorSets, uint32_t count)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();
    LvnDescriptorSet* descriptorSets = *pDescriptorSets;

    VkDescriptorSetLayout vkDescriptorLayout = static_cast<VkDescriptorSetLayout>(descriptorLayout->descriptorLayout);
    VkDescriptorPool descriptorPool = static_cast<VkDescriptorPool>(descriptorLayout->descriptorPool);

    LvnVector<VkDescriptorSetLayout> layouts(count, vkDescriptorLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = layouts.size();
    allocInfo.pSetLayouts = layouts.data();

    LvnVector<VkDescriptorSet> vkDescriptorSets(count);
    if (vkAllocateDescriptorSets(vkBackends->device, &allocInfo, vkDescriptorSets.data()) != VK_SUCCESS)
    {
        LVN_CORE_ERROR("[vulkan] failed to allocate descriptor sets <VkDescriptorSet>");
        return Lvn_Result_Failure;
    }

    for (uint32_t i = 0; i < count; i++)
        descriptorSets[i].descriptorSet = vkDescriptorSets[i];

    return Lvn_Result_Success;
}

LvnResult vksImplCreatePipeline(LvnPipeline* pipeline, const LvnPipelineCreateInfo* createInfo)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();

    // shader modules
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = static_cast<VkShaderModule>(createInfo->shader->nativeVertexShaderModule);
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = static_cast<VkShaderModule>(createInfo->shader->nativeFragmentShaderModule);
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    // vertex binding descriptions & attributes
    LvnVector<VkVertexInputBindingDescription> bindingDescriptions(createInfo->vertexBindingDescriptionCount);
    LvnVector<VkVertexInputAttributeDescription> vertexAttributes(createInfo->vertexAttributeCount);

    for (uint32_t i = 0; i < createInfo->vertexBindingDescriptionCount; i++)
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = createInfo->pVertexBindingDescriptions[i].binding;
        bindingDescription.stride = createInfo->pVertexBindingDescriptions[i].stride;
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        bindingDescriptions[i] = bindingDescription;
    }

    for (uint32_t i = 0; i < createInfo->vertexAttributeCount; i++)
    {
        if (createInfo->pVertexAttributes[i].format == Lvn_AttributeFormat_Undefined)
            LVN_CORE_WARN("createPipeline(LvnPipeline**, LvnPipelineCreateInfo*) | createInfo->pVertexAttributes[%d].type is \'Lvn_VertexDataType_None\'; vertex data type is set to None, vertex input attribute format will be undefined", i);

        VkVertexInputAttributeDescription attributeDescription{};
        attributeDescription.binding = createInfo->pVertexAttributes[i].binding;
        attributeDescription.location = createInfo->pVertexAttributes[i].layout;
        attributeDescription.format = vks::getVertexAttributeFormatEnum(createInfo->pVertexAttributes[i].format);
        attributeDescription.offset = createInfo->pVertexAttributes[i].offset;

        vertexAttributes[i] = attributeDescription;
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    if (createInfo->pVertexBindingDescriptions && createInfo->vertexBindingDescriptionCount > 0)
    {
        vertexInputInfo.vertexBindingDescriptionCount = createInfo->vertexBindingDescriptionCount;
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
    }

    if (createInfo->pVertexAttributes && createInfo->vertexAttributeCount > 0)
    {
        vertexInputInfo.vertexAttributeDescriptionCount = createInfo->vertexAttributeCount;
        vertexInputInfo.pVertexAttributeDescriptions = vertexAttributes.data();
    }

    // descriptor layouts
    LvnVector<VkDescriptorSetLayout> descriptorLayouts(createInfo->descriptorLayoutCount);
    for (uint32_t i = 0; i < createInfo->descriptorLayoutCount; i++)
    {
        VkDescriptorSetLayout descriptorLayout = static_cast<VkDescriptorSetLayout>(createInfo->pDescriptorLayouts[i]->descriptorLayout);
        descriptorLayouts[i] = descriptorLayout;
    }

    // render pass
    VkRenderPass renderPass = static_cast<VkRenderPass>(createInfo->renderPass->nativeRenderPass);

    // prepare pipeline create info
    VulkanPipelineCreateData pipelineCreateData{};
    pipelineCreateData.shaderStages = shaderStages;
    pipelineCreateData.shaderStageCount = ARRAY_LEN(shaderStages);
    pipelineCreateData.vertexInputInfo = vertexInputInfo;
    pipelineCreateData.renderPass = renderPass;
    pipelineCreateData.pipelineFixedFuncs = createInfo->pipelineFixedFuncs != nullptr ? createInfo->pipelineFixedFuncs : &vkBackends->defaultPipelineFixedFuncs;
    pipelineCreateData.pDescrptorSetLayouts = descriptorLayouts.data();
    pipelineCreateData.descriptorSetLayoutCount = createInfo->descriptorLayoutCount;

    // create pipeline
    VulkanPipeline vkPipeline = vks::createVulkanPipeline(vkBackends, &pipelineCreateData);

    pipeline->nativePipeline = vkPipeline.pipeline;
    pipeline->nativePipelineLayout = vkPipeline.pipelineLayout;

    return Lvn_Result_Success;
}

LvnResult vksImplCreateCommandPool(LvnCommandPool* cmdPool)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();
    VkCommandPool vkcmdPool;

    // create command buffer pool
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = vkBackends->deviceIndices.graphicsIndex;

    if (vkCreateCommandPool(vkBackends->device, &poolInfo, nullptr, &vkcmdPool) != VK_SUCCESS)
    {
        LVN_CORE_ERROR("[vulkan] failed to create command pool for swap chain");
        return Lvn_Result_Failure;
    }

    cmdPool->commandPool = vkcmdPool;

    return Lvn_Result_Success;
}

LvnResult vksImplAllocateCommandBuffers(LvnCommandPool* cmdPool, LvnCommandBuffer** pCmdBuffers, uint32_t count)
{
    LVN_ASSERT(cmdPool, "cmdPool is nullptr");
    LVN_ASSERT(pCmdBuffers, "pCmdBuffers is nullptr");

    VulkanBackends* vkBackends = vks::getVulkanBackends();

    LvnVector<LvnCommandBuffer> cmdBuffs(count);

    // command buffer create info
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = static_cast<VkCommandPool>(cmdPool->commandPool);
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = count;

    LvnVector<VkCommandBuffer> vkcmdbuffers(count);
    if (vkAllocateCommandBuffers(vkBackends->device, &allocInfo, vkcmdbuffers.data()) != VK_SUCCESS)
    {
        LVN_CORE_ERROR("[vulkan] failed to allocate command buffers");
        return Lvn_Result_Failure;
    }

    // sync objects create info
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    bool errorOccured = false;
    for (uint32_t i = 0; i < count; i++)
    {
        cmdBuffs[i].commandBuffers = vkcmdbuffers[i];

        VkSemaphore imageAvailSemaphore;
        if (vkCreateSemaphore(vkBackends->device, &semaphoreInfo, nullptr, &imageAvailSemaphore) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] failed to create semaphore for swap chain");
            errorOccured = true;
            break;
        }

        VkFence inFlightFence;
        if (vkCreateFence(vkBackends->device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] failed to create fence for swap chain");
            errorOccured = true;
            break;
        }

        cmdBuffs[i].imageAvailableSemaphores = imageAvailSemaphore;
        cmdBuffs[i].inFlightFences = inFlightFence;
    }

    if (errorOccured)
    {
        for (uint32_t i = 0; i < cmdBuffs.size(); i++)
        {
            if (cmdBuffs[i].imageAvailableSemaphores)
            {
                VkSemaphore semaphore = static_cast<VkSemaphore>(cmdBuffs[i].imageAvailableSemaphores);
                vkDestroySemaphore(vkBackends->device, semaphore, nullptr);
            }
            if (cmdBuffs[i].inFlightFences)
            {
                VkFence fence = static_cast<VkFence>(cmdBuffs[i].inFlightFences);
                vkDestroyFence(vkBackends->device, fence, nullptr);
            }
        }

        return Lvn_Result_Failure;
    }

    cmdPool->commandBuffers.push_back(lvn::move(cmdBuffs));
    *pCmdBuffers = cmdPool->commandBuffers.back().data();

    return Lvn_Result_Success;
}

LvnResult vksImplCreateFrameBuffer(LvnFrameBuffer* frameBuffer, const LvnFrameBufferCreateInfo* createInfo)
{
    return Lvn_Result_Failure;
}

LvnResult vksImplCreateBuffer(LvnBuffer* buffer, const LvnBufferCreateInfo* createInfo)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();
    VkDeviceSize bufferSize = createInfo->size;

    VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    if (createInfo->type & Lvn_BufferType_Vertex)
        usageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if (createInfo->type & Lvn_BufferType_Index)
        usageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if (createInfo->type & Lvn_BufferType_Uniform)
        usageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    if (createInfo->type & Lvn_BufferType_Storage)
        usageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

    // if buffer is static, transfer memory to gpu
    if (createInfo->usage == Lvn_BufferUsage_Static)
    {
        VkBuffer stagingBuffer;
        VmaAllocation stagingMemory;

        VkBuffer vkBuffer;
        VmaAllocation bufferMemory;

        // create staging buffer to pass vertex data into
        vks::createBuffer(vkBackends, &stagingBuffer, &stagingMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

        if (createInfo->data)
        {
            void* data;
            vmaMapMemory(vkBackends->vmaAllocator, stagingMemory, &data);
            memcpy(data, createInfo->data, bufferSize);
            vmaUnmapMemory(vkBackends->vmaAllocator, stagingMemory);
        }

        // create the main buffer to be used
        vks::createBuffer(vkBackends, &vkBuffer, &bufferMemory, bufferSize, usageFlags, VMA_MEMORY_USAGE_GPU_ONLY);
        vks::copyBuffer(vkBackends, stagingBuffer, vkBuffer, bufferSize, 0, 0);

        vkDestroyBuffer(vkBackends->device, stagingBuffer, nullptr);
        vmaFreeMemory(vkBackends->vmaAllocator, stagingMemory);

        buffer->buffer = vkBuffer;
        buffer->bufferMemory = bufferMemory;
    }
    else // dynamic buffers will have their memory stored on the cpu
    {
        VkBuffer vkBuffer;
        VmaAllocation bufferMemory;

        vks::createBuffer(vkBackends, &vkBuffer, &bufferMemory, bufferSize, usageFlags, VMA_MEMORY_USAGE_CPU_ONLY);

        vmaMapMemory(vkBackends->vmaAllocator, bufferMemory, &buffer->bufferMap);
        if (createInfo->data)
            memcpy(buffer->bufferMap, createInfo->data, bufferSize);

        buffer->buffer = vkBuffer;
        buffer->bufferMemory = bufferMemory;
    }

    buffer->type = createInfo->type;
    buffer->usage = createInfo->usage;
    buffer->size = createInfo->size;

    return Lvn_Result_Success;
}

LvnResult vksImplCreateSampler(LvnSampler* sampler, const LvnSamplerCreateInfo* createInfo)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.minFilter = vks::getTextureFilterEnum(createInfo->minFilter);
    samplerInfo.magFilter = vks::getTextureFilterEnum(createInfo->magFilter);
    samplerInfo.addressModeU = vks::getTextureWrapModeEnum(createInfo->wrapS);
    samplerInfo.addressModeV = vks::getTextureWrapModeEnum(createInfo->wrapT);
    samplerInfo.addressModeW = vks::getTextureWrapModeEnum(createInfo->wrapR);

    if (vkBackends->deviceSupportedFeatures.samplerAnisotropy)
    {
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = vkBackends->deviceProperties.limits.maxSamplerAnisotropy;
    }
    else
    {
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 1.0f;
    }

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VkSampler textureSampler;

    if (vkCreateSampler(vkBackends->device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
    {
        LVN_CORE_ERROR("[vulkan] failed to create texture sampler <VkSampler> (%p)", textureSampler);
        return Lvn_Result_Failure;
    }

    sampler->sampler = textureSampler;

    return Lvn_Result_Success;
}

LvnResult vksImplCreateTexture(LvnTexture* texture, const LvnTextureCreateInfo* createInfo)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();

    VkBuffer stagingBuffer;
    VmaAllocation stagingBufferMemory;
    VkDeviceSize imageSize = createInfo->imageData.pixels.memsize();

    vks::createBuffer(vkBackends, &stagingBuffer, &stagingBufferMemory, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

    void* data;
    vmaMapMemory(vkBackends->vmaAllocator, stagingBufferMemory, &data);
    memcpy(data, createInfo->imageData.pixels.data(), imageSize);
    vmaUnmapMemory(vkBackends->vmaAllocator, stagingBufferMemory);

    VkFormat format = createInfo->format == Lvn_TextureFormat_Unorm ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8A8_SRGB;
    switch (createInfo->imageData.channels)
    {
        case 1: { format = createInfo->format == Lvn_TextureFormat_Unorm ? VK_FORMAT_R8_UNORM : VK_FORMAT_R8_SRGB; break; }
        case 2: { format = createInfo->format == Lvn_TextureFormat_Unorm ? VK_FORMAT_R8G8_UNORM : VK_FORMAT_R8G8_SRGB; break; }
        case 4: { format = createInfo->format == Lvn_TextureFormat_Unorm ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8A8_SRGB; break; }
    }

    // create texture image
    VkImage textureImage;
    VmaAllocation textureImageMemory;

    if (vks::createImage(vkBackends,
        &textureImage,
        &textureImageMemory,
        createInfo->imageData.width,
        createInfo->imageData.height,
        format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_SAMPLE_COUNT_1_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY) != Lvn_Result_Success)
    {
        vkDestroyBuffer(vkBackends->device, stagingBuffer, nullptr);
        vmaFreeMemory(vkBackends->vmaAllocator, stagingBufferMemory);
        LVN_CORE_ERROR("[vulkan] failed to create texture image <VkImage> for texture (%p)", texture);
        return Lvn_Result_Failure;
    }

    // transition buffer to image
    vks::transitionImageLayout(vkBackends, textureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);
    vks::copyBufferToImage(vkBackends, stagingBuffer, textureImage, createInfo->imageData.width, createInfo->imageData.height, 1);
    vks::transitionImageLayout(vkBackends, textureImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);


    // texture image view
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = textureImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(vkBackends->device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        vkDestroyImage(vkBackends->device, textureImage, nullptr);
        vkDestroyBuffer(vkBackends->device, stagingBuffer, nullptr);
        vmaFreeMemory(vkBackends->vmaAllocator, stagingBufferMemory);
        LVN_CORE_ERROR("[vulkan] failed to create texture image view <VkImageView> for texture (%p)", texture);
        return Lvn_Result_Failure;
    }

    texture->image = textureImage;
    texture->imageMemory = textureImageMemory;
    texture->imageView = imageView;
    texture->sampler = createInfo->sampler->sampler;
    texture->width = createInfo->imageData.width;
    texture->height = createInfo->imageData.height;

    vkDestroyBuffer(vkBackends->device, stagingBuffer, nullptr);
    vmaFreeMemory(vkBackends->vmaAllocator, stagingBufferMemory);

    return Lvn_Result_Success;
}

LvnResult vksImplCreateCubemap(LvnCubemap* cubemap, const LvnCubemapCreateInfo* createInfo)
{
    return Lvn_Result_Failure;
}


void vksImplDestroyShader(LvnShader* shader)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();

    VkShaderModule vertShaderModule = static_cast<VkShaderModule>(shader->nativeVertexShaderModule);
    VkShaderModule fragShaderModule = static_cast<VkShaderModule>(shader->nativeFragmentShaderModule);
    vkDestroyShaderModule(vkBackends->device, fragShaderModule, nullptr);
    vkDestroyShaderModule(vkBackends->device, vertShaderModule, nullptr);
}

void vksImplDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();

    VkDescriptorSetLayout vkDescriptorLayout = static_cast<VkDescriptorSetLayout>(descriptorLayout->descriptorLayout);
    VkDescriptorPool descriptorPool = static_cast<VkDescriptorPool>(descriptorLayout->descriptorPool);

    vkDestroyDescriptorPool(vkBackends->device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(vkBackends->device, vkDescriptorLayout, nullptr);
}

void vksImplDestroyPipeline(LvnPipeline* pipeline)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();
    vkDeviceWaitIdle(vkBackends->device);

    VkPipeline vkPipeline = static_cast<VkPipeline>(pipeline->nativePipeline);
    VkPipelineLayout vkPipelineLayout = static_cast<VkPipelineLayout>(pipeline->nativePipelineLayout);

    vkDestroyPipeline(vkBackends->device, vkPipeline, nullptr);
    vkDestroyPipelineLayout(vkBackends->device, vkPipelineLayout, nullptr);
}

void vksImplDestroyCommandPool(LvnCommandPool* cmdPool)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();
    vkDeviceWaitIdle(vkBackends->device);

    // destroy sync objects
    while (!cmdPool->commandBuffers.empty())
    {
        LvnVector<LvnCommandBuffer>& commandBuffs = cmdPool->commandBuffers.front();
        for (uint32_t i = 0; i < commandBuffs.size(); i++)
        {
            VkSemaphore imageAvailSemaphore = static_cast<VkSemaphore>(commandBuffs[i].imageAvailableSemaphores);
            VkFence inFlightFence = static_cast<VkFence>(commandBuffs[i].inFlightFences);
            vkDestroySemaphore(vkBackends->device, imageAvailSemaphore, nullptr);
            vkDestroyFence(vkBackends->device, inFlightFence, nullptr);
        }

        cmdPool->commandBuffers.pop_front();
    }

    VkCommandPool commandPool = static_cast<VkCommandPool>(cmdPool->commandPool);
    vkDestroyCommandPool(vkBackends->device, commandPool, nullptr);
}

void vksImplDestroyFrameBuffer(LvnFrameBuffer* frameBuffer)
{

}

void vksImplDestroyBuffer(LvnBuffer* buffer)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();
    vkDeviceWaitIdle(vkBackends->device);

    VkBuffer vkBuffer = static_cast<VkBuffer>(buffer->buffer);
    VmaAllocation bufferMemory = static_cast<VmaAllocation>(buffer->bufferMemory);

    if (buffer->usage != Lvn_BufferUsage_Static)
        vmaUnmapMemory(vkBackends->vmaAllocator, bufferMemory);

    vkDestroyBuffer(vkBackends->device, vkBuffer, nullptr);
    vmaFreeMemory(vkBackends->vmaAllocator, bufferMemory);
}

void vksImplDestroySampler(LvnSampler* sampler)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();
    vkDeviceWaitIdle(vkBackends->device);

    VkSampler textureSampler = static_cast<VkSampler>(sampler->sampler);
    vkDestroySampler(vkBackends->device, textureSampler, nullptr);
}

void vksImplDestroyTexture(LvnTexture* texture)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();
    vkDeviceWaitIdle(vkBackends->device);

    VkImage image = static_cast<VkImage>(texture->image);
    VmaAllocation imageMemory = static_cast<VmaAllocation>(texture->imageMemory);
    VkImageView imageView = static_cast<VkImageView>(texture->imageView);

    vkDestroyImage(vkBackends->device, image, nullptr);
    vmaFreeMemory(vkBackends->vmaAllocator, imageMemory);;
    vkDestroyImageView(vkBackends->device, imageView, nullptr);
}

void vksImplDestroyCubemap(LvnCubemap* cubemap)
{

}


void vksImplRenderBeginNextFrame(LvnWindow* window, LvnCommandBuffer* cmdBuffer)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();
    VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
    VkFence inFlightFence = static_cast<VkFence>(cmdBuffer->inFlightFences);
    VkSemaphore imageAvailSemaphore = static_cast<VkSemaphore>(cmdBuffer->imageAvailableSemaphores);

    vkWaitForFences(vkBackends->device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(vkBackends->device, 1, &inFlightFence);

    VkResult result = vkAcquireNextImageKHR(vkBackends->device, surfaceData->swapChain, UINT64_MAX, imageAvailSemaphore, VK_NULL_HANDLE, &surfaceData->imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        vks::recreateSwapChain(vkBackends, window);
        return;
    }
    LVN_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "[vulkan] failed to acquire swap chain image!");
}

void vksImplRenderDrawSubmit(LvnWindow* window, LvnCommandBuffer* cmdBuffer)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();
    VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
    VkCommandBuffer vkcmdbuffer = static_cast<VkCommandBuffer>(cmdBuffer->commandBuffers);

    VkFence inFlightFence = static_cast<VkFence>(cmdBuffer->inFlightFences);
    VkSemaphore imageAvailSemaphore = static_cast<VkSemaphore>(cmdBuffer->imageAvailableSemaphores);
    VkSemaphore renderFinishedSemaphore = static_cast<VkSemaphore>(surfaceData->renderFinishedSemaphores[surfaceData->imageIndex]);

    VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAvailSemaphore;
    submitInfo.pWaitDstStageMask = &waitStages;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderFinishedSemaphore;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkcmdbuffer;

    LVN_ASSERT(vkQueueSubmit(vkBackends->graphicsQueue, 1, &submitInfo, inFlightFence) == VK_SUCCESS, "[vulkan] failed to submit draw command buffer!");


    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &surfaceData->swapChain;
    presentInfo.pImageIndices = &surfaceData->imageIndex;
    presentInfo.pResults = nullptr;

    VkResult result = vkQueuePresentKHR(vkBackends->presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || surfaceData->frameBufferResized)
    {
        surfaceData->frameBufferResized = false;
        vks::recreateSwapChain(vkBackends, window);
    }
    else
        LVN_ASSERT(result == VK_SUCCESS, "[vulkan] failed to present swap chain image");
}

void vksImplRenderBeginCommandRecording(LvnCommandBuffer* cmdBuffer)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VkCommandBuffer vkcmdbuffer = static_cast<VkCommandBuffer>(cmdBuffer->commandBuffers);
    vkResetCommandBuffer(vkcmdbuffer, 0);
    VkResult result = vkBeginCommandBuffer(vkcmdbuffer, &beginInfo);
    LVN_ASSERT(result == VK_SUCCESS, "[vulkan] failed to begin recording command buffer!");
}

void vksImplRenderEndCommandRecording(LvnCommandBuffer* cmdBuffer)
{
    VkCommandBuffer vkcmdbuffer = static_cast<VkCommandBuffer>(cmdBuffer->commandBuffers);
    VkResult result = vkEndCommandBuffer(vkcmdbuffer);
    LVN_ASSERT(result == VK_SUCCESS, "[vulkan] failed to record command buffer!");
}

void vksImplRenderCmdDraw(LvnCommandBuffer* cmdBuffer, uint32_t vertexCount)
{
    VkCommandBuffer vkcmdbuffer = static_cast<VkCommandBuffer>(cmdBuffer->commandBuffers);
    vkCmdDraw(vkcmdbuffer, vertexCount, 1, 0, 0);
}

void vksImplRenderCmdDrawIndexed(LvnCommandBuffer* cmdBuffer, uint32_t indexCount)
{
    VkCommandBuffer vkcmdbuffer = static_cast<VkCommandBuffer>(cmdBuffer->commandBuffers);
    vkCmdDrawIndexed(vkcmdbuffer, indexCount, 1, 0, 0, 0);
}

void vksImplRenderCmdDrawInstanced(LvnCommandBuffer* cmdBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance)
{
    VkCommandBuffer vkcmdbuffer = static_cast<VkCommandBuffer>(cmdBuffer->commandBuffers);
    vkCmdDraw(vkcmdbuffer, vertexCount, instanceCount, 0, firstInstance);
}

void vksImplRenderCmdDrawIndexedInstanced(LvnCommandBuffer* cmdBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance)
{
    VkCommandBuffer vkcmdbuffer = static_cast<VkCommandBuffer>(cmdBuffer->commandBuffers);
    vkCmdDrawIndexed(vkcmdbuffer, indexCount, instanceCount, 0, 0, firstInstance);
}

void vksImplRenderCmdSetStencilReference(uint32_t reference)
{

}

void vksImplRenderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask)
{

}

void vksImplRenderCmdBeginRenderPass(LvnCommandBuffer* cmdBuffer, LvnWindow* window, float r, float g, float b, float a)
{
    LVN_ASSERT(cmdBuffer != nullptr, "command buffer is nullptr");
    LVN_ASSERT(window != nullptr, "window buffer is nullptr");

    VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
    VkCommandBuffer vkcmdbuffer = static_cast<VkCommandBuffer>(cmdBuffer->commandBuffers);

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = surfaceData->renderPass;
    renderPassInfo.framebuffer = surfaceData->frameBuffers[surfaceData->imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = surfaceData->swapChainExtent;

    VkClearValue clearColor[2];
    clearColor[0].color = {{ r, g, b, a }};
    clearColor[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = ARRAY_LEN(clearColor);
    renderPassInfo.pClearValues = clearColor;

    vkCmdBeginRenderPass(vkcmdbuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = static_cast<float>(surfaceData->swapChainExtent.height);
    viewport.width = static_cast<float>(surfaceData->swapChainExtent.width);
    viewport.height = -static_cast<float>(surfaceData->swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(vkcmdbuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = surfaceData->swapChainExtent;
    vkCmdSetScissor(vkcmdbuffer, 0, 1, &scissor);
}

void vksImplRenderCmdEndRenderPass(LvnCommandBuffer* cmdBuffer)
{
    vkCmdEndRenderPass(static_cast<VkCommandBuffer>(cmdBuffer->commandBuffers));
}

void vksImplRenderCmdBindPipeline(LvnCommandBuffer* cmdBuffer, LvnPipeline* pipeline)
{
    VkCommandBuffer vkcmdbuffer = static_cast<VkCommandBuffer>(cmdBuffer->commandBuffers);
    VkPipeline graphicsPipeline = static_cast<VkPipeline>(pipeline->nativePipeline);
    vkCmdBindPipeline(vkcmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
}

void vksImplRenderCmdBindVertexBuffer(LvnCommandBuffer* cmdBuffer, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets)
{
    VkCommandBuffer vkcmdbuffer = static_cast<VkCommandBuffer>(cmdBuffer->commandBuffers);

    LvnVector<VkBuffer> buffers(bindingCount);
    for (uint32_t i = 0; i < bindingCount; i++)
        buffers[i] = static_cast<VkBuffer>(pBuffers[i]->buffer);

    vkCmdBindVertexBuffers(vkcmdbuffer, firstBinding, bindingCount, buffers.data(), pOffsets);
}

void vksImplRenderCmdBindIndexBuffer(LvnCommandBuffer* cmdBuffer, LvnBuffer* buffer, uint64_t offset)
{
    VkCommandBuffer vkcmdbuffer = static_cast<VkCommandBuffer>(cmdBuffer->commandBuffers);
    VkBuffer indexBuffer = static_cast<VkBuffer>(buffer->buffer);

    vkCmdBindIndexBuffer(vkcmdbuffer, indexBuffer, offset, VK_INDEX_TYPE_UINT32);
}

void vksImplRenderCmdBindDescriptorSets(LvnCommandBuffer* cmdBuffer, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets)
{
    VkCommandBuffer vkcmdbuffer = static_cast<VkCommandBuffer>(cmdBuffer->commandBuffers);
    VkPipelineLayout pipelineLayout = static_cast<VkPipelineLayout>(pipeline->nativePipelineLayout);

    LvnVector<VkDescriptorSet> descriptorSets(descriptorSetCount);
    for (uint32_t i = 0; i < descriptorSetCount; i++)
        descriptorSets[i] = static_cast<VkDescriptorSet>(pDescriptorSets[i]->descriptorSet);

    vkCmdBindDescriptorSets(vkcmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, firstSetIndex, descriptorSetCount, descriptorSets.data(), 0, nullptr);
}

void vksImplRenderCmdBeginFrameBuffer(LvnCommandBuffer* cmdBuffer, LvnFrameBuffer* frameBuffer)
{

}

void vksImplRenderCmdEndFrameBuffer(LvnCommandBuffer* cmdBuffer, LvnFrameBuffer* frameBuffer)
{

}


void vksImplBufferUpdateData(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset)
{
    memcpy((uint8_t*)buffer->bufferMap + offset, data, size);
}

void vksImplBufferResize(LvnBuffer* buffer, uint64_t size)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();
    VmaAllocator vmaAllocator = vkBackends->vmaAllocator;

    VkBuffer vkBuffer = static_cast<VkBuffer>(buffer->buffer);
    VmaAllocation bufferMemory = static_cast<VmaAllocation>(buffer->bufferMemory);

    vkDestroyBuffer(vkBackends->device, vkBuffer, nullptr);
    vmaFreeMemory(vmaAllocator, bufferMemory);

    VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    if (buffer->type & Lvn_BufferType_Vertex)
        usageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if (buffer->type & Lvn_BufferType_Index)
        usageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

    vks::createBuffer(vkBackends, &vkBuffer, &bufferMemory, size, usageFlags, VMA_MEMORY_USAGE_CPU_ONLY);

    buffer->buffer = vkBuffer;
    buffer->bufferMemory = bufferMemory;
}

void vksImplUpdateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();
    VkDescriptorSet vkDescriptorSet = static_cast<VkDescriptorSet>(descriptorSet->descriptorSet);

    vkDeviceWaitIdle(vkBackends->device);

    for (uint32_t i = 0; i < count; i++)
    {
        const LvnDescriptorUpdateInfo updateInfo = pUpdateInfo[i];

        VkDescriptorBufferInfo bufferInfo{};
        LvnVector<VkDescriptorImageInfo> imageInfos(updateInfo.descriptorCount);

        if (updateInfo.descriptorType == Lvn_DescriptorType_ImageSampler ||
            updateInfo.descriptorType == Lvn_DescriptorType_ImageSamplerBindless)
        {
            for (uint32_t j = 0; j < updateInfo.descriptorCount; j++)
            {
                imageInfos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfos[j].imageView = static_cast<VkImageView>(updateInfo.pTextureInfos[j]->imageView);
                imageInfos[j].sampler = static_cast<VkSampler>(updateInfo.pTextureInfos[j]->sampler);
            }
        }

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = vkDescriptorSet;
        descriptorWrite.dstBinding = updateInfo.binding;
        descriptorWrite.dstArrayElement = updateInfo.firstIndex;
        descriptorWrite.descriptorType = vks::getDescriptorTypeEnum(updateInfo.descriptorType);
        descriptorWrite.descriptorCount = updateInfo.descriptorCount;

        // if descriptor using uniform buffers
        if (updateInfo.descriptorType == Lvn_DescriptorType_UniformBuffer || updateInfo.descriptorType == Lvn_DescriptorType_StorageBuffer)
        {
            bufferInfo.buffer = static_cast<VkBuffer>(updateInfo.bufferInfo->buffer->buffer);
            bufferInfo.offset = updateInfo.bufferInfo->offset; // offset buffer size for each frame in flight
            bufferInfo.range = updateInfo.bufferInfo->range;
            descriptorWrite.pBufferInfo = &bufferInfo;
        }

        // if descriptor using textures
        else if (updateInfo.descriptorType == Lvn_DescriptorType_ImageSampler || updateInfo.descriptorType == Lvn_DescriptorType_ImageSamplerBindless)
            descriptorWrite.pImageInfo = imageInfos.data();

        vkUpdateDescriptorSets(vkBackends->device, 1, &descriptorWrite, 0, nullptr);
    }
}

LvnTexture* vksImplFrameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex)
{
    return nullptr;
}

LvnRenderPass* vksImplFrameBufferGetRenderPass(LvnFrameBuffer* frameBuffer)
{
    return nullptr;
}

void vksImplFrameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height)
{

}

void vksImplFrameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a)
{

}

LvnDepthImageFormat vksImplFindSupportedDepthImageFormat(LvnDepthImageFormat* pDepthImageFormats, uint32_t count)
{
    return {};
}

void vksImplInternalWindowListenEventFn(LvnWindow* window, LvnEvent* event)
{
    VulkanBackends* vkBackends = vks::getVulkanBackends();

    if (event->type == Lvn_EventType_WindowCreated)
    {
        VkSurfaceKHR surface;
        vks::createVulkanSurface(vkBackends->instance, window, &surface);

        if (!vkBackends->physicalDeviceSet)
        {
            LvnVector<VkPhysicalDevice> physicalDevices = vks::getPhysicalDevices(vkBackends->instance);
            VkPhysicalDevice physicalDevice = vks::getBestPhysicalDevice(vkBackends->instance, physicalDevices, surface);
            vks::setupRenderInit(vkBackends, physicalDevice, surface);
            vkBackends->physicalDeviceSet = true;
        }

        vks::createVulkanWindowSurfaceData(window, surface);
    }
    else if (event->type == Lvn_EventType_WindowDestroy)
        vks::destroyVulkanWindowSurfaceData(window);

    if (event->type == Lvn_EventType_WindowFramebufferResize)
        vks::recreateSwapChain(vkBackends, window);
}

} /* namespace lvn */
