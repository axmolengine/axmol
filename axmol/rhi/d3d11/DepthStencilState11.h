/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once
#include <d3d11.h>
#include "axmol/rhi/DXUtils.h"
#include "axmol/rhi/DepthStencilState.h"

namespace ax::rhi::d3d11
{
using namespace Microsoft::WRL;

/**
 * @addtogroup _d3d11
 * @{
 */

/**
 * @brief A D3D11-based DepthStencilState implementation
 *
 */
class DepthStencilStateImpl : public DepthStencilState
{
public:
    explicit DepthStencilStateImpl(ID3D11Device* device);

    void update(const DepthStencilDesc& desc) override;

    void apply(ID3D11DeviceContext* ctx, UINT stencilRef = 0) const;

private:
    ID3D11Device* _device = nullptr;  // weak ref
    tlx::hash_map<uint32_t, ComPtr<ID3D11DepthStencilState>> _stateCache;

    ComPtr<ID3D11DepthStencilState> _activeState;
    ComPtr<ID3D11DepthStencilState> _disableState;
};

/** @} */

}  // namespace ax::rhi::d3d11
