/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismBreathUpdater.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

CubismBreathUpdater::CubismBreathUpdater(CubismBreath& breath)
    : ICubismUpdater(CubismUpdateOrder_Breath)
    , _breath(breath)
{
}

CubismBreathUpdater::CubismBreathUpdater(CubismBreath& breath, const csmInt32 executionOrder)
    : ICubismUpdater(executionOrder)
    , _breath(breath)
{
}

void CubismBreathUpdater::OnLateUpdate(CubismModel* model, const csmFloat32 deltaTimeSeconds)
{
    if (model == nullptr)
    {
        return;
    }

    _breath.UpdateParameters(model, deltaTimeSeconds);
}

}}}
