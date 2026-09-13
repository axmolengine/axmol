/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "axmol/vr/OpenXRVulkanInterop.h"

#if defined(AX_ENABLE_OPENXR)
#    include "openxr/openxr_platform.h"
#    include "axmol/base/Logging.h"

using namespace std::string_view_literals;

namespace ax::rhi
{

void OpenXRVulkanInterop::setXrHandles(XrInstance instance, XrSystemId system)
{
    _xrInstance = instance;
    _xrSystem   = system;
}

static void splitExtensionString(const std::string& str, std::vector<std::string>& extensions)
{
    const char* cursor = str.c_str();
    while (*cursor)
    {
        while (*cursor == ' ')
            ++cursor;

        const char* begin = cursor;
        while (*cursor && *cursor != ' ')
            ++cursor;

        if (cursor > begin)
            extensions.emplace_back(begin, cursor);
    }
}

void OpenXRVulkanInterop::collectXRVulkanExtensions(std::string_view functionName, std::vector<std::string>& extensions)
{
    if (_xrInstance == XR_NULL_HANDLE)
        return;

    PFN_xrVoidFunction proc = nullptr;
    if (XR_FAILED(xrGetInstanceProcAddr(_xrInstance, functionName.data(), &proc)) || !proc)
        return;

    uint32_t bufferCount = 0;
    XrResult result      = XR_ERROR_FUNCTION_UNSUPPORTED;
    if (functionName == "xrGetVulkanInstanceExtensionsKHR"sv)
    {
        auto getExtensions = reinterpret_cast<PFN_xrGetVulkanInstanceExtensionsKHR>(proc);
        result             = getExtensions(_xrInstance, _xrSystem, 0, &bufferCount, nullptr);
        if (XR_SUCCEEDED(result) && bufferCount > 0)
        {
            std::string buffer(bufferCount, '\0');
            result = getExtensions(_xrInstance, _xrSystem, bufferCount, &bufferCount, buffer.data());
            if (XR_SUCCEEDED(result))
                splitExtensionString(buffer, extensions);
        }
    }
    else if (functionName == "xrGetVulkanDeviceExtensionsKHR"sv)
    {
        auto getExtensions = reinterpret_cast<PFN_xrGetVulkanDeviceExtensionsKHR>(proc);
        result             = getExtensions(_xrInstance, _xrSystem, 0, &bufferCount, nullptr);
        if (XR_SUCCEEDED(result) && bufferCount > 0)
        {
            std::string buffer(bufferCount, '\0');
            result = getExtensions(_xrInstance, _xrSystem, bufferCount, &bufferCount, buffer.data());
            if (XR_SUCCEEDED(result))
                splitExtensionString(buffer, extensions);
        }
    }

    if (XR_FAILED(result))
        AXLOGW("axmol: {} failed, ec:{}", functionName, static_cast<int>(result));
}

void OpenXRVulkanInterop::collectInstanceExtensions(std::vector<std::string>& extensions)
{
    collectXRVulkanExtensions("xrGetVulkanInstanceExtensionsKHR", extensions);
}

VkPhysicalDevice OpenXRVulkanInterop::selectPhysicalDevice(VkInstance instance,
                                                           const VkPhysicalDevice* /*devices*/,
                                                           uint32_t /*deviceCount*/)
{
    if (_xrInstance == XR_NULL_HANDLE)
        return VK_NULL_HANDLE;

    PFN_xrGetVulkanGraphicsDeviceKHR getGraphicsDevice = nullptr;
    if (XR_FAILED(xrGetInstanceProcAddr(_xrInstance, "xrGetVulkanGraphicsDeviceKHR",
                                        reinterpret_cast<PFN_xrVoidFunction*>(&getGraphicsDevice))) ||
        !getGraphicsDevice)
    {
        return VK_NULL_HANDLE;
    }

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    const XrResult result           = getGraphicsDevice(_xrInstance, _xrSystem, instance, &physicalDevice);
    if (XR_FAILED(result))
    {
        AXLOGW("axmol: xrGetVulkanGraphicsDeviceKHR failed, ec:{}", static_cast<int>(result));
        return VK_NULL_HANDLE;
    }

    return physicalDevice;
}

void OpenXRVulkanInterop::collectDeviceExtensions(VkPhysicalDevice /*physicalDevice*/,
                                                  std::vector<std::string>& extensions)
{
    collectXRVulkanExtensions("xrGetVulkanDeviceExtensionsKHR", extensions);
}

void OpenXRVulkanInterop::configureDeviceFeatures(VkPhysicalDevice physicalDevice,
                                                  VkPhysicalDeviceFeatures& enabledFeatures,
                                                  VulkanFeatureChain& /*featureChain*/)
{
    VkPhysicalDeviceFeatures supportedFeatures{};
    vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);
    if (supportedFeatures.geometryShader)
        enabledFeatures.geometryShader = VK_TRUE;
}

void OpenXRVulkanInterop::setVulkanBinding(const VulkanBinding& binding)
{
    _binding = binding;
}

void OpenXRVulkanInterop::clearVulkanBinding()
{
    _binding = {};
}

}  // namespace ax::rhi
#endif
