/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "axmol/rhi/vulkan/GraphicsDeviceVK.h"
#include "axmol/rhi/vulkan/GraphicsContextVK.h"
#include "axmol/rhi/vulkan/BufferVK.h"
#include "axmol/rhi/vulkan/TextureVK.h"
#include "axmol/rhi/vulkan/ProgramVK.h"
#include "axmol/rhi/vulkan/ShaderModuleVK.h"
#include "axmol/rhi/vulkan/RenderTargetVK.h"
#include "axmol/rhi/vulkan/RenderPipelineVK.h"
#include "axmol/rhi/vulkan/ComputePipelineVK.h"
#include "axmol/rhi/vulkan/DepthStencilStateVK.h"
#include "axmol/rhi/vulkan/VertexLayoutVK.h"
#include "axmol/rhi/vulkan/UtilsVK.h"
#include "axmol/rhi/GraphicsCore.h"
#include "axmol/rhi/GraphicsDeviceFactory.h"
#include "axmol/rhi/RHIUtils.h"
#include "axmol/rhi/VulkanInterop.h"
#include "axmol/tlx/hash.hpp"
#include "axmol/tlx/utility.hpp"
#include "axmol/base/Logging.h"

#include <algorithm>
#include <vector>
#include <limits>

namespace ax::rhi
{
std::unique_ptr<GraphicsDevice> VulkanGraphicsDeviceFactory::create()
{
    return std::make_unique<vk::GraphicsDeviceImpl>();
}
}  // namespace ax::rhi

namespace ax::rhi::vk
{
namespace
{

static bool hasExtensionName(const tlx::pod_vector<const char*>& extensions, std::string_view name)
{
    for (auto extension : extensions)
    {
        if (extension == name)
            return true;
    }
    return false;
}

static uint32_t findGraphicsQueueFamily(VkPhysicalDevice physicalDevice)
{
    uint32_t qCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &qCount, nullptr);
    std::vector<VkQueueFamilyProperties> qprops(qCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &qCount, qprops.data());

    // Compute work is deliberately submitted on the graphics queue. Prefer a
    // family that supports both so the regular renderer still works on devices
    // whose first graphics family is graphics-only.
    for (uint32_t i = 0; i < qCount; ++i)
    {
        constexpr auto required = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
        if (qprops[i].queueCount > 0 && (qprops[i].queueFlags & required) == required)
            return i;
    }

    for (uint32_t i = 0; i < qCount; ++i)
    {
        if (qprops[i].queueCount > 0 && (qprops[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
            return i;
    }

    return UINT32_MAX;
}

static bool isValidationLayerAvailable(const char* layerName)
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const auto& layer : availableLayers)
    {
        if (strcmp(layer.layerName, layerName) == 0)
            return true;
    }
    return false;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                      VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                      void* pUserData)
{
    const char* subTag = "vulkan";

    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    {
        AXLOGD("[{}] {}", subTag, pCallbackData->pMessage);
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        AXLOGI("[{}] {}", subTag, pCallbackData->pMessage);
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        AXLOGW("[{}] {}", subTag, pCallbackData->pMessage);
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        AXLOGE("[{}] {}", subTag, pCallbackData->pMessage);
    }
    return VK_FALSE;
}

static std::pair<VkPhysicalDevice, uint32_t> resolveAdapter(const tlx::pod_vector<VkPhysicalDevice>& devices,
                                                            VkInstance instance,
                                                            PowerPreference pref)
{
    VkPhysicalDevice bestDevice      = VK_NULL_HANDLE;
    uint32_t bestGraphicsQueueFamily = UINT32_MAX;
    int bestScore                    = -1;

    for (auto index = 0; index < devices.size(); ++index)
    {
        auto pd = devices[index];
        VkPhysicalDeviceProperties props{};
        vkGetPhysicalDeviceProperties(pd, &props);

        uint32_t graphicsQueueFamily = findGraphicsQueueFamily(pd);
        if (graphicsQueueFamily == UINT32_MAX)
            continue;  // skip devices without graphics queue

        // --- Score device ---
        int score = 0;

        // Power preference
        switch (pref)
        {
        case PowerPreference::HighPerformance:
            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                score += 100;
                // Prefer newer Vulkan versions
                score += static_cast<int>(props.apiVersion);

                // Prefer larger VRAM (optional)
                VkPhysicalDeviceMemoryProperties memProps{};
                vkGetPhysicalDeviceMemoryProperties(pd, &memProps);
                VkDeviceSize vram = 0;
                for (uint32_t i = 0; i < memProps.memoryHeapCount; ++i)
                {
                    if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                        vram += memProps.memoryHeaps[i].size;
                }
                score += static_cast<int>(vram / (1024 * 1024 * 256));  // add points per 256MB
            }
            break;
        case PowerPreference::LowPower:
            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
                score += 100;
            break;
        case PowerPreference::Auto:
            score += (100 - index);
            break;
        }

        // --- Select best ---
        if (score > bestScore)
        {
            bestScore               = score;
            bestDevice              = pd;
            bestGraphicsQueueFamily = graphicsQueueFamily;
        }
    }

