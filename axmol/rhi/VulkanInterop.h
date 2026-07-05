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

#include "axmol/platform/PlatformMacros.h"

#include <glad/vulkan.h>

#include <cstdint>
#include <string>
#include <vector>

namespace ax::rhi
{

/**
 * @brief Engine-owned Vulkan objects exposed to an external interop system.
 *
 * This is typically used by systems such as OpenXR that need to create
 * sessions from the same VkInstance/VkPhysicalDevice/VkDevice selected by
 * the engine.
 *
 * VulkanInterop does not own these handles.
 */
struct VulkanBinding
{
    VkInstance instance{VK_NULL_HANDLE};
    VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
    VkDevice device{VK_NULL_HANDLE};

    VkQueue graphicsQueue{VK_NULL_HANDLE};
    uint32_t graphicsQueueFamily{UINT32_MAX};
    uint32_t graphicsQueueIndex{0};
};

/**
 * @brief Helper for building the VkDeviceCreateInfo::pNext feature chain.
 *
 * The feature structs themselves must outlive vkCreateDevice(). This helper
 * only links externally-owned feature structs into a pNext chain.
 */
class VulkanFeatureChain
{
public:
    VulkanFeatureChain() = default;

    VulkanFeatureChain(const VulkanFeatureChain&)            = delete;
    VulkanFeatureChain& operator=(const VulkanFeatureChain&) = delete;

    template <class FeatureT>
    void push(FeatureT& feature)
    {
        feature.pNext = _head;
        _head         = &feature;
    }

    void* head() const { return _head; }

private:
    void* _head{nullptr};
};

/**
 * @brief Optional Vulkan initialization hook for external graphics interop.
 *
 * VulkanInterop is intentionally Vulkan-only and does not depend on OpenXR or
 * any other external runtime. Implementations may live in higher layers, such
 * as platform/openxr, and register themselves before the Vulkan driver is
 * initialized.
 *
 * Typical responsibilities:
 * - append required VkInstance extensions before vkCreateInstance();
 * - constrain/select the VkPhysicalDevice before vkCreateDevice();
 * - append required VkDevice extensions before vkCreateDevice();
 * - enable required device features before vkCreateDevice();
 * - receive the final engine-owned Vulkan binding after device creation.
 */
class AX_DLL VulkanInterop
{
public:
    virtual ~VulkanInterop() = default;

    /**
     * @brief Appends required Vulkan instance extensions.
     *
     * Called before vkCreateInstance().
     *
     * The implementation should append extension names only. The caller is
     * responsible for deduplication and conversion to const char* arrays.
     */
    virtual void collectInstanceExtensions(std::vector<std::string>& extensions) {}

    /**
     * @brief Selects or constrains the physical device.
     *
     * Called after vkCreateInstance() and vkEnumeratePhysicalDevices(), before
     * the engine applies its normal adapter selection.
     *
     * Return VK_NULL_HANDLE to let the Vulkan driver use its default adapter
     * selection logic.
     */
    virtual VkPhysicalDevice selectPhysicalDevice(VkInstance instance,
                                                  const VkPhysicalDevice* devices,
                                                  uint32_t deviceCount)
    {
        return VK_NULL_HANDLE;
    }

    /**
     * @brief Appends required Vulkan device extensions.
     *
     * Called after the final VkPhysicalDevice has been selected, before
     * vkCreateDevice().
     *
     * The implementation should append extension names only. The caller is
     * responsible for deduplication and conversion to const char* arrays.
     */
    virtual void collectDeviceExtensions(VkPhysicalDevice physicalDevice,
                                         std::vector<std::string>& extensions)
    {
    }

    /**
     * @brief Enables required or preferred device features.
     *
     * Called before vkCreateDevice().
     *
     * Implementations may:
     * - set fields on enabledFeatures for Vulkan 1.0 core features;
     * - push externally-owned feature structs into featureChain.
     *
     * Feature structs pushed into featureChain must remain alive until
     * vkCreateDevice() returns.
     */
    virtual void configureDeviceFeatures(VkPhysicalDevice physicalDevice,
                                         VkPhysicalDeviceFeatures& enabledFeatures,
                                         VulkanFeatureChain& featureChain)
    {
    }

    /**
     * @brief Receives the final Vulkan objects created and owned by the engine.
     *
     * Called after vkCreateDevice() and vkGetDeviceQueue().
     *
     * The implementation must not destroy these handles.
     */
    virtual void setVulkanBinding(const VulkanBinding& binding) {}

    /**
     * @brief Called before the engine destroys the Vulkan device/instance.
     *
     * Implementations should drop cached Vulkan handles here.
     */
    virtual void clearVulkanBinding() {}
};

}  // namespace ax::rhi
