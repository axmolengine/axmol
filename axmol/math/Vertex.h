/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2011 ForzeField Studios S.L
 Copyright (c) 2013-2016 Chukong Technologies Inc.
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
#pragma once

#include "axmol/math/Math.h"

/**
 * @addtogroup base
 * @{
 */

namespace ax
{

/** @file Vertex.h */

/** @struct Tex2F
 * A TEXCOORD composed of 2 floats: u, v
 * @since v3.0
 */
typedef Vec2 Tex2F;

/** @struct Quad2
 * A 2D Quad. 4 * 2 floats.
 */
struct Quad2
{
    Vec2 tl;
    Vec2 tr;
    Vec2 bl;
    Vec2 br;
};

/** @struct Quad3
 * A 3D Quad. 4 * 3 floats.
 */
struct Quad3
{
    Vec3 bl;
    Vec3 br;
    Vec3 tl;
    Vec3 tr;
};

/* @struct V2F_T2F
 * A Vec2 with a vertex point and a tex coord point.
 * MotionStreak
 */
struct V2F_T2F_C4B
{
    /// position (2F)
    Vec2 position;
    /// tex coords (2F)
    Tex2F texCoord;
    /// color (4F)
    Color32 color;
};

/** @struct V2F_T2F_C4F
 * A Vec2 with a vertex point, a tex coord point and a color 4F.
 */
struct V2F_T2F_C4F
{
    /// position (2F)
    Vec2 position;
    /// tex coords (2F)
    Tex2F texCoord;
    /// color (4F)
    Color color;
};

/** @struct V3F_T2F_C4B
 * A Vec2 with a vertex point, a tex coord point and a color 4B.
 */
struct V3F_T2F_C4B
{
    /// position (3F)
    Vec3 position;  // 12 bytes

    // tex coords (2F)
    Tex2F texCoord;  // 8 bytes

    /// color (4B)
    Color32 color;  // 4 bytes
};

/** @struct V3F_T2F_C4F
 * A Vec2 with a vertex point, a tex coord point and a color 4F.
 */
struct V3F_T2F_C4F
{
    /// position (3F)
    Vec3 position;  // 12 bytes

    // tex coords (2F)
    Tex2F texCoord;  // 8 bytes

    /// color (4F)
    Color color;  // 16 bytes
};

/** @struct V3F_T2F
 * A Vec2 with a vertex point, a tex coord point.
 */
struct V3F_T2F
{
    /// position (2F)
    Vec3 position;
    /// tex coords (2F)
    Tex2F texCoord;
};

/** @struct V3F_C4F
 * A Vec3 with a vertex point, a color.
 */
struct V3F_C4F
{
    /// position (3F)
    Vec3 position;
    /// color (4F)
    Color color;
};

struct V3F_T2F_N3F
{
    Vec3 position;
    Tex2F texcoord;
    Vec3 normal;
};

struct V2F_T2F_C4F_Triangle
{
    V2F_T2F_C4F a;
    V2F_T2F_C4F b;
    V2F_T2F_C4F c;
};

/** @struct V3F_T2F_C4F_Quad
 * 4 Vertex3FTex2FColor32.
 */
struct V3F_T2F_C4B_Quad
{
    /// top left
    V3F_T2F_C4B tl;
    /// bottom left
    V3F_T2F_C4B bl;
    /// top right
    V3F_T2F_C4B tr;
    /// bottom right
    V3F_T2F_C4B br;
};

/** @struct V3F_T2F_C4F_Quad
 * 4 Vertex3FTex2FColor4F.
 */
struct V3F_T2F_C4F_Quad
{
    /// top left
    V3F_T2F_C4F tl;
    /// bottom left
    V3F_T2F_C4F bl;
    /// top right
    V3F_T2F_C4F tr;
    /// bottom right
    V3F_T2F_C4F br;
};

/** @struct V3F_T2F_Quad
 *
 */
struct V3F_T2F_Quad
{
    /// bottom left
    V3F_T2F bl;
    /// bottom right
    V3F_T2F br;
    /// top left
    V3F_T2F tl;
    /// top right
    V3F_T2F tr;
};

/** converts a line to a polygon */
void AX_DLL
vertexLineToPolygon(Vec2* points, float stroke, V2F_T2F_C4B* vertices, unsigned int offset, unsigned int nuPoints);

/** returns whether or not the line intersects */
bool AX_DLL
vertexLineIntersect(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Dx, float Dy, float* T);

}  // namespace ax

// end of base group
/// @}
