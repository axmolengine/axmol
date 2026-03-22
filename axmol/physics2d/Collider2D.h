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
#if defined(AX_ENABLE_PHYSICS_2D)

#    include "axmol/physics2d/PhysicsMaterial2D.h"
#    include "axmol/base/Object.h"
#    include "axmol/math/Math.h"
#    include "box2d/box2d.h"

#    include "axmol/tlx/vector.hpp"
#    include <span>

namespace ax
{

class Rigidbody2D;

/**
 *  @addtogroup physics
 *  @{
 *  @addtogroup physics_2d
 *  @{

 */

/**
 * @brief A shape for body. You do not create PhysicsWorld2D objects directly, instead, you can view Rigidbody2D to see
 * how to create it.
 */
class AX_DLL Collider2D : public Object
{
    friend class Rigidbody2D;

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

    /*
     * @see also b2Filter
     *   @member categoryBits The collision category bits.
     *   @member maskBits The collision mask bits.This states the categories that this shape would accept for collision.
     *           For example, you may want your player to only collide with static objects and other players.
     *           maskBits = Static | Player;
     *   @member groupIndex Collision groups allow a certain group of objects to never collide (negative) or always
     * collide (positive).
     *
     */
    enum CategoryBits : uint64_t
    {
        Static  = 1,
        Dynamic = 1 << 1,
        Debris  = 1 << 2,
        Player  = 1 << 3,

        All = (~0llu)
    };

    using CollisionBits   = CategoryBits;
    using CollisionFilter = b2Filter;

    static constexpr uint64_t DefaultCategoryBits = B2_DEFAULT_CATEGORY_BITS;
    static constexpr uint64_t DefaultMaskBits     = B2_DEFAULT_MASK_BITS;
    static constexpr uint64_t DefaultGroupIndex   = 0;

    static constexpr CollisionFilter DefaultCollisionFilter =
        CollisionFilter{DefaultCategoryBits, DefaultMaskBits, DefaultGroupIndex};

public:
    /**
     * Get the body that this shape attaches.
     *
     * @return A Rigidbody2D object pointer.
     */
    Rigidbody2D* getAttachedBody() const { return _attachedBody; }

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
     * Get this shape's PhysicsMaterial2D object.
     *
     * @return A PhysicsMaterial2D object reference.
     */
    const PhysicsMaterial2D& getMaterial() const { return _material; }

    /**
     * Set this shape's material.
     *
     * It will change the shape's mass, elasticity and friction.
     *
     * @param material A PhysicsMaterial2D object.
     */
    void setMaterial(const PhysicsMaterial2D& material);
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
    static Vec2 getPolygonCenter(std::span<const Vec2> points);
    /**
     * A mask that defines which categories of bodies cause intersection notifications with this physics body.
     *
     * When two bodies share the same space, each body's category mask is tested against the other body's contact mask
     * by performing a logical AND operation. If either comparison results in a non-zero value, an Contact2D object
     * is created and passed to the physics world’s delegate. For best performance, only set bits in the contacts mask
     * for interactions you are interested in.
     * @param contactMaskBits An integer number, the default value is 0 (all bits cleared).
     */
    void setContactMaskBits(uint64_t contactMaskBits) { _contactMaskBits = contactMaskBits; }

    /**
     * Get a mask that defines which categories of bodies cause intersection notifications with this physics body.
     *
     * @return An integer number.
     */
    uint64_t getContactMaskBits() const { return _contactMaskBits; }

    /**
     * Set a mask that defines which categories this physics body belongs to.
     *
     * Every physics body in a scene can be assigned to up to 64 different categories, each corresponding to a bit in
     * the bit mask. You define the mask values used in your game. In conjunction with the collisionBits and
     * contactTestBits properties, you define which physics bodies interact with each other and when your game is
     * notified of these interactions.
     * @param categoryBits An integer number, the default value is UINT64_MAX (all bits set).
     */
    void setCategoryBits(uint64_t categoryBits);

    /**
     * Get a mask that defines which categories this physics body belongs to.
     *
     * @return An integer number.
     */
    int getCategoryBits() const { return _collisionFilter.categoryBits; }

    /**
     * A mask that defines which categories of physics bodies can collide with this physics body.
     *
     * When two physics bodies contact each other, a collision may occur. This body's collision mask is compared to the
     * other body's category mask by performing a logical AND operation. If the result is a non-zero value, then this
     * body is affected by the collision. Each body independently chooses whether it wants to be affected by the other
     * body. For example, you might use this to avoid collision calculations that would make negligible changes to a
     * body's velocity.
     * @param maskBits An integer number, the default value is UINT64_MAX (all bits set).
     */
    void setMaskBits(uint64_t maskBits);

