/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismPhysicsUpdater.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

CubismPhysicsUpdater::CubismPhysicsUpdater(CubismPhysics& physics)
    : ICubismUpdater(CubismUpdateOrder_Physics)
    , _physics(physics)
{
}

CubismPhysicsUpdater::CubismPhysicsUpdater(CubismPhysics& physics, const csmInt32 executionOrder)
    : ICubismUpdater(executionOrder)
    , _physics(physics)
{
}

void CubismPhysicsUpdater::OnLateUpdate(CubismModel* model, const csmFloat32 deltaTimeSeconds)
{
    if (model == nullptr)
    {
        return;
    }

    _physics.Evaluate(model, deltaTimeSeconds);
}

}}}
