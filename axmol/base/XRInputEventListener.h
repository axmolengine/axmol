/****************************************************************************
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

#include "axmol/base/EventListener.h"
#include "axmol/base/XRInputEvent.h"

/**
 * @addtogroup base
 * @{
 */

namespace ax
{

/** @class XRInputEventListener
 * @brief Listener for XRInputEvent button, axis and pose callbacks.
 */
class AX_DLL XRInputEventListener : public EventListener
{
public:
    static const std::string_view LISTENER_ID;

    static XRInputEventListener* create();

    bool checkAvailable() override;
    XRInputEventListener* clone() override;

    std::function<void(XRInputEvent*)> onButton;
    std::function<void(XRInputEvent*)> onAxis;
    std::function<void(XRInputEvent*)> onPose;

protected:
    bool init();
};

using EventListenerXRInput = XRInputEventListener;

}  // namespace ax

// end of base group
/// @}
