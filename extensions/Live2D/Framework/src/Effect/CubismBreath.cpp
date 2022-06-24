/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismBreath.hpp"
#include <math.h>

namespace Live2D { namespace Cubism { namespace Framework {

CubismBreath* CubismBreath::Create()
{
    return CSM_NEW CubismBreath();
}

void CubismBreath::Delete(CubismBreath* instance)
{
    CSM_DELETE_SELF(CubismBreath, instance);
}

CubismBreath::CubismBreath()
                            : _currentTime(0.0f)
{ }

CubismBreath::~CubismBreath()
{ }

void CubismBreath::SetParameters(const csmVector<BreathParameterData>& breathParameters)
{
    _breathParameters = breathParameters;
}

const csmVector<CubismBreath::BreathParameterData>& CubismBreath::GetParameters() const
{
    return _breathParameters;
}

void CubismBreath::UpdateParameters(CubismModel* model, csmFloat32 deltaTimeSeconds)
{
    _currentTime += deltaTimeSeconds;

    const csmFloat32 t = _currentTime * 2.0f * 3.14159f;

    for (csmUint32 i = 0; i < _breathParameters.GetSize(); ++i)
    {
        BreathParameterData* data = &_breathParameters[i];

        model->AddParameterValue(data->ParameterId, data->Offset + (data->Peak * sinf(t / data->Cycle)), data->Weight);
    }
}

}}}
