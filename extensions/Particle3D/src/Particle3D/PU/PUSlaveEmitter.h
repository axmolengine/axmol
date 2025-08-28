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

#pragma once

#include "Particle3D/PU/PUEmitter.h"
#include "Particle3D/PU/PUListener.h"

namespace ax
{
// FIXME
class AX_EX_DLL PUSlaveEmitter : public PUEmitter, public PUListener
{
public:
    static PUSlaveEmitter* create();

    /**
     */
    std::string_view getMasterTechniqueName() const;

    /**
     */
    void setMasterTechniqueName(std::string_view masterTechniqueName);

    /**
     */
    std::string_view getMasterEmitterName() const;

    /** See ParticleEmitter.
     */
    void initParticlePosition(PUParticle3D* particle) override;

    /** See ParticleEmitter.
     */
    void initParticleDirection(PUParticle3D* particle) override;

    /** See ParticleEmitter.
     */
    void prepare() override;

    /** See ParticleEmitter.
     */
    void unPrepare() override;

    /** See ParticleEmitter.
     */
    void notifyStart() override;

    /** Initialise the emitted particle. This means that its position is set.
     */
    void particleEmitted(PUParticleSystem3D* particleSystem, PUParticle3D* particle) override;

    /** No implementation.
     */
    void particleExpired(PUParticleSystem3D* particleSystem, PUParticle3D* particle) override;

    /**
     */
    void setMasterEmitterName(std::string_view masterEmitterName);

    PUSlaveEmitter* clone() override;
    void copyAttributesTo(PUEmitter* emitter) override;

    PUSlaveEmitter();
    virtual ~PUSlaveEmitter() {};

protected:
    PUParticle3D* _masterParticle;
    std::string _masterTechniqueName;
    std::string _masterEmitterName;
    Vec3 _masterPosition;
    Vec3 _masterDirection;
    bool _masterEmitterNameSet;
};
}  // namespace ax
