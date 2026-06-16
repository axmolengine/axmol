/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismPhysicsJson.hpp"
#include "Id/CubismIdManager.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/// Physics Json Constants
namespace {
// JSON keys
const csmChar* Position = "Position";
const csmChar* X = "X";
const csmChar* Y = "Y";
const csmChar* Angle = "Angle";
const csmChar* Type = "Type";
const csmChar* Id = "Id";

// Meta
const csmChar* Meta = "Meta";
const csmChar* EffectiveForces = "EffectiveForces";
const csmChar* TotalInputCount = "TotalInputCount";
const csmChar* TotalOutputCount = "TotalOutputCount";
const csmChar* PhysicsSettingCount = "PhysicsSettingCount";
const csmChar* Gravity = "Gravity";
const csmChar* Wind = "Wind";
const csmChar* VertexCount = "VertexCount";
const csmChar* Fps = "Fps";

// PhysicsSettings
const csmChar* PhysicsSettings = "PhysicsSettings";
const csmChar* Normalization = "Normalization";
const csmChar* Minimum = "Minimum";
const csmChar* Maximum = "Maximum";
const csmChar* Default = "Default";
const csmChar* Reflect = "Reflect";
const csmChar* Weight = "Weight";

// Input
const csmChar* Input = "Input";
const csmChar* Source = "Source";

// Output
const csmChar* Output = "Output";
const csmChar* Scale = "Scale";
const csmChar* VertexIndex = "VertexIndex";
const csmChar* Destination = "Destination";

// Particle
const csmChar* Vertices = "Vertices";
const csmChar* Mobility = "Mobility";
const csmChar* Delay = "Delay";
const csmChar* Radius = "Radius";
const csmChar* Acceleration = "Acceleration";
}

CubismPhysicsJson::CubismPhysicsJson(const csmByte* buffer, csmSizeInt size)
{
    CreateCubismJson(buffer, size);
}

CubismPhysicsJson::~CubismPhysicsJson()
{
    DeleteCubismJson();
}

CubismVector2 CubismPhysicsJson::GetGravity() const
{
    CubismVector2 ret;
    ret.X = _json->GetRoot()[Meta][EffectiveForces][Gravity][X].ToFloat();
    ret.Y = _json->GetRoot()[Meta][EffectiveForces][Gravity][Y].ToFloat();
    return ret;
}

CubismVector2 CubismPhysicsJson::GetWind() const
{
    CubismVector2 ret;
    ret.X = _json->GetRoot()[Meta][EffectiveForces][Wind][X].ToFloat();
    ret.Y = _json->GetRoot()[Meta][EffectiveForces][Wind][Y].ToFloat();
    return ret;
}

csmFloat32 CubismPhysicsJson::GetFps() const
{
    // if FPS information does not exist in physics3.json, 0.0f is returned.
    return _json->GetRoot()[Meta][Fps].ToFloat(0.0f);
}

csmInt32 CubismPhysicsJson::GetSubRigCount() const
{
    return _json->GetRoot()[Meta][PhysicsSettingCount].ToInt();
}

csmInt32 CubismPhysicsJson::GetTotalInputCount() const
{
    return _json->GetRoot()[Meta][TotalInputCount].ToInt();
}

csmInt32 CubismPhysicsJson::GetTotalOutputCount() const
{
    return _json->GetRoot()[Meta][TotalOutputCount].ToInt();
}

csmInt32 CubismPhysicsJson::GetVertexCount() const
{
    return _json->GetRoot()[Meta][VertexCount].ToInt();
}

// Input
csmFloat32 CubismPhysicsJson::GetNormalizationPositionMinimumValue(csmInt32 physicsSettingIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Normalization][Position][Minimum].ToFloat();
}

csmFloat32 CubismPhysicsJson::GetNormalizationPositionMaximumValue(csmInt32 physicsSettingIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Normalization][Position][Maximum].ToFloat();
}

csmFloat32 CubismPhysicsJson::GetNormalizationPositionDefaultValue(csmInt32 physicsSettingIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Normalization][Position][Default].ToFloat();
}

csmFloat32 CubismPhysicsJson::GetNormalizationAngleMinimumValue(csmInt32 physicsSettingIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Normalization][Angle][Minimum].ToFloat();
}

csmFloat32 CubismPhysicsJson::GetNormalizationAngleMaximumValue(csmInt32 physicsSettingIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Normalization][Angle][Maximum].ToFloat();
}

