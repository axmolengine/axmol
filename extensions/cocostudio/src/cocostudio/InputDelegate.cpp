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

#include "InputDelegate.h"
#include "base/Director.h"
#include "platform/Device.h"
#include "base/EventListenerTouch.h"
#include "base/EventListenerAcceleration.h"
#include "base/EventListenerKeyboard.h"
#include "base/EventDispatcher.h"

USING_NS_AX;

namespace cocostudio
{

InputDelegate::InputDelegate(void)
    : _touchEnabled(false)
    , _touchListener(nullptr)
    , _accelerometerEnabled(false)
    , _accelerometerListener(nullptr)
    , _keypadEnabled(false)
    , _keyboardListener(nullptr)
    , _touchPriority(-1)
    , _touchMode(Touch::DispatchMode::ALL_AT_ONCE)
{}

InputDelegate::~InputDelegate(void)
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeEventListener(_touchListener);
    dispatcher->removeEventListener(_keyboardListener);
    dispatcher->removeEventListener(_accelerometerListener);
    Device::setAccelerometerEnabled(false);
}

void InputDelegate::didAccelerate(ax::Acceleration* /*accelerationValue*/) {}

bool InputDelegate::ccTouchBegan(ax::Touch* /*touch*/, ax::Event* /*event*/)
{
    return false;
}

void InputDelegate::ccTouchMoved(ax::Touch* /*touch*/, ax::Event* /*event*/) {}

void InputDelegate::ccTouchEnded(ax::Touch* /*touch*/, ax::Event* /*event*/) {}

void InputDelegate::ccTouchCancelled(ax::Touch* /*touch*/, ax::Event* /*event*/) {}

void InputDelegate::ccTouchesBegan(ax::__Set* /*touches*/, ax::Event* /*event*/) {}

void InputDelegate::ccTouchesMoved(ax::__Set* /*touches*/, ax::Event* /*event*/) {}

void InputDelegate::ccTouchesEnded(ax::__Set* /*touches*/, ax::Event* /*event*/) {}

void InputDelegate::ccTouchesCancelled(ax::__Set* /*touches*/, ax::Event* /*event*/) {}

void InputDelegate::onAcceleration(ax::Acceleration* /*acc*/, ax::Event* /*event*/) {}

void InputDelegate::onKeyPressed(ax::EventKeyboard::KeyCode /*keyCode*/, ax::Event* /*event*/) {}

void InputDelegate::onKeyReleased(ax::EventKeyboard::KeyCode /*keyCode*/, ax::Event* /*event*/) {}

bool InputDelegate::onTouchBegan(Touch* /*pTouch*/, Event* /*pEvent*/)
{
    return true;
}

void InputDelegate::onTouchMoved(Touch* /*pTouch*/, Event* /*pEvent*/) {}

void InputDelegate::onTouchEnded(Touch* /*pTouch*/, Event* /*pEvent*/) {}

void InputDelegate::onTouchCancelled(Touch* /*pTouch*/, Event* /*pEvent*/) {}

void InputDelegate::onTouchesBegan(const std::vector<Touch*>& /*pTouches*/, Event* /*pEvent*/) {}

void InputDelegate::onTouchesMoved(const std::vector<Touch*>& /*pTouches*/, Event* /*pEvent*/) {}

void InputDelegate::onTouchesEnded(const std::vector<Touch*>& /*pTouches*/, Event* /*pEvent*/) {}

void InputDelegate::onTouchesCancelled(const std::vector<Touch*>& /*pTouches*/, Event* /*pEvent*/) {}

bool InputDelegate::isTouchEnabled() const
{
    return _touchEnabled;
}

void InputDelegate::setTouchEnabled(bool enabled)
{
    if (_touchEnabled != enabled)
    {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        _touchEnabled   = enabled;
        if (enabled)
        {
            if (_touchMode == Touch::DispatchMode::ALL_AT_ONCE)
            {
                // Register Touch Event
                auto listener = EventListenerTouchAllAtOnce::create();

                listener->onTouchesBegan     = AX_CALLBACK_2(InputDelegate::onTouchesBegan, this);
                listener->onTouchesMoved     = AX_CALLBACK_2(InputDelegate::onTouchesMoved, this);
                listener->onTouchesEnded     = AX_CALLBACK_2(InputDelegate::onTouchesEnded, this);
                listener->onTouchesCancelled = AX_CALLBACK_2(InputDelegate::onTouchesCancelled, this);

                dispatcher->addEventListenerWithFixedPriority(listener, _touchPriority);
                _touchListener = listener;
            }
            else
            {
                // Register Touch Event
                auto listener = EventListenerTouchOneByOne::create();
                listener->setSwallowTouches(true);

                listener->onTouchBegan     = AX_CALLBACK_2(InputDelegate::onTouchBegan, this);
                listener->onTouchMoved     = AX_CALLBACK_2(InputDelegate::onTouchMoved, this);
                listener->onTouchEnded     = AX_CALLBACK_2(InputDelegate::onTouchEnded, this);
                listener->onTouchCancelled = AX_CALLBACK_2(InputDelegate::onTouchCancelled, this);

                dispatcher->addEventListenerWithFixedPriority(listener, _touchPriority);
                _touchListener = listener;
            }
        }
        else
        {
            dispatcher->removeEventListener(_touchListener);
        }
    }
}

void InputDelegate::setTouchMode(Touch::DispatchMode mode)
{
    if (_touchMode != mode)
    {
        _touchMode = mode;

        if (_touchEnabled)
        {
            setTouchEnabled(false);
            setTouchEnabled(true);
        }
    }
}

void InputDelegate::setTouchPriority(int priority)
{
    if (_touchPriority != priority)
    {
        _touchPriority = priority;

        if (_touchEnabled)
        {
            setTouchEnabled(false);
            setTouchEnabled(true);
        }
    }
}

int InputDelegate::getTouchPriority() const
{
    return _touchPriority;
}

Touch::DispatchMode InputDelegate::getTouchMode() const
{
    return _touchMode;
}

bool InputDelegate::isAccelerometerEnabled() const
{
    return _accelerometerEnabled;
}

void InputDelegate::setAccelerometerEnabled(bool enabled)
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
            auto listener = EventListenerAcceleration::create(AX_CALLBACK_2(InputDelegate::onAcceleration, this));
            dispatcher->addEventListenerWithFixedPriority(listener, -1);
            _accelerometerListener = listener;
        }
    }
}

bool InputDelegate::isKeypadEnabled() const
{
    return _keypadEnabled;
}

void InputDelegate::setKeypadEnabled(bool enabled)
{
    if (enabled != _keypadEnabled)
    {
        _keypadEnabled = enabled;

        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeEventListener(_keyboardListener);

        if (enabled)
        {
            auto listener           = EventListenerKeyboard::create();
            listener->onKeyPressed  = AX_CALLBACK_2(InputDelegate::onKeyPressed, this);
            listener->onKeyReleased = AX_CALLBACK_2(InputDelegate::onKeyReleased, this);

            dispatcher->addEventListenerWithFixedPriority(listener, -1);
            _keyboardListener = listener;
        }
    }
}

}  // namespace cocostudio
