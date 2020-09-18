/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

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

#include <array>
#include <vector>

#include "Macros.h"
#include "Types.h"

CC_BACKEND_BEGIN

class TextureBackend;
/**
 * @addtogroup _backend
 * @{
 */

struct RenderPassFlags {
    /**
     * bitmask indicating which buffers to clear at the beginning of a render pass.
     * This implies discard.
     */
    TargetBufferFlags clear;

    /**
     * bitmask indicating which buffers to discard at the beginning of a render pass.
     * Discarded buffers have uninitialized content, they must be entirely drawn over or cleared.
     */
    TargetBufferFlags discardStart;

    /**
     * bitmask indicating which buffers to discard at the end of a render pass.
     * Discarded buffers' content becomes invalid, they must not be read from again.
     */
    TargetBufferFlags discardEnd;
};


/**
 * Store values about color, depth and stencil attachment.
 */
struct RenderPassParams
{
    RenderPassParams& operator=(const RenderPassParams& descriptor) = default;
    bool operator==(const RenderPassParams& descriptor) const;
    // bool needDepthStencilAttachment() const { return depthTestEnabled || stencilTestEnabled; }

    float clearDepthValue = 0.f;
    float clearStencilValue = 0.f;
    std::array<float, 4> clearColorValue {{0.f, 0.f, 0.f, 0.f}}; // double-braces required in C++11
    
    // TODO: renderTarget flags
    //bool needColorAttachment = true;

    // TODO: renderTarget flags For setup Default RenderTarget pipeline
    //bool depthTestEnabled = false;
    //bool stencilTestEnabled = false;

    // TODO: explicit use, only clear flag needed?
    RenderPassFlags flags{};
};
typedef RenderPassParams RenderPassParams;

//end of _backend group
/// @}
CC_BACKEND_END
