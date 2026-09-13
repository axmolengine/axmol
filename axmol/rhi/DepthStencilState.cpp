/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/rhi/DepthStencilState.h"
#include "axmol/rhi/RenderTarget.h"

namespace ax::rhi
{

bool StencilDesc::operator==(const StencilDesc& rhs) const
{
    return (stencilFailureOp == rhs.stencilFailureOp && depthFailureOp == rhs.depthFailureOp &&
            depthStencilPassOp == rhs.depthStencilPassOp && stencilCompareFunc == rhs.stencilCompareFunc &&
            readMask == rhs.readMask && writeMask == rhs.writeMask);
}

DepthStencilState::~DepthStencilState() {}

void DepthStencilState::update(const DepthStencilDesc& dsDesc)
{
    _dsDesc                  = dsDesc;
    _isBackFrontStencilEqual = dsDesc.backFaceStencil == dsDesc.frontFaceStencil;
}

}  // namespace ax::rhi
