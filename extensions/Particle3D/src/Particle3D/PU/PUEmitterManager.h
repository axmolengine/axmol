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

#ifndef __AX_PU_PARTICLE_3D_EMITTER_MANAGER_H__
#define __AX_PU_PARTICLE_3D_EMITTER_MANAGER_H__

#include "base/Object.h"
#include "Particle3D/PU/PUScriptTranslator.h"
#include "Particle3D/PU/PUEmitter.h"
#include "Particle3D/PU/PUBoxEmitterTranslator.h"
#include "Particle3D/PU/PUCircleEmitterTranslator.h"
#include "Particle3D/PU/PULineEmitterTranslator.h"
#include "Particle3D/PU/PUMeshSurfaceEmitterTranslator.h"
#include "Particle3D/PU/PUPointEmitterTranslator.h"
#include "Particle3D/PU/PUPositionEmitterTranslator.h"
#include "Particle3D/PU/PUSlaveEmitterTranslator.h"
#include "Particle3D/PU/PUSphereSurfaceEmitterTranslator.h"

namespace ax
{

class PUEmitterManager
{
public:
    static PUEmitterManager* Instance();

    /**
     */
    PUScriptTranslator* getTranslator(std::string_view type);
    PUEmitter* createEmitter(std::string_view type);

    PUEmitterManager();
    ~PUEmitterManager();

protected:
    CCPUBoxEmitterTranslator _boxEmitterTranlator;
    PUCircleEmitterTranslator _circleEmitterTranlator;
    PULineEmitterTranslator _lineEmitterTranlator;
    PUMeshSurfaceEmitterTranslator _meshSurfaceEmitterTranlator;
    PUPointEmitterTranslator _pointEmitterTranlator;
    PUPositionEmitterTranslator _positionEmitterTranlator;
    PUSlaveEmitterTranslator _slaveEmitterTranlator;
    PUSphereSurfaceEmitterTranslator _sphereSurfaceEmitterTranlator;
};

}

#endif
