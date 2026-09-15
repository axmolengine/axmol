/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/base/ControllerEventListener.h"
#include "axmol/base/ControllerEvent.h"
#include "axmol/base/Macros.h"
#include "axmol/base/Controller.h"

namespace ax
{

const std::string_view ControllerEventListener::LISTENER_ID = "__ax_controller"sv;

ControllerEventListener* ControllerEventListener::create()
{
    auto ret = new ControllerEventListener();
    if (ret->init())
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

bool ControllerEventListener::init()
{
    auto listener = [this](Event* event) {
        auto evtController = static_cast<ControllerEvent*>(event);
        switch (evtController->getControllerEventType())
        {
        case ControllerEvent::ControllerEventType::CONNECTION:
            if (evtController->isConnected())
            {
                if (this->onConnected)
                    this->onConnected(evtController);
            }
            else
            {
                if (this->onDisconnected)
                    this->onDisconnected(evtController);
            }
            break;
        case ControllerEvent::ControllerEventType::BUTTON_STATUS_CHANGED:
        {
            const auto& keyStatus     = evtController->_controller->_allKeyStatus[evtController->_keyCode];
            const auto& keyPrevStatus = evtController->_controller->_allKeyPrevStatus[evtController->_keyCode];

            if (this->onKeyDown && keyStatus.isPressed && !keyPrevStatus.isPressed)
            {
                this->onKeyDown(evtController);
            }
            else if (this->onKeyUp && !keyStatus.isPressed && keyPrevStatus.isPressed)
            {
                this->onKeyUp(evtController);
            }
            else if (this->onKeyRepeat && keyStatus.isPressed && keyPrevStatus.isPressed)
            {
                this->onKeyRepeat(evtController);
            }
        }
        break;
        case ControllerEvent::ControllerEventType::AXIS_STATUS_CHANGED:
        {
            if (this->onAxisEvent)
            {
                this->onAxisEvent(evtController);
            }
        }
        break;
        default:
            AXASSERT(false, "Invalid ControllerEvent type");
            break;
        }
    };

    if (EventListener::init(EventListener::Type::GAME_CONTROLLER, LISTENER_ID, listener))
    {
        return true;
    }
    return false;
}

bool ControllerEventListener::checkAvailable()
{
    return true;
}

ControllerEventListener* ControllerEventListener::clone()
{
    return nullptr;
}

}  // namespace ax
