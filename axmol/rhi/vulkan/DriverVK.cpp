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
#include "axmol/rhi/vulkan/DriverVK.h"
#include "axmol/rhi/vulkan/RenderContextVK.h"
#include "axmol/rhi/vulkan/BufferVK.h"
#include "axmol/rhi/vulkan/TextureVK.h"
#include "axmol/rhi/vulkan/ProgramVK.h"
#include "axmol/rhi/vulkan/ShaderModuleVK.h"
#include "axmol/rhi/vulkan/RenderTargetVK.h"
#include "axmol/rhi/vulkan/RenderPipelineVK.h"
#include "axmol/rhi/vulkan/DepthStencilStateVK.h"
#include "axmol/rhi/vulkan/VertexLayoutVK.h"
#include "axmol/rhi/vulkan/UtilsVK.h"
#include "axmol/rhi/RHIUtils.h"
#include "axmol/tlx/hash.hpp"
#include "axmol/base/Logging.h"

#include <algorithm>
#include <vector>
#include <limits>

namespace ax::rhi
{
DriverBase* DriverBase::getInstance()
{
    if (!_instance)
    {
        _instance = new vk::DriverImpl();
        static_cast<vk::DriverImpl*>(_instance)->init();
    }

    return _instance;
}

void DriverBase::destroyInstance()
{
    AX_SAFE_DELETE(_instance);
}
}  // namespace ax::rhi

namespace ax::rhi::vk
{
namespace
{
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

        uint32_t qCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(pd, &qCount, nullptr);
        std::vector<VkQueueFamilyProperties> qprops(qCount);
        vkGetPhysicalDeviceQueueFamilyProperties(pd, &qCount, qprops.data());

