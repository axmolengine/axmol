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

#include "2d/Scene.h"
#include "base/Director.h"
#include "2d/Camera.h"
#include "base/EventDispatcher.h"
#include "base/EventListenerCustom.h"
#include "base/UTF8.h"
#include "renderer/Renderer.h"

#if defined(AX_ENABLE_PHYSICS)
#    include "physics/PhysicsWorld.h"
#endif

#if defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION
#    include "physics3d/Physics3DWorld.h"
#    include "physics3d/Physics3DComponent.h"
#endif

#if defined(AX_ENABLE_NAVMESH)
#    include "navmesh/NavMesh.h"
#endif

namespace ax
{

Scene::Scene()
    : _event(_director->getEventDispatcher()->addCustomEventListener(
          Director::EVENT_PROJECTION_CHANGED,
          std::bind(&Scene::onProjectionChanged, this, std::placeholders::_1)))
{
    _event->retain();

    _ignoreAnchorPointForPosition = true;
    setAnchorPoint(Vec2(0.5f, 0.5f));

    Camera::_visitingCamera = nullptr;
}

Scene::~Scene()
{
#if defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION
    AX_SAFE_RELEASE(_physics3DWorld);
    AX_SAFE_RELEASE(_physics3dDebugCamera);
#endif
#if defined(AX_ENABLE_NAVMESH)
    AX_SAFE_RELEASE(_navMesh);
#endif
    _director->getEventDispatcher()->removeEventListener(_event);
    AX_SAFE_RELEASE(_event);

#if defined(AX_ENABLE_PHYSICS)
    delete _physicsWorld;
#endif

#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
    if (sEngine)
    {
        sEngine->releaseAllChildrenRecursive(this);
    }
#endif  // AX_ENABLE_GC_FOR_NATIVE_OBJECTS
}

#if defined(AX_ENABLE_NAVMESH)
void Scene::setNavMesh(NavMesh* navMesh)
{
    if (_navMesh != navMesh)
    {
        AX_SAFE_RETAIN(navMesh);
        AX_SAFE_RELEASE(_navMesh);
        _navMesh = navMesh;
    }
}
#endif

bool Scene::init()
{
    auto size = _director->getWinSize();
    return initWithSize(size);
}

bool Scene::initWithSize(const Vec2& size)
{
    initDefaultCamera();
    setContentSize(size);
    return true;
}

void Scene::initDefaultCamera()
{
    if (!_defaultCamera)
    {
        _defaultCamera = Camera::create();
        addChild(_defaultCamera);
    }
}

Scene* Scene::create()
{
    Scene* ret = new Scene();
    if (ret->init())
    {
        ret->autorelease();
        return ret;
    }
    else
    {
        AX_SAFE_DELETE(ret);
        return nullptr;
    }
}

Scene* Scene::createWithSize(const Vec2& size)
{
    Scene* ret = new Scene();
    if (ret->initWithSize(size))
    {
        ret->autorelease();
        return ret;
    }
    else
    {
        AX_SAFE_DELETE(ret);
        return nullptr;
    }
}

std::string Scene::getDescription() const
{
    return fmt::format("<Scene | tag = {}>", _tag);
}

void Scene::onProjectionChanged(EventCustom* /*event*/)
{
    if (_defaultCamera)
    {
        _defaultCamera->initDefault();
    }
}

static bool camera_cmp(const Camera* a, const Camera* b)
{
    return a->getRenderOrder() < b->getRenderOrder();
}

const std::vector<Camera*>& Scene::getCameras()
{
    if (_cameraOrderDirty)
    {
        stable_sort(_cameras.begin(), _cameras.end(), camera_cmp);
        _cameraOrderDirty = false;
    }
    return _cameras;
}

void Scene::render(Renderer* renderer, const Mat4& eyeTransform, const Mat4* eyeProjection)
{
    Camera* defaultCamera = nullptr;
    const auto& transform = getNodeToParentTransform();

    for (const auto& camera : getCameras())
    {
        if (!camera->isVisible())
            continue;

        Camera::_visitingCamera = camera;
        if (Camera::_visitingCamera->getCameraFlag() == CameraFlag::DEFAULT)
        {
            defaultCamera = Camera::_visitingCamera;
        }

        // There are two ways to modify the "default camera" with the eye Transform:
        // a) modify the "nodeToParentTransform" matrix
        // b) modify the "additional transform" matrix
        // both alternatives are correct, if the user manually modifies the camera with a camera->setPosition()
        // then the "nodeToParent transform" will be lost.
        // And it is important that the change is "permanent", because the matrix might be used for calculate
        // culling and other stuff.
        if (eyeProjection)
            camera->setAdditionalProjection(*eyeProjection * camera->getProjectionMatrix().getInversed());

        camera->setAdditionalTransform(eyeTransform.getInversed());
        _director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
        _director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION,
                              Camera::_visitingCamera->getViewProjectionMatrix());

        camera->apply();
        // clear background with max depth
        camera->clearBackground();
        // visit the scene
        visit(renderer, transform, 0);
#if defined(AX_ENABLE_NAVMESH)
        if (_navMesh && _navMeshDebugCamera == camera)
        {
            _navMesh->debugDraw(renderer);
        }
#endif

        renderer->render();

        _director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

        // we shouldn't restore the transform matrix since it could be used
        // from "update" or other parts of the game to calculate culling or something else.
        //        camera->setNodeToParentTransform(eyeCopy);
    }

#if defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION
    if (_physics3DWorld && _physics3DWorld->isDebugDrawEnabled())
    {
        Camera* physics3dDebugCamera = _physics3dDebugCamera != nullptr ? _physics3dDebugCamera : defaultCamera;

        if (eyeProjection)
            physics3dDebugCamera->setAdditionalProjection(*eyeProjection *
                                                          physics3dDebugCamera->getProjectionMatrix().getInversed());

        physics3dDebugCamera->setAdditionalTransform(eyeTransform.getInversed());
        _director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
        _director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION,
                              physics3dDebugCamera->getViewProjectionMatrix());

