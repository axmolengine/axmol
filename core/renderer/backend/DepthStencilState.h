/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

 https://axmolengine.github.io/

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

#include "Macros.h"
#include "Types.h"

#include "base/Object.h"

NS_AX_BACKEND_BEGIN

class RenderTarget;
/**
 * @addtogroup _backend
 * @{
 */

/**
 * Stencil descriptor.
 */
struct StencilDescriptor
{
    bool operator==(const StencilDescriptor& rhs) const;

    StencilOperation stencilFailureOperation   = StencilOperation::KEEP;
    StencilOperation depthFailureOperation     = StencilOperation::KEEP;
    StencilOperation depthStencilPassOperation = StencilOperation::KEEP;
    CompareFunction stencilCompareFunction     = CompareFunction::ALWAYS;
    unsigned int readMask                      = 0;
    unsigned int writeMask                     = 0;
};

/**
 * Depth and stencil descriptor.
 */
struct DepthStencilDescriptor
{
    CompareFunction depthCompareFunction = CompareFunction::LESS;
    StencilDescriptor backFaceStencil;
    StencilDescriptor frontFaceStencil;
    void addFlag(DepthStencilFlags flag) { this->flags |= flag; }
    void removeFlag(DepthStencilFlags flag) { this->flags &= ~flag; }
    // must match current render target
    DepthStencilFlags flags = DepthStencilFlags::ALL;
};

/**
 * Store Depth and stencil status.
 */
class DepthStencilState : public ax::Object
{
public:
    virtual void update(const DepthStencilDescriptor& desc);
    const DepthStencilDescriptor& getDepthStencilInfo() const { return _depthStencilInfo; }
    bool isEnabled() const { return bitmask::any(_depthStencilInfo.flags, DepthStencilFlags::DEPTH_STENCIL_TEST); }

protected:
    /**
     * @param descriptor Specifies depth and stencil descriptor.
     */
    DepthStencilState() = default;
    virtual ~DepthStencilState();

    DepthStencilDescriptor _depthStencilInfo{};  ///< depth and stencil descriptor.
    bool _isBackFrontStencilEqual = false;       ///< Does front stencil status equals to back stencil's.
};

// end of _backend group
/// @}
NS_AX_BACKEND_END
