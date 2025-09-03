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

#include "axmol/rhi/Texture.h"
#include "axmol/rhi/metal/DriverMTL.h"
#import <Metal/Metal.h>

namespace ax::rhi::mtl
{
/**
 * @addtogroup _metal
 * @{
 */

/**
 * The texture implementation supports 2D, cubemap, and 2D array textures.
 */
class TextureImpl : public rhi::Texture
{
public:
    /**
     * @param mtlDevice The device for which MTLTexture and MTLSamplerState object was created.
     * @param descriptor Specify texture and sampler description.
     */
    TextureImpl(id<MTLDevice> mtlDevice, const TextureDesc& descriptor);
    ~TextureImpl();

    /**
     * Update a two-dimensional texture image
     * @param data Specifies a pointer to the image data in memory.
     * @param width Specifies the width of the texture image.
     * @param height Specifies the height of the texture image.
     * @param level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap
     * reduction image.
     */
    void updateData(const void* data, int width, int height, int level, int layerIndex = 0) override;

    /**
     * Update a two-dimensional texture image in a compressed format
     * @param data Specifies a pointer to the compressed image data in memory.
     * @param width Specifies the width of the texture image.
     * @param height Specifies the height of the texture image.
     * @param dataSize Specifies the totoal size of compressed image in bytes.
     * @param level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap
     * reduction image.
     */
    void updateCompressedData(const void* data,
                              int width,
                              int height,
                              std::size_t dataSize,
                              int level,
                              int layerIndex = 0) override;

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
    void updateSubData(int xoffset, int yoffset, int width, int height, int level, const void* data, int layerIndex = 0)
        override;

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
     */
    void updateCompressedSubData(int xoffset,
                                 int yoffset,
                                 int width,
                                 int height,
                                 std::size_t dataSize,
                                 int level,
                                 const void* data,
                                 int layerIndex = 0) override;

    /**
     * Update sampler
     * @param desc Specifies the sampler descriptor.
     */
    void updateSamplerDesc(const SamplerDesc& desc) override;

    /**
     * Generate mipmaps.
     */
    void generateMipmaps();

    /**
     * Update texture description.
     * @param desc Specifies texture and sampler descriptor.
     */
    void updateTextureDesc(const TextureDesc& desc) override;

    /**
     * Update texutre cube data in give slice side.
     * @param side Specifies which slice texture of cube to be update.
     * @param data Specifies a pointer to the image data in memory.
     */
    void updateFaceData(TextureCubeFace side, const void* data) override;

    /**
     * Get MTLTexture object. reinterpret_cast<id<MTLTexture>>(handler);
     * @return A MTLTexture object.
     */
    id<MTLTexture> internalHandle() const { return _mtlTexture; }

    /**
     * Get MTLSamplerState object
     * @return A MTLSamplerState object.
     */
    inline id<MTLSamplerState> internalSampler() const { return _mtlSamplerState; }

private:
    void ensureNativeTexture();

    id<MTLDevice> _mtlDevice;
    id<MTLTexture> _mtlTexture           = nil;
    id<MTLSamplerState> _mtlSamplerState = nil;
};

// end of _metal group
/// @}
}  // namespace ax::rhi::mtl
