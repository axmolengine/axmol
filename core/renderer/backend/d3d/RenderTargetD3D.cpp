#include "RenderTargetD3D.h"
#include "TextureD3D.h"
#include "UtilsD3D.h"

namespace ax::backend::d3d
{
RenderTargetImpl::RenderTargetImpl(ID3D11Device* device, bool defaultRenderTarget)
    : _device(device), RenderTarget(defaultRenderTarget)
{
    if (_defaultRenderTarget)
        _dirtyFlags = TargetBufferFlags::ALL;
}

RenderTargetImpl::~RenderTargetImpl()
{
    if (_srv)
        _srv->Release();
    for (auto& rtv : _rtvs)
    {
        if (rtv)
            rtv->Release();
        rtv = nullptr;
    }
}

void RenderTargetImpl::update(ID3D11DeviceContext* context) const
{
    if (!_dirtyFlags)
        return;

    if (_defaultRenderTarget)
    {
        context->OMGetRenderTargets(1, &_rtvs[0], &_dsv);
    }
    else
    {
        if (bitmask::any(_dirtyFlags, TargetBufferFlags::COLOR_ALL))
        {  // color attachments
            for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
            {
                auto textureInfo = _color[i];
                if (bitmask::any(_dirtyFlags, getMRTColorFlag(i)))
                {
                    if (textureInfo.texture)
                    {
                        _device->CreateRenderTargetView(
                            static_cast<TextureImpl*>(textureInfo.texture)->internalHandle().tex2d, nullptr, &_rtvs[i]);
                    }
                    else if (_rtvs[i])
                    {
                        _rtvs[i]->Release();
                        _rtvs[i] = nullptr;
                    }
                }
            }
        }

        if (bitmask::any(_dirtyFlags, TargetBufferFlags::DEPTH_AND_STENCIL))
        {
            if (_depthStencil)
                _device->CreateDepthStencilView(
                    static_cast<TextureImpl*>(_depthStencil.texture)->internalHandle().tex2d, nullptr, &_dsv);
            else if (_dsv)
            {
                _dsv->Release();
                _dsv = nullptr;
            }
        }
    }

    _dirtyFlags = TargetBufferFlags::NONE;
}

RenderTargetImpl::Attachment RenderTargetImpl::getColorAttachment(int index) const
{

    auto textureImpl = _defaultRenderTarget ? UtilsD3D::getDefaultColorAttachment() : static_cast<TextureImpl*>(_color[index].texture);
    return textureImpl
               ? RenderTargetImpl::Attachment{static_cast<ID3D11Texture2D*>(textureImpl->internalHandle().tex2d),
                                              textureImpl->getDesc()}
               : RenderTargetImpl::Attachment{};
}

RenderTargetImpl::Attachment RenderTargetImpl::getDepthStencilAttachment(int index) const
{
    auto textureImpl = _defaultRenderTarget ? UtilsD3D::getDefaultColorAttachment()
                                            : static_cast<TextureImpl*>(_depthStencil.texture);

    return textureImpl
               ? RenderTargetImpl::Attachment{static_cast<ID3D11Texture2D*>(textureImpl->internalHandle().tex2d),
                                              textureImpl->getDesc()}
               : RenderTargetImpl::Attachment{};
}

void RenderTargetImpl::apply(ID3D11DeviceContext* context) const
{
    context->OMSetRenderTargets(_rtvs.size(), _rtvs.data(), _dsv);
}

ID3D11ShaderResourceView* RenderTargetImpl::getShaderResourceView() const
{
    return _srv;
}

}  // namespace ax::backend::d3d
