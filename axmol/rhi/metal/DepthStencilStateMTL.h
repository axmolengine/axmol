/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/rhi/DepthStencilState.h"
#import <Metal/Metal.h>

#include "tsl/robin_map.h"

namespace ax::rhi::mtl
{

/**
 * @addtogroup _metal
 * @{
 */

/**
 * A class represent a depth/stencil test status.
 */
class DepthStencilStateImpl : public DepthStencilState
{
public:
    /// @name Constructor, Destructor and Initializers
    /**
     * @param mtlDevice The device for which MTLDepthStencilState object was created.
     */
    DepthStencilStateImpl(id<MTLDevice> mtlDevice);
    ~DepthStencilStateImpl();

    void update(const DepthStencilDesc& dsDesc) override;

    /// @name Setters & Getters
    /**
     * get a MTLDepthStencilState object
     */
    inline id<MTLDepthStencilState> getMTLDepthStencilState() const { return _mtlDepthStencilState; }

private:
    id<MTLDevice> _mtlDevice = nil;

    // the current depth stencil state
    id<MTLDepthStencilState> _mtlDepthStencilState = nil;
    // static state for disabled depth and stencil
    id<MTLDepthStencilState> _mtlDepthStencilDisabledState = nil;

    tsl::robin_map<uint32_t, id<MTLDepthStencilState>> _mtlStateCache;
};

// end of _metal group
/// @}
}  // namespace ax::rhi::mtl
