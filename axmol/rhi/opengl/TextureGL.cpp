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
#include "axmol/rhi/RHIUtils.h"

namespace ax::rhi::gl
{

#define ISPOW2(n) (((n) & (n - 1)) == 0)

/// CLASS TextureImpl
TextureImpl::TextureImpl(const TextureDesc& desc)
{
    updateTextureDesc(desc);

#if AX_ENABLE_CACHE_TEXTURE_DATA
    // Listen this event to restored texture id after coming to foreground on GLES.
    _rendererRecreatedListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom*) {
        if (_usedForRT)
            this->ensureTexStorageForRT();
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_rendererRecreatedListener, -1);
#endif
}

void TextureImpl::updateTextureDesc(const TextureDesc& desc)
{
    assert(desc.textureType == rhi::TextureType::TEXTURE_2D || _desc.width == _desc.height);

    Texture::updateTextureDesc(desc);

    UtilsGL::toGLTypes(desc.pixelFormat, _nativeDesc.internalFormat, _nativeDesc.format, _nativeDesc.type);
    switch (desc.textureType)
    {
    case TextureType::TEXTURE_2D:
        _nativeDesc.target = _desc.arraySize == 1 ? GL_TEXTURE_2D : GL_TEXTURE_2D_ARRAY;
        break;
    case TextureType::TEXTURE_CUBE:
        _nativeDesc.target = GL_TEXTURE_CUBE_MAP;
        break;
    }

    updateSamplerDesc(desc.samplerDesc);

    if (_desc.arraySize == 1)
    {
        const bool useForRT = desc.textureUsage == TextureUsage::RENDER_TARGET;
#if AX_ENABLE_CACHE_TEXTURE_DATA
        _usedForRT = useForRT;
#endif
        if (useForRT)
        {
            ensureTexStorageForRT();
        }
    }
}

void TextureImpl::ensureTexStorageForRT()
{
    const auto w = getWidth();
    const auto h = getHeight();
    auto size    = w * h * _bitsPerPixel / 8;
    assert(size > 0);

    axstd::byte_buffer blackPixels;
    blackPixels.resize(size, 0);

    updateData(blackPixels.data(), w, h, 0, 0);
}

TextureImpl::~TextureImpl()
{
#if AX_ENABLE_CACHE_TEXTURE_DATA
    Director::getInstance()->getEventDispatcher()->removeEventListener(_rendererRecreatedListener);
#endif
    if (_nativeTexture)
    {
        __state->deleteTexture(_nativeTexture);
        _nativeTexture = 0;
    }

    _nativeSampler = 0;
}

void TextureImpl::updateSamplerDesc(const SamplerDesc& desc)
{
    // TODO: check mip POT
    // bool isPow2 = ISPOW2(_width) && ISPOW2(_height);
    // _textureInfo.applySampler(sampler, isPow2, _hasMipmaps);

    this->_nativeSampler =
        static_cast<GLuint>(reinterpret_cast<uintptr_t>(SamplerCache::getInstance()->getSampler(desc)));
}

void TextureImpl::ensureNativeTexture(size_t imageSize)
{
    bool initial = !_nativeTexture;

    if (initial)
        glGenTextures(1, &_nativeTexture);

    __state->bindTexture(_nativeDesc.target, _nativeTexture);

    if (initial && _desc.arraySize > 1)
    { // allocate texture2d array storage
        if (_nativeDesc.type != 0)
        {
            glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, _nativeDesc.internalFormat, _desc.width, _desc.height, _desc.arraySize,
                         0, _nativeDesc.format, _nativeDesc.type, nullptr);
        }
        else
        {
            glCompressedTexImage3D(GL_TEXTURE_2D_ARRAY, 0, _nativeDesc.internalFormat, _desc.width, _desc.height,
                                   _desc.arraySize, 0, imageSize * _desc.arraySize, nullptr);
        }
        CHECK_GL_ERROR_DEBUG();
    }
}

void TextureImpl::updateData(const void* data, std::size_t width, std::size_t height, std::size_t level, int layerIndex)
{
    assert(_desc.textureType == TextureType::TEXTURE_2D);
    ensureNativeTexture();

    // Set the row align only when mipmapsNum == 1 and the data is uncompressed
    auto mipmapEnabled = _desc.samplerDesc.mipFilter != SamplerFilter::MIP_NONE;
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

    if (_desc.arraySize == 1)
        glTexImage2D(GL_TEXTURE_2D, level, _nativeDesc.internalFormat, width, height, 0, _nativeDesc.format,
                     _nativeDesc.type, data);
    else
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, level, 0, 0, layerIndex, width, height, 1, _nativeDesc.internalFormat,
                     _nativeDesc.type, data);

    CHECK_GL_ERROR_DEBUG();

