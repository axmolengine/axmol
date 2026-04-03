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
#include "axmol/rhi/ShaderModule.h"
#include "axmol/rhi/DXUtils.h"
#include <d3d12.h>
#include <span>

namespace ax::rhi::d3d12
{
/**
 * @addtogroup _d3d12
 * @{
 */

class DriverImpl;

struct SLCReflectContext;

struct D3D12BlobHandle
{
    ComPtr<IUnknown> blob;
    std::span<uint8_t> view;
};

struct UniformBlockInfo
{
    int binding;          // Vulkan binding index
    uint32_t sizeBytes;   // total size of the UBO
    uint16_t numMembers;  // number of uniforms in this block
    std::string name;     // block name
};

/**
 * @brief A D3D12-based ShaderModule implementation
 *
 */
class ShaderModuleImpl : public ShaderModule
{
public:
    ShaderModuleImpl(DriverImpl* driver, ShaderStage stage, Data& chunk);
    ~ShaderModuleImpl();

    const D3D12BlobHandle& internalHandle() const { return _nativeHandle; }

private:
    D3D12BlobHandle _nativeHandle;
};

/** @} */

}  // namespace ax::rhi::d3d12
