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

#include "base/Config.h"
#if defined(AX_ENABLE_PHYSICS)

#    include "2d/Component.h"
#    include "math/Math.h"
#    include "physics/PhysicsCollider.h"
#    include "base/Vector.h"

namespace ax
{

class Node;
class PhysicsWorld;
class PhysicsJoint;

const PhysicsMaterial PHYSICSBODY_MATERIAL_DEFAULT(0.1f, 0.5f, 0.5f);

/**
 * @addtogroup physics
 * @{
 * @addtogroup physics_2d
 * @{
 */

/**
 * A body affect by physics.
 *
 * It can attach one or more colliders.
 * If you create body with createXXX, it will automatically compute mass and moment with density your specified(which is
 * PHYSICSBODY_MATERIAL_DEFAULT by default, and the density value is 0.1f), and it based on the formula: mass = density
 * * area. If you create body with createEdgeXXX, the mass and moment will be PHYSICS_INFINITY by default. And it's a
 * static body. 
 * or static by use function setDynamic().
 * Note: box2d not allow change mass and moment dynamic
 */
class AX_DLL PhysicsBody : public Component
{
public:
    const static std::string COMPONENT_NAME;

    /**
     * Create a rigibody.
     *
     * The mass and moment automaticall computed by box2d internal
     * @return  An autoreleased PhysicsBody object pointer.
     */
    static PhysicsBody* create();

    /**
     * Create a body contains a circle.
     *
     * @param   radius A float number, it is the circle's radius.
     * @param   material A PhysicsMaterial object, the default value is PHYSICSSHAPE_MATERIAL_DEFAULT.
     * @param   offset A Vec2 object, it is the offset from the body's center of gravity in body local coordinates.
     * @return  An autoreleased PhysicsBody object pointer.
     */
    static PhysicsBody* createCircle(float radius,
                                     const PhysicsMaterial& material = PHYSICSBODY_MATERIAL_DEFAULT,
                                     const Vec2& offset              = Vec2::ZERO);
    /**
     * Create a body contains a box collider.
     *
     * @param   size Size contains this box's width and height.
     * @param   material A PhysicsMaterial object, the default value is PHYSICSSHAPE_MATERIAL_DEFAULT.
     * @param   offset A Vec2 object, it is the offset from the body's center of gravity in body local coordinates.
     * @return  An autoreleased PhysicsBody object pointer.
     */
    static PhysicsBody* createBox(const Vec2& size,
                                  const PhysicsMaterial& material = PHYSICSBODY_MATERIAL_DEFAULT,
                                  const Vec2& offset              = Vec2::ZERO);

    /**
     * @brief Create a body contains a polygon collider.
     *
     * @param   points Points is an array of Vec2 structs defining a convex hull with a clockwise winding.
     * @param   count An integer number, contains the count of the points array.
     * @param   material A PhysicsMaterial object, the default value is PHYSICSSHAPE_MATERIAL_DEFAULT.
     * @param   offset A Vec2 object, it is the offset from the body's center of gravity in body local coordinates.
     * @return  An autoreleased PhysicsBody object pointer.
     */
    static PhysicsBody* createPolygon(const Vec2* points,
                                      int count,
                                      const PhysicsMaterial& material = PHYSICSBODY_MATERIAL_DEFAULT,
                                      const Vec2& offset              = Vec2::ZERO);

    /**
     * Create a body contains a EdgeSegment collider.
     *
     * @param   a It's the edge's begin position.
     * @param   b It's the edge's end position.
     * @param   material A PhysicsMaterial object, the default value is PHYSICSSHAPE_MATERIAL_DEFAULT.
     * @param   border It's a edge's border width.
     * @return  An autoreleased PhysicsBody object pointer.
     */
    static PhysicsBody* createEdgeSegment(const Vec2& a,
                                          const Vec2& b,
                                          const PhysicsMaterial& material = PHYSICSBODY_MATERIAL_DEFAULT,
                                          float border                    = 1);

