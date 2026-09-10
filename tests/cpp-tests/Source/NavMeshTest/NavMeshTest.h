/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#ifndef _NAVMESH_TEST_H_
#define _NAVMESH_TEST_H_

#include "../BaseTest.h"
#include "axmol/navmesh/NavMesh.h"
#include <string>

DEFINE_TEST_SUITE(NavMeshTests);

#if !defined(AX_ENABLE_NAVMESH) || !defined(AX_ENABLE_PHYSICS_3D)
class NavMeshDisabled : public TestCase
{
public:
    CREATE_FUNC(NavMeshDisabled);

    virtual void onEnter() override;
};
#else

class NavMeshBaseTestDemo : public TestCase
{
public:
    CREATE_FUNC(NavMeshBaseTestDemo);
    NavMeshBaseTestDemo();
    virtual ~NavMeshBaseTestDemo();

    // overrides
    virtual bool init() override;
    virtual void update(float delta) override;
    virtual bool onPointerDown(ax::PointerEvent* event);
    virtual void onPointerMove(ax::PointerEvent* event);
    virtual void onPointerUp(ax::PointerEvent* event);

protected:
    void initScene();
    void createAgent(const ax::Vec3& pos);
    void createObstacle(const ax::Vec3& pos);
    void moveAgents(const ax::Vec3& des);
    virtual void touchesBegan(ax::PointerEvent* event) {};
    virtual void touchesMoved(ax::PointerEvent* event) {};
    virtual void touchesEnded(ax::PointerEvent* event) {};

protected:
    ax::Camera* _camera;
    float _angle;
    std::vector<std::pair<ax::NavMeshAgent*, ax::Animate3D*>> _agents;
    bool _needMoveAgents;
};

class NavMeshBasicTestDemo : public NavMeshBaseTestDemo
{
public:
    CREATE_FUNC(NavMeshBasicTestDemo);
    NavMeshBasicTestDemo();
    virtual ~NavMeshBasicTestDemo();

    // overrides
    virtual bool init() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    virtual void onEnter() override;

protected:
    virtual void touchesBegan(ax::PointerEvent* event) override {};
    virtual void touchesMoved(ax::PointerEvent* event) override {};
    virtual void touchesEnded(ax::PointerEvent* event) override;

protected:
    ax::Label* _debugLabel;
};

class NavMeshAdvanceTestDemo : public NavMeshBaseTestDemo
{
public:
    CREATE_FUNC(NavMeshAdvanceTestDemo);
    NavMeshAdvanceTestDemo();
    virtual ~NavMeshAdvanceTestDemo();

    // overrides
    virtual bool init() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    virtual void onEnter() override;

protected:
    virtual void touchesBegan(ax::PointerEvent* event) override {};
    virtual void touchesMoved(ax::PointerEvent* event) override {};
    virtual void touchesEnded(ax::PointerEvent* event) override;

protected:
    ax::Label* _obstacleLabel;
    ax::Label* _agentLabel;
    ax::Label* _debugLabel;
};

#endif

#endif
