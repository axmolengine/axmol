/**
 Copyright 2013 BlackBerry Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 */

#include "math/Vec4.h"

namespace ax
{

inline constexpr Vec4::Vec4() {}

inline constexpr Vec4::Vec4(float xx, float yy, float zz, float ww) : Vec4Base(xx, yy, zz, ww) {}

inline constexpr Vec4::Vec4(const float* src) : Vec4Base(src) {}

inline constexpr Vec4::Vec4(const Vec4& p1, const Vec4& p2)
{
    setDirection(p1, p2);
}

inline constexpr void Vec4::setDirection(const Vec4& p1, const Vec4& p2)
{
    x = p2.x - p1.x;
    y = p2.y - p1.y;
    z = p2.z - p1.z;
    w = p2.w - p1.w;
}

#if !(defined(AX_DLLEXPORT) || defined(AX_DLLIMPORT))
    inline constexpr Vec4 Vec4::ZERO   = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
    inline constexpr Vec4 Vec4::ONE    = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    inline constexpr Vec4 Vec4::UNIT_X = Vec4(1.0f, 0.0f, 0.0f, 0.0f);
    inline constexpr Vec4 Vec4::UNIT_Y = Vec4(0.0f, 1.0f, 0.0f, 0.0f);
    inline constexpr Vec4 Vec4::UNIT_Z = Vec4(0.0f, 0.0f, 1.0f, 0.0f);
    inline constexpr Vec4 Vec4::UNIT_W = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
#endif

}
