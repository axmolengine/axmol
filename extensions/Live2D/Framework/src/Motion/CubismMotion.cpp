/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismMotion.hpp"
#include <float.h>
#include "CubismFramework.hpp"
#include "CubismMotionInternal.hpp"
#include "CubismMotionJson.hpp"
#include "CubismMotionQueueManager.hpp"
#include "CubismMotionQueueEntry.hpp"
#include "Math/CubismMath.hpp"
#include "Type/csmVector.hpp"
#include "Id/CubismIdManager.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

namespace {

const csmChar* EffectNameEyeBlink = "EyeBlink";
const csmChar* EffectNameLipSync  = "LipSync";
const csmChar* TargetNameModel = "Model";
const csmChar* TargetNameParameter = "Parameter";
const csmChar* TargetNamePartOpacity = "PartOpacity";

// Id
const csmChar* IdNameOpacity = "Opacity";

/**
* Cubism SDK R2 以前のモーションを再現させるなら true 、アニメータのモーションを正しく再現するなら false 。
*/
const csmBool UseOldBeziersCurveMotion = false;

CubismMotionPoint LerpPoints(const CubismMotionPoint a, const CubismMotionPoint b, const csmFloat32 t)
{
    CubismMotionPoint result;

    result.Time = a.Time + ((b.Time - a.Time) * t);
    result.Value = a.Value + ((b.Value - a.Value) * t);

    return result;
}

csmFloat32 LinearEvaluate(const CubismMotionPoint* points, const csmFloat32 time)
{
    csmFloat32 t = (time - points[0].Time) / (points[1].Time - points[0].Time);

    if (t < 0.0f)
    {
        t = 0.0f;
    }

    return points[0].Value + ((points[1].Value - points[0].Value) * t);
}

csmFloat32 BezierEvaluate(const CubismMotionPoint* points, const csmFloat32 time)
{
    csmFloat32 t = (time - points[0].Time) / (points[3].Time - points[0].Time);

    if (t < 0.0f)
    {
        t = 0.0f;
    }

    const CubismMotionPoint p01 = LerpPoints(points[0], points[1], t);
    const CubismMotionPoint p12 = LerpPoints(points[1], points[2], t);
    const CubismMotionPoint p23 = LerpPoints(points[2], points[3], t);

    const CubismMotionPoint p012 = LerpPoints(p01, p12, t);
    const CubismMotionPoint p123 = LerpPoints(p12, p23, t);

    return LerpPoints(p012, p123, t).Value;
}

csmFloat32 BezierEvaluateBinarySearch(const CubismMotionPoint* points, const csmFloat32 time)
{
    const csmFloat32 x_error = 0.01f;

    const csmFloat32 x = time;
    csmFloat32 x1 = points[0].Time;
    csmFloat32 x2 = points[3].Time;
    csmFloat32 cx1 = points[1].Time;
    csmFloat32 cx2 = points[2].Time;

    csmFloat32 ta = 0.0f;
    csmFloat32 tb = 1.0f;
    csmFloat32 t = 0.0f;
    int i = 0;

    for (csmBool var33 = true; i < 20; ++i) {
        if (x < x1 + x_error) {
            t = ta;
            break;
        }

        if (x2 - x_error < x) {
            t = tb;
            break;
        }

        csmFloat32 centerx = (cx1 + cx2) * 0.5f;
        cx1 = (x1 + cx1) * 0.5f;
        cx2 = (x2 + cx2) * 0.5f;
        csmFloat32 ctrlx12 = (cx1 + centerx) * 0.5f;
        csmFloat32 ctrlx21 = (cx2 + centerx) * 0.5f;
        centerx = (ctrlx12 + ctrlx21) * 0.5f;
        if (x < centerx) {
            tb = (ta + tb) * 0.5f;
            if (centerx - x_error < x) {
                t = tb;
                break;
            }

            x2 = centerx;
            cx2 = ctrlx12;
        }
        else {
            ta = (ta + tb) * 0.5f;
            if (x < centerx + x_error) {
                t = ta;
                break;
            }

            x1 = centerx;
            cx1 = ctrlx21;
        }
    }

    if (i == 20) {
        t = (ta + tb) * 0.5f;
    }

    if (t < 0.0f)
    {
        t = 0.0f;
    }
    if (t > 1.0f)
    {
        t = 1.0f;
    }

    const CubismMotionPoint p01 = LerpPoints(points[0], points[1], t);
    const CubismMotionPoint p12 = LerpPoints(points[1], points[2], t);
    const CubismMotionPoint p23 = LerpPoints(points[2], points[3], t);

    const CubismMotionPoint p012 = LerpPoints(p01, p12, t);
    const CubismMotionPoint p123 = LerpPoints(p12, p23, t);

    return LerpPoints(p012, p123, t).Value;
}

csmFloat32 BezierEvaluateCardanoInterpretation(const CubismMotionPoint* points, const csmFloat32 time)
{
    const csmFloat32 x = time;
    csmFloat32 x1 = points[0].Time;
    csmFloat32 x2 = points[3].Time;
    csmFloat32 cx1 = points[1].Time;
    csmFloat32 cx2 = points[2].Time;

    csmFloat32 a = x2 - 3.0f * cx2 + 3.0f * cx1 - x1;
    csmFloat32 b = 3.0f * cx2 - 6.0f * cx1 + 3.0f * x1;
    csmFloat32 c = 3.0f * cx1 - 3.0f * x1;
    csmFloat32 d = x1 - x;

    csmFloat32 t = CubismMath::CardanoAlgorithmForBezier(a, b, c, d);

    const CubismMotionPoint p01 = LerpPoints(points[0], points[1], t);
    const CubismMotionPoint p12 = LerpPoints(points[1], points[2], t);
    const CubismMotionPoint p23 = LerpPoints(points[2], points[3], t);

    const CubismMotionPoint p012 = LerpPoints(p01, p12, t);
    const CubismMotionPoint p123 = LerpPoints(p12, p23, t);

    return LerpPoints(p012, p123, t).Value;
}

csmFloat32 SteppedEvaluate(const CubismMotionPoint* points, const csmFloat32 time)
{
    return points[0].Value;
}

csmFloat32 InverseSteppedEvaluate(const CubismMotionPoint* points, const csmFloat32 time)
{
    return points[1].Value;
}

csmFloat32 EvaluateCurve(const CubismMotionData* motionData, const csmInt32 index, csmFloat32 time)
{
    // Find segment to evaluate.
    const CubismMotionCurve& curve = motionData->Curves[index];

    csmInt32 target = -1;
    const csmInt32 totalSegmentCount = curve.BaseSegmentIndex + curve.SegmentCount;
    csmInt32 pointPosition = 0;
    for (csmInt32 i = curve.BaseSegmentIndex; i < totalSegmentCount; ++i)
    {
        // Get first point of next segment.
        pointPosition = motionData->Segments[i].BasePointIndex
            + (motionData->Segments[i].SegmentType == CubismMotionSegmentType_Bezier
                ? 3
                : 1);


        // Break if time lies within current segment.
        if (motionData->Points[pointPosition].Time > time)
        {
            target = i;
            break;
        }
    }


    if (target == -1)
    {
        return motionData->Points[pointPosition].Value;
    }


    const CubismMotionSegment& segment = motionData->Segments[target];

    return segment.Evaluate(&motionData->Points[segment.BasePointIndex], time);
}

}

