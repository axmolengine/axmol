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
#include "2d/AnchoredSprite.h"
#include "renderer/backend/DriverBase.h"

namespace ax
{

// FIXME: HACK: optimization
#define SET_DIRTY_RECURSIVELY()            \
    {                                      \
        if (!_recursiveDirty)              \
        {                                  \
            _recursiveDirty = true;        \
            setDirty(true);                \
            if (!_children.empty())        \
                setDirtyRecursively(true); \
        }                                  \
    }

// MARK: create, init, dealloc
AnchoredSprite* AnchoredSprite::createWithTexture(Texture2D* texture)
{
    AnchoredSprite* asprite = new AnchoredSprite();
    if (asprite->initWithTexture(texture))
    {
        asprite->autorelease();
        return asprite;
    }
    AX_SAFE_DELETE(asprite);
    return nullptr;
}

AnchoredSprite* AnchoredSprite::createWithTexture(Texture2D* texture, const Rect& rect, bool rotated)
{
    AnchoredSprite* asprite = new AnchoredSprite();
    if (asprite->initWithTexture(texture, rect, rotated))
    {
        asprite->autorelease();
        return asprite;
    }
    AX_SAFE_DELETE(asprite);
    return nullptr;
}

AnchoredSprite* AnchoredSprite::create(std::string_view filename)
{
    return AnchoredSprite::create(filename, Texture2D::getDefaultAlphaPixelFormat());
}

AnchoredSprite* AnchoredSprite::create(std::string_view filename, PixelFormat format)
{
    AnchoredSprite* asprite = new AnchoredSprite();
    if (asprite->initWithFile(filename, format))
    {
        asprite->autorelease();
        return asprite;
    }
    AX_SAFE_DELETE(asprite);
    return nullptr;
}

AnchoredSprite* AnchoredSprite::create(const PolygonInfo& info)
{
    AnchoredSprite* asprite = new AnchoredSprite();
    if (asprite->initWithPolygon(info))
    {
        asprite->autorelease();
        return asprite;
    }
    AX_SAFE_DELETE(asprite);
    return nullptr;
}

AnchoredSprite* AnchoredSprite::create(std::string_view filename, const Rect& rect)
{
    AnchoredSprite* asprite = new AnchoredSprite();
    if (asprite->initWithFile(filename, rect))
    {
        asprite->autorelease();
        return asprite;
    }
    AX_SAFE_DELETE(asprite);
    return nullptr;
}

AnchoredSprite* AnchoredSprite::createWithSpriteFrame(SpriteFrame* spriteFrame)
{
    AnchoredSprite* asprite = new AnchoredSprite();
    if (spriteFrame && asprite->initWithSpriteFrame(spriteFrame))
    {
        asprite->autorelease();
        return asprite;
    }
    AX_SAFE_DELETE(asprite);
    return nullptr;
}

AnchoredSprite* AnchoredSprite::createWithSpriteFrameName(std::string_view spriteFrameName)
{
    SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);

#if _AX_DEBUG > 0
    char msg[256] = {0};
    snprintf(msg, sizeof(msg), "Invalid spriteFrameName: %s", spriteFrameName.data());
    AXASSERT(frame != nullptr, msg);
#endif

    return createWithSpriteFrame(frame);
}

AnchoredSprite* AnchoredSprite::create()
{
    AnchoredSprite* asprite = new AnchoredSprite();
    if (asprite->init())
    {
        asprite->autorelease();
        return asprite;
    }
    AX_SAFE_DELETE(asprite);
    return nullptr;
}

void AnchoredSprite::setVertexCoords(const Rect& rect, V3F_C4B_T2F_Quad* outQuad)
{
    float relativeOffsetX = _unflippedOffsetPositionFromCenter.x - getContentSize().x * _spriteVertexAnchor.x;
    float relativeOffsetY = _unflippedOffsetPositionFromCenter.y - getContentSize().y * _spriteVertexAnchor.y;

    // issue #732
    if (_flippedX)
        relativeOffsetX = -relativeOffsetX;
    if (_flippedY)
        relativeOffsetY = -relativeOffsetY;

    _offsetPosition.x = relativeOffsetX + (_originalContentSize.width - _rect.size.width) / 2;
    _offsetPosition.y = relativeOffsetY + (_originalContentSize.height - _rect.size.height) / 2;

    // FIXME: Stretching should be applied to the "offset" as well
    // but probably it should be calculated in the caller function. It will be tidier
    if (_renderMode == RenderMode::QUAD)
    {
        _offsetPosition.x *= _stretchFactor.x;
        _offsetPosition.y *= _stretchFactor.y;
    }

    // rendering using batch node
    if (_renderMode == RenderMode::QUAD_BATCHNODE)
    {
        // update dirty_, don't update recursiveDirty_
        setDirty(true);
    }
    else
    {
        // self rendering

        // Atlas: Vertex
        const float x1 = 0.0f + _offsetPosition.x + rect.origin.x;
        const float y1 = 0.0f + _offsetPosition.y + rect.origin.y;
        const float x2 = x1 + rect.size.width;
        const float y2 = y1 + rect.size.height;

        // Don't update Z.
        outQuad->bl.vertices.set(x1, y1, 0.0f);
        outQuad->br.vertices.set(x2, y1, 0.0f);
        outQuad->tl.vertices.set(x1, y2, 0.0f);
        outQuad->tr.vertices.set(x2, y2, 0.0f);
    }
}

void AnchoredSprite::setAnchorPoint(const Vec2& anchor)
{
    _spriteVertexAnchor = anchor;
    SET_DIRTY_RECURSIVELY();
    updatePoly();
}

Rect AnchoredSprite::getTouchRect()
{
    Size s = getContentSize();
    Size a = _spriteVertexAnchor * s;
    return Rect(-a.x, -a.y, s.width, s.height);
}

}
