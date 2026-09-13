/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
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
