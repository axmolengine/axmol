/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#include "axmol/2d/ActionCoroutine.h"

namespace ax
{

//
// Coroutine
//
Coroutine::Coroutine(Coroutine&& rhs) noexcept : _handle(rhs._handle)
{
    rhs._handle = nullptr;
}

Coroutine::~Coroutine()
{
    if (_handle)
    {
        _handle.destroy();
    }
}

Coroutine& Coroutine::operator=(Coroutine&& rhs) noexcept
{
    if (this != std::addressof(rhs))
    {
        _handle     = rhs._handle;
        rhs._handle = nullptr;
    }
    return *this;
}

Action* Coroutine::currentAction() const noexcept
{
    return _handle.promise().currentAction();
}

bool Coroutine::isDone() const
{
    return _handle && _handle.done();
}

bool Coroutine::moveNext() const
{
    if (_handle)
    {
        _handle.promise().resume();
        return !_handle.done();
    }
    return false;
}

//
// ActionCoroutine
//
ActionCoroutine* ActionCoroutine::create(const std::function<Coroutine()>& function)
{
    auto ret = new ActionCoroutine();
    if (ret && ret->initWithCoroutine(function))
    {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ActionCoroutine::initWithCoroutine(const std::function<Coroutine()>& function) noexcept
{
    _coroutine = std::forward<Coroutine>(function());
    return true;
}

bool ActionCoroutine::isDone() const
{
    auto action = _coroutine.currentAction();
    if (action && !action->isDone())
        return false;

    return _coroutine.isDone();
}

void ActionCoroutine::step(float dt)
{
    auto action = _coroutine.currentAction();
    if (action && !action->isDone())
    {
        action->step(dt);
        return;
    }
    _coroutine.moveNext();
}

}  // namespace ax
