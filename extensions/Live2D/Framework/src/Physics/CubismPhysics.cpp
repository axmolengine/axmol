/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismPhysics.hpp"
#include "CubismPhysicsInternal.hpp"
#include "CubismPhysicsJson.hpp"
#include "Model/CubismModel.hpp"
#include "Utils/CubismString.hpp"
#include "Math/CubismMath.hpp"
#include "Math/CubismVector2.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/// physics constants
namespace {

/// physics types tags.
const csmChar* PhysicsTypeTagX = "X";
const csmChar* PhysicsTypeTagY = "Y";
const csmChar* PhysicsTypeTagAngle = "Angle";

/// Constant of air resistance.
const csmFloat32 AirResistance = 5.0f;

/// Constant of maximum weight of input and output ratio.
const csmFloat32 MaximumWeight = 100.0f;

/// Constant of threshold of movement.
const csmFloat32 MovementThreshold = 0.001f;

/// Constant of maximum allowed delta time
const csmFloat32 MaxDeltaTime = 5.0f;

csmFloat32 GetRangeValue(csmFloat32 min, csmFloat32 max)
{
    csmFloat32 maxValue = CubismMath::Max(min, max);
    csmFloat32 minValue = CubismMath::Min(min, max);

    return CubismMath::AbsF(maxValue - minValue);
}

/// Gets sign.
///
/// @param  value  Evaluation target value.
///
/// @return  Sign of value.
csmInt32 Sign(csmFloat32 value)
{
    csmInt32 ret = 0;

    if (value > 0.0f)
    {
        ret = 1;
    }
    else if (value < 0.0f)
    {
        ret = -1;
    }

    return ret;
}

csmFloat32 GetDefaultValue(csmFloat32 min, csmFloat32 max)
{
    const csmFloat32 minValue = CubismMath::Min(min, max);
    return minValue + (GetRangeValue(min, max) / 2.0f);
}

csmFloat32 NormalizeParameterValue(
    csmFloat32 value,
    csmFloat32 parameterMinimum,
    csmFloat32 parameterMaximum,
    csmFloat32 parameterDefault,
    csmFloat32 normalizedMinimum,
    csmFloat32 normalizedMaximum,
    csmFloat32 normalizedDefault,
    csmInt32 isInverted)
{
    csmFloat32 result = 0.0f;

    const csmFloat32 maxValue = CubismMath::Max(parameterMaximum, parameterMinimum);

    if (maxValue < value)
    {
        value = maxValue;
    }

    const csmFloat32 minValue = CubismMath::Min(parameterMaximum, parameterMinimum);

    if (minValue > value)
    {
        value = minValue;
    }

    const csmFloat32 minNormValue = CubismMath::Min(normalizedMinimum, normalizedMaximum);
    const csmFloat32 maxNormValue = CubismMath::Max(normalizedMinimum, normalizedMaximum);
    const csmFloat32 middleNormValue = normalizedDefault;

    const csmFloat32 middleValue = GetDefaultValue(minValue, maxValue);
    const csmFloat32 paramValue = value - middleValue;

    switch (Sign(paramValue))
    {
    case 1: {
        const csmFloat32 nLength = maxNormValue - middleNormValue;
        const csmFloat32 pLength = maxValue - middleValue;
        if (pLength != 0.0f)
        {
            result = paramValue * (nLength / pLength);
            result += middleNormValue;
        }

        break;
    }
    case -1: {
        const csmFloat32 nLength = minNormValue - middleNormValue;
        const csmFloat32 pLength = minValue - middleValue;
        if (pLength != 0.0f)
        {
            result = paramValue * (nLength / pLength);
            result += middleNormValue;
        }

        break;
    }
    case 0: {
        result = middleNormValue;

        break;
    }
    default: {
        break;
    }
    }

    return (isInverted)
        ? result
        : (result * -1.0f);
}

void GetInputTranslationXFromNormalizedParameterValue(CubismVector2* targetTranslation, csmFloat32* targetAngle, csmFloat32 value,
    csmFloat32 parameterMinimumValue, csmFloat32 parameterMaximumValue,
    csmFloat32 parameterDefaultValue,
    CubismPhysicsNormalization* normalizationPosition,
    CubismPhysicsNormalization* normalizationAngle, csmInt32 isInverted,
    csmFloat32 weight)
{
    targetTranslation->X += NormalizeParameterValue(
        value,
        parameterMinimumValue,
        parameterMaximumValue,
        parameterDefaultValue,
        normalizationPosition->Minimum,
        normalizationPosition->Maximum,
        normalizationPosition->Default,
        isInverted
    ) * weight;
}

void GetInputTranslationYFromNormalizedParameterValue(CubismVector2* targetTranslation, csmFloat32* targetAngle, csmFloat32 value,
    csmFloat32 parameterMinimumValue, csmFloat32 parameterMaximumValue,
    csmFloat32 parameterDefaultValue,
    CubismPhysicsNormalization* normalizationPosition,
    CubismPhysicsNormalization* normalizationAngle,
    csmInt32 isInverted, csmFloat32 weight)
{
    targetTranslation->Y += NormalizeParameterValue(
        value,
        parameterMinimumValue,
        parameterMaximumValue,
        parameterDefaultValue,
        normalizationPosition->Minimum,
        normalizationPosition->Maximum,
        normalizationPosition->Default,
        isInverted
    ) * weight;
}

void GetInputAngleFromNormalizedParameterValue(CubismVector2* targetTranslation, csmFloat32* targetAngle, csmFloat32 value,
    csmFloat32 parameterMinimumValue, csmFloat32 parameterMaximumValue,
    csmFloat32 parameterDefaultValue,
    CubismPhysicsNormalization* normalizationPosition,
    CubismPhysicsNormalization* normalizationAngle,
    csmInt32 isInverted, csmFloat32 weight)
{
    *targetAngle += NormalizeParameterValue(
        value,
        parameterMinimumValue,
        parameterMaximumValue,
        parameterDefaultValue,
        normalizationAngle->Minimum,
        normalizationAngle->Maximum,
        normalizationAngle->Default,
        isInverted
    ) * weight;
}

csmFloat32 GetOutputTranslationX(CubismVector2 translation, CubismPhysicsParticle* particles, csmInt32 particleIndex,
    csmInt32 isInverted, CubismVector2 parentGravity)
{
    csmFloat32 outputValue = translation.X;

    if (isInverted)
    {
        outputValue *= -1.0f;
    }

    return outputValue;
}

csmFloat32 GetOutputTranslationY(CubismVector2 translation, CubismPhysicsParticle* particles, csmInt32 particleIndex,
    csmInt32 isInverted, CubismVector2 parentGravity)
{
    csmFloat32 outputValue = translation.Y;

    if (isInverted)
    {
        outputValue *= -1.0f;
    }

    return outputValue;
}

csmFloat32 GetOutputAngle(CubismVector2 translation, CubismPhysicsParticle* particles, csmInt32 particleIndex, csmInt32 isInverted,
    CubismVector2 parentGravity)
{
    csmFloat32 outputValue;

    if (particleIndex >= 2)
    {
        parentGravity = particles[particleIndex - 1].Position - particles[particleIndex - 2].Position;
    }
    else
    {
        parentGravity *= -1.0f;
    }

    outputValue = CubismMath::DirectionToRadian(parentGravity, translation);

    if (isInverted)
    {
        outputValue *= -1.0f;
    }

    return outputValue;
}

csmFloat32 GetOutputScaleTranslationX(CubismVector2 translationScale, csmFloat32 angleScale)
{
    return translationScale.X;
}

csmFloat32 GetOutputScaleTranslationY(CubismVector2 translationScale, csmFloat32 angleScale)
{
    return translationScale.Y;
}

csmFloat32 GetOutputScaleAngle(CubismVector2 translationScale, csmFloat32 angleScale)
{
    return angleScale;
}

/// Updates particles.
///
/// @param  strand            Target array of particle.
/// @param  strandCount       Count of particle.
/// @param  totalTranslation  Total translation value.
/// @param  totalAngle        Total angle.
/// @param  windDirection              Direction of wind.
/// @param  thresholdValue    Threshold of movement.
/// @param  deltaTimeSeconds  Delta time.
/// @param  airResistance     Air resistance.
void UpdateParticles(CubismPhysicsParticle* strand, csmInt32 strandCount, CubismVector2 totalTranslation, csmFloat32 totalAngle,
    CubismVector2 windDirection, csmFloat32 thresholdValue, csmFloat32 deltaTimeSeconds, csmFloat32 airResistance)
{
    csmInt32 i;
    csmFloat32 totalRadian;
    csmFloat32 delay;
    csmFloat32 radian;
    CubismVector2 currentGravity;
    CubismVector2 direction;
    CubismVector2 velocity;
    CubismVector2 force;
    CubismVector2 newDirection;

    strand[0].Position = totalTranslation;

    totalRadian = CubismMath::DegreesToRadian(totalAngle);
    currentGravity = CubismMath::RadianToDirection(totalRadian);
    currentGravity.Normalize();

    for (i = 1; i < strandCount; ++i)
    {
        strand[i].Force = (currentGravity * strand[i].Acceleration) + windDirection;

        strand[i].LastPosition = strand[i].Position;

        delay = strand[i].Delay * deltaTimeSeconds * 30.0f;

        direction.X = strand[i].Position.X - strand[i - 1].Position.X;
        direction.Y = strand[i].Position.Y - strand[i - 1].Position.Y;

        radian = CubismMath::DirectionToRadian(strand[i].LastGravity, currentGravity) / airResistance;

        direction.X = ((CubismMath::CosF(radian) * direction.X) - (direction.Y * CubismMath::SinF(radian)));
        direction.Y = ((CubismMath::SinF(radian) * direction.X) + (direction.Y * CubismMath::CosF(radian)));

        strand[i].Position = strand[i - 1].Position + direction;

        velocity.X = strand[i].Velocity.X * delay;
        velocity.Y = strand[i].Velocity.Y * delay;
        force = strand[i].Force * delay * delay;

        strand[i].Position = strand[i].Position + velocity + force;

        newDirection = strand[i].Position - strand[i - 1].Position;

        newDirection.Normalize();

        strand[i].Position = strand[i - 1].Position + (newDirection * strand[i].Radius);

        if (CubismMath::AbsF(strand[i].Position.X) < thresholdValue)
        {
            strand[i].Position.X = 0.0f;
        }

        if (delay != 0.0f)
        {
            strand[i].Velocity.X = strand[i].Position.X - strand[i].LastPosition.X;
            strand[i].Velocity.Y = strand[i].Position.Y - strand[i].LastPosition.Y;
            strand[i].Velocity /= delay;
            strand[i].Velocity *= strand[i].Mobility;
        }

        strand[i].Force = CubismVector2(0.0f, 0.0f);
        strand[i].LastGravity = currentGravity;
    }
}

/**
 * Updates particles for stabilization.
 *
 * @param strand                Target array of particle.
 * @param strandCount           Count of particle.
 * @param totalTranslation      Total translation value.
 * @param totalAngle            Total angle.
 * @param windDirection         Direction of Wind.
 * @param thresholdValue        Threshold of movement.
 */
void UpdateParticlesForStabilization(CubismPhysicsParticle* strand, csmInt32 strandCount, CubismVector2 totalTranslation, csmFloat32 totalAngle,
    CubismVector2 windDirection, csmFloat32 thresholdValue)
{
    csmInt32 i;
    csmFloat32 totalRadian;
    CubismVector2 currentGravity;
    CubismVector2 force;

    strand[0].Position = totalTranslation;

    totalRadian = CubismMath::DegreesToRadian(totalAngle);
    currentGravity = CubismMath::RadianToDirection(totalRadian);
    currentGravity.Normalize();

    for (i = 1; i < strandCount; ++i)
    {
        strand[i].Force = (currentGravity * strand[i].Acceleration) + windDirection;

        strand[i].LastPosition = strand[i].Position;

        strand[i].Velocity = CubismVector2(0.0, 0.0);

        force = strand[i].Force;
        force.Normalize();

        force *= strand[i].Radius;
        strand[i].Position = strand[i - 1].Position + force;

        if (CubismMath::AbsF(strand[i].Position.X) < thresholdValue)
        {
            strand[i].Position.X = 0.0f;
        }

        strand[i].Force = CubismVector2(0.0f, 0.0f);
        strand[i].LastGravity = currentGravity;
    }
}

/// Updates output parameter value.
///
/// @param  parameterValue         Target parameter value.
/// @param  parameterValueMinimum  Minimum of parameter value.
/// @param  parameterValueMaximum  Maximum of parameter value.
/// @param  translation            Translation value.
void UpdateOutputParameterValue(csmFloat32* parameterValue, csmFloat32 parameterValueMinimum, csmFloat32 parameterValueMaximum,
    csmFloat32 translation, CubismPhysicsOutput* output)
{
    csmFloat32 outputScale;
    csmFloat32 value;
    csmFloat32 weight;

    outputScale = output->GetScale(output->TranslationScale, output->AngleScale);

    value = translation * outputScale;

    if (value < parameterValueMinimum)
    {
        if (value < output->ValueBelowMinimum)
        {
            output->ValueBelowMinimum = value;
        }

        value = parameterValueMinimum;
    }
    else if (value > parameterValueMaximum)
    {
        if (value > output->ValueExceededMaximum)
        {
            output->ValueExceededMaximum = value;
        }

        value = parameterValueMaximum;
    }

    weight = (output->Weight / MaximumWeight);

    if (weight >= 1.0f)
    {
        *parameterValue = value;
    }
    else
    {
        value = (*parameterValue * (1.0f - weight)) + (value * weight);
        *parameterValue = value;
    }
}

}

