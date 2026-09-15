/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/platform/PlatformMacros.h"
#include "axmol/base/EventListener.h"
#include "axmol/base/ControllerEvent.h"

/**
 * @addtogroup base
 * @{
 */

namespace ax
{

class Event;
class Controller;

/** @class ControllerEventListener
 * @param Controller event listener.
 */
class AX_DLL ControllerEventListener : public EventListener
{
public:
    static const std::string_view LISTENER_ID;

    /** Create a controller event listener.
     *
     * @return An autoreleased ControllerEventListener object.
     */
    static ControllerEventListener* create();

    /// Overrides
    bool checkAvailable() override;
    ControllerEventListener* clone() override;

    std::function<void(ControllerEvent*)> onConnected;
    std::function<void(ControllerEvent*)> onDisconnected;

    std::function<void(ControllerEvent*)> onKeyDown;
    std::function<void(ControllerEvent*)> onKeyUp;
    std::function<void(ControllerEvent*)> onKeyRepeat;

    std::function<void(ControllerEvent*)> onAxisEvent;

protected:
    bool init();
};

// deprecated alias
using EventListenerController = ControllerEventListener;

}  // namespace ax

// end of base group
/// @}
