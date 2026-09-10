/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "axmol/rhi/vulkan/ShaderModuleVK.h"
#include "axmol/rhi/vulkan/GraphicsDeviceVK.h"
#include "axmol/base/Logging.h"
#include <assert.h>
#include <algorithm>

namespace ax::rhi::vk
{

ShaderModuleImpl::ShaderModuleImpl(VkDevice device, ShaderStage stage, Data& chunk) : ShaderModule(stage, chunk)
{
    compileShader(device);
}

ShaderModuleImpl::~ShaderModuleImpl()
{
    if (_shader)
    {
        // Device should be accessible from a driver; here we assume destruction is handled externally.
        // If needed, store VkDevice in this class like other implementations.
        // Intentionally left minimal: the lifetime is managed by the owning renderer/driver.
        auto device = static_cast<GraphicsDeviceImpl*>(axdrv)->getDevice();
        vkDestroyShaderModule(device, _shader, nullptr);
    }
}

void ShaderModuleImpl::compileShader(VkDevice device)
{
    // Create VkShaderModule from SPIR-V bytes
    if (_codeSpan.empty())
    {
        AXLOGE("axmol: Shader code is empty.");
        assert(false);
        return;
    }

    VkShaderModuleCreateInfo smci{};
    smci.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    smci.codeSize = _codeSpan.size();
    smci.pCode    = reinterpret_cast<const uint32_t*>(_codeSpan.data());

    _precompiled = true;

    VkResult ret = vkCreateShaderModule(device, &smci, nullptr, &_shader);

    _compiled = ret == VK_SUCCESS && _shader != VK_NULL_HANDLE;
    if (!_compiled)
    {
        AXLOGE("axmol: Failed to create VkShaderModule (VkResult={}).", (int)ret);
    }
}

}  // namespace ax::rhi::vk