    /**
     * Create a body contains a EdgeBox collider.
     * @param   size The size contains this box's width and height.
     * @param   material A PhysicsMaterial object, the default value is PHYSICSSHAPE_MATERIAL_DEFAULT.
     * @param   border It's a edge's border width.
     * @param   offset A Vec2 object, it is the offset from the body's center of gravity in body local coordinates.
     * @return  An autoreleased PhysicsBody object pointer.
     */
    static PhysicsBody* createEdgeBox(const Vec2& size,
                                      const PhysicsMaterial& material = PHYSICSBODY_MATERIAL_DEFAULT,
                                      float border                    = 1,
                                      const Vec2& offset              = Vec2::ZERO);

    /**
     * Create a body contains a EdgePolygon collider.
     *
     * @param   points Points is an array of Vec2 structs defining a convex hull with a clockwise winding.
     * @param   count An integer number, contains the count of the points array.
     * @param   material A PhysicsMaterial object, the default value is PHYSICSSHAPE_MATERIAL_DEFAULT.
     * @param   border It's a edge's border width.
     * @return  An autoreleased PhysicsBody object pointer.
     */
    static PhysicsBody* createEdgePolygon(const Vec2* points,
                                          int count,
                                          const PhysicsMaterial& material = PHYSICSBODY_MATERIAL_DEFAULT,
                                          float border                    = 1);

    /**
     * Create a body contains a EdgeChain collider.
     *
     * @param   points A Vec2 object pointer, it contains an array of points.
     * @param   count An integer number, contains the count of the points array.
     * @param   material A PhysicsMaterial object, the default value is PHYSICSSHAPE_MATERIAL_DEFAULT.
     * @param   border It's a edge's border width.
     * @return  An autoreleased PhysicsBody object pointer.
     */
    static PhysicsBody* createEdgeChain(const Vec2* points,
                                        int count,
                                        const PhysicsMaterial& material = PHYSICSBODY_MATERIAL_DEFAULT,
                                        float border                    = 1);

    /**
     * @brief Add a collider to body.
     * @param collider The collider to be added.
     * @param addMassAndMoment If this is true, the collider's mass and moment will be added to body. The default is true.
     * @return This collider's pointer if added success or nullptr if failed.
     */
    virtual PhysicsCollider* addCollider(PhysicsCollider* collider, bool addMassAndMoment = true);

    /**
     * @brief Remove a collider from body.
     * @param collider the collider to be removed.
     * @param reduceMassAndMoment If this is true, the body mass and moment will be reduced by collider. The default is
     * true.
     */
    void removeCollider(PhysicsCollider* collider, bool reduceMassAndMoment = true);

    /**
     * @brief Remove a collider from body.
     * @param tag The tag of the collider to be removed.
     * @param reduceMassAndMoment If this is true, the body mass and moment will be reduced by collider. The default is
     * true.
     */
    void removeCollider(int tag, bool reduceMassAndMoment = true);

    /**
     * Remove all colliders.
     *
     * @param reduceMassAndMoment If this is true, the body mass and moment will be reduced by collider. The default is
     * true.
     */
    void removeAllColliders(bool reduceMassAndMoment = true);

    /**
     * Get the body colliders.
     *
     * @return A Vector<PhysicsCollider*> object contains PhysicsCollider pointer.
     */
    const Vector<PhysicsCollider*>& getColliders() const { return _colliders; }

    /**
     * Get the first collider of the body colliders.
     *
     * @return The first collider in this body.
     */
    PhysicsCollider* getFirstCollider() const { return _colliders.size() >= 1 ? _colliders.at(0) : nullptr; }

    /**
     * get the collider of the body.
     *
     * @param   tag   An integer number that identifies a PhysicsCollider object.
     * @return A PhysicsCollider object pointer or nullptr if no colliders were found.
     */
    PhysicsCollider* getCollider(int tag) const;

    /**
     * Applies a continuous force to body.
     *
     * @param force The force is applies to this body.
     * @param offset A Vec2 object, it is the offset from the body's center of gravity in world coordinates.
     */
    virtual void applyForce(const Vec2& force, const Vec2& offset = Vec2::ZERO);

    /**
     * reset all the force applied to body.
     */
    virtual void resetForces();

    /**
     * Applies a immediate force to body.
     *
     * @param impulse The impulse is applies to this body.
     * @param offset A Vec2 object, it is the offset from the body's center of gravity in world coordinates.
     */
    virtual void applyImpulse(const Vec2& impulse, const Vec2& offset = Vec2::ZERO);

