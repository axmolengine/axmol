/****************************************************************************
Copyright (c) 2013 cocos2d-x.org
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

https://axmolengine.github.io/

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

NS_AX_BEGIN
class MeshRenderer;
class Delay;
NS_AX_END  // namespace ax

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
    axis::BillBoard* bill1;
    axis::BillBoard* bill2;
    axis::Label* l1;
    axis::Label* l2;
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
    axis::Node* _camControlNode;
    axis::Node* _camNode;
    axis::EventListenerTouchOneByOne* _lis;
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
    void addNewSpriteWithCoords(axis::Vec3 p,
                                std::string fileName,
                                bool playAnimation = false,
                                float scale        = 1.0f,
                                bool bindCamera    = false);

    void onTouchesBegan(const std::vector<axis::Touch*>& touches, axis::Event* event);
    void onTouchesMoved(const std::vector<axis::Touch*>& touches, axis::Event* event);
    void onTouchesEnded(const std::vector<axis::Touch*>& touches, axis::Event* event);

    void scaleCameraCallback(axis::Ref* sender, float value);
    void rotateCameraCallback(axis::Ref* sender, float value);
    void SwitchViewCallback(axis::Ref* sender, CameraType cameraType);
    void updateCamera(float fDelta);
    void move3D(float elapsedTime);
    void updateState(float elapsedTime);
    bool isState(unsigned int state, unsigned int bit) const;
    void reachEndCallBack();

    bool onTouchesCommon(axis::Touch* touch, axis::Event* event, bool* touchProperty);
    bool onTouchesZoomOut(axis::Touch* touch, axis::Event* event);
    void onTouchesZoomOutEnd(axis::Touch* touch, axis::Event* event);
    bool onTouchesZoomIn(axis::Touch* touch, axis::Event* event);
    void onTouchesZoomInEnd(axis::Touch* touch, axis::Event* event);

    bool onTouchesRotateLeft(axis::Touch* touch, axis::Event* event);
    void onTouchesRotateLeftEnd(axis::Touch* touch, axis::Event* event);
    bool onTouchesRotateRight(axis::Touch* touch, axis::Event* event);
    void onTouchesRotateRightEnd(axis::Touch* touch, axis::Event* event);

protected:
    std::string _title;
    axis::Layer* _layer3D;
    axis::MeshRenderer* _mesh;
    axis::Vec3 _targetPos;
    CameraType _cameraType;
    axis::MenuItem* _incRot;
    axis::MenuItem* _decRot;
    unsigned int _curState;
    axis::Camera* _camera;
    axis::MoveTo* _moveAction;
    bool _bZoomOut;
    bool _bZoomIn;
    bool _bRotateLeft;
    bool _bRotateRight;
    axis::Label* _RotateRightlabel;
    axis::Label* _RotateLeftlabel;
    axis::Label* _ZoomInlabel;
    axis::Label* _ZoomOutlabel;
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
    void switchViewCallback(axis::Ref* sender);
    void addMeshCallback(axis::Ref* sender);
    void delMeshCallback(axis::Ref* sender);

    void drawCameraFrustum();

protected:
    axis::Label* _labelMeshCount;
    axis::Layer* _layer3D;
    std::vector<axis::MeshRenderer*> _objects;
    CameraType _cameraType;
    axis::Camera* _cameraFirst;
    axis::Camera* _cameraThird;
    axis::MoveBy* _moveAction;
    axis::DrawNode3D* _drawAABB;
    axis::DrawNode3D* _drawFrustum;
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

    void switchOperateCallback(axis::Ref* sender);
    void switchTargetCallback(axis::Ref* sender);
    void onTouchsMoved(const std::vector<axis::Touch*>& touchs, axis::Event* event);
    void updateCameraTransform();
    void calculateArcBall(axis::Vec3& axis,
                          float& angle,
                          float p1x,
                          float p1y,
                          float p2x,
                          float p2y);                  // calculate  rotation quaternion parameters
    float projectToSphere(float r, float x, float y);  // points on the screen project to arc ball

protected:
    axis::Layer* _layer3D;
    CameraType _cameraType;
    axis::Camera* _camera;
    axis::DrawNode3D* _drawGrid;
    axis::Quaternion _rotationQuat;  // rotation Quaternion
    float _radius;                      // arc ball radius
    float _distanceZ;
    OperateCamType _operate;  // switch rotate or zoom
    axis::Vec3 _center;    // camera look target
    int _target;              // switch camera look target
    axis::MeshRenderer* _mesh1;
    axis::MeshRenderer* _mesh2;
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

    void onTouchesMoved(const std::vector<axis::Touch*>& touches, axis::Event* event);

    void switchTypeCallback(axis::Ref* sender, int type);

protected:
    CameraType _cameraType                         = CameraType::Free;
    axis::Layer* _layer3D                       = nullptr;
    axis::Camera* _camera                       = nullptr;
    axis::MeshRenderer* _mesh1                  = nullptr;
    axis::MeshRenderer* _mesh2                  = nullptr;
    axis::backend::ProgramState* _programState1 = nullptr;
    axis::backend::ProgramState* _programState2 = nullptr;

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    axis::EventListenerCustom* _backToForegroundListener;
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