CubismPhysics::CubismPhysics()
    : _physicsRig(NULL)
{
    // set default options.
    _options.Gravity.Y = -1.0f;
    _options.Gravity.X = 0;
    _options.Wind.X = 0;
    _options.Wind.Y = 0;
    _currentRemainTime = 0.0f;
}

CubismPhysics::~CubismPhysics()
{
    CSM_DELETE(_physicsRig);
    _parameterCaches.Clear();
    _parameterInputCaches.Clear();
}

/// Initializes physics.
///
/// @param  physics  Target rig.
void CubismPhysics::Initialize()
{
    CubismPhysicsParticle* strand;
    CubismPhysicsSubRig* currentSetting;
    csmInt32 i, settingIndex;
    CubismVector2 radius;

    for (settingIndex = 0; settingIndex < _physicsRig->SubRigCount; ++settingIndex)
    {
        currentSetting = &_physicsRig->Settings[settingIndex];
        strand = &_physicsRig->Particles[currentSetting->BaseParticleIndex];

        // Initialize the top of particle.
        strand[0].InitialPosition = CubismVector2(0.0f, 0.0f);
        strand[0].LastPosition = strand[0].InitialPosition;
        strand[0].LastGravity = CubismVector2(0.0f, -1.0f);
        strand[0].LastGravity.Y *= -1.0f;
        strand[0].Velocity = CubismVector2(0.0f, 0.0f);
        strand[0].Force = CubismVector2(0.0f, 0.0f);

        // Initialize particles.
        for (i = 1; i < currentSetting->ParticleCount; ++i)
        {
            radius = CubismVector2(0.0f, 0.0f);
            radius.Y = strand[i].Radius;
            strand[i].InitialPosition = strand[i - 1].InitialPosition + radius;
            strand[i].Position = strand[i].InitialPosition;
            strand[i].LastPosition = strand[i].InitialPosition;
            strand[i].LastGravity = CubismVector2(0.0f, -1.0f);
            strand[i].LastGravity.Y *= -1.0f;
            strand[i].Velocity = CubismVector2(0.0f, 0.0f);
            strand[i].Force = CubismVector2(0.0f, 0.0f);
        }
    }
}

