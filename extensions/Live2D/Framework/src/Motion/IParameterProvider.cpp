/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "IParameterProvider.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

csmBool IParameterProvider::Update()
{
    return false;
}

csmBool IParameterProvider::Update(const csmFloat32 deltaTimeSeconds)
{
    return false;
}
}}}
