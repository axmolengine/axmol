/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
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
