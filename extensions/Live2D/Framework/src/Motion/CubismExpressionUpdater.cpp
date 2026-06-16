/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismExpressionUpdater.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

CubismExpressionUpdater::CubismExpressionUpdater(CubismExpressionMotionManager& expressionManager)
    : ICubismUpdater(CubismUpdateOrder_Expression)
    , _expressionManager(expressionManager)
{
}

CubismExpressionUpdater::CubismExpressionUpdater(CubismExpressionMotionManager& expressionManager, const csmInt32 executionOrder)
    : ICubismUpdater(executionOrder)
    , _expressionManager(expressionManager)
{
}

void CubismExpressionUpdater::OnLateUpdate(CubismModel* model, const csmFloat32 deltaTimeSeconds)
{
    if (model == nullptr)
    {
        return;
    }

    // 表情でパラメータ更新（相対変化）
    _expressionManager.UpdateMotion(model, deltaTimeSeconds);
}

}}}
