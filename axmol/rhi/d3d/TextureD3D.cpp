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
#include "axmol/rhi/d3d/TextureD3D.h"
#include "axmol/rhi/d3d/DriverD3D.h"
#include "axmol/rhi/d3d/UtilsD3D.h"
#include "axmol/rhi/PixelFormatUtils.h"
#include "axmol/rhi/SamplerCache.h"

namespace ax::rhi::d3d
{

static void translateUsage(TextureUsage usage, D3D11_TEXTURE2D_DESC& outDesc)
{
    outDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    switch (usage)
    {
    case TextureUsage::READ:
    case TextureUsage::RENDER_TARGET:
        outDesc.Usage          = D3D11_USAGE_DEFAULT;
        outDesc.CPUAccessFlags = 0;
        break;
    case TextureUsage::WRITE:
        outDesc.Usage          = D3D11_USAGE_DYNAMIC;
        outDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        outDesc.BindFlags |= (UINT)D3D11_BIND_UNORDERED_ACCESS;
        break;
    }
}

static void translateTexDesc(const TextureDesc& desc, D3D11_TEXTURE2D_DESC& outDesc)
{
    outDesc.Width              = desc.width;
    outDesc.Height             = desc.height;
    outDesc.SampleDesc.Count   = 1;
    outDesc.SampleDesc.Quality = 0;
    translateUsage(desc.textureUsage, outDesc);

    switch (desc.textureType)
    {
    case TextureType::TEXTURE_2D:
        outDesc.ArraySize = 1;
        break;
    case TextureType::TEXTURE_CUBE:
        outDesc.ArraySize = 6;
        outDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
        break;
    }

    // depth-stencil
    switch (desc.textureFormat)
    {
    case PixelFormat::D24S8:
        outDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
        break;
    default:
        if (desc.textureUsage == TextureUsage::RENDER_TARGET)
            outDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
        break;
    }
}

static void fromD3DTexDesc(TextureDesc& td, const D3D11_TEXTURE2D_DESC& desc)
{
    td.textureType = TextureType::TEXTURE_2D;

    td.width  = desc.Width;
    td.height = desc.Height;

    td.depth = 1;  // desc.ArraySize

    switch (desc.Format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        td.textureFormat = PixelFormat::RGBA8;
        break;
    case DXGI_FORMAT_R8_UNORM:
        td.textureFormat = PixelFormat::R8;
        break;
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        td.textureFormat = PixelFormat::D24S8;
        break;
    default:
        td.textureFormat = PixelFormat::NONE;
        break;
    }

    if (desc.BindFlags & D3D11_BIND_RENDER_TARGET)
        td.textureUsage = TextureUsage::RENDER_TARGET;
    else if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
        td.textureUsage = TextureUsage::READ;
}

TextureHandle TextureResource::createTexture(UINT mipLevels)
{
    D3D11_TEXTURE2D_DESC texDesc{};
    translateTexDesc(_desc, texDesc);

    texDesc.MipLevels = mipLevels;

    if (mipLevels == 0)
    {
        texDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
        texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
    }

    auto fmtInfo = UtilsD3D::toDxgiFormatInfo(_desc.textureFormat);
    assert(fmtInfo);
    if (fmtInfo->format == DXGI_FORMAT_UNKNOWN)
    {
        AXLOGE("axmol: D3D not support pixel format: {}", (int)_desc.textureFormat);
        return TextureHandle{};
    }

    texDesc.Format = fmtInfo->format;

    ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = _device->CreateTexture2D(&texDesc, nullptr, texture.GetAddressOf());
    if (FAILED(hr) || !texture)
    {
        return TextureHandle{};
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = fmtInfo->fmtSrv;
    switch (_desc.textureType)
    {
    case TextureType::TEXTURE_CUBE:
        srvDesc.ViewDimension               = D3D11_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.TextureCube.MipLevels       = -1;
        srvDesc.TextureCube.MostDetailedMip = 0;
        break;
    default:
        srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels       = -1;
        srvDesc.Texture2D.MostDetailedMip = 0;
    }

    ComPtr<ID3D11ShaderResourceView> srv;
    hr = _device->CreateShaderResourceView(texture.Get(), &srvDesc, srv.GetAddressOf());

    if (FAILED(hr) || !srv)
    {
        return TextureHandle{};
    }

    return TextureHandle{texture.Detach(), srv.Detach()};
}

TextureHandle TextureResource::ensure(int index)
{
    if (index < 0 || index >= _textures.size())
        return TextureHandle{};

    if (_textures[index])
        return _textures[index];

    _textures[index] = createTexture(1);
    _maxIdx          = (std::max)(_maxIdx, index + 1);

    return _textures[index];
}

// ------------------------------------------------------------
// ctor / dtor
// ------------------------------------------------------------
TextureImpl::TextureImpl(ID3D11Device* device, const TextureDesc& descriptor) : _textureRes(device)
{
    updateTextureDesc(descriptor);
    // TODO:
    // _rendererRecreatedListener = EventDispatcher::addListener(
    //     EventType::RENDERER_RECREATED, [this](auto&&) { updateTextureDesc(this->_textureDesc); });
}

TextureImpl::TextureImpl(ID3D11Device* device, ID3D11Texture2D* texture) : _textureRes(device)
{
    texture->AddRef();
    _textureRes._textures[0].tex2d = texture;
    _textureRes._maxIdx            = 0;

    D3D11_TEXTURE2D_DESC desc;
    texture->GetDesc(&desc);
    fromD3DTexDesc(_textureRes._desc, desc);

    Texture::updateTextureDesc(_textureRes._desc);
}

TextureImpl::~TextureImpl()
{
    // TODO:
    // EventDispatcher::removeListener(_rendererRecreatedListener);
    _textureRes.destroy();
}

// ------------------------------------------------------------
// updateSamplerDesc
// ------------------------------------------------------------
void TextureImpl::updateSamplerDesc(const SamplerDesc& desc)
{
    _textureRes._samplerState = static_cast<ID3D11SamplerState*>(SamplerCache::getInstance()->getSampler(desc));
}

// ------------------------------------------------------------
// updateTextureDesc
// ------------------------------------------------------------
void TextureImpl::updateTextureDesc(const TextureDesc& descriptor, int index /*=0*/)
{

    _textureRes._desc = descriptor;

    Texture::updateTextureDesc(descriptor);

    updateSamplerDesc(descriptor.samplerDesc);

    if (descriptor.textureFormat != PixelFormat::NONE)
        _textureRes.ensure(index);
}

// ------------------------------------------------------------
// generateMipmaps
// ------------------------------------------------------------
void TextureImpl::generateMipmaps()
{
    if (TextureUsage::RENDER_TARGET == _textureUsage)
        return;

    if (!_hasMipmaps)
    {
        _hasMipmaps  = true;
        auto context = static_cast<DriverImpl*>(DriverBase::getInstance())->getContext();

        // FIXME: consider add a member mipLevels to TextureDesc to avoid recreate texture
        //
        _textureRes.foreachTextures([context, this](TextureHandle& h, int) {
            if (h && h.srv)
            {
                // recreate texture with mip
                auto tmp = _textureRes.createTexture(0);
                if (tmp)
                {
                    context->CopySubresourceRegion(tmp.tex2d, 0, 0, 0, 0, h.tex2d, 0, nullptr);
                    context->GenerateMips(tmp.srv);
                    h.Release();
                    h = tmp;
                }
            }
        });

        auto mainTexture = _textureRes._textures[0];
        if (mainTexture.srv)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
            mainTexture.srv->GetDesc(&srvDesc);
            _textureRes._mipLevels = srvDesc.Texture2D.MipLevels;
        }
    }
}

void TextureImpl::updateData(uint8_t* data, std::size_t width, std::size_t height, std::size_t level, int index /*=0*/)
{
    assert(_textureType == TextureType::TEXTURE_2D);
    _textureRes.ensure(index);
    AX_ASSERT(index <= _textureRes._maxIdx);

    auto rowPitch = PixelFormatUtils::computeRowPitch(_textureFormat, static_cast<uint32_t>(width));

    UINT slicePitch = rowPitch * static_cast<UINT>(height);

    auto context = static_cast<DriverImpl*>(DriverBase::getInstance())->getContext();
    context->UpdateSubresource(_textureRes._textures[index].tex2d, static_cast<UINT>(level), nullptr, data, rowPitch,
                               slicePitch);
}

void TextureImpl::updateCompressedData(uint8_t* data,
                                       std::size_t width,
                                       std::size_t height,
                                       std::size_t dataLen,
                                       std::size_t level,
                                       int index /*=0*/)
{
    assert(_textureType == TextureType::TEXTURE_2D);
    _textureRes.ensure(index);
    AX_ASSERT(index <= _textureRes._maxIdx);

    auto context = static_cast<DriverImpl*>(DriverBase::getInstance())->getContext();
    context->UpdateSubresource(_textureRes._textures[index].tex2d, static_cast<UINT>(level), nullptr, data, 0, 0);
}

void TextureImpl::updateSubData(std::size_t xoffset,
                                std::size_t yoffset,
                                std::size_t width,
                                std::size_t height,
                                std::size_t level,
                                uint8_t* data,
                                int index /*=0*/)
{
    assert(_textureType == TextureType::TEXTURE_2D);
    _textureRes.ensure(index);
    AX_ASSERT(index <= _textureRes._maxIdx);

    D3D11_BOX box{};
    box.left   = static_cast<UINT>(xoffset);
    box.top    = static_cast<UINT>(yoffset);
    box.front  = 0;
    box.right  = static_cast<UINT>(xoffset + width);
    box.bottom = static_cast<UINT>(yoffset + height);
    box.back   = 1;

    auto rowPitch   = PixelFormatUtils::computeRowPitch(_textureFormat, static_cast<uint32_t>(width));
    UINT slicePitch = rowPitch * static_cast<UINT>(height);

    auto context = static_cast<DriverImpl*>(DriverBase::getInstance())->getContext();
    context->UpdateSubresource(_textureRes._textures[index].tex2d, static_cast<UINT>(level), &box, data, rowPitch,
                               slicePitch);
}

void TextureImpl::updateCompressedSubData(std::size_t xoffset,
                                          std::size_t yoffset,
                                          std::size_t width,
                                          std::size_t height,
                                          std::size_t dataLen,
                                          std::size_t level,
                                          uint8_t* data,
                                          int index /*=0*/)
{
    assert(_textureType == TextureType::TEXTURE_2D);
    _textureRes.ensure(index);
    AX_ASSERT(index <= _textureRes._maxIdx);

    D3D11_BOX box{};
    box.left   = static_cast<UINT>(xoffset);
    box.top    = static_cast<UINT>(yoffset);
    box.front  = 0;
    box.right  = static_cast<UINT>(xoffset + width);
    box.bottom = static_cast<UINT>(yoffset + height);
    box.back   = 1;

    auto context = static_cast<DriverImpl*>(DriverBase::getInstance())->getContext();
    context->UpdateSubresource(_textureRes._textures[index].tex2d, static_cast<UINT>(level), &box, data, 0, 0);
}

void TextureImpl::updateFaceData(TextureCubeFace side, void* data, int index)
{
    assert(_textureType == TextureType::TEXTURE_CUBE);
    _textureRes.ensure(index);
    assert(index <= _textureRes._maxIdx);

    auto context = static_cast<DriverImpl*>(DriverBase::getInstance())->getContext();

    auto& texHandle = _textureRes.getTexture(index);

    //-------------------------------------------------------------------
    // 1. compute SubResource： = 6 * (mip-levels)
    //-------------------------------------------------------------------
    const uint32_t mipLevels   = 1;  // texHandle.tex2d->getMipLevels();
    const uint32_t mipLevel    = 0;
    const uint32_t arraySlice  = static_cast<uint32_t>(side) + index * 6;
    const uint32_t subresource = ::D3D11CalcSubresource(mipLevel, arraySlice, mipLevels);

    //-------------------------------------------------------------------
    // 2. compute RowPitch / SlicePitch
    //-------------------------------------------------------------------
    const uint32_t rowPitch =
        PixelFormatUtils::computeRowPitch(_textureFormat, static_cast<uint32_t>(_textureRes._desc.width));
    const uint32_t slicePitch = _textureRes._desc.height * rowPitch;

    //-------------------------------------------------------------------
    // 3. update
    //-------------------------------------------------------------------
    context->UpdateSubresource(_textureRes._textures[index].tex2d, subresource, nullptr, data, rowPitch, slicePitch);
}

}  // namespace ax::rhi::d3d
