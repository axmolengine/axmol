/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

//Cubism
#include "SampleScene.h"
#include "LAppLive2DManager.hpp"
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppView.hpp"
#include "LAppSprite.hpp"
#include <Rendering/CubismRenderer.hpp>
#ifdef CSM_TARGET_ANDROID_ES2
#include <Rendering/Cocos2d/CubismRenderer_Cocos2dx.hpp>
#endif

//cocos2d
#include "base/Director.h"
#include "renderer/backend/DriverBase.h"

using namespace Csm;
using namespace LAppDefine;

//#define USE_RENDER_TARGET
//#define USE_MODEL_RENDER_TARGET

using namespace ax;

namespace {
    LAppLive2DManager* s_instance = NULL;

    void FinishedMotion(ACubismMotion* self)
    {
        LAppPal::PrintLog("Motion Finished: %x", self);
    }
}

LAppLive2DManager* LAppLive2DManager::GetInstance()
{
    if (s_instance == NULL)
    {
        s_instance = new LAppLive2DManager();
    }

    return s_instance;
}

void LAppLive2DManager::ReleaseInstance()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }

    s_instance = NULL;
}

LAppLive2DManager::LAppLive2DManager()
    : _sceneIndex(0)
    , _viewMatrix(NULL)
    , _renderTarget(SelectTarget_None)
    , _program(NULL)
    , _sprite(NULL)
    , _renderBuffer(NULL)
{
    assert(CubismFramework::IsStarted());

    std::fill_n(_clearColor, 4, 0);

    CubismFramework::Initialize();

    CreateShader();

    int width = static_cast<int>(ax::Director::getInstance()->getGLView()->getFrameSize().width);
    int height = static_cast<int>(ax::Director::getInstance()->getGLView()->getFrameSize().height);

    // 画面全体を覆うサイズ
    _sprite = new LAppSprite(_program);

    _viewMatrix = new CubismMatrix44();

    // 使用するターゲット
    _renderBuffer = new Csm::Rendering::CubismOffscreenFrame_Cocos2dx;
    if (_renderBuffer)
    {// 描画ターゲット作成

#if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
        // Retina対策でこっちからとる
        GLViewImpl *glimpl = (GLViewImpl *)Director::getInstance()->getGLView();
        glfwGetFramebufferSize(glimpl->getWindow(), &width, &height);
#endif

        // モデル描画キャンバス
        _renderBuffer->CreateOffscreenFrame(static_cast<csmUint32>(width), static_cast<csmUint32>(height));
    }

#ifdef CSM_TARGET_ANDROID_ES2
    char *exts = (char*)backend::DriverBase::getInstance()->getExtension();
    if(strstr(exts, "GL_NV_shader_framebuffer_fetch ")){
        Rendering::CubismRenderer_Cocos2dx::SetExtShaderMode( true , true );
    }
#endif

    ChangeScene(_sceneIndex);
}

LAppLive2DManager::~LAppLive2DManager()
{
    if (_renderBuffer)
    {
        _renderBuffer->DestroyOffscreenFrame();
        delete _renderBuffer;
        _renderBuffer = NULL;
    }
    delete _sprite;
    _sprite = NULL;

    ReleaseAllModel();
    CubismFramework::Dispose();
}

void LAppLive2DManager::ReleaseAllModel()
{
    for (csmUint32 i = 0; i < _models.GetSize(); i++)
    {
        delete _models[i];
    }

    _models.Clear();
}

LAppModel* LAppLive2DManager::GetModel(csmUint32 no) const
{
    if(no < _models.GetSize())
    {
        return _models[no];
    }

    return NULL;
}

csmUint32 LAppLive2DManager::GetModelCount() const
{
    return _models.GetSize();
}

void LAppLive2DManager::RecreateRenderer() const
{
    Rendering::CubismRenderer::StaticRelease();

    for (csmUint32 i = 0; i < _models.GetSize(); i++)
    {
        LAppModel* model = GetModel(i);
        model->ReloadRnederer();
    }
}

void LAppLive2DManager::SetViewMatrix(Csm::CubismMatrix44* matrix)
{
    _viewMatrix = matrix;
}


void LAppLive2DManager::OnDrag(csmFloat32 x, csmFloat32 y) const
{
    for (csmUint32 i = 0; i < _models.GetSize(); i++)
    {
        LAppModel* model = GetModel(i);

        model->SetDragging(x, y);
    }
}

void LAppLive2DManager::OnTap(csmFloat32 x, csmFloat32 y)
{
    if (DebugLogEnable) LAppPal::PrintLog("[APP]tap point: {x:%.2f y:%.2f}", x, y);

    for (csmUint32 i = 0; i < _models.GetSize(); i++)
    {
        if (_models[i]->HitTest(HitAreaNameHead, x, y))
        {
            if (DebugLogEnable) LAppPal::PrintLog("[APP]hit area: [%s]", HitAreaNameHead);
            _models[i]->SetRandomExpression();
        }
        else if (_models[i]->HitTest(HitAreaNameBody, x, y))
        {
            if (DebugLogEnable) LAppPal::PrintLog("[APP]hit area: [%s]", HitAreaNameBody);
            _models[i]->StartRandomMotion(MotionGroupTapBody, PriorityNormal, FinishedMotion);
        }
    }
}