    /**
     * Get a mask that defines which categories of physics bodies can collide with this physics body.
     *
     * @return An integer number.
     */
    uint64_t getMaskBits() const { return _collisionFilter.maskBits; }

    /**
     * Set the group of body.
     *
     * Collision groups let you specify an integral group index. You can have all fixtures with the same group index
     * always collide (positive index) or never collide (negative index).
     * @param groupIndex An integer number, it have high priority than bit masks.
     */
    void setGroup(int groupIndex);

    /**
     * Get the group of body.
     *
     * @return An integer number.
     */
    int getGroup() const { return _collisionFilter.groupIndex; }

    void deatchFromBody();

    // A attached collider can't be re-attach to rigibody again
    bool isAttached() const;

protected:
    void applyFilter();
    virtual bool attachToBody(Rigidbody2D* body) = 0;

    /** calculate the area of this shape */
    virtual float calculateArea() { return 0.0f; }

    virtual void setScale(float scaleX, float scaleY);
    virtual void updateScale();

    void updatePolyScale();

    void addShape(b2ShapeId shape);

    Collider2D(const PhysicsMaterial2D& material);
    virtual ~Collider2D() = 0;

    Rigidbody2D* _attachedBody{nullptr};

    tlx::pod_vector<b2ShapeId> _b2Shapes;

    Type _type{Type::UNKNOWN};
    float _area;
    bool _sensor;
    float _scaleX;
    float _scaleY;
    float _newScaleX;
    float _newScaleY;
    PhysicsMaterial2D _material;
    int _tag;

    CollisionFilter _collisionFilter;

    uint64_t _contactMaskBits;

    friend class PhysicsWorld2D;
    friend class Rigidbody2D;
    friend class Joint2D;
};

/** A circle shape. */
class AX_DLL CircleCollider2D : public Collider2D
{
public:
    /**
     * Creates a CircleCollider2D with specified value.
     *
     * @param   radius A float number, it is the circle's radius.
     * @param   material A PhysicsMaterial2D object, the default value is PHYSICS_MATERIAL_2D_DEFAULT.
     * @param   offset A Vec2 object, it is the offset from the body's center of gravity in body local coordinates.
     * @return  An autoreleased CircleCollider2D object pointer.
     */
    static CircleCollider2D* create(float radius,
                                    const PhysicsMaterial2D& material = PHYSICS_MATERIAL_2D_DEFAULT,
                                    const Vec2& offset                = Vec2(0.0f, 0.0f));

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
    bool attachToBody(Rigidbody2D*) override;

    float calculateArea() override;
    void updateScale() override;

protected:
    CircleCollider2D(const PhysicsMaterial2D& material, float radius, const Vec2& offset)
        : Collider2D(material), _radius(radius), _offset(offset)
    {}

    float _radius{0};
    Vec2 _offset;
};

/** A polygon shape. */
class AX_DLL PolygonCollider2D : public Collider2D
{
public:
    /**
     * Creates a PolygonCollider2D with specified value.
     *
     * @param   points A Vec2 object pointer, it is an array of Vec2.
     * @param   material A PhysicsMaterial2D object, the default value is PHYSICS_MATERIAL_2D_DEFAULT.
     * @param   offset A Vec2 object, it is the offset from the body's center of gravity in body local coordinates.
     * @return  An autoreleased PolygonCollider2D object pointer.
     */
    static PolygonCollider2D* create(std::span<const Vec2> points,
                                     const PhysicsMaterial2D& material = PHYSICS_MATERIAL_2D_DEFAULT,
                                     const Vec2& offset                = Vec2::ZERO,
                                     float radius                      = 0.0f);

    /**
     * Calculate the area of a polygon with specified value.
     *
     * @param   points A Vec2 object pointer, it is an array of Vec2.
     * @return A float number.
     */
    static float calculateArea(std::span<const Vec2> points);

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
    bool attachToBody(Rigidbody2D*) override;
    float calculateArea() override;
    void updateScale() override;

protected:
    PolygonCollider2D(const PhysicsMaterial2D& material, std::span<const Vec2> points, const Vec2& offset, float radius)
        : Collider2D(material), _points(points.begin(), points.end()), _offset(offset), _radius(radius)
    {}

    std::vector<Vec2> _points;
    Vec2 _offset;
    float _radius{0};
};

/** A box shape. */
class AX_DLL BoxCollider2D : public Collider2D
{
public:
    /**
     * Creates a BoxCollider2D with specified value.
     *
     * @param   size The size contains this box's width and height.
     * @param   material A PhysicsMaterial2D object, the default value is PHYSICS_MATERIAL_2D_DEFAULT.
     * @param   offset A Vec2 object, it is the offset from the body's center of gravity in body local coordinates.
     * @return  An autoreleased BoxCollider2D object pointer.
     */
    static BoxCollider2D* create(const Vec2& size,
                                 const PhysicsMaterial2D& material = PHYSICS_MATERIAL_2D_DEFAULT,
                                 const Vec2& offset                = Vec2::ZERO,
                                 float radius                      = 0.0f);

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
    bool attachToBody(Rigidbody2D*) override;
    void updateScale() override;

