/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/VertexLayout.h"
#include "axmol/rhi/RHITypes.h"
#include <glad/vulkan.h>
#include <vector>

namespace ax::rhi::vk
{
/**
 * @brief A Vulkan-based VertexLayout implementation
 */
class VertexLayoutImpl : public VertexLayout
{
public:
    explicit VertexLayoutImpl(VertexLayoutDesc&&);
    ~VertexLayoutImpl() override = default;

    /**
     * @brief Get Vulkan vertex input state create info
     */
    const VkPipelineVertexInputStateCreateInfo& getVkCreateInfo() const { return _vkCreateInfo; }

private:
    std::vector<VkVertexInputBindingDescription> _bindings;
    std::vector<VkVertexInputAttributeDescription> _attributes;
    VkPipelineVertexInputStateCreateInfo _vkCreateInfo{};
};

}  // namespace ax::rhi::vk
