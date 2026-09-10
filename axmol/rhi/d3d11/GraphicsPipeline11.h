/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/GraphicsPipeline.h"
#include "axmol/rhi/DXUtils.h"

#include <d3d11.h>

namespace ax::rhi::d3d11
{
/**
 * @addtogroup _d3d11
 * @{
 */

/**
 * @brief A D3D11-based graphics pipeline implementation
 *
 */
class GraphicsPipelineImpl : public GraphicsPipeline
{
public:
    GraphicsPipelineImpl(ID3D11Device* device, ID3D11DeviceContext* context) : _device(device), _context(context) {}
    void update(const RenderTarget*, const PipelineDesc& desc);

private:
    ID3D11Device* _device         = nullptr;
    ID3D11DeviceContext* _context = nullptr;

    tlx::hash_map<uint32_t, ComPtr<ID3D11BlendState>> _blendCache;
};

/** @} */

}  // namespace ax::rhi::d3d11
