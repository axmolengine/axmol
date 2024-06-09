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

#ifndef __AX_PU_PARTICLE_3D_AFFECTOR_MANAGER_H__
#define __AX_PU_PARTICLE_3D_AFFECTOR_MANAGER_H__

#include "base/Object.h"
#include "Particle3D/PU/PUScriptTranslator.h"
#include "Particle3D/PU/PUAffector.h"
#include "Particle3D/PU/PUAlignAffectorTranslator.h"
#include "Particle3D/PU/PUBaseColliderTranslator.h"
#include "Particle3D/PU/PUBaseForceAffectorTranslator.h"
#include "Particle3D/PU/PUBoxColliderTranslator.h"
#include "Particle3D/PU/PUCollisionAvoidanceAffectorTranslator.h"
#include "Particle3D/PU/PUColorAffectorTranslator.h"
#include "Particle3D/PU/PUFlockCenteringAffectorTranslator.h"
#include "Particle3D/PU/PUForceFieldAffectorTranslator.h"
#include "Particle3D/PU/PUGeometryRotatorTranslator.h"
#include "Particle3D/PU/PUGravityAffectorTranslator.h"
#include "Particle3D/PU/PUInterParticleColliderTranslator.h"
#include "Particle3D/PU/PUJetAffectorTranslator.h"
#include "Particle3D/PU/PULineAffectorTranslator.h"
#include "Particle3D/PU/PULinearForceAffectorTranslator.h"
#include "Particle3D/PU/PUParticleFollowerTranslator.h"
#include "Particle3D/PU/PUPathFollowerTranslator.h"
#include "Particle3D/PU/PUPlaneColliderTranslator.h"
#include "Particle3D/PU/PURandomiserTranslator.h"
#include "Particle3D/PU/PUScaleAffectorTranslator.h"
#include "Particle3D/PU/PUScaleVelocityAffectorTranslator.h"
#include "Particle3D/PU/PUSineForceAffectorTranslator.h"
#include "Particle3D/PU/PUSphereColliderTranslator.h"
#include "Particle3D/PU/PUTextureAnimatorTranslator.h"
#include "Particle3D/PU/PUTextureRotatorTranslator.h"
#include "Particle3D/PU/PUVelocityMatchingAffectorTranslator.h"
#include "Particle3D/PU/PUVortexAffectorTranslator.h"

NS_AX_BEGIN

class AX_EX_DLL PUAffectorManager
{
public:
    static PUAffectorManager* Instance();

    /**
     */
    PUScriptTranslator* getTranslator(std::string_view type);
    PUAffector* createAffector(std::string_view type);

    PUAffectorManager();
    ~PUAffectorManager();

protected:
    PUAlignAffectorTranslator _alignAffectorTranslator;
    PUBaseColliderTranslator _baseColliderTranslator;
    PUBaseForceAffectorTranslator _baseForceAffectorTranslator;
    PUBoxColliderTranslator _boxColliderTranslator;
    PUCollisionAvoidanceAffectorTranslator _collisionAvoidanceAffectorTranstor;
    PUColorAffectorTranslator _colorAffectorTranslator;
    PUFlockCenteringAffectorTranslator _flockCenteringAffectorTranslator;
    PUForceFieldAffectorTranslator _forceFieldAffectorTranslator;
    PUGeometryRotatorTranslator _geometryRotatorTranslator;
    PUGravityAffectorTranslator _gravityAffectorTranslator;
    PUInterParticleColliderTranslator _interParticleColliderTranslator;
    PUJetAffectorTranslator _jetAffectorTranslator;
    PULineAffectorTranslator _lineAffectorTranslator;
    PULinearForceAffectorTranslator _linearForceAffectorTranslator;
    PUParticleFollowerTranslator _particleFollowerTranslator;
    PUPathFollowerTranslator _pathFollowerTranslator;
    PUPlaneColliderTranslator _planeColliderTranslator;
    PURandomiserTranslator _randomiserTranslator;
    PUScaleAffectorTranslator _scaleAffectorTranslator;
    PUScaleVelocityAffectorTranslator _scaleVelocityAffectorTranslator;
    PUSineForceAffectorTranslator _sineForceAffectorTranslator;
    PUSphereColliderTranslator _sphereColliderTranslator;
    PUTextureAnimatorTranslator _textureAnimatorTranslator;
    PUTextureRotatorTranslator _textureRotatorTranslator;
    PUVelocityMatchingAffectorTranslator _velocityMatchingAffectorTranslator;
    PUVortexAffectorTranslator _vortexAffectorTranslator;
};

NS_AX_END

#endif
