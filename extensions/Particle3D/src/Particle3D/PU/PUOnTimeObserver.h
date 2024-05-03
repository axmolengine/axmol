/****************************************************************************
 Copyright (C) 2013 Henry van Merode. All rights reserved.
 Copyright (c) 2015-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axmolengine.github.io/

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

#ifndef __AX_PU_PARTICLE_3D_ON_TIME_OBSERVER_H__
#define __AX_PU_PARTICLE_3D_ON_TIME_OBSERVER_H__

#include "base/Object.h"
#include "math/Math.h"
#include "Particle3D/PU/PUObserver.h"
#include <vector>
#include <string>

NS_AX_BEGIN

struct PUParticle3D;
class PUParticleSystem3D;
class AX_EX_DLL PUOnTimeObserver : public PUObserver
{
public:
    // Constants
    static const float DEFAULT_THRESHOLD;
    static const bool DEFAULT_SINCE_START_SYSTEM;

    static PUOnTimeObserver* create();

    /** In case there are no particles, but the observation returns true, the event handlers must still be
        called.
    */
    virtual void preUpdateObserver(float deltaTime) override;

    /**
     */
    virtual bool observe(PUParticle3D* particle, float timeElapsed) override;

    /**
     */
    float getThreshold() const { return _threshold; };
    void setThreshold(float threshold) { _threshold = threshold; };

    /**
     */
    PUComparisionOperator getCompare() const { return _compare; };
    void setCompare(PUComparisionOperator op) { _compare = op; };

    /**
     */
    bool isSinceStartSystem() const { return _sinceStartSystem; };
    void setSinceStartSystem(bool sinceStartSystem) { _sinceStartSystem = sinceStartSystem; };

    virtual void copyAttributesTo(PUObserver* observer) override;

    PUOnTimeObserver();
    virtual ~PUOnTimeObserver(){};

protected:
    float _threshold;
    PUComparisionOperator _compare;
    bool _sinceStartSystem;
};

NS_AX_END

#endif
