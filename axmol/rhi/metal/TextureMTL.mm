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
#include "axmol/rhi/RHIUtils.h"
#include "axmol/rhi/SamplerCache.h"

namespace ax::rhi::mtl
{

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
}  // namespace

/// CLASS TextureImpl
TextureImpl::TextureImpl(id<MTLDevice> mtlDevice, const TextureDesc& desc) : _mtlDevice(mtlDevice)
{
    updateTextureDesc(desc);
}

TextureImpl::~TextureImpl() {}

void TextureImpl::updateSamplerDesc(const SamplerDesc& desc)
{
    _mtlSamplerState = static_cast<id<MTLSamplerState>>(SamplerCache::getInstance()->getSampler(desc));
}

void TextureImpl::updateTextureDesc(const TextureDesc& desc)
{
    Texture::updateTextureDesc(desc);

    updateSamplerDesc(desc.samplerDesc);
}

void TextureImpl::updateData(const void* data, int width, int height, int level, int index)
{
    updateSubData(0, 0, width, height, level, data, index);
}

void TextureImpl::updateSubData(int xoffset,
                                int yoffset,
                                int width,
                                int height,
                                int level,
                                const void* data,
                                int layerIndex)
{
    ensureNativeTexture();

    if (!data) [[unlikely]]
        return;

    MTLRegion region = {
        {(NSUInteger)xoffset, (NSUInteger)yoffset, 0},  // MTLOrigin
        {(NSUInteger)width, (NSUInteger)height, 1}      // MTLSize
    };

    auto bytesPerRow = RHIUtils::computeRowPitch(_desc.pixelFormat, static_cast<uint32_t>(width));
    auto slicePitch  = bytesPerRow * _desc.height;
    [_mtlTexture replaceRegion:region
                   mipmapLevel:level
                         slice:layerIndex
                     withBytes:data
                   bytesPerRow:bytesPerRow
                 bytesPerImage:slicePitch];

    if (shouldGenMipmaps(level))
        generateMipmaps();
}

void TextureImpl::updateCompressedData(const void* data,
                                       int width,
                                       int height,
                                       std::size_t dataSize,
                                       int level,
                                       int layerIndex)
{
    updateCompressedSubData(0, 0, width, height, dataSize, level, data, layerIndex);
}

void TextureImpl::updateCompressedSubData(int xoffset,
                                          int yoffset,
                                          int width,
                                          int height,
                                          std::size_t /*dataSize*/,
                                          int level,
                                          const void* data,
                                          int layerIndex)
{
    updateSubData(xoffset, yoffset, width, height, level, data, layerIndex);
}

void TextureImpl::generateMipmaps()
{
    if (TextureUsage::RENDER_TARGET == _desc.textureUsage || !isColorRenderable(_desc.pixelFormat))
        return;

    UtilsMTL::generateMipmaps(_mtlTexture);
    _overrideMipLevels = RHIUtils::computeMipLevels(_desc.width, _desc.height);
}

void TextureImpl::updateFaceData(TextureCubeFace side, const void* data)
{
    ensureNativeTexture();
    NSUInteger slice = static_cast<int>(side);

    auto bytesPerRow = RHIUtils::computeRowPitch(_desc.pixelFormat, static_cast<uint32_t>(_desc.width));
    auto slicePitch  = bytesPerRow * _desc.height;

    auto region = MTLRegionMake2D(0, 0, _desc.width, _desc.height);
    [_mtlTexture replaceRegion:region
                   mipmapLevel:0
                         slice:slice
                     withBytes:data
                   bytesPerRow:bytesPerRow
                 bytesPerImage:slicePitch];

    if (shouldGenMipmaps())
        generateMipmaps();
}

void TextureImpl::ensureNativeTexture()
{
    if (_mtlTexture)
        return;
    MTLPixelFormat pixelFormat = UtilsMTL::toMTLPixelFormat(_desc.pixelFormat);
    if (pixelFormat == MTLPixelFormatInvalid)
        return;

    bool needMipmaps = _desc.mipLevels != 1;

    MTLTextureDescriptor* textureDesc = nil;
    switch (_desc.textureType)
    {
    case TextureType::TEXTURE_2D:
        textureDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:pixelFormat
                                                                         width:_desc.width
                                                                        height:_desc.height
                                                                     mipmapped:needMipmaps];
        if (_desc.arraySize > 1)
        {
            textureDesc.textureType = MTLTextureType2DArray;
            textureDesc.arrayLength = _desc.arraySize;
        }
        break;
    case TextureType::TEXTURE_CUBE:
        textureDesc = [MTLTextureDescriptor textureCubeDescriptorWithPixelFormat:pixelFormat
                                                                            size:_desc.width
                                                                       mipmapped:needMipmaps];
        break;
    default:
        return;
    }

    if (TextureUsage::RENDER_TARGET == _desc.textureUsage)
    {
        // DepthStencil, and Multisample textures must be allocated with the MTLResourceStorageModePrivate resource
        // option
        if (PixelFormat::D24S8 == _desc.pixelFormat && _desc.textureType == TextureType::TEXTURE_2D)
            textureDesc.resourceOptions = MTLResourceStorageModePrivate;
        textureDesc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
    }

    _mtlTexture = [_mtlDevice newTextureWithDescriptor:textureDesc];
}

}  // namespace ax::rhi::mtl
