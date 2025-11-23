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
#include "axmol/rhi/DepthStencilState.h"
#include <d3d12.h>
#include <unordered_map>

namespace ax::rhi::d3d12
{
/**
 * @brief A D3D12-based DepthStencilState implementation
 *
 * Note:
 * - In D3D12, depth-stencil state is part of the PSO. We keep a cached
 *   D3D12_DEPTH_STENCIL_DESC to plug into D3D12_GRAPHICS_PIPELINE_STATE_DESC.
 */
class DepthStencilStateImpl : public DepthStencilState
{
public:
    DepthStencilStateImpl();

    void update(const DepthStencilDesc& desc) override;

    uintptr_t getHash() const { return _hash; }

    // Return the D3D12 depth-stencil state desc to embed in PSO
    const D3D12_DEPTH_STENCIL_DESC& getD3D12DepthStencilDesc() const { return _activeDesc; }

private:
    uintptr_t _hash{0};

    D3D12_DEPTH_STENCIL_DESC _activeDesc{};
    D3D12_DEPTH_STENCIL_DESC _disableDesc{};
};

}  // namespace ax::rhi::d3d12
