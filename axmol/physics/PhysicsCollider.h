/****************************************************************************
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

#include "axmol/base/Config.h"
#if defined(AX_ENABLE_PHYSICS)

#    include "axmol/base/Object.h"
#    include "axmol/math/Math.h"
#    include "box2d/box2d.h"

#    include "axmol/base/axstd.h"

namespace ax
{

class PhysicsBody;

typedef struct AX_DLL PhysicsMaterial
{
    float density;      ///< The density of the object.
    float restitution;  ///< The bounciness of the physics body.
    float friction;     ///< The roughness of the surface of a shape.

    PhysicsMaterial() : density(0.0f), restitution(0.0f), friction(0.0f) {}

    PhysicsMaterial(float aDensity, float aRestitution, float aFriction)
        : density(aDensity), restitution(aRestitution), friction(aFriction)
    {}
} PhysicsMaterial;

const PhysicsMaterial PHYSICSSHAPE_MATERIAL_DEFAULT;

/**
 *  @addtogroup physics
 *  @{
 *  @addtogroup physics_2d
 *  @{

 */

/**
 * @brief A shape for body. You do not create PhysicsWorld objects directly, instead, you can view PhysicsBody to see
 * how to create it.
 */
class AX_DLL PhysicsCollider : public Object
{
public:
    enum class Type
    {
        UNKNOWN,
        CIRCLE,
        BOX,
        POLYGON,
        EDGESEGMENT,
        EDGEBOX,
        EDGEPOLYGON,
        EDGECHAIN,

        /** @deprecated Use Type::POLYGON instead. */
        POLYGEN = POLYGON,

        /** @deprecated Use Type::EDGEPOLYGON instead. */
        EDGEPOLYGEN = EDGEPOLYGON,
    };

    enum CollisionBits
    {
        Static  = 0x00000001,
        Dynamic = 0x00000002,
        Debris  = 0x00000004,
        Player  = 0x00000008,

        All = (~0u)
    };

public:
    /**
     * Get the body that this shape attaches.
     *
     * @return A PhysicsBody object pointer.
     */
    PhysicsBody* getBody() const { return _body; }

    /**
     * Return this shape's type.
     *
     * @return A Type object.
     */
    Type getType() const { return _type; }

    /**
     * Return this shape's area.
     *
     * @return A float number.
     */
    float getArea() const { return _area; }

    /**
     * Set this shape's tag.
     *
     * @param tag An integer number that identifies a shape object.
     */
    void setTag(int tag) { _tag = tag; }

    /**
     * Get this shape's tag.
     *
     * @return An integer number.
     */
    int getTag() const { return _tag; }


    /**
     * Get this shape's density.
     *
     * @return A float number.
     */
    float getDensity() const { return _material.density; }

    /**
     * Set this shape's density.
     *
     * It will change the body's mass this shape attaches.
     *
     * @param density A float number.
     */
    void setDensity(float density, bool apply = true);

    /**
     * Get this shape's restitution.
     *
     * @return A float number.
     */
    float getRestitution() const { return _material.restitution; }

    /**
     * Set this shape's restitution.
     *
     * It will change the shape's elasticity.
     *
     * @param restitution A float number.
     */
    void setRestitution(float restitution, bool apply = true);

    /**
     * Get this shape's friction.
     *
     * @return A float number.
     */
    float getFriction() const { return _material.friction; }

    /**
     * Set this shape's friction.
     *
     * It will change the shape's friction.
     *
     * @param friction A float number.
     */
    void setFriction(float friction, bool apply = true);

    /**
     * Get this shape's PhysicsMaterial object.
     *
     * @return A PhysicsMaterial object reference.
     */
    const PhysicsMaterial& getMaterial() const { return _material; }

    /**
     * Set this shape's material.
     *
     * It will change the shape's mass, elasticity and friction.
     *
     * @param material A PhysicsMaterial object.
     */
    void setMaterial(const PhysicsMaterial& material);
    bool isSensor() const { return _sensor; }

    /**
     * box2d v3 doesn't support modify sensor for shapes runtime once the shape is created
     */
    void setSensor(bool sensor);