/// Reset the physics states.
void CubismPhysics::Reset()
{
    // set default options.
    _options.Gravity.Y = -1.0f;
    _options.Gravity.X = 0.0f;
    _options.Wind.X = 0.0f;
    _options.Wind.Y = 0.0f;

    _physicsRig->Gravity.X = 0.0f;
    _physicsRig->Gravity.Y = 0.0f;
    _physicsRig->Wind.X = 0.0f;
    _physicsRig->Wind.Y = 0.0f;

    Initialize();
}

CubismPhysics* CubismPhysics::Create(const csmByte* buffer, csmSizeInt size)
{
    CubismPhysics* ret = CSM_NEW CubismPhysics();

    ret->Parse(buffer, size);

    if (!ret->_isJsonValid)
    {
        Delete(ret);
        return NULL;
    }

    ret->_physicsRig->Gravity.Y = 0;
    return ret;
}

void CubismPhysics::Delete(CubismPhysics* physics)
{
    CSM_DELETE_SELF(CubismPhysics, physics);
}

void CubismPhysics::Parse(const csmByte* physicsJson, csmSizeInt size)
{
    _physicsRig = CSM_NEW CubismPhysicsRig;

    CubismPhysicsJson* json = CSM_NEW CubismPhysicsJson(physicsJson, size);

    _isJsonValid = json->IsValid();

    if (!_isJsonValid)
    {
        CSM_DELETE(json);
        return;
    }

    _physicsRig->Gravity = json->GetGravity();
    _physicsRig->Wind = json->GetWind();
    _physicsRig->SubRigCount = json->GetSubRigCount();

    _physicsRig->Fps = json->GetFps();

    _physicsRig->Settings.UpdateSize(_physicsRig->SubRigCount, CubismPhysicsSubRig(), true);
    _physicsRig->Inputs.UpdateSize(json->GetTotalInputCount(), CubismPhysicsInput(), true);
    _physicsRig->Outputs.UpdateSize(json->GetTotalOutputCount(), CubismPhysicsOutput(), true);
    _physicsRig->Particles.UpdateSize(json->GetVertexCount(), CubismPhysicsParticle(), true);

    _currentRigOutputs.Clear();
    _previousRigOutputs.Clear();

    csmInt32 inputIndex = 0, outputIndex = 0, particleIndex = 0;
    for (csmUint32 i = 0; i < _physicsRig->Settings.GetSize(); ++i)
    {
        _physicsRig->Settings[i].NormalizationPosition.Minimum = json->GetNormalizationPositionMinimumValue(i);
        _physicsRig->Settings[i].NormalizationPosition.Maximum = json->GetNormalizationPositionMaximumValue(i);
        _physicsRig->Settings[i].NormalizationPosition.Default = json->GetNormalizationPositionDefaultValue(i);

        _physicsRig->Settings[i].NormalizationAngle.Minimum = json->GetNormalizationAngleMinimumValue(i);
        _physicsRig->Settings[i].NormalizationAngle.Maximum = json->GetNormalizationAngleMaximumValue(i);
        _physicsRig->Settings[i].NormalizationAngle.Default = json->GetNormalizationAngleDefaultValue(i);

        // Input
        _physicsRig->Settings[i].InputCount = json->GetInputCount(i);
        _physicsRig->Settings[i].BaseInputIndex = inputIndex;
        for (csmInt32 j = 0; j < _physicsRig->Settings[i].InputCount; ++j)
        {
            _physicsRig->Inputs[inputIndex + j].SourceParameterIndex = -1;
            _physicsRig->Inputs[inputIndex + j].Weight = json->GetInputWeight(i, j);
            _physicsRig->Inputs[inputIndex + j].Reflect = json->GetInputReflect(i, j);

            if (strcmp(json->GetInputType(i, j), PhysicsTypeTagX) == 0)
            {
                _physicsRig->Inputs[inputIndex + j].Type = CubismPhysicsSource_X;
                _physicsRig->Inputs[inputIndex + j].GetNormalizedParameterValue = GetInputTranslationXFromNormalizedParameterValue;
            }
            else if (strcmp(json->GetInputType(i, j), PhysicsTypeTagY) == 0)
            {
                _physicsRig->Inputs[inputIndex + j].Type = CubismPhysicsSource_Y;
                _physicsRig->Inputs[inputIndex + j].GetNormalizedParameterValue = GetInputTranslationYFromNormalizedParameterValue;
            }
            else if (strcmp(json->GetInputType(i, j), PhysicsTypeTagAngle) == 0)
            {
                _physicsRig->Inputs[inputIndex + j].Type = CubismPhysicsSource_Angle;
                _physicsRig->Inputs[inputIndex + j].GetNormalizedParameterValue = GetInputAngleFromNormalizedParameterValue;
            }

            _physicsRig->Inputs[inputIndex + j].Source.TargetType = CubismPhysicsTargetType_Parameter;
            _physicsRig->Inputs[inputIndex + j].Source.Id = json->GetInputSourceId(i, j);
        }
        inputIndex += _physicsRig->Settings[i].InputCount;

        // Output
        _physicsRig->Settings[i].OutputCount = json->GetOutputCount(i);
        _physicsRig->Settings[i].BaseOutputIndex = outputIndex;

        PhysicsOutput currentRigOutput;
        currentRigOutput.outputs.Resize(_physicsRig->Settings[i].OutputCount);
        _currentRigOutputs.PushBack(currentRigOutput);

        PhysicsOutput previousRigOutput;
        previousRigOutput.outputs.Resize(_physicsRig->Settings[i].OutputCount);
        _previousRigOutputs.PushBack(previousRigOutput);

        for (csmInt32 j = 0; j < _physicsRig->Settings[i].OutputCount; ++j)
        {
            _physicsRig->Outputs[outputIndex + j].DestinationParameterIndex = -1;
            _physicsRig->Outputs[outputIndex + j].VertexIndex = json->GetOutputVertexIndex(i, j);
            _physicsRig->Outputs[outputIndex + j].AngleScale = json->GetOutputAngleScale(i, j);
            _physicsRig->Outputs[outputIndex + j].Weight = json->GetOutputWeight(i, j);
            _physicsRig->Outputs[outputIndex + j].Destination.TargetType = CubismPhysicsTargetType_Parameter;

            _physicsRig->Outputs[outputIndex + j].Destination.Id = json->GetOutputsDestinationId(i, j);
            if (strcmp(json->GetOutputType(i, j), PhysicsTypeTagX) == 0)
            {
                _physicsRig->Outputs[outputIndex + j].Type = CubismPhysicsSource_X;
                _physicsRig->Outputs[outputIndex + j].GetValue = GetOutputTranslationX;
                _physicsRig->Outputs[outputIndex + j].GetScale = GetOutputScaleTranslationX;
            }
            else if (strcmp(json->GetOutputType(i, j), PhysicsTypeTagY) == 0)
            {
                _physicsRig->Outputs[outputIndex + j].Type = CubismPhysicsSource_Y;
                _physicsRig->Outputs[outputIndex + j].GetValue = GetOutputTranslationY;
                _physicsRig->Outputs[outputIndex + j].GetScale = GetOutputScaleTranslationY;
            }
            else if (strcmp(json->GetOutputType(i, j), PhysicsTypeTagAngle) == 0)
            {
                _physicsRig->Outputs[outputIndex + j].Type = CubismPhysicsSource_Angle;
                _physicsRig->Outputs[outputIndex + j].GetValue = GetOutputAngle;
                _physicsRig->Outputs[outputIndex + j].GetScale = GetOutputScaleAngle;
            }

            _physicsRig->Outputs[outputIndex + j].Reflect = json->GetOutputReflect(i, j);
        }
        outputIndex += _physicsRig->Settings[i].OutputCount;

        // Particle
        _physicsRig->Settings[i].ParticleCount = json->GetParticleCount(i);
        _physicsRig->Settings[i].BaseParticleIndex = particleIndex;
        for (csmInt32 j = 0; j < _physicsRig->Settings[i].ParticleCount; ++j)
        {
            _physicsRig->Particles[particleIndex + j].Mobility = json->GetParticleMobility(i, j);
            _physicsRig->Particles[particleIndex + j].Delay = json->GetParticleDelay(i, j);
            _physicsRig->Particles[particleIndex + j].Acceleration = json->GetParticleAcceleration(i, j);
            _physicsRig->Particles[particleIndex + j].Radius = json->GetParticleRadius(i, j);
            _physicsRig->Particles[particleIndex + j].Position = json->GetParticlePosition(i, j);
        }

        particleIndex += _physicsRig->Settings[i].ParticleCount;
    }

    Initialize();

    CSM_DELETE(json);
}