void LAppLive2DManager::OnUpdate(Csm::Rendering::CubismCommandBuffer_Cocos2dx* commandBuffer) const
{
    Director* director = Director::getInstance();
    Size window = director->getWinSize();

    Csm::Rendering::CubismRenderer_Cocos2dx::StartFrame(commandBuffer);

    for (csmUint32 i = 0; i < _models.GetSize(); ++i)
    {
        CubismMatrix44 projection;
        LAppModel* model = GetModel(i);
        if (model->GetModel()->GetCanvasWidth() > 1.0f && window.width < window.height)
        {
            // 横に長いモデルを縦長ウィンドウに表示する際モデルの横サイズでscaleを算出する
            model->GetModelMatrix()->SetWidth(2.0f);
            projection.Scale(1.0f, static_cast<float>(window.width) / static_cast<float>(window.height));
        }
        else
        {
            projection.Scale(static_cast<float>(window.height) / static_cast<float>(window.width), 1.0f);
        }

        // 必要があればここで乗算
        if (_viewMatrix != NULL)
        {
            projection.MultiplyByMatrix(_viewMatrix);
        }

        if (_renderTarget == SelectTarget_ViewFrameBuffer && _renderBuffer && _sprite)
        {// レンダリングターゲット使いまわしの場合
            // レンダリング開始
            _renderBuffer->BeginDraw(commandBuffer, NULL);
            _renderBuffer->Clear(commandBuffer, _clearColor[0], _clearColor[1], _clearColor[2], _clearColor[3]); // 背景クリアカラー

            model->Update();
            model->Draw(commandBuffer, projection);///< 参照渡しなのでprojectionは変質する

            _renderBuffer->EndDraw(commandBuffer);

            float uvVertex[] =
            {
                1.0f, 1.0f,
                0.0f, 1.0f,
                0.0f, 0.0f,
                1.0f, 0.0f,
            };

            // program退避
            Csm::Rendering::CubismCommandBuffer_Cocos2dx* lastCommandBuffer = commandBuffer;

            _sprite->SetColor(1.0f, 1.0f, 1.0f, 0.25f + (float)i * 0.5f);
            _sprite->RenderImmidiate(commandBuffer, _renderBuffer->GetColorBuffer()->getBackendTexture(), uvVertex);

            // 元に戻す
            commandBuffer = lastCommandBuffer;
        }
        else {
            model->Update();
            model->Draw(commandBuffer, projection);///< 参照渡しなのでprojectionは変質する
        }
    }
}

void LAppLive2DManager::NextScene()
{
    csmInt32 no = (_sceneIndex + 1) % ModelDirSize;
    ChangeScene(no);
}

void LAppLive2DManager::ChangeScene(Csm::csmInt32 index)
{
    _sceneIndex = index;
    if (DebugLogEnable) LAppPal::PrintLog("[APP]model index: %d", _sceneIndex);

    // ModelDir[]に保持したディレクトリ名から
    // model3.jsonのパスを決定する.
    // ディレクトリ名とmodel3.jsonの名前を一致させておくこと.
    std::string dir = ModelDir[index];
    dir += "/";
    std::string modelJsonName = ModelDir[index];
    modelJsonName += ".model3.json";

    ReleaseAllModel();
    _models.PushBack(new LAppModel());
    _models[0]->LoadAssets(dir.c_str(), modelJsonName.c_str());

    /*
     * モデル半透明表示を行うサンプルを提示する。
     * ここでUSE_RENDER_TARGET、USE_MODEL_RENDER_TARGETが定義されている場合
     * 別のレンダリングターゲットにモデルを描画し、描画結果をテクスチャとして別のスプライトに張り付ける。
     */
    {
#if defined(USE_RENDER_TARGET)
        // Live2DManagerの持つターゲットに描画を行う場合、こちらを選択
        _renderTarget = SelectTarget_ViewFrameBuffer;
#elif defined(USE_MODEL_RENDER_TARGET)
        // 各LAppModelの持つターゲットに描画を行う場合、こちらを選択
        _renderTarget = SelectTarget_ModelFrameBuffer;
#else
        // デフォルトのメインフレームバッファへレンダリングする(通常)
        _renderTarget = SelectTarget_None;
#endif

#if defined(USE_RENDER_TARGET) || defined(USE_MODEL_RENDER_TARGET)
        // モデル個別にαを付けるサンプルとして、もう1体モデルを作成し、少し位置をずらす
        _models.PushBack(new LAppModel());
        _models[1]->LoadAssets(dir.c_str(), modelJsonName.c_str());
        _models[1]->GetModelMatrix()->TranslateX(0.2f);
#endif

        if(_renderTarget == SelectTarget_ModelFrameBuffer)
        {
            for(Csm::csmUint32 i=0;i<_models.GetSize(); i++)
            {
                // レンダリング先とスプライトを作成
                _models[i]->MakeRenderingTarget();
                // 適当なαを付ける
                _models[i]->SetSpriteColor(1.0f, 1.0f, 1.0f, 0.25f + 0.5f*(float)i);
            }
        }
    }
}

void LAppLive2DManager::CreateShader()
{
    auto* program = ProgramManager::getInstance()->loadProgram("custom/live2d_test_vs", "custom/live2d_test_fs");
    _program = program;
}

void LAppLive2DManager::SetRenderTargetClearColor(float r, float g, float b)
{
    _clearColor[0] = r;
    _clearColor[1] = g;
    _clearColor[2] = b;
}
