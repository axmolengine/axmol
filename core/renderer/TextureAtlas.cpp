/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include "renderer/TextureAtlas.h"

#include <stdlib.h>

#include "base/Macros.h"
#include "base/UTF8.h"
#include "base/EventType.h"
#include "base/Director.h"
#include "base/Configuration.h"
#include "base/EventDispatcher.h"
#include "base/EventListenerCustom.h"
#include "renderer/TextureCache.h"
#include "renderer/Renderer.h"
#include "renderer/Texture2D.h"

// According to some tests GL_TRIANGLE_STRIP is slower, MUCH slower. Probably I'm doing something very wrong

// implementation TextureAtlas

namespace ax
{

TextureAtlas::TextureAtlas() {}

TextureAtlas::~TextureAtlas()
{
    AXLOGV("deallocing TextureAtlas: {}", fmt::ptr(this));

    AX_SAFE_FREE(_quads);
    AX_SAFE_FREE(_indices);

    AX_SAFE_RELEASE(_texture);

#if AX_ENABLE_CACHE_TEXTURE_DATA
    Director::getInstance()->getEventDispatcher()->removeEventListener(_rendererRecreatedListener);
#endif
}

size_t TextureAtlas::getTotalQuads() const
{
    return _totalQuads;
}

size_t TextureAtlas::getCapacity() const
{
    return _capacity;
}

Texture2D* TextureAtlas::getTexture() const
{
    return _texture;
}

void TextureAtlas::setTexture(Texture2D* var)
{
    AX_SAFE_RETAIN(var);
    AX_SAFE_RELEASE(_texture);
    _texture = var;
}

V3F_C4B_T2F_Quad* TextureAtlas::getQuads()
{
    // if someone accesses the quads directly, presume that changes will be made
    _dirty = true;
    return _quads;
}

void TextureAtlas::setQuads(V3F_C4B_T2F_Quad* quads)
{
    _quads = quads;
}

// TextureAtlas - alloc & init

TextureAtlas* TextureAtlas::create(std::string_view file, ssize_t capacity)
{
    TextureAtlas* textureAtlas = new TextureAtlas();
    if (textureAtlas->initWithFile(file, capacity))
    {
        textureAtlas->autorelease();
        return textureAtlas;
    }
    AX_SAFE_DELETE(textureAtlas);
    return nullptr;
}

TextureAtlas* TextureAtlas::createWithTexture(Texture2D* texture, ssize_t capacity)
{
    TextureAtlas* textureAtlas = new TextureAtlas();
    if (textureAtlas->initWithTexture(texture, capacity))
    {
        textureAtlas->autorelease();
        return textureAtlas;
    }
    AX_SAFE_DELETE(textureAtlas);
    return nullptr;
}

bool TextureAtlas::initWithFile(std::string_view file, ssize_t capacity)
{
    // retained in property
    Texture2D* texture = Director::getInstance()->getTextureCache()->addImage(file);

    if (texture)
    {
        return initWithTexture(texture, capacity);
    }
    else
    {
        AXLOGD("Could not open file: {}", file);
        return false;
    }
}

bool TextureAtlas::initWithTexture(Texture2D* texture, ssize_t capacity)
{
    AXASSERT(capacity >= 0, "Capacity must be >= 0");

    //    AXASSERT(texture != nullptr, "texture should not be null");
    _capacity   = capacity;
    _totalQuads = 0;

    // retained in property
    this->_texture = texture;
    AX_SAFE_RETAIN(_texture);

    // Re-initialization is not allowed
    AXASSERT(_quads == nullptr && _indices == nullptr, "_quads and _indices should be nullptr.");

    _quads   = (V3F_C4B_T2F_Quad*)malloc(_capacity * sizeof(V3F_C4B_T2F_Quad));
    _indices = (uint16_t*)malloc(_capacity * 6 * sizeof(uint16_t));

    if (!(_quads && _indices) && _capacity > 0)
    {
        // AXLOGD("TextureAtlas: not enough memory");
        AX_SAFE_FREE(_quads);
        AX_SAFE_FREE(_indices);

        // release texture, should set it to null, because the destruction will
        // release it too. see cocos2d-x issue #484
        AX_SAFE_RELEASE_NULL(_texture);
        return false;
    }

    memset(_quads, 0, _capacity * sizeof(V3F_C4B_T2F_Quad));
    memset(_indices, 0, _capacity * 6 * sizeof(uint16_t));

    this->setupIndices();

    _dirty = true;

    return true;
}

std::string TextureAtlas::getDescription() const
{
    return fmt::format("<TextureAtlas | totalQuads = {}>", static_cast<int>(_totalQuads));
}

void TextureAtlas::setupIndices()
{
    if (_capacity == 0)
        return;

    for (int i = 0; i < _capacity; i++)
    {
        _indices[i * 6 + 0] = i * 4 + 0;
        _indices[i * 6 + 1] = i * 4 + 1;
        _indices[i * 6 + 2] = i * 4 + 2;

        // inverted index. issue #179
        _indices[i * 6 + 3] = i * 4 + 3;
        _indices[i * 6 + 4] = i * 4 + 2;
        _indices[i * 6 + 5] = i * 4 + 1;
    }
}

// TextureAtlas - Update, Insert, Move & Remove

void TextureAtlas::updateQuad(const V3F_C4B_T2F_Quad& quad, ssize_t index)
{
    AXASSERT(index >= 0 && index < _capacity, "updateQuadWithTexture: Invalid index");

    _totalQuads = MAX(index + 1, _totalQuads);

    _quads[index] = quad;

    _dirty = true;
}

void TextureAtlas::insertQuad(const V3F_C4B_T2F_Quad& quad, ssize_t index)
{
    AXASSERT(index >= 0 && index < _capacity, "insertQuadWithTexture: Invalid index");

    _totalQuads++;
    AXASSERT(_totalQuads <= _capacity, "invalid totalQuads");

    // issue #575. index can be > totalQuads
    auto remaining = (_totalQuads - 1) - index;

    // last object doesn't need to be moved
    if (remaining > 0)
    {
        // texture coordinates
        memmove(&_quads[index + 1], &_quads[index], sizeof(_quads[0]) * remaining);
    }

    _quads[index] = quad;

    _dirty = true;
}

void TextureAtlas::insertQuads(V3F_C4B_T2F_Quad* quads, ssize_t index, ssize_t amount)
{
    AXASSERT(index >= 0 && amount >= 0 && index + amount <= _capacity, "insertQuadWithTexture: Invalid index + amount");

    _totalQuads += amount;

    AXASSERT(_totalQuads <= _capacity, "invalid totalQuads");

    // issue #575. index can be > totalQuads
    auto remaining = (_totalQuads - 1) - index - amount;

    // last object doesn't need to be moved
    if (remaining > 0)
    {
        // tex coordinates
        memmove(&_quads[index + amount], &_quads[index], sizeof(_quads[0]) * remaining);
    }

    auto max = index + amount;
    int j    = 0;
    for (ssize_t i = index; i < max; i++)
    {
        _quads[index] = quads[j];
        index++;
        j++;
    }

    _dirty = true;
}

void TextureAtlas::insertQuadFromIndex(ssize_t oldIndex, ssize_t newIndex)
{
    AXASSERT(newIndex >= 0 && newIndex < _totalQuads, "insertQuadFromIndex:atIndex: Invalid index");
    AXASSERT(oldIndex >= 0 && oldIndex < _totalQuads, "insertQuadFromIndex:atIndex: Invalid index");

    if (oldIndex == newIndex)
    {
        return;
    }
    // because it is ambiguous in iphone, so we implement abs ourselves
    // unsigned int howMany = std::abs( oldIndex - newIndex);
    auto howMany = (oldIndex - newIndex) > 0 ? (oldIndex - newIndex) : (newIndex - oldIndex);
    auto dst     = oldIndex;
    auto src     = oldIndex + 1;
    if (oldIndex > newIndex)
    {
        dst = newIndex + 1;
        src = newIndex;
    }

    // texture coordinates
    V3F_C4B_T2F_Quad quadsBackup = _quads[oldIndex];
    memmove(&_quads[dst], &_quads[src], sizeof(_quads[0]) * howMany);
    _quads[newIndex] = quadsBackup;

    _dirty = true;
}

void TextureAtlas::removeQuadAtIndex(ssize_t index)
{
    AXASSERT(index >= 0 && index < _totalQuads, "removeQuadAtIndex: Invalid index");

    auto remaining = (_totalQuads - 1) - index;

    // last object doesn't need to be moved
    if (remaining)
    {
        // texture coordinates
        memmove(&_quads[index], &_quads[index + 1], sizeof(_quads[0]) * remaining);
    }

    _totalQuads--;

    _dirty = true;
}

void TextureAtlas::removeQuadsAtIndex(ssize_t index, ssize_t amount)
{
    AXASSERT(index >= 0 && amount >= 0 && index + amount <= _totalQuads,
             "removeQuadAtIndex: index + amount out of bounds");

    auto remaining = (_totalQuads) - (index + amount);

    _totalQuads -= amount;

    if (remaining)
    {
        memmove(&_quads[index], &_quads[index + amount], sizeof(_quads[0]) * remaining);
    }

    _dirty = true;
}

void TextureAtlas::removeAllQuads()
{
    _totalQuads = 0;
}

// TextureAtlas - Resize
bool TextureAtlas::resizeCapacity(ssize_t newCapacity)
{
    AXASSERT(newCapacity >= 0, "capacity >= 0");
    if (newCapacity == _capacity)
    {
        return true;
    }
    auto oldCapacity = _capacity;

    // update capacity and totalQuads
    _totalQuads = MIN(_totalQuads, newCapacity);
    _capacity   = newCapacity;

    V3F_C4B_T2F_Quad* tmpQuads = nullptr;
    uint16_t* tmpIndices       = nullptr;

    // when calling initWithTexture(fileName, 0) on bada device, calloc(0, 1) will fail and return nullptr,
    // so here must judge whether _quads and _indices is nullptr.

    ssize_t _quads_size    = sizeof(_quads[0]);
    ssize_t new_quads_size = _capacity * _quads_size;
    if (_quads == nullptr)
    {
        tmpQuads = (V3F_C4B_T2F_Quad*)malloc(new_quads_size);
        if (tmpQuads != nullptr)
        {
            memset(tmpQuads, 0, new_quads_size);
        }
    }
    else
    {
        tmpQuads = (V3F_C4B_T2F_Quad*)realloc(_quads, new_quads_size);
        if (tmpQuads != nullptr && _capacity > oldCapacity)
        {
            memset(tmpQuads + oldCapacity, 0, (_capacity - oldCapacity) * _quads_size);
        }
        _quads = nullptr;
    }

    ssize_t _indices_size = sizeof(_indices[0]);
    ssize_t new_size      = _capacity * 6 * _indices_size;

    if (_indices == nullptr)
    {
        tmpIndices = (uint16_t*)malloc(new_size);
        if (tmpIndices != nullptr)
        {
            memset(tmpIndices, 0, new_size);
        }
    }
    else
    {
        tmpIndices = (uint16_t*)realloc(_indices, new_size);
        if (tmpIndices != nullptr && _capacity > oldCapacity)
        {
            memset(tmpIndices + oldCapacity, 0, (_capacity - oldCapacity) * 6 * _indices_size);
        }
        _indices = nullptr;
    }

    if (!(tmpQuads && tmpIndices))
    {
        AXLOGD("TextureAtlas: not enough memory");
        AX_SAFE_FREE(tmpQuads);
        AX_SAFE_FREE(tmpIndices);
        AX_SAFE_FREE(_quads);
        AX_SAFE_FREE(_indices);
        _capacity = _totalQuads = 0;
        return false;
    }

    _quads   = tmpQuads;
    _indices = tmpIndices;

    setupIndices();

    _dirty = true;

    return true;
}

void TextureAtlas::increaseTotalQuadsWith(ssize_t amount)
{
    AXASSERT(amount >= 0, "amount >= 0");
    _totalQuads += amount;
}

void TextureAtlas::moveQuadsFromIndex(ssize_t oldIndex, ssize_t amount, ssize_t newIndex)
{
    AXASSERT(oldIndex >= 0 && amount >= 0 && newIndex >= 0, "values must be >= 0");
    AXASSERT(newIndex + amount <= _totalQuads, "insertQuadFromIndex:atIndex: Invalid index");
    AXASSERT(oldIndex < _totalQuads, "insertQuadFromIndex:atIndex: Invalid index");

    if (oldIndex == newIndex)
    {
        return;
    }
    // create buffer
    size_t quadSize             = sizeof(V3F_C4B_T2F_Quad);
    V3F_C4B_T2F_Quad* tempQuads = (V3F_C4B_T2F_Quad*)malloc(quadSize * amount);
    memcpy(tempQuads, &_quads[oldIndex], quadSize * amount);

    if (newIndex < oldIndex)
    {
        // move quads from newIndex to newIndex + amount to make room for buffer
        memmove(&_quads[newIndex], &_quads[newIndex + amount], (oldIndex - newIndex) * quadSize);
    }
    else
    {
        // move quads above back
        memmove(&_quads[oldIndex], &_quads[oldIndex + amount], (newIndex - oldIndex) * quadSize);
    }
    memcpy(&_quads[newIndex], tempQuads, amount * quadSize);

    free(tempQuads);

    _dirty = true;
}

void TextureAtlas::moveQuadsFromIndex(ssize_t index, ssize_t newIndex)
{
    AXASSERT(index >= 0 && newIndex >= 0, "values must be >= 0");
    AXASSERT(newIndex + (_totalQuads - index) <= _capacity, "moveQuadsFromIndex move is out of bounds");

    memmove(_quads + newIndex, _quads + index, (_totalQuads - index) * sizeof(_quads[0]));
}

void TextureAtlas::fillWithEmptyQuadsFromIndex(ssize_t index, ssize_t amount)
{
    AXASSERT(index >= 0 && amount >= 0, "values must be >= 0");
    V3F_C4B_T2F_Quad quad;
    memset(&quad, 0, sizeof(quad));

    auto to = index + amount;
    for (ssize_t i = index; i < to; i++)
    {
        _quads[i] = quad;
    }
}

}
