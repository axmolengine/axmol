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

    // Vulkan specific: return VkShaderModule handles
    VkShaderModule getNativeVSModule() const { return static_cast<ShaderModuleImpl*>(_vsModule)->internalHandle(); }
    VkShaderModule getNativeFSModule() const { return static_cast<ShaderModuleImpl*>(_fsModule)->internalHandle(); }
};

}  // namespace ax::rhi::vk