CubismMotion::CubismMotion()
    : _sourceFrameRate(30.0f)
    , _loopDurationSeconds(-1.0f)
    , _isLoop(false)                // trueから false へデフォルトを変更
    , _isLoopFadeIn(true)           // ループ時にフェードインが有効かどうかのフラグ
    , _lastWeight(0.0f)
    , _motionData(NULL)
    , _modelCurveIdEyeBlink(NULL)
    , _modelCurveIdLipSync(NULL)
{ }

CubismMotion::~CubismMotion()
{
    CSM_DELETE(_motionData);
}

CubismMotion* CubismMotion::Create(const csmByte* buffer, csmSizeInt size, FinishedMotionCallback onFinishedMotionHandler)
{
    CubismMotion* ret = CSM_NEW CubismMotion();

    ret->Parse(buffer, size);
    ret->_sourceFrameRate = ret->_motionData->Fps;
    ret->_loopDurationSeconds = ret->_motionData->Duration;
    ret->_onFinishedMotion = onFinishedMotionHandler;

    // NOTE: Editorではループありのモーション書き出しは非対応
    // ret->_loop = (ret->_motionData->Loop > 0);

    return ret;
}

csmFloat32 CubismMotion::GetDuration()
{
    return _isLoop ? -1.0f : _loopDurationSeconds;
}

