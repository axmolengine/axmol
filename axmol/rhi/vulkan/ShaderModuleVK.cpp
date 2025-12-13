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
#include "axmol/rhi/vulkan/ShaderModuleVK.h"
#include "axmol/rhi/vulkan/DriverVK.h"
#include "axmol/base/Logging.h"
#include <cassert>
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
        auto device = static_cast<DriverImpl*>(axdrv)->getDevice();
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

    VkResult vr = vkCreateShaderModule(device, &smci, nullptr, &_shader);
    if (vr != VK_SUCCESS || _shader == VK_NULL_HANDLE)
    {
        AXLOGE("axmol: Failed to create VkShaderModule (VkResult={}).", (int)vr);
        assert(false);
    }
}

}  // namespace ax::rhi::vk