    return {bestDevice, bestGraphicsQueueFamily};
}

// Helper: create a depth-stencil image as TextureImpl
TextureImpl* createDepthStencilAttachment(GraphicsDeviceImpl* driver, const VkExtent2D& extent)
{
    TextureDesc depthDesc{};
    depthDesc.textureType  = TextureType::TEXTURE_2D;
    depthDesc.width        = static_cast<uint16_t>(extent.width);
    depthDesc.height       = static_cast<uint16_t>(extent.height);
    depthDesc.arraySize    = 1;
    depthDesc.mipLevels    = 1;
    depthDesc.pixelFormat  = PixelFormat::D24S8;
    depthDesc.textureUsage = TextureUsage::RENDER_TARGET;

    auto tex = new TextureImpl(driver, depthDesc);
    // init image, imageView
    tex->updateData(nullptr, extent.width, extent.height, 0);
    return tex;
}

}  // namespace

GraphicsDeviceImpl::GraphicsDeviceImpl() {}
GraphicsDeviceImpl::~GraphicsDeviceImpl()
{
    if (auto interop = GraphicsCore::getVulkanInterop())
        interop->clearVulkanBinding();

    AX_SAFE_RELEASE_NULL(_currentGraphicsContext);

    destroyStaleResources();

    if (_commandPool)
    {
        vkDestroyCommandPool(_device, _commandPool, nullptr);
        _commandPool = VK_NULL_HANDLE;
    }

    if (_surface)
        vkDestroySurfaceKHR(_factory, _surface, nullptr);
    if (_debugMessenger)
        vkDestroyDebugUtilsMessengerEXT(_factory, _debugMessenger, nullptr);
    if (_vmaAllocator)
        vmaDestroyAllocator(_vmaAllocator);
    if (_device)
        vkDestroyDevice(_device, nullptr);
    if (_factory)
        vkDestroyInstance(_factory, nullptr);
}

bool GraphicsDeviceImpl::init()
{
    // Load basic Vulkan functions without instance/device
    auto gladVulkanVer = gladLoaderLoadVulkan(nullptr, nullptr, nullptr);
    AXLOGI("axmol: vulkan gladVulkanVer: {}.{}", GLAD_VERSION_MAJOR(gladVulkanVer), GLAD_VERSION_MINOR(gladVulkanVer));

    VK_VERIFY_EXPR(gladVulkanVer != 0, "Vulkan is not supported on this device!");

    if (GLAD_VERSION_MAJOR(gladVulkanVer) < 1 || GLAD_VERSION_MINOR(gladVulkanVer) < 1)
    {
        AXLOGW("Axmol requires vulkan-1.1");
        return false;
    }

    if (!initializeFactory())
        return false;

    if (!initializeDevice())
        return false;

    // Load remaining Vulkan functions with instance/device
    gladLoaderLoadVulkan(_factory, _physical, _device);

    if (_debugCreateInfo.sType == VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT)
    {
        if (vkCreateDebugUtilsMessengerEXT(_factory, &_debugCreateInfo, nullptr, &_debugMessenger) != VK_SUCCESS)
        {
            AXLOGW("Vulkan validation layer not available!");
        }
    }

    // Vma allocator
    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.flags                  = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
    allocatorCreateInfo.vulkanApiVersion       = _apiVersion;
    allocatorCreateInfo.physicalDevice         = _physical;
    allocatorCreateInfo.device                 = _device;
    allocatorCreateInfo.instance               = _factory;
    allocatorCreateInfo.pVulkanFunctions       = nullptr;

    if (_vkCaps.memoryPrioritySupported)
        allocatorCreateInfo.flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT;
    VK_VERIFY(vmaCreateAllocator(&allocatorCreateInfo, &_vmaAllocator), "vmaCreateAllocator fail");

    return true;
}

bool GraphicsDeviceImpl::initializeFactory()
{
    auto& contextAttrs = Application::getContextAttrs();

    // vkEnumerateInstanceVersion is available since Vulkan 1.1; while Axmol can initialize on Android 7/8 devices
    // limited to Vulkan 1.0, that version is known to be buggy. For maximum compatibility, GLES is recommended
    // until dynamic RHI support is available.
    _apiVersion = vkEnumerateInstanceVersion ? VK_API_VERSION_1_1 : VK_API_VERSION_1_0;

    constexpr auto engineVersion = VK_MAKE_VERSION(AX_VERSION_MAJOR, AX_VERSION_MINOR, AX_VERSION_PATCH);
    VkApplicationInfo appInfo{
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName   = "axmol3",
        .applicationVersion = engineVersion,
        .pEngineName        = "axmol3",
        .engineVersion      = engineVersion,
        .apiVersion         = _apiVersion,
    };

    // Collect required extensions
    tlx::pod_vector<const char*> extensions;
    std::vector<std::string> extensionStorage;
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    extensions.push_back("VK_KHR_win32_surface");
#elif AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
    extensions.push_back("VK_KHR_android_surface");
#elif AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
    const char* waylandDisplay = getenv("WAYLAND_DISPLAY");
    const char* x11Display     = getenv("DISPLAY");
    if (waylandDisplay)
        extensions.push_back("VK_KHR_wayland_surface");
    else if (x11Display)
        extensions.push_back("VK_KHR_xcb_surface");
    else
    {
        AXLOGE("Unsupported window platform: neither WAYLAND_DISPLAY nor DISPLAY found");
        return false;
    }
#endif

    const auto shouldCreateDebugLayer =
        contextAttrs.debugLayerEnabled && isValidationLayerAvailable("VK_LAYER_KHRONOS_validation");

    if (shouldCreateDebugLayer)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    if (auto interop = GraphicsCore::getVulkanInterop())
        interop->collectInstanceExtensions(extensionStorage);

    for (const auto& name : extensionStorage)
    {
        if (!name.empty() && !hasExtensionName(extensions, name))
            extensions.push_back(name.c_str());
    }

    VkInstanceCreateInfo createInfo{};
    createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo        = &appInfo;
    createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    constexpr std::array<const char*, 1> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    if (shouldCreateDebugLayer)
    {
        _debugCreateInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        _debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        _debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        _debugCreateInfo.pfnUserCallback = vkDebugCallback;
        createInfo.enabledLayerCount     = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames   = validationLayers.data();
        createInfo.pNext                 = (VkDebugUtilsMessengerCreateInfoEXT*)&_debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext             = nullptr;
    }

    // Instance layers/extensions are platform-dependent; keep minimal for core init
    VkResult vr = vkCreateInstance(&createInfo, nullptr, &_factory);
    VK_VERIFY(vr, "vkCreateInstance failed");

    return true;
}

