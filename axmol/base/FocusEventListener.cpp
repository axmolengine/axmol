/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/base/FocusEventListener.h"
#include "axmol/base/FocusEvent.h"
#include "axmol/base/Macros.h"

namespace ax
{

const std::string_view FocusEventListener::LISTENER_ID = "__ax_focus_event"sv;

FocusEventListener::FocusEventListener() : onFocusChanged(nullptr) {}

FocusEventListener::~FocusEventListener()
{
    AXLOGV("In the destructor of FocusEventListener, {}", fmt::ptr(this));
}

FocusEventListener* FocusEventListener::create()
{
    FocusEventListener* ret = new FocusEventListener;
    if (ret->init())
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

FocusEventListener* FocusEventListener::clone()
{
    FocusEventListener* ret = new FocusEventListener;
    if (ret->init())
    {
        ret->autorelease();

        ret->onFocusChanged = onFocusChanged;
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

bool FocusEventListener::init()
{
    auto listener = [this](Event* event) {
        auto focusEvent = static_cast<FocusEvent*>(event);
        onFocusChanged(focusEvent->_widgetLoseFocus, focusEvent->_widgetGetFocus);
    };
    if (EventListener::init(Type::FOCUS, LISTENER_ID, listener))
    {
        return true;
    }
    return false;
}

bool FocusEventListener::checkAvailable()
{
    if (onFocusChanged == nullptr)
    {
        AXASSERT(false, "Invalid FocusEventListener!");
        return false;
    }

    return true;
}

}  // namespace ax