        bool hasGraphicsQueue        = false;
        uint32_t graphicsQueueFamily = UINT32_MAX;
        for (uint32_t i = 0; i < qCount; ++i)
        {
            if (qprops[i].queueCount > 0 && (qprops[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
            {
                hasGraphicsQueue    = true;
                graphicsQueueFamily = i;
                break;
            }
        }
        if (!hasGraphicsQueue)
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
TextureImpl* createDepthStencilAttachment(DriverImpl* driver, const VkExtent2D& extent)
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

DriverImpl::DriverImpl() {}
DriverImpl::~DriverImpl()
{
    AX_SAFE_RELEASE_NULL(_currentRenderContext);

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
    if (_device)
        vkDestroyDevice(_device, nullptr);
    if (_factory)
        vkDestroyInstance(_factory, nullptr);
}

void DriverImpl::init()
{
    // Load basic Vulkan functions without instance/device
    auto gladVulkanVer = gladLoaderLoadVulkan(nullptr, nullptr, nullptr);
    AXLOGI("axmol: vulkan gladVulkanVer: {}.{}", GLAD_VERSION_MAJOR(gladVulkanVer), GLAD_VERSION_MINOR(gladVulkanVer));

    VK_VERIFY(gladVulkanVer != 0, "Vulkan is not supported on this device!");

    initializeFactory();
    initializeDevice();

    // Load remaining Vulkan functions with instance/device
    gladLoaderLoadVulkan(_factory, _physical, _device);

    if (_debugCreateInfo.sType == VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT)
    {
        if (vkCreateDebugUtilsMessengerEXT(_factory, &_debugCreateInfo, nullptr, &_debugMessenger) != VK_SUCCESS)
        {
            AXLOGW("Vulkan validation layer not available!");
        }
    }
}

void DriverImpl::initializeFactory()
{
    auto& contextAttrs = Application::getContextAttrs();

    // vkEnumerateInstanceVersion is available since Vulkan 1.1; while Axmol can initialize on Android 7/8 devices
    // limited to Vulkan 1.0, that version is known to be buggy. For maximum compatibility, GLES is recommended
    // until dynamic RHI support is available.
    const uint32_t apiVersion = vkEnumerateInstanceVersion ? VK_API_VERSION_1_1 : VK_API_VERSION_1_0;

    constexpr auto engineVersion = VK_MAKE_VERSION(AX_VERSION_MAJOR, AX_VERSION_MINOR, AX_VERSION_PATCH);
    VkApplicationInfo appInfo{
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName   = "Axmol3",
        .applicationVersion = engineVersion,
        .pEngineName        = "Axmol3",
        .engineVersion      = engineVersion,
        .apiVersion         = apiVersion,
    };

    // Collect required extensions
    tlx::pod_vector<const char*> extensions;
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
        assert(false);
    }
#endif

    const auto shouldCreateDebugLayer =
        contextAttrs.debugLayerEnabled && isValidationLayerAvailable("VK_LAYER_KHRONOS_validation");

    if (shouldCreateDebugLayer)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

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
    VK_VERIFY_RESULT(vr, "vkCreateInstance failed");
}

void DriverImpl::initializeDevice()
{
    auto& contextAttrs = Application::getContextAttrs();

    // Enumerate physical devices
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(_factory, &count, nullptr);
    VK_VERIFY(count > 0, "No Vulkan physical devices found");

    tlx::pod_vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(_factory, &count, devices.data());

    auto [physical, graphicsQueueFamily] = resolveAdapter(devices, _factory, contextAttrs.powerPreference);
    VK_VERIFY(physical != VK_NULL_HANDLE && graphicsQueueFamily != UINT32_MAX, "No available GPU");
    _physical            = physical;
    _graphicsQueueFamily = graphicsQueueFamily;

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

    // Always require swapchain
    VK_VERIFY(hasExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME), "VK_KHR_swapchain extension is required");
    deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    // Android device not support extended dynamic state
    if (hasExtension(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME))
    {
        _vkCaps.extendedDynamicStateSupported = true;
        deviceExtensions.push_back(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME);
        AXLOGI("axmol: VK_EXT_extended_dynamic_state extension supported");
    }
    else
    {
        AXLOGW("axmol: VK_EXT_extended_dynamic_state extension not supported, fallback to baked InputAssemblyState");
    }

    // Query device properties and capabilities
    VkPhysicalDeviceProperties props{};
    vkGetPhysicalDeviceProperties(_physical, &props);

    _vendor        = RHIUtils::vendorToString(props.vendorID);
    _renderer      = props.deviceName;
    _version       = fmt::format("Vulkan-{}.{}.{}", VK_API_VERSION_MAJOR(props.apiVersion),
                                 VK_API_VERSION_MINOR(props.apiVersion), VK_API_VERSION_PATCH(props.apiVersion));
    _shaderVersion = "SPIR-V 1.x";

    _caps.maxAttributes     = static_cast<int32_t>(MAX_VERTEX_ATTRIBS);  // pipeline-defined
    _caps.maxTextureUnits   = 32;  // conservative default; descriptor count varies per layout
    _caps.maxTextureSize    = static_cast<int32_t>(props.limits.maxImageDimension2D);
    _caps.maxSamplesAllowed = static_cast<int32_t>(props.limits.framebufferColorSampleCounts);

    // Query device properties
    VkPhysicalDeviceProperties2 props2{};
    props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    // Optional: query extended dynamic state 3 properties only if extension is supported
    VkPhysicalDeviceExtendedDynamicState3PropertiesEXT dynState3Props{};
    if (hasExtension(VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME))
    {
        dynState3Props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_PROPERTIES_EXT;
        props2.pNext         = &dynState3Props;
    }

    // Choose correct function pointer
    if (VK_VERSION_MAJOR(props.apiVersion) > 1 ||
        (VK_VERSION_MAJOR(props.apiVersion) == 1 && VK_VERSION_MINOR(props.apiVersion) >= 1))
    {
        // Vulkan 1.1+, core function
        vkGetPhysicalDeviceProperties2(_physical, &props2);
    }
    else if (hasExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
    {
        // Vulkan 1.0 + extension
        vkGetPhysicalDeviceProperties2KHR(_physical, &props2);
    }
    else
    {
        // Fallback: Vulkan 1.0 without extension -> only vkGetPhysicalDeviceProperties available
        props2.properties = props;  // copy into props2 for consistency
    }

    AXLOGI("axmol: Vulkan device={}, driverVersion={}.{}", props2.properties.deviceName,
           VK_VERSION_MAJOR(props2.properties.driverVersion), VK_VERSION_MINOR(props2.properties.driverVersion));

    // Prepare feature chain for extended dynamic state
    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT extDynState{};
    extDynState.sType                = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
    extDynState.extendedDynamicState = VK_TRUE;

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
        extDynState.pNext                  = &extDynState2;

        extDynState3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT;
        extDynState2.pNext = &extDynState3;

        AXLOGI("axmol: Extended Dynamic State 2/3 enabled");
    }
    else
    {
        AXLOGW(
            "axmol: dynamicPrimitiveTopologyUnrestricted not supported or extensions missing, fallback to baked "
            "InputAssemblyState");
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
    dinfo.pNext                   = &extDynState;
    dinfo.enabledExtensionCount   = deviceExtensions.size();
    dinfo.ppEnabledExtensionNames = deviceExtensions.data();

    // Query device features
    VkPhysicalDeviceFeatures deviceFeatures{};
    vkGetPhysicalDeviceFeatures(_physical, &deviceFeatures);
    AXLOGI("axmol: samplerAnisotropy supported={}", deviceFeatures.samplerAnisotropy);

    if (deviceFeatures.samplerAnisotropy)
    {
        _vkCaps.samplerAnisotropySupported = true;
        memset(&deviceFeatures, 0, sizeof(deviceFeatures));
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        dinfo.pEnabledFeatures           = &deviceFeatures;
    }

    // Create logical device
    VkResult vr = vkCreateDevice(_physical, &dinfo, nullptr, &_device);
    VK_VERIFY_RESULT(vr, "vkCreateDevice failed");

    vkGetDeviceQueue(_device, _graphicsQueueFamily, 0, &_graphicsQueue);
    VK_VERIFY(_graphicsQueue != VK_NULL_HANDLE, "vkGetDeviceQueue graphics failed");

    // Create transient command pool
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = _graphicsQueueFamily;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    vr = vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool);
    VK_VERIFY_RESULT(vr, "vkCreateCommandPool failed for transient pool");
}

bool DriverImpl::recreateSurface(const SurfaceCreateInfo& info)
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

    VK_VERIFY(_presentQueueFamily != UINT32_MAX, "No present queue family found");

    if (_presentQueueFamily == _graphicsQueueFamily)
    {
        _presentQueue = _graphicsQueue;
    }
    else
    {
        vkGetDeviceQueue(_device, _presentQueueFamily, 0, &_presentQueue);
        VK_VERIFY(_presentQueue != VK_NULL_HANDLE, "vkGetDeviceQueue present failed");
    }

    if (oldSurface)
        vkDestroySurfaceKHR(_factory, oldSurface, nullptr);

    return true;
}

RenderContext* DriverImpl::createRenderContext(SurfaceHandle surface)
{
    auto context = new RenderContextImpl(this, surface);
    Object::assign(_currentRenderContext, context);
    return context;
}

Buffer* DriverImpl::createBuffer(std::size_t size, BufferType type, BufferUsage usage, const void* initial)
{
    return new BufferImpl(this, size, type, usage, initial);
}

Texture* DriverImpl::createTexture(const TextureDesc& descriptor)
{
    return new TextureImpl(this, descriptor);
}

RenderTarget* DriverImpl::createRenderTarget(Texture* colorAttachment, Texture* depthStencilAttachment)
{
    auto rt = new RenderTargetImpl(this, false);
    rt->setColorTexture(colorAttachment);
    rt->setDepthStencilTexture(depthStencilAttachment);
    return rt;
}

DepthStencilState* DriverImpl::createDepthStencilState()
{
    return new DepthStencilStateImpl();
}

RenderPipeline* DriverImpl::createRenderPipeline()
{
    return new RenderPipelineImpl(this);
}

Program* DriverImpl::createProgram(Data vsData, Data fsData)
{
    return new ProgramImpl(vsData, fsData);
}

ShaderModule* DriverImpl::createShaderModule(ShaderStage stage, Data& chunk)
{
    return new ShaderModuleImpl(_device, stage, chunk);
}

SamplerHandle DriverImpl::createSampler(const SamplerDesc& desc)
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
    VK_VERIFY_RESULT(vr, "vkCreateSampler failed");
    return SamplerHandle(sampler);
}

void DriverImpl::destroySampler(SamplerHandle& h)
{  // sampler is cached, so don't need queue
    if (h)
    {
        vkDestroySampler(_device, static_cast<VkSampler>(h), nullptr);
        h = nullptr;
    }
}

VertexLayout* DriverImpl::createVertexLayout(VertexLayoutDesc&& desc)
{
    return new VertexLayoutImpl(std::move(desc));
}

std::string DriverImpl::getVendor() const
{
    return _vendor;
}
std::string DriverImpl::getRenderer() const
{
    return _renderer;
}
std::string DriverImpl::getVersion() const
{
    return _version;
}
std::string DriverImpl::getShaderVersion() const
{
    return _shaderVersion;
}

bool DriverImpl::checkForFeatureSupported(FeatureType feature)
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

