/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/base/XRInputEvent.h"

namespace ax
{

XRInputEvent::XRInputEvent(const State& state) : Event(Type::XR_INPUT), _state(state) {}

}  // namespace ax
