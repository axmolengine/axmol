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

/// physics constatnts
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
    default:{
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

        strand[i].Force = CubismVector2( 0.0f, 0.0f );
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
}

CubismPhysics::~CubismPhysics()
{
    CSM_DELETE(_physicsRig);
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

        // Initialize paritcles.
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

CubismPhysics* CubismPhysics::Create(const csmByte* buffer, csmSizeInt size)
{
    CubismPhysics* ret = CSM_NEW CubismPhysics();

    ret->Parse(buffer, size);
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

    _physicsRig->Gravity = json->GetGravity();
    _physicsRig->Wind = json->GetWind();
    _physicsRig->SubRigCount = json->GetSubRigCount();

    _physicsRig->Settings.UpdateSize(_physicsRig->SubRigCount, CubismPhysicsSubRig(), true);
    _physicsRig->Inputs.UpdateSize(json->GetTotalInputCount(), CubismPhysicsInput(), true);
    _physicsRig->Outputs.UpdateSize(json->GetTotalOutputCount(), CubismPhysicsOutput(), true);
    _physicsRig->Particles.UpdateSize(json->GetVertexCount(), CubismPhysicsParticle(), true);

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

void CubismPhysics::Evaluate(CubismModel* model, csmFloat32 deltaTimeSeconds)
{
    csmFloat32 totalAngle;
    csmFloat32 weight;
    csmFloat32 radAngle;
    csmFloat32 outputValue;
    CubismVector2 totalTranslation;
    csmInt32 i, settingIndex, particleIndex;
    CubismPhysicsSubRig* currentSetting;
    CubismPhysicsInput* currentInput;
    CubismPhysicsOutput* currentOutput;
    CubismPhysicsParticle* currentParticles;

    csmFloat32* parameterValue;
    const csmFloat32* parameterMaximumValue;
    const csmFloat32* parameterMinimumValue;
    const csmFloat32* parameterDefaultValue;

    parameterValue = Core::csmGetParameterValues(model->GetModel());
    parameterMaximumValue = Core::csmGetParameterMaximumValues(model->GetModel());
    parameterMinimumValue = Core::csmGetParameterMinimumValues(model->GetModel());
    parameterDefaultValue = Core::csmGetParameterDefaultValues(model->GetModel());

    for (settingIndex = 0; settingIndex < _physicsRig->SubRigCount; ++settingIndex)
    {
        totalAngle = 0.0f;
        totalTranslation.X = 0.0f;
        totalTranslation.Y = 0.0f;
        currentSetting = &_physicsRig->Settings[settingIndex];
        currentInput = &_physicsRig->Inputs[currentSetting->BaseInputIndex];
        currentOutput = &_physicsRig->Outputs[currentSetting->BaseOutputIndex];
        currentParticles = &_physicsRig->Particles[currentSetting->BaseParticleIndex];

        // Load input parameters.
        for (i = 0; i < currentSetting->InputCount; ++i)
        {
            weight = currentInput[i].Weight / MaximumWeight;

            if (currentInput[i].SourceParameterIndex == -1)
            {
                currentInput[i].SourceParameterIndex = model->GetParameterIndex(currentInput[i].Source.Id);
            }

            currentInput[i].GetNormalizedParameterValue(
                &totalTranslation,
                &totalAngle,
                parameterValue[currentInput[i].SourceParameterIndex],
                parameterMinimumValue[currentInput[i].SourceParameterIndex],
                parameterMaximumValue[currentInput[i].SourceParameterIndex],
                parameterDefaultValue[currentInput[i].SourceParameterIndex],
                &currentSetting->NormalizationPosition,
                &currentSetting->NormalizationAngle,
                currentInput[i].Reflect,
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
            deltaTimeSeconds,
            AirResistance
        );

        // Update output parameters.
        for (i = 0; i < currentSetting->OutputCount; ++i)
        {
            particleIndex = currentOutput[i].VertexIndex;

            if (particleIndex < 1 || particleIndex >= currentSetting->ParticleCount)
            {
                break;
            }

            if (currentOutput[i].DestinationParameterIndex == -1)
            {
                currentOutput[i].DestinationParameterIndex = model->GetParameterIndex(currentOutput[i].Destination.Id);
            }

            CubismVector2 translation;
            translation.X = currentParticles[particleIndex].Position.X - currentParticles[particleIndex - 1].Position.X;
            translation.Y = currentParticles[particleIndex].Position.Y - currentParticles[particleIndex - 1].Position.Y;

            outputValue = currentOutput[i].GetValue(
                translation,
                currentParticles,
                particleIndex,
                currentOutput[i].Reflect,
                _options.Gravity
            );

            UpdateOutputParameterValue(
                &parameterValue[currentOutput[i].DestinationParameterIndex],
                parameterMinimumValue[currentOutput[i].DestinationParameterIndex],
                parameterMaximumValue[currentOutput[i].DestinationParameterIndex],
                outputValue,
                &currentOutput[i]);
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
