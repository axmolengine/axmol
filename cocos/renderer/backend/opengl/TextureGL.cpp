/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

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
#include "base/ccMacros.h"
#include "base/CCEventListenerCustom.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventType.h"
#include "base/CCDirector.h"
#include "platform/CCPlatformConfig.h"
#include "renderer/backend/opengl/UtilsGL.h"

CC_BACKEND_BEGIN

#define ISPOW2(n) (((n) & (n-1)) == 0)

namespace {
    bool isMipmapEnabled(GLint filter)
    {
        switch(filter)
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
}

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
    foreach([=](GLuint texID, int index) {
        glBindTexture(target, textures[index]);

        setCurrentTexParameters(target);

        glBindTexture(target, 0); // unbind
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
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(target, index < CC_META_TEXTURES ? textures[index] : textures[0]);
}

GLuint TextureInfoGL::ensure(int index, GLenum target)
{
    if (index >= CC_META_TEXTURES) return 0;
    // glActiveTexture(GL_TEXTURE0 + index);
    auto& texID = this->textures[index];
    if (!texID)
        glGenTextures(1, &texID);
    glBindTexture(target, texID);

    setCurrentTexParameters(target); // set once

    if (this->maxIdx < index) this->maxIdx = index;

    return texID;
}

void TextureInfoGL::recreateAll(GLenum target)
{
    int idx = 0;
    for (auto& texID : textures) {
        if (texID) {
            glDeleteTextures(1, &texID);
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

#if CC_ENABLE_CACHE_TEXTURE_DATA
    // Listen this event to restored texture id after coming to foreground on Android.
    _backToForegroundListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom*){
        _textureInfo.recreateAll(GL_TEXTURE_2D);
        this->initWithZeros();
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, -1);
#endif

    // Update data here because `updateData()` may not be invoked later.
    // For example, a texture used as depth buffer will not invoke updateData(), see cpp-tests 'Effect Basic/Effects Advanced'.
    // FIXME: Don't call at Texture2DGL::updateTextureDescriptor, when the texture is compressed, initWithZeros will cause GL Error: 0x501
    // We call at here once to ensure depth buffer works well.
    initWithZeros();
}

void Texture2DGL::initWithZeros()
{
    // Ensure the final data size at least 4 byte
    _width = (std::max)(_width, (uint32_t)1);
    _height = (std::max)(_height, (uint32_t)1);
    _bitsPerPixel = (std::max)(_bitsPerPixel, (uint8_t)(8 * 4));

    auto size = _width * _height * _bitsPerPixel / 8;
    uint8_t* data = (uint8_t*)malloc(size);
    memset(data, 0, size);
    updateData(data, _width, _height, 0);
    free(data);
}

void Texture2DGL::updateTextureDescriptor(const cocos2d::backend::TextureDescriptor &descriptor, int index)
{
    TextureBackend::updateTextureDescriptor(descriptor, index);

    UtilsGL::toGLTypes(descriptor.textureFormat, _textureInfo.internalFormat, _textureInfo.format, _textureInfo.type, _isCompressed);

    updateSamplerDescriptor(descriptor.samplerDescriptor);
}

Texture2DGL::~Texture2DGL()
{
#if CC_ENABLE_CACHE_TEXTURE_DATA
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}

void Texture2DGL::updateSamplerDescriptor(const SamplerDescriptor &sampler) {
    bool isPow2 = ISPOW2(_width) && ISPOW2(_height);
    _textureInfo.applySampler(sampler, isPow2, _hasMipmaps, GL_TEXTURE_2D);
}

void Texture2DGL::updateData(uint8_t* data, std::size_t width , std::size_t height, std::size_t level, int index)
{
    if (!_textureInfo.ensure(index, GL_TEXTURE_2D)) return;

    //Set the row align only when mipmapsNum == 1 and the data is uncompressed
    auto mipmapEnalbed = isMipmapEnabled(_textureInfo.minFilterGL) || isMipmapEnabled(_textureInfo.magFilterGL);
    if(!mipmapEnalbed)
    {
        unsigned int bytesPerRow = width * _bitsPerPixel / 8;

        if(bytesPerRow % 8 == 0)
        {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
        }
        else if(bytesPerRow % 4 == 0)
        {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        }
        else if(bytesPerRow % 2 == 0)
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

    glTexImage2D(GL_TEXTURE_2D,
                level,
                _textureInfo.internalFormat,
                width,
                height,
                0, // border
                _textureInfo.format,
                _textureInfo.type,
                data);

    CHECK_GL_ERROR_DEBUG();

    if(!_hasMipmaps && level > 0)
        _hasMipmaps = true;
}

void Texture2DGL::updateCompressedData(uint8_t *data, std::size_t width, std::size_t height,
                                       std::size_t dataLen, std::size_t level, int index)
{
    if (!_textureInfo.ensure(index, GL_TEXTURE_2D)) return;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glCompressedTexImage2D(GL_TEXTURE_2D,
                           level,
                           _textureInfo.internalFormat,
                           (GLsizei)width,
                           (GLsizei)height,
                           0, // border
                           dataLen,
                           data);
    CHECK_GL_ERROR_DEBUG();

    if(!_hasMipmaps && level > 0)
        _hasMipmaps = true;
}

void Texture2DGL::updateSubData(std::size_t xoffset, std::size_t yoffset, std::size_t width, std::size_t height, std::size_t level, uint8_t* data, int index)
{
    if (!_textureInfo.ensure(index, GL_TEXTURE_2D)) return;

    glTexSubImage2D(GL_TEXTURE_2D,
                    level,
                    xoffset,
                    yoffset,
                    width,
                    height,
                    _textureInfo.format,
                    _textureInfo.type,
                    data);
    CHECK_GL_ERROR_DEBUG();

    if(!_hasMipmaps && level > 0)
        _hasMipmaps = true;
}

void Texture2DGL::updateCompressedSubData(std::size_t xoffset, std::size_t yoffset, std::size_t width,
                                          std::size_t height, std::size_t dataLen, std::size_t level,
                                          uint8_t *data, int index)
{
    if (!_textureInfo.ensure(index, GL_TEXTURE_2D)) return;

    glCompressedTexSubImage2D(GL_TEXTURE_2D,
                              level,
                              xoffset,
                              yoffset,
                              width,
                              height,
                              _textureInfo.internalFormat,
                              dataLen,
                              data);
    CHECK_GL_ERROR_DEBUG();

    if(!_hasMipmaps && level > 0)
        _hasMipmaps = true;
}

void Texture2DGL::generateMipmaps()
{
    if (TextureUsage::RENDER_TARGET == _textureUsage)
        return;

    if(!_hasMipmaps)
    {
        _hasMipmaps = true;
        glBindTexture(GL_TEXTURE_2D, this->getHandler());
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

/// CLASS TextureCubeGL
TextureCubeGL::TextureCubeGL(const TextureDescriptor& descriptor)
{
    assert(_width == _height);
    _textureType = TextureType::TEXTURE_CUBE;
    updateTextureDescriptor(descriptor);

#if CC_ENABLE_CACHE_TEXTURE_DATA
    // Listen this event to restored texture id after coming to foreground on Android.
    _backToForegroundListener = EventListenerCustom::create(EVENT_COME_TO_FOREGROUND, [this](EventCustom*){
        _textureInfo.recreateAll(GL_TEXTURE_CUBE_MAP);
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, -1);
#endif
    CHECK_GL_ERROR_DEBUG();
}

void TextureCubeGL::updateTextureDescriptor(const cocos2d::backend::TextureDescriptor &descriptor, int index)
{
    backend::TextureCubemapBackend::updateTextureDescriptor(descriptor, index);
    
    UtilsGL::toGLTypes(descriptor.textureFormat, _textureInfo.internalFormat, _textureInfo.format, _textureInfo.type, _isCompressed);
    
    updateSamplerDescriptor(descriptor.samplerDescriptor);
}

TextureCubeGL::~TextureCubeGL()
{
#if CC_ENABLE_CACHE_TEXTURE_DATA
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}

void TextureCubeGL::updateSamplerDescriptor(const SamplerDescriptor &sampler)
{
    _textureInfo.applySampler(sampler, true, _hasMipmaps, GL_TEXTURE_CUBE_MAP);
}

void TextureCubeGL::updateFaceData(TextureCubeFace side, void *data, int index)
{   
    if (!_textureInfo.ensure(index, GL_TEXTURE_CUBE_MAP)) return;

    CHECK_GL_ERROR_DEBUG();
    int i = static_cast<int>(side);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
        0, // level
        _textureInfo.internalFormat,
        _width,
        _height,
        0, // border
        _textureInfo.format,
        _textureInfo.type,
        data);

    CHECK_GL_ERROR_DEBUG();
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void TextureCubeGL::generateMipmaps()
{
    if (TextureUsage::RENDER_TARGET == _textureUsage)
        return;

    if(!_hasMipmaps)
    {
        _hasMipmaps = true;
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->getHandler());
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
}

CC_BACKEND_END
