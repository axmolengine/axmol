/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/math/AABB.h"
#include "axmol/math/OBB.h"
#include "axmol/math/Plane.h"

namespace ax
{

class Camera;

/**
 * the frustum is a six-side geometry, usually use the frustum to do fast-culling:
 * check a entity whether is a potential visible entity
 * @lua NA
 */
class AX_DLL Frustum
{
    friend class Camera;

public:
    /**
     * Constructor & Destructor.
     */
    Frustum() : _clipZ(true), _initialized(false) {}
    ~Frustum() {}

    /**
     * init frustum from camera.
     */
    bool initFrustum(const Camera* camera);

    /**
     * is aabb out of frustum.
     */
    bool isOutOfFrustum(const AABB& aabb) const;
    /**
     * is obb out of frustum
     */
    bool isOutOfFrustum(const OBB& obb) const;

    /**
     * get & set z clip. if bclipZ == true use near and far plane
     */
    void setClipZ(bool clipZ) { _clipZ = clipZ; }
    bool isClipZ() { return _clipZ; }

protected:
    /**
     * create clip plane
     */
    void createPlane(const Camera* camera);

    Plane _plane[6];  // clip plane, left, right, top, bottom, near, far
    bool _clipZ;      // use near and far clip plane
    bool _initialized;
};

}  // namespace ax
