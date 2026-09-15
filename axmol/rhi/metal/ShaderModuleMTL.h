/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/rhi/ShaderModule.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#import <Metal/Metal.h>

namespace ax::rhi::mtl
{
/**
 * @addtogroup _metal
 * @{
 */

/**
 * To Create a vertex or fragment shader.
 */
class ShaderModuleImpl : public ShaderModule
{
public:
    /**
     * @param mtlDevice The device for which MTLFunction object was created.
     * @param stage Specify what kinds of shader to be created.
     * @param source Specify the shader source.
     */
    ShaderModuleImpl(id<MTLDevice> mtlDevice, ShaderStage stage, Data& chunk);
    ~ShaderModuleImpl();

    /**
     * Get MTLFunction object.
     * @return A MTLFunction object.
     */
    inline id<MTLFunction> getMTLFunction() const { return _mtlFunction; }

private:
    id<MTLFunction> _mtlFunction = nil;
};

// end of _metal group
/// @}
}  // namespace ax::rhi::mtl
