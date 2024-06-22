/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
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

#ifndef __CCSCENE_H__
#define __CCSCENE_H__

#include <string>
#include "2d/Node.h"

NS_AX_BEGIN

class Camera;
class BaseLight;
class Renderer;
class EventListenerCustom;
class EventCustom;
#if defined(AX_ENABLE_PHYSICS)
class PhysicsWorld;
#endif
#if defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION
class Physics3DWorld;
#endif
#if defined(AX_ENABLE_NAVMESH)
class NavMesh;
#endif

/**
 * @addtogroup _2d
 * @{
 */

/** @class Scene
* @brief Scene is a subclass of Node that is used only as an abstract concept.

Scene and Node are almost identical with the difference that Scene has its
anchor point (by default) at the center of the screen.

For the moment Scene has no other logic than that, but in future releases it might have
additional logic.

It is a good practice to use a Scene as the parent of all your nodes.

Scene will create a default camera for you.
*/
class AX_DLL Scene : public Node
{
public:
    /** Creates a new Scene object.
     *
     * @return An autoreleased Scene object.
     */
    static Scene* create();

    /** Creates a new Scene object with a predefined size.
     *
     * @param size The predefined size of scene.
     * @return An autoreleased Scene object.
     * @js NA
     */
    static Scene* createWithSize(const Vec2& size);

    using Node::addChild;
    virtual std::string getDescription() const override;

    /** Get all cameras.
     *
     * @return The vector of all cameras, ordered by camera depth.
     * @js NA
     */
    const std::vector<Camera*>& getCameras();

    /** Get the default camera.
     * @js NA
     * @return The default camera of scene.
     */
    Camera* getDefaultCamera() const { return _defaultCamera; }

    /** Get lights.
     * @return The vector of lights.
     * @js NA
     */
    const std::vector<BaseLight*>& getLights() const { return _lights; }

    /** Render the scene.
     * @param renderer The renderer use to render the scene.
     * @param eyeTransform The AdditionalTransform of camera.
     * @param eyeProjection The projection matrix of camera.
     * @js NA
     */
    virtual void render(Renderer* renderer, const Mat4& eyeTransform, const Mat4* eyeProjection = nullptr);

    void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override;
    void visit() override;

    /** override function */
    virtual void removeAllChildren() override;

    Scene();
    virtual ~Scene();

    bool init() override;
    bool initWithSize(const Vec2& size);

    void setCameraOrderDirty() { _cameraOrderDirty = true; }

    void onProjectionChanged(EventCustom* event);

private:
    void initDefaultCamera();

protected:
    friend class Node;
    friend class ProtectedNode;
    friend class SpriteBatchNode;
    friend class Camera;
    friend class BaseLight;
    friend class Renderer;

    std::vector<Camera*> _cameras;     // weak ref to Camera

    /* weak ref, default camera created by scene, at _cameras[0], Caution! the default camera can not be added to _cameras
     before onEnter is called. */
    Camera* _defaultCamera = nullptr;
    /* indicates if the order is dirty and if so then it needs sorting */
    bool _cameraOrderDirty = true;
    EventListenerCustom* _event;

    std::vector<BaseLight*> _lights;

private:
    AX_DISALLOW_COPY_AND_ASSIGN(Scene);

#if (AX_ENABLE_PHYSICS || (defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION))
public:
#    if defined(AX_ENABLE_PHYSICS)
    /** Get the physics world of the scene.
     * @return The physics world of the scene.
     * @js NA
     */
    PhysicsWorld* getPhysicsWorld() const { return _physicsWorld; }
#    endif

#    if defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION
    /** Get the 3d physics world of the scene.
     * @return The 3d physics world of the scene.
     * @js NA
     */
    Physics3DWorld* getPhysics3DWorld() { return _physics3DWorld; }

    /**
     * Set Physics3D debug draw camera.
     */
    void setPhysics3DDebugCamera(Camera* camera);
#    endif

    /** Create a scene with physics.
     * @return An autoreleased Scene object with physics.
     * @js NA
     */
    static Scene* createWithPhysics();

    bool initWithPhysics();
    bool initPhysicsWorld();
    virtual void fixedUpdate(float delta) {}

protected:
    void addChildToPhysicsWorld(Node* child);

#    if defined(AX_ENABLE_PHYSICS)
    PhysicsWorld* _physicsWorld = nullptr;
#    endif

#    if defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION
    Physics3DWorld* _physics3DWorld = nullptr;
    Camera* _physics3dDebugCamera   = nullptr;
#    endif
#endif  // (defined(AX_ENABLE_PHYSICS) || defined(AX_ENABLE_3D_PHYSICS))

#if defined(AX_ENABLE_NAVMESH)
public:
    /** set navigation mesh */
    void setNavMesh(NavMesh* navMesh);
    /** get navigation mesh */
    NavMesh* getNavMesh() const { return _navMesh; }
    /**
     * Set NavMesh debug draw camera.
     */
    void setNavMeshDebugCamera(Camera* camera);

protected:
    NavMesh* _navMesh           = nullptr;
    Camera* _navMeshDebugCamera = nullptr;
#endif

#if (defined(AX_ENABLE_PHYSICS) || (defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION) || defined(AX_ENABLE_NAVMESH))
public:
    void stepPhysicsAndNavigation(float deltaTime);
#endif
};

// end of _2d group
/// @}

NS_AX_END

#endif  // __CCSCENE_H__
