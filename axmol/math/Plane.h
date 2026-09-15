/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/math/Vec3.h"

namespace ax
{

enum class PointSide
{
    IN_PLANE,
    FRONT_PLANE,
    BEHIND_PLANE,
};

/**
 * Defines plane
 * @lua NA
 **/
class AX_DLL Plane
{
public:
    /**
     * create plane from tree point.
     */
    Plane(const Vec3& p1, const Vec3& p2, const Vec3& p3);

    /**
     * create plane from normal and dist.
     */
    Plane(const Vec3& normal, float dist);

    /**
     * create plane from normal and a point on plane.
     */
    Plane(const Vec3& normal, const Vec3& point);

    /**
     * create a default plan whose normal is (0, 0, 1), and _dist is 0, xoy plan in fact.
     */
    Plane();

    /**
     * init plane from tree point.
     */
    void initPlane(const Vec3& p1, const Vec3& p2, const Vec3& p3);

    /**
     * init plane from normal and dist.
     */
    void initPlane(const Vec3& normal, float dist);

    /**
     * init plane from normal and a point on plane.
     */
    void initPlane(const Vec3& normal, const Vec3& point);

    /**
     * dist to plane, > 0 normal direction
     */
    float dist2Plane(const Vec3& p) const;

    /**
     * Gets the plane's normal.
     */
    const Vec3& getNormal() const { return _normal; }

    /**
     * Gets the plane's distance to the origin along its normal.
     */
    float getDist() const { return _dist; }

    /**
     * Return the side where the point is.
     */
    PointSide getSide(const Vec3& point) const;

protected:
    Vec3 _normal;  // the normal line of the plane
    float _dist;   // original displacement of the normal
};

}  // namespace ax