    /**
     * Get this shape's position offset.
     *
     * This function should be overridden in inherit classes.
     * @return A Vec2 object.
     */
    virtual Vec2 getOffset() { return Vec2::ZERO; }

    /**
     * Get this shape's center position.
     *
     * This function should be overridden in inherit classes.
     * @return A Vec2 object.
     */
    virtual Vec2 getCenter() { return getOffset(); }

    /**
     * Test point is inside this shape or not.
     *
     * @param point A Vec2 object.
     * @return A bool object.
     */
    bool containsPoint(const Vec2& point) const;

    /**
     * Move the points to the center.
     *
     * @param points A Vec2 object pointer.
     * @param count An integer number.
     * @param center A Vec2 object, default value is Vec2(0,0).
     */
    static void recenterPoints(Vec2* points, int count, const Vec2& center = Vec2::ZERO);

    /**
     * Get center of the polygon points.
     *
     * @param points A Vec2 object pointer.
     * @param count An integer number.
     * @return A Vec2 object.
     */
    static Vec2 getPolygonCenter(const Vec2* points, int count);

    /**
     * Set a mask that defines which categories this physics body belongs to.
     *
     * Every physics body in a scene can be assigned to up to 32 different categories, each corresponding to a bit in
     * the bit mask. You define the mask values used in your game. In conjunction with the collisionBitMask and
     * contactTestBitMask properties, you define which physics bodies interact with each other and when your game is
     * notified of these interactions.
     * @param bitmask An integer number, the default value is 0xFFFFFFFF (all bits set).
     */
    void setCategoryBitmask(int bitmask) { _categoryBitmask = bitmask; }

    /**
     * Get a mask that defines which categories this physics body belongs to.
     *
     * @return An integer number.
     */
    int getCategoryBitmask() const { return _categoryBitmask; }

    /**
     * A mask that defines which categories of bodies cause intersection notifications with this physics body.
     *
     * When two bodies share the same space, each body's category mask is tested against the other body's contact mask
     * by performing a logical AND operation. If either comparison results in a non-zero value, an PhysicsContact object
     * is created and passed to the physics world’s delegate. For best performance, only set bits in the contacts mask
     * for interactions you are interested in.
     * @param bitmask An integer number, the default value is 0x00000000 (all bits cleared).
     */
    void setContactTestBitmask(int bitmask) { _contactTestBitmask = bitmask; }

    /**
     * Get a mask that defines which categories of bodies cause intersection notifications with this physics body.
     *
     * @return An integer number.
     */
    int getContactTestBitmask() const { return _contactTestBitmask; }

    /**
     * A mask that defines which categories of physics bodies can collide with this physics body.
     *
     * When two physics bodies contact each other, a collision may occur. This body's collision mask is compared to the
     * other body's category mask by performing a logical AND operation. If the result is a non-zero value, then this
     * body is affected by the collision. Each body independently chooses whether it wants to be affected by the other
     * body. For example, you might use this to avoid collision calculations that would make negligible changes to a
     * body's velocity.
     * @param bitmask An integer number, the default value is 0xFFFFFFFF (all bits set).
     */
    void setCollisionBitmask(int bitmask) { _collisionBitmask = bitmask; }

    /**
     * Get a mask that defines which categories of physics bodies can collide with this physics body.
     *
     * @return An integer number.
     */
    int getCollisionBitmask() const { return _collisionBitmask; }

    /**
     * Set the group of body.
     *
     * Collision groups let you specify an integral group index. You can have all fixtures with the same group index
     * always collide (positive index) or never collide (negative index).
     * @param group An integer number, it have high priority than bit masks.
     */
    void setGroup(int group);

    /**
     * Get the group of body.
     *
     * @return An integer number.
     */
    int getGroup() { return _group; }

    void deatchFromBody();

    // A deatched collider can't be re-attach to rigibody again
    bool isDeatched() const;

protected:
    /** calculate the area of this shape */
    virtual float calculateArea() { return 0.0f; }

    virtual void setScale(float scaleX, float scaleY);
    virtual void updateScale();
    void addShape(b2ShapeId shape);

protected:
    PhysicsCollider(PhysicsBody* body);
    virtual ~PhysicsCollider() = 0;

protected:
    PhysicsBody* _body;

