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

#    include <list>
#    include "axmol/base/Vector.h"
#    include "axmol/math/Math.h"
#    include "axmol/physics2d/Rigidbody2D.h"

namespace ax
{

class Rigidbody2D;
class Joint2D;
class Collider2D;
class Contact2D;

class Director;
class Node;
class Sprite;
class Scene;
class DrawNode;
class EventDispatcher;

class PhysicsWorld2D;

typedef struct PhysicsRayCastInfo
{
    Collider2D* collider;
    Vec2 start;
    Vec2 end;  ///< in lua, it's name is "ended"
    Vec2 contact;
    Vec2 normal;

    // FIXME: correct thing to do is use `cpFloat` instead of float.
    // but in order to do so, we should include "chipmunk_types.h"
    // in Chipmunk v7.0, chipmunk_types includes all the mac types that
    // conflicts with cocos2d Vec2, Point,... etc types. And all the CocosStudio
    // lib will need to use the `ax::` namespace prefix. And it is easier to do this
    // than change all the cocosstudio library (and also users code)
    float fraction;
    void* data;
} PhysicsRayCastInfo;

/**
 * @brief Called for each fixture found in the query. You control how the ray cast
 * proceeds by returning a float:
 * return true: continue
 * return false: terminate the ray cast
 * @param fixture the fixture hit by the ray
 * @param point the point of initial intersection
 * @param normal the normal vector at the point of intersection
 * @return true to continue, false to terminate
 */
using PhysicsRayCastCallback   = std::function<bool(PhysicsWorld2D& world, const PhysicsRayCastInfo& info, void* data)>;
using PhysicsQueryRectCallback = std::function<bool(PhysicsWorld2D&, Collider2D&, void*)>;
using PhysicsQueryPointCallback = PhysicsQueryRectCallback;

/**
 * @addtogroup physics
 * @{
 * @addtogroup physics_2d
 * @{
 */

/**
 * @class PhysicsWorld2D CCPhysicsWorld.h
 * @brief An PhysicsWorld2D object simulates collisions and other physical properties. You do not create PhysicsWorld2D
 * objects directly; instead, you can get it from an Scene object.
 */
class AX_DLL PhysicsWorld2D
{
    friend class Node;
    friend class Sprite;
    friend class Scene;
    friend class Director;
    friend class Rigidbody2D;
    friend class Collider2D;
    friend class Joint2D;
    friend struct PhysicsWorldCallback;

protected:
    PhysicsWorld2D();
    virtual ~PhysicsWorld2D();
    static PhysicsWorld2D* obtain(Scene* scene);
    bool init(Scene* scene);

public:
    static const int DEBUGDRAW_NONE;     ///< draw nothing
    static const int DEBUGDRAW_SHAPE;    ///< draw shapes
    static const int DEBUGDRAW_JOINT;    ///< draw joints
    static const int DEBUGDRAW_CONTACT;  ///< draw contact
    static const int DEBUGDRAW_ALL;      ///< draw all

    b2WorldId internalHandle() const { return _b2World; }

    /**
     * Searches for physics shapes that intersects the ray.
     *
     * Query this physics world along the line segment from start to end.
     * @param   func   Func is called for each shape found.
     * @param   start   A Vec2 object contains the begin position of the ray.
     * @param   end   A Vec2 object contains the end position of the ray.
     * @param   data   User defined data, it is passed to func.
     */
    [[internal]] void rayCast(PhysicsRayCastCallback func, const Vec2& start, const Vec2& end, void* data);

    /**f
     * Enumerates all physics shapes whose bounding box overlaps the specified rectangle.
     *
     * Queries the physics world for shapes intersecting the given rect.
     * For each overlapping shape, the provided callback function is invoked.
     *
     * @param func  Callback function executed for each overlapping shape.
     * @param rect  Rectangle region (x, y, width, height) used for overlap testing.
     * @param data  User-defined data passed to the callback.
     */
    [[internal]] void overlapBox(PhysicsQueryRectCallback func, const Rect& rect, void* data);

    /**
     * Returns the nearest physics shape whose bounding box overlaps the specified rectangle.
     *
     * Queries the physics world at the given rect and finds the closest shape
     * that intersects it. If no shape overlaps the rect, nullptr is returned.
     *
     * @param rect  Rectangle region (x, y, width, height) used for overlap testing.
     * @return      Pointer to the nearest Collider2D overlapping the rect,
     *              or nullptr if none were found.
     */
    Collider2D* overlapBox(const Rect& rect) const;

    /**
     * Returns all physics shapes whose bounding box overlaps the specified rectangle.
     *
     * Queries the physics world and collects every shape intersecting the given rect
     * into a vector.
     *
     * @param rect  Rectangle region (x, y, width, height) used for overlap testing.
     * @return      A vector of Collider2D pointers representing all shapes
     *              overlapping the rect.
     */
    Vector<Collider2D*> overlapBoxAll(const Rect& rect) const;

    /**
     * Enumerates all physics shapes that contain the specified point.
     *
     * Queries the physics world for shapes covering the given point.
     * For each shape that contains the point, the provided callback function is invoked.
     *
     * @attention The point must lie inside at least one shape to trigger callbacks.
     *
     * @param func   Callback function executed for each shape containing the point.
     * @param point  Position of the point to test.
     * @param data   User-defined data passed to the callback.
     */
    [[internal]] void overlapPoint(PhysicsQueryPointCallback func, const Vec2& point, void* data);

