/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
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
