/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/rhi/GraphicsCore.h"

#include "axmol/rhi/VertexLayout.h"

namespace ax::rhi
{

Program* GraphicsDevice::createComputeProgram(Data csData)
{
    AX_UNUSED_PARAM(csData);
    return nullptr;
}

VertexLayout* GraphicsDevice::createVertexLayout(VertexLayoutDesc&& desc)
{
    return new VertexLayout(std::forward<VertexLayoutDesc>(desc));
}

}  // namespace ax::rhi