bool GraphicsDeviceImpl::initializeDevice()
{
    auto& contextAttrs = Application::getContextAttrs();

    // Enumerate physical devices
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(_factory, &count, nullptr);
    VK_VERIFY_EXPR(count > 0, "No Vulkan physical devices found");

    auto interop = GraphicsCore::getVulkanInterop();

    tlx::pod_vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(_factory, &count, devices.data());

    {
        VkPhysicalDevice preferredDevice = VK_NULL_HANDLE;
        if (interop)
            preferredDevice = interop->selectPhysicalDevice(_factory, devices.data(), count);

        if (preferredDevice != VK_NULL_HANDLE && count > 0 &&
            std::find(devices.begin(), devices.end(), preferredDevice) != devices.end())
        {
            _physical            = preferredDevice;
            _graphicsQueueFamily = findGraphicsQueueFamily(_physical);
            AXLOGI("axmol: Vulkan physical device selected by VulkanInterop");
        }
        else
        {
            if (preferredDevice != VK_NULL_HANDLE)
                AXLOGW(
                    "axmol: VulkanInterop returned a Vulkan physical device that is not in vkEnumeratePhysicalDevices");
            auto [physical, graphicsQueueFamily] = resolveAdapter(devices, _factory, contextAttrs.powerPreference);
            _physical                            = physical;
            _graphicsQueueFamily                 = graphicsQueueFamily;
        }
    }

    VK_VERIFY_EXPR(_physical != VK_NULL_HANDLE && _graphicsQueueFamily != UINT32_MAX, "No available GPU");

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_physical, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(_physical, &queueFamilyCount, queueFamilyProperties.data());
    _vkCaps.computeQueueSupported =
        _graphicsQueueFamily < queueFamilyProperties.size() &&
        (queueFamilyProperties[_graphicsQueueFamily].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0;

    // Enumerate available device extensions
    uint32_t extCount = 0;
    vkEnumerateDeviceExtensionProperties(_physical, nullptr, &extCount, nullptr);
    std::vector<VkExtensionProperties> availableExts(extCount);
    vkEnumerateDeviceExtensionProperties(_physical, nullptr, &extCount, availableExts.data());

    _supportedExtensions.reserve(availableExts.size());

    AXLOGI("axmol: available device extensions:");
    for (auto& ext : availableExts)
    {
        AXLOGI("    {}", ext.extensionName);
        const auto key = tlx::hash32_str(ext.extensionName);
        _supportedExtensions.insert(key);
    }

    // Helper to require extension and log availability
    tlx::pod_vector<const char*> deviceExtensions;
    std::vector<std::string> deviceExtensionStorage;

    // Always require swapchain
    VK_VERIFY_EXPR(hasExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME), "VK_KHR_swapchain extension is required");
    deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    if (interop)
        interop->collectDeviceExtensions(_physical, deviceExtensionStorage);

    for (const auto& name : deviceExtensionStorage)
    {
        if (!name.empty() && !hasExtensionName(deviceExtensions, name))
            deviceExtensions.push_back(name.c_str());
    }

    // Some generated/translated SPIR-V modules may declare
    // ShaderViewportIndexLayerEXT. Vulkan requires the matching device extension.
    if (hasExtension(VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME))
    {
        deviceExtensions.push_back(VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME);
        AXLOGI("axmol: VK_EXT_shader_viewport_index_layer extension enabled");
    }

    // Android device not support extended dynamic state
    if (hasExtension(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME))
    {
        _vkCaps.extendedDynamicStateSupported = true;
        deviceExtensions.push_back(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME);
        AXLOGI("axmol: VK_EXT_extended_dynamic_state extension enabled");
    }
    else
    {
        AXLOGW("axmol: VK_EXT_extended_dynamic_state extension not supported, fallback to baked InputAssemblyState");
    }

    if (hasExtension(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME))
    {
        _vkCaps.memoryPrioritySupported = true;
        deviceExtensions.push_back(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME);
        AXLOGI("axmol: VK_EXT_memory_priority extension enabled");
    }

    // Query device properties and capabilities
    VkPhysicalDeviceProperties2 props2{};
    props2.sType                      = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    VkPhysicalDeviceProperties& props = props2.properties;
    vkGetPhysicalDeviceProperties(_physical, &props);

    // Query device features
    VkPhysicalDeviceFeatures2 supportedFeatures2{};
    supportedFeatures2.sType                    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    VkPhysicalDeviceFeatures& supportedFeatures = supportedFeatures2.features;
    vkGetPhysicalDeviceFeatures(_physical, &supportedFeatures);
    VkPhysicalDeviceTimelineSemaphoreFeatures timelineSemaphoreFeatures{};
    timelineSemaphoreFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;

    // Timeline semaphore support.
    // Vulkan 1.2 has it in core; Vulkan 1.1 needs VK_KHR_timeline_semaphore.
    const bool vulkan12OrNewer = VK_VERSION_MAJOR(props.apiVersion) > 1 ||
                                 (VK_VERSION_MAJOR(props.apiVersion) == 1 && VK_VERSION_MINOR(props.apiVersion) >= 2);

    const bool timelineSemaphoreAvailable = vulkan12OrNewer || hasExtension(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
    if (timelineSemaphoreAvailable)
    {
        timelineSemaphoreFeatures.pNext = supportedFeatures2.pNext;
        supportedFeatures2.pNext        = &timelineSemaphoreFeatures;
    }
    if (!vulkan12OrNewer && hasExtension(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME))
    {
        deviceExtensions.push_back(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
    }

    _vendor        = RHIUtils::vendorToString(props.vendorID);
    _renderer      = props.deviceName;
    _version       = fmt::format("Vulkan-{}.{}.{}", VK_API_VERSION_MAJOR(props.apiVersion),
                                 VK_API_VERSION_MINOR(props.apiVersion), VK_API_VERSION_PATCH(props.apiVersion));
    _shaderVersion = "SPIR-V 1.x";

    _caps.maxAttributes     = static_cast<int32_t>(MAX_VERTEX_ATTRIBS);  // pipeline-defined
    _caps.maxTextureUnits   = 32;  // conservative default; descriptor count varies per layout
    _caps.maxTextureSize    = static_cast<int32_t>(props.limits.maxImageDimension2D);
    _caps.maxTexture3DSize  = static_cast<int32_t>(props.limits.maxImageDimension3D);
    _caps.maxSamplesAllowed = static_cast<int32_t>(props.limits.framebufferColorSampleCounts);
    _caps.maxComputeWorkGroupCount[0] = static_cast<int>(props.limits.maxComputeWorkGroupCount[0]);
    _caps.maxComputeWorkGroupCount[1] = static_cast<int>(props.limits.maxComputeWorkGroupCount[1]);
    _caps.maxComputeWorkGroupCount[2] = static_cast<int>(props.limits.maxComputeWorkGroupCount[2]);
    _caps.maxComputeWorkGroupSize[0] = static_cast<int>(props.limits.maxComputeWorkGroupSize[0]);
    _caps.maxComputeWorkGroupSize[1] = static_cast<int>(props.limits.maxComputeWorkGroupSize[1]);
    _caps.maxComputeWorkGroupSize[2] = static_cast<int>(props.limits.maxComputeWorkGroupSize[2]);
    _caps.maxComputeWorkGroupInvocations = static_cast<int>(props.limits.maxComputeWorkGroupInvocations);
    _caps.maxStorageBufferBindings = static_cast<int>(props.limits.maxPerStageDescriptorStorageBuffers);
    _caps.maxStorageBufferSize = static_cast<size_t>(props.limits.maxStorageBufferRange);

    // Query device properties
    // Optional: query extended dynamic state 3 properties only if extension is supported
    VkPhysicalDeviceExtendedDynamicState3PropertiesEXT dynState3Props{};
    if (hasExtension(VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME))
    {
        dynState3Props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_PROPERTIES_EXT;
        props2.pNext         = &dynState3Props;
    }

    // Choose correct function pointer
    const bool vulkan11OrNewer = VK_VERSION_MAJOR(props.apiVersion) > 1 ||
                                 (VK_VERSION_MAJOR(props.apiVersion) == 1 && VK_VERSION_MINOR(props.apiVersion) >= 1);
    if (vulkan11OrNewer)
    {
        // Vulkan 1.1+, core function
        vkGetPhysicalDeviceProperties2(_physical, &props2);
        vkGetPhysicalDeviceFeatures2(_physical, &supportedFeatures2);
    }
    else if (hasExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
    {
        // Vulkan 1.0 + extension
        vkGetPhysicalDeviceProperties2KHR(_physical, &props2);
        vkGetPhysicalDeviceFeatures2KHR(_physical, &supportedFeatures2);
    }
    else
    {
        vkGetPhysicalDeviceFeatures(_physical, &supportedFeatures);
    }

    AXLOGI("axmol: Vulkan device={}, driverVersion={}.{}", props2.properties.deviceName,
           VK_VERSION_MAJOR(props2.properties.driverVersion), VK_VERSION_MINOR(props2.properties.driverVersion));

    // Prepare feature chain
    VulkanFeatureChain featureChain;

    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT extDynState{};
    extDynState.sType                = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
    extDynState.extendedDynamicState = VK_TRUE;
    featureChain.push(extDynState);

    VkPhysicalDeviceExtendedDynamicState2FeaturesEXT extDynState2{};
    VkPhysicalDeviceExtendedDynamicState3FeaturesEXT extDynState3{};

    // Enable extended dynamic state chain only if extensions are supported
    if (dynState3Props.dynamicPrimitiveTopologyUnrestricted &&
        hasExtension(VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME) &&
        hasExtension(VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME))
    {
        deviceExtensions.push_back(VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME);
        deviceExtensions.push_back(VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME);

        _vkCaps.dynamicPrimitiveTopologyUnrestricted = true;

        extDynState2.sType                 = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT;
        extDynState2.extendedDynamicState2 = VK_TRUE;
        featureChain.push(extDynState2);

        extDynState3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT;
        featureChain.push(extDynState3);

        AXLOGI("axmol: Extended Dynamic State 2/3 enabled");
    }
    else
    {
        AXLOGW(
            "axmol: dynamicPrimitiveTopologyUnrestricted not supported or extensions missing, fallback to baked "
            "InputAssemblyState");
    }

    // Feature: Timeline Semaphore
    VkPhysicalDeviceTimelineSemaphoreFeatures enabledTimelineSemaphoreFeatures{};
    enabledTimelineSemaphoreFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;

    if (timelineSemaphoreAvailable && timelineSemaphoreFeatures.timelineSemaphore)
    {
        enabledTimelineSemaphoreFeatures.timelineSemaphore = VK_TRUE;
        featureChain.push(enabledTimelineSemaphoreFeatures);

        _vkCaps.timelineSemaphoreSupported = true;
        AXLOGI("axmol: timelineSemaphore feature enabled");
    }

    // Queue creation info
    float priority = 1.0f;
    VkDeviceQueueCreateInfo qinfo{};
    qinfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    qinfo.queueFamilyIndex = _graphicsQueueFamily;
    qinfo.queueCount       = 1;
    qinfo.pQueuePriorities = &priority;

    // Device creation info
    VkDeviceCreateInfo dinfo{};
    dinfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    dinfo.queueCreateInfoCount    = 1;
    dinfo.pQueueCreateInfos       = &qinfo;
    dinfo.pNext                   = featureChain.head();
    dinfo.enabledExtensionCount   = deviceExtensions.size();
    dinfo.ppEnabledExtensionNames = deviceExtensions.data();

    // Query device features
    AXLOGI("axmol: samplerAnisotropy supported={}", supportedFeatures.samplerAnisotropy);

    VkPhysicalDeviceFeatures enabledFeatures{};  // 1.0 core features
    if (supportedFeatures.samplerAnisotropy)
    {
        _vkCaps.samplerAnisotropySupported = true;
        enabledFeatures.samplerAnisotropy  = VK_TRUE;
    }

    if (supportedFeatures.geometryShader)
    {
        _vkCaps.geometryShaderSupported = true;
        enabledFeatures.geometryShader  = VK_TRUE;
    }

    if (interop)
        interop->configureDeviceFeatures(_physical, enabledFeatures, featureChain);

    dinfo.pNext            = featureChain.head();
    dinfo.pEnabledFeatures = &enabledFeatures;

    // Create logical device
    VkResult vr = vkCreateDevice(_physical, &dinfo, nullptr, &_device);
    VK_VERIFY(vr, "vkCreateDevice failed");

    vkGetDeviceQueue(_device, _graphicsQueueFamily, 0, &_graphicsQueue);
    VK_VERIFY_EXPR(_graphicsQueue != VK_NULL_HANDLE, "vkGetDeviceQueue graphics failed");

    if (interop)
    {
        VulkanBinding binding{};
        binding.instance            = _factory;
        binding.physicalDevice      = _physical;
        binding.device              = _device;
        binding.graphicsQueue       = _graphicsQueue;
        binding.graphicsQueueFamily = _graphicsQueueFamily;
        binding.graphicsQueueIndex  = 0;
        interop->setVulkanBinding(binding);
    }

    // Create transient command pool
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = _graphicsQueueFamily;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    vr = vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool);
    VK_VERIFY(vr, "vkCreateCommandPool failed for transient pool");

    return true;
}

bool GraphicsDeviceImpl::recreateSurface(const SurfaceCreateInfo& info)
{
    auto oldSurface = _surface;
    auto result     = info.createFunc(_factory, info.window, &_surface);
    if (result != VK_SUCCESS)
        return false;

    _surfaceInitalExtent.width  = info.width;
    _surfaceInitalExtent.height = info.height;

    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_physical, &queueCount, nullptr);
    std::vector<VkQueueFamilyProperties> qprops(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(_physical, &queueCount, qprops.data());

    _presentQueueFamily = UINT32_MAX;
    for (uint32_t i = 0; i < queueCount; ++i)
    {
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(_physical, i, _surface, &presentSupport);
        if (qprops[i].queueCount > 0 && presentSupport == VK_TRUE)
        {
            _presentQueueFamily = i;
            break;
        }
    }

    VK_REQUIRE_EXPR(_presentQueueFamily != UINT32_MAX, "No present queue family found");

    if (_presentQueueFamily == _graphicsQueueFamily)
    {
        _presentQueue = _graphicsQueue;
    }
    else
    {
        vkGetDeviceQueue(_device, _presentQueueFamily, 0, &_presentQueue);
        VK_REQUIRE_EXPR(_presentQueue != VK_NULL_HANDLE, "vkGetDeviceQueue present failed");
    }

    if (oldSurface)
        vkDestroySurfaceKHR(_factory, oldSurface, nullptr);

    return true;
}

GraphicsContext* GraphicsDeviceImpl::createGraphicsContext(SurfaceHandle surface)
{
    auto context = new GraphicsContextImpl(this, surface);
    Object::assign(_currentGraphicsContext, context);
    return context;
}

Buffer* GraphicsDeviceImpl::createBuffer(size_t size, BufferType type, BufferUsage usage, const void* initial)
{
    return new BufferImpl(this, size, type, usage, initial);
}

Buffer* GraphicsDeviceImpl::createBuffer(const BufferDesc& desc, const void* initial)
{
    return new BufferImpl(this, desc.size, desc.type, desc.usage, initial, desc.stride);
}

Texture* GraphicsDeviceImpl::createTexture(const TextureDesc& descriptor, std::optional<Color>)
{
    return new TextureImpl(this, descriptor);
}

Texture* GraphicsDeviceImpl::createTextureFromNativeHandle(const ExternalTextureDesc& descriptor)
{
    auto nativeImage = static_cast<VkImage>(descriptor.nativeTexture);
    if (nativeImage == VK_NULL_HANDLE)
        return nullptr;

    auto nativeView = static_cast<VkImageView>(descriptor.nativeTextureView);
    auto usage      = static_cast<VkImageUsageFlags>(descriptor.nativeUsage);
    auto initialLayout =
        descriptor.nativeState ? static_cast<VkImageLayout>(descriptor.nativeState) : VK_IMAGE_LAYOUT_UNDEFINED;
    auto finalLayout = descriptor.nativeFinalState ? static_cast<VkImageLayout>(descriptor.nativeFinalState)
                                                   : VK_IMAGE_LAYOUT_UNDEFINED;
    return new TextureImpl(this, nativeImage, nativeView, usage, descriptor.desc, initialLayout, finalLayout);
}

RenderTarget* GraphicsDeviceImpl::createRenderTarget(Texture* colorAttachment, Texture* depthStencilAttachment)
{
    auto rt = new RenderTargetImpl(this, false);
    rt->setColorTexture(colorAttachment);
    rt->setDepthStencilTexture(depthStencilAttachment);
    return rt;
}

DepthStencilState* GraphicsDeviceImpl::createDepthStencilState()
{
    return new DepthStencilStateImpl();
}

RenderPipeline* GraphicsDeviceImpl::createRenderPipeline()
{
    return new RenderPipelineImpl(this);
}

ComputePipeline* GraphicsDeviceImpl::createComputePipeline(Program* program)
{
    if (!program || !program->isValid() || !program->getCSModule())
        return nullptr;

    auto* pipeline = new ComputePipelineImpl(this, static_cast<ProgramImpl*>(program));
    if (!pipeline->isValid())
    {
        pipeline->release();
        return nullptr;
    }
    return pipeline;
}

Program* GraphicsDeviceImpl::createProgram(Data vsData, Data fsData)
{
    return new ProgramImpl(vsData, fsData);
}

Program* GraphicsDeviceImpl::createComputeProgram(Data csData)
{
    return new ProgramImpl(csData);
}

ShaderModule* GraphicsDeviceImpl::createShaderModule(ShaderStage stage, Data& chunk)
{
    return new ShaderModuleImpl(_device, stage, chunk);
}

SamplerHandle GraphicsDeviceImpl::createSampler(const SamplerDesc& desc)
{
    VkSamplerCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    if (desc.anisotropy > 0 && !isSamplerAnisotropySupported())
        return nullptr;

    // Filter mapping
    const bool minLinear = ((int)desc.minFilter & (int)SamplerFilter::MIN_LINEAR) != 0;
    const bool magLinear = ((int)desc.magFilter & (int)SamplerFilter::MAG_LINEAR) != 0;
    const bool mipLinear = ((int)desc.mipFilter & (int)SamplerFilter::MIP_LINEAR) != 0;

    info.magFilter  = magLinear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
    info.minFilter  = minLinear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
    info.mipmapMode = mipLinear ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;

    // Address mode mapping
    auto addrModeOf = [](SamplerAddressMode m) -> VkSamplerAddressMode {
        switch (m)
        {
        case SamplerAddressMode::REPEAT:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case SamplerAddressMode::MIRROR:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case SamplerAddressMode::CLAMP:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case SamplerAddressMode::BORDER:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        default:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        }
    };
    info.addressModeU = addrModeOf(desc.sAddressMode);
    info.addressModeV = addrModeOf(desc.tAddressMode);
    info.addressModeW = addrModeOf(desc.wAddressMode);

    // Compare func (used for shadow samplers)
    info.compareEnable = (desc.compareFunc != CompareFunc::ALWAYS && desc.compareFunc != CompareFunc::NEVER);
    auto cmpOf         = [](CompareFunc f) -> VkCompareOp {
        switch (f)
        {
        case CompareFunc::NEVER:
            return VK_COMPARE_OP_NEVER;
        case CompareFunc::LESS:
            return VK_COMPARE_OP_LESS;
        case CompareFunc::LESS_EQUAL:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareFunc::GREATER:
            return VK_COMPARE_OP_GREATER;
        case CompareFunc::GREATER_EQUAL:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case CompareFunc::EQUAL:
            return VK_COMPARE_OP_EQUAL;
        case CompareFunc::NOT_EQUAL:
            return VK_COMPARE_OP_NOT_EQUAL;
        case CompareFunc::ALWAYS:
            return VK_COMPARE_OP_ALWAYS;
        default:
            return VK_COMPARE_OP_ALWAYS;
        }
    };
    info.compareOp = cmpOf(desc.compareFunc);

    // Anisotropy
    info.anisotropyEnable = (desc.minFilter == SamplerFilter::MIN_ANISOTROPIC) ? VK_TRUE : VK_FALSE;
    info.maxAnisotropy    = static_cast<float>(std::clamp(desc.anisotropy + 1u, 1u, 16u));

    info.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    info.unnormalizedCoordinates = VK_FALSE;
    info.minLod                  = 0.0f;
    info.maxLod                  = VK_LOD_CLAMP_NONE;
    info.mipLodBias              = 0.0f;

    VkSampler sampler{};
    VkResult vr = vkCreateSampler(_device, &info, nullptr, &sampler);
    VK_REQUIRE(vr, "vkCreateSampler failed");
    return SamplerHandle(sampler);
}

void GraphicsDeviceImpl::destroySampler(SamplerHandle& h)
{  // sampler is cached, so don't need queue
    if (h)
    {
        vkDestroySampler(_device, static_cast<VkSampler>(h), nullptr);
        h = nullptr;
    }
}

VertexLayout* GraphicsDeviceImpl::createVertexLayout(VertexLayoutDesc&& desc)
{
    return new VertexLayoutImpl(std::move(desc));
}

std::string GraphicsDeviceImpl::getVendor() const
{
    return _vendor;
}
std::string GraphicsDeviceImpl::getRenderer() const
{
    return _renderer;
}
std::string GraphicsDeviceImpl::getVersion() const
{
    return _version;
}
std::string GraphicsDeviceImpl::getShaderVersion() const
{
    return _shaderVersion;
}

bool GraphicsDeviceImpl::checkForFeatureSupported(FeatureType feature)
{
    // Basic, conservative feature checks; consider querying format properties for stricter checks
    switch (feature)
    {
    case FeatureType::VAO:
    case FeatureType::VERTEX_ATTRIB_BINDING:
        return true;  // Vulkan pipelines handle vertex input layouts

    case FeatureType::DEPTH24:
    {
        VkFormatProperties fp{};
        vkGetPhysicalDeviceFormatProperties(_physical, VK_FORMAT_X8_D24_UNORM_PACK32, &fp);
        return (fp.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0;
    }

    case FeatureType::PACKED_DEPTH_STENCIL:
    {
        VkFormatProperties fp{};
        vkGetPhysicalDeviceFormatProperties(_physical, VK_FORMAT_D24_UNORM_S8_UINT, &fp);
        return (fp.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0;
    }

    case FeatureType::IMG_FORMAT_BGRA8888:
    {
        VkFormatProperties fp{};
        vkGetPhysicalDeviceFormatProperties(_physical, VK_FORMAT_B8G8R8A8_UNORM, &fp);
        return (fp.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) != 0;
    }

    case FeatureType::S3TC:
    {
        VkFormatProperties fp{};
        vkGetPhysicalDeviceFormatProperties(_physical, VK_FORMAT_BC3_UNORM_BLOCK, &fp);
        return (fp.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) != 0;
    }

    case FeatureType::ASTC:
    {
        VkFormatProperties fp{};
        vkGetPhysicalDeviceFormatProperties(_physical, VK_FORMAT_ASTC_4x4_UNORM_BLOCK, &fp);
        return (fp.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) != 0;
    }

    case FeatureType::COMPUTE_SHADER:
        return _vkCaps.computeQueueSupported && _caps.maxComputeWorkGroupInvocations > 0;

    case FeatureType::STORAGE_BUFFER:
        return _caps.maxStorageBufferBindings > 0 && _caps.maxStorageBufferSize > 0;

    case FeatureType::TEXTURE_3D:
        return _caps.maxTexture3DSize > 0;

    default:
        return false;
    }
}

uint32_t GraphicsDeviceImpl::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(_physical, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }
    VK_ABORT("failed to find suitable memory type!");
    return 0;
}

VkResult GraphicsDeviceImpl::allocateCommandBuffers(VkCommandBuffer* cmds, uint32_t count)
{  // allocate one primary command buffer from transient pool
    std::lock_guard<std::mutex> lk(_commandPoolMutex);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = _commandPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = count;

    VkResult res = vkAllocateCommandBuffers(_device, &allocInfo, cmds);
    VK_REQUIRE(res, "vkAllocateCommandBuffers failed");
    return res;
}

void GraphicsDeviceImpl::freeCommandBuffers(VkCommandBuffer* cmds, uint32_t count)
{
    std::lock_guard<std::mutex> lk(_commandPoolMutex);
    vkFreeCommandBuffers(_device, _commandPool, count, cmds);
}

IsolateSubmission GraphicsDeviceImpl::allocateIsolateSubmission()
{
    VkCommandBuffer cmd{VK_NULL_HANDLE};
    VkFence fence{VK_NULL_HANDLE};

    allocateCommandBuffers(&cmd, 1);

    constexpr VkFenceCreateInfo fenceInfo{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                          .flags = VK_FENCE_CREATE_SIGNALED_BIT};

    auto res = vkCreateFence(_device, &fenceInfo, nullptr, &fence);
    VK_REQUIRE(res, "vkCreateFence failed");

    return IsolateSubmission{cmd, fence};
}

void GraphicsDeviceImpl::freeIsolateSubmission(IsolateSubmission& submission)
{
    freeCommandBuffers(&submission.cmd, 1);
    if (submission.fence)
        vkDestroyFence(_device, submission.fence, nullptr);

    submission.cmd   = VK_NULL_HANDLE;
    submission.fence = VK_NULL_HANDLE;
}

void GraphicsDeviceImpl::beginRecordingIsolateSubmission(const IsolateSubmission& submission)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (submission.fence)
        vkResetFences(_device, 1, &submission.fence);

    auto res = vkBeginCommandBuffer(submission.cmd, &beginInfo);
    VK_REQUIRE(res, "vkBeginCommandBuffer failed");
}

void GraphicsDeviceImpl::commitIsolateSubmission(const IsolateSubmission& submission)
{
    VK_REQUIRE_EXPR(submission.cmd != VK_NULL_HANDLE, "endSingleTimeCommands called with null cmd");

    VkResult res = vkEndCommandBuffer(submission.cmd);
    VK_REQUIRE(res, "vkEndCommandBuffer failed");

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &submission.cmd;

    res = vkQueueSubmit(_graphicsQueue, 1, &submitInfo, submission.fence);
    VK_REQUIRE(res, "vkQueueSubmit failed");

    // wait for this fence (only this submission)
    if (submission.fence)
    {
        res = vkWaitForFences(_device, 1, &submission.fence, VK_TRUE, UINT64_MAX);
        VK_REQUIRE(res, "vkWaitForFences failed");
    }
}

void GraphicsDeviceImpl::destroyFramebuffer(VkFramebuffer fb)
{
    vkDestroyFramebuffer(_device, fb, nullptr);
}

void GraphicsDeviceImpl::destroyRenderPass(VkRenderPass key)
{
    if (_currentGraphicsContext)
        _currentGraphicsContext->removeCachedPipelineObjects(key);
    vkDestroyRenderPass(_device, key, nullptr);
}

void GraphicsDeviceImpl::removeCachedPipelineObjects(Program* key)
{
    if (_currentGraphicsContext)
        _currentGraphicsContext->removeCachedPipelineObjects(key);
}

void GraphicsDeviceImpl::disposeSampler(VkSampler sampler, uint64_t fenceValue)
{
    queueDisposalInternal({.type = DisposableResource::Type::Sampler, .sampler = sampler, .fenceValue = fenceValue});
}

void GraphicsDeviceImpl::disposeImage(VkImage image, uint64_t fenceValue)
{
    queueDisposalInternal({.type = DisposableResource::Type::Image, .image = image, .fenceValue = fenceValue});
}

void GraphicsDeviceImpl::disposeImageView(VkImageView view, uint64_t fenceValue)
{
    queueDisposalInternal({.type = DisposableResource::Type::ImageView, .view = view, .fenceValue = fenceValue});
}

void GraphicsDeviceImpl::disposeBuffer(VkBuffer buffer, uint64_t fenceValue)
{
    queueDisposalInternal({.type = DisposableResource::Type::Buffer, .buffer = buffer, .fenceValue = fenceValue});
}

void GraphicsDeviceImpl::disposeVmaMemory(VmaAllocation memory, uint64_t fenceValue)
{
    queueDisposalInternal({.type = DisposableResource::Type::VmaMemory, .vmaMemory = memory, .fenceValue = fenceValue});
}

void GraphicsDeviceImpl::queueDisposalInternal(DisposableResource&& disposal)
{
    _disposalQueue.emplace_back(disposal);
}

void GraphicsDeviceImpl::processDisposalQueue(uint64_t completedFenceValue)
{
    if (!_disposalQueue.empty())
    {
        for (size_t i = 0; i < _disposalQueue.size();)
        {
            auto& res = _disposalQueue[i];
            if ((res.fenceValue <= completedFenceValue))
            {
                switch (res.type)
                {
                case DisposableResource::Type::Image:
                    vkDestroyImage(_device, res.image, nullptr);
                    break;
                case DisposableResource::Type::ImageView:
                    vkDestroyImageView(_device, res.view, nullptr);
                    break;
                case DisposableResource::Type::Buffer:
                    vkDestroyBuffer(_device, res.buffer, nullptr);
                    break;
                case DisposableResource::Type::Sampler:
                    vkDestroySampler(_device, res.sampler, nullptr);
                    break;
                case DisposableResource::Type::VmaMemory:
                    vmaFreeMemory(_vmaAllocator, res.vmaMemory);
                    break;
                }

                _disposalQueue[i] = _disposalQueue.back();
                _disposalQueue.pop_back();
            }
            else
            {
                ++i;
            }
        }
    }
}

void GraphicsDeviceImpl::destroyStaleResources()
{
    if (!_disposalQueue.empty())
    {
        vkDeviceWaitIdle(_device);
        processDisposalQueue((std::numeric_limits<uint64_t>::max)());
    }
}

bool GraphicsDeviceImpl::hasExtension(std::string_view extName) const
{
    const auto key = tlx::hash32_bytes(extName.data(), extName.size());
    return _supportedExtensions.contains(key);
}

}  // namespace ax::rhi::vk
