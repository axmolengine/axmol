/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/rhi/RHITypes.h"

#include "axmol/base/Object.h"

namespace ax::rhi
{

class RenderTarget;
/**
 * @addtogroup _rhi
 * @{
 */

/**
 * Stencil descriptor.
 */
struct StencilDesc
{
    bool operator==(const StencilDesc& rhs) const;

    StencilOp stencilFailureOp     = StencilOp::KEEP;
    StencilOp depthFailureOp       = StencilOp::KEEP;
    StencilOp depthStencilPassOp   = StencilOp::KEEP;
    CompareFunc stencilCompareFunc = CompareFunc::ALWAYS;
    unsigned int readMask          = 0;
    unsigned int writeMask         = 0;
};

/**
 * Depth and stencil descriptor.
 */
struct DepthStencilDesc
{
    CompareFunc depthCompareFunc = CompareFunc::LESS;
    StencilDesc backFaceStencil;
    StencilDesc frontFaceStencil;
    void addFlag(DepthStencilFlags flag) { this->flags |= flag; }
    void removeFlag(DepthStencilFlags flag) { this->flags &= ~flag; }
    DepthStencilFlags flags = DepthStencilFlags::ALL;
};

/**
 * Store Depth and stencil status.
 */
class DepthStencilState : public ax::Object
{
public:
    virtual void update(const DepthStencilDesc& desc);
    const DepthStencilDesc& getDepthStencilDesc() const { return _dsDesc; }
    bool isEnabled() const { return bitmask::any(_dsDesc.flags, DepthStencilFlags::DEPTH_STENCIL_TEST); }

protected:
    /**
     * @param descriptor Specifies depth and stencil descriptor.
     */
    DepthStencilState() = default;
    virtual ~DepthStencilState();

    DepthStencilDesc _dsDesc{};             ///< depth and stencil descriptor.
    bool _isBackFrontStencilEqual = false;  ///< Does front stencil status equals to back stencil's.
};

// end of _rhi group
/// @}
}  // namespace ax::rhi
