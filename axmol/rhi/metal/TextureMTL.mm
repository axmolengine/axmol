/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
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

#include "axmol/rhi/metal/TextureMTL.h"
#include "axmol/rhi/metal/UtilsMTL.h"
#include "axmol/base/Macros.h"
#include "axmol/rhi/PixelFormatUtils.h"
#include "axmol/rhi/SamplerCache.h"

namespace ax::rhi::mtl {

namespace
{

static bool isColorRenderable(PixelFormat textureFormat)
{
    switch (textureFormat)
    {
    case PixelFormat::RGBA8:
    case PixelFormat::RGBA4:
    case PixelFormat::RGB565:
    case PixelFormat::RGB5A1:
        return true;
    default:
        return false;
    }
}
}

/// CLASS TextureInfoMTL
id<MTLTexture> TextureInfoMTL::ensure(int index)
{
    if (index < AX_META_TEXTURES)
    {
        id<MTLTexture>& mtlTexture = _mtlTextures[index];
        if (mtlTexture)
            return mtlTexture;
        mtlTexture = createTexture(_mtlDevice, _desc);
        if (_maxIdx < index)
            _maxIdx = index;
        return mtlTexture;
    }
    return nil;
}

void TextureInfoMTL::destroy()
{
    if (_maxIdx == -1)
        return;

    id<MTLTexture> texture;
    int i = 0;
    while ((texture = _mtlTextures[i++]))
        [texture release];

    _mtlSamplerState = nil;

    _maxIdx = -1;
}

id<MTLTexture> TextureInfoMTL::createTexture(id<MTLDevice> mtlDevice, const TextureDesc& descriptor)
{
    MTLPixelFormat pixelFormat = UtilsMTL::toMTLPixelFormat(descriptor.textureFormat);
    if (pixelFormat == MTLPixelFormatInvalid)
        return nil;

    MTLTextureDescriptor* textureDesc = nil;
    switch (descriptor.textureType)
    {
    case TextureType::TEXTURE_2D:
        textureDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:pixelFormat
                                                                               width:descriptor.width
                                                                              height:descriptor.height
                                                                           mipmapped:YES];
        break;
    case TextureType::TEXTURE_CUBE:
        textureDesc = [MTLTextureDescriptor textureCubeDescriptorWithPixelFormat:pixelFormat
                                                                                  size:descriptor.width
                                                                             mipmapped:YES];
        break;
    default:
        return nil;
    }

    if (TextureUsage::RENDER_TARGET == descriptor.textureUsage)
    {
        // DepthStencil, and Multisample textures must be allocated with the MTLResourceStorageModePrivate resource
        // option
        if (PixelFormat::D24S8 == descriptor.textureFormat && descriptor.textureType == TextureType::TEXTURE_2D)
            textureDesc.resourceOptions = MTLResourceStorageModePrivate;
        textureDesc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
    }

    return [mtlDevice newTextureWithDescriptor:textureDesc];
}

void TextureInfoMTL::recreateSampler(const SamplerDesc& desc)
{
    _mtlSamplerState = (id<MTLSamplerState>)SamplerCache::getInstance()->getSampler(desc);
}

/// CLASS TextureImpl
TextureImpl::TextureImpl(id<MTLDevice> mtlDevice, const TextureDesc& descriptor) : _textureInfo(mtlDevice)
{
    updateTextureDesc(descriptor);
}

TextureImpl::~TextureImpl() {}

void TextureImpl::updateSamplerDesc(const SamplerDesc& sampler)
{
    _textureInfo.recreateSampler(sampler);
}

void TextureImpl::updateTextureDesc(const TextureDesc& desc, int index)
{
    Texture::updateTextureDesc(desc, index);

    _textureInfo._desc = desc;
    _textureInfo.ensure(index);
    updateSamplerDesc(desc.samplerDesc);

    _textureInfo._bytesPerRow = PixelFormatUtils::computeRowPitch(desc.textureFormat, desc.width);

    if (desc.textureType == TextureType::TEXTURE_CUBE) {
        _region                   = MTLRegionMake2D(0, 0, desc.width, desc.height);
    }
}

void TextureImpl::updateData(uint8_t* data, std::size_t width, std::size_t height, std::size_t level, int index)
{
    updateSubData(0, 0, width, height, level, data, index);
}

void TextureImpl::updateSubData(std::size_t xoffset,
                               std::size_t yoffset,
                               std::size_t width,
                               std::size_t height,
                               std::size_t level,
                               uint8_t* data,
                               int index)
{
    auto mtlTexture = _textureInfo.ensure(index);
    if (!mtlTexture)
        return;

    MTLRegion region = {
        {xoffset, yoffset, 0},  // MTLOrigin
        {width, height, 1}      // MTLSize
    };

    auto bytesPerRow = PixelFormatUtils::computeRowPitch(_textureFormat, static_cast<uint32_t>(width));

    [mtlTexture replaceRegion:region mipmapLevel:level withBytes:data bytesPerRow:bytesPerRow];

    if (!_hasMipmaps && level > 0)
        _hasMipmaps = true;
}

void TextureImpl::updateCompressedData(uint8_t* data,
                                      std::size_t width,
                                      std::size_t height,
                                      std::size_t dataLen,
                                      std::size_t level,
                                      int index)
{
    updateCompressedSubData(0, 0, width, height, dataLen, level, data, index);
}

void TextureImpl::updateCompressedSubData(std::size_t xoffset,
                                         std::size_t yoffset,
                                         std::size_t width,
                                         std::size_t height,
                                         std::size_t dataLen,
                                         std::size_t level,
                                         uint8_t* data,
                                         int index)
{
    updateSubData(xoffset, yoffset, width, height, level, data, index);
}

void TextureImpl::generateMipmaps()
{
    if (TextureUsage::RENDER_TARGET == _textureUsage || !isColorRenderable(_textureFormat))
        return;

    if (!_hasMipmaps)
    {
        _hasMipmaps = true;
        UtilsMTL::generateMipmaps(internalHandle());
    }
}

void TextureImpl::updateFaceData(TextureCubeFace side, void* data, int index)
{
    NSUInteger slice = static_cast<int>(side);
    auto mtlTexture  = _textureInfo.ensure(index);
    if (!mtlTexture)
        return;

    auto slicePitch = _textureInfo._bytesPerRow * _textureInfo._desc.height;
    [mtlTexture replaceRegion:_region
                  mipmapLevel:0
                        slice:slice
                    withBytes:data
                  bytesPerRow:_textureInfo._bytesPerRow
                bytesPerImage:slicePitch];
}

}
