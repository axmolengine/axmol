/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/base/CustomEventListener.h"
#include "axmol/base/CustomEvent.h"

namespace ax
{

CustomEventListener::CustomEventListener() : _onCustomEvent(nullptr) {}

CustomEventListener* CustomEventListener::create(std::string_view eventName,
                                                 const std::function<void(CustomEvent*)>& callback)
{
    CustomEventListener* ret = new CustomEventListener();
    if (ret->init(eventName, callback))
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

bool CustomEventListener::init(std::string_view listenerId, const std::function<void(CustomEvent*)>& callback)
{
    bool ret = false;

    _onCustomEvent = callback;

    auto listener = [this](Event* event) {
        if (_onCustomEvent != nullptr)
        {
            _onCustomEvent(static_cast<CustomEvent*>(event));
        }
    };

    if (EventListener::init(EventListener::Type::CUSTOM, listenerId, listener))
    {
        ret = true;
    }
    return ret;
}

CustomEventListener* CustomEventListener::clone()
{
    CustomEventListener* ret = new CustomEventListener();
    if (ret->init(_listenerID, _onCustomEvent))
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

bool CustomEventListener::checkAvailable()
{
    bool ret = false;
    if (EventListener::checkAvailable() && _onCustomEvent != nullptr)
    {
        ret = true;
    }
    return ret;
}

}  // namespace ax
