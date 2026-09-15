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
#include "axmol/base/Event.h"

/**
 * @addtogroup base
 * @{
 */

namespace ax
{

/// @cond ControllerEvent
class Controller;
class ControllerEventListener;

/** @class ControllerEvent
 * @brief Controller event.
 */
class AX_DLL ControllerEvent : public Event
{
public:
    /** ControllerEventType Controller event type.*/
    enum class ControllerEventType
    {
        CONNECTION,
        BUTTON_STATUS_CHANGED,
        AXIS_STATUS_CHANGED,
    };

    /** Create a ControllerEvent with controller event type, controller and key code.
     *
     * @param type A given controller event type.
     * @param controller A given controller pointer.
     * @param keyCode A given key code.
     * @return An autoreleased ControllerEvent object.
     */
    ControllerEvent(ControllerEventType type, Controller* controller, int keyCode);
    /** Create a ControllerEvent with controller event type, controller and whether or not is connected.
     *
     * @param type A given controller event type.
     * @param controller A given controller pointer.
     * @param isConnected True if it is connected.
     * @return An autoreleased ControllerEvent object.
     */
    ControllerEvent(ControllerEventType type, Controller* controller, bool isConnected);

    /** Gets the event type of the controller.
     *
     * @return The event type of the controller.
     */
    ControllerEventType getControllerEventType() const { return _controllerEventType; }
    Controller* getController() const { return _controller; }

    /** Gets the key code of the controller.
     *
     * @return The key code of the controller.
     */
    int getKeyCode() const { return _keyCode; }
    void setKeyCode(int keyCode) { _keyCode = keyCode; }

    /** Sets the connect status.
     *
     * @param True if it's connected.
     */
    void setConnectStatus(bool isConnected) { _isConnected = isConnected; }
    /** Gets the connect status.
     *
     * @return True if it's connected.
     */
    bool isConnected() const { return _isConnected; }

protected:
    ControllerEventType _controllerEventType;
    Controller* _controller;
    int _keyCode;
    bool _isConnected;

    friend class ControllerEventListener;
};

// deprecated alias
using EventController = ControllerEvent;

/// @endcond ControllerEvent
}  // namespace ax

// end of base group
/// @}
