/****************************************************************************
 Copyright (c) 2016 Google Inc.
 Copyright (c) 2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/vr/VRDistortion.h"
#include <math.h>

namespace ax
{
inline namespace experimental
{
Distortion::Distortion()
{
    _coefficients[0] = 0.441f;
    _coefficients[1] = 0.156f;
}

void Distortion::setCoefficients(float* coefficients)
{
    for (int i = 0; i < s_numberOfCoefficients; i++)
    {
        _coefficients[i] = coefficients[i];
    }
}

float* Distortion::coefficients()
{
    return _coefficients;
}

float Distortion::distortionFactor(float radius)
{
    float result        = 1.0f;
    float rFactor       = 1.0f;
    float squaredRadius = radius * radius;
    for (int i = 0; i < s_numberOfCoefficients; i++)
    {
        rFactor *= squaredRadius;
        result += _coefficients[i] * rFactor;
    }
    return result;
}

float Distortion::distort(float radius)
{
    return radius * distortionFactor(radius);
}

float Distortion::distortInverse(float radius)
{
    float r0  = radius / 0.9f;
    float r   = radius * 0.9f;
    float dr0 = radius - distort(r0);
    while (fabsf(r - r0) > 0.0001f)
    {
        float dr = radius - distort(r);
        float r2 = r - dr * ((r - r0) / (dr - dr0));
        r0       = r;
        r        = r2;
        dr0      = dr;
    }
    return r;
}
}  // namespace experimental
}  // namespace ax
