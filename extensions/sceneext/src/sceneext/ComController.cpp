/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.

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

#include "sceneext/ComController.h"
#include "axmol/scene/Node.h"
#include "axmol/base/Director.h"
#include "axmol/platform/Device.h"
#include "axmol/base/PointerEventListener.h"
#include "axmol/base/AccelerationEventListener.h"
#include "axmol/base/KeyboardEventListener.h"
#include "axmol/base/EventDispatcher.h"

namespace ax::ext
{

IMPLEMENT_CLASS_COMPONENT_INFO(ComController)

const std::string ComController::COMPONENT_NAME = "CCComController";

ComController::ComController()
{
    _name = COMPONENT_NAME;
}

ComController::~ComController()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeEventListener(_pointerListener);
    dispatcher->removeEventListener(_keyboardListener);
    dispatcher->removeEventListener(_accelerometerListener);
    Device::setAccelerometerEnabled(false);
}

bool ComController::init()
{
    return true;
}

void ComController::onEnter()
{
    if (_owner != nullptr)
    {
        _owner->scheduleUpdate();
    }
}

void ComController::onExit() {}

void ComController::onAdd()
{
    if (_owner != nullptr)
    {
        _owner->scheduleUpdate();
    }
}

void ComController::onRemove() {}

void ComController::update(float /*delta*/) {}

ComController* ComController::create()
{
    ComController* pRet = new ComController();
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

//////////////////////////////////////////////////////////////////////////
// Events
//////////////////////////////////////////////////////////////////////////

void ComController::onAcceleration(ax::AccelerationEvent* /*event*/) {}

void ComController::onKeyPressed(ax::KeyboardEvent* /*event*/) {}

void ComController::onKeyReleased(ax::KeyboardEvent* /*event*/) {}

bool ComController::onPointerDown(PointerEvent*)
{
    return true;
}

void ComController::onPointerMove(PointerEvent* /*event*/) {}

void ComController::onPointerUp(PointerEvent* /*event*/) {}

void ComController::onPointerCancel(PointerEvent* /*event*/) {}

bool ComController::isPointerEnabled() const
{
    return _pointerEnabled;
}

void ComController::setPointerEnabled(bool enabled)
{
    if (_pointerEnabled != enabled)
    {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        _pointerEnabled = enabled;
        if (enabled)
        {
            // Register Touch Event
            auto listener = PointerEventListener::create();

            listener->onPointerDown   = AX_CALLBACK_1(ComController::onPointerDown, this);
            listener->onPointerMove   = AX_CALLBACK_1(ComController::onPointerMove, this);
            listener->onPointerUp     = AX_CALLBACK_1(ComController::onPointerUp, this);
            listener->onPointerCancel = AX_CALLBACK_1(ComController::onPointerCancel, this);

            dispatcher->addEventListenerWithFixedPriority(listener, _pointerPriority);
            _pointerListener = listener;
        }
        else
        {
            dispatcher->removeEventListener(_pointerListener);
        }
    }
}

void ComController::setPointerPriority(int priority)
{
    if (_pointerPriority != priority)
    {
        _pointerPriority = priority;

        if (_pointerEnabled)
        {
            setPointerEnabled(false);
            setPointerEnabled(true);
        }
    }
}

int ComController::getPointerPriority() const
{
    return _pointerPriority;
}

bool ComController::isAccelerometerEnabled() const
{
    return _accelerometerEnabled;
}

void ComController::setAccelerometerEnabled(bool enabled)
{
    if (enabled != _accelerometerEnabled)
    {
        _accelerometerEnabled = enabled;

        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeEventListener(_accelerometerListener);
        _accelerometerListener = nullptr;

        Device::setAccelerometerEnabled(enabled);

        if (enabled)
        {
            auto listener = AccelerationEventListener::create(AX_CALLBACK_1(ComController::onAcceleration, this));
            dispatcher->addEventListenerWithFixedPriority(listener, -1);
            _accelerometerListener = listener;
        }
    }
}

bool ComController::isKeypadEnabled() const
{
    return _keypadEnabled;
}

void ComController::setKeypadEnabled(bool enabled)
{
    if (enabled != _keypadEnabled)
    {
        _keypadEnabled = enabled;

        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeEventListener(_keyboardListener);

        if (enabled)
        {
            auto listener           = KeyboardEventListener::create();
            listener->onKeyPressed  = AX_CALLBACK_1(ComController::onKeyPressed, this);
            listener->onKeyReleased = AX_CALLBACK_1(ComController::onKeyReleased, this);

            dispatcher->addEventListenerWithFixedPriority(listener, -1);
            _keyboardListener = listener;
        }
    }
}

}  // namespace ax::ext
