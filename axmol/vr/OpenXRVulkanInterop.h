/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
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
