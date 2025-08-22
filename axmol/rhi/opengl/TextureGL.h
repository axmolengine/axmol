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

#include <array>
#include "axmol/rhi/Texture.h"
#include "axmol/platform/GL.h"
#include "axmol/base/EventListenerCustom.h"

#include "axmol/rhi/opengl/OpenGLState.h"

namespace ax::rhi::gl
{

struct NativeTextureDesc
{
    GLenum target        = GL_TEXTURE_2D;
    GLint internalFormat = GL_RGBA;
    GLenum format        = GL_RGBA;
    GLenum type          = GL_UNSIGNED_BYTE;
};

/**
 * @addtogroup _opengl
 * @{
 */

/**
 * The texture implementation supports 2D, cubemap, and 2D array textures.
 */
class TextureImpl : public rhi::Texture
{
public:
    /**
     * @param desc Specifies the texture description.
     */
    TextureImpl(const TextureDesc& desc);
    ~TextureImpl() override;

    /**
     * Update a two-dimensional texture image
     * @param data Specifies a pointer to the image data in memory.
     * @param width Specifies the width of the texture image.
     * @param height Specifies the height of the texture image.
     * @param level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap
     * @param layerIndex Specifies the layer index for 2D array textures.
     * reduction image.
     */
    void updateData(const void* data,
                    int width,
                    int height,
                    int level,
                    int layerIndex) override;

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
    void updateCompressedData(const void* data,
                              int width,
                              int height,
                              std::size_t dataSize,
                              int level,
                              int layerIndex) override;

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
    void updateSubData(int xoffset,
                       int yoffset,
                       int width,
                       int height,
                       int level,
                       const void* data,
                       int layerIndex) override;

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
    void updateCompressedSubData(int xoffset,
                                 int yoffset,
                                 int width,
                                 int height,
                                 std::size_t dataSize,
                                 int level,
                                 const void* data,
                                 int layerIndex) override;

    void updateFaceData(TextureCubeFace side, const void* data) override;

    /**
     * Update sampler
     * @param sampler Specifies the sampler descriptor.
     */
    void updateSamplerDesc(const SamplerDesc& sampler) override;

    /**
     * Generate mipmaps.
     */
    void generateMipmaps();

    /**
     * Update texture description.
     * @param descriptor Specifies texture and sampler descriptor.
     */
    void updateTextureDesc(const TextureDesc& descriptor) override;

    void invalidate() override;

    /**
     * Get internal texture object handle.
     * @return Texture object.
     */
    GLuint internalHandle() const { return _nativeTexture; }

    /**
     * Set texture to pipeline
     * @param index Specifies the texture image unit selector.
     * @param target GL_TEXTURE_2D or GL_TEXTURE_CUBE_MAP
     */
    void apply(int slot) const;

    void ensureNativeTexture(size_t imageSize = 0);

private:
    void configureUnpackAlignment(unsigned int width);

    NativeTextureDesc _nativeDesc{};
    GLuint _nativeTexture{0};
    GLuint _nativeSampler{0};  // weak ref
};

// end of _opengl group
/// @}
}  // namespace ax::rhi::gl
