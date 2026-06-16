/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "ACubismMotion.hpp"
#include "Model/CubismModel.hpp"
#include "CubismMotionQueueEntry.hpp"
#include "Math/CubismMath.hpp"


namespace Live2D { namespace Cubism { namespace Framework {

void ACubismMotion::Delete(ACubismMotion* motion)
{
    CSM_DELETE_SELF(ACubismMotion, motion);
}

ACubismMotion::ACubismMotion()
    : _fadeInSeconds(-1.0f)
    , _fadeOutSeconds(-1.0f)
    , _weight(1.0f)
    , _offsetSeconds(0.0f) // 再生の開始時刻
    , _isLoop(false)       // trueから false へデフォルトを変更
    , _isLoopFadeIn(true)  // ループ時にフェードインが有効かどうかのフラグ
    , _previousLoopState(_isLoop)
    , _onBeganMotion(NULL)
    , _onBeganMotionCustomData(NULL)
    , _onFinishedMotion(NULL)
    , _onFinishedMotionCustomData(NULL)
{ }

ACubismMotion::~ACubismMotion()
{
    this->_weight = 0.0f;
}

void ACubismMotion::UpdateParameters(CubismModel* model, CubismMotionQueueEntry* motionQueueEntry, csmFloat32 userTimeSeconds)
{
    if (!motionQueueEntry->IsAvailable() || motionQueueEntry->IsFinished())
    {
        return;
    }

    SetupMotionQueueEntry(motionQueueEntry, userTimeSeconds);

    csmFloat32 fadeWeight = UpdateFadeWeight(motionQueueEntry, userTimeSeconds);

    //---- 全てのパラメータIDをループする ----
    DoUpdateParameters(model, userTimeSeconds, fadeWeight, motionQueueEntry);

    //後処理
    //終了時刻を過ぎたら終了フラグを立てる（CubismMotionQueueManager）
    if ((motionQueueEntry->GetEndTime() > 0.0f) && (motionQueueEntry->GetEndTime() < userTimeSeconds))
    {
        motionQueueEntry->IsFinished(true);      //終了
    }
}

void ACubismMotion::SetupMotionQueueEntry(CubismMotionQueueEntry* motionQueueEntry, csmFloat32 userTimeSeconds)
{
    if (!motionQueueEntry->IsAvailable() || motionQueueEntry->IsFinished()) {
        return;
    }

    if (motionQueueEntry->IsStarted()) {
        return;
    }

    motionQueueEntry->IsStarted(true);
    motionQueueEntry->SetStartTime(userTimeSeconds - _offsetSeconds); //モーションの開始時刻を記録
    motionQueueEntry->SetFadeInStartTime(userTimeSeconds); //フェードインの開始時刻


    if (motionQueueEntry->GetEndTime() < 0)
    {
        //開始していないうちに終了設定している場合がある。
        AdjustEndTime(motionQueueEntry);
    }

    if (this->_onBeganMotion != NULL)
    {
        this->_onBeganMotion(this);
    }
}

csmFloat32 ACubismMotion::UpdateFadeWeight(CubismMotionQueueEntry* motionQueueEntry, csmFloat32 userTimeSeconds)
{
    if (motionQueueEntry == NULL)
    {
        CubismLogError("motionQueueEntry is null.");
        return -1;
    }

    csmFloat32 fadeWeight = _weight; //現在の値と掛け合わせる割合

    //---- フェードイン・アウトの処理 ----
    //単純なサイン関数でイージングする
    const csmFloat32 fadeIn = _fadeInSeconds == 0.0f
        ? 1.0f
        : CubismMath::GetEasingSine((userTimeSeconds - motionQueueEntry->GetFadeInStartTime()) / _fadeInSeconds);

    const csmFloat32 fadeOut = (_fadeOutSeconds == 0.0f || motionQueueEntry->GetEndTime() < 0.0f)
        ? 1.0f
        : CubismMath::GetEasingSine((motionQueueEntry->GetEndTime() - userTimeSeconds) / _fadeOutSeconds);

    fadeWeight = fadeWeight * fadeIn * fadeOut;

    motionQueueEntry->SetState(userTimeSeconds, fadeWeight);

    CSM_ASSERT(0.0f <= fadeWeight && fadeWeight <= 1.0f);

    return fadeWeight;
}


void ACubismMotion::SetFadeInTime(csmFloat32 fadeInSeconds)
{
    this->_fadeInSeconds = fadeInSeconds;
}

void ACubismMotion::SetFadeOutTime(csmFloat32 fadeOutSeconds)
{
    this->_fadeOutSeconds = fadeOutSeconds;
}

csmFloat32 ACubismMotion::GetFadeOutTime() const
{
    return this->_fadeOutSeconds;
}

csmFloat32 ACubismMotion::GetFadeInTime() const
{
    return this->_fadeInSeconds;
}

void ACubismMotion::SetWeight(csmFloat32 weight)
{
    this->_weight = weight;
}

csmFloat32 ACubismMotion::GetWeight() const
{
    return this->_weight;
}

csmFloat32 ACubismMotion::GetDuration()
{
    return -1.0f;
}

csmFloat32 ACubismMotion::GetLoopDuration()
{
    return -1.0f;
}

void ACubismMotion::SetOffsetTime(csmFloat32 offsetSeconds)
{
    this->_offsetSeconds = offsetSeconds;
}

void ACubismMotion::SetLoop(csmBool loop)
{
    this->_isLoop = loop;
}

csmBool ACubismMotion::GetLoop() const
{
    return this->_isLoop;
}

void ACubismMotion::SetLoopFadeIn(csmBool loopFadeIn)
{
    this->_isLoopFadeIn = loopFadeIn;
}

csmBool ACubismMotion::GetLoopFadeIn() const
{
    return this->_isLoopFadeIn;
}

const csmVector<const csmString*>& ACubismMotion::GetFiredEvent(csmFloat32 beforeCheckTimeSeconds, csmFloat32 motionTimeSeconds)
{
    return _firedEventValues;
}

void ACubismMotion::SetBeganMotionHandler(BeganMotionCallback onBeganMotionHandler)
{
    this->_onBeganMotion = onBeganMotionHandler;
}

ACubismMotion::BeganMotionCallback ACubismMotion::GetBeganMotionHandler() const
{
    return this->_onBeganMotion;
}

void ACubismMotion::SetBeganMotionCustomData(void* onBeganMotionCustomData)
{
    this->_onBeganMotionCustomData = onBeganMotionCustomData;
}

void* ACubismMotion::GetBeganMotionCustomData() const
{
    return this->_onBeganMotionCustomData;
}

void ACubismMotion::SetBeganMotionHandlerAndMotionCustomData(BeganMotionCallback onBeganMotionHandler,
    void* onBeganMotionCustomData)
{
    this->_onBeganMotion = onBeganMotionHandler;
    this->_onBeganMotionCustomData = onBeganMotionCustomData;
}

void ACubismMotion::SetFinishedMotionHandler(FinishedMotionCallback onFinishedMotionHandler)
{
    this->_onFinishedMotion = onFinishedMotionHandler;
}

ACubismMotion::FinishedMotionCallback ACubismMotion::GetFinishedMotionHandler()
{
    return this->_onFinishedMotion;
}

void ACubismMotion::SetFinishedMotionCustomData(void* onFinishedMotionCustomData)
{
    this->_onFinishedMotionCustomData = onFinishedMotionCustomData;
}

void* ACubismMotion::GetFinishedMotionCustomData()
{
    return this->_onFinishedMotionCustomData;
}

void ACubismMotion::SetFinishedMotionHandlerAndMotionCustomData(FinishedMotionCallback onFinishedMotionHandler, void* onFinishedMotionCustomData)
{
    this->_onFinishedMotion = onFinishedMotionHandler;
    this->_onFinishedMotionCustomData = onFinishedMotionCustomData;
}

csmBool ACubismMotion::IsExistModelOpacity() const
{
    return false;
}

csmInt32 ACubismMotion::GetModelOpacityIndex() const
{
    return -1;
}

CubismIdHandle ACubismMotion::GetModelOpacityId(csmInt32 index)
{
    return NULL;
}

csmFloat32 ACubismMotion::GetModelOpacityValue() const
{
    return 1.0f;
}

void ACubismMotion::AdjustEndTime(CubismMotionQueueEntry* motionQueueEntry)
{
    const csmFloat32 duration = GetDuration();

    // duration == -1 の場合はループする
    const csmFloat32 endTime = (duration <= 0) ?
        -1 :
        motionQueueEntry->GetStartTime() + duration;

    motionQueueEntry->SetEndTime(endTime);
}

}}}
