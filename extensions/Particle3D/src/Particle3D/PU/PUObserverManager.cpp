/****************************************************************************
 Copyright (C) 2013 Henry van Merode. All rights reserved.
 Copyright (c) 2015-2016 Chukong Technologies Inc.
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

#include "Particle3D/PU/PUObserverManager.h"
#include "Particle3D/PU/PUOnClearObserver.h"
#include "Particle3D/PU/PUOnCollisionObserver.h"
#include "Particle3D/PU/PUOnCountObserver.h"
#include "Particle3D/PU/PUOnEmissionObserver.h"
#include "Particle3D/PU/PUOnEventFlagObserver.h"
#include "Particle3D/PU/PUOnExpireObserver.h"
#include "Particle3D/PU/PUOnPositionObserver.h"
#include "Particle3D/PU/PUOnQuotaObserver.h"
#include "Particle3D/PU/PUOnRandomObserver.h"
#include "Particle3D/PU/PUOnTimeObserver.h"
#include "Particle3D/PU/PUOnVelocityObserver.h"

NS_AX_BEGIN
PUObserverManager::PUObserverManager() {}

PUObserverManager::~PUObserverManager() {}

PUObserverManager* PUObserverManager::Instance()
{
    static PUObserverManager pem;
    return &pem;
}

PUScriptTranslator* PUObserverManager::getTranslator(std::string_view type)
{
    if (type == "OnClear")
    {
        return &_onClearObserverTranslator;
    }
    else if (type == "OnCollision")
    {
        return &_onCollisionObserverTranslator;
    }
    else if (type == "OnCount")
    {
        return &_onCountObserverTranslator;
    }
    else if (type == "OnEmission")
    {
        return &_onEmissionObserverTranslator;
    }
    else if (type == "OnEventFlag")
    {
        return &_onEventFlagObserverTranslator;
    }
    else if (type == "OnExpire")
    {
        return &_onExpireObserverTranslator;
    }
    else if (type == "OnPosition")
    {
        return &_onPositionObserverTranslator;
    }
    else if (type == "OnQuota")
    {
        return &_onQuotaObserverTranslator;
    }
    else if (type == "OnRandom")
    {
        return &_onRandomObserverTranslator;
    }
    else if (type == "OnTime")
    {
        return &_onTimeObserverTranslator;
    }
    else if (type == "OnVelocity")
    {
        return &_onVelocityObserverTranslator;
    }
    return nullptr;
}

PUObserver* PUObserverManager::createObserver(std::string_view type)
{
    if (type == "OnClear")
    {
        return PUOnClearObserver::create();
    }
    else if (type == "OnCollision")
    {
        return PUOnCollisionObserver::create();
    }
    else if (type == "OnCount")
    {
        return PUOnCountObserver::create();
    }
    else if (type == "OnEmission")
    {
        return PUOnEmissionObserver::create();
    }
    else if (type == "OnEventFlag")
    {
        return PUOnEventFlagObserver::create();
    }
    else if (type == "OnExpire")
    {
        return PUOnExpireObserver::create();
    }
    else if (type == "OnPosition")
    {
        return PUOnPositionObserver::create();
    }
    else if (type == "OnQuota")
    {
        return PUOnQuotaObserver::create();
    }
    else if (type == "OnRandom")
    {
        return PUOnRandomObserver::create();
    }
    else if (type == "OnTime")
    {
        return PUOnTimeObserver::create();
    }
    else if (type == "OnVelocity")
    {
        return PUOnVelocityObserver::create();
    }
    return nullptr;
}

NS_AX_END
