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
#include "axmol/rhi/RHIUtils.h"
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
    outDesc.MipLevels          = desc.mipLevels;
    translateUsage(desc.textureUsage, outDesc);

    switch (desc.textureType)
    {
    case TextureType::TEXTURE_2D:
        outDesc.ArraySize = desc.arraySize;
        break;
    case TextureType::TEXTURE_CUBE:
        outDesc.ArraySize = 6;
        outDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
        break;
    }

    // depth-stencil
    switch (desc.pixelFormat)
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

    td.width     = static_cast<uint16_t>(desc.Width);
    td.height    = static_cast<uint16_t>(desc.Height);
    td.arraySize = desc.ArraySize;

    switch (desc.Format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        td.pixelFormat = PixelFormat::RGBA8;
        break;
    case DXGI_FORMAT_R8_UNORM:
        td.pixelFormat = PixelFormat::R8;
        break;
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        td.pixelFormat = PixelFormat::D24S8;
        break;
    default:
        td.pixelFormat = PixelFormat::NONE;
        break;
    }

    if (desc.BindFlags & D3D11_BIND_RENDER_TARGET)
        td.textureUsage = TextureUsage::RENDER_TARGET;
    else if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
        td.textureUsage = TextureUsage::READ;
}

// ------------------------------------------------------------
// ctor / dtor
// ------------------------------------------------------------
TextureImpl::TextureImpl(ID3D11Device* device, const TextureDesc& desc) : _device(device)
{
    updateTextureDesc(desc);
}

TextureImpl::TextureImpl(ID3D11Device* device, ID3D11Texture2D* texture) : _device(device)
{
    texture->AddRef();
    _nativeTexture.tex2d = texture;

    D3D11_TEXTURE2D_DESC desc;
    texture->GetDesc(&desc);
    fromD3DTexDesc(_desc, desc);

    Texture::updateTextureDesc(_desc);
}

TextureImpl::~TextureImpl()
{
    _nativeTexture.destroy();
}

// ------------------------------------------------------------
// updateSamplerDesc
// ------------------------------------------------------------
void TextureImpl::updateSamplerDesc(const SamplerDesc& desc)
{
    _desc.samplerDesc = desc;
    _samplerState     = static_cast<ID3D11SamplerState*>(SamplerCache::getInstance()->getSampler(desc));
}

// ------------------------------------------------------------
// updateTextureDesc
// ------------------------------------------------------------
void TextureImpl::updateTextureDesc(const TextureDesc& desc)
{
    _desc = desc;

    Texture::updateTextureDesc(desc);

    updateSamplerDesc(desc.samplerDesc);
}

void TextureImpl::updateData(const void* data, int width, int height, int level, int layerIndex /*=0*/)
{
    updateSubData(0, 0, width, height, level, data, layerIndex);
}

void TextureImpl::updateCompressedData(const void* data,
                                       int width,
                                       int height,
                                       std::size_t dataSize,
                                       int level,
                                       int layerIndex /*=0*/)
{
    updateCompressedSubData(0, 0, width, height, dataSize, level, data, layerIndex);
}

void TextureImpl::updateSubData(int xoffset,
                                int yoffset,
                                int width,
                                int height,
                                int level,
                                const void* data,
                                int layerIndex /*=0*/)
{
    assert(_desc.textureType == TextureType::TEXTURE_2D);
    ensureNativeTexture();

    if (!data) [[unlikely]]
        return;

    D3D11_BOX box{};
    box.left   = static_cast<UINT>(xoffset);
    box.top    = static_cast<UINT>(yoffset);
    box.front  = 0;
    box.right  = static_cast<UINT>(xoffset + width);
    box.bottom = static_cast<UINT>(yoffset + height);
    box.back   = 1;

    auto rowPitch   = RHIUtils::computeRowPitch(_desc.pixelFormat, static_cast<uint32_t>(width));
    UINT slicePitch = rowPitch * static_cast<UINT>(height);

    auto context     = static_cast<DriverImpl*>(DriverBase::getInstance())->getContext();
    UINT subresource = D3D11CalcSubresource(level, layerIndex, _desc.mipLevels);
    context->UpdateSubresource(_nativeTexture, subresource, &box, data, rowPitch, slicePitch);

    if (shouldGenMipmaps(level))
        generateMipmaps(context);
}

void TextureImpl::updateCompressedSubData(int xoffset,
                                          int yoffset,
                                          int width,
                                          int height,
                                          std::size_t /*dataSize*/,
                                          int level,
                                          const void* data,
                                          int layerIndex /*=0*/)
{
    assert(_desc.textureType == TextureType::TEXTURE_2D);
    ensureNativeTexture();

    D3D11_BOX box{};
    box.left   = static_cast<UINT>(xoffset);
    box.top    = static_cast<UINT>(yoffset);
    box.front  = 0;
    box.right  = static_cast<UINT>(xoffset + width);
    box.bottom = static_cast<UINT>(yoffset + height);
    box.back   = 1;

    auto context     = static_cast<DriverImpl*>(DriverBase::getInstance())->getContext();
    UINT subresource = D3D11CalcSubresource(level, layerIndex, _desc.mipLevels);
    context->UpdateSubresource(_nativeTexture, subresource, &box, data, 0, 0);

    if (shouldGenMipmaps(level))
        generateMipmaps(context);
}

