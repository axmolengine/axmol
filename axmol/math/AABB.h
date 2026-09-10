/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/math/Vec3.h"
#include "axmol/math/Mat4.h"

namespace ax
{

/**
 * @addtogroup _3d
 * @{
 */

/**
 * Axis Aligned Bounding Box (AABB), usually calculate some rough but fast collision detection.
 */
class AX_DLL AABB
{

public:
    /**
     * Constructor.
     * @lua new
     */
    AABB();

    /**
     * Constructor.
     * @lua new
     */
    AABB(const Vec3& min, const Vec3& max);

    /**
     * Gets the center point of the bounding box.
     */
    Vec3 getCenter();

    /* Near face, specified counter-clockwise looking towards the origin from the positive z-axis.
     * verts[0] : left top front
     * verts[1] : left bottom front
     * verts[2] : right bottom front
     * verts[3] : right top front
     *
     * Far face, specified counter-clockwise looking towards the origin from the negative z-axis.
     * verts[4] : right top back
     * verts[5] : right bottom back
     * verts[6] : left bottom back
     * verts[7] : left top back
     */
    void getCorners(Vec3* dst) const;

    /**
     * Tests whether this bounding box intersects the specified bounding object.
     */
    bool intersects(const AABB& aabb) const;

    /**
     * check whether the point is in.
     */
    bool containPoint(const Vec3& point) const;

    /**
     * Sets this bounding box to the smallest bounding box
     * that contains both this bounding object and the specified bounding box.
     */
    void merge(const AABB& box);

    /**
     * Sets this bounding box to the specified values.
     */
    void set(const Vec3& min, const Vec3& max);

    /**
     * Reset min and max value.If you invoke this method, isEmpty() shall return true.
     */
    void reset();

    /**
     * check the AABB object is empty(reset).
     */
    bool isEmpty() const;

    /**
     * update the _min and _max from the given point.
     */
    void updateMinMax(const Vec3* point, ssize_t num);

    /**
     * Transforms the bounding box by the given transformation matrix.
     */
    void transform(const Mat4& mat);

public:
    Vec3 _min;
    Vec3 _max;
};

// end of 3d group
/// @}

}  // namespace ax
