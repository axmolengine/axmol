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

#include "axmol/scene/Scene.h"

#include <algorithm>

#include "axmol/base/Director.h"
#include "axmol/scene/Camera.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/CustomEventListener.h"
#include "axmol/base/text_utils.h"
#include "axmol/renderer/Renderer.h"
#include "axmol/scene/SceneRenderer.h"

#if defined(AX_ENABLE_PHYSICS_2D)
#    include "axmol/physics/2d/PhysicsWorld2D.h"
#endif

#if defined(AX_ENABLE_PHYSICS_3D)
#    include "axmol/physics/3d/PhysicsWorld3D.h"
#endif

#if defined(AX_ENABLE_NAVMESH)
#    include "axmol/navmesh/NavMesh.h"
#endif

namespace ax
{

namespace
{
bool camera_cmp(const Camera* a, const Camera* b)
{
    return a->getRenderOrder() < b->getRenderOrder();
}
}  // namespace

Scene::Scene()
{
    _event = (_director->getEventDispatcher()->addCustomEventListener(
        Director::EVENT_PROJECTION_CHANGED, std::bind(&Scene::onProjectionChanged, this, std::placeholders::_1)));
    _event->retain();

    _ignoreAnchorPointForPosition = true;
    setAnchorPoint(Vec2(0.5f, 0.5f));

    // Set accumulator to fixedDeltaTime so the next tick will immediately run at least one fixedUpdate,
    // avoiding a stall after changing step size.
    _fixedAccumulator = _fixedDeltaTime;

    Camera::_visitingCamera = nullptr;
}

Scene::~Scene()
{
    AX_SAFE_RELEASE(_debugCamera);

#if defined(AX_ENABLE_PHYSICS_3D)
    PhysicsWorld3D::release(_physicsWorld3D);
#endif
#if defined(AX_ENABLE_NAVMESH)
    AX_SAFE_RELEASE(_navMesh);
#endif
    _director->getEventDispatcher()->removeEventListener(_event);
    AX_SAFE_RELEASE(_event);

#if defined(AX_ENABLE_PHYSICS_2D)
    delete _physicsWorld2D;
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
    auto size = _director->getCanvasSize();
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

void Scene::setCameraOrderDirty()
{
    _cameraOrderDirty = true;
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

void Scene::onProjectionChanged(CustomEvent* /*event*/)
{
    if (_defaultCamera)
    {
        _defaultCamera->initDefault();
    }
}

void Scene::registerCamera(Camera* camera)
{
    if (!camera)
        return;

    auto it = std::find(_cameras.begin(), _cameras.end(), camera);
    if (it == _cameras.end())
    {
        _cameras.emplace_back(camera);
        setCameraOrderDirty();
    }
}

void Scene::unregisterCamera(Camera* camera)
{
    if (!camera)
        return;

    auto it = std::find(_cameras.begin(), _cameras.end(), camera);
    if (it != _cameras.end())
        _cameras.erase(it);
}

const std::vector<Camera*>& Scene::getCameras()
{
    if (_cameraOrderDirty)
    {
        std::stable_sort(_cameras.begin(), _cameras.end(), camera_cmp);
        _cameraOrderDirty = false;
    }
    return _cameras;
}

void Scene::setDebugCamera(Camera* camera)
{
    Object::assign(_debugCamera, camera);
}

void Scene::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    Node::visit(renderer, parentTransform, parentFlags);
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

#if (defined(AX_ENABLE_PHYSICS_2D) || defined(AX_ENABLE_PHYSICS_3D))

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
#    if defined(AX_ENABLE_PHYSICS_2D)
    _physicsWorld2D = PhysicsWorld2D::obtain(this);
#    endif

    bool ret = false;
    do
    {
        this->setContentSize(_director->getCanvasSize());

#    if defined(AX_ENABLE_PHYSICS_3D)
        _physicsWorld3D = PhysicsWorld3D::obtain(this);
#    endif

        // success
        ret = true;
    } while (0);
    return ret;
}

#endif

void Scene::setFixedDeltaTime(float fixedStep)
{
    fixedStep = std::clamp<float>(fixedStep, 0.0001F, 10.0F);

    _fixedDeltaTime = fixedStep;

    // Reset accumulator to fixedDeltaTime so the next tick will immediately run at least one fixedUpdate,
    // avoiding a stall after changing step size.
    _fixedAccumulator = _fixedDeltaTime;
}

void Scene::tick(float deltaTime)
{
    if (_fixedUpdateEnabled)
    {
        // apply time scale and clamp to avoid huge dt spikes
        deltaTime = (std::min)(deltaTime * _timeScale, _maxDeltaTime);

        // accumulate time
        _fixedAccumulator += deltaTime;

        int steps = 0;
        while (_fixedAccumulator >= _fixedDeltaTime && steps < _maxFixedStepsPerFrame)
        {
            fixedUpdate(_fixedDeltaTime);

            _fixedAccumulator -= _fixedDeltaTime;
            ++steps;
        }

        // spiral of death protection: if we hit max steps, drop remaining accumulator
        if (steps == _maxFixedStepsPerFrame)
            _fixedAccumulator = 0.0f;

        // compute interpolation alpha for rendering (0..1)
        _physicsInterpolationAlpha = static_cast<float>(_fixedAccumulator) / _fixedDeltaTime;
    }
    else
    {
#if (defined(AX_ENABLE_PHYSICS_2D) || defined(AX_ENABLE_PHYSICS_3D) || defined(AX_ENABLE_NAVMESH))
        // apply time scale and clamp to avoid huge dt spikes
        deltaTime = (std::min)(deltaTime * _timeScale, _maxDeltaTime);
        stepPhysicsAndNavigation(deltaTime);
#endif
    }
}

void Scene::fixedUpdate(float delta)
{
#if (defined(AX_ENABLE_PHYSICS_2D) || defined(AX_ENABLE_PHYSICS_3D) || defined(AX_ENABLE_NAVMESH))
    stepPhysicsAndNavigation(delta);
#endif
}

#if (defined(AX_ENABLE_PHYSICS_2D) || defined(AX_ENABLE_PHYSICS_3D) || defined(AX_ENABLE_NAVMESH))
void Scene::stepPhysicsAndNavigation(float deltaTime)
{
#    if defined(AX_ENABLE_PHYSICS_2D)
    if (_physicsWorld2D && _physicsWorld2D->isAutoStep())
        _physicsWorld2D->stepSimulation(deltaTime);
#    endif

#    if defined(AX_ENABLE_PHYSICS_3D)
    if (_physicsWorld3D && _physicsWorld3D->isAutoStep())
        _physicsWorld3D->stepSimulation(deltaTime);
#    endif
#    if defined(AX_ENABLE_NAVMESH)
    if (_navMesh)
    {
        _navMesh->update(deltaTime);
    }
#    endif
}
#endif

}  // namespace ax
