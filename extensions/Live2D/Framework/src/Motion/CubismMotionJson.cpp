/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismMotionJson.hpp"
#include "Id/CubismId.hpp"
#include "Id/CubismIdManager.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

namespace {
// JSON keys
const csmChar* Meta = "Meta";
const csmChar* Duration = "Duration";
const csmChar* Loop = "Loop";
const csmChar* AreBeziersRestricted = "AreBeziersRestricted";
const csmChar* CurveCount = "CurveCount";
const csmChar* Fps = "Fps";
const csmChar* TotalSegmentCount = "TotalSegmentCount";
const csmChar* TotalPointCount = "TotalPointCount";
const csmChar* Curves = "Curves";
const csmChar* Target = "Target";
const csmChar* Id = "Id";
const csmChar* FadeInTime = "FadeInTime";
const csmChar* FadeOutTime = "FadeOutTime";
const csmChar* Segments = "Segments";
const csmChar* UserData = "UserData";
const csmChar* UserDataCount = "UserDataCount";
const csmChar* TotalUserDataSize = "TotalUserDataSize";
const csmChar* Time = "Time";
const csmChar* Value = "Value";
}

CubismMotionJson::CubismMotionJson(const csmByte* buffer, csmSizeInt size)
{
    CreateCubismJson(buffer, size);
}

CubismMotionJson::~CubismMotionJson()
{
    DeleteCubismJson();
}

csmFloat32 CubismMotionJson::GetMotionDuration() const
{
    return _json->GetRoot()[Meta][Duration].ToFloat();
}

csmBool CubismMotionJson::IsMotionLoop() const
{
    return _json->GetRoot()[Meta][Loop].ToBoolean();
}

csmBool CubismMotionJson::GetEvaluationOptionFlag(const csmInt32 flagType) const
{
    if (EvaluationOptionFlag_AreBeziersRistricted == flagType)
    {
        return _json->GetRoot()[Meta][AreBeziersRestricted].ToBoolean();
    }

    return false;
}

csmInt32 CubismMotionJson::GetMotionCurveCount() const
{
    return _json->GetRoot()[Meta][CurveCount].ToInt();
}

csmFloat32 CubismMotionJson::GetMotionFps() const
{
    return _json->GetRoot()[Meta][Fps].ToFloat();
}

csmInt32 CubismMotionJson::GetMotionTotalSegmentCount() const
{
    return _json->GetRoot()[Meta][TotalSegmentCount].ToInt();
}

csmInt32 CubismMotionJson::GetMotionTotalPointCount() const
{
    return _json->GetRoot()[Meta][TotalPointCount].ToInt();
}

csmBool CubismMotionJson::IsExistMotionFadeInTime() const
{
    return !_json->GetRoot()[Meta][FadeInTime].IsNull();
}

csmBool CubismMotionJson::IsExistMotionFadeOutTime() const
{
    return !_json->GetRoot()[Meta][FadeOutTime].IsNull();
}

csmFloat32 CubismMotionJson::GetMotionFadeInTime() const
{
    return _json->GetRoot()[Meta][FadeInTime].ToFloat();
}

csmFloat32 CubismMotionJson::GetMotionFadeOutTime() const
{
    return _json->GetRoot()[Meta][FadeOutTime].ToFloat();
}

const csmChar* CubismMotionJson::GetMotionCurveTarget(csmInt32 curveIndex) const
{
    return _json->GetRoot()[Curves][curveIndex][Target].GetRawString();
}

CubismIdHandle CubismMotionJson::GetMotionCurveId(csmInt32 curveIndex) const
{
    return CubismFramework::GetIdManager()->GetId(_json->GetRoot()[Curves][curveIndex][Id].GetRawString());
}

csmBool CubismMotionJson::IsExistMotionCurveFadeInTime(csmInt32 curveIndex) const
{
    return !_json->GetRoot()[Curves][curveIndex][FadeInTime].IsNull();
}

csmBool CubismMotionJson::IsExistMotionCurveFadeOutTime(csmInt32 curveIndex) const
{
    return !_json->GetRoot()[Curves][curveIndex][FadeOutTime].IsNull();
}

csmFloat32 CubismMotionJson::GetMotionCurveFadeInTime(csmInt32 curveIndex) const
{
    return _json->GetRoot()[Curves][curveIndex][FadeInTime].ToFloat();
}

csmFloat32 CubismMotionJson::GetMotionCurveFadeOutTime(csmInt32 curveIndex) const
{
    return _json->GetRoot()[Curves][curveIndex][FadeOutTime].ToFloat();
}

csmInt32 CubismMotionJson::GetMotionCurveSegmentCount(csmInt32 curveIndex) const
{
    return static_cast<csmInt32>(_json->GetRoot()[Curves][curveIndex][Segments].GetVector()->GetSize());
}

csmFloat32 CubismMotionJson::GetMotionCurveSegment(csmInt32 curveIndex, csmInt32 segmentIndex) const
{
    return _json->GetRoot()[Curves][curveIndex][Segments][segmentIndex].ToFloat();
}

csmInt32 CubismMotionJson::GetEventCount() const
{
    return _json->GetRoot()[Meta][UserDataCount].ToInt();
}

csmInt32 CubismMotionJson::GetTotalEventValueSize() const
{
    return _json->GetRoot()[Meta][TotalUserDataSize].ToInt();
}

csmFloat32 CubismMotionJson::GetEventTime(csmInt32 userDataIndex) const
{
    return _json->GetRoot()[UserData][userDataIndex][Time].ToFloat();
}

const csmChar* CubismMotionJson::GetEventValue(csmInt32 userDataIndex) const
{
    return _json->GetRoot()[UserData][userDataIndex][Value].GetRawString();
}

}}}
