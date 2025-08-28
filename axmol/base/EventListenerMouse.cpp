/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include "axmol/base/EventListenerMouse.h"

namespace ax
{

const std::string EventListenerMouse::LISTENER_ID = "__ax_mouse";

bool EventListenerMouse::checkAvailable()
{
    return true;
}

void EventListenerMouse::setSwallowMouse(bool needSwallow)
{
    _needSwallow = needSwallow;
}

bool EventListenerMouse::isSwallowMouse()
{
    return _needSwallow;
}

EventListenerMouse* EventListenerMouse::create()
{
    auto ret = new EventListenerMouse();
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

EventListenerMouse* EventListenerMouse::clone()
{
    auto ret = new EventListenerMouse();
    if (ret->init())
    {
        ret->autorelease();
        ret->onMouseUp     = onMouseUp;
        ret->onMouseDown   = onMouseDown;
        ret->onMouseMove   = onMouseMove;
        ret->onMouseScroll = onMouseScroll;
        ret->_needSwallow  = _needSwallow;
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

EventListenerMouse::EventListenerMouse()
    : onMouseDown(nullptr), onMouseUp(nullptr), onMouseMove(nullptr), onMouseScroll(nullptr), _needSwallow(false)
{}

bool EventListenerMouse::init()
{
    if (EventListener::init(Type::MOUSE, LISTENER_ID, nullptr))
    {
        return true;
    }

    return false;
}

}  // namespace ax
