/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
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
