/****************************************************************************
Copyright (c) 2013 cocos2d-x.org
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include "../BaseTest.h"
#include "../MeshRendererTest/DrawNode3D.h"

#include "renderer/backend/ProgramState.h"
#include "renderer/backend/Types.h"

#include <string>

namespace ax
{
class MeshRenderer;
class Delay;
}  // namespace ax

enum State
{
    State_None         = 0,
    State_Idle         = 0x01,
    State_Move         = 0x02,
    State_Rotate       = 0x04,
    State_Speak        = 0x08,
    State_MeleeAttack  = 0x10,
    State_RemoteAttack = 0x20,
    State_Attack       = 0x40,
};
enum class CameraType
{
    Free        = 0,
    FirstPerson = 1,
    ThirdPerson = 2,
};

enum class OperateCamType
{
    MoveCamera   = 0,
    RotateCamera = 1,
};

DEFINE_TEST_SUITE(Camera3DTests);

class CameraBaseTest : public TestCase
{
public:
protected:
    ax::BillBoard* bill1;
    ax::BillBoard* bill2;
    ax::Label* l1;
    ax::Label* l2;
};

class CameraRotationTest : public CameraBaseTest
{

public:
    CREATE_FUNC(CameraRotationTest);
    CameraRotationTest();
    virtual ~CameraRotationTest();

    virtual void onEnter() override;
    virtual void onExit() override;

    virtual void update(float dt) override;

    // overrides
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

protected:
    ax::Node* _camControlNode;
    ax::Node* _camNode;
    ax::EventListenerTouchOneByOne* _lis;
};

class Camera3DTestDemo : public CameraBaseTest
{
public:
    CREATE_FUNC(Camera3DTestDemo);
    Camera3DTestDemo();
    virtual ~Camera3DTestDemo();

    virtual void onEnter() override;
    virtual void onExit() override;
    // overrides
    virtual std::string title() const override;
    void addNewSpriteWithCoords(ax::Vec3 p,
                                std::string fileName,
                                bool playAnimation = false,
                                float scale        = 1.0f,
                                bool bindCamera    = false);

    void onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);

    void scaleCameraCallback(ax::Object* sender, float value);
    void rotateCameraCallback(ax::Object* sender, float value);
    void SwitchViewCallback(ax::Object* sender, CameraType cameraType);
    void updateCamera(float fDelta);
    void move3D(float elapsedTime);
    void updateState(float elapsedTime);
    bool isState(unsigned int state, unsigned int bit) const;
    void reachEndCallBack();

    bool onTouchesCommon(ax::Touch* touch, ax::Event* event, bool* touchProperty);
    bool onTouchesZoomOut(ax::Touch* touch, ax::Event* event);
    void onTouchesZoomOutEnd(ax::Touch* touch, ax::Event* event);
    bool onTouchesZoomIn(ax::Touch* touch, ax::Event* event);
    void onTouchesZoomInEnd(ax::Touch* touch, ax::Event* event);

    bool onTouchesRotateLeft(ax::Touch* touch, ax::Event* event);
    void onTouchesRotateLeftEnd(ax::Touch* touch, ax::Event* event);
    bool onTouchesRotateRight(ax::Touch* touch, ax::Event* event);
    void onTouchesRotateRightEnd(ax::Touch* touch, ax::Event* event);

protected:
    std::string _title;
    ax::Layer* _layer3D;
    ax::MeshRenderer* _mesh;
    ax::Vec3 _targetPos;
    CameraType _cameraType;
    ax::MenuItem* _incRot;
    ax::MenuItem* _decRot;
    unsigned int _curState;
    ax::Camera* _camera;
    ax::MoveTo* _moveAction;
    bool _bZoomOut;
    bool _bZoomIn;
    bool _bRotateLeft;
    bool _bRotateRight;
    ax::Label* _RotateRightlabel;
    ax::Label* _RotateLeftlabel;
    ax::Label* _ZoomInlabel;
    ax::Label* _ZoomOutlabel;
};

class CameraCullingDemo : public CameraBaseTest
{
public:
    CREATE_FUNC(CameraCullingDemo);
    CameraCullingDemo();
    virtual ~CameraCullingDemo();

    virtual void onEnter() override;
    virtual void onExit() override;

    virtual void update(float dt) override;

    // overrides
    virtual std::string title() const override;
    void reachEndCallBack();
    void switchViewCallback(ax::Object* sender);
    void addMeshCallback(ax::Object* sender);
    void delMeshCallback(ax::Object* sender);

    void drawCameraFrustum();

protected:
    ax::Label* _labelMeshCount;
    ax::Layer* _layer3D;
    std::vector<ax::MeshRenderer*> _objects;
    CameraType _cameraType;
    ax::Camera* _cameraFirst;
    ax::Camera* _cameraThird;
    ax::MoveBy* _moveAction;
    ax::DrawNode3D* _drawAABB;
    ax::DrawNode3D* _drawFrustum;
    int _row;
};

class CameraArcBallDemo : public CameraBaseTest
{
public:
    CREATE_FUNC(CameraArcBallDemo);
    CameraArcBallDemo();
    virtual ~CameraArcBallDemo();

    virtual void onEnter() override;
    virtual void onExit() override;

    virtual void update(float dt) override;

    // overrides
    virtual std::string title() const override;

    void switchOperateCallback(ax::Object* sender);
    void switchTargetCallback(ax::Object* sender);
    void onTouchsMoved(const std::vector<ax::Touch*>& touchs, ax::Event* event);
    void updateCameraTransform();
    void calculateArcBall(ax::Vec3& axis,
                          float& angle,
                          float p1x,
                          float p1y,
                          float p2x,
                          float p2y);                  // calculate  rotation quaternion parameters
    float projectToSphere(float r, float x, float y);  // points on the screen project to arc ball

protected:
    ax::Layer* _layer3D;
    CameraType _cameraType;
    ax::Camera* _camera;
    ax::DrawNode3D* _drawGrid;
    ax::Quaternion _rotationQuat;  // rotation Quaternion
    float _radius;                      // arc ball radius
    float _distanceZ;
    OperateCamType _operate;  // switch rotate or zoom
    ax::Vec3 _center;    // camera look target
    int _target;              // switch camera look target
    ax::MeshRenderer* _mesh1;
    ax::MeshRenderer* _mesh2;
};

class FogTestDemo : public CameraBaseTest
{
public:
    CREATE_FUNC(FogTestDemo);
    FogTestDemo();
    virtual ~FogTestDemo();

    virtual void onEnter() override;
    virtual void onExit() override;

    virtual void update(float dt) override;

    // overrides
    virtual std::string title() const override;

    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);

    void switchTypeCallback(ax::Object* sender, int type);

protected:
    CameraType _cameraType                         = CameraType::Free;
    ax::Layer* _layer3D                       = nullptr;
    ax::Camera* _camera                       = nullptr;
    ax::MeshRenderer* _mesh1                  = nullptr;
    ax::MeshRenderer* _mesh2                  = nullptr;
    ax::backend::ProgramState* _programState1 = nullptr;
    ax::backend::ProgramState* _programState2 = nullptr;

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    ax::EventListenerCustom* _backToForegroundListener;
#endif
};

// class CameraFrameBufferTest : public CameraBaseTest
//{
// public:
//     CREATE_FUNC(CameraFrameBufferTest);
//     CameraFrameBufferTest();
//     virtual ~CameraFrameBufferTest();
//     // overrides
//     virtual std::string title() const override;
//
//     virtual void onEnter() override;
// };

class BackgroundColorBrushTest : public CameraBaseTest
{
public:
    CREATE_FUNC(BackgroundColorBrushTest);
    BackgroundColorBrushTest();
    virtual ~BackgroundColorBrushTest();

    // overrides
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    virtual void onEnter() override;
};