void CubismPhysics::Stabilization(CubismModel* model)
{
    csmFloat32 totalAngle;
    csmFloat32 weight;
    csmFloat32 radAngle;
    csmFloat32 outputValue;
    CubismVector2 totalTranslation;
    csmInt32 i, settingIndex, particleIndex;
    CubismPhysicsSubRig* currentSetting;
    CubismPhysicsInput* currentInputs;
    CubismPhysicsOutput* currentOutputs;
    CubismPhysicsParticle* currentParticles;

    csmFloat32* parameterValues;
    const csmFloat32* parameterMaximumValues;
    const csmFloat32* parameterMinimumValues;
    const csmFloat32* parameterDefaultValues;

    parameterValues = Core::csmGetParameterValues(model->GetModel());
    parameterMaximumValues = Core::csmGetParameterMaximumValues(model->GetModel());
    parameterMinimumValues = Core::csmGetParameterMinimumValues(model->GetModel());
    parameterDefaultValues = Core::csmGetParameterDefaultValues(model->GetModel());

    if (_parameterCaches.GetSize() < model->GetParameterCount())
    {
        _parameterCaches.Resize(model->GetParameterCount());
    }
    if (_parameterInputCaches.GetSize() < model->GetParameterCount())
    {
        _parameterInputCaches.Resize(model->GetParameterCount());
    }

    for (int j = 0; j < model->GetParameterCount(); ++j)
    {
        _parameterCaches[j] = parameterValues[j];
        _parameterInputCaches[j] = parameterValues[j];
    }

    for (settingIndex = 0; settingIndex < _physicsRig->SubRigCount; ++settingIndex)
    {
        totalAngle = 0.0f;
        totalTranslation.X = 0.0f;
        totalTranslation.Y = 0.0f;
        currentSetting = &_physicsRig->Settings[settingIndex];
        currentInputs = &_physicsRig->Inputs[currentSetting->BaseInputIndex];
        currentOutputs = &_physicsRig->Outputs[currentSetting->BaseOutputIndex];
        currentParticles = &_physicsRig->Particles[currentSetting->BaseParticleIndex];

        // Load input parameters
        for (i = 0; i < currentSetting->InputCount; ++i)
        {
            weight = currentInputs[i].Weight / MaximumWeight;

            if (currentInputs[i].SourceParameterIndex == -1)
            {
                currentInputs[i].SourceParameterIndex = model->GetParameterIndex(currentInputs[i].Source.Id);
            }

            currentInputs[i].GetNormalizedParameterValue(
                &totalTranslation,
                &totalAngle,
                parameterValues[currentInputs[i].SourceParameterIndex],
                parameterMinimumValues[currentInputs[i].SourceParameterIndex],
                parameterMaximumValues[currentInputs[i].SourceParameterIndex],
                parameterDefaultValues[currentInputs[i].SourceParameterIndex],
                &currentSetting->NormalizationPosition,
                &currentSetting->NormalizationAngle,
                currentInputs[i].Reflect,
                weight
            );

            _parameterCaches[currentInputs[i].SourceParameterIndex] =
                parameterValues[currentInputs[i].SourceParameterIndex];
        }

        radAngle = CubismMath::DegreesToRadian(-totalAngle);

        totalTranslation.X = (totalTranslation.X * CubismMath::CosF(radAngle) - totalTranslation.Y * CubismMath::SinF(radAngle));
        totalTranslation.Y = (totalTranslation.X * CubismMath::SinF(radAngle) + totalTranslation.Y * CubismMath::CosF(radAngle));

        // Calculate particles position.
        UpdateParticlesForStabilization(
            currentParticles,
            currentSetting->ParticleCount,
            totalTranslation,
            totalAngle,
            _options.Wind,
            MovementThreshold * currentSetting->NormalizationPosition.Maximum
        );

        // Update output parameters.
        for (i = 0; i < currentSetting->OutputCount; ++i)
        {
            particleIndex = currentOutputs[i].VertexIndex;

            if (currentOutputs[i].DestinationParameterIndex == -1)
            {
                currentOutputs[i].DestinationParameterIndex = model->GetParameterIndex(
                    currentOutputs[i].Destination.Id);
            }

            if (particleIndex < 1 || particleIndex >= currentSetting->ParticleCount)
            {
                continue;
            }

            CubismVector2 translation;
            translation.X = currentParticles[particleIndex].Position.X - currentParticles[particleIndex - 1].Position.X;
            translation.Y = currentParticles[particleIndex].Position.Y - currentParticles[particleIndex - 1].Position.Y;

            outputValue = currentOutputs[i].GetValue(
                translation,
                currentParticles,
                particleIndex,
                currentOutputs[i].Reflect,
                _options.Gravity
            );

            _currentRigOutputs[settingIndex].outputs[i] = outputValue;
            _previousRigOutputs[settingIndex].outputs[i] = outputValue;

            UpdateOutputParameterValue(
                &parameterValues[currentOutputs[i].DestinationParameterIndex],
                parameterMinimumValues[currentOutputs[i].DestinationParameterIndex],
                parameterMaximumValues[currentOutputs[i].DestinationParameterIndex],
                outputValue,
                &currentOutputs[i]);

            _parameterCaches[currentOutputs[i].DestinationParameterIndex] = parameterValues[currentOutputs[i].DestinationParameterIndex];
        }
    }
}