csmFloat32 CubismPhysicsJson::GetNormalizationAngleDefaultValue(csmInt32 physicsSettingIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Normalization][Angle][Default].ToFloat();
}

csmInt32 CubismPhysicsJson::GetInputCount(csmInt32 physicsSettingIndex) const
{
    return static_cast<csmInt32>(_json->GetRoot()[PhysicsSettings][physicsSettingIndex][Input].GetVector()->GetSize());
}

csmFloat32 CubismPhysicsJson::GetInputWeight(csmInt32 physicsSettingIndex, csmInt32 inputIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Input][inputIndex][Weight].ToFloat();
}

csmBool CubismPhysicsJson::GetInputReflect(csmInt32 physicsSettingIndex, csmInt32 inputIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Input][inputIndex][Reflect].ToBoolean();
}

const csmChar* CubismPhysicsJson::GetInputType(csmInt32 physicsSettingIndex, csmInt32 inputIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Input][inputIndex][Type].GetRawString();
}

CubismIdHandle CubismPhysicsJson::GetInputSourceId(csmInt32 physicsSettingIndex, csmInt32 inputIndex) const
{
    return CubismFramework::GetIdManager()->GetId(_json->GetRoot()[PhysicsSettings][physicsSettingIndex][Input][inputIndex][Source][Id].GetRawString());
}

// Output
csmInt32 CubismPhysicsJson::GetOutputCount(csmInt32 physicsSettingIndex) const
{
    return static_cast<csmInt32>(_json->GetRoot()[PhysicsSettings][physicsSettingIndex][Output].GetVector()->GetSize());
}

csmInt32 CubismPhysicsJson::GetOutputVertexIndex(csmInt32 physicsSettingIndex, csmInt32 outputIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Output][outputIndex][VertexIndex].ToInt();
}

csmFloat32 CubismPhysicsJson::GetOutputAngleScale(csmInt32 physicsSettingIndex, csmInt32 outputIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Output][outputIndex][Scale].ToFloat();
}

csmFloat32 CubismPhysicsJson::GetOutputWeight(csmInt32 physicsSettingIndex, csmInt32 outputIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Output][outputIndex][Weight].ToFloat();
}

CubismIdHandle CubismPhysicsJson::GetOutputsDestinationId(csmInt32 physicsSettingIndex, csmInt32 outputIndex) const
{
    return CubismFramework::GetIdManager()->GetId(_json->GetRoot()[PhysicsSettings][physicsSettingIndex][Output][outputIndex][Destination][Id].GetRawString());
}

const csmChar* CubismPhysicsJson::GetOutputType(csmInt32 physicsSettingIndex, csmInt32 outputIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Output][outputIndex][Type].GetRawString();
}

csmBool CubismPhysicsJson::GetOutputReflect(csmInt32 physicsSettingIndex, csmInt32 outputIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Output][outputIndex][Reflect].ToBoolean();
}

// Particle
csmInt32 CubismPhysicsJson::GetParticleCount(csmInt32 physicsSettingIndex) const
{
    return static_cast<csmInt32>(_json->GetRoot()[PhysicsSettings][physicsSettingIndex][Vertices].GetVector()->GetSize());
}

csmFloat32 CubismPhysicsJson::GetParticleMobility(csmInt32 physicsSettingIndex, csmInt32 vertexIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Vertices][vertexIndex][Mobility].ToFloat();
}

csmFloat32 CubismPhysicsJson::GetParticleDelay(csmInt32 physicsSettingIndex, csmInt32 vertexIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Vertices][vertexIndex][Delay].ToFloat();
}

csmFloat32 CubismPhysicsJson::GetParticleAcceleration(csmInt32 physicsSettingIndex, csmInt32 vertexIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Vertices][vertexIndex][Acceleration].ToFloat();
}

csmFloat32 CubismPhysicsJson::GetParticleRadius(csmInt32 physicsSettingIndex, csmInt32 vertexIndex) const
{
    return _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Vertices][vertexIndex][Radius].ToFloat();
}

CubismVector2 CubismPhysicsJson::GetParticlePosition(csmInt32 physicsSettingIndex, csmInt32 vertexIndex) const
{
    return CubismVector2(_json->GetRoot()[PhysicsSettings][physicsSettingIndex][Vertices][vertexIndex][Position][X].ToFloat(),
        _json->GetRoot()[PhysicsSettings][physicsSettingIndex][Vertices][vertexIndex][Position][Y].ToFloat() );
}
}}}
