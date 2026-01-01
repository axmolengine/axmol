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
#include "axmol/rhi/RenderTarget.h"

namespace ax::rhi
{

RenderTarget::~RenderTarget()
{
    cleanupResources();
}

void RenderTarget::cleanupResources()
{
    for (auto colorItem : _color)
        AX_SAFE_RELEASE(colorItem.texture);
    _color.clear();
    AX_SAFE_RELEASE_NULL(_depthStencil.texture);
}

void RenderTarget::setColorTexture(Texture* texture, int level, int index)
{
    assert(index >= 0 && index < MAX_COLOR_COUNT);
    if (index > static_cast<int>(_color.size()))
    {
        assert(false && "Color attachment must be set in order, no gaps allowed");
        return;
    }

    if (texture == nullptr && index == static_cast<int>(_color.size()) - 1)
    {
        AX_SAFE_RELEASE(_color[index].texture);
        _color.pop_back();
        _dirtyFlags |= getMRTColorFlag(index);
        return;
    }

    if (_color.size() <= static_cast<size_t>(index))
    {
        _color.resize(index + 1);
    }

    if (_color[index].texture != texture || _color[index].level != level)
    {
        _dirtyFlags |= getMRTColorFlag(index);

        AX_SAFE_RELEASE(_color[index].texture);
        _color[index].texture = texture;
        _color[index].level   = level;
        AX_SAFE_RETAIN(_color[index].texture);
    }
}

void RenderTarget::setDepthStencilTexture(Texture* texture, int level)
{
    if (_depthStencil.texture != texture || _depthStencil.level != level)
    {
        _dirtyFlags |= TargetBufferFlags::DEPTH_AND_STENCIL;
        AX_SAFE_RELEASE(_depthStencil.texture);
        _depthStencil.texture = texture;
        _depthStencil.level   = level;
        AX_SAFE_RETAIN(_depthStencil.texture);
    }
}

}  // namespace ax::rhi
