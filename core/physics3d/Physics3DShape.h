/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
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

#ifndef __PHYSICS_3D_SHAPE_H__
#define __PHYSICS_3D_SHAPE_H__

#include "base/Object.h"
#include "base/Config.h"
#include "math/Math.h"

#if defined(AX_ENABLE_3D_PHYSICS)

class btCollisionShape;

namespace ax
{
/**
 * @addtogroup _3d
 * @{
 */

/**
 * @brief Create a physical shape(box, sphere, cylinder, capsule, convexhull, mesh and heightfield)
 */
class AX_DLL Physics3DShape : public Object
{
public:
    enum class ShapeType
    {
        UNKNOWN = 0,
        BOX,
        SPHERE,
        CYLINDER,
        CAPSULE,
        CONVEX,
        MESH,
        HEIGHT_FIELD,
        COMPOUND
    };

    /**
     * get shape type
     */
    virtual ShapeType getShapeType() const;

    /**
     * create box shape
     * @param extent The extent of sphere.
     */
    static Physics3DShape* createBox(const ax::Vec3& extent);

    /**
     * create sphere shape
     * @param radius The radius of sphere.
     */
    static Physics3DShape* createSphere(float radius);

    /**
     * create cylinder shape
     * @param radius The radius of cylinder.
     * @param height The height.
     */
    static Physics3DShape* createCylinder(float radius, float height);

    /**
     * create capsule shape
     * @param radius The radius of capsule.
     * @param height The height (cylinder part).
     */
    static Physics3DShape* createCapsule(float radius, float height);

    /**
     * create convex hull
     * @param points The vertices of convex hull
     * @param numPoints The number of vertices.
     */
    static Physics3DShape* createConvexHull(const ax::Vec3* points, int numPoints);

    /**
     * create mesh
     * @param triangles The pointer of triangle list
     * @param numTriangles The number of triangles.
     */
    static Physics3DShape* createMesh(const ax::Vec3* triangles, int numTriangles);

    /**
     * create heightfield
     * @param heightStickWidth The Width of heightfield
     * @param heightStickLength The Length of heightfield.
     * @param heightfieldData The Data of heightfield.
     * @param minHeight The minHeight of heightfield.
     * @param maxHeight The maxHeight of heightfield.
     * @param flipQuadEdges if flip QuadEdges
     */
    static Physics3DShape* createHeightfield(int heightStickWidth,
                                             int heightStickLength,
                                             const void* heightfieldData,
                                             float heightScale,
                                             float minHeight,
                                             float maxHeight,
                                             bool useFloatDatam,
                                             bool flipQuadEdges,
                                             bool useDiamondSubdivision = false);

    /**
     * create Compound Shape
     * @param shapes The list of child shape
     */
    static Physics3DShape* createCompoundShape(const std::vector<std::pair<Physics3DShape*, Mat4>>& shapes);

    btCollisionShape* getbtShape() const { return _btShape; }

    Physics3DShape();
    ~Physics3DShape();

    bool initBox(const ax::Vec3& ext);
    bool initSphere(float radius);
    bool initCylinder(float radius, float height);
    bool initCapsule(float radius, float height);
    bool initConvexHull(const ax::Vec3* points, int numPoints);
    bool initMesh(const ax::Vec3* triangles, int numTriangles);
    bool initHeightfield(int heightStickWidth,
                         int heightStickLength,
                         const void* heightfieldData,
                         float heightScale,
                         float minHeight,
                         float maxHeight,
                         bool useFloatDatam,
                         bool flipQuadEdges,
                         bool useDiamondSubdivision);
    bool initCompoundShape(const std::vector<std::pair<Physics3DShape*, Mat4>>& shapes);

protected:
    ShapeType _shapeType;  // shape type

    btCollisionShape* _btShape;
    unsigned char* _heightfieldData;
    std::vector<Physics3DShape*> _compoundChildShapes;
};

// end of 3d group
/// @}

}

#endif  // defined(AX_ENABLE_3D_PHYSICS)

#endif  // __PHYSICS_3D_SHAPE_H__