void CubismMotion::DoUpdateParameters(CubismModel* model, csmFloat32 userTimeSeconds, csmFloat32 fadeWeight, CubismMotionQueueEntry* motionQueueEntry)
{
    if (_modelCurveIdEyeBlink == NULL)
    {
        _modelCurveIdEyeBlink = CubismFramework::GetIdManager()->GetId(EffectNameEyeBlink);
    }

    if (_modelCurveIdLipSync == NULL)
    {
        _modelCurveIdLipSync = CubismFramework::GetIdManager()->GetId(EffectNameLipSync);
    }

    csmFloat32 timeOffsetSeconds = userTimeSeconds - motionQueueEntry->GetStartTime();

    if (timeOffsetSeconds < 0.0f)
    {
        timeOffsetSeconds = 0.0f; // エラー回避
    }

    csmFloat32 lipSyncValue = FLT_MAX;
    csmFloat32 eyeBlinkValue = FLT_MAX;

    //まばたき、リップシンクのうちモーションの適用を検出するためのビット（maxFlagCount個まで
    const csmInt32 MaxTargetSize = 64;
    csmUint64 lipSyncFlags = 0ULL;
    csmUint64 eyeBlinkFlags = 0ULL;

    //瞬き、リップシンクのターゲット数が上限を超えている場合
    if (_eyeBlinkParameterIds.GetSize() > MaxTargetSize)
    {
        CubismLogDebug("too many eye blink targets : %d", _eyeBlinkParameterIds.GetSize());
    }
    if (_lipSyncParameterIds.GetSize() > MaxTargetSize)
    {
        CubismLogDebug("too many lip sync targets : %d", _lipSyncParameterIds.GetSize());
    }

    const csmFloat32 tmpFadeIn = (_fadeInSeconds <= 0.0f)
                                     ? 1.0f
                                     : CubismMath::GetEasingSine((userTimeSeconds - motionQueueEntry->GetFadeInStartTime()) / _fadeInSeconds);

    const csmFloat32 tmpFadeOut = (_fadeOutSeconds <= 0.0f || motionQueueEntry->GetEndTime() < 0.0f)
                                      ? 1.0f
                                      : CubismMath::GetEasingSine((motionQueueEntry->GetEndTime() - userTimeSeconds) / _fadeOutSeconds);

    csmFloat32 value;
    csmInt32 c, parameterIndex;

    // 'Repeat' time as necessary.
    csmFloat32 time = timeOffsetSeconds;

    if (_isLoop)
    {
        while (time > _motionData->Duration)
        {
            time -= _motionData->Duration;
        }
    }

    csmVector<CubismMotionCurve>& curves = _motionData->Curves;

    // Evaluate model curves.
    for (c = 0; c < _motionData->CurveCount && curves[c].Type == CubismMotionCurveTarget_Model; ++c)
    {
        // Evaluate curve and call handler.
        value = EvaluateCurve(_motionData, c, time);

        if (curves[c].Id == _modelCurveIdEyeBlink)
        {
            eyeBlinkValue = value;
        }
        else if (curves[c].Id == _modelCurveIdLipSync)
        {
            lipSyncValue = value;
        }
    }

    csmInt32 parameterMotionCurveCount = 0;

    for (; c < _motionData->CurveCount && curves[c].Type == CubismMotionCurveTarget_Parameter; ++c)
    {
        parameterMotionCurveCount++;

        // Find parameter index.
        parameterIndex = model->GetParameterIndex(curves[c].Id);

        // Skip curve evaluation if no value in sink.
        if (parameterIndex == -1)
        {
            continue;
        }

        const csmFloat32 sourceValue = model->GetParameterValue(parameterIndex);

        // Evaluate curve and apply value.
        value = EvaluateCurve(_motionData, c, time);

        if (eyeBlinkValue != FLT_MAX)
        {
            for (csmUint32 i = 0; i < _eyeBlinkParameterIds.GetSize() && i < MaxTargetSize; ++i)
            {
                if (_eyeBlinkParameterIds[i] == curves[c].Id)
                {
                    value *= eyeBlinkValue;
                    eyeBlinkFlags |= 1ULL << i;
                    break;
                }
            }
        }

        if (lipSyncValue != FLT_MAX)
        {
            for (csmUint32 i = 0; i < _lipSyncParameterIds.GetSize() && i < MaxTargetSize; ++i)
            {
                if (_lipSyncParameterIds[i] == curves[c].Id)
                {
                    value += lipSyncValue;
                    lipSyncFlags |= 1ULL << i;
                    break;
                }
            }
        }

        csmFloat32 v;
        // パラメータごとのフェード
        if (curves[c].FadeInTime < 0.0f && curves[c].FadeOutTime < 0.0f)
        {
            //モーションのフェードを適用
            v = sourceValue + (value - sourceValue) * fadeWeight;
        }
        else
        {
            // パラメータに対してフェードインかフェードアウトが設定してある場合はそちらを適用
            csmFloat32 fin;
            csmFloat32 fout;

            if (curves[c].FadeInTime < 0.0f)
            {
                fin = tmpFadeIn;
            }
            else
            {
                fin = curves[c].FadeInTime == 0.0f
                            ? 1.0f
                        : CubismMath::GetEasingSine((userTimeSeconds - motionQueueEntry->GetFadeInStartTime()) / curves[c].FadeInTime);
            }

            if (curves[c].FadeOutTime < 0.0f)
            {
                fout = tmpFadeOut;
            }
            else
            {
                fout = (curves[c].FadeOutTime == 0.0f || motionQueueEntry->GetEndTime() < 0.0f)
                            ? 1.0f
                        : CubismMath::GetEasingSine((motionQueueEntry->GetEndTime() - userTimeSeconds) / curves[c].FadeOutTime );
            }

            const csmFloat32 paramWeight = _weight * fin * fout;

            // パラメータごとのフェードを適用
            v = sourceValue + (value - sourceValue) * paramWeight;
        }

        model->SetParameterValue(parameterIndex, v);
    }

    {
        if (eyeBlinkValue != FLT_MAX)
        {
            for (csmUint32 i = 0; i < _eyeBlinkParameterIds.GetSize() && i < MaxTargetSize; ++i)
            {
                const csmFloat32 sourceValue = model->GetParameterValue(_eyeBlinkParameterIds[i]);
                //モーションでの上書きがあった時にはまばたきは適用しない
                if ((eyeBlinkFlags >> i) & 0x01)
                {
                    continue;
                }

                const csmFloat32 v = sourceValue + (eyeBlinkValue - sourceValue) * fadeWeight;

                model->SetParameterValue(_eyeBlinkParameterIds[i], v);
            }
        }

        if (lipSyncValue != FLT_MAX)
        {
            for (csmUint32 i = 0; i < _lipSyncParameterIds.GetSize() && i < MaxTargetSize; ++i)
            {
                const csmFloat32 sourceValue = model->GetParameterValue(_lipSyncParameterIds[i]);
                //モーションでの上書きがあった時にはリップシンクは適用しない
                if ((lipSyncFlags >> i) & 0x01)
                {
                    continue;
                }

                const csmFloat32 v = sourceValue + (lipSyncValue - sourceValue) * fadeWeight;

                model->SetParameterValue(_lipSyncParameterIds[i], v);
            }
        }
    }

    for (; c < _motionData->CurveCount && curves[c].Type == CubismMotionCurveTarget_PartOpacity; ++c)
    {
        // Find parameter index.
        parameterIndex = model->GetParameterIndex(curves[c].Id);

        // Skip curve evaluation if no value in sink.
        if (parameterIndex == -1)
        {
            continue;
        }

        // Evaluate curve and apply value.
        value = EvaluateCurve(_motionData, c, time);

        model->SetParameterValue(parameterIndex, value);
    }

    if (timeOffsetSeconds >= _motionData->Duration)
    {
        if (_isLoop)
        {
            motionQueueEntry->SetStartTime(userTimeSeconds); //最初の状態へ
            if (_isLoopFadeIn)
            {
                //ループ中でループ用フェードインが有効のときは、フェードイン設定し直し
                motionQueueEntry->SetFadeInStartTime(userTimeSeconds);
            }
        }
        else
        {
            if (this->_onFinishedMotion != NULL)
            {
                this->_onFinishedMotion(this);
            }

            motionQueueEntry->IsFinished(true);
        }
    }

    _lastWeight = fadeWeight;
}

