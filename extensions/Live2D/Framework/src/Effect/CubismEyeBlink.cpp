/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismEyeBlink.hpp"
#include "Id/CubismId.hpp"
#include <stdlib.h>

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * IDで指定された目のパラメータが、0のときに閉じるなら true 、1の時に閉じるなら false 。
 */
const csmBool CloseIfZero = true;

CubismEyeBlink* CubismEyeBlink::Create(ICubismModelSetting* modelSetting)
{
    return CSM_NEW CubismEyeBlink(modelSetting);
}

void CubismEyeBlink::Delete(CubismEyeBlink* eyeBlink)
{
    CSM_DELETE_SELF(CubismEyeBlink, eyeBlink);
}

CubismEyeBlink::CubismEyeBlink(ICubismModelSetting* modelSetting)
    : _blinkingState(EyeState_First)
    , _nextBlinkingTime(0.0f)
    , _stateStartTimeSeconds(0.0f)
    , _blinkingIntervalSeconds(4.0f)
    , _closingSeconds(0.1f)
    , _closedSeconds(0.05f)
    , _openingSeconds(0.15f)
    , _userTimeSeconds(0.0f)
{
    if (modelSetting == NULL)
    {
        return;
    }

    for (csmInt32 i = 0; i < modelSetting->GetEyeBlinkParameterCount(); ++i)
    {
        _parameterIds.PushBack(modelSetting->GetEyeBlinkParameterId(i));
    }
}

CubismEyeBlink::~CubismEyeBlink()
{ }

csmFloat32 CubismEyeBlink::DeterminNextBlinkingTiming() const
{
    const csmFloat32 r = static_cast<csmFloat32>(rand()) / RAND_MAX;

    return _userTimeSeconds + (r * (2.0f * _blinkingIntervalSeconds - 1.0f));
}

void CubismEyeBlink::SetBlinkingInterval(csmFloat32 blinkingInterval)
{
    _blinkingIntervalSeconds = blinkingInterval;
}

void CubismEyeBlink::SetBlinkingSettings(csmFloat32 closing, csmFloat32 closed, csmFloat32 opening)
{
    _closingSeconds = closing;
    _closedSeconds =  closed;
    _openingSeconds = opening;
}

void CubismEyeBlink::SetParameterIds(const csmVector<CubismIdHandle>& parameterIds)
{
    _parameterIds = parameterIds;
}

const csmVector<CubismIdHandle>& CubismEyeBlink::GetParameterIds() const
{
    return _parameterIds;
}

void CubismEyeBlink::UpdateParameters(CubismModel* model, csmFloat32 deltaTimeSeconds)
{
    _userTimeSeconds += deltaTimeSeconds;
    csmFloat32 parameterValue;
    csmFloat32 t = 0.0f;

    switch (_blinkingState)
    {
    case EyeState_Closing:
        t = ((_userTimeSeconds - _stateStartTimeSeconds) / _closingSeconds);

        if (t >= 1.0f)
        {
            t = 1.0f;
            _blinkingState = EyeState_Closed;
            _stateStartTimeSeconds = _userTimeSeconds;
        }

        parameterValue = 1.0f - t;

        break;
    case EyeState_Closed:
        t = ((_userTimeSeconds - _stateStartTimeSeconds) / _closedSeconds);

        if (t >= 1.0f)
        {
            _blinkingState = EyeState_Opening;
            _stateStartTimeSeconds = _userTimeSeconds;
        }

        parameterValue = 0.0f;

        break;
    case EyeState_Opening:
        t = ((_userTimeSeconds - _stateStartTimeSeconds) /_openingSeconds);

        if (t >= 1.0f)
        {
            t = 1.0f;
            _blinkingState = EyeState_Interval;
            _nextBlinkingTime = DeterminNextBlinkingTiming();
        }

        parameterValue = t;

        break;
    case EyeState_Interval:
        if (_nextBlinkingTime < _userTimeSeconds)
        {
            _blinkingState = EyeState_Closing;
            _stateStartTimeSeconds = _userTimeSeconds;
        }

        parameterValue = 1.0f;

        break;
    case EyeState_First:
    default:
        _blinkingState = EyeState_Interval;
        _nextBlinkingTime = DeterminNextBlinkingTiming();

        parameterValue = 1.0f;

        break;
    }

    if (!CloseIfZero)
    {
        parameterValue = -parameterValue;
    }

    for (csmUint32 i = 0; i < _parameterIds.GetSize(); ++i)
    {
        model->SetParameterValue(_parameterIds[i], parameterValue);
    }
}

}}}