/// Pendulum interpolation weights
///
/// 振り子の計算結果は保存され、パラメータへの出力は保存された前回の結果で補間されます。
/// The result of the pendulum calculation is saved and
/// the output to the parameters is interpolated with the saved previous result of the pendulum calculation.
///
/// 図で示すと[1]と[2]で補間されます。
/// The figure shows the interpolation between [1] and [2].
///
/// 補間の重みは最新の振り子計算タイミングと次回のタイミングの間で見た現在時間で決定する。
/// The weight of the interpolation are determined by the current time seen between
/// the latest pendulum calculation timing and the next timing.
///
/// 図で示すと[2]と[4]の間でみた(3)の位置の重みになる。
/// Figure shows the weight of position (3) as seen between [2] and [4].
///
/// 解釈として振り子計算のタイミングと重み計算のタイミングがズレる。
/// As an interpretation, the pendulum calculation and weights are misaligned.
///
/// physics3.jsonにFPS情報が存在しない場合は常に前の振り子状態で設定される。
/// If there is no FPS information in physics3.json, it is always set in the previous pendulum state.
///
/// この仕様は補間範囲を逸脱したことが原因の震えたような見た目を回避を目的にしている。
/// The purpose of this specification is to avoid the quivering appearance caused by deviations from the interpolation range.
///
/// ------------ time -------------->
///
///    　　　　　　　　|+++++|------| <- weight
/// ==[1]====#=====[2]---(3)----(4)
///          ^ output contents
///
/// 1:_previousRigOutputs
/// 2:_currentRigOutputs
/// 3:_currentRemainTime (now rendering)
/// 4:next particles timing
///
/// @param model
/// @param deltaTimeSeconds  rendering delta time.
void CubismPhysics::Evaluate(CubismModel* model, csmFloat32 deltaTimeSeconds)
{
    csmFloat32 totalAngle;
    csmFloat32 weight;
    csmFloat32 radAngle;
    csmFloat32 outputValue;
    CubismVector2 totalTranslation;
    csmInt32 i, settingIndex, particleIndex;
    CubismPhysicsSubRig* currentSetting;
    CubismPhysicsInput* currentInputs;
    CubismPhysicsOutput* currentOutputs;
    CubismPhysicsParticle* currentParticles;

    if (0.0f >= deltaTimeSeconds)
    {
        return;
    }

    csmFloat32* parameterValues;
    const csmFloat32* parameterMaximumValues;
    const csmFloat32* parameterMinimumValues;
    const csmFloat32* parameterDefaultValues;

    csmFloat32 physicsDeltaTime;
    _currentRemainTime += deltaTimeSeconds;
    if (_currentRemainTime > MaxDeltaTime)
    {
        _currentRemainTime = 0.0f;
    }

    parameterValues = Core::csmGetParameterValues(model->GetModel());
    parameterMaximumValues = Core::csmGetParameterMaximumValues(model->GetModel());
    parameterMinimumValues = Core::csmGetParameterMinimumValues(model->GetModel());
    parameterDefaultValues = Core::csmGetParameterDefaultValues(model->GetModel());

    if (_parameterCaches.GetSize() < model->GetParameterCount())
    {
        _parameterCaches.Resize(model->GetParameterCount());
    }
    if (_parameterInputCaches.GetSize() < model->GetParameterCount())
    {
        _parameterInputCaches.Resize(model->GetParameterCount());
        for (int j = 0; j < model->GetParameterCount(); ++j) {
            _parameterInputCaches[j] = parameterValues[j];
        }
    }

    if (_physicsRig->Fps > 0.0f)
    {
        physicsDeltaTime = 1.0f / _physicsRig->Fps;
    }
    else
    {
        physicsDeltaTime = deltaTimeSeconds;
    }

    while (_currentRemainTime >= physicsDeltaTime)
    {
        // copyRigOutputs _currentRigOutputs to _previousRigOutputs
        for (settingIndex = 0; settingIndex < _physicsRig->SubRigCount; ++settingIndex)
        {
            currentSetting = &_physicsRig->Settings[settingIndex];
            currentOutputs = &_physicsRig->Outputs[currentSetting->BaseOutputIndex];
            for (i = 0; i < currentSetting->OutputCount; ++i)
            {
                _previousRigOutputs[settingIndex].outputs[i] = _currentRigOutputs[settingIndex].outputs[i];
            }
        }

        // 入力キャッシュとパラメータで線形補間してUpdateParticlesするタイミングでの入力を計算する。
        // Calculate the input at the timing to UpdateParticles by linear interpolation with the _parameterInputCaches and parameterValues.
        // _parameterCachesはグループ間での値の伝搬の役割があるので_parameterInputCachesとの分離が必要。
        // _parameterCaches needs to be separated from _parameterInputCaches because of its role in propagating values between groups.
        float inputWeight =  physicsDeltaTime / _currentRemainTime;
        for (csmInt32 j = 0; j < model->GetParameterCount(); ++j)
        {
            _parameterCaches[j] = _parameterInputCaches[j] * (1.0f - inputWeight) + parameterValues[j] * inputWeight;
            _parameterInputCaches[j] = _parameterCaches[j];
        }

        for (settingIndex = 0; settingIndex < _physicsRig->SubRigCount; ++settingIndex)
        {
            totalAngle = 0.0f;
            totalTranslation.X = 0.0f;
            totalTranslation.Y = 0.0f;
            currentSetting = &_physicsRig->Settings[settingIndex];
            currentInputs = &_physicsRig->Inputs[currentSetting->BaseInputIndex];
            currentOutputs = &_physicsRig->Outputs[currentSetting->BaseOutputIndex];
            currentParticles = &_physicsRig->Particles[currentSetting->BaseParticleIndex];

            // Load input parameters.
            for (i = 0; i < currentSetting->InputCount; ++i)
            {
                weight = currentInputs[i].Weight / MaximumWeight;

                if (currentInputs[i].SourceParameterIndex == -1)
                {
                    currentInputs[i].SourceParameterIndex = model->GetParameterIndex(currentInputs[i].Source.Id);
                }

                currentInputs[i].GetNormalizedParameterValue(
                    &totalTranslation,
                    &totalAngle,
                    _parameterCaches[currentInputs[i].SourceParameterIndex],
                    parameterMinimumValues[currentInputs[i].SourceParameterIndex],
                    parameterMaximumValues[currentInputs[i].SourceParameterIndex],
                    parameterDefaultValues[currentInputs[i].SourceParameterIndex],
                    &currentSetting->NormalizationPosition,
                    &currentSetting->NormalizationAngle,
                    currentInputs[i].Reflect,
                    weight
                );
            }

            radAngle = CubismMath::DegreesToRadian(-totalAngle);

            totalTranslation.X = (totalTranslation.X * CubismMath::CosF(radAngle) - totalTranslation.Y * CubismMath::SinF(radAngle));
            totalTranslation.Y = (totalTranslation.X * CubismMath::SinF(radAngle) + totalTranslation.Y * CubismMath::CosF(radAngle));

            // Calculate particles position.
            UpdateParticles(
                currentParticles,
                currentSetting->ParticleCount,
                totalTranslation,
                totalAngle,
                _options.Wind,
                MovementThreshold * currentSetting->NormalizationPosition.Maximum,
                physicsDeltaTime,
                AirResistance
            );

            // Update output parameters.
            for (i = 0; i < currentSetting->OutputCount; ++i)
            {
                particleIndex = currentOutputs[i].VertexIndex;

                if (currentOutputs[i].DestinationParameterIndex == -1)
                {
                    currentOutputs[i].DestinationParameterIndex = model->GetParameterIndex(currentOutputs[i].Destination.Id);
                }

                if (particleIndex < 1 || particleIndex >= currentSetting->ParticleCount)
                {
                    continue;
                }

                CubismVector2 translation;
                translation.X = currentParticles[particleIndex].Position.X - currentParticles[particleIndex - 1].Position.X;
                translation.Y = currentParticles[particleIndex].Position.Y - currentParticles[particleIndex - 1].Position.Y;

                outputValue = currentOutputs[i].GetValue(
                    translation,
                    currentParticles,
                    particleIndex,
                    currentOutputs[i].Reflect,
                    _options.Gravity
                );

                _currentRigOutputs[settingIndex].outputs[i] = outputValue;

                UpdateOutputParameterValue(
                        &_parameterCaches[currentOutputs[i].DestinationParameterIndex],
                        parameterMinimumValues[currentOutputs[i].DestinationParameterIndex],
                        parameterMaximumValues[currentOutputs[i].DestinationParameterIndex],
                        outputValue,
                        &currentOutputs[i]);
            }
        }

        _currentRemainTime -= physicsDeltaTime;
    }

    const float alpha = _currentRemainTime / physicsDeltaTime;
    Interpolate(model, alpha);
}

