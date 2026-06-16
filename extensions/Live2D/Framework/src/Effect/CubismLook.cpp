/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismLook.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

CubismLook* CubismLook::Create()
{
    return CSM_NEW CubismLook();
}

void CubismLook::Delete(CubismLook* instance)
{
    CSM_DELETE_SELF(CubismLook, instance);
}

void CubismLook::SetParameters(const csmVector<LookParameterData>& lookParameters)
{
    _lookParameters = lookParameters;
}

const csmVector<CubismLook::LookParameterData>& CubismLook::GetParameters() const
{
    return _lookParameters;
}

void CubismLook::UpdateParameters(CubismModel* model, const csmFloat32 dragX, const csmFloat32 dragY) const
{
    const csmFloat32 dragXY = dragX * dragY;

    for (csmUint32 i = 0; i < _lookParameters.GetSize(); ++i)
    {
        const LookParameterData& data = _lookParameters[i];

        model->AddParameterValue(data.ParameterId, data.FactorX * dragX + data.FactorY * dragY + data.FactorXY * dragXY);
    }
}

}}}
