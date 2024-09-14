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

#include "TextureGL.h"
#include "base/EventListenerCustom.h"
#include "base/EventDispatcher.h"
#include "base/EventType.h"
#include "base/Director.h"
#include "platform/PlatformConfig.h"
#include "renderer/backend/opengl/MacrosGL.h"
#include "renderer/backend/opengl/UtilsGL.h"

NS_AX_BACKEND_BEGIN

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
void TextureInfoGL::applySampler(const SamplerDescriptor& descriptor, bool isPow2, bool hasMipmaps, GLenum target)
{
    if (descriptor.magFilter != SamplerFilter::DONT_CARE)
    {
        magFilterGL = UtilsGL::toGLMagFilter(descriptor.magFilter);
    }

    if (descriptor.minFilter != SamplerFilter::DONT_CARE)
    {
        minFilterGL = UtilsGL::toGLMinFilter(descriptor.minFilter, hasMipmaps, isPow2);
    }

    if (descriptor.sAddressMode != SamplerAddressMode::DONT_CARE)
    {
        sAddressModeGL = UtilsGL::toGLAddressMode(descriptor.sAddressMode, isPow2);
    }

    if (descriptor.tAddressMode != SamplerAddressMode::DONT_CARE)
    {
        tAddressModeGL = UtilsGL::toGLAddressMode(descriptor.tAddressMode, isPow2);
    }

    // apply sampler for all internal textures
    foreachTextures([this, target](GLuint texID, int index) {
        __gl->bindTexture(target, textures[index]);
        setCurrentTexParameters(target);
    });
}

void TextureInfoGL::setCurrentTexParameters(GLenum target)
{
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilterGL);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilterGL);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, sAddressModeGL);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, tAddressModeGL);
}

void TextureInfoGL::apply(int slot, int index, GLenum target) const
{
    __gl->activeTexture(GL_TEXTURE0 + slot);
    __gl->bindTexture(target, index < AX_META_TEXTURES ? textures[index] : textures[0]);
}

GLuint TextureInfoGL::ensure(int index, GLenum target)
{
    if (index >= AX_META_TEXTURES)
        return 0;
    // glActiveTexture(GL_TEXTURE0 + index);
    auto& texID = this->textures[index];
    if (!texID)
        glGenTextures(1, &texID);
    __gl->bindTexture(target, texID);

    setCurrentTexParameters(target);  // set once

    if (this->maxIdx < index)
        this->maxIdx = index;

    return texID;
}

void TextureInfoGL::onRendererRecreated(GLenum target)
{
    int idx = 0;
    for (auto&& texID : textures)
    {
        if (texID)
        {
            // NOTE: glDeleteTextures() doesn't need to be called here, because the textures were
            // destroyed when the GL context was lost.
            texID = 0;
            ensure(idx, target);
        }
        ++idx;
    }
}

/// CLASS Texture2DGL
Texture2DGL::Texture2DGL(const TextureDescriptor& descriptor)
{
    updateTextureDescriptor(descriptor);

#if AX_ENABLE_CACHE_TEXTURE_DATA
    // Listen this event to restored texture id after coming to foreground on GLES.
    _rendererRecreatedListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom*) {
        _textureInfo.onRendererRecreated(GL_TEXTURE_2D);
        if (_usedForRT)
            this->ensureTexStorageForRT();
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_rendererRecreatedListener, -1);
#endif
}

void Texture2DGL::updateTextureDescriptor(const ax::backend::TextureDescriptor& descriptor, int index)
{
    TextureBackend::updateTextureDescriptor(descriptor, index);

    UtilsGL::toGLTypes(descriptor.textureFormat, _textureInfo.internalFormat, _textureInfo.format, _textureInfo.type,
                       _isCompressed);

    updateSamplerDescriptor(descriptor.samplerDescriptor);

    const bool useForRT = descriptor.textureUsage == TextureUsage::RENDER_TARGET;
#if AX_ENABLE_CACHE_TEXTURE_DATA
    _usedForRT = useForRT;
#endif
    if(useForRT) {
        ensureTexStorageForRT();
    }
}

void Texture2DGL::ensureTexStorageForRT()
{
    auto size     = _width * _height * _bitsPerPixel / 8;
    assert(size > 0);

    uint8_t* data = (uint8_t*)malloc(size);
    memset(data, 0, size);
    updateData(data, _width, _height, 0);
    free(data);
}

Texture2DGL::~Texture2DGL()
{
#if AX_ENABLE_CACHE_TEXTURE_DATA
    Director::getInstance()->getEventDispatcher()->removeEventListener(_rendererRecreatedListener);
#endif
    _textureInfo.destroy(GL_TEXTURE_2D);
}

void Texture2DGL::updateSamplerDescriptor(const SamplerDescriptor& sampler)
{
    bool isPow2 = ISPOW2(_width) && ISPOW2(_height);
    _textureInfo.applySampler(sampler, isPow2, _hasMipmaps, GL_TEXTURE_2D);
}

