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

#include "axmol/rhi/opengl/TextureGL.h"
#include "axmol/base/EventListenerCustom.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/EventType.h"
#include "axmol/base/Director.h"
#include "axmol/platform/PlatformConfig.h"
#include "axmol/rhi/opengl/MacrosGL.h"
#include "axmol/rhi/opengl/UtilsGL.h"
#include "axmol/rhi/SamplerCache.h"

namespace ax::rhi::gl {

#define ISPOW2(n) (((n) & (n - 1)) == 0)

namespace
{
bool isMipmapEnabled(GLint filter)
{
    switch (filter)
    {
    case GL_LINEAR_MIPMAP_LINEAR:
    case GL_LINEAR_MIPMAP_NEAREST:
    case GL_NEAREST_MIPMAP_NEAREST:
    case GL_NEAREST_MIPMAP_LINEAR:
        return true;
    default:
        break;
    }
    return false;
}
}  // namespace

/// CLASS TextureInfoGL
void TextureInfoGL::applySampler(const SamplerDesc& desc, bool isPow2, bool hasMipmaps)
{
    this->sampler = static_cast<GLuint>(reinterpret_cast<uintptr_t>(SamplerCache::getInstance()->getSampler(desc)));
}

void TextureInfoGL::apply(int slot, int index) const
{
    __state->activeTexture(GL_TEXTURE0 + slot);
    __state->bindTexture(this->target, index < AX_META_TEXTURES ? textures[index] : textures[0]);
    __state->bindSampler(slot, this->sampler);
}

GLuint TextureInfoGL::ensure(int index)
{
    if (index >= AX_META_TEXTURES)
        return 0;
    // glActiveTexture(GL_TEXTURE0 + index);
    auto& texID = this->textures[index];
    if (!texID)
        glGenTextures(1, &texID);
    __state->bindTexture(this->target, texID);

    if (this->maxIdx < index)
        this->maxIdx = index;

    return texID;
}

void TextureInfoGL::onRendererRecreated()
{
    int idx = 0;
    for (auto&& texID : textures)
    {
        if (texID)
        {
            // NOTE: glDeleteTextures() doesn't need to be called here, because the textures were
            // destroyed when the GL context was lost.
            texID = 0;
            ensure(idx);
        }
        ++idx;
    }
}

/// CLASS TextureImpl
TextureImpl::TextureImpl(const TextureDesc& descriptor)
{
    _textureType = descriptor.textureType;
    updateTextureDesc(descriptor);

#if AX_ENABLE_CACHE_TEXTURE_DATA
    // Listen this event to restored texture id after coming to foreground on GLES.
    _rendererRecreatedListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom*) {
        _textureInfo.onRendererRecreated();
        if (_usedForRT)
            this->ensureTexStorageForRT();
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_rendererRecreatedListener, -1);
#endif
}

void TextureImpl::updateTextureDesc(const TextureDesc& descriptor, int index)
{
    _textureInfo.target = _textureType == rhi::TextureType::TEXTURE_2D ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP;

    assert(descriptor.textureType == rhi::TextureType::TEXTURE_2D || _width == _height);

    Texture::updateTextureDesc(descriptor, index);

    UtilsGL::toGLTypes(descriptor.textureFormat, _textureInfo.internalFormat, _textureInfo.format, _textureInfo.type);

    updateSamplerDesc(descriptor.samplerDesc);

    const bool useForRT = descriptor.textureUsage == TextureUsage::RENDER_TARGET;
#if AX_ENABLE_CACHE_TEXTURE_DATA
    _usedForRT = useForRT;
#endif
    if(useForRT) {
        ensureTexStorageForRT();
    }
}

void TextureImpl::ensureTexStorageForRT()
{
    auto size     = _width * _height * _bitsPerPixel / 8;
    assert(size > 0);

    uint8_t* data = (uint8_t*)malloc(size);
    memset(data, 0, size);
    updateData(data, _width, _height, 0);
    free(data);
}

