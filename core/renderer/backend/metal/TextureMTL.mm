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

#include "TextureMTL.h"
#include "UtilsMTL.h"
#include "base/Macros.h"
#include "../PixelFormatUtils.h"

NS_AX_BACKEND_BEGIN

namespace
{
MTLSamplerAddressMode toMTLSamplerAddressMode(SamplerAddressMode mode)
{
    MTLSamplerAddressMode ret = MTLSamplerAddressModeRepeat;
    switch (mode)
    {
    case SamplerAddressMode::REPEAT:
        ret = MTLSamplerAddressModeRepeat;
        break;
    case SamplerAddressMode::MIRROR_REPEAT:
        ret = MTLSamplerAddressModeMirrorRepeat;
        break;
    case SamplerAddressMode::CLAMP_TO_EDGE:
        ret = MTLSamplerAddressModeClampToEdge;
        break;
    default:
        AXASSERT(false, "Not supported sampler address mode!");
        break;
    }
    return ret;
}

MTLSamplerMinMagFilter toMTLSamplerMinMagFilter(SamplerFilter mode)
{
    switch (mode)
    {
    case SamplerFilter::NEAREST:
    case SamplerFilter::NEAREST_MIPMAP_LINEAR:
    case SamplerFilter::NEAREST_MIPMAP_NEAREST:
        return MTLSamplerMinMagFilterNearest;
    case SamplerFilter::LINEAR:
    case SamplerFilter::LINEAR_MIPMAP_LINEAR:
    case SamplerFilter::LINEAR_MIPMAP_NEAREST:
        return MTLSamplerMinMagFilterLinear;
    case SamplerFilter::DONT_CARE:
        return MTLSamplerMinMagFilterNearest;
    }
}

bool isColorRenderable(PixelFormat textureFormat)
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
id<MTLTexture> TextureInfoMTL::ensure(int index, int target)
{
    if (index < AX_META_TEXTURES)
    {
        id<MTLTexture>& mtlTexture = _mtlTextures[index];
        if (mtlTexture)
            return mtlTexture;
        mtlTexture = createTexture(_mtlDevice, _descriptor, target);
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

    if (_mtlSamplerState)
    {
        [_mtlSamplerState release];
        _mtlSamplerState = nil;
    }
    _maxIdx = -1;
}

id<MTLTexture> TextureInfoMTL::createTexture(id<MTLDevice> mtlDevice, const TextureDescriptor& descriptor, int target)
{
    MTLPixelFormat pixelFormat = UtilsMTL::toMTLPixelFormat(descriptor.textureFormat);
    if (pixelFormat == MTLPixelFormatInvalid)
        return nil;

    MTLTextureDescriptor* textureDescriptor = nil;
    switch (target)
    {
    case MTL_TEXTURE_2D:
        textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:pixelFormat
                                                                               width:descriptor.width
                                                                              height:descriptor.height
                                                                           mipmapped:YES];
        break;
    case MTL_TEXTURE_CUBE:
        textureDescriptor = [MTLTextureDescriptor textureCubeDescriptorWithPixelFormat:pixelFormat
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
        if (PixelFormat::D24S8 == descriptor.textureFormat && target == MTL_TEXTURE_2D)
            textureDescriptor.resourceOptions = MTLResourceStorageModePrivate;
        textureDescriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
    }

    return [mtlDevice newTextureWithDescriptor:textureDescriptor];
}

void TextureInfoMTL::recreateSampler(const SamplerDescriptor& descriptor)
{
    MTLSamplerDescriptor* mtlDescriptor = [MTLSamplerDescriptor new];
    mtlDescriptor.sAddressMode          = descriptor.sAddressMode == SamplerAddressMode::DONT_CARE
                                              ? _sAddressMode
                                              : toMTLSamplerAddressMode(descriptor.sAddressMode);
    mtlDescriptor.tAddressMode          = descriptor.tAddressMode == SamplerAddressMode::DONT_CARE
                                              ? _tAddressMode
                                              : toMTLSamplerAddressMode(descriptor.tAddressMode);

    mtlDescriptor.minFilter =
        descriptor.minFilter == SamplerFilter::DONT_CARE ? _minFilter : toMTLSamplerMinMagFilter(descriptor.minFilter);
    mtlDescriptor.magFilter =
        descriptor.magFilter == SamplerFilter::DONT_CARE ? _magFilter : toMTLSamplerMinMagFilter(descriptor.magFilter);

    if (_mtlSamplerState)
    {
        [_mtlSamplerState release];
        _mtlSamplerState = nil;
    }

    _sAddressMode = mtlDescriptor.sAddressMode;
    _tAddressMode = mtlDescriptor.tAddressMode;
    _minFilter    = mtlDescriptor.minFilter;
    _magFilter    = mtlDescriptor.magFilter;
    _mipFilter    = mtlDescriptor.mipFilter;

    _mtlSamplerState = [_mtlDevice newSamplerStateWithDescriptor:mtlDescriptor];

    [mtlDescriptor release];
}

/// CLASS TextureMTL
TextureMTL::TextureMTL(id<MTLDevice> mtlDevice, const TextureDescriptor& descriptor) : _textureInfo(mtlDevice)
{
    updateTextureDescriptor(descriptor);
}

TextureMTL::~TextureMTL() {}

void TextureMTL::updateSamplerDescriptor(const SamplerDescriptor& sampler)
{
    _textureInfo.recreateSampler(sampler);
}

void TextureMTL::updateTextureDescriptor(const ax::backend::TextureDescriptor& descriptor, int index)
{
    TextureBackend::updateTextureDescriptor(descriptor, index);

    _textureInfo._descriptor = descriptor;
    _textureInfo.ensure(index, MTL_TEXTURE_2D);
    updateSamplerDescriptor(descriptor.samplerDescriptor);

    _textureInfo._bytesPerRow = PixelFormatUtils::computeRowPitch(descriptor.textureFormat, descriptor.width);
}

void TextureMTL::updateData(uint8_t* data, std::size_t width, std::size_t height, std::size_t level, int index)
{
    updateSubData(0, 0, width, height, level, data, index);
}

void TextureMTL::updateSubData(std::size_t xoffset,
                               std::size_t yoffset,
                               std::size_t width,
                               std::size_t height,
                               std::size_t level,
                               uint8_t* data,
                               int index)
{
    auto mtlTexture = _textureInfo.ensure(index, MTL_TEXTURE_2D);
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

void TextureMTL::updateCompressedData(uint8_t* data,
                                      std::size_t width,
                                      std::size_t height,
                                      std::size_t dataLen,
                                      std::size_t level,
                                      int index)
{
    updateCompressedSubData(0, 0, width, height, dataLen, level, data, index);
}

void TextureMTL::updateCompressedSubData(std::size_t xoffset,
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

void TextureMTL::generateMipmaps()
{
    if (TextureUsage::RENDER_TARGET == _textureUsage || isColorRenderable(_textureFormat) == false)
        return;

    if (!_hasMipmaps)
    {
        _hasMipmaps = true;
        UtilsMTL::generateMipmaps(reinterpret_cast<id<MTLTexture>>(this->getHandler()));
    }
}

/// CLASS TextureCubeMTL
TextureCubeMTL::TextureCubeMTL(id<MTLDevice> mtlDevice, const TextureDescriptor& descriptor) : _textureInfo(mtlDevice)
{
    updateTextureDescriptor(descriptor);
}

TextureCubeMTL::~TextureCubeMTL() {}

void TextureCubeMTL::updateTextureDescriptor(const ax::backend::TextureDescriptor& descriptor, int index)
{
    TextureBackend::updateTextureDescriptor(descriptor, index);

    _textureInfo._descriptor = descriptor;
    _textureInfo.ensure(index, MTL_TEXTURE_CUBE);
    updateSamplerDescriptor(descriptor.samplerDescriptor);

    _textureInfo._bytesPerRow = PixelFormatUtils::computeRowPitch(descriptor.textureFormat, descriptor.width);
    _bytesPerImage            = _textureInfo._bytesPerRow * descriptor.width;
    _region                   = MTLRegionMake2D(0, 0, descriptor.width, descriptor.height);
}

void TextureCubeMTL::updateSamplerDescriptor(const SamplerDescriptor& sampler)
{
    _textureInfo.recreateSampler(sampler);
}

void TextureCubeMTL::updateFaceData(TextureCubeFace side, void* data, int index)
{
    NSUInteger slice = static_cast<int>(side);
    auto mtlTexture  = _textureInfo.ensure(index, MTL_TEXTURE_CUBE);
    if (!mtlTexture)
        return;

    [mtlTexture replaceRegion:_region
                  mipmapLevel:0
                        slice:slice
                    withBytes:data
                  bytesPerRow:_textureInfo._bytesPerRow
                bytesPerImage:_bytesPerImage];
}

void TextureCubeMTL::generateMipmaps()
{
    if (TextureUsage::RENDER_TARGET == _textureUsage || isColorRenderable(_textureFormat) == false)
        return;

    if (!_hasMipmaps)
    {
        _hasMipmaps = true;
        UtilsMTL::generateMipmaps(reinterpret_cast<id<MTLTexture>>(this->getHandler()));
    }
}

NS_AX_BACKEND_END
