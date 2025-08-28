/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md)

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

#include "axmol/rhi/RHITypes.h"
#include "axmol/base/Object.h"
#include <cassert>

#include <functional>

namespace ax::rhi
{
/**
 * @addtogroup _rhi
 * @{
 */

/**
 * A base texture
 */
class Texture : public Object
{
public:
    /**
     * Update sampler
     * @param sampler Specifies the sampler descriptor.
     */
    virtual void updateSamplerDesc(const SamplerDesc& desc) = 0;

    /**
     * Update texture description.
     * @param descriptor Specifies texture and sampler descriptor.
     */
    virtual void updateTextureDesc(const TextureDesc& desc);

    /**
     * Get texture format.
     * @return Texture pixel format.
     */
    inline PixelFormat getPixelFormat() const { return _desc.pixelFormat; }

    /**
     * Get texture usage. Symbolic constant can be READ, WRITE or RENDER_TARGET.
     * @return Texture usage.
     */
    inline TextureUsage getTextureUsage() const { return _desc.textureUsage; }

    /**
     * Get texture type. Symbolic constant value can be either TEXTURE_2D or TEXTURE_CUBE.
     * @return Texture type.
     */
    inline TextureType getTextureType() const { return _desc.textureType; }

    /**
     * Check if mipmap had generated before.
     * @return true if the mipmap has always generated before, otherwise false.
     */
    inline bool hasMipmaps() const { return _overrideMipLevels > 1; }

    int getWidth() const { return static_cast<int>(_desc.width); }
    int getHeight() const { return static_cast<int>(_desc.height); }

    /**
     * Update a two-dimensional texture image
     * @param data Specifies a pointer to the image data in memory.
     * @param width Specifies the width of the texture image.
     * @param height Specifies the height of the texture image.
     * @param level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap
     * reduction image.
     * @param layerIndex Specifies the layer index for 2D array textures.
     */
    virtual void updateData(const void* data, int width, int height, int level, int layerIndex) = 0;

    /**
     * Update a two-dimensional texture image in a compressed format
     * @param data Specifies a pointer to the compressed image data in memory.
     * @param width Specifies the width of the texture image.
     * @param height Specifies the height of the texture image.
     * @param dataSize Specifies the totoal size of compressed image in bytes.
     * @param level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap
     * reduction image.
     * @param layerIndex Specifies the layer index for 2D array textures.
     */
    virtual void updateCompressedData(const void* data,
                                      int width,
                                      int height,
                                      std::size_t dataSize,
                                      int level,
                                      int layerIndex) = 0;

    /**
     * Update a two-dimensional texture subimage
     * @param xoffset Specifies a texel offset in the x direction within the texture array.
     * @param yoffset Specifies a texel offset in the y direction within the texture array.
     * @param width Specifies the width of the texture subimage.
     * @param height Specifies the height of the texture subimage.
     * @param level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap
     * reduction image.
     * @param data Specifies a pointer to the image data in memory.
     * @param layerIndex Specifies the layer index for 2D array textures.
     */
    virtual void
    updateSubData(int xoffset, int yoffset, int width, int height, int level, const void* data, int layerIndex) = 0;

    /**
     * Update a two-dimensional texture subimage in a compressed format
     * @param xoffset Specifies a texel offset in the x direction within the texture array.
     * @param yoffset Specifies a texel offset in the y direction within the texture array.
     * @param width Specifies the width of the texture subimage.
     * @param height Specifies the height of the texture subimage.
     * @param dataSize Specifies the totoal size of compressed subimage in bytes.
     * @param level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap
     * reduction image.
     * @param data Specifies a pointer to the compressed image data in memory.
     * @param layerIndex Specifies the layer index for 2D array textures.
     */
    virtual void updateCompressedSubData(int xoffset,
                                         int yoffset,
                                         int width,
                                         int height,
                                         std::size_t dataSize,
                                         int level,
                                         const void* data,
                                         int layerIndex) = 0;

    /**
     * Update texutre cube data in give slice side.
     * @param side Specifies which slice texture of cube to be update.
     * @param data Specifies a pointer to the image data in memory.
     */
    virtual void updateFaceData(TextureCubeFace side, const void* data) = 0;

    virtual void invalidate();

    bool shouldGenMipmaps(int level = 0) const { return _desc.mipLevels == 0 && level == 0 && !_overrideMipLevels; }

    /**
     * clear texture data to zero.
     * @remark: only works for uncompressed texture.
     */
    void zeroTexData();

protected:
    /**
     * @param descriptor Specifies the texture descriptor.
     */
    Texture() {}
    virtual ~Texture();

    /// The bytes of all components.
    uint8_t _bitsPerPixel       = 0;
    uint16_t _overrideMipLevels = 0;
    TextureDesc _desc;
};

// end of _rhi group
/// @}
}  // namespace ax::rhi