        physics3dDebugCamera->apply();
        physics3dDebugCamera->clearBackground();

        _physics3DWorld->debugDraw(renderer);
        renderer->render();

        _director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    }
#endif

    Camera::_visitingCamera = nullptr;
}

void Scene::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    Node::visit(renderer, parentTransform, parentFlags);
}

void Scene::visit()
{
    const auto eyeTransform = Mat4::IDENTITY;

    for (const auto& camera : getCameras())
    {
        if (!camera->isVisible())
            continue;

        Camera::_visitingCamera = camera;

        camera->setAdditionalTransform(eyeTransform.getInversed());
        _director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
        _director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION,
                              Camera::_visitingCamera->getViewProjectionMatrix());

        camera->apply();
        // clear background with max depth
        camera->clearBackground();
        // visit the scene
        Node::visit();

        _director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    }

    Camera::_visitingCamera = nullptr;
}

void Scene::removeAllChildren()
{
    if (_defaultCamera)
        _defaultCamera->retain();

    Node::removeAllChildren();

    if (_defaultCamera)
    {
        addChild(_defaultCamera);
        _defaultCamera->release();
    }
}

#if defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION
void Scene::setPhysics3DDebugCamera(Camera* camera)
{
    AX_SAFE_RETAIN(camera);
    AX_SAFE_RELEASE(_physics3dDebugCamera);
    _physics3dDebugCamera = camera;
}
#endif

#if defined(AX_ENABLE_NAVMESH)
void Scene::setNavMeshDebugCamera(Camera* camera)
{
    AX_SAFE_RETAIN(camera);
    AX_SAFE_RELEASE(_navMeshDebugCamera);
    _navMeshDebugCamera = camera;
}

#endif

#if (defined(AX_ENABLE_PHYSICS) || (defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION))

Scene* Scene::createWithPhysics()
{
    Scene* ret = new Scene();
    if (ret->initWithPhysics())
    {
        ret->autorelease();
        return ret;
    }
    else
    {
        AX_SAFE_DELETE(ret);
        return nullptr;
    }
}

bool Scene::initWithPhysics()
{
    initDefaultCamera();
    return initPhysicsWorld();
}

bool Scene::initPhysicsWorld()
{
#    if defined(AX_ENABLE_PHYSICS)
    _physicsWorld = PhysicsWorld::construct(this);
#    endif

    bool ret = false;
    do
    {
        this->setContentSize(_director->getWinSize());

#    if defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION
        Physics3DWorldDes info;
        AX_BREAK_IF(!(_physics3DWorld = Physics3DWorld::create(&info)));
        _physics3DWorld->retain();
#    endif

        // success
        ret = true;
    } while (0);
    return ret;
}

#endif

#if (defined(AX_ENABLE_PHYSICS) || (defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION) || defined(AX_ENABLE_NAVMESH))
void Scene::stepPhysicsAndNavigation(float deltaTime)
{
#    if defined(AX_ENABLE_PHYSICS)
    if (_physicsWorld && _physicsWorld->isAutoStep())
        _physicsWorld->update(deltaTime);
#    endif

#    if defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION
    if (_physics3DWorld)
    {
        _physics3DWorld->stepSimulate(deltaTime);
    }
#    endif
#    if defined(AX_ENABLE_NAVMESH)
    if (_navMesh)
    {
        _navMesh->update(deltaTime);
    }
#    endif
}
#endif

}