void CubismMotion::Parse(const csmByte* motionJson, const csmSizeInt size)
{
    _motionData = CSM_NEW CubismMotionData;

    CubismMotionJson* json = CSM_NEW CubismMotionJson(motionJson, size);

    _motionData->Duration = json->GetMotionDuration();
    _motionData->Loop = json->IsMotionLoop();
    _motionData->CurveCount = json->GetMotionCurveCount();
    _motionData->Fps = json->GetMotionFps();
    _motionData->EventCount = json->GetEventCount();

    csmBool areBeziersRestructed = json->GetEvaluationOptionFlag( EvaluationOptionFlag_AreBeziersRistricted );

    if (json->IsExistMotionFadeInTime())
    {
        _fadeInSeconds = (json->GetMotionFadeInTime() < 0.0f)
                             ? 1.0f
                             : json->GetMotionFadeInTime();
    }
    else
    {
        _fadeInSeconds = 1.0f;
    }

    if (json->IsExistMotionFadeOutTime())
    {
        _fadeOutSeconds = (json->GetMotionFadeOutTime() < 0.0f)
                              ? 1.0f
                              : json->GetMotionFadeOutTime();
    }
    else
    {
        _fadeOutSeconds = 1.0f;
    }

    _motionData->Curves.UpdateSize(_motionData->CurveCount, CubismMotionCurve(), true);
    _motionData->Segments.UpdateSize(json->GetMotionTotalSegmentCount(), CubismMotionSegment(), true);
    _motionData->Points.UpdateSize(json->GetMotionTotalPointCount(), CubismMotionPoint(), true);
    _motionData->Events.UpdateSize(_motionData->EventCount, CubismMotionEvent(), true);

    csmInt32 totalPointCount = 0;
    csmInt32 totalSegmentCount = 0;

    // Curves
    for (csmInt32 curveCount = 0; curveCount < _motionData->CurveCount; ++curveCount)
    {
        if (strcmp(json->GetMotionCurveTarget(curveCount), TargetNameModel) == 0)
        {
            _motionData->Curves[curveCount].Type = CubismMotionCurveTarget_Model;
        }
        else if (strcmp(json->GetMotionCurveTarget(curveCount), TargetNameParameter) == 0)
        {
            _motionData->Curves[curveCount].Type = CubismMotionCurveTarget_Parameter;
        }
        else if (strcmp(json->GetMotionCurveTarget(curveCount), TargetNamePartOpacity) == 0)
        {
            _motionData->Curves[curveCount].Type = CubismMotionCurveTarget_PartOpacity;
        }
        else
        {
            CubismLogWarning("Warning : Unable to get segment type from Curve! The number of \"CurveCount\" may be incorrect!");
        }

        _motionData->Curves[curveCount].Id = json->GetMotionCurveId(curveCount);

        _motionData->Curves[curveCount].BaseSegmentIndex = totalSegmentCount;

        _motionData->Curves[curveCount].FadeInTime =
                (json->IsExistMotionCurveFadeInTime(curveCount))
                    ? json->GetMotionCurveFadeInTime(curveCount)
                    : -1.0f ;
        _motionData->Curves[curveCount].FadeOutTime =
                (json->IsExistMotionCurveFadeOutTime(curveCount))
                    ? json->GetMotionCurveFadeOutTime(curveCount)
                    : -1.0f;

        // Segments
        for (csmInt32 segmentPosition = 0; segmentPosition < json->GetMotionCurveSegmentCount(curveCount);)
        {
            if (segmentPosition == 0)
            {
                _motionData->Segments[totalSegmentCount].BasePointIndex = totalPointCount;

                _motionData->Points[totalPointCount].Time = json->GetMotionCurveSegment(curveCount, segmentPosition);
                _motionData->Points[totalPointCount].Value = json->GetMotionCurveSegment(curveCount, segmentPosition + 1);

                totalPointCount += 1;
                segmentPosition += 2;
            }
            else
            {
                _motionData->Segments[totalSegmentCount].BasePointIndex = totalPointCount - 1;
            }

            const csmInt32 segment = static_cast<csmInt32>(json->GetMotionCurveSegment(curveCount, segmentPosition));

            switch (segment)
            {
            case CubismMotionSegmentType_Linear: {
                _motionData->Segments[totalSegmentCount].SegmentType = CubismMotionSegmentType_Linear;
                _motionData->Segments[totalSegmentCount].Evaluate = LinearEvaluate;

                _motionData->Points[totalPointCount].Time = json->GetMotionCurveSegment(curveCount, (segmentPosition + 1));
                _motionData->Points[totalPointCount].Value = json->GetMotionCurveSegment(curveCount, (segmentPosition + 2));

                totalPointCount += 1;
                segmentPosition += 3;

                break;
            }
            case CubismMotionSegmentType_Bezier: {
                _motionData->Segments[totalSegmentCount].SegmentType = CubismMotionSegmentType_Bezier;
                if (areBeziersRestructed || UseOldBeziersCurveMotion) {
                    _motionData->Segments[totalSegmentCount].Evaluate = BezierEvaluate;
                }
                else
                {
                    _motionData->Segments[totalSegmentCount].Evaluate = BezierEvaluateCardanoInterpretation;
                }

                _motionData->Points[totalPointCount].Time = json->GetMotionCurveSegment(curveCount, (segmentPosition + 1));
                _motionData->Points[totalPointCount].Value = json->GetMotionCurveSegment(curveCount, (segmentPosition + 2));

                _motionData->Points[totalPointCount + 1].Time = json->GetMotionCurveSegment(curveCount, (segmentPosition + 3));
                _motionData->Points[totalPointCount + 1].Value = json->GetMotionCurveSegment(curveCount, (segmentPosition + 4));

                _motionData->Points[totalPointCount + 2].Time = json->GetMotionCurveSegment(curveCount, (segmentPosition + 5));
                _motionData->Points[totalPointCount + 2].Value = json->GetMotionCurveSegment(curveCount, (segmentPosition + 6));

                totalPointCount += 3;
                segmentPosition += 7;

                break;
            }
            case CubismMotionSegmentType_Stepped: {
                _motionData->Segments[totalSegmentCount].SegmentType = CubismMotionSegmentType_Stepped;
                _motionData->Segments[totalSegmentCount].Evaluate = SteppedEvaluate;

                _motionData->Points[totalPointCount].Time = json->GetMotionCurveSegment(curveCount, (segmentPosition + 1));
                _motionData->Points[totalPointCount].Value = json->GetMotionCurveSegment(curveCount, (segmentPosition + 2));

                totalPointCount += 1;
                segmentPosition += 3;

                break;
            }
            case CubismMotionSegmentType_InverseStepped: {
                _motionData->Segments[totalSegmentCount].SegmentType = CubismMotionSegmentType_InverseStepped;
                _motionData->Segments[totalSegmentCount].Evaluate = InverseSteppedEvaluate;

                _motionData->Points[totalPointCount].Time = json->GetMotionCurveSegment(curveCount, (segmentPosition + 1));
                _motionData->Points[totalPointCount].Value = json->GetMotionCurveSegment(curveCount, (segmentPosition + 2));

                totalPointCount += 1;
                segmentPosition += 3;

                break;
            }
            default: {
                CSM_ASSERT(0);
                break;
            }
            }

            ++_motionData->Curves[curveCount].SegmentCount;
            ++totalSegmentCount;
        }
    }


    for (csmInt32 userdatacount = 0; userdatacount < json->GetEventCount(); ++userdatacount)
    {
        _motionData->Events[userdatacount].FireTime = json->GetEventTime(userdatacount);
        _motionData->Events[userdatacount].Value = json->GetEventValue(userdatacount);
    }

    CSM_DELETE(json);
}

