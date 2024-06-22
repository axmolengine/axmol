/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __CCPHYSICS_HELPER_H__
#define __CCPHYSICS_HELPER_H__

#include "base/Config.h"
#if defined(AX_ENABLE_PHYSICS)

#    include "chipmunk/chipmunk.h"
#    include "platform/PlatformMacros.h"
#    include "math/Math.h"

NS_AX_BEGIN

/**
 * @addtogroup physics
 * @{
 * @addtogroup physics_2d
 * @{
 */

/**
 * A physics helper class.
 *
 * Support for conversion between the chipmunk types and cocos types, eg: cpVect to Vec2, Vec2 to cpVect, cpFloat to
 * float.
 */
class PhysicsHelper
{
public:
    /** Make cpVect type convert to Vec2 type. */
    static Vec2 cpv2vec2(const cpVect& vec) { return Vec2(vec.x, vec.y); }

    /** Make Vec2 type convert to cpVect type. */
    static cpVect vec22cpv(const Vec2& point) { return cpv(point.x, point.y); }

    /** Make cpFloat type convert to float type. */
    static float cpfloat2float(cpFloat f) { return f; }

    /** Make Rect type convert to cpBB type. */
    static cpBB rect2cpbb(const Rect& rect)
    {
        return cpBBNew(rect.origin.x, rect.origin.y, rect.origin.x + rect.size.width, rect.origin.y + rect.size.height);
    }

    /** Make cpBB type convert to Rect type. */
    static Rect cpbb2rect(const cpBB& bb) { return Rect(bb.l, bb.b, bb.r - bb.l, bb.t - bb.b); }

    /**
     Make cpVect array convert to Vec2 array.

     @param cpvs The be converted object, it's a cpVect array.
     @param out The converted object, it's a Vec2 array.
     @param count It's cpvs array length.
     @return The out object's pointer.
     */
    static Vec2* cpvs2points(const cpVect* cpvs, Vec2* out, int count)
    {
        for (int i = 0; i < count; ++i)
        {
            out[i] = cpv2vec2(cpvs[i]);
        }

        return out;
    }

    /**
     Make Vec2 array convert to cpVect array.

     @param points The be converted object, it's a Vec2 array.
     @param out The converted object, it's a cpVect array.
     @param count It's points array length.
     @return The out object's pointer.
     */
    static cpVect* points2cpvs(const Vec2* points, cpVect* out, int count)
    {
        for (int i = 0; i < count; ++i)
        {
            out[i] = vec22cpv(points[i]);
        }

        return out;
    }
};

/** @} */
/** @} */

NS_AX_END

#endif  // AX_ENABLE_PHYSICS
#endif  // __CCPHYSICS_HELPER_H__
