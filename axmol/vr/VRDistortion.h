/****************************************************************************
 Copyright (c) 2016 Google Inc.
 Copyright (c) 2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/platform/PlatformMacros.h"

namespace ax
{
inline namespace experimental
{
// Barrel Distortion
class AX_DLL Distortion
{
public:
    Distortion();

    void setCoefficients(float* coefficients);
    float* coefficients();

    float distortionFactor(float radius);
    float distort(float radius);
    float distortInverse(float radius);

private:
    static const int s_numberOfCoefficients = 2;
    float _coefficients[s_numberOfCoefficients];
};

}  // namespace experimental
}  // namespace ax