    axstd::pod_vector<b2ShapeId> _b2Shapes;

    Type _type;
    float _area;
    bool _sensor;
    float _scaleX;
    float _scaleY;
    float _newScaleX;
    float _newScaleY;
    PhysicsMaterial _material;
    int _tag;
    int _categoryBitmask;
    int _collisionBitmask;
    int _contactTestBitmask;
    int _group;

    friend class PhysicsWorld;
    friend class PhysicsBody;
    friend class PhysicsJoint;
    friend class PhysicsDebugDraw;
};

/** A circle shape. */
class AX_DLL PhysicsColliderCircle : public PhysicsCollider
{
public:
    /**
     * Creates a PhysicsColliderCircle with specified value.
     *
     * @param   radius A float number, it is the circle's radius.
     * @param   material A PhysicsMaterial object, the default value is PHYSICSSHAPE_MATERIAL_DEFAULT.
     * @param   offset A Vec2 object, it is the offset from the body's center of gravity in body local coordinates.
     * @return  An autoreleased PhysicsColliderCircle object pointer.
     */
    static PhysicsColliderCircle* create(PhysicsBody* body,
                                         float radius,
                                         const PhysicsMaterial& material = PHYSICSSHAPE_MATERIAL_DEFAULT,
                                         const Vec2& offset              = Vec2(0.0f, 0.0f));

    /**
     * Calculate the area of a circle with specified radius.
     *
     * @param radius A float number
     * @return A float number
     */
    static float calculateArea(float radius);

    /**
     * Get the circle's radius.
     *
     * @return A float number.
     */
    float getRadius() const;

    /**
     * Get this circle's position offset.
     *
     * @return A Vec2 object.
     */
    Vec2 getOffset() override;

protected:
    bool init(float radius,
              const PhysicsMaterial& material = PHYSICSSHAPE_MATERIAL_DEFAULT,
              const Vec2& offset              = Vec2::ZERO);
    float calculateArea() override;
    void updateScale() override;

protected:
    PhysicsColliderCircle(PhysicsBody* body) : PhysicsCollider(body) {}
    virtual ~PhysicsColliderCircle();
};

/** A polygon shape. */
class AX_DLL PhysicsColliderPolygon : public PhysicsCollider
{
public:
    /**
     * Creates a PhysicsColliderPolygon with specified value.
     *
     * @param   points A Vec2 object pointer, it is an array of Vec2.
     * @param   count An integer number, contains the count of the points array.
     * @param   material A PhysicsMaterial object, the default value is PHYSICSSHAPE_MATERIAL_DEFAULT.
     * @param   offset A Vec2 object, it is the offset from the body's center of gravity in body local coordinates.
     * @return  An autoreleased PhysicsColliderPolygon object pointer.
     */
    static PhysicsColliderPolygon* create(PhysicsBody* body,
                                          const Vec2* points,
                                          int count,
                                          const PhysicsMaterial& material = PHYSICSSHAPE_MATERIAL_DEFAULT,
                                          const Vec2& offset              = Vec2::ZERO,
                                          float radius                    = 0.0f);

    /**
     * Calculate the area of a polygon with specified value.
     *
     * @param   points A Vec2 object pointer, it is an array of Vec2.
     * @param   count An integer number, contains the count of the points array.
     * @return A float number.
     */
    static float calculateArea(const Vec2* points, int count);

    /**
     * Get a point of this polygon's points array.
     *
     * @param i A index of this polygon's points array.
     * @return A point value.
     */
    Vec2 getPoint(int i) const;

    /**
     * Get this polygon's points array.
     *
     * @param outPoints A Vec2 array pointer.
     */
    void getPoints(Vec2* outPoints) const;

    /**
     * Get this polygon's points array count.
     *
     * @return An integer number.
     */
    int getPointsCount() const;

