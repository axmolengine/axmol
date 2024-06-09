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

#pragma once

#include "../Texture.h"
#include "DriverMTL.h"
#import <Metal/Metal.h>
#include <array>

NS_AX_BACKEND_BEGIN
/**
 * @addtogroup _metal
 * @{
 */

enum
{
    MTL_TEXTURE_2D = 1,
    MTL_TEXTURE_CUBE,
};

struct TextureInfoMTL
{
    TextureInfoMTL(id<MTLDevice> mtlDevice)
    {
        _mtlDevice = mtlDevice;
        _mtlTextures.fill(nil);
    }
    ~TextureInfoMTL() { destroy(); }

    id<MTLTexture> ensure(int index, int target);
    void destroy();

    id<MTLTexture> createTexture(id<MTLDevice> mtlDevice, const TextureDescriptor& descriptor, int target);
    void recreateSampler(const SamplerDescriptor& descriptor);

    MTLSamplerAddressMode _sAddressMode;
    MTLSamplerAddressMode _tAddressMode;
    MTLSamplerMinMagFilter _minFilter;
    MTLSamplerMinMagFilter _magFilter;
    MTLSamplerMipFilter _mipFilter;

    TextureDescriptor _descriptor;

    id<MTLDevice> _mtlDevice;
    std::array<id<MTLTexture>, AX_META_TEXTURES + 1> _mtlTextures;
    int _maxIdx = -1;

    id<MTLSamplerState> _mtlSamplerState = nil;
    unsigned int _bytesPerRow            = 0;
};

/**
 * A 2D texture
 */
class TextureMTL : public backend::Texture2DBackend
{
public:
    /**
     * @param mtlDevice The device for which MTLTexture and MTLSamplerState object was created.
     * @param descriptor Specify texture and sampler description.
     */
    TextureMTL(id<MTLDevice> mtlDevice, const TextureDescriptor& descriptor);
    ~TextureMTL();

    /**
     * Update a two-dimensional texture image
     * @param data Specifies a pointer to the image data in memory.
     * @param width Specifies the width of the texture image.
     * @param height Specifies the height of the texture image.
     * @param level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap
     * reduction image.
     */
    virtual void updateData(uint8_t* data,
                            std::size_t width,
                            std::size_t height,
                            std::size_t level,
                            int index = 0) override;

    /**
     * Update a two-dimensional texture image in a compressed format
     * @param data Specifies a pointer to the compressed image data in memory.
     * @param width Specifies the width of the texture image.
     * @param height Specifies the height of the texture image.
     * @param dataLen Specifies the totoal size of compressed image in bytes.
     * @param level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap
     * reduction image.
     */
    virtual void updateCompressedData(uint8_t* data,
                                      std::size_t width,
                                      std::size_t height,
                                      std::size_t dataLen,
                                      std::size_t level,
                                      int index = 0) override;

    /**
     * Update a two-dimensional texture subimage
     * @param xoffset Specifies a texel offset in the x direction within the texture array.
     * @param yoffset Specifies a texel offset in the y direction within the texture array.
     * @param width Specifies the width of the texture subimage.
     * @param height Specifies the height of the texture subimage.
     * @param level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap
     * reduction image.
     * @param data Specifies a pointer to the image data in memory.
     */
    virtual void updateSubData(std::size_t xoffset,
                               std::size_t yoffset,
                               std::size_t width,
                               std::size_t height,
                               std::size_t level,
                               uint8_t* data,
                               int index = 0) override;

    /**
     * Update a two-dimensional texture subimage in a compressed format
     * @param xoffset Specifies a texel offset in the x direction within the texture array.
     * @param yoffset Specifies a texel offset in the y direction within the texture array.
     * @param width Specifies the width of the texture subimage.
     * @param height Specifies the height of the texture subimage.
     * @param dataLen Specifies the totoal size of compressed subimage in bytes.
     * @param level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap
     * reduction image.
     * @param data Specifies a pointer to the compressed image data in memory.
     */
    virtual void updateCompressedSubData(std::size_t xoffset,
                                         std::size_t yoffset,
                                         std::size_t width,
                                         std::size_t height,
                                         std::size_t dataLen,
                                         std::size_t level,
                                         uint8_t* data,
                                         int index = 0) override;

    /**
     * Update sampler
     * @param sampler Specifies the sampler descriptor.
     */
    virtual void updateSamplerDescriptor(const SamplerDescriptor& sampler) override;

    /**
     * Generate mipmaps.
     */
    virtual void generateMipmaps() override;

    /**
     * Update texture description.
     * @param descriptor Specifies texture and sampler descriptor.
     */
    virtual void updateTextureDescriptor(const ax::backend::TextureDescriptor& descriptor, int index = 0) override;

    int getCount() const override { return _textureInfo._maxIdx + 1; }

    /**
     * Get MTLTexture object. reinterpret_cast<id<MTLTexture>>(handler);
     * @return A MTLTexture object.
     */
    uintptr_t getHandler(int index = 0) const override
    {
        return reinterpret_cast<uintptr_t>((void*)_textureInfo._mtlTextures[index]);
    }

    /**
     * Get MTLSamplerState object
     * @return A MTLSamplerState object.
     */
    inline id<MTLSamplerState> getMTLSamplerState() const { return _textureInfo._mtlSamplerState; }

private:
    TextureInfoMTL _textureInfo;
};

/**
 * A texture cube
 */
class TextureCubeMTL : public backend::TextureCubemapBackend
{
public:
    /**
     * @param mtlDevice The device for which MTLTexture and MTLSamplerState object was created.
     * @param descriptor Specify texture and sampler description.
     */
    TextureCubeMTL(id<MTLDevice> mtlDevice, const TextureDescriptor& descriptor);
    ~TextureCubeMTL();

    /**
     * Update sampler
     * @param sampler Specifies the sampler descriptor.
     */
    virtual void updateSamplerDescriptor(const SamplerDescriptor& sampler) override;

    /**
     * Update texutre cube data in give slice side.
     * @param side Specifies which slice texture of cube to be update.
     * @param data Specifies a pointer to the image data in memory.
     */
    virtual void updateFaceData(TextureCubeFace side, void* data, int index = 0) override;

    /**
     * Generate mipmaps.
     */
    virtual void generateMipmaps() override;

    /**
     * Update texture description.
     * @param descriptor Specifies texture and sampler descriptor.
     */
    virtual void updateTextureDescriptor(const ax::backend::TextureDescriptor& descriptor, int index = 0) override;

    int getCount() const override { return _textureInfo._maxIdx + 1; }

    uintptr_t getHandler(int index = 0) const override
    {
        return reinterpret_cast<uintptr_t>((void*)_textureInfo._mtlTextures[index]);
    }

    /**
     * Get MTLSamplerState object
     * @return A MTLSamplerState object.
     */
    inline id<MTLSamplerState> getMTLSamplerState() const { return _textureInfo._mtlSamplerState; }

private:
    TextureInfoMTL _textureInfo;

    MTLRegion _region;
    std::size_t _bytesPerImage = 0;
};

// end of _metal group
/// @}
NS_AX_BACKEND_END
