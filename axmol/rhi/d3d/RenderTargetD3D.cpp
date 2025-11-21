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
#include "axmol/rhi/d3d/RenderTargetD3D.h"
#include "axmol/rhi/d3d/TextureD3D.h"
#include "axmol/rhi/d3d/UtilsD3D.h"

namespace ax::rhi::d3d
{
RenderTargetImpl::RenderTargetImpl(ID3D11Device* device, bool defaultRenderTarget)
    : _device(device), RenderTarget(defaultRenderTarget)
{
    if (_defaultRenderTarget)
        _dirtyFlags = TargetBufferFlags::ALL;
}

RenderTargetImpl::~RenderTargetImpl()
{
    invalidate();
}

void RenderTargetImpl::invalidate()
{
    for (auto& rtv : _rtvs)
        SafeRelease(rtv);

    SafeRelease(_dsv);

    if (_defaultRenderTarget)  // release default render target attachments
        UtilsD3D::updateDefaultRenderTargetAttachments(nullptr, nullptr);

    _dirtyFlags = TargetBufferFlags::ALL;
}

void RenderTargetImpl::update(ID3D11DeviceContext* context)
{
    if (!_dirtyFlags)
        return;

    if (_defaultRenderTarget)
    {
        _rtvCuont = 1;
        context->OMGetRenderTargets(_rtvCuont, &_rtvs[0], &_dsv);
    }
    else
    {
        if (bitmask::any(_dirtyFlags, TargetBufferFlags::COLOR_ALL))
        {  // color attachments
            _rtvCuont = 0;
            for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
            {
                auto textureInfo = _color[i];
                if (bitmask::any(_dirtyFlags, getMRTColorFlag(i)))
                {
                    if (textureInfo.texture)
                    {
                        ++_rtvCuont;
                        _device->CreateRenderTargetView(
                            static_cast<TextureImpl*>(textureInfo.texture)->internalHandle().tex2d, nullptr, &_rtvs[i]);
                    }
                    else if (_rtvs[i])
                    {
                        SafeRelease(_rtvs[i]);
                    }
                }
            }
        }

        if (bitmask::any(_dirtyFlags, TargetBufferFlags::DEPTH_AND_STENCIL))
        {
            if (_depthStencil)
            {
                auto fmtInfo = UtilsD3D::toDxgiFormatInfo(_depthStencil.texture->getPixelFormat());
                D3D11_DEPTH_STENCIL_VIEW_DESC desc{};
                desc.Format        = fmtInfo->fmtDsv;
                desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
                _device->CreateDepthStencilView(
                    static_cast<TextureImpl*>(_depthStencil.texture)->internalHandle().tex2d, &desc, &_dsv);
            }
            else if (_dsv)
                SafeRelease(_dsv);
        }
    }

    _dirtyFlags = TargetBufferFlags::NONE;
}

RenderTargetImpl::Attachment RenderTargetImpl::getColorAttachment(int index) const
{

    auto textureImpl =
        _defaultRenderTarget ? UtilsD3D::getDefaultColorAttachment() : static_cast<TextureImpl*>(_color[index].texture);
    return textureImpl
               ? RenderTargetImpl::Attachment{static_cast<ID3D11Texture2D*>(textureImpl->internalHandle().tex2d),
                                              textureImpl->getDesc()}
               : RenderTargetImpl::Attachment{};
}

RenderTargetImpl::Attachment RenderTargetImpl::getDepthStencilAttachment() const
{
    auto textureImpl = _defaultRenderTarget ? UtilsD3D::getDefaultDepthStencilAttachment()
                                            : static_cast<TextureImpl*>(_depthStencil.texture);

    return textureImpl
               ? RenderTargetImpl::Attachment{static_cast<ID3D11Texture2D*>(textureImpl->internalHandle().tex2d),
                                              textureImpl->getDesc()}
               : RenderTargetImpl::Attachment{};
}

void RenderTargetImpl::apply(ID3D11DeviceContext* context) const
{
    context->OMSetRenderTargets(_rtvCuont, _rtvs.data(), _dsv);
}

}  // namespace ax::rhi::d3d
