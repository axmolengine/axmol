/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT

 ****************************************************************************/

#include "axmol/base/KeyboardEventListener.h"
#include "axmol/base/Macros.h"

namespace ax
{

const std::string_view KeyboardEventListener::LISTENER_ID = "__ax_keyboard"sv;

bool KeyboardEventListener::checkAvailable()
{
    if (onKeyPressed == nullptr && onKeyReleased == nullptr)
    {
        AXASSERT(false, "Invalid KeyboardEventListener!");
        return false;
    }

    return true;
}

KeyboardEventListener* KeyboardEventListener::create()
{
    auto ret = new KeyboardEventListener();
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

KeyboardEventListener* KeyboardEventListener::clone()
{
    auto ret = new KeyboardEventListener();
    if (ret->init())
    {
        ret->autorelease();
        ret->onKeyPressed  = onKeyPressed;
        ret->onKeyReleased = onKeyReleased;
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

KeyboardEventListener::KeyboardEventListener() : onKeyPressed(nullptr), onKeyReleased(nullptr) {}

bool KeyboardEventListener::init()
{
    auto listener = [this](Event* event) {
        auto keyboardEvent = static_cast<KeyboardEvent*>(event);
        switch (keyboardEvent->getPhase())
        {
        case InputPhase::KeyDown:
            if (onKeyPressed)
                onKeyPressed(keyboardEvent);
            break;
        case InputPhase::KeyUp:
            if (onKeyReleased)
                onKeyReleased(keyboardEvent);
            break;
        case InputPhase::KeyRepeat:
            if (onKeyRepeat)
                onKeyRepeat(keyboardEvent);
            break;
        default:;
        }
    };

    if (EventListener::init(Type::KEYBOARD, LISTENER_ID, listener))
    {
        return true;
    }

    return false;
}

}  // namespace ax
