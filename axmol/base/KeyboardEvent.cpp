/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT

 ****************************************************************************/

#include "axmol/base/KeyboardEvent.h"

namespace ax
{

KeyboardEvent::KeyboardEvent(KeyCode keyCode, InputPhase phase)
    : Event(Type::KEYBOARD), _keyCode(keyCode), _phase(phase), _modifiers(0)
{}

KeyboardEvent::KeyboardEvent(KeyCode keyCode, InputPhase phase, uint32_t modifiers)
    : Event(Type::KEYBOARD), _keyCode(keyCode), _phase(phase), _modifiers(modifiers)
{}

}  // namespace ax
