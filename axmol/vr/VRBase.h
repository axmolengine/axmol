/****************************************************************************
 Copyright (c) 2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/base/Types.h"

namespace ax
{

inline namespace experimental
{
class AX_DLL IVRHeadTracker
{
public:
    virtual ~IVRHeadTracker() {}

    virtual Vec3 getLocalPosition() = 0;
    virtual Mat4 getLocalRotation() = 0;
};
}  // namespace experimental

}  // namespace ax
