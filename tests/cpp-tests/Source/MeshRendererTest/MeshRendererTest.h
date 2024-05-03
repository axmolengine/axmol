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

#include "BaseTest.h"
#include "renderer/backend/ProgramState.h"
#include <string>

NS_AX_BEGIN
class Animate3D;
class MeshRenderer;
class Delay;
class Ray;
class DrawNode3D;
class GLProgramState;
class MotionStreak3D;
NS_AX_END  // namespace ax

DEFINE_TEST_SUITE(MeshRendererTests);

class MeshRendererTestDemo : public TestCase
{
public:
    // overrides
    virtual std::string title() const override;

    virtual ~MeshRendererTestDemo();
};

class MeshRendererForceDepthTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererForceDepthTest);
    MeshRendererForceDepthTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class MeshRendererEmptyTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererEmptyTest);
    MeshRendererEmptyTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class MeshRendererBasicTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererBasicTest);
    MeshRendererBasicTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void addNewMeshWithCoords(ax::Vec2 p);
    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);
};

class MeshRendererStaticInstancingBasicTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererStaticInstancingBasicTest);
    MeshRendererStaticInstancingBasicTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class MeshRendererDynamicInstancingBasicTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererDynamicInstancingBasicTest);
    MeshRendererDynamicInstancingBasicTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class MeshRendererPreallocatedInstancingBufferTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererPreallocatedInstancingBufferTest);
    MeshRendererPreallocatedInstancingBufferTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class MeshRendererInstancingStressTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererInstancingStressTest);
    MeshRendererInstancingStressTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class MeshRendererUVAnimationTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererUVAnimationTest);
    MeshRendererUVAnimationTest();
    virtual ~MeshRendererUVAnimationTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

protected:
    void cylinderUpdate(float dt);

    float _cylinder_texture_offset;
    float _shining_duration;
    ax::backend::ProgramState* _state = nullptr;

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    ax::EventListenerCustom* _backToForegroundListener;
#endif
};

class MeshRendererFakeShadowTest : public MeshRendererTestDemo
{
public:
    enum State
    {
        State_None   = 0,
        State_Idle   = 0x01,
        State_Move   = 0x02,
        State_Rotate = 0x04
    };
    CREATE_FUNC(MeshRendererFakeShadowTest);
    MeshRendererFakeShadowTest();
    virtual ~MeshRendererFakeShadowTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void Move(ax::Object* sender, int value);
    void updateCamera(float fDelta);
    void move3D(float elapsedTime);
    void updateState(float elapsedTime);
    bool isState(unsigned int state, unsigned int bit) const;
    void onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);

private:
    ax::Camera* _camera;
    ax::Vec3 _targetPos;
    unsigned int _curState;
    ax::MeshRenderer* _plane;
    ax::MeshRenderer* _orc;
    ax::backend::ProgramState* _state = nullptr;

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    ax::EventListenerCustom* _backToForegroundListener;
#endif
};

class MeshRendererLightMapTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererLightMapTest);
    MeshRendererLightMapTest();
    virtual ~MeshRendererLightMapTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);

private:
    ax::Camera* _camera;
};

class MeshRendererBasicToonShaderTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererBasicToonShaderTest);
    MeshRendererBasicToonShaderTest();
    virtual ~MeshRendererBasicToonShaderTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

protected:
    ax::backend::ProgramState* _state;

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    ax::EventListenerCustom* _backToForegroundListener;
#endif
};

class MeshRendererHitTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererHitTest);
    MeshRendererHitTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class MeshRendererEffectTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererEffectTest);
    MeshRendererEffectTest();
    virtual ~MeshRendererEffectTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void addNewMeshWithCoords(ax::Vec2 p);

    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);

protected:
    std::vector<ax::MeshRenderer*> _meshes;

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    ax::EventListenerCustom* _backToForegroundListener;
#endif
};

class AsyncLoadMeshRendererTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(AsyncLoadMeshRendererTest);
    AsyncLoadMeshRendererTest();
    virtual ~AsyncLoadMeshRendererTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void menuCallback_asyncLoadMesh(ax::Object* sender);

    void asyncLoad_Callback(ax::MeshRenderer* mesh, void* param);

