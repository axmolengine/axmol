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

#include "Particle3D/PU/PUEventHandlerManager.h"
#include "Particle3D/PU/PUDoAffectorEventHandler.h"
#include "Particle3D/PU/PUDoEnableComponentEventHandler.h"
#include "Particle3D/PU/PUDoExpireEventHandler.h"
#include "Particle3D/PU/PUDoFreezeEventHandler.h"
#include "Particle3D/PU/PUDoPlacementParticleEventHandler.h"
#include "Particle3D/PU/PUDoScaleEventHandler.h"
#include "Particle3D/PU/PUDoStopSystemEventHandler.h"

NS_AX_BEGIN
PUEventHandlerManager::PUEventHandlerManager() {}

PUEventHandlerManager::~PUEventHandlerManager() {}

PUEventHandlerManager* PUEventHandlerManager::Instance()
{
    static PUEventHandlerManager pem;
    return &pem;
}

PUScriptTranslator* PUEventHandlerManager::getTranslator(std::string_view type)
{
    if (type == "DoAffector")
    {
        return &_doAffectorEventHandlerTranslator;
    }
    else if (type == "DoEnableComponent")
    {
        return &_doEnableComponentEventHandlerTranslator;
    }
    else if (type == "DoExpire")
    {
        return &_doExpireEventHandlerTranslator;
    }
    else if (type == "DoFreeze")
    {
        return &_doFreezeEventHandlerTranslator;
    }
    else if (type == "DoPlacementParticle")
    {
        return &_doPlacementParticleEventHandlerTranslator;
    }
    else if (type == "DoScale")
    {
        return &_doScaleEventHandlerTranslator;
    }
    else if (type == "DoStopSystem")
    {
        return &_doStopSystemEventHandlerTranslator;
    }
    return nullptr;
}

PUEventHandler* PUEventHandlerManager::createEventHandler(std::string_view type)
{
    if (type == "DoAffector")
    {
        return PUDoAffectorEventHandler::create();
    }
    else if (type == "DoEnableComponent")
    {
        return PUDoEnableComponentEventHandler::create();
    }
    else if (type == "DoExpire")
    {
        return PUDoExpireEventHandler::create();
    }
    else if (type == "DoFreeze")
    {
        return PUDoFreezeEventHandler::create();
    }
    else if (type == "DoPlacementParticle")
    {
        return PUDoPlacementParticleEventHandler::create();
    }
    else if (type == "DoScale")
    {
        return PUDoScaleEventHandler::create();
    }
    else if (type == "DoStopSystem")
    {
        return PUDoStopSystemEventHandler::create();
    }
    return nullptr;
}

NS_AX_END
