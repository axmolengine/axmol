/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include <string>
#include "axmol/base/Event.h"

/**
 * @addtogroup base
 * @{
 */

namespace ax
{

/** @class CustomEvent
 * @brief Custom event.
 */
class AX_DLL CustomEvent : public Event
{
public:
    /** Constructor.
     *
     * @param eventName A given name of the custom event.
     */
    CustomEvent(std::string_view eventName);

    /** Sets user data.
     *
     * @param data The user data pointer, it's a void*.
     */
    void setUserData(void* data) { _userData = data; }

    /** Gets user data.
     *
     * @return The user data pointer, it's a void*.
     */
    void* getUserData() const { return _userData; }

    /** Gets event name.
     *
     * @return The name of the event.
     */
    std::string_view getEventName() const { return _eventName; }

protected:
    void* _userData;  ///< User data
    std::string _eventName;
};

// deprecated alias
using EventCustom = CustomEvent;

}  // namespace ax

// end of base group
/// @}
