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

#ifndef __AX_PU_PARTICLE_3D_PLANE_COLLIDER_H__
#define __AX_PU_PARTICLE_3D_PLANE_COLLIDER_H__

#include "PUBaseCollider.h"
#include "Particle3D/PU/PUPlane.h"
#include "base/Types.h"

NS_AX_BEGIN

class AX_EX_DLL PUPlaneCollider : public PUBaseCollider
{
public:
    // Constants
    static const Vec3 DEFAULT_NORMAL;

    static PUPlaneCollider* create();

    virtual void notifyRescaled(const Vec3& scale) override;
    virtual void updatePUAffector(PUParticle3D* particle, float deltaTime) override;

    /** Returns the normal of the plane
     */
    const Vec3 getNormal() const;

    /** Sets the normal of the plane
     */
    void setNormal(const Vec3& normal);

    /**
     */
    void calculateDirectionAfterCollision(PUParticle3D* particle, float timeElapsed);

    virtual void copyAttributesTo(PUAffector* affector) override;

    PUPlaneCollider();
    virtual ~PUPlaneCollider();

protected:
    Vec3 _normal;
    Vec3 _predictedPosition;
    PUPlane _plane;
};
NS_AX_END

#endif