    default:
        return false;
    }
}

uint32_t DriverImpl::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(_physical, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }
    VK_VERIFY(false, "failed to find suitable memory type!");
    return 0;
}

VkResult DriverImpl::allocateCommandBuffers(VkCommandBuffer* cmds, uint32_t count)
{  // allocate one primary command buffer from transient pool
    std::lock_guard<std::mutex> lk(_commandPoolMutex);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = _commandPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = count;

    VkResult res = vkAllocateCommandBuffers(_device, &allocInfo, cmds);
    VK_VERIFY_RESULT(res, "vkAllocateCommandBuffers failed");
    return res;
}

void DriverImpl::freeCommandBuffers(VkCommandBuffer* cmds, uint32_t count)
{
    std::lock_guard<std::mutex> lk(_commandPoolMutex);
    vkFreeCommandBuffers(_device, _commandPool, count, cmds);
}

IsolateSubmission DriverImpl::allocateIsolateSubmission()
{
    VkCommandBuffer cmd{VK_NULL_HANDLE};
    VkFence fence{VK_NULL_HANDLE};

    allocateCommandBuffers(&cmd, 1);

    constexpr VkFenceCreateInfo fenceInfo{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                          .flags = VK_FENCE_CREATE_SIGNALED_BIT};

    auto res = vkCreateFence(_device, &fenceInfo, nullptr, &fence);
    VK_VERIFY_RESULT(res, "vkCreateFence failed");

    return IsolateSubmission{cmd, fence};
}

