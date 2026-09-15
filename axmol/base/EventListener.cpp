/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.
 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/base/EventListener.h"
#include "axmol/base/Logging.h"

namespace ax
{

EventListener::EventListener() {}

EventListener::~EventListener()
{
    AXLOGV("In the destructor of EventListener. {}", fmt::ptr(this));
}

bool EventListener::init(Type t, std::string_view listenerID, const std::function<void(Event*)>& callback)
{
    _onEvent    = callback;
    _type       = t;
    _listenerID = listenerID;
    _isAttached = false;
    _paused     = false;
    _isEnabled  = true;

    return true;
}

bool EventListener::checkAvailable()
{
    return (_onEvent != nullptr);
}

}  // namespace ax
