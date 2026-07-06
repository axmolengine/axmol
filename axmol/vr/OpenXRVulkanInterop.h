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
#pragma once

#if defined(AX_ENABLE_OPENXR)
#    include "axmol/rhi/VulkanInterop.h"

#    if defined(__ANDROID__)
#        define XR_USE_PLATFORM_ANDROID
#    endif
#    define XR_USE_GRAPHICS_API_VULKAN
#    include "openxr/openxr.h"

namespace ax::rhi
{

/**
 * @brief Passive adapter that bridges an externally-owned XrInstance to VulkanInterop queries.
 *
 * OpenXRDriver creates an XrInstance early and passes it via setXrHandles().
 * This class does not own any OpenXR handles and performs no lifecycle management.
 */
class OpenXRVulkanInterop final : public VulkanInterop
{
public:
    OpenXRVulkanInterop() = default;

    void setXrHandles(XrInstance instance, XrSystemId system);

    void collectInstanceExtensions(std::vector<std::string>& extensions) override;
    VkPhysicalDevice selectPhysicalDevice(VkInstance instance,
                                          const VkPhysicalDevice* devices,
                                          uint32_t deviceCount) override;
    void collectDeviceExtensions(VkPhysicalDevice physicalDevice, std::vector<std::string>& extensions) override;
    void configureDeviceFeatures(VkPhysicalDevice physicalDevice,
                                 VkPhysicalDeviceFeatures& enabledFeatures,
                                 VulkanFeatureChain& featureChain) override;
    void setVulkanBinding(const VulkanBinding& binding) override;
    void clearVulkanBinding() override;

private:
    void collectXRVulkanExtensions(std::string_view functionName, std::vector<std::string>& extensions);

    XrInstance _xrInstance{XR_NULL_HANDLE};
    XrSystemId _xrSystem{XR_NULL_SYSTEM_ID};
    VulkanBinding _binding;
};

}  // namespace ax::rhi

#endif  // AX_ENABLE_OPENXR
