/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/base/EventListener.h"
#include "axmol/base/AccelerationEvent.h"

/**
 * @addtogroup base
 * @{
 */

namespace ax
{
/** @class AccelerationEventListener
 * @brief Acceleration event listener.
 */
class AX_DLL AccelerationEventListener : public EventListener
{
public:
    static const std::string_view LISTENER_ID;

    /** Create a acceleration EventListener.
     *
     * @param callback The acceleration callback method.
     * @return An autoreleased AccelerationEventListener object.
     */
    static AccelerationEventListener* create(const std::function<void(AccelerationEvent*)>& callback);

    /** Destructor.
     */
    virtual ~AccelerationEventListener();

    /// Overrides
    AccelerationEventListener* clone() override;
    bool checkAvailable() override;

    AccelerationEventListener();

    bool init(const std::function<void(AccelerationEvent* event)>& callback);

private:
    std::function<void(AccelerationEvent*)> onAcceleration;

    friend class LuaAccelerationEventListener;
};

// deprecated alias
using EventListenerAcceleration = AccelerationEventListener;

}  // namespace ax

// end of base group
/// @}
