/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/base/XRInputEvent.h"

namespace ax
{

XRInputEvent::XRInputEvent(const State& state) : Event(Type::XR_INPUT), _state(state) {}

}  // namespace ax
