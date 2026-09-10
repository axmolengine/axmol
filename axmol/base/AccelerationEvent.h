/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/base/Event.h"
#include "axmol/base/Types.h"

/**
 * @addtogroup base
 * @{
 */

namespace ax
{

/** @class AccelerationEvent
 * @brief Accelerometer event.
 */
class AX_DLL AccelerationEvent : public Event
{
public:
    /** Constructor.
     *
     * @param acc A given Acceleration.
     */
    AccelerationEvent(const Acceleration& acc);

    const Acceleration& getAcceleration() const { return _acc; }

private:
    Acceleration _acc;
    friend class AccelerationEventListener;
};

// deprecated alias
using EventAcceleration = AccelerationEvent;

}  // namespace ax

// end of base group
/// @}
