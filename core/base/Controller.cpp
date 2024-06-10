/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2014-2016 Chukong Technologies Inc.
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

#include "base/Controller.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS || \
     AX_TARGET_PLATFORM == AX_PLATFORM_MAC || AX_TARGET_PLATFORM == AX_PLATFORM_LINUX ||   \
     defined(_WIN32) || AX_TARGET_PLATFORM == AX_PLATFORM_WASM)

#    include "base/EventDispatcher.h"
#    include "base/EventController.h"
#    include "base/Director.h"

NS_AX_BEGIN

std::vector<Controller*> Controller::s_allController;

Controller* Controller::getControllerByTag(int tag)
{
    for (auto&& controller : Controller::s_allController)
    {
        if (controller->_controllerTag == tag)
        {
            return controller;
        }
    }
    return nullptr;
}

Controller* Controller::getControllerByDeviceId(int deviceId)
{
    for (auto&& controller : Controller::s_allController)
    {
        if (controller->_deviceId == deviceId)
        {
            return controller;
        }
    }
    return nullptr;
}

void Controller::init()
{
    for (int key = Key::JOYSTICK_LEFT_X; key < Key::KEY_MAX; ++key)
    {
        _allKeyStatus[key].isPressed = false;
        _allKeyStatus[key].value     = 0.0f;

        _allKeyPrevStatus[key].isPressed = false;
        _allKeyPrevStatus[key].value     = 0.0f;
    }

    _eventDispatcher = Director::getInstance()->getEventDispatcher();
    _connectEvent    = new EventController(EventController::ControllerEventType::CONNECTION, this, false);
    _keyEvent        = new EventController(EventController::ControllerEventType::BUTTON_STATUS_CHANGED, this, 0);
    _axisEvent       = new EventController(EventController::ControllerEventType::AXIS_STATUS_CHANGED, this, 0);
}

const Controller::KeyStatus& Controller::getKeyStatus(int keyCode)
{
    if (_allKeyStatus.find(keyCode) == _allKeyStatus.end())
    {
        _allKeyStatus[keyCode].isPressed = false;
        _allKeyStatus[keyCode].value     = 0.0f;
    }

    return _allKeyStatus[keyCode];
}

void Controller::onConnected()
{
    _connectEvent->setConnectStatus(true);
    _eventDispatcher->dispatchEvent(_connectEvent);
}

void Controller::onDisconnected()
{
    _connectEvent->setConnectStatus(false);
    _eventDispatcher->dispatchEvent(_connectEvent);

    delete this;
}

void Controller::onButtonEvent(int keyCode, bool isPressed, float value, bool isAnalog)
{
    _allKeyPrevStatus[keyCode]       = _allKeyStatus[keyCode];
    _allKeyStatus[keyCode].isPressed = isPressed;
    _allKeyStatus[keyCode].value     = value;
    _allKeyStatus[keyCode].isAnalog  = isAnalog;

    _keyEvent->setKeyCode(keyCode);
    _eventDispatcher->dispatchEvent(_keyEvent);
}

void Controller::onAxisEvent(int axisCode, float value, bool isAnalog)
{
    _allKeyPrevStatus[axisCode]      = _allKeyStatus[axisCode];
    _allKeyStatus[axisCode].value    = value;
    _allKeyStatus[axisCode].isAnalog = isAnalog;

    _axisEvent->setKeyCode(axisCode);
    _eventDispatcher->dispatchEvent(_axisEvent);
}

NS_AX_END

#endif  // (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS || AX_TARGET_PLATFORM ==
        // AX_PLATFORM_MAC || AX_TARGET_PLATFORM == AX_PLATFORM_LINUX || defined(_WIN32) || AX_TARGET_PLATFORM == AX_PLATFORM_WASM)
