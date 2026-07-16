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
