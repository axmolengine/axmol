/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/base/CustomEvent.h"
#include "axmol/base/Event.h"

namespace ax
{

CustomEvent::CustomEvent(std::string_view eventName) : Event(Type::CUSTOM), _userData(nullptr), _eventName(eventName) {}

}  // namespace ax
