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

#include "Particle3D/PU/PUDoAffectorEventHandler.h"
#include "Particle3D/PU/PUAffector.h"
#include "Particle3D/PU/PUParticleSystem3D.h"

NS_AX_BEGIN
// Constants
const bool PUDoAffectorEventHandler::DEFAULT_PRE_POST = false;

//-----------------------------------------------------------------------
PUDoAffectorEventHandler::PUDoAffectorEventHandler() : PUEventHandler(), _prePost(DEFAULT_PRE_POST) {}
//-----------------------------------------------------------------------
void PUDoAffectorEventHandler::handle(PUParticleSystem3D* particleSystem, PUParticle3D* particle, float timeElapsed)
{
    /** Search for the affector.
     */
    PUParticleSystem3D* technique = 0;
    PUAffector* affector          = particleSystem->getAffector(_affectorName);
    if (!affector)
    {
        // Search all techniques in this ParticleSystem for an affector with the correct name
        PUParticleSystem3D* system = particleSystem->getParentParticleSystem();
        auto children              = system->getChildren();
        for (auto&& iter : children)
        {
            technique = dynamic_cast<PUParticleSystem3D*>(iter);
            if (technique)
            {
                affector = technique->getAffector(_affectorName);
                if (affector)
                {
                    break;
                }
            }
        }
    }

    if (affector)
    {
        // Call the affector even if it has enabled set to 'false'.
        if (_prePost)
        {
            affector->preUpdateAffector(timeElapsed);
            affector->updatePUAffector(particle, timeElapsed);
            affector->postUpdateAffector(timeElapsed);
        }
        else
        {
            affector->updatePUAffector(particle, timeElapsed);
        }
    }
}

PUDoAffectorEventHandler* PUDoAffectorEventHandler::create()
{
    auto peh = new PUDoAffectorEventHandler();
    peh->autorelease();
    return peh;
}

void PUDoAffectorEventHandler::copyAttributesTo(PUEventHandler* eventHandler)
{
    PUEventHandler::copyAttributesTo(eventHandler);
    PUDoAffectorEventHandler* doAffectorEventHandler = static_cast<PUDoAffectorEventHandler*>(eventHandler);
    doAffectorEventHandler->setAffectorName(_affectorName);
    doAffectorEventHandler->setPrePost(_prePost);
}

NS_AX_END
