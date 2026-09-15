/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/base/AccelerationEventListener.h"
#include "axmol/base/AccelerationEvent.h"
#include "axmol/base/Logging.h"

namespace ax
{

const std::string_view AccelerationEventListener::LISTENER_ID = "__ax_acceleration"sv;

AccelerationEventListener::AccelerationEventListener() {}

AccelerationEventListener::~AccelerationEventListener()
{
    AXLOGV("In the destructor of AccelerationEventListener. {}", fmt::ptr(this));
}

AccelerationEventListener* AccelerationEventListener::create(const std::function<void(AccelerationEvent*)>& callback)
{
    AccelerationEventListener* ret = new AccelerationEventListener();
    if (ret->init(callback))
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }

    return ret;
}

bool AccelerationEventListener::init(const std::function<void(AccelerationEvent* event)>& callback)
{
    auto listener = [this](Event* event) {
        auto accEvent = static_cast<AccelerationEvent*>(event);
        this->onAcceleration(accEvent);
    };

    if (EventListener::init(Type::ACCELERATION, LISTENER_ID, listener))
    {
        onAcceleration = callback;
        return true;
    }

    return false;
}

AccelerationEventListener* AccelerationEventListener::clone()
{
    auto ret = new AccelerationEventListener();

    if (ret->init(onAcceleration))
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }

    return ret;
}

bool AccelerationEventListener::checkAvailable()
{
    AXASSERT(onAcceleration, "onAcceleration can't be nullptr!");

    return true;
}

}  // namespace ax
