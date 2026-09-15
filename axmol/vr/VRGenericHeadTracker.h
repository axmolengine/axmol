/****************************************************************************
 Copyright (c) 2016 Google Inc.
 Copyright (c) 2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/vr/VRBase.h"
#include "axmol/math/Vec3.h"
#include "axmol/math/Mat4.h"

namespace ax
{
inline namespace experimental
{
class AX_DLL VRGenericHeadTracker : public IVRHeadTracker
{
public:
    VRGenericHeadTracker();
    virtual ~VRGenericHeadTracker();

    virtual Vec3 getLocalPosition();
    virtual Mat4 getLocalRotation();

protected:
    void startTracking();
    void stopTracking();

    Vec3 _localPosition;

    Mat4 _deviceToDisplay;
    Mat4 _worldToInertialReferenceFrame;

    void* _motionMgr;
};
}  // namespace experimental
}  // namespace ax
