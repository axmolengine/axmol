/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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
