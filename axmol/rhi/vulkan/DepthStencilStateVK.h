/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once
#include "axmol/rhi/DepthStencilState.h"
#include <glad/vulkan.h>
#include <unordered_map>

namespace ax::rhi::vk
{
/**
 * @brief A Vulkan-based DepthStencilState implementation
 */
class DepthStencilStateImpl : public DepthStencilState
{
public:
    DepthStencilStateImpl();

    void update(const DepthStencilDesc& desc) override;

    uintptr_t getHash() const { return _hash; }

    // Return the Vulkan depth-stencil state create info
    const VkPipelineDepthStencilStateCreateInfo& getVkDepthStencilState() const { return _activeInfo; }

private:
    VkDevice _device{VK_NULL_HANDLE};

    uintptr_t _hash{0};

    VkPipelineDepthStencilStateCreateInfo _activeInfo{};
    VkPipelineDepthStencilStateCreateInfo _disableInfo{};
};

}  // namespace ax::rhi::vk