void Texture2DGL::updateData(uint8_t* data, std::size_t width, std::size_t height, std::size_t level, int index)
{
    if (!_textureInfo.ensure(index, GL_TEXTURE_2D))
        return;

    // Set the row align only when mipmapsNum == 1 and the data is uncompressed
    auto mipmapEnabled = isMipmapEnabled(_textureInfo.minFilterGL) || isMipmapEnabled(_textureInfo.magFilterGL);
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

void Texture2DGL::updateCompressedData(uint8_t* data,
                                       std::size_t width,
                                       std::size_t height,
                                       std::size_t dataLen,
                                       std::size_t level,
                                       int index)
{
    if (!_textureInfo.ensure(index, GL_TEXTURE_2D))
        return;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glCompressedTexImage2D(GL_TEXTURE_2D, level, _textureInfo.internalFormat, (GLsizei)width, (GLsizei)height,
                           0,  // border
                           dataLen, data);
    CHECK_GL_ERROR_DEBUG();

    if (!_hasMipmaps && level > 0)
        _hasMipmaps = true;
}

void Texture2DGL::updateSubData(std::size_t xoffset,
                                std::size_t yoffset,
                                std::size_t width,
                                std::size_t height,
                                std::size_t level,
                                uint8_t* data,
                                int index)
{
    if (!_textureInfo.ensure(index, GL_TEXTURE_2D))
        return;

    glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, _textureInfo.format, _textureInfo.type,
                    data);
    CHECK_GL_ERROR_DEBUG();

    if (!_hasMipmaps && level > 0)
        _hasMipmaps = true;
}

void Texture2DGL::updateCompressedSubData(std::size_t xoffset,
                                          std::size_t yoffset,
                                          std::size_t width,
                                          std::size_t height,
                                          std::size_t dataLen,
                                          std::size_t level,
                                          uint8_t* data,
                                          int index)
{
    if (!_textureInfo.ensure(index, GL_TEXTURE_2D))
        return;

    glCompressedTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, _textureInfo.internalFormat,
                              dataLen, data);
    CHECK_GL_ERROR_DEBUG();

    if (!_hasMipmaps && level > 0)
        _hasMipmaps = true;
}

void Texture2DGL::generateMipmaps()
{
    if (TextureUsage::RENDER_TARGET == _textureUsage)
        return;

    if (!_hasMipmaps)
    {
#if AX_ENABLE_CACHE_TEXTURE_DATA
        _generateMipmaps = true;
#endif
        _hasMipmaps = true;
        __gl->bindTexture(GL_TEXTURE_2D, (GLuint)this->getHandler());
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

/// CLASS TextureCubeGL
TextureCubeGL::TextureCubeGL(const TextureDescriptor& descriptor)
{
    assert(_width == _height);
    _textureType = TextureType::TEXTURE_CUBE;
    updateTextureDescriptor(descriptor);

#if AX_ENABLE_CACHE_TEXTURE_DATA
    // Listen this event to restored texture id after coming to foreground on Android.
    _rendererRecreatedListener = EventListenerCustom::create(
        EVENT_RENDERER_RECREATED, [this](EventCustom*) { _textureInfo.onRendererRecreated(GL_TEXTURE_CUBE_MAP); });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_rendererRecreatedListener, -1);
#endif
    CHECK_GL_ERROR_DEBUG();
}

void TextureCubeGL::updateTextureDescriptor(const ax::backend::TextureDescriptor& descriptor, int index)
{
    backend::TextureCubemapBackend::updateTextureDescriptor(descriptor, index);

    UtilsGL::toGLTypes(descriptor.textureFormat, _textureInfo.internalFormat, _textureInfo.format, _textureInfo.type,
                       _isCompressed);

    updateSamplerDescriptor(descriptor.samplerDescriptor);
}

TextureCubeGL::~TextureCubeGL()
{
#if AX_ENABLE_CACHE_TEXTURE_DATA
    Director::getInstance()->getEventDispatcher()->removeEventListener(_rendererRecreatedListener);
#endif
    _textureInfo.destroy(GL_TEXTURE_CUBE_MAP);
}

void TextureCubeGL::updateSamplerDescriptor(const SamplerDescriptor& sampler)
{
    _textureInfo.applySampler(sampler, true, _hasMipmaps, GL_TEXTURE_CUBE_MAP);
}

void TextureCubeGL::updateFaceData(TextureCubeFace side, void* data, int index)
{
    if (!_textureInfo.ensure(index, GL_TEXTURE_CUBE_MAP))
        return;

    CHECK_GL_ERROR_DEBUG();
    int i = static_cast<int>(side);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                 0,  // level
                 _textureInfo.internalFormat, _width, _height,
                 0,  // border
                 _textureInfo.format, _textureInfo.type, data);

    CHECK_GL_ERROR_DEBUG();
    __gl->bindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void TextureCubeGL::generateMipmaps()
{
    if (TextureUsage::RENDER_TARGET == _textureUsage)
        return;

    if (!_hasMipmaps)
    {
        _hasMipmaps = true;
        __gl->bindTexture(GL_TEXTURE_CUBE_MAP, (GLuint)this->getHandler());
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
}

NS_AX_BACKEND_END
