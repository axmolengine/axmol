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

#include "navmesh/NavMeshObstacle.h"
#if defined(AX_ENABLE_NAVMESH)

#    include "navmesh/NavMesh.h"
#    include "2d/Node.h"
#    include "2d/Scene.h"
#    include "recast/DetourTileCache.h"

namespace ax
{

NavMeshObstacle* NavMeshObstacle::create(float radius, float height)
{
    auto ref = new NavMeshObstacle();
    if (ref->initWith(radius, height))
    {
        ref->autorelease();
        return ref;
    }
    AX_SAFE_DELETE(ref);
    return nullptr;
}

std::string_view NavMeshObstacle::getNavMeshObstacleComponentName()
{
    static std::string comName = "___NavMeshObstacleComponent___";
    return comName;
}

NavMeshObstacle::NavMeshObstacle()
    : _radius(0.0f), _height(0.0f), _syncFlag(NODE_AND_NODE), _obstacleID(-1), _tileCache(nullptr)
{}

ax::NavMeshObstacle::~NavMeshObstacle() {}

bool NavMeshObstacle::initWith(float radius, float height)
{
    _radius = radius;
    _height = height;
    setName(getNavMeshObstacleComponentName());
    return true;
}

void ax::NavMeshObstacle::removeFrom(dtTileCache* /*tileCache*/)
{
    _tileCache->removeObstacle(_obstacleID);
    _tileCache  = nullptr;
    _obstacleID = -1;
}

void ax::NavMeshObstacle::addTo(dtTileCache* tileCache)
{
    _tileCache = tileCache;
    Mat4 mat   = _owner->getNodeToWorldTransform();
    _tileCache->addObstacle(&mat.m[12], _radius, _height, &_obstacleID);
}

void ax::NavMeshObstacle::onExit()
{
    if (_obstacleID == -1)
        return;
    Component::onExit();
    auto scene = _owner->getScene();
    if (scene && scene->getNavMesh())
    {
        scene->getNavMesh()->removeNavMeshObstacle(this);
    }
}

void ax::NavMeshObstacle::onEnter()
{
    if (_obstacleID != -1)
        return;
    Component::onEnter();
    auto scene = _owner->getScene();
    if (scene && scene->getNavMesh())
    {
        scene->getNavMesh()->addNavMeshObstacle(this);
    }
}

void ax::NavMeshObstacle::postUpdate(float /*delta*/)
{
    if ((_syncFlag & OBSTACLE_TO_NODE) != 0)
        syncToNode();
}

void ax::NavMeshObstacle::preUpdate(float /*delta*/)
{
    if ((_syncFlag & NODE_TO_OBSTACLE) != 0)
        syncToObstacle();
}

void NavMeshObstacle::syncToNode()
{
    if (_tileCache)
    {
        auto obstacle = _tileCache->getObstacleByRef(_obstacleID);
        if (obstacle && obstacle->type == DT_OBSTACLE_CYLINDER)
        {
            auto& cylinder = obstacle->cylinder;
            Vec3 localPos  = Vec3(cylinder.pos[0], cylinder.pos[1], cylinder.pos[2]);
            if (_owner->getParent())
                _owner->getParent()->getWorldToNodeTransform().transformPoint(localPos, &localPos);
            _owner->setPosition3D(localPos);
            _radius = cylinder.radius;
            _height = cylinder.height;
        }
    }
}

void ax::NavMeshObstacle::setRadius(float radius)
{
    _radius = radius;
}

void ax::NavMeshObstacle::setHeight(float height)
{
    _height = height;
}

void NavMeshObstacle::syncToObstacle()
{
    if (_tileCache)
    {
        auto obstacle = _tileCache->getObstacleByRef(_obstacleID);
        if (obstacle && obstacle->type == DT_OBSTACLE_CYLINDER)
        {
            Mat4 mat       = _owner->getNodeToWorldTransform();
            auto& cylinder = obstacle->cylinder;
            if ((mat.m[12] != cylinder.pos[0] && mat.m[13] != cylinder.pos[1] && mat.m[14] != cylinder.pos[2]) ||
                cylinder.radius != _radius || cylinder.height != _height)
            {
                _tileCache->removeObstacle(_obstacleID);
                _tileCache->addObstacle(&mat.m[12], _radius, _height, &_obstacleID);
            }
        }
    }
}

}

#endif  // AX_ENABLE_NAVMESH