    /**
     * Returns the nearest physics shape that contains the specified point.
     *
     * Queries the physics world at the given point and finds the closest shape
     * that covers it. If no shape contains the point, nullptr is returned.
     *
     * @param point  Position of the point to test.
     * @return       Pointer to the nearest Collider2D containing the point,
     *               or nullptr if none were found.
     */
    Collider2D* overlapPoint(const Vec2& point) const;

    /**
     * Returns all physics shapes that contain the specified point.
     *
     * Queries the physics world and collects every shape covering the given point
     * into a vector.
     *
     * @attention The point must lie inside at least one shape to produce results.
     *
     * @param point  Position of the point to test.
     * @return       A vector of Collider2D pointers representing all shapes
     *               that contain the point.
     */
    Vector<Collider2D*> overlapPointAll(const Vec2& point) const;

    /**
     * Get a scene contain this physics world.
     *
     * @attention This value is initialized in constructor
     * @return A Scene object reference.
     */
    Scene* getScene() const { return _scene; }

    /**
     * Get the gravity value of this physics world.
     *
     * @return A Vec2 object.
     */
    Vec2 getGravity() const { return _gravity; }

    /**
     * set the gravity value of this physics world.
     *
     * @param gravity A gravity value of this physics world.
     */
    void setGravity(const Vec2& gravity);

    /**
     * set the slop and bias value of this physics world.
     *
     * @param slop Amount of encouraged penetration between colliding shapes. Used to reduce oscillating contacts and
     * keep the collision cache warm.
     * @param bias Determines how fast overlapping shapes are pushed apart. Expressed as a fraction of the error
     * remaining after each second. Defaults to pow(1.0 - 0.1, 60.0) meaning that Chipmunk fixes 10% of overlap each
     * frame at 60Hz.
     */
    void setSlopBias(float slop, float bias);

    /**
     * Set the speed of this physics world.
     *
     * @attention if you setAutoStep(false), this won't work.
     * @param speed  A float number. Speed is the rate at which the simulation executes. default value is 1.0.
     */
    void setSpeed(float speed)
    {
        if (speed >= 0.0f)
        {
            _speed = speed;
        }
    }

    /**
     * Get the speed of this physics world.
     *
     * @return A float number.
     */
    float getSpeed() { return _speed; }

    /**
     * Set the update rate of this physics world
     *
     * Update rate is the value of EngineUpdateTimes/PhysicsWorldUpdateTimes.
     * Set it higher can improve performance, set it lower can improve accuracy of physics world simulation.
     * @attention if you setAutoStep(false), this won't work.
     * @param rate An integer number, default value is 1.0.
     */
    void setUpdateRate(int rate)
    {
        if (rate > 0)
        {
            _updateRate = rate;
        }
    }

    /**
     * Get the update rate of this physics world.
     *
     * @return An integer number.
     */
    int getUpdateRate() { return _updateRate; }

    /**
     * set the number of substeps in an update of the physics world.
     *
     * One physics update will be divided into several substeps to increase its accuracy.
     * @param steps An integer number, default value is 1.
     */
    void setSubsteps(int steps);

    /**
     * Get the number of substeps of this physics world.
     *
     * @return An integer number.
     */
    int getSubsteps() const { return _substeps; }

    /**
     * set the number of update of the physics world in a second.
     * 0 - disable fixed step system
     * default value is 0
     */
    void setFixedUpdateRate(int updatesPerSecond)
    {
        if (updatesPerSecond > 0)
        {
            _fixedUpdateRate = updatesPerSecond;
        }
    }
    /** get the number of substeps */
    int getFixedUpdateRate() const { return _fixedUpdateRate; }

    /**
     * set the callback which invoked before update of each object in physics world.
     */
    void setPreUpdateCallback(const std::function<void()>& callback);

    /**
     * set the callback which invoked after update of each object in physics world.
     */
    void setPostUpdateCallback(const std::function<void()>& callback);

    /**
     * To control the step of physics.
     *
     * If you want control it by yourself( fixed-timestep for example ), you can set this to false and call step by
     * yourself.
     * @attention If you set auto step to false, setSpeed setSubsteps and setUpdateRate won't work, you need to control
     * the time step by yourself.
     * @param autoStep A bool object, default value is true.
     */
    void setAutoStep(bool autoStep) { _autoStep = autoStep; }

    /**
     * Get the auto step of this physics world.
     *
     * @return A bool object.
     */
    bool isAutoStep() { return _autoStep; }

    /**
     * The step for physics world.
     *
     * The times passing for simulate the physics.
     * @attention You need to setAutoStep(false) first before it can work.
     * @param   delta   A float number.
     */
    void step(float delta);

protected:
    virtual void update(float delta, bool userCall = false);

    virtual bool collisionBeginCallback(Contact2D& contact);
    virtual bool collisionPreSolveCallback(Contact2D& contact);
    virtual void collisionPostSolveCallback(Contact2D& contact);
    virtual void collisionSeparateCallback(Contact2D& contact);

    void beforeSimulation(Node* node,
                          const Mat4& parentToWorldTransform,
                          float nodeParentScaleX,
                          float nodeParentScaleY,
                          float parentRotation);
    void afterSimulation(Node* node, const Mat4& parentToWorldTransform, float parentRotation);

protected:
    Vec2 _gravity;
    float _PTMRatio;
    float _speed;
    int _updateRate;
    int _updateRateCount;
    float _updateTime;
    int _substeps;
    int _fixedUpdateRate;
    b2WorldId _b2World;
    bool _isWorldLocked = false;

    bool _updateBodyTransform;
    Scene* _scene;

    bool _autoStep;

    EventDispatcher* _eventDispatcher;

    std::function<void()> _preUpdateCallback;
    std::function<void()> _postUpdateCallback;
};

/** @} */
/** @} */

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS_2D)