void CubismPhysics::Interpolate(CubismModel* model, csmFloat32 weight)
{
    csmInt32 i, settingIndex;
    CubismPhysicsOutput* currentOutputs;
    CubismPhysicsSubRig* currentSetting;
    csmFloat32* parameterValues;
    const csmFloat32* parameterMaximumValues;
    const csmFloat32* parameterMinimumValues;

    parameterValues = Core::csmGetParameterValues(model->GetModel());
    parameterMaximumValues = Core::csmGetParameterMaximumValues(model->GetModel());
    parameterMinimumValues = Core::csmGetParameterMinimumValues(model->GetModel());

    for (settingIndex = 0; settingIndex < _physicsRig->SubRigCount; ++settingIndex)
    {
        currentSetting = &_physicsRig->Settings[settingIndex];
        currentOutputs = &_physicsRig->Outputs[currentSetting->BaseOutputIndex];

        // Load input parameters.
        for (i = 0; i < currentSetting->OutputCount; ++i)
        {
            if (currentOutputs[i].DestinationParameterIndex == -1)
            {
                continue;
            }

            UpdateOutputParameterValue(
                &parameterValues[currentOutputs[i].DestinationParameterIndex],
                parameterMinimumValues[currentOutputs[i].DestinationParameterIndex],
                parameterMaximumValues[currentOutputs[i].DestinationParameterIndex],
                _previousRigOutputs[settingIndex].outputs[i] * (1 - weight) + _currentRigOutputs[settingIndex].outputs[i] * weight,
                &currentOutputs[i]
            );
        }
    }
}

void CubismPhysics::SetOptions(const Options& options)
{
    _options = options;
}

const CubismPhysics::Options& CubismPhysics::GetOptions() const
{
    return _options;
}

}}}
