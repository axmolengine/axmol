/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
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

#include "EventListenerAssetsManagerEx.h"
#include "EventAssetsManagerEx.h"
#include "AssetsManagerEx.h"
#include "base/UTF8.h"

NS_AX_EXT_BEGIN

const std::string EventListenerAssetsManagerEx::LISTENER_ID = "__cc_assets_manager_";

EventListenerAssetsManagerEx::EventListenerAssetsManagerEx()
    : _onAssetsManagerExEvent(nullptr), _AssetsManagerEx(nullptr)
{}

EventListenerAssetsManagerEx* EventListenerAssetsManagerEx::create(
    ax::extension::AssetsManagerEx* AssetsManagerEx,
    const std::function<void(EventAssetsManagerEx*)>& callback)
{
    EventListenerAssetsManagerEx* ret = new EventListenerAssetsManagerEx();
    if (ret->init(AssetsManagerEx, callback))
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

bool EventListenerAssetsManagerEx::init(const AssetsManagerEx* AssetsManagerEx,
                                        const std::function<void(EventAssetsManagerEx*)>& callback)
{
    bool ret = false;

    _AssetsManagerEx        = AssetsManagerEx;
    _onAssetsManagerExEvent = callback;

    auto func = [this](EventCustom* event) -> void {
        EventAssetsManagerEx* eventAssetsManagerEx = dynamic_cast<EventAssetsManagerEx*>(event);
        _onAssetsManagerExEvent(eventAssetsManagerEx);
    };
    std::string pointer = fmt::format("{}", fmt::ptr(AssetsManagerEx));
    if (EventListenerCustom::init(LISTENER_ID + pointer, func))
    {
        ret = true;
    }
    return ret;
}

EventListenerAssetsManagerEx* EventListenerAssetsManagerEx::clone()
{
    EventListenerAssetsManagerEx* ret = new EventListenerAssetsManagerEx();
    if (ret->init(_AssetsManagerEx, _onAssetsManagerExEvent))
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

bool EventListenerAssetsManagerEx::checkAvailable()
{
    bool ret = false;
    if (EventListener::checkAvailable() && _AssetsManagerEx != nullptr && _onAssetsManagerExEvent != nullptr)
    {
        ret = true;
    }
    return ret;
}

NS_AX_EXT_END