void CubismMotion::SetParameterFadeInTime(CubismIdHandle parameterId, csmFloat32 value)
{
    csmVector<CubismMotionCurve>& curves = _motionData->Curves;

    for (csmInt16 i = 0; i < _motionData->CurveCount; ++i)
    {
        if (parameterId == curves[i].Id)
        {
            curves[i].FadeInTime = value;
            return;
        }
    }
}

void CubismMotion::SetParameterFadeOutTime(CubismIdHandle parameterId, csmFloat32 value)
{
    csmVector<CubismMotionCurve>& curves = _motionData->Curves;

    for (csmInt16 i = 0; i < _motionData->CurveCount; ++i)
    {
        if (parameterId == curves[i].Id)
        {
            curves[i].FadeOutTime = value;
            return;
        }
    }
}

csmFloat32 CubismMotion::GetParameterFadeInTime(CubismIdHandle parameterId) const
{
    csmVector<CubismMotionCurve>& curves = _motionData->Curves;

    for (csmInt16 i = 0; i < _motionData->CurveCount; ++i)
    {
        if (parameterId == curves[i].Id)
        {
            return curves[i].FadeInTime;
        }
    }

    return -1;
}

csmFloat32 CubismMotion::GetParameterFadeOutTime(CubismIdHandle parameterId) const
{
    csmVector<CubismMotionCurve>& curves = _motionData->Curves;

    for (csmInt16 i = 0; i < _motionData->CurveCount; ++i)
    {
        if (parameterId == curves[i].Id)
        {
            return curves[i].FadeOutTime;
        }
    }

    return -1;
}

