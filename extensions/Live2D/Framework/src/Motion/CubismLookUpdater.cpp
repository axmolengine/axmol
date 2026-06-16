/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismLookUpdater.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

CubismLookUpdater::CubismLookUpdater(
    const CubismLook& look,
  CubismTargetPoint& dragManager)
    : ICubismUpdater(CubismUpdateOrder_Look)
    , _look(look)
    , _dragManager(dragManager)
{
}

CubismLookUpdater::CubismLookUpdater(
    const CubismLook& look,
    CubismTargetPoint& dragManager,
    const csmInt32 executionOrder)
    : ICubismUpdater(executionOrder)
    , _look(look)
    , _dragManager(dragManager)
{
}

void CubismLookUpdater::OnLateUpdate(CubismModel* model, const csmFloat32 deltaTimeSeconds)
{
    if (model == nullptr)
    {
        return;
    }

    _dragManager.Update(deltaTimeSeconds);
    const csmFloat32 dragX = _dragManager.GetX();
    const csmFloat32 dragY = _dragManager.GetY();

    _look.UpdateParameters(model, dragX, dragY);
}

}}}
