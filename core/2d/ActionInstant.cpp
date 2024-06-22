/****************************************************************************
 Copyright (c) 2008-2010 Ricardo Quesada
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2011      Zynga Inc.
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include "2d/ActionInstant.h"
#include "2d/Node.h"
#include "2d/Sprite.h"

NS_AX_BEGIN
//
// InstantAction
//
void ActionInstant::startWithTarget(Node* target)
{
    FiniteTimeAction::startWithTarget(target);
    _done = false;
}

bool ActionInstant::isDone() const
{
    return _done;
}

void ActionInstant::step(float /*dt*/)
{
    float updateDt = 1;
    update(updateDt);
}

void ActionInstant::update(float /*time*/)
{
    _done = true;
}

//
// Show
//

Show* Show::create()
{
    Show* ret = new Show();
    ret->autorelease();
    return ret;
}

void Show::update(float time)
{
    ActionInstant::update(time);
    _target->setVisible(true);
}

ActionInstant* Show::reverse() const
{
    return Hide::create();
}

Show* Show::clone() const
{
    // no copy constructor
    return Show::create();
}

//
// Hide
//
Hide* Hide::create()
{
    Hide* ret = new Hide();

    ret->autorelease();

    return ret;
}

void Hide::update(float time)
{
    ActionInstant::update(time);
    _target->setVisible(false);
}

ActionInstant* Hide::reverse() const
{
    return Show::create();
}

Hide* Hide::clone() const
{
    // no copy constructor
    return Hide::create();
}

//
// ToggleVisibility
//
ToggleVisibility* ToggleVisibility::create()
{
    ToggleVisibility* ret = new ToggleVisibility();
    ret->autorelease();
    return ret;
}

void ToggleVisibility::update(float time)
{
    ActionInstant::update(time);
    _target->setVisible(!_target->isVisible());
}

ToggleVisibility* ToggleVisibility::reverse() const
{
    return ToggleVisibility::create();
}

ToggleVisibility* ToggleVisibility::clone() const
{
    // no copy constructor
    return ToggleVisibility::create();
}

//
// Remove Self
//
RemoveSelf* RemoveSelf::create(bool isNeedCleanUp /*= true*/)
{
    RemoveSelf* ret = new RemoveSelf();

    if (ret->init(isNeedCleanUp))
        ret->autorelease();
    else
        AX_SAFE_DELETE(ret);

    return ret;
}

bool RemoveSelf::init(bool isNeedCleanUp)
{
    _isNeedCleanUp = isNeedCleanUp;
    return true;
}

void RemoveSelf::update(float time)
{
    ActionInstant::update(time);
    _target->removeFromParentAndCleanup(_isNeedCleanUp);
}

RemoveSelf* RemoveSelf::reverse() const
{
    return RemoveSelf::create(_isNeedCleanUp);
}

RemoveSelf* RemoveSelf::clone() const
{
    // no copy constructor
    return RemoveSelf::create(_isNeedCleanUp);
}

//
// FlipX
//

FlipX* FlipX::create(bool x)
{
    FlipX* ret = new FlipX();

    if (ret->initWithFlipX(x))
    {
        ret->autorelease();
        return ret;
    }

    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool FlipX::initWithFlipX(bool x)
{
    _flipX = x;
    return true;
}

void FlipX::update(float time)
{
    ActionInstant::update(time);
    static_cast<Sprite*>(_target)->setFlippedX(_flipX);
}

FlipX* FlipX::reverse() const
{
    return FlipX::create(!_flipX);
}

FlipX* FlipX::clone() const
{
    // no copy constructor
    return FlipX::create(_flipX);
}
//
// FlipY
//

FlipY* FlipY::create(bool y)
{
    FlipY* ret = new FlipY();

    if (ret->initWithFlipY(y))
    {
        ret->autorelease();
        return ret;
    }

    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool FlipY::initWithFlipY(bool y)
{
    _flipY = y;
    return true;
}

void FlipY::update(float time)
{
    ActionInstant::update(time);
    static_cast<Sprite*>(_target)->setFlippedY(_flipY);
}

FlipY* FlipY::reverse() const
{
    return FlipY::create(!_flipY);
}

FlipY* FlipY::clone() const
{
    // no copy constructor
    return FlipY::create(_flipY);
}

//
// Place
//

Place* Place::create(const Vec2& pos)
{
    Place* ret = new Place();

    if (ret->initWithPosition(pos))
    {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool Place::initWithPosition(const Vec2& pos)
{
    _position = pos;
    return true;
}

Place* Place::clone() const
{
    // no copy constructor
    return Place::create(_position);
}

Place* Place::reverse() const
{
    // no reverse, just clone
    return this->clone();
}

void Place::update(float time)
{
    ActionInstant::update(time);
    _target->setPosition(_position);
}

//
// CallFunc
//

CallFunc* CallFunc::create(const std::function<void()>& func)
{
    CallFunc* ret = new CallFunc();

    if (ret->initWithFunction(func))
    {
        ret->autorelease();
        return ret;
    }

    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool CallFunc::initWithFunction(const std::function<void()>& func)
{
    _function = func;
    return true;
}

CallFunc* CallFunc::clone() const
{
    // no copy constructor
    auto a = new CallFunc();
    if (_function)
        a->initWithFunction(_function);

    a->autorelease();
    return a;
}

CallFunc* CallFunc::reverse() const
{
    // no reverse here, just return a clone
    return this->clone();
}

void CallFunc::update(float time)
{
    ActionInstant::update(time);
    this->execute();
}

void CallFunc::execute()
{
    if (_function)
    {
        _function();
    }
}

//
// CallFuncN
//

CallFuncN* CallFuncN::create(const std::function<void(Node*)>& func)
{
    auto ret = new CallFuncN();

    if (ret->initWithFunction(func))
    {
        ret->autorelease();
        return ret;
    }

    AX_SAFE_DELETE(ret);
    return nullptr;
}

void CallFuncN::execute()
{
    if (_functionN)
    {
        _functionN(_target);
    }
}

bool CallFuncN::initWithFunction(const std::function<void(Node*)>& func)
{
    _functionN = func;
    return true;
}

CallFuncN* CallFuncN::clone() const
{
    // no copy constructor
    auto a = new CallFuncN();
    if (_functionN)
        a->initWithFunction(_functionN);

    a->autorelease();
    return a;
}

NS_AX_END
