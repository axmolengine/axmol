/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/base/AccelerationEvent.h"

namespace ax
{

AccelerationEvent::AccelerationEvent(const Acceleration& acc) : Event(Type::ACCELERATION), _acc(acc) {}

}  // namespace ax