void CubismMotion::IsLoop(csmBool loop)
{
    this->_isLoop = loop;
}

csmBool CubismMotion::IsLoop() const
{
    return this->_isLoop;
}

void CubismMotion::IsLoopFadeIn(csmBool loopFadeIn)
{
    this->_isLoopFadeIn = loopFadeIn;
}

csmBool CubismMotion::IsLoopFadeIn() const
{
    return this->_isLoopFadeIn;
}

csmFloat32 CubismMotion::GetLoopDuration()
{
    return _loopDurationSeconds;
}

void CubismMotion::SetEffectIds(const csmVector<CubismIdHandle>& eyeBlinkParameterIds, const csmVector<CubismIdHandle>& lipSyncParameterIds)
{
    _eyeBlinkParameterIds = eyeBlinkParameterIds;
    _lipSyncParameterIds = lipSyncParameterIds;
}

const csmVector<const csmString*>& CubismMotion::GetFiredEvent(csmFloat32 beforeCheckTimeSeconds, csmFloat32 motionTimeSeconds)
{
    _firedEventValues.UpdateSize(0);
    /// イベントの発火チェック
    for (csmInt32 u = 0; u < _motionData->EventCount; ++u)
    {
        if ((_motionData->Events[u].FireTime >beforeCheckTimeSeconds) &&
            (_motionData->Events[u].FireTime <= motionTimeSeconds))
        {
            _firedEventValues.PushBack(&_motionData->Events[u].Value);
        }
    }

    return _firedEventValues;
}