    /**
     * Get this polygon's center position.
     *
     * @return A Vec2 object.
     */
    Vec2 getCenter() override;

protected:
    bool init(const Vec2* points,
              int count,
              const PhysicsMaterial& material = PHYSICSSHAPE_MATERIAL_DEFAULT,
              const Vec2& offset              = Vec2::ZERO,
              float radius                    = 0.0f);
    float calculateArea() override;
    void updateScale() override;

protected:
    PhysicsColliderPolygon(PhysicsBody* body) : PhysicsCollider(body) {}
    virtual ~PhysicsColliderPolygon();
};

/** A box shape. */
class AX_DLL PhysicsColliderBox : public PhysicsColliderPolygon
{
public:
    /**
     * Creates a PhysicsColliderBox with specified value.
     *
     * @param   size The size contains this box's width and height.
     * @param   material A PhysicsMaterial object, the default value is PHYSICSSHAPE_MATERIAL_DEFAULT.
     * @param   offset A Vec2 object, it is the offset from the body's center of gravity in body local coordinates.
     * @return  An autoreleased PhysicsColliderBox object pointer.
     */
    static PhysicsColliderBox* create(PhysicsBody* body,
                                      const Vec2& size,
                                      const PhysicsMaterial& material = PHYSICSSHAPE_MATERIAL_DEFAULT,
                                      const Vec2& offset              = Vec2::ZERO,
                                      float radius                    = 0.0f);

    /**
     * Get this box's width and height.
     *
     * @return An Vec2 object.
     */
    Vec2 getSize() const;

    /**
     * Get this box's position offset.
     *
     * @return A Vec2 object.
     */
    Vec2 getOffset() override { return getCenter(); }

protected:
    bool init(const Vec2& size,
              const PhysicsMaterial& material = PHYSICSSHAPE_MATERIAL_DEFAULT,
              const Vec2& offset              = Vec2::ZERO,
              float radius                    = 0.0f);

protected:
    PhysicsColliderBox(PhysicsBody* body) : PhysicsColliderPolygon(body) {}
    virtual ~PhysicsColliderBox();
};

/** A segment shape. */
class AX_DLL PhysicsColliderEdgeSegment : public PhysicsCollider
{
public:
    /**
     * Creates a PhysicsColliderEdgeSegment with specified value.
     *
     * @param   a It's the edge's begin position.
     * @param   b It's the edge's end position.
     * @param   material A PhysicsMaterial object, the default value is PHYSICSSHAPE_MATERIAL_DEFAULT.
     * @param   border It's a edge's border width.
     * @return  An autoreleased PhysicsColliderEdgeSegment object pointer.
     */
    static PhysicsColliderEdgeSegment* create(PhysicsBody* body,
                                              const Vec2& a,
                                              const Vec2& b,
                                              const PhysicsMaterial& material = PHYSICSSHAPE_MATERIAL_DEFAULT,
                                              float border                    = 1);

    /**
     * Get this edge's begin position.
     *
     * @return A Vec2 object.
     */
    Vec2 getPointA() const;

    /**
     * Get this edge's end position.
     *
     * @return A Vec2 object.
     */
    Vec2 getPointB() const;

    /**
     * Get this edge's center position.
     *
     * @return A Vec2 object.
     */
    Vec2 getCenter() override;

protected:
    bool init(const Vec2& a,
              const Vec2& b,
              const PhysicsMaterial& material = PHYSICSSHAPE_MATERIAL_DEFAULT,
              float border                    = 1);
    void updateScale() override;

protected:
    PhysicsColliderEdgeSegment(PhysicsBody* body) : PhysicsCollider(body) {}
    virtual ~PhysicsColliderEdgeSegment();

    friend class PhysicsBody;
};

/** An edge polygon shape. */
class AX_DLL PhysicsColliderEdgePolygon : public PhysicsCollider
{
public:
    /**
     * Creates a PhysicsColliderEdgePolygon with specified value.
     *
     * @param   points A Vec2 object pointer, it contains an array of points.
     * @param   count An integer number, contains the count of the points array.
     * @param   material A PhysicsMaterial object, the default value is PHYSICSSHAPE_MATERIAL_DEFAULT.
     * @param   border It's a edge's border width.
     * @return  An autoreleased PhysicsColliderEdgePolygon object pointer.
     */
    static PhysicsColliderEdgePolygon* create(PhysicsBody* body,
                                              const Vec2* points,
                                              int count,
                                              const PhysicsMaterial& material = PHYSICSSHAPE_MATERIAL_DEFAULT,
                                              float border                    = 1);

