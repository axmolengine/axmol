/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/base/ControllerEvent.h"

namespace ax
{

ControllerEvent::ControllerEvent(ControllerEventType type, Controller* controller, int keyCode)
    : Event(Type::GAME_CONTROLLER)
    , _controllerEventType(type)
    , _controller(controller)
    , _keyCode(keyCode)
    , _isConnected(true)
{}

ControllerEvent::ControllerEvent(ControllerEventType type, Controller* controller, bool isConnected)
    : Event(Type::GAME_CONTROLLER)
    , _controllerEventType(type)
    , _controller(controller)
    , _keyCode(0)
    , _isConnected(isConnected)
{}

}  // namespace ax
