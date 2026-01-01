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
#include "axmol/rhi/d3d11/RenderTarget11.h"
#include "axmol/rhi/d3d11/Texture11.h"
#include "axmol/rhi/d3d11/Driver11.h"
#include "axmol/rhi/DXUtils.h"

namespace ax::rhi::d3d11
{
RenderTargetImpl::RenderTargetImpl(DriverImpl* driver, bool defaultRenderTarget)
    : _driver(driver), _device(driver->getDevice()), RenderTarget(defaultRenderTarget)
{}

RenderTargetImpl::~RenderTargetImpl() {}

void RenderTargetImpl::cleanupResources()
{
    for (auto& rtv : _rtvs)
        SafeRelease(rtv);
    SafeRelease(_dsv);

    RenderTarget::cleanupResources();
}

void RenderTargetImpl::setColorTexture(Texture* texture, int level, int index)
{
    RenderTarget::setColorTexture(texture, level, index);
    _rtvs.resize(_color.size());
}

void RenderTargetImpl::beginRenderPass(ID3D11DeviceContext* context)
{
    if (_dirtyFlags != TargetBufferFlags::NONE) [[unlikely]]
    {
        if (_defaultRenderTarget)
        {
            if (bitmask::any(_dirtyFlags, TargetBufferFlags::COLOR))
            {
                SafeRelease(_rtvs[0]);

                ID3D11Resource* resource = static_cast<TextureImpl*>(_color[0].texture)->internalHandle();
                _AXASSERT_HR(_device->CreateRenderTargetView(resource, nullptr, &_rtvs[0]));
            }

            if (bitmask::any(_dirtyFlags, TargetBufferFlags::COLOR))
            {
                SafeRelease(_dsv);

                if (_depthStencil)
                {
                    auto resource = static_cast<TextureImpl*>(_depthStencil.texture)->internalHandle();
                    _AXASSERT_HR(_device->CreateDepthStencilView(resource, nullptr, &_dsv));
                }
            }

            _rtvCuont = 1;
        }
        else
        {
            if (bitmask::any(_dirtyFlags, TargetBufferFlags::COLOR_ALL))
            {  // color attachments
                _rtvCuont             = 0;
                const auto colorCount = _color.size();
                for (size_t i = 0; i < colorCount; ++i)
                {
                    if (bitmask::any(_dirtyFlags, getMRTColorFlag(i)))
                    {
                        auto textureInfo = _color[i];
                        SafeRelease(_rtvs[i]);
                        if (textureInfo.texture)
                        {
                            ++_rtvCuont;
                            _device->CreateRenderTargetView(
                                static_cast<TextureImpl*>(textureInfo.texture)->internalHandle(), nullptr, &_rtvs[i]);
                        }
                    }
                }
            }

            if (bitmask::any(_dirtyFlags, TargetBufferFlags::DEPTH_AND_STENCIL))
            {
                SafeRelease(_dsv);
                if (_depthStencil)
                {
                    auto fmtInfo = dxutils::toDxgiFormatInfo(_depthStencil.texture->getPixelFormat());
                    D3D11_DEPTH_STENCIL_VIEW_DESC desc{};
                    desc.Format        = fmtInfo->fmtDsv;
                    desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
                    _device->CreateDepthStencilView(static_cast<TextureImpl*>(_depthStencil.texture)->internalHandle(),
                                                    &desc, &_dsv);
                }
            }
        }

        _dirtyFlags = TargetBufferFlags::NONE;
    }

    context->OMSetRenderTargets(_rtvCuont, _rtvs.data(), _dsv);
}

bool RenderTargetImpl::rebuildSwapchainBuffers(IDXGISwapChain* swapchain,
                                               uint32_t width,
                                               uint32_t height,
                                               std::optional<UINT> swapchainFlags)
{
    cleanupResources();

    if (swapchainFlags)
    {
        HRESULT hr = swapchain->ResizeBuffers(0, width, height, DEFAULT_SWAPCHAIN_FORMAT, swapchainFlags.value());
        if (FAILED(hr))
        {
            AXLOGE("D3D11: swapchain ResizeBuffers failed: {}", hr);
            return false;
        }
    }

    _color.resize(1);
    _rtvs.resize(1);

    /// rebuild color attachment: texture
    ComPtr<ID3D11Texture2D> backBuffer;
    _AXASSERT_HR(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf()));
    _color[0].texture = new TextureImpl(_device, backBuffer.Get());

    UINT fmtSupport = 0;
    _device->CheckFormatSupport(DXGI_FORMAT_D24_UNORM_S8_UINT, &fmtSupport);
    assert(fmtSupport & D3D11_FORMAT_SUPPORT_DEPTH_STENCIL);

    /// rebuild depth-stencil attachment: texture
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width                = width;
    depthDesc.Height               = height;
    depthDesc.MipLevels            = 1;
    depthDesc.ArraySize            = 1;
    depthDesc.Format               = DXGI_FORMAT_D24_UNORM_S8_UINT;  // 24-bit depth, 8-bit stencil
    depthDesc.Usage                = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags            = D3D11_BIND_DEPTH_STENCIL;
    depthDesc.SampleDesc.Count     = 1;
    depthDesc.SampleDesc.Quality   = 0;

    ComPtr<ID3D11Texture2D> depthStencilTexture;
    _AXASSERT_HR(_device->CreateTexture2D(&depthDesc, nullptr, depthStencilTexture.GetAddressOf()));
    _depthStencil.texture = new TextureImpl(_device, depthStencilTexture.Get());

    _dirtyFlags = TargetBufferFlags::ALL;

    return true;
}

RenderTargetImpl::Attachment RenderTargetImpl::getColorAttachment(int index) const
{
    auto textureImpl = static_cast<TextureImpl*>(_defaultRenderTarget ? _color[0].texture : _color[index].texture);
    return textureImpl
               ? RenderTargetImpl::Attachment{static_cast<ID3D11Texture2D*>(textureImpl->internalHandle().resource),
                                              textureImpl->getDesc()}
               : RenderTargetImpl::Attachment{};
}

RenderTargetImpl::Attachment RenderTargetImpl::getDepthStencilAttachment() const
{
    auto textureImpl = static_cast<TextureImpl*>(_depthStencil.texture);

    return textureImpl
               ? RenderTargetImpl::Attachment{static_cast<ID3D11Texture2D*>(textureImpl->internalHandle().resource),
                                              textureImpl->getDesc()}
               : RenderTargetImpl::Attachment{};
}

}  // namespace ax::rhi::d3d11