    /**
     * Get this polygon's center position.
     *
     * @return A Vec2 object.
     */
    Vec2 getCenter() override;

    /**
     * Get this polygon's points array.
     *
     * @param outPoints A Vec2 array pointer.
     */
    void getPoints(Vec2* outPoints) const;

    /**
     * Get this polygon's points array count.
     *
     * @return An integer number.
     */
    int getPointsCount() const;

protected:
    bool init(const Vec2* points,
              int count,
              const PhysicsMaterial& material = PHYSICSSHAPE_MATERIAL_DEFAULT,
              float border                    = 1);
    void updateScale() override;

protected:
    PhysicsColliderEdgePolygon(PhysicsBody* body) : PhysicsCollider(body) {}
    virtual ~PhysicsColliderEdgePolygon();

    friend class PhysicsBody;
};

/** An edge box shape. */
class AX_DLL PhysicsColliderEdgeBox : public PhysicsColliderEdgePolygon
{
public:
    /**
     * Creates a PhysicsColliderEdgeBox with specified value.
     *
     * @param   size The size contains this box's width and height.
     * @param   material A PhysicsMaterial object, the default value is PHYSICSSHAPE_MATERIAL_DEFAULT.
     * @param   border It's a edge's border width.
     * @param   offset A Vec2 object, it is the offset from the body's center of gravity in body local coordinates.
     * @return  An autoreleased PhysicsColliderEdgeBox object pointer.
     */
    static PhysicsColliderEdgeBox* create(PhysicsBody* body,
                                          const Vec2& size,
                                          const PhysicsMaterial& material = PHYSICSSHAPE_MATERIAL_DEFAULT,
                                          float border                    = 0,
                                          const Vec2& offset              = Vec2::ZERO);

    /**
     * Get this box's position offset.
     *
     * @return A Vec2 object.
     */
    Vec2 getOffset() override { return getCenter(); }

protected:
    bool init(const Vec2& size,
              const PhysicsMaterial& material = PHYSICSSHAPE_MATERIAL_DEFAULT,
              float border                    = 1,
              const Vec2& offset              = Vec2::ZERO);

protected:
    PhysicsColliderEdgeBox(PhysicsBody* body) : PhysicsColliderEdgePolygon(body) {}
    virtual ~PhysicsColliderEdgeBox();

    friend class PhysicsBody;
};

/** A chain shape. */
class AX_DLL PhysicsColliderEdgeChain : public PhysicsCollider
{
public:
    /**
     * Creates a PhysicsColliderEdgeChain with specified value.
     *
     * @param   points A Vec2 object pointer, it contains an array of points.
     * @param   count An integer number, contains the count of the points array.
     * @param   material A PhysicsMaterial object, the default value is PHYSICSSHAPE_MATERIAL_DEFAULT.
     * @param   border It's a edge's border width.
     * @return  An autoreleased PhysicsColliderEdgeChain object pointer.
     */
    static PhysicsColliderEdgeChain* create(PhysicsBody* body,
                                            const Vec2* points,
                                            int count,
                                            const PhysicsMaterial& material = PHYSICSSHAPE_MATERIAL_DEFAULT,
                                            float border                    = 1);

    /**
     * Get this chain's center position.
     *
     * @return A Vec2 object.
     */
    Vec2 getCenter() override;

    /**
     * Get this chain's points array.
     *
     * @param outPoints A Vec2 array pointer.
     */
    void getPoints(Vec2* outPoints) const;

    /**
     * Get this chain's points array count.
     *
     * @return An integer number.
     */
    int getPointsCount() const;

protected:
    bool init(const Vec2* points,
              int count,
              const PhysicsMaterial& material = PHYSICSSHAPE_MATERIAL_DEFAULT,
              float border                    = 1);
    void updateScale() override;

protected:
    PhysicsColliderEdgeChain(PhysicsBody* body) : PhysicsCollider(body) {}
    virtual ~PhysicsColliderEdgeChain();

    friend class PhysicsBody;
};

/** @} */
/** @} */

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS)