#if AX_ENABLE_CACHE_TEXTURE_DATA
    if (_generateMipmaps && level == 0)
    {
        // _hasMipmaps = false;
        generateMipmaps();
    }
#endif

    if (!_desc.mipLevels && level > 0)
        _desc.mipLevels = level + 1;
}

void TextureImpl::updateCompressedData(const void* data,
                                       std::size_t width,
                                       std::size_t height,
                                       std::size_t dataSize,
                                       std::size_t level,
                                       int layerIndex)
{
    assert(_desc.textureType == TextureType::TEXTURE_2D);
    ensureNativeTexture(dataSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (_desc.arraySize == 1)
        glCompressedTexImage2D(GL_TEXTURE_2D, level, _nativeDesc.internalFormat, (GLsizei)width, (GLsizei)height, 0,
                               dataSize, data);
    else
        glCompressedTexSubImage3D(GL_TEXTURE_2D_ARRAY, level, 0, 0, layerIndex, (GLsizei)width, (GLsizei)height, 1,
                                  _nativeDesc.internalFormat, dataSize, data);
    CHECK_GL_ERROR_DEBUG();

    if (!_desc.mipLevels && level > 0)
        _desc.mipLevels = level + 1;
}

void TextureImpl::updateSubData(std::size_t xoffset,
                                std::size_t yoffset,
                                std::size_t width,
                                std::size_t height,
                                std::size_t level,
                                const void* data,
                                int layerIndex)
{
    assert(_desc.textureType == TextureType::TEXTURE_2D);
    ensureNativeTexture();

    // !IMPORTANT: Set the unpack alignment to 1 byte to avoid issues with width or height no align with 4.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (_desc.arraySize == 1)
        glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, _nativeDesc.format, _nativeDesc.type, data);
    else
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, level, xoffset, yoffset, layerIndex, width, height, 1,
                        _nativeDesc.internalFormat,
                        _nativeDesc.type, data);

    CHECK_GL_ERROR_DEBUG();

    // TODO:
    // if (!_hasMipmaps && level > 0)
    //     _hasMipmaps = true;
}

void TextureImpl::updateCompressedSubData(std::size_t xoffset,
                                          std::size_t yoffset,
                                          std::size_t width,
                                          std::size_t height,
                                          std::size_t dataSize,
                                          std::size_t level,
                                          const void* data,
                                          int layerIndex)
{
    assert(_desc.textureType == TextureType::TEXTURE_2D);
    ensureNativeTexture(dataSize);

    if (_desc.arraySize == 1)
        glCompressedTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, _nativeDesc.internalFormat,
                              dataSize, data);
    else
        glCompressedTexSubImage3D(GL_TEXTURE_2D_ARRAY, level, xoffset, yoffset, layerIndex, width, height, 1,
                                  _nativeDesc.internalFormat, dataSize, data);
    CHECK_GL_ERROR_DEBUG();

    // TODO:
    // if (!_hasMipmaps && level > 0)
    //    _hasMipmaps = true;
}

void TextureImpl::updateFaceData(TextureCubeFace side, const void* data)
{
    assert(_desc.textureType == TextureType::TEXTURE_CUBE);
    ensureNativeTexture();

    CHECK_GL_ERROR_DEBUG();
    int i = static_cast<int>(side);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, _nativeDesc.internalFormat, _desc.width, _desc.height, 0,
                _nativeDesc.format, _nativeDesc.type, data);

    CHECK_GL_ERROR_DEBUG();
    __state->bindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void TextureImpl::apply(int slot) const
{
    __state->activeTexture(GL_TEXTURE0 + slot);
    __state->bindTexture(_nativeDesc.target, _nativeTexture);
    __state->bindSampler(slot, _nativeSampler);
}

void TextureImpl::generateMipmaps()
{
    if (TextureUsage::RENDER_TARGET == _desc.textureUsage)
        return;

    if (_desc.mipLevels == 0)  // 0 means chould auto generate by GPU
    {
        __state->bindTexture(_nativeDesc.target, (GLuint)this->internalHandle());
        glGenerateMipmap(_nativeDesc.target);

#if AX_ENABLE_CACHE_TEXTURE_DATA
        _generateMipmaps = true;
#endif

        _desc.mipLevels = RHIUtils::computeMipLevels(_desc);
    }
}

}  // namespace ax::rhi::gl