csmBool CubismMotion::IsExistOpacity() const
{
    for (csmInt32 i = 0; i < _motionData->CurveCount; i++)
    {
        CubismMotionCurve curve = _motionData->Curves[i];

        if (curve.Type != CubismMotionCurveTarget_Model)
        {
            continue;
        }

        if (strcmp(curve.Id->GetString().GetRawString(), IdNameOpacity) == 0)
        {
            return true;
        }
    }

    return false;
}

csmInt32 CubismMotion::GetOpacityIndex() const
{
    if (IsExistOpacity())
    {
        for (csmInt32 i = 0; i < _motionData->CurveCount; i++)
        {
            CubismMotionCurve curve = _motionData->Curves[i];

            if (curve.Type != CubismMotionCurveTarget_Model)
            {
                continue;
            }

            if (strcmp(curve.Id->GetString().GetRawString(), IdNameOpacity) == 0)
            {
                return i;
            }
        }
    }

    return -1;
}

CubismIdHandle CubismMotion::GetOpacityId(csmInt32 index)
{
    if (index != -1)
    {
        CubismMotionCurve curve = _motionData->Curves[index];

        if (curve.Type == CubismMotionCurveTarget_Model)
        {
            if (strcmp(curve.Id->GetString().GetRawString(), IdNameOpacity) == 0)
            {
                return CubismFramework::GetIdManager()->GetId(curve.Id->GetString().GetRawString());
            }
        }
    }

    return NULL;
}

