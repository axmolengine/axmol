/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/base/PointerEventListener.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/PointerEvent.h"

#include <algorithm>

namespace ax
{

const std::string_view PointerEventListener::LISTENER_ID = "__ax_pointer_listener"sv;

PointerEventListener::PointerEventListener() = default;

PointerEventListener::~PointerEventListener()
{
    AXLOGV("In the destructor of PointerEventListener, {}", fmt::ptr(this));
}

bool PointerEventListener::init()
{
    if (EventListener::init(Type::POINTER, LISTENER_ID, nullptr))
    {
        return true;
    }

    return false;
}

PointerEventListener* PointerEventListener::create()
{
    auto ret = new PointerEventListener();
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

bool PointerEventListener::checkAvailable()
{
    if (!onPointerDown && !onPointerMove && !onPointerUp && !onPointerCancel && !onPointerScroll)
    {
        AXASSERT(false, "Invalid PointerEventListener!");
        return false;
    }

    return true;
}

PointerEventListener* PointerEventListener::clone()
{
    auto ret = new PointerEventListener();
    if (ret->init())
    {
        ret->autorelease();

        ret->onPointerHitTest = onPointerHitTest;
        ret->onPointerDown    = onPointerDown;
        ret->onPointerMove    = onPointerMove;
        ret->onPointerUp      = onPointerUp;
        ret->onPointerCancel  = onPointerCancel;
        ret->onPointerScroll  = onPointerScroll;
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

}  // namespace ax