    /**
     * Applies a torque force to body.
     *
     * @param torque The torque is applies to this body.
     */
    virtual void applyTorque(float torque);

    /**
     * Set the velocity of a body.
     *
     * @param velocity The velocity is set to this body.
     */
    virtual void setVelocity(const Vec2& velocity);

    /** Get the velocity of a body. */
    virtual Vec2 getVelocity();

    /**
     * Set the angular velocity of a body.
     *
     * @param velocity The angular velocity is set to this body.
     */
    virtual void setAngularVelocity(float velocity);

    /** Get the angular velocity of a body at a local point.*/
    virtual Vec2 getVelocityAtLocalPoint(const Vec2& point);

    /** get the angular velocity of a body at a world point */
    virtual Vec2 getVelocityAtWorldPoint(const Vec2& point);

    /** get the angular velocity of a body */
    virtual float getAngularVelocity();

    /** set the max of velocity */
    virtual void setVelocityLimit(float limit);

    /** get the max of velocity */
    virtual float getVelocityLimit();

    /** set the max of angular velocity */
    virtual void setAngularVelocityLimit(float limit);

    /** get the max of angular velocity */
    virtual float getAngularVelocityLimit();

    /** remove the body from the world it added to */
    void removeFromWorld();

    /** get the world body added to. */
    PhysicsWorld* getWorld() const { return _world; }

    /** get all joints the body have */
    const std::vector<PhysicsJoint*>& getJoints() const { return _joints; }

    /** get the node the body set to. */
    Node* getNode() const { return _owner; }

    /**
     * A mask that defines which categories this physics body belongs to.
     *
     * Every physics body in a scene can be assigned to up to 32 different categories, each corresponding to a bit in
     * the bit mask. You define the mask values used in your game. In conjunction with the collisionBitMask and
     * contactTestBitMask properties, you define which physics bodies interact with each other and when your game is
     * notified of these interactions.
     * @param bitmask An integer number, the default value is 0xFFFFFFFF (all bits set).
     */
    void setCategoryBitmask(int bitmask);

    /**
     * A mask that defines which categories of bodies cause intersection notifications with this physics body.
     *
     * When two bodies share the same space, each body's category mask is tested against the other body's contact mask
     * by performing a logical AND operation. If either comparison results in a non-zero value, an PhysicsContact object
     * is created and passed to the physics world’s delegate. For best performance, only set bits in the contacts mask
     * for interactions you are interested in.
     * @param bitmask An integer number, the default value is 0x00000000 (all bits cleared).
     */
    void setContactTestBitmask(int bitmask);

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
    void setCollisionBitmask(int bitmask);

    /**
     * Return bitmask of first collider.
     *
     * @return If there is no collider in body, return default value.(0xFFFFFFFF)
     */
    int getCategoryBitmask() const;

    /**
     * Return bitmask of first collider.
     *
     * @return If there is no collider in body, return default value.(0x00000000)
     */
    int getContactTestBitmask() const;

    /**
     * Return bitmask of first collider.
     *
     * @return If there is no collider in body, return default value.(0xFFFFFFFF)
     */
    int getCollisionBitmask() const;

    /**
     * Set the group of body.
     *
     * Collision groups let you specify an integral group index. You can have all fixtures with the same group index
     * always collide (positive index) or never collide (negative index). It have high priority than bit masks.
     */
    void setGroup(int group);

    /**
     * Return group of first collider.
     *
     * @return If there is no collider in body, return default value.(0)
     */
    int getGroup() const;

    /** get the body position. */
    Vec2 getPosition() const;

    /** get the body rotation. */
    float getRotation();

    /** set body position offset, it's the position witch relative to node */
    void setPositionOffset(const Vec2& position);

    /** get body position offset. */
    const Vec2& getPositionOffset() const { return _positionOffset; }

    /** set body rotation offset, it's the rotation witch relative to node */
    void setRotationOffset(float rotation);

    /** set the body rotation offset */
    float getRotationOffset() const { return _rotationOffset; }

    /**
     * @brief Test the body is dynamic or not.
     *
     * A dynamic body will effect with gravity.
     */
    bool isDynamic() const { return _dynamic; }
    /**
     * @brief Set dynamic to body.
     *
     * A dynamic body will effect with gravity.
     */
    void setDynamic(bool dynamic);

