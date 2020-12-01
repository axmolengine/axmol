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

#include "CCPhysicsSpriteBox2D.h"
#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"


#include "box2d/box2d.h"


NS_CC_EXT_BEGIN

PhysicsSpriteBox2D::PhysicsSpriteBox2D()
: _ignoreBodyRotation(false)
, _CPBody(nullptr)
, _pB2Body(nullptr)
, _PTMRatio(0.0f)
, _syncTransform(nullptr)
{}

PhysicsSpriteBox2D* PhysicsSpriteBox2D::create()
{
    PhysicsSpriteBox2D* pRet = new (std::nothrow) PhysicsSpriteBox2D();
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

PhysicsSpriteBox2D* PhysicsSpriteBox2D::createWithTexture(Texture2D *pTexture)
{
    PhysicsSpriteBox2D* pRet = new (std::nothrow) PhysicsSpriteBox2D();
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

PhysicsSpriteBox2D* PhysicsSpriteBox2D::createWithTexture(Texture2D *pTexture, const Rect& rect)
{
    PhysicsSpriteBox2D* pRet = new (std::nothrow) PhysicsSpriteBox2D();
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

PhysicsSpriteBox2D* PhysicsSpriteBox2D::createWithSpriteFrame(SpriteFrame *pSpriteFrame)
{
    PhysicsSpriteBox2D* pRet = new (std::nothrow) PhysicsSpriteBox2D();
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

PhysicsSpriteBox2D* PhysicsSpriteBox2D::createWithSpriteFrameName(const char *pszSpriteFrameName)
{
    PhysicsSpriteBox2D* pRet = new (std::nothrow) PhysicsSpriteBox2D();
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

PhysicsSpriteBox2D* PhysicsSpriteBox2D::create(const char *pszFileName)
{
    PhysicsSpriteBox2D* pRet = new (std::nothrow) PhysicsSpriteBox2D();
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

PhysicsSpriteBox2D* PhysicsSpriteBox2D::create(const char *pszFileName, const Rect& rect)
{
    PhysicsSpriteBox2D* pRet = new (std::nothrow) PhysicsSpriteBox2D();
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
bool PhysicsSpriteBox2D::isDirty() const
{
    return true;
}

bool PhysicsSpriteBox2D::isIgnoreBodyRotation() const
{
    return _ignoreBodyRotation;
}

void PhysicsSpriteBox2D::setIgnoreBodyRotation(bool bIgnoreBodyRotation)
{
    _ignoreBodyRotation = bIgnoreBodyRotation;
}

// Override the setters and getters to always reflect the body's properties.
const Vec2& PhysicsSpriteBox2D::getPosition() const
{
    return getPosFromPhysics();
}

void PhysicsSpriteBox2D::getPosition(float* x, float* y) const
{
    if (x == nullptr || y == nullptr) {
        return;
    }
    const Vec2& pos = getPosFromPhysics();
    *x = pos.x;
    *y = pos.y;
}

float PhysicsSpriteBox2D::getPositionX() const
{
    return getPosFromPhysics().x;
}

float PhysicsSpriteBox2D::getPositionY() const
{
    return getPosFromPhysics().y;
}

Vec3 PhysicsSpriteBox2D::getPosition3D() const
{
    Vec2 pos = getPosFromPhysics();
    return Vec3(pos.x, pos.y, 0);
}

//
// Chipmunk only
//



cpBody* PhysicsSpriteBox2D::getCPBody() const
{

    CCASSERT(false, "Can't call chipmunk methods when Chipmunk is disabled");
    return nullptr;
}

void PhysicsSpriteBox2D::setCPBody(cpBody *pBody)
{
    _CPBody = pBody;
}

b2Body* PhysicsSpriteBox2D::getB2Body() const
{
    return _pB2Body;
}

void PhysicsSpriteBox2D::setB2Body(b2Body *pBody)
{
    _pB2Body = pBody;
}

float PhysicsSpriteBox2D::getPTMRatio() const
{
    return _PTMRatio;
}

void PhysicsSpriteBox2D::setPTMRatio(float fRatio)
{
     _PTMRatio = fRatio;
}

//
// Common to Box2d and Chipmunk
//

const Vec2& PhysicsSpriteBox2D::getPosFromPhysics() const
{
    static Vec2 s_physicPosion;

    b2Vec2 pos = _pB2Body->GetPosition();
    float x = pos.x * _PTMRatio;
    float y = pos.y * _PTMRatio;
    s_physicPosion.set(x,y);

    return s_physicPosion;
}

void PhysicsSpriteBox2D::setPosition(float x, float y)
{

    
    float angle = _pB2Body->GetAngle();
    _pB2Body->SetTransform(b2Vec2(x / _PTMRatio, y / _PTMRatio), angle);

}

void PhysicsSpriteBox2D::setPosition(const Vec2 &pos)
{
    setPosition(pos.x, pos.y);
}

void PhysicsSpriteBox2D::setPositionX(float x)
{
    setPosition(x, getPositionY());
}

void PhysicsSpriteBox2D::setPositionY(float y)
{
    setPosition(getPositionX(), y);
}

void PhysicsSpriteBox2D::setPosition3D(const Vec3& position)
{
    setPosition(position.x, position.y);
}

float PhysicsSpriteBox2D::getRotation() const
{
    return (_ignoreBodyRotation ? Sprite::getRotation() :
            CC_RADIANS_TO_DEGREES(_pB2Body->GetAngle()));
}

void PhysicsSpriteBox2D::setRotation(float fRotation)
{
    if (_ignoreBodyRotation)
    {
        Sprite::setRotation(fRotation);
    }

    else
    {
        b2Vec2 p = _pB2Body->GetPosition();
        float radians = CC_DEGREES_TO_RADIANS(fRotation);
        _pB2Body->SetTransform(p, radians);
    }
}

void PhysicsSpriteBox2D::syncPhysicsTransform() const
{
    // Although scale is not used by physics engines, it is calculated just in case
    // the sprite is animated (scaled up/down) using actions.
    // For more info see: http://www.cocos2d-iphone.org/forum/topic/68990
    
    
    b2Vec2 pos  = _pB2Body->GetPosition();
    
    float x = pos.x * _PTMRatio;
    float y = pos.y * _PTMRatio;
    
    if (_ignoreAnchorPointForPosition)
    {
        x += _anchorPointInPoints.x;
        y += _anchorPointInPoints.y;
    }
    
    // Make matrix
    float radians = _pB2Body->GetAngle();
    float c = cosf(radians);
    float s = sinf(radians);
    
    if (!_anchorPointInPoints.isZero())
    {
        x += ((c * -_anchorPointInPoints.x * _scaleX) + (-s * -_anchorPointInPoints.y * _scaleY));
        y += ((s * -_anchorPointInPoints.x * _scaleX) + (c * -_anchorPointInPoints.y * _scaleY));
    }
    
    // Rot, Translate Matrix
    
    float mat[] = {  (float)c * _scaleX, (float)s * _scaleX, 0,  0,
        (float)-s * _scaleY, (float)c * _scaleY,  0,  0,
        0,  0,  1,  0,
        x,    y,  0,  1};
    
    _transform.set(mat);

}

void PhysicsSpriteBox2D::onEnter()
{
    Node::onEnter();
    _syncTransform = Director::getInstance()->getEventDispatcher()->addCustomEventListener(Director::EVENT_AFTER_UPDATE, std::bind(&PhysicsSpriteBox2D::afterUpdate, this, std::placeholders::_1));
    _syncTransform->retain();
}

void PhysicsSpriteBox2D::onExit()
{
    if (_syncTransform != nullptr)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(_syncTransform);
        _syncTransform->release();
    }
    Node::onExit();
}

void PhysicsSpriteBox2D::afterUpdate(EventCustom* /*event*/)
{
    syncPhysicsTransform();
    
    _transformDirty = false;
    _transformUpdated = true;
    setDirtyRecursively(true);
}

NS_CC_EXT_END