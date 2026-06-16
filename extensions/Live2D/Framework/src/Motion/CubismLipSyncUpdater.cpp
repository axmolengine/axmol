/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismLipSyncUpdater.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

CubismLipSyncUpdater::CubismLipSyncUpdater(csmVector<CubismIdHandle>& lipSyncIds, IParameterProvider& wavFileHandler)
    : ICubismUpdater(CubismUpdateOrder_LipSync)
    , _wavFileHandler(wavFileHandler)
    , _lipSyncIds(lipSyncIds)
{
}

CubismLipSyncUpdater::CubismLipSyncUpdater(csmVector<CubismIdHandle>& lipSyncIds, IParameterProvider& wavFileHandler, const csmInt32 executionOrder)
    : ICubismUpdater(executionOrder)
    , _wavFileHandler(wavFileHandler)
    , _lipSyncIds(lipSyncIds)
{
}

void CubismLipSyncUpdater::OnLateUpdate(CubismModel* model, const csmFloat32 deltaTimeSeconds)
{
    if (model == nullptr)
    {
        return;
    }

    // リアルタイムでリップシンクを行う場合、システムから音量を取得して0〜1の範囲で値を入力します。
    csmFloat32 value = 0.0f;

    // 状態更新/パラメータ値取得
    _wavFileHandler.Update(deltaTimeSeconds);
    value = _wavFileHandler.GetParameter();

    for (csmUint32 i = 0; i < _lipSyncIds.GetSize(); ++i)
    {
        model->AddParameterValue(_lipSyncIds[i], value, 0.8f);
    }
}

}}}
