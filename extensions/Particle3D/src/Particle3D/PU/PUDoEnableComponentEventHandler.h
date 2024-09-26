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

#ifndef __AX_PU_PARTICLE_3D_DO_ENABLE_COMPONENT_EVENT_HANDLER_H__
#define __AX_PU_PARTICLE_3D_DO_ENABLE_COMPONENT_EVENT_HANDLER_H__

#include "base/Object.h"
#include "math/Math.h"
#include "Particle3D/PU/PUParticleSystem3D.h"
#include "Particle3D/PU/PUEventHandler.h"
#include <vector>
#include <string>

namespace ax
{

struct PUParticle3D;
class PUObserver;
class PUParticleSystem3D;

class AX_EX_DLL PUDoEnableComponentEventHandler : public PUEventHandler
{
public:
    static PUDoEnableComponentEventHandler* create();

    /** Get the name of the component that must be enabled or disabled.
     */
    std::string_view getComponentName() const { return _componentName; };

    /** Set the name of the component that must be enabled or disables.
     */
    void setComponentName(std::string_view componentName) { _componentName = componentName; };

    /** Get the value that identifies whether the component must be enabled or disabled.
     */
    bool isComponentEnabled() const { return _componentEnabled; };

    /** Set the value that identifies whether the component must be enabled or disabled.
     */
    void setComponentEnabled(bool enabled) { _componentEnabled = enabled; };

    /** Get the value that identifies whether the component must be enabled or disabled.
     */
    PUComponentType getComponentType() const { return _componentType; };

    /** Set the value that identifies whether the component must be enabled or disabled.
     */
    void setComponentType(PUComponentType componentType) { _componentType = componentType; };

    /** If the _handle() function of this class is invoked (by an Observer), it searches the
        ParticleEmitter, ParticleAffector or ParticleTechnique defined by the its name.
        The ParticleEmitter/Affector is either part of the ParticleTechnique in which the
        DoEnableComponentEventHandler is defined, or if the ParticleEmitter/Affector is not
        found, other ParticleTechniques are searched.
    */
    virtual void handle(PUParticleSystem3D* particleSystem, PUParticle3D* particle, float timeElapsed) override;

    virtual void copyAttributesTo(PUEventHandler* eventHandler) override;

    PUDoEnableComponentEventHandler();
    virtual ~PUDoEnableComponentEventHandler(){};

protected:
    // Identifies the name of component
    std::string _componentName;

    // Identifies the type of component
    PUComponentType _componentType;

    /** Determines whether the Component must be enabled or disabled.
     */
    bool _componentEnabled;
};

}

#endif
