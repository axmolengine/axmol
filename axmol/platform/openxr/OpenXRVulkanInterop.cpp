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
#include "axmol/platform/openxr/OpenXRVulkanInterop.h"

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

static std::vector<std::string> splitExtensionString(const std::string& str)
{
    std::vector<std::string> result;
    const char* cursor = str.c_str();
    while (*cursor)
    {
        while (*cursor == ' ')
            ++cursor;

        const char* begin = cursor;
        while (*cursor && *cursor != ' ')
            ++cursor;

        if (cursor > begin)
            result.emplace_back(begin, cursor);
    }
    return result;
}

std::vector<std::string> OpenXRVulkanInterop::getXRVulkanExtensions(std::string_view functionName)
{
    std::vector<std::string> names;
    if (_xrInstance == XR_NULL_HANDLE)
        return names;

    PFN_xrVoidFunction proc = nullptr;
    if (XR_FAILED(xrGetInstanceProcAddr(_xrInstance, functionName.data(), &proc)) || !proc)
        return names;

    uint32_t bufferCount = 0;
    XrResult result      = XR_ERROR_FUNCTION_UNSUPPORTED;
    if (functionName == "xrGetVulkanInstanceExtensionsKHR"sv)
    {
        auto getExtensions = reinterpret_cast<PFN_xrGetVulkanInstanceExtensionsKHR>(proc);
        result = getExtensions(_xrInstance, _xrSystem, 0, &bufferCount, nullptr);
        if (XR_SUCCEEDED(result) && bufferCount > 0)
        {
            std::string buffer(bufferCount, '\0');
            result = getExtensions(_xrInstance, _xrSystem, bufferCount, &bufferCount, buffer.data());
            if (XR_SUCCEEDED(result))
                names = splitExtensionString(buffer);
        }
    }
    else if (functionName == "xrGetVulkanDeviceExtensionsKHR"sv)
    {
        auto getExtensions = reinterpret_cast<PFN_xrGetVulkanDeviceExtensionsKHR>(proc);
        result = getExtensions(_xrInstance, _xrSystem, 0, &bufferCount, nullptr);
        if (XR_SUCCEEDED(result) && bufferCount > 0)
        {
            std::string buffer(bufferCount, '\0');
            result = getExtensions(_xrInstance, _xrSystem, bufferCount, &bufferCount, buffer.data());
            if (XR_SUCCEEDED(result))
                names = splitExtensionString(buffer);
        }
    }

    if (XR_FAILED(result))
        AXLOGW("axmol: {} failed, ec:{}", functionName, static_cast<int>(result));

    return names;
}

void OpenXRVulkanInterop::collectInstanceExtensions(std::vector<std::string>& extensions)
{
    auto names = getXRVulkanExtensions("xrGetVulkanInstanceExtensionsKHR");
    extensions.insert(extensions.end(), names.begin(), names.end());
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
    const XrResult result = getGraphicsDevice(_xrInstance, _xrSystem, instance, &physicalDevice);
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
    auto names = getXRVulkanExtensions("xrGetVulkanDeviceExtensionsKHR");
    extensions.insert(extensions.end(), names.begin(), names.end());
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
