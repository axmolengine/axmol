/****************************************************************************

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

#include "axmol/base/Object.h"
#include "axmol/rhi/Texture.h"
#include "axmol/tlx/inlined_vector.hpp"
#include <assert.h>

namespace ax::rhi
{

/**
 * @addtogroup _rhi
 * @{
 */

static constexpr uint32_t INITIAL_COLOR_CAPACITY = 4;

/**
 * Render Target
 */
class RenderTarget : public ax::Object
{
public:
    struct RenderBuffer
    {
        Texture* texture = nullptr;
        uint8_t level    = 0;  // level when attached to a texture
        explicit operator bool() const { return texture != nullptr; }
    };
    using ColorAttachment = tlx::inlined_vector<RenderBuffer, INITIAL_COLOR_CAPACITY>;

    RenderTarget(bool defaultRenderTarget) : _defaultRenderTarget(defaultRenderTarget) {}
    virtual ~RenderTarget();

    bool isDefaultRenderTarget() const { return _defaultRenderTarget; }

    virtual void cleanupResources();

    virtual void setColorTexture(Texture* texture, int level = 0, int index = 0);
    virtual void setDepthStencilTexture(Texture* texture, int level = 0);

    bool isDirty() const { return !!_dirtyFlags; }

    ColorAttachment _color{};
    RenderBuffer _depthStencil{};

protected:
    bool _defaultRenderTarget = false;
    mutable TargetBufferFlags _dirtyFlags{};
};
// end of _rhi group
/// @}
}  // namespace ax::rhi
