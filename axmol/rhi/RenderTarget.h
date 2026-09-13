/****************************************************************************

Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
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
protected:
    void dispose() override;

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

    virtual PixelFormat getColorAttachmentPixelFormat(int index = 0) const;
    virtual PixelFormat getDepthStencilAttachmentPixelFormat() const;

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
