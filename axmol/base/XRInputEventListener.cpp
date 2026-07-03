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

#include "axmol/base/XRInputEventListener.h"

#include "axmol/base/Macros.h"

namespace ax
{

const std::string_view XRInputEventListener::LISTENER_ID = "__ax_xr_input"sv;

XRInputEventListener* XRInputEventListener::create()
{
    auto ret = new XRInputEventListener();
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

bool XRInputEventListener::init()
{
    auto listener = [this](Event* event) {
        auto xrEvent = static_cast<XRInputEvent*>(event);
        switch (xrEvent->getEventType())
        {
        case XRInputEvent::EventType::Button:
            if (onButton)
                onButton(xrEvent);
            break;
        case XRInputEvent::EventType::Axis:
            if (onAxis)
                onAxis(xrEvent);
            break;
        case XRInputEvent::EventType::Pose:
            if (onPose)
                onPose(xrEvent);
            break;
        }
    };

    return EventListener::init(EventListener::Type::XR_INPUT, LISTENER_ID, listener);
}

bool XRInputEventListener::checkAvailable()
{
    return onButton || onAxis || onPose;
}

XRInputEventListener* XRInputEventListener::clone()
{
    auto ret = new XRInputEventListener();
    if (ret->init())
    {
        ret->onButton = onButton;
        ret->onAxis   = onAxis;
        ret->onPose   = onPose;
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

}  // namespace ax
