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

#include "axmol/rhi/RenderTarget.h"
#include <d3d11.h>
#include <array>

namespace ax::rhi::d3d11
{
/**
 * @addtogroup _d3d11
 * @{
 */

/**
 * @brief A D3D11-based Shader RenderTarget implementation
 *
 */
class RenderTargetImpl : public RenderTarget
{
public:
    struct Attachment
    {
        ID3D11Texture2D* texure;
        TextureDesc desc;
    };
    RenderTargetImpl(ID3D11Device* device, bool defaultRenderTarget);
    ~RenderTargetImpl();

    void invalidate();

    void update(ID3D11DeviceContext*);

    void apply(ID3D11DeviceContext*) const;

    ID3D11RenderTargetView* getRTV(size_t idx) const { return _rtvs[idx]; }
    ID3D11DepthStencilView* getDSV() const { return _dsv; }

    Attachment getColorAttachment(int index) const;
    Attachment getDepthStencilAttachment() const;

private:
    ID3D11Device* _device = nullptr;
    std::array<ID3D11RenderTargetView*, MAX_COLOR_ATTCHMENT> _rtvs{};
    ID3D11DepthStencilView* _dsv = nullptr;
    uint32_t _rtvCuont           = 0;
};

/** @} */

}  // namespace ax::rhi::d3d11