csmFloat32 CubismMotion::GetOpacityValue(csmFloat32 motionTimeSeconds) const
{
    if (motionTimeSeconds >= 0.0f)
    {
        csmInt32 index = GetOpacityIndex();

        if (index != -1)
        {
            csmInt32 baseSegmentIndex = _motionData->Curves[index].BaseSegmentIndex;
            csmInt32 basePointIndex = _motionData->Segments[baseSegmentIndex].BasePointIndex;

            csmFloat32 fps = 1.0f / _motionData->Fps;

            csmFloat32 pointTime = -1.0f;
            csmInt32 segmentPosition = 0;
            CubismMotionSegmentType segmentType = static_cast<CubismMotionSegmentType>(_motionData->Segments[index + segmentPosition].SegmentType);

            for (segmentPosition = 0; segmentPosition < _motionData->Curves[index].SegmentCount; segmentPosition++)
            {
                if (segmentPosition == 0)
                {
                    if (motionTimeSeconds == 0.0f)
                    {
                        return LinearEvaluate(&_motionData->Points[basePointIndex], motionTimeSeconds);
                    }

                    segmentPosition += 2;
                }

                segmentType = static_cast<CubismMotionSegmentType>(_motionData->Segments[index + segmentPosition].SegmentType);
                basePointIndex = _motionData->Segments[index + segmentPosition].BasePointIndex;

                pointTime = _motionData->Points[basePointIndex].Time;

                switch (segmentType)
                {
                    case Live2D::Cubism::Framework::CubismMotionSegmentType_Linear:
                        segmentPosition += 3;
                        break;
                    case Live2D::Cubism::Framework::CubismMotionSegmentType_Bezier:
                        basePointIndex += 3;
                        segmentPosition += 7;
                        break;
                    case Live2D::Cubism::Framework::CubismMotionSegmentType_Stepped:
                        segmentPosition += 3;
                        break;
                    case Live2D::Cubism::Framework::CubismMotionSegmentType_InverseStepped:
                        segmentPosition += 3;
                        break;
                    default:
                        CSM_ASSERT(0);
                        break;
                }


                if (pointTime + fps < motionTimeSeconds)
                {
                    continue;
                }

                switch (segmentType)
                {
                    case Live2D::Cubism::Framework::CubismMotionSegmentType_Linear:
                        return LinearEvaluate(&_motionData->Points[basePointIndex], motionTimeSeconds);

                    case Live2D::Cubism::Framework::CubismMotionSegmentType_Bezier:
                        return BezierEvaluateCardanoInterpretation(&_motionData->Points[basePointIndex - 3], motionTimeSeconds);

                    case Live2D::Cubism::Framework::CubismMotionSegmentType_Stepped:
                        return SteppedEvaluate(&_motionData->Points[basePointIndex], motionTimeSeconds);

                    case Live2D::Cubism::Framework::CubismMotionSegmentType_InverseStepped:
                        return InverseSteppedEvaluate(&_motionData->Points[basePointIndex], motionTimeSeconds);

                    default:
                        CSM_ASSERT(0);
                        break;
                }
            }
        }
    }

    return 1.0f;
}

}}}
