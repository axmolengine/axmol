/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/Program.h"
#include "axmol/rhi/vulkan/ShaderModuleVK.h"

namespace ax::rhi::vk
{
class BufferImpl;

/**
 * @brief A Vulkan-based ShaderProgram implementation
 */
class ProgramImpl : public Program
{
public:
    ProgramImpl(Data& vsData, Data& fsData);

    /**
     * @param csData Specifies the compute shader source.
     */
    explicit ProgramImpl(Data& csData);

    ~ProgramImpl() override;

    // Vulkan specific: return VkShaderModule handles
    VkShaderModule getNativeVSModule() const { return static_cast<ShaderModuleImpl*>(_vsModule)->internalHandle(); }
    VkShaderModule getNativeFSModule() const { return static_cast<ShaderModuleImpl*>(_fsModule)->internalHandle(); }
    VkShaderModule getNativeCSModule() const { return static_cast<ShaderModuleImpl*>(_csModule)->internalHandle(); }
};

}  // namespace ax::rhi::vk
