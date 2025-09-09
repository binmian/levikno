#include "lvn_vulkan_impl.h"
#include "lvn_vulkan_backends.h"
#include "lvn_graphics_internal.h"


#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#ifdef LVN_ENABLE_ASSERTS
    #define VMA_ASSERT(expr) (static_cast<bool>(expr) ? void(0) : LVN_CORE_ERROR("[VMA] " #expr))
#endif

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

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
    static VkPhysicalDevice                     getBestPhysicalDevice(VkInstance instance, const LvnVector<VkPhysicalDevice>& physicalDevices);
    static VulkanQueueFamilyIndices             findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
    static bool                                 checkDeviceExtensionSupport(VkPhysicalDevice device);
    static VulkanSwapChainSupportDetails        querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice device);
    static LvnResult                            createLogicalDevice(VulkanBackends* vkBackends, VkSurfaceKHR surface);
    static LvnResult                            setupRenderInit(VulkanBackends* vkBackends, VkPhysicalDevice physicalDevice);
    static VkFormat                             findSupportedFormat(VkPhysicalDevice physicalDevice, const VkFormat* candidates, uint32_t count, VkImageTiling tiling, VkFormatFeatureFlags features);
    static VkFormat                             findDepthFormat(VkPhysicalDevice physicalDevice);
    static bool                                 hasStencilComponent(VkFormat format);
    static LvnResult                            createRenderPass(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData, VkFormat format);
    static VkSurfaceFormatKHR                   chooseSwapSurfaceFormat(VulkanBackends* vkBackends, const VkSurfaceFormatKHR* pAvailableFormats, uint32_t count);
    static VkPresentModeKHR                     chooseSwapPresentMode(const VkPresentModeKHR* pAvailablePresentModes, uint32_t count, bool vSync);
    static VkExtent2D                           chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR* capabilities);
    static LvnResult                            createSwapChain(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData, VulkanSwapChainSupportDetails swapChainSupport, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode, VkExtent2D extent);
    static VkImageView                          createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    static void                                 createImageViews(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
    static void                                 createDepthResources(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
    static LvnResult                            createFrameBuffers(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
    static LvnResult                            createCommandBuffers(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
    static LvnResult                            createSyncObjects(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData);
    static LvnResult                            createBuffer(VulkanBackends* vkBackends, VkBuffer* buffer, VmaAllocation* bufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memUsage);
    static void                                 copyBuffer(VulkanBackends* vkBackends, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset);
    static LvnResult                            createImage(VulkanBackends* vkBackends, VkImage* image, VmaAllocation* imageMemory, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkSampleCountFlagBits samples, VmaMemoryUsage memUsage);
    static void                                 transitionImageLayout(VulkanBackends* vkBackends, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount);
    static void                                 copyBufferToImage(VulkanBackends* vkBackends, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

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
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        LvnVector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (vkBackends->enableValidationLayers)
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

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

    static VkPhysicalDevice getBestPhysicalDevice(VkInstance instance, const LvnVector<VkPhysicalDevice>& physicalDevices)
    {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        VkSurfaceKHR surface;
        GLFWwindow* glfwWindow = glfwCreateWindow(1, 1, "", nullptr, nullptr);

        if (glfwCreateWindowSurface(instance, glfwWindow, nullptr, &surface) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] check physical device support, failed to create temporary window surface at (%p) when checking physical device support", surface);
            return VK_NULL_HANDLE;
        }

        size_t bestScore = 0;
        VkPhysicalDevice bestDevice = VK_NULL_HANDLE;

        for (const auto& physicalDevice : physicalDevices)
        {
            VulkanQueueFamilyIndices queueIndices = vks::findQueueFamilies(physicalDevice, surface);

            // check queue families
            if (!queueIndices.has_graphics || !queueIndices.has_present)
                continue;

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

        vkDestroySurfaceKHR(instance, surface, nullptr);
        glfwDestroyWindow(glfwWindow);
        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

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
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport)
            {
                indices.presentIndex = i;
                indices.has_present = true;
            }

            if (indices.has_graphics && indices.has_present)
            {
                break;
            }

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
        // Find queue families
        VulkanQueueFamilyIndices queueIndices = vks::findQueueFamilies(vkBackends->physicalDevice, surface);
        vkBackends->deviceIndices = queueIndices;

        // Check queue families
        if (!queueIndices.has_graphics || !queueIndices.has_present)
        {
            LVN_CORE_ERROR("[vulkan] failed to create logical device, physical device does not support queue families needed");
            return Lvn_Result_Failure;
        }

        // Check device extension support
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

        // Create Logical Device
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

        // Get device queues
        vkGetDeviceQueue(vkBackends->device, queueIndices.presentIndex, 0, &vkBackends->presentQueue);
        vkGetDeviceQueue(vkBackends->device, queueIndices.graphicsIndex, 0, &vkBackends->graphicsQueue);

        return Lvn_Result_Success;
    }

    static LvnResult setupRenderInit(VulkanBackends* vkBackends, VkPhysicalDevice physicalDevice)
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

        vkBackends->physicalDevice = physicalDevice;

        VkPhysicalDeviceProperties physicalDeviceProperties{};
        vkGetPhysicalDeviceProperties(vkBackends->physicalDevice, &physicalDeviceProperties);
        LVN_CORE_TRACE("[vulkan] physical device (GPU) selected for rendering: \"%s\", driverVersion: (%u), apiVersion: (%u)", physicalDeviceProperties.deviceName, physicalDeviceProperties.driverVersion, physicalDeviceProperties.apiVersion);
        vkBackends->deviceProperties = physicalDeviceProperties;

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(vkBackends->physicalDevice, &supportedFeatures);
        vkBackends->deviceSupportedFeatures = supportedFeatures;

        // create dummy window and surface to get device queue indices support
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        VkSurfaceKHR surface;
        GLFWwindow* glfwWindow = glfwCreateWindow(1, 1, "", nullptr, nullptr);

        if (glfwCreateWindowSurface(vkBackends->instance, glfwWindow, nullptr, &surface) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] failed to create temporary window surface at (%p)", surface);
            vkDestroyDevice(vkBackends->device, nullptr);
            return Lvn_Result_Failure;
        }

        // create logical device once
        if (vks::createLogicalDevice(vkBackends, surface) != Lvn_Result_Success)
        {
            vkDestroySurfaceKHR(vkBackends->instance, surface, nullptr);
            glfwDestroyWindow(glfwWindow);
            glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
            vkDestroyDevice(vkBackends->device, nullptr);
            return Lvn_Result_Failure;
        }

        // get and check swap chain specs
        VulkanSwapChainSupportDetails swapChainSupport = vks::querySwapChainSupport(surface, vkBackends->physicalDevice);
        if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty())
        {
            LVN_CORE_ERROR("[vulkan] selected physical device does not have supported swap chain formats or present modes");
            vkDestroySurfaceKHR(vkBackends->instance, surface, nullptr);
            glfwDestroyWindow(glfwWindow);
            glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
            vkDestroyDevice(vkBackends->device, nullptr);
            return Lvn_Result_Failure;
        }

        // destroy dummy window and surface
        vkDestroySurfaceKHR(vkBackends->instance, surface, nullptr);
        glfwDestroyWindow(glfwWindow);
        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

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

    static VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const VkFormat* candidates, uint32_t count, VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, candidates[i], &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            {
                return candidates[i];
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            {
                return candidates[i];
            }
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
            {
                return pAvailablePresentModes[i];
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    static VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR* capabilities)
    {
        if (capabilities->currentExtent.width != UINT32_MAX)
        {
            return capabilities->currentExtent;
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        actualExtent.width = lvn::clamp(actualExtent.width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
        actualExtent.height = lvn::clamp(actualExtent.height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);

        return actualExtent;
    }

    static LvnResult createSwapChain(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData, VulkanSwapChainSupportDetails swapChainSupport, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode, VkExtent2D extent)
    {
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

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
        if (vkGetSwapchainImagesKHR(vkBackends->device, surfaceData->swapChain, &imageCount, surfaceData->swapChainImages.data()) == VK_SUCCESS)
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
        {
            surfaceData->swapChainImageViews[i] = vks::createImageView(vkBackends->device, surfaceData->swapChainImages[i], surfaceData->swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
        }
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

    static LvnResult createCommandBuffers(VulkanBackends* vkBackends, VulkanWindowSurfaceData* surfaceData)
    {
        // create command buffer pool
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = vkBackends->deviceIndices.graphicsIndex;

        if (vkCreateCommandPool(vkBackends->device, &poolInfo, nullptr, &surfaceData->commandPool) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] failed to create command pool for swap chain");
            return Lvn_Result_Failure;
        }

        // create command buffers
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = surfaceData->commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = vkBackends->maxFramesInFlight;

        surfaceData->commandBuffers.resize(vkBackends->maxFramesInFlight);
        if (vkAllocateCommandBuffers(vkBackends->device, &allocInfo, surfaceData->commandBuffers.data()) != VK_SUCCESS)
        {
            LVN_CORE_ERROR("[vulkan] failed to allocate command buffers");
            return Lvn_Result_Failure;
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

        surfaceData->imageAvailableSemaphores.resize(vkBackends->maxFramesInFlight);
        surfaceData->inFlightFences.resize(vkBackends->maxFramesInFlight);

        for (uint32_t i = 0; i < vkBackends->maxFramesInFlight; i++)
        {
            if (vkCreateSemaphore(vkBackends->device, &semaphoreInfo, nullptr, &surfaceData->imageAvailableSemaphores[i]) != VK_SUCCESS)
            {
                LVN_CORE_ERROR("[vulkan] failed to create semaphore for swap chain");
                return Lvn_Result_Failure;
            }
            if (vkCreateFence(vkBackends->device, &fenceInfo, nullptr, &surfaceData->inFlightFences[i]) != VK_SUCCESS)
            {
                LVN_CORE_ERROR("[vulkan] failed to create fence for swap chain");
                return Lvn_Result_Failure;
            }
        }

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

    static LvnMutex s_CopyBufferMutex;
    void copyBuffer(VulkanBackends* vkBackends, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset)
    {
        LvnLockGaurd lock(s_CopyBufferMutex);

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

    static LvnMutex s_TransitionImageLayoutMutex;
    static void transitionImageLayout(VulkanBackends* vkBackends, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount)
    {
        LvnLockGaurd lock(s_TransitionImageLayoutMutex);

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

    static LvnMutex s_CopyBufferToImageMutex;
    static void copyBufferToImage(VulkanBackends* vkBackends, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount)
    {
        LvnLockGaurd lock(s_CopyBufferToImageMutex);

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

} /* namespace vks */

LvnResult implVkInitGraphicsContext(LvnGraphicsContext* graphicsContext)
{
    if (s_VkBackends)
        return Lvn_Result_AlreadyCalled;

    s_VkBackends = lvn::memNew<VulkanBackends>();
    VulkanBackends* vkBackends = s_VkBackends;

    vkBackends->enableValidationLayers = graphicsContext->enableGraphicsApiDebugLogs;
    vkBackends->defaultPipelineSpecification = lvn::configPipelineFixedFuncInit();
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

    // get physical devices and setup render init
    LvnVector<VkPhysicalDevice> physicalDevices = vks::getPhysicalDevices(vkBackends->instance);
    VkPhysicalDevice physicalDevice = vks::getBestPhysicalDevice(vkBackends->instance, physicalDevices);

    if (vks::setupRenderInit(vkBackends, physicalDevice) != Lvn_Result_Success)
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
    graphicsContext->createShaderFromFileSrc = vksImplCreateShaderFromFileSrc;
    graphicsContext->createShaderFromFileBin = vksImplCreateShaderFromFileBin;
    graphicsContext->createDescriptorLayout = vksImplCreateDescriptorLayout;
    graphicsContext->createPipeline = vksImplCreatePipeline;
    graphicsContext->createFrameBuffer = vksImplCreateFrameBuffer;
    graphicsContext->createBuffer = vksImplCreateBuffer;
    graphicsContext->createSampler = vksImplCreateSampler;
    graphicsContext->createTexture = vksImplCreateTexture;
    graphicsContext->createTextureSampler = vksImplCreateTextureSampler;
    graphicsContext->createCubemap = vksImplCreateCubemap;

    graphicsContext->destroyShader = vksImplDestroyShader;
    graphicsContext->destroyDescriptorLayout = vksImplDestroyDescriptorLayout;
    graphicsContext->destroyPipeline = vksImplDestroyPipeline;
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
    graphicsContext->allocateDescriptorSet = vksImplAllocateDescriptorSet;
    graphicsContext->updateDescriptorSetData = vksImplUpdateDescriptorSetData;
    graphicsContext->frameBufferGetImage = vksImplFrameBufferGetImage;
    graphicsContext->frameBufferGetRenderPass = vksImplFrameBufferGetRenderPass;
    graphicsContext->framebufferResize = vksImplFrameBufferResize;
    graphicsContext->frameBufferSetClearColor = vksImplFrameBufferSetClearColor;
    graphicsContext->findSupportedDepthImageFormat = vksImplFindSupportedDepthImageFormat;

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
    VulkanBackends* vkBackends = s_VkBackends;

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
    VulkanBackends* vkBackends = s_VkBackends;
    VkPhysicalDevice vkDevice = static_cast<VkPhysicalDevice>(physicalDevice->physicalDevice);

    if (vkDevice == VK_NULL_HANDLE)
    {
        LVN_CORE_ERROR("[vulkan]: physical device <VkPhysicalDevice> is nullptr, cannot check for physical device support");
        return Lvn_Result_Failure;
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    VkSurfaceKHR surface;
    GLFWwindow* glfwWindow = glfwCreateWindow(1, 1, "", nullptr, nullptr);

    if (glfwCreateWindowSurface(vkBackends->instance, glfwWindow, nullptr, &surface) != VK_SUCCESS)
    {
        LVN_CORE_ERROR("[vulkan] check physical device support, failed to create temporary window surface at (%p) when checking physical device support", surface);
        return Lvn_Result_Failure;
    }

    VulkanQueueFamilyIndices queueIndices = vks::findQueueFamilies(vkDevice, surface);

    // check queue families
    if (!queueIndices.has_graphics || !queueIndices.has_present)
    {
        LVN_CORE_ERROR("[vulkan] check physical device support, physical device does not support queue families needed");
        return Lvn_Result_Failure;
    }

    // check device extension support
    if (!vks::checkDeviceExtensionSupport(vkDevice))
    {
        LVN_CORE_ERROR("[vulkan] check physical device support, physical device does not support required extensions");
        return Lvn_Result_Failure;
    }

    vkDestroySurfaceKHR(vkBackends->instance, surface, nullptr);
    glfwDestroyWindow(glfwWindow);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

    return Lvn_Result_Success;
}

LvnResult vksImplSetPhysicalDevice(LvnPhysicalDevice* physicalDevice)
{
    VulkanBackends* vkBackends = s_VkBackends;
    VkPhysicalDevice vkPhysicalDevice = static_cast<VkPhysicalDevice>(physicalDevice->physicalDevice);
    return vks::setupRenderInit(vkBackends, vkPhysicalDevice);
}

LvnResult vksImplCreateWindowSurfaceData(LvnWindow* window)
{
    return Lvn_Result_Failure;
}


LvnResult vksImplCreateShaderFromSrc(LvnShader* shader, const LvnShaderCreateInfo* createInfo)
{
    return Lvn_Result_Failure;
}

LvnResult vksImplCreateShaderFromFileSrc(LvnShader* shader, const LvnShaderCreateInfo* createInfo)
{
    return Lvn_Result_Failure;
}

LvnResult vksImplCreateShaderFromFileBin(LvnShader* shader, const LvnShaderCreateInfo* createInfo)
{
    return Lvn_Result_Failure;
}

LvnResult vksImplCreateDescriptorLayout(LvnDescriptorLayout* descriptorLayout, const LvnDescriptorLayoutCreateInfo* createInfo)
{
    return Lvn_Result_Failure;
}

LvnResult vksImplAllocateDescriptorSet(LvnDescriptorSet* descriptorSet, LvnDescriptorLayout* descriptorLayout)
{
    return Lvn_Result_Failure;
}

LvnResult vksImplCreatePipeline(LvnPipeline* pipeline, const LvnPipelineCreateInfo* createInfo)
{
    return Lvn_Result_Failure;
}

LvnResult vksImplCreateFrameBuffer(LvnFrameBuffer* frameBuffer, const LvnFrameBufferCreateInfo* createInfo)
{
    return Lvn_Result_Failure;
}

LvnResult vksImplCreateBuffer(LvnBuffer* buffer, const LvnBufferCreateInfo* createInfo)
{
    return Lvn_Result_Failure;
}

LvnResult vksImplCreateSampler(LvnSampler* sampler, const LvnSamplerCreateInfo* createInfo)
{
    return Lvn_Result_Failure;
}

LvnResult vksImplCreateTexture(LvnTexture* texture, const LvnTextureCreateInfo* createInfo)
{
    return Lvn_Result_Failure;
}

LvnResult vksImplCreateTextureSampler(LvnTexture* texture, const LvnTextureSamplerCreateInfo* createInfo)
{
    return Lvn_Result_Failure;
}

LvnResult vksImplCreateCubemap(LvnCubemap* cubemap, const LvnCubemapCreateInfo* createInfo)
{
    return Lvn_Result_Failure;
}


void vksImplDestroyShader(LvnShader* shader)
{

}

void vksImplDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout)
{

}

void vksImplDestroyPipeline(LvnPipeline* pipeline)
{

}

void vksImplDestroyFrameBuffer(LvnFrameBuffer* frameBuffer)
{

}

void vksImplDestroyBuffer(LvnBuffer* vertexArrayBuffer)
{

}

void vksImplDestroySampler(LvnSampler* sampler)
{

}

void vksImplDestroyTexture(LvnTexture* texture)
{

}

void vksImplDestroyCubemap(LvnCubemap* cubemap)
{

}


void vksImplRenderBeginNextFrame(LvnCommandBuffer* window)
{

}

void vksImplRenderDrawSubmit(LvnCommandBuffer* window)
{

}

void vksImplRenderBeginCommandRecording(LvnCommandBuffer* window)
{

}

void vksImplRenderEndCommandRecording(LvnCommandBuffer* window)
{

}

void vksImplRenderCmdDraw(LvnCommandBuffer* window, uint32_t vertexCount)
{

}

void vksImplRenderCmdDrawIndexed(LvnCommandBuffer* window, uint32_t indexCount)
{

}

void vksImplRenderCmdDrawInstanced(LvnCommandBuffer* window, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance)
{

}

void vksImplRenderCmdDrawIndexedInstanced(LvnCommandBuffer* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance)
{

}

void vksImplRenderCmdSetStencilReference(uint32_t reference)
{

}

void vksImplRenderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask)
{

}

void vksImplRenderCmdBeginRenderPass(LvnCommandBuffer* window, float r, float g, float b, float a)
{

}

void vksImplRenderCmdEndRenderPass(LvnCommandBuffer* window)
{

}

void vksImplRenderCmdBindPipeline(LvnCommandBuffer* window, LvnPipeline* pipeline)
{

}

void vksImplRenderCmdBindVertexBuffer(LvnCommandBuffer* window, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets)
{

}

void vksImplRenderCmdBindIndexBuffer(LvnCommandBuffer* window, LvnBuffer* buffer, uint64_t offset)
{

}

void vksImplRenderCmdBindDescriptorSets(LvnCommandBuffer* window, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets)
{

}

void vksImplRenderCmdBeginFrameBuffer(LvnCommandBuffer* window, LvnFrameBuffer* frameBuffer)
{

}

void vksImplRenderCmdEndFrameBuffer(LvnCommandBuffer* window, LvnFrameBuffer* frameBuffer)
{

}


void vksImplBufferUpdateData(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset)
{

}

void vksImplBufferResize(LvnBuffer* buffer, uint64_t size)
{

}

void vksImplUpdateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count)
{

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


} /* namespace lvn */
