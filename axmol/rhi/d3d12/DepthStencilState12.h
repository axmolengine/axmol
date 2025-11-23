/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/
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
