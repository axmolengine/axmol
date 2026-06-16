/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismEyeBlinkUpdater.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

CubismEyeBlinkUpdater::CubismEyeBlinkUpdater(const csmBool& motionUpdated, CubismEyeBlink& eyeBlink)
    : ICubismUpdater(CubismUpdateOrder_EyeBlink)
    , _motionUpdated(motionUpdated)
    , _eyeBlink(eyeBlink)
{
}

CubismEyeBlinkUpdater::CubismEyeBlinkUpdater(const csmBool& motionUpdated, CubismEyeBlink& eyeBlink, const csmInt32 executionOrder)
    : ICubismUpdater(executionOrder)
    , _motionUpdated(motionUpdated)
    , _eyeBlink(eyeBlink)
{
}

void CubismEyeBlinkUpdater::OnLateUpdate(CubismModel* model, csmFloat32 deltaTimeSeconds)
{
    if (model == nullptr)
    {
        return;
    }

    if (!_motionUpdated)
    {
        // メインモーションの更新がないとき
        // 目パチ
        _eyeBlink.UpdateParameters(model, deltaTimeSeconds);
    }
}

}}}