void DriverImpl::freeIsolateSubmission(IsolateSubmission& submission)
{
    freeCommandBuffers(&submission.cmd, 1);
    if (submission.fence)
        vkDestroyFence(_device, submission.fence, nullptr);

    submission.cmd   = VK_NULL_HANDLE;
    submission.fence = VK_NULL_HANDLE;
}

void DriverImpl::beginRecordingIsolateSubmission(const IsolateSubmission& submission)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (submission.fence)
        vkResetFences(_device, 1, &submission.fence);

    auto res = vkBeginCommandBuffer(submission.cmd, &beginInfo);
    VK_VERIFY_RESULT(res, "vkBeginCommandBuffer failed");
}

void DriverImpl::commitIsolateSubmission(const IsolateSubmission& submission)
{
    VK_VERIFY(submission.cmd != VK_NULL_HANDLE, "endSingleTimeCommands called with null cmd");

    VkResult res = vkEndCommandBuffer(submission.cmd);
    VK_VERIFY_RESULT(res, "vkEndCommandBuffer failed");

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &submission.cmd;

    res = vkQueueSubmit(_graphicsQueue, 1, &submitInfo, submission.fence);
    VK_VERIFY_RESULT(res, "vkQueueSubmit failed");

    // wait for this fence (only this submission)
    if (submission.fence)
    {
        res = vkWaitForFences(_device, 1, &submission.fence, VK_TRUE, UINT64_MAX);
        VK_VERIFY_RESULT(res, "vkWaitForFences failed");
    }
}

void DriverImpl::destroyFramebuffer(VkFramebuffer fb)
{
    vkDestroyFramebuffer(_device, fb, nullptr);
}

void DriverImpl::destroyRenderPass(VkRenderPass rp)
{
    vkDestroyRenderPass(_device, rp, nullptr);
    if (_currentRenderContext)
        _currentRenderContext->removeCachedPSOsByRenderPass(rp);
}

void DriverImpl::removeCachedObjectsByProgram(Program* program)
{
    if (_currentRenderContext)
        _currentRenderContext->removeCachedObjectsByProgram(program);
}

void DriverImpl::disposeSampler(VkSampler sampler, uint64_t fenceValue)
{
    queueDisposalInternal({.type = DisposableResource::Type::Sampler, .sampler = sampler, .fenceValue = fenceValue});
}
void DriverImpl::disposeImage(VkImage image, uint64_t fenceValue)
{
    queueDisposalInternal({.type = DisposableResource::Type::Image, .image = image, .fenceValue = fenceValue});
}
void DriverImpl::disposeImageView(VkImageView view, uint64_t fenceValue)
{
    queueDisposalInternal({.type = DisposableResource::Type::ImageView, .view = view, .fenceValue = fenceValue});
}
void DriverImpl::disposeBuffer(VkBuffer buffer, uint64_t fenceValue)
{
    queueDisposalInternal({.type = DisposableResource::Type::Buffer, .buffer = buffer, .fenceValue = fenceValue});
}
void DriverImpl::disposeMemory(VkDeviceMemory memory, uint64_t fenceValue)
{
    queueDisposalInternal({.type = DisposableResource::Type::Memory, .memory = memory, .fenceValue = fenceValue});
}

void DriverImpl::queueDisposalInternal(DisposableResource&& disposal)
{
    _disposalQueue.emplace_back(disposal);
}

void DriverImpl::processDisposalQueue(uint64_t completedFenceValue)
{
    if (!_disposalQueue.empty())
    {
        for (size_t i = 0; i < _disposalQueue.size();)
        {
            auto& res = _disposalQueue[i];
            if ((res.fenceValue <= completedFenceValue) != 0)
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
                case DisposableResource::Type::Memory:
                    vkFreeMemory(_device, res.memory, nullptr);
                    break;
                case DisposableResource::Type::Sampler:
                    vkDestroySampler(_device, res.sampler, nullptr);
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

void DriverImpl::destroyStaleResources()
{
    if (!_disposalQueue.empty())
    {
        vkDeviceWaitIdle(_device);
        processDisposalQueue((std::numeric_limits<uint64_t>::max)());
    }
}

bool DriverImpl::hasExtension(std::string_view extName) const
{
    const auto key = tlx::hash32_bytes(extName.data(), extName.size());
    return _supportedExtensions.contains(key);
}

}  // namespace ax::rhi::vk
