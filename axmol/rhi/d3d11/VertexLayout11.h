/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/VertexLayout.h"
#include "axmol/rhi/RHITypes.h"
#include <d3d11.h>

namespace ax::rhi::d3d11
{
/**
 * @addtogroup _d3d11
 * @{
 */

/**
 * @brief A D3D11-based VertexLayout implementation
 *
 */
class VertexLayoutImpl : public VertexLayout
{
public:
    explicit VertexLayoutImpl(VertexLayoutDesc&&);
    ~VertexLayoutImpl() override;

    /**
     * @brief apply vertex layout
     *
     * @param context
     * @param program
     */
    void apply(ID3D11DeviceContext* context, Program* program) const;

private:
    mutable ID3D11InputLayout* _d3dVL{nullptr};
};

/** @} */

}  // namespace ax::rhi::d3d11