protected:
    std::vector<std::string> _paths;  // model paths to be loaded
};

class MeshRendererWithSkinTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererWithSkinTest);
    MeshRendererWithSkinTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void addNewMeshWithCoords(ax::Vec2 p);

    void switchAnimationQualityCallback(ax::Object* sender);
    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);

    std::string getAnimationQualityMessage() const;

private:
    std::vector<ax::MeshRenderer*> _meshes;
    int _animateQuality;
    ax::MenuItemFont* _menuItem;
};

class MeshRendererWithSkinOutlineTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererWithSkinOutlineTest);
    MeshRendererWithSkinOutlineTest();
    virtual ~MeshRendererWithSkinOutlineTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void addNewMeshWithCoords(ax::Vec2 p);

    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);

protected:
    std::vector<ax::MeshRenderer*> _meshes;

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    ax::EventListenerCustom* _backToForegroundListener;
#endif
};

class Animate3DTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(Animate3DTest);
    Animate3DTest();
    ~Animate3DTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);

    virtual void update(float dt) override;

protected:
    void addMeshRenderer();

    enum class State
    {
        SWIMMING,
        SWIMMING_TO_HURT,
        HURT,
        HURT_TO_SWIMMING,
    };

    void reachEndCallBack();

    void renewCallBack();

    ax::MeshRenderer* _mesh;

    ax::Action* _swim;
    ax::Animate3D* _hurt;

    float _elapseTransTime;

    State _state;

    ax::MoveTo* _moveAction;
};

class AttachmentTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(AttachmentTest);
    AttachmentTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);

    void addNewMeshWithCoords(ax::Vec2 p);

protected:
    bool _hasWeapon;
    ax::MeshRenderer* _mesh;
};

class MeshRendererReskinTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererReskinTest);
    MeshRendererReskinTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void addNewMeshWithCoords(ax::Vec2 p);

    void menuCallback_reSkin(ax::Object* sender);

protected:
    void applyCurSkin();

    enum class SkinType
    {
        UPPER_BODY = 0,
        PANTS,
        SHOES,
        HAIR,
        FACE,
        HAND,
        GLASSES,
        MAX_TYPE,
    };

    std::vector<std::string> _skins[(int)SkinType::MAX_TYPE];  // all skins
    int _curSkin[(int)SkinType::MAX_TYPE];                     // current skin index
    ax::MeshRenderer* _mesh;
};

class MeshRendererWithOBBPerformanceTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererWithOBBPerformanceTest);
    MeshRendererWithOBBPerformanceTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void update(float dt) override;
    void addNewOBBWithCoords(ax::Vec2 p);
    void addNewMeshWithCoords(ax::Vec2 p);
    void onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void addOBBCallback(ax::Object* sender);
    void delOBBCallback(ax::Object* sender);
    void addOBBWithCount(float value);
    void delOBBWithCount(float value);

protected:
    ax::MeshRenderer* _mesh;
    std::vector<ax::OBB> _obb;
    ax::DrawNode3D* _drawOBB;
    ax::Label* _labelCubeCount;
    ax::MoveTo* _moveAction;
    ax::OBB _obbt;
    ax::OBB _obbtOri;  // tortoise origin obb
    ax::DrawNode3D* _drawDebug;
    bool _hasCollider;
    std::set<int> _intersetList;
    void initDrawBox();
    void reachEndCallBack();

    void unproject(const ax::Mat4& viewProjection,
                   const ax::Size* viewport,
                   ax::Vec3* src,
                   ax::Vec3* dst);
    void calculateRayByLocationInView(ax::Ray* ray, const ax::Vec2& location);
};

class MeshRendererMirrorTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererMirrorTest);
    MeshRendererMirrorTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void addNewMeshWithCoords(ax::Vec2 p);

protected:
    bool _hasWeapon;
    ax::MeshRenderer* _mesh;
    ax::MeshRenderer* _mirrorMesh;
};

class QuaternionTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(QuaternionTest);
    QuaternionTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void addNewMeshWithCoords(ax::Vec2 p);
    virtual void update(float delta) override;

protected:
    ax::MeshRenderer* _mesh;
    float _arcSpeed;
    float _radius;
    float _accAngle;
};

// 3d + 2d use case
class UseCaseMeshRenderer : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(UseCaseMeshRenderer);
    UseCaseMeshRenderer();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    virtual void update(float delta) override;

    void menuCallback_Message(ax::Object* sender);

protected:
    void switchCase();

    enum class USECASE
    {
        _3D_WITH_2D,
        _UI_3D_UI,
        MAX_CASE_NUM,
    };
    ax::Label* _label;
    int _caseIdx;  // use case index
    std::string _useCaseTitles[(int)USECASE::MAX_CASE_NUM];
};

// node animation test, axmol supports both skeletal animation and node animation
class NodeAnimationTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(NodeAnimationTest);
    NodeAnimationTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void addNewMeshWithCoords(ax::Vec2 p);

protected:
    std::vector<ax::MeshRenderer*> _meshes;
    int _vectorIndex;
};

NS_AX_BEGIN
class TextureCube;
class Skybox;
NS_AX_END  // namespace ax

class MeshRendererCubeMapTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererCubeMapTest);
    MeshRendererCubeMapTest();
    ~MeshRendererCubeMapTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void addNewMeshWithCoords(ax::Vec2);

    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);

protected:
    ax::TextureCube* _textureCube;
    ax::Skybox* _skyBox;
    ax::MeshRenderer* _teapot;
    ax::Camera* _camera;

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    ax::EventListenerCustom* _backToForegroundListener;
#endif
};

/// issue 9767 setGLProgram
class Issue9767 : public MeshRendererTestDemo
{
    enum class ShaderType
    {
        SHADER_TEX,
        SHADER_COLOR,
    };

public:
    CREATE_FUNC(Issue9767);
    Issue9767();
    ~Issue9767();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void menuCallback_SwitchShader(ax::Object* sender);

protected:
    ShaderType _shaderType;
    ax::MeshRenderer* _mesh;
};

/// Clipping MeshRenderer
class MeshRendererClippingTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererClippingTest);
    MeshRendererClippingTest();
    ~MeshRendererClippingTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class Animate3DCallbackTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(Animate3DCallbackTest);
    Animate3DCallbackTest();
    ~Animate3DCallbackTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

protected:
    ax::MeshRenderer* _meshRenderer;
};

class CameraBackgroundClearTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(CameraBackgroundClearTest);
    CameraBackgroundClearTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void switch_CameraClearMode(ax::Object* sender);

protected:
    ax::Camera* _camera;
    ax::Label* _label;
};

class MeshRendererVertexColorTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererVertexColorTest);
    MeshRendererVertexColorTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    virtual ~MeshRendererVertexColorTest();

protected:
    ax::MeshRenderer* _mesh;
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    ax::EventListenerCustom* _backToForegroundListener;
#endif
};

class MotionStreak3DTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MotionStreak3DTest);
    MotionStreak3DTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void update(float delta) override;

protected:
    ax::MeshRenderer* _mesh;
    ax::MotionStreak3D* _streak;
};

class MeshRendererNormalMappingTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererNormalMappingTest);
    MeshRendererNormalMappingTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void update(float dt) override;

    virtual ~MeshRendererNormalMappingTest();
};

class MeshRendererPropertyTest : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(MeshRendererPropertyTest);
    MeshRendererPropertyTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void update(float delta) override;

    void printMeshName(ax::Object* sender);
    void removeUsedTexture(ax::Object* sender);
    void resetTexture(ax::Object* sender);

    void refreshMeshRender();

protected:
    ax::MeshRenderer* _mesh;
    ax::Texture2D* _meshTex;
    std::string _texFile;
};

class Issue16155Test : public MeshRendererTestDemo
{
public:
    CREATE_FUNC(Issue16155Test);
    Issue16155Test();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};
