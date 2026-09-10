/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once
#include "axmol/rhi/ShaderModule.h"
#include <glad/vulkan.h>

namespace ax::rhi::vk
{
/**
 * @addtogroup _vk
 * @{
 */

/**
 * @brief A Vulkan-based ShaderModule implementation
 *
 */
class ShaderModuleImpl : public ShaderModule
{
public:
    ShaderModuleImpl(VkDevice device, ShaderStage stage, Data& chunk);
    ~ShaderModuleImpl();

    VkShaderModule internalHandle() const { return _shader; }

private:
    void compileShader(VkDevice device);
    VkShaderModule _shader = VK_NULL_HANDLE;
};

/** @} */

}  // namespace ax::rhi::vk
