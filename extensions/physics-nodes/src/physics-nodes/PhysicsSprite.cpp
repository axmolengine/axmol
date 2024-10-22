/* Copyright (c) 2012 Scott Lembcke and Howling Moon Software
 * Copyright (c) 2012 cocos2d-x.org
 * Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 * Copyright (c) 2021 @aismann; Peter Eismann, Germany; dreifrankensoft
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "PhysicsSprite.h"
#include "base/Director.h"
#include "base/EventDispatcher.h"

NS_AX_EXT_BEGIN

PhysicsSprite::PhysicsSprite()
    : _ignoreBodyRotation(false), _PTMRatio(0.0f), _syncTransform(nullptr)
{}

PhysicsSprite* PhysicsSprite::create()
{
    PhysicsSprite* pRet = new PhysicsSprite();
    if (pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }

    return pRet;
}

PhysicsSprite* PhysicsSprite::createWithTexture(Texture2D* pTexture)
{
    PhysicsSprite* pRet = new PhysicsSprite();
    if (pRet->initWithTexture(pTexture))
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }

    return pRet;
}

PhysicsSprite* PhysicsSprite::createWithTexture(Texture2D* pTexture, const Rect& rect)
{
    PhysicsSprite* pRet = new PhysicsSprite();
    if (pRet->initWithTexture(pTexture, rect))
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }

    return pRet;
}

PhysicsSprite* PhysicsSprite::createWithSpriteFrame(SpriteFrame* pSpriteFrame)
{
    PhysicsSprite* pRet = new PhysicsSprite();
    if (pRet->initWithSpriteFrame(pSpriteFrame))
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }

    return pRet;
}

PhysicsSprite* PhysicsSprite::createWithSpriteFrameName(const char* pszSpriteFrameName)
{
    PhysicsSprite* pRet = new PhysicsSprite();
    if (pRet->initWithSpriteFrameName(pszSpriteFrameName))
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }

    return pRet;
}

PhysicsSprite* PhysicsSprite::create(const char* pszFileName)
{
    PhysicsSprite* pRet = new PhysicsSprite();
    if (pRet->initWithFile(pszFileName))
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }

    return pRet;
}

PhysicsSprite* PhysicsSprite::create(const char* pszFileName, const Rect& rect)
{
    PhysicsSprite* pRet = new PhysicsSprite();
    if (pRet->initWithFile(pszFileName, rect))
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }

    return pRet;
}

// this method will only get called if the sprite is batched.
// return YES if the physic's values (angles, position ) changed.
// If you return NO, then getNodeToParentTransform won't be called.
bool PhysicsSprite::isDirty() const
{
    return true;
}

bool PhysicsSprite::isIgnoreBodyRotation() const
{
    return _ignoreBodyRotation;
}

void PhysicsSprite::setIgnoreBodyRotation(bool bIgnoreBodyRotation)
{
    _ignoreBodyRotation = bIgnoreBodyRotation;
}

// Override the setters and getters to always reflect the body's properties.
const Vec2& PhysicsSprite::getPosition() const
{
    return getPosFromPhysics();
}

void PhysicsSprite::getPosition(float* x, float* y) const
{
    if (x == nullptr || y == nullptr)
    {
        return;
    }
    const Vec2& pos = getPosFromPhysics();
    *x              = pos.x;
    *y              = pos.y;
}

float PhysicsSprite::getPositionX() const
{
    return getPosFromPhysics().x;
}

float PhysicsSprite::getPositionY() const
{
    return getPosFromPhysics().y;
}

Vec3 PhysicsSprite::getPosition3D() const
{
    Vec2 pos = getPosFromPhysics();
    return Vec3(pos.x, pos.y, 0);
}

b2BodyId PhysicsSprite::getB2Body() const
{
    return _bodyId;
}

void PhysicsSprite::setB2Body(b2BodyId pBody)
{
    _bodyId = pBody;
}

float PhysicsSprite::getPTMRatio() const
{
    return _PTMRatio;
}

void PhysicsSprite::setPTMRatio(float fRatio)
{
    _PTMRatio = fRatio;
}

//
// Common to Box2d and Chipmunk
//

const Vec2& PhysicsSprite::getPosFromPhysics() const
{
    static Vec2 s_physicPosion;
    b2Vec2 pos = b2Body_GetPosition(_bodyId);
    float x    = pos.x * _PTMRatio;
    float y    = pos.y * _PTMRatio;
    s_physicPosion.set(x, y);

    return s_physicPosion;
}

void PhysicsSprite::setPosition(float x, float y)
{
    auto rot = b2Body_GetRotation(_bodyId);
    b2Body_SetTransform(_bodyId, b2Vec2{x / _PTMRatio, y / _PTMRatio}, rot);
}

void PhysicsSprite::setPosition(const Vec2& pos)
{
    setPosition(pos.x, pos.y);
}

void PhysicsSprite::setPositionX(float x)
{
    setPosition(x, getPositionY());
}

void PhysicsSprite::setPositionY(float y)
{
    setPosition(getPositionX(), y);
}

void PhysicsSprite::setPosition3D(const Vec3& position)
{
    setPosition(position.x, position.y);
}

float PhysicsSprite::getRotation() const
{
    auto angle = b2Rot_GetAngle(b2Body_GetRotation(_bodyId));
    return (_ignoreBodyRotation ? Sprite::getRotation() : AX_RADIANS_TO_DEGREES(angle));
}

void PhysicsSprite::setRotation(float fRotation)
{
    if (_ignoreBodyRotation)
    {
        Sprite::setRotation(fRotation);
    }

    else
    {
        b2Vec2 p      = b2Body_GetPosition(_bodyId);
        float radians = AX_DEGREES_TO_RADIANS(fRotation);
        auto rot = b2MakeRot(radians);
        b2Body_SetTransform(_bodyId, p, rot);
    }
}

void PhysicsSprite::syncPhysicsTransform() const
{
    // Although scale is not used by physics engines, it is calculated just in case
    // the sprite is animated (scaled up/down) using actions.
    // For more info see: http://www.cocos2d-iphone.org/forum/topic/68990

    b2Vec2 pos = b2Body_GetPosition(_bodyId);

    float x = pos.x * _PTMRatio;
    float y = pos.y * _PTMRatio;

    if (_ignoreAnchorPointForPosition)
    {
        x += _anchorPointInPoints.x;
        y += _anchorPointInPoints.y;
    }

    // Make matrix
    auto rot      = b2Body_GetRotation(_bodyId);
    float radians = b2Rot_GetAngle(rot);
    float c       = cosf(radians);
    float s       = sinf(radians);

    if (!_anchorPointInPoints.isZero())
    {
        x += ((c * -_anchorPointInPoints.x * _scaleX) + (-s * -_anchorPointInPoints.y * _scaleY));
        y += ((s * -_anchorPointInPoints.x * _scaleX) + (c * -_anchorPointInPoints.y * _scaleY));
    }

    // Rot, Translate Matrix

    float mat[] = {(float)c * _scaleX,
                   (float)s * _scaleX,
                   0,
                   0,
                   (float)-s * _scaleY,
                   (float)c * _scaleY,
                   0,
                   0,
                   0,
                   0,
                   1,
                   0,
                   x,
                   y,
                   0,
                   1};

    _transform.set(mat);
}

void PhysicsSprite::onEnter()
{
    Node::onEnter();
    _syncTransform = Director::getInstance()->getEventDispatcher()->addCustomEventListener(
        Director::EVENT_AFTER_UPDATE, std::bind(&PhysicsSprite::afterUpdate, this, std::placeholders::_1));
    _syncTransform->retain();
}

void PhysicsSprite::onExit()
{
    if (_syncTransform != nullptr)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(_syncTransform);
        _syncTransform->release();
    }
    Node::onExit();
}

void PhysicsSprite::afterUpdate(EventCustom* /*event*/)
{
    syncPhysicsTransform();

    _transformDirty   = false;
    _transformUpdated = true;
    setDirtyRecursively(true);
}

NS_AX_EXT_END
