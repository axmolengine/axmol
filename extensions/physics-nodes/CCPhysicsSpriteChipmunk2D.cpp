/* Copyright (c) 2012 Scott Lembcke and Howling Moon Software
 * Copyright (c) 2012 cocos2d-x.org
 * Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include "CCPhysicsSpriteChipmunk2D.h"
#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"


#include "chipmunk/chipmunk.h"


NS_CC_EXT_BEGIN

PhysicsSpriteChipmunk2D::PhysicsSpriteChipmunk2D()
: _ignoreBodyRotation(false)
, _CPBody(nullptr)
, _pB2Body(nullptr)
, _PTMRatio(0.0f)
, _syncTransform(nullptr)
{}

PhysicsSpriteChipmunk2D* PhysicsSpriteChipmunk2D::create()
{
    PhysicsSpriteChipmunk2D* pRet = new (std::nothrow) PhysicsSpriteChipmunk2D();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    
    return pRet;
}

PhysicsSpriteChipmunk2D* PhysicsSpriteChipmunk2D::createWithTexture(Texture2D *pTexture)
{
    PhysicsSpriteChipmunk2D* pRet = new (std::nothrow) PhysicsSpriteChipmunk2D();
    if (pRet && pRet->initWithTexture(pTexture))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

PhysicsSpriteChipmunk2D* PhysicsSpriteChipmunk2D::createWithTexture(Texture2D *pTexture, const Rect& rect)
{
    PhysicsSpriteChipmunk2D* pRet = new (std::nothrow) PhysicsSpriteChipmunk2D();
    if (pRet && pRet->initWithTexture(pTexture, rect))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

PhysicsSpriteChipmunk2D* PhysicsSpriteChipmunk2D::createWithSpriteFrame(SpriteFrame *pSpriteFrame)
{
    PhysicsSpriteChipmunk2D* pRet = new (std::nothrow) PhysicsSpriteChipmunk2D();
    if (pRet && pRet->initWithSpriteFrame(pSpriteFrame))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

PhysicsSpriteChipmunk2D* PhysicsSpriteChipmunk2D::createWithSpriteFrameName(const char *pszSpriteFrameName)
{
    PhysicsSpriteChipmunk2D* pRet = new (std::nothrow) PhysicsSpriteChipmunk2D();
    if (pRet && pRet->initWithSpriteFrameName(pszSpriteFrameName))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

PhysicsSpriteChipmunk2D* PhysicsSpriteChipmunk2D::create(const char *pszFileName)
{
    PhysicsSpriteChipmunk2D* pRet = new (std::nothrow) PhysicsSpriteChipmunk2D();
    if (pRet && pRet->initWithFile(pszFileName))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

PhysicsSpriteChipmunk2D* PhysicsSpriteChipmunk2D::create(const char *pszFileName, const Rect& rect)
{
    PhysicsSpriteChipmunk2D* pRet = new (std::nothrow) PhysicsSpriteChipmunk2D();
    if (pRet && pRet->initWithFile(pszFileName, rect))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

// this method will only get called if the sprite is batched.
// return YES if the physic's values (angles, position ) changed.
// If you return NO, then getNodeToParentTransform won't be called.
bool PhysicsSpriteChipmunk2D::isDirty() const
{
    return true;
}

bool PhysicsSpriteChipmunk2D::isIgnoreBodyRotation() const
{
    return _ignoreBodyRotation;
}

void PhysicsSpriteChipmunk2D::setIgnoreBodyRotation(bool bIgnoreBodyRotation)
{
    _ignoreBodyRotation = bIgnoreBodyRotation;
}

// Override the setters and getters to always reflect the body's properties.
const Vec2& PhysicsSpriteChipmunk2D::getPosition() const
{
    return getPosFromPhysics();
}

void PhysicsSpriteChipmunk2D::getPosition(float* x, float* y) const
{
    if (x == nullptr || y == nullptr) {
        return;
    }
    const Vec2& pos = getPosFromPhysics();
    *x = pos.x;
    *y = pos.y;
}

float PhysicsSpriteChipmunk2D::getPositionX() const
{
    return getPosFromPhysics().x;
}

float PhysicsSpriteChipmunk2D::getPositionY() const
{
    return getPosFromPhysics().y;
}

Vec3 PhysicsSpriteChipmunk2D::getPosition3D() const
{
    Vec2 pos = getPosFromPhysics();
    return Vec3(pos.x, pos.y, 0);
}

//
// Chipmunk only
//



cpBody* PhysicsSpriteChipmunk2D::getCPBody() const
{
    return _CPBody;
}

void PhysicsSpriteChipmunk2D::setCPBody(cpBody *pBody)
{
    _CPBody = pBody;
}

b2Body* PhysicsSpriteChipmunk2D::getB2Body() const
{
    CCASSERT(false, "Can't call box2d methods when Box2d is disabled");
    return nullptr;
}

void PhysicsSpriteChipmunk2D::setB2Body(b2Body *pBody)
{
    CC_UNUSED_PARAM(pBody);
    CCASSERT(false, "Can't call box2d methods when Box2d is disabled");
}

float PhysicsSpriteChipmunk2D::getPTMRatio() const
{
    CCASSERT(false, "Can't call box2d methods when Box2d is disabled");
    return 0;
}

void PhysicsSpriteChipmunk2D::setPTMRatio(float fRatio)
{
    CC_UNUSED_PARAM(fRatio);
    CCASSERT(false, "Can't call box2d methods when Box2d is disabled");
}

//
// Common to Box2d and Chipmunk
//

const Vec2& PhysicsSpriteChipmunk2D::getPosFromPhysics() const
{
    static Vec2 s_physicPosion;

    cpVect cpPos = cpBodyGetPosition(_CPBody);
    s_physicPosion = Vec2(cpPos.x, cpPos.y);

    return s_physicPosion;
}

void PhysicsSpriteChipmunk2D::setPosition(float x, float y)
{
    cpVect cpPos = cpv(x, y);
    cpBodySetPosition(_CPBody, cpPos);
}

void PhysicsSpriteChipmunk2D::setPosition(const Vec2 &pos)
{
    setPosition(pos.x, pos.y);
}

void PhysicsSpriteChipmunk2D::setPositionX(float x)
{
    setPosition(x, getPositionY());
}

void PhysicsSpriteChipmunk2D::setPositionY(float y)
{
    setPosition(getPositionX(), y);
}

void PhysicsSpriteChipmunk2D::setPosition3D(const Vec3& position)
{
    setPosition(position.x, position.y);
}

float PhysicsSpriteChipmunk2D::getRotation() const
{
    return (_ignoreBodyRotation ? Sprite::getRotation() : -CC_RADIANS_TO_DEGREES(cpBodyGetAngle(_CPBody)));
}

void PhysicsSpriteChipmunk2D::setRotation(float fRotation)
{
    if (_ignoreBodyRotation)
    {
        Sprite::setRotation(fRotation);
    }
    else
    {
        cpBodySetAngle(_CPBody, -CC_DEGREES_TO_RADIANS(fRotation));
    }

}

void PhysicsSpriteChipmunk2D::syncPhysicsTransform() const
{
    // Although scale is not used by physics engines, it is calculated just in case
    // the sprite is animated (scaled up/down) using actions.
    // For more info see: http://www.cocos2d-iphone.org/forum/topic/68990
    
    
    cpVect rot = (_ignoreBodyRotation ? cpvforangle(-CC_DEGREES_TO_RADIANS(_rotationX)) : cpBodyGetRotation(_CPBody));
    float x = cpBodyGetPosition(_CPBody).x + rot.x * -_anchorPointInPoints.x * _scaleX - rot.y * -_anchorPointInPoints.y * _scaleY;
    float y = cpBodyGetPosition(_CPBody).y + rot.y * -_anchorPointInPoints.x * _scaleX + rot.x * -_anchorPointInPoints.y * _scaleY;
    
    if (_ignoreAnchorPointForPosition)
    {
        x += _anchorPointInPoints.x;
        y += _anchorPointInPoints.y;
    }
    
    float mat[] = {  (float)rot.x * _scaleX, (float)rot.y * _scaleX, 0,  0,
        (float)-rot.y * _scaleY, (float)rot.x * _scaleY,  0,  0,
        0,  0,  1,  0,
        x,    y,  0,  1};
    
    _transform.set(mat);
}

void PhysicsSpriteChipmunk2D::onEnter()
{
    Node::onEnter();
    _syncTransform = Director::getInstance()->getEventDispatcher()->addCustomEventListener(Director::EVENT_AFTER_UPDATE, std::bind(&PhysicsSpriteChipmunk2D::afterUpdate, this, std::placeholders::_1));
    _syncTransform->retain();
}

void PhysicsSpriteChipmunk2D::onExit()
{
    if (_syncTransform != nullptr)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(_syncTransform);
        _syncTransform->release();
    }
    Node::onExit();
}

void PhysicsSpriteChipmunk2D::afterUpdate(EventCustom* /*event*/)
{
    syncPhysicsTransform();
    
    _transformDirty = false;
    _transformUpdated = true;
    setDirtyRecursively(true);
}

NS_CC_EXT_END