TextureImpl::~TextureImpl()
{
#if AX_ENABLE_CACHE_TEXTURE_DATA
    Director::getInstance()->getEventDispatcher()->removeEventListener(_rendererRecreatedListener);
#endif
    _textureInfo.destroy();
}

void TextureImpl::updateSamplerDesc(const SamplerDesc& sampler)
{
    bool isPow2 = ISPOW2(_width) && ISPOW2(_height);
    _textureInfo.applySampler(sampler, isPow2, _hasMipmaps);
}

void TextureImpl::updateData(uint8_t* data, std::size_t width, std::size_t height, std::size_t level, int index)
{
    assert(_textureInfo.target == GL_TEXTURE_2D);
    if (!_textureInfo.ensure(index))
        return;

    // Set the row align only when mipmapsNum == 1 and the data is uncompressed
    auto mipmapEnabled = _hasMipmaps; // TODO: mipLevels
    if (!mipmapEnabled)
    {
        unsigned int bytesPerRow = width * _bitsPerPixel / 8;

        if (bytesPerRow % 8 == 0)
        {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
        }
        else if (bytesPerRow % 4 == 0)
        {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        }
        else if (bytesPerRow % 2 == 0)
        {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
        }
        else
        {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        }
    }
    else
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    glTexImage2D(GL_TEXTURE_2D, level, _textureInfo.internalFormat, width, height,
                 0,  // border
                 _textureInfo.format, _textureInfo.type, data);

    CHECK_GL_ERROR_DEBUG();

#if AX_ENABLE_CACHE_TEXTURE_DATA
    if (_generateMipmaps)
    {
        _hasMipmaps = false;
        generateMipmaps();
    }
#endif

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
    assert(_textureInfo.target == GL_TEXTURE_2D);
    if (!_textureInfo.ensure(index))
        return;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glCompressedTexImage2D(GL_TEXTURE_2D, level, _textureInfo.internalFormat, (GLsizei)width, (GLsizei)height,
                           0,  // border
                           dataLen, data);
    CHECK_GL_ERROR_DEBUG();

    if (!_hasMipmaps && level > 0)
        _hasMipmaps = true;
}

void TextureImpl::updateSubData(std::size_t xoffset,
                                std::size_t yoffset,
                                std::size_t width,
                                std::size_t height,
                                std::size_t level,
                                uint8_t* data,
                                int index)
{
    assert(_textureInfo.target == GL_TEXTURE_2D);
    if (!_textureInfo.ensure(index))
        return;

    // !IMPORTANT: Set the unpack alignment to 1 byte to avoid issues with width or height no align with 4.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, _textureInfo.format, _textureInfo.type,
                    data);
    CHECK_GL_ERROR_DEBUG();

    if (!_hasMipmaps && level > 0)
        _hasMipmaps = true;
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
    assert(_textureInfo.target == GL_TEXTURE_2D);
    if (!_textureInfo.ensure(index))
        return;

    glCompressedTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, _textureInfo.internalFormat,
                              dataLen, data);
    CHECK_GL_ERROR_DEBUG();

    if (!_hasMipmaps && level > 0)
        _hasMipmaps = true;
}

void TextureImpl::updateFaceData(TextureCubeFace side, void* data, int index)
{
    assert(_textureInfo.target == GL_TEXTURE_CUBE_MAP);
    if (!_textureInfo.ensure(index))
        return;

    CHECK_GL_ERROR_DEBUG();
    int i = static_cast<int>(side);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                 0,  // level
                 _textureInfo.internalFormat, _width, _height,
                 0,  // border
                 _textureInfo.format, _textureInfo.type, data);

    CHECK_GL_ERROR_DEBUG();
    __state->bindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void TextureImpl::generateMipmaps()
{
    if (TextureUsage::RENDER_TARGET == _textureUsage)
        return;

    if (!_hasMipmaps)
    {
#if AX_ENABLE_CACHE_TEXTURE_DATA
        _generateMipmaps = true;
#endif
        _hasMipmaps = true;
        __state->bindTexture(_textureInfo.target, (GLuint)this->internalHandle());
        glGenerateMipmap(_textureInfo.target);
    }
}

}