    /** Get the body mass. */
    float getMass() const;

    /** Get the body moment of inertia. */
    float getMoment() const;

    /** get linear damping. */
    float getLinearDamping() const { return _linearDamping; }

    /**
     * Set linear damping.
     *
     * it is used to simulate fluid or air friction forces on the body.
     * @param damping The value is 0.0f to 1.0f.
     */
    void setLinearDamping(float damping)
    {
        _linearDamping = damping;
        updateDamping();
    }

    /** Get angular damping. */
    float getAngularDamping() const { return _angularDamping; }

    /**
     * Set angular damping.
     *
     * It is used to simulate fluid or air friction forces on the body.
     * @param damping The value is 0.0f to 1.0f.
     */
    void setAngularDamping(float damping)
    {
        _angularDamping = damping;
        updateDamping();
    }

    /** Whether the body is at rest. */
    bool isResting() const;

    /** set body to rest */
    void setResting(bool rest) const;

    /**
     * Set the enable value.
     *
     * If the body it isn't enabled, it will not has simulation by world.
     */
    void setEnabled(bool enable) override;

    /** Whether the body can rotation. */
    bool isRotationEnabled() const { return _rotationEnabled; }

    /** Set the body is allow rotation or not */
    void setRotationEnable(bool enable);

    /** Whether this physics body is affected by the physics world's gravitational force. */
    bool isGravityEnabled() const { return _gravityEnabled; }

    /** Set the body is affected by the physics world's gravitational force or not. */
    void setGravityEnable(bool enable);

    /** Get the body's tag. */
    int getTag() const { return _tag; }

    /** set the body's tag. */
    void setTag(int tag) { _tag = tag; }

    /** Convert the world point to local. */
    Vec2 world2Local(const Vec2& point);

    /** Convert the local point to world. */
    Vec2 local2World(const Vec2& point);

    /** Get the rigid body of chipmunk. */
    b2BodyId getB2Body() const { return _b2Body; }

    /** Set fixed update state */
    void setFixedUpdate(bool fixedUpdate) { _fixedUpdate = fixedUpdate; }

    void onEnter() override;
    void onExit() override;
    void onAdd() override;
    void onRemove() override;

protected:
    PhysicsBody();
    virtual ~PhysicsBody();

    bool init() override;

    virtual void setPosition(float positionX, float positionY);

    virtual void setRotation(float rotation);

    virtual void setScale(float scaleX, float scaleY);

    void update(float delta) override;
    void fixedUpdate(float delta);

    void removeJoint(PhysicsJoint* joint);

    void updateDamping() { _isDamping = _linearDamping != 0.0f || _angularDamping != 0.0f; }

    void addToPhysicsWorld();
    void removeFromPhysicsWorld();

    void beforeSimulation(const Mat4& parentToWorldTransform,
                          const Mat4& nodeToWorldTransform,
                          float scaleX,
                          float scaleY,
                          float rotation);
    void afterSimulation(const Mat4& parentToWorldTransform, float parentRotation);

protected:

    void deatchFromWOrld();

    std::vector<PhysicsJoint*> _joints;
    Vector<PhysicsCollider*> _colliders;
    PhysicsWorld* _world;

    b2BodyId _b2Body;
    bool _dynamic;
    bool _rotationEnabled;
    bool _gravityEnabled;
    float _area;
    float _density;
    float _moment;
    float _velocityLimit;
    float _angularVelocityLimit;
    bool _isDamping;
    float _linearDamping;
    float _angularDamping;

    int _tag;

    Vec2 _positionOffset;
    float _rotationOffset;
    float _recordedRotation;
    float _recordedAngle;

    // offset between owner's center point and down left point
    Vec3 _ownerCenterOffset;
    // offset of owner's center point and anchor point in parent coordinate
    Vec2 _offset;
    float _recordScaleX;
    float _recordScaleY;

    float _recordPosX;
    float _recordPosY;

    // fixed update state
    bool _fixedUpdate;

    friend class PhysicsWorld;
    friend class PhysicsCollider;
    friend class PhysicsJoint;
};

/** @} */
/** @} */

}

#endif  // defined(AX_ENABLE_PHYSICS)