void TextureImpl::updateFaceData(TextureCubeFace side, const void* data)
{
    assert(_desc.textureType == TextureType::TEXTURE_CUBE);
    ensureNativeTexture();

    auto context = static_cast<DriverImpl*>(DriverBase::getInstance())->getContext();

    //-------------------------------------------------------------------
    // 1. compute SubResource： = 6 * (mip-levels)
    //-------------------------------------------------------------------
    const uint32_t mipLevel    = 0;
    const uint32_t arraySlice  = static_cast<uint32_t>(side);
    const uint32_t subresource = ::D3D11CalcSubresource(mipLevel, arraySlice, _desc.mipLevels);

    //-------------------------------------------------------------------
    // 2. compute RowPitch / SlicePitch
    //-------------------------------------------------------------------
    const uint32_t rowPitch   = RHIUtils::computeRowPitch(_desc.pixelFormat, static_cast<uint32_t>(_desc.width));
    const uint32_t slicePitch = _desc.height * rowPitch;

    //-------------------------------------------------------------------
    // 3. update
    //-------------------------------------------------------------------
    context->UpdateSubresource(_nativeTexture, subresource, nullptr, data, rowPitch, slicePitch);

    if (shouldGenMipmaps())
        generateMipmaps(context);
}

void TextureImpl::ensureNativeTexture()
{
    if (_nativeTexture)
        return;
    D3D11_TEXTURE2D_DESC texDesc{};
    translateTexDesc(_desc, texDesc);

    texDesc.MipLevels = _desc.mipLevels;

    if (_desc.mipLevels == 0)
    {  // means we must invoke generateMipmaps by GPU
        texDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
        texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
    }

    auto fmtInfo = UtilsD3D::toDxgiFormatInfo(_desc.pixelFormat);
    assert(fmtInfo);
    if (fmtInfo->format == DXGI_FORMAT_UNKNOWN)
    {
        AXLOGE("axmol: D3D not support pixel format: {}", (int)_desc.pixelFormat);
        return;
    }

    texDesc.Format = fmtInfo->format;

    ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = _device->CreateTexture2D(&texDesc, nullptr, texture.GetAddressOf());
    if (FAILED(hr) || !texture)
    {
        return;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = fmtInfo->fmtSrv;

    // srvDesc.xxx.MipLevels:
    //   Set to -1 to indicate all the mipmap levels from MostDetailedMip on down to least detailed.
    // srvDesc.xxx.MostDetailedMip
    //   Index of the most detailed mipmap level to use; this number is between 0 and MipLevels (from the original
    //   Texture2D for which ID3D11Device::CreateShaderResourceView creates a view) -1.
    switch (_desc.textureType)
    {
    case TextureType::TEXTURE_CUBE:
        srvDesc.ViewDimension               = D3D11_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.TextureCube.MipLevels       = -1;
        srvDesc.TextureCube.MostDetailedMip = 0;
        break;
    default:
        if (texDesc.ArraySize == 1)
        {
            srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels       = -1;
            srvDesc.Texture2D.MostDetailedMip = 0;
        }
        else
        {
            srvDesc.ViewDimension                  = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            srvDesc.Texture2DArray.MipLevels       = -1;
            srvDesc.Texture2DArray.MostDetailedMip = 0;
            srvDesc.Texture2DArray.ArraySize       = texDesc.ArraySize;
        }
    }

    ComPtr<ID3D11ShaderResourceView> srv;
    hr = _device->CreateShaderResourceView(texture.Get(), &srvDesc, srv.GetAddressOf());

    if (FAILED(hr) || !srv)
    {
        AXLOGE("create srv from texture fail:{}", hr);
        assert(false && "create srv from texture fail:{}");
        return;
    }

    _nativeTexture = TextureHandle{texture.Detach(), srv.Detach()};
}

// ------------------------------------------------------------
// generateMipmaps
// ------------------------------------------------------------
void TextureImpl::generateMipmaps(ID3D11DeviceContext* context)
{
    if (TextureUsage::RENDER_TARGET == _desc.textureUsage)
        return;
    if (!_nativeTexture)
    {
        // geneate mipmaps by GPU required at least upload data once
        return;
    }

    // generate mipmaps
    context->GenerateMips(_nativeTexture.srv);
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    _nativeTexture.srv->GetDesc(&srvDesc);

    _overrideMipLevels = (std::max)(1u, srvDesc.Texture2D.MipLevels);
}

}  // namespace ax::rhi::d3d