    BoxCollider2D(const PhysicsMaterial2D& material,
                  const Vec2& size,
                  const Vec2& offset = Vec2::ZERO,
                  float radius       = 0.0f)
        : Collider2D(material), _size(size), _offset(offset), _radius(radius)
    {}

    Vec2 _size;
    Vec2 _offset;
    float _radius{0};
};

/** A segment shape. */
class AX_DLL EdgeSegmentCollider2D : public Collider2D
{
public:
    /**
     * Creates a EdgeSegmentCollider2D with specified value.
     *
     * @param   a It's the edge's begin position.
     * @param   b It's the edge's end position.
     * @param   material A PhysicsMaterial2D object, the default value is PHYSICS_MATERIAL_2D_DEFAULT.
     * @return  An autoreleased EdgeSegmentCollider2D object pointer.
     */
    static EdgeSegmentCollider2D* create(const Vec2& a,
                                         const Vec2& b,
                                         const PhysicsMaterial2D& material = PHYSICS_MATERIAL_2D_DEFAULT);

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
    bool attachToBody(Rigidbody2D*) override;
    void updateScale() override;

protected:
    EdgeSegmentCollider2D(const PhysicsMaterial2D& material, const Vec2& a, const Vec2& b)
        : Collider2D(material), _pointA(a), _pointB(b)
    {}

    Vec2 _pointA;
    Vec2 _pointB;
};

/** An edge polygon shape. */
class AX_DLL EdgePolygonCollider2D : public Collider2D
{
public:
    /**
     * Creates a EdgePolygonCollider2D with specified value.
     *
     * @param   points A Vec2 object pointer, it contains an array of points.
     * @param   material A PhysicsMaterial2D object, the default value is PHYSICS_MATERIAL_2D_DEFAULT.
     * @return  An autoreleased EdgePolygonCollider2D object pointer.
     */
    static EdgePolygonCollider2D* create(std::span<const Vec2> points,
                                         const PhysicsMaterial2D& material = PHYSICS_MATERIAL_2D_DEFAULT);

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
    bool attachToBody(Rigidbody2D*) override;

    void updateScale() override;

protected:
    EdgePolygonCollider2D(const PhysicsMaterial2D& material, std::span<const Vec2> points)
        : Collider2D(material), _points(points.begin(), points.end())
    {}

    std::vector<Vec2> _points;
};

/** An edge box shape. */
class AX_DLL EdgeBoxCollider2D : public Collider2D
{
public:
    /**
     * Creates a EdgeBoxCollider2D with specified value.
     *
     * @param   size The size contains this box's width and height.
     * @param   material A PhysicsMaterial2D object, the default value is PHYSICS_MATERIAL_2D_DEFAULT.
     * @param   offset A Vec2 object, it is the offset from the body's center of gravity in body local coordinates.
     * @return  An autoreleased EdgeBoxCollider2D object pointer.
     */
    static EdgeBoxCollider2D* create(const Vec2& size,
                                     const PhysicsMaterial2D& material = PHYSICS_MATERIAL_2D_DEFAULT,
                                     const Vec2& offset                = Vec2::ZERO);

    /**
     * Get this box's position offset.
     *
     * @return A Vec2 object.
     */
    Vec2 getOffset() override { return getCenter(); }

protected:
    bool attachToBody(Rigidbody2D*) override;

protected:
    EdgeBoxCollider2D(const PhysicsMaterial2D& material, const Vec2& size, const Vec2& offset)
        : Collider2D(material), _size(size), _offset(offset)
    {}

    Vec2 _size;
    Vec2 _offset;
};

/** A chain shape. */
class AX_DLL EdgeChainCollider2D : public Collider2D
{
public:
    /**
     * Creates a EdgeChainCollider2D with specified value.
     *
     * @param   points A Vec2 object pointer, it contains an array of points.
     * @param   material A PhysicsMaterial2D object, the default value is PHYSICS_MATERIAL_2D_DEFAULT.
     * @return  An autoreleased EdgeChainCollider2D object pointer.
     */
    static EdgeChainCollider2D* create(std::span<const Vec2> points,
                                       const PhysicsMaterial2D& material = PHYSICS_MATERIAL_2D_DEFAULT);

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
    bool attachToBody(Rigidbody2D*) override;
    void updateScale() override;

protected:
    EdgeChainCollider2D(const PhysicsMaterial2D& material, std::span<const Vec2> points)
        : Collider2D(material), _points(points.begin(), points.end())
    {}

    std::vector<Vec2> _points;
};

/** @} */
/** @} */

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS_2D)
