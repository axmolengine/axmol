/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppView.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppDefine.hpp"
#include "LAppPal.hpp"
#include "LAppModel.hpp"

using namespace Csm;
using namespace LAppDefine;

using namespace ax;

LAppView::LAppView(): DrawNode()
                    , _debugRects(NULL)
{
}

void LAppView::onEnter()
{
    DrawNode::onEnter();

    // タッチ関係のイベント管理
    touchMgr = new TouchManager();

    // デバイス座標からスクリーン座標に変換するための
    deviceToScreen = new CubismMatrix44();

    // 画面の表示の拡大縮小や移動の変換を行う行列
    viewMatrix = new CubismViewMatrix();

    Size size = Director::getInstance()->getWinSize();

    float width = size.width;
    float height = size.height;
    // 縦サイズを基準とする
    float ratio = width / height;
    float left = -ratio;
    float right = ratio;
    float bottom = ViewLogicalLeft;
    float top = ViewLogicalRight;

    viewMatrix->SetScreenRect(left, right, bottom, top); // デバイスに対応する画面の範囲。 Xの左端, Xの右端, Yの下端, Yの上端
    viewMatrix->Scale(ViewScale, ViewScale);

    deviceToScreen->LoadIdentity(); // サイズが変わった際などリセット必須
    if (width > height)
    {
      float screenW = fabsf(right - left);
      deviceToScreen->ScaleRelative(screenW / width, -screenW / width);
    }
    else
    {
      float screenH = fabsf(top - bottom);
      deviceToScreen->ScaleRelative(screenH / height, -screenH / height);
    }
    deviceToScreen->TranslateRelative(-width / 2.0f, -height / 2.0f);

    // 表示範囲の設定
    viewMatrix->SetMaxScale(ViewMaxScale); // 限界拡大率
    viewMatrix->SetMinScale(ViewMinScale); // 限界縮小率

    // 表示できる最大範囲
    viewMatrix->SetMaxScreenRect(
        ViewLogicalMaxLeft,
        ViewLogicalMaxRight,
        ViewLogicalMaxBottom,
        ViewLogicalMaxTop
    );

    // イベントリスナー作成
    EventListenerTouchAllAtOnce* listener = EventListenerTouchAllAtOnce::create();

    // タッチメソッド設定
    listener->onTouchesBegan = AX_CALLBACK_2(LAppView::onTouchesBegan, this);
    listener->onTouchesMoved = AX_CALLBACK_2(LAppView::onTouchesMoved, this);
    listener->onTouchesEnded = AX_CALLBACK_2(LAppView::onTouchesEnded, this);

    // 優先度100でディスパッチャーに登録
    this->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 100);
}

void LAppView::onExit()
{
    DrawNode::onExit();

    delete touchMgr;
    delete deviceToScreen;
    delete viewMatrix;
}

void LAppView::draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags)
{
    DrawNode::draw(renderer, transform, flags);
    onDraw(transform, flags);
}

void LAppView::onDraw(const ax::Mat4& transform, uint32_t flags)
{
    _commandBuffer.PushCommandGroup();

    Director::getInstance()->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    Director::getInstance()->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);

    LAppLive2DManager* Live2DMgr = LAppLive2DManager::GetInstance();

    Live2DMgr->SetViewMatrix(viewMatrix);

    // Cubism更新・描画
    Live2DMgr->OnUpdate(&_commandBuffer);

    if (_debugRects)
    {
        drawDebugRects(Live2DMgr);
    }

    Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

    _commandBuffer.PopCommandGroup();
}

void LAppView::onTouchesBegan(const std::vector<Touch*>& touches, Event* event)
{
    // タッチ開始
    size_t touchNum = touches.size();

    if (touchNum == 1)
    {
        Point pt = touches[0]->getLocationInView();
        if (DebugTouchLogEnable)LAppPal::PrintLog("[APP]touchesBegan x:%.0f y:%.0f", pt.x, pt.y);
        touchMgr->touchesBegan(pt.x, pt.y);
    }
}

void LAppView::onTouchesMoved(const std::vector<Touch*>& touches, Event* event)
{
    // タッチ中
    size_t touchNum = touches.size();

    float screenX = this->transformScreenX(touchMgr->getX());
    float screenY = this->transformScreenY(touchMgr->getY());
    float viewX = this->transformViewX(touchMgr->getX());
    float viewY = this->transformViewY(touchMgr->getY());

    if (touchNum == 1)
    {
        Point pt = touches[0]->getLocationInView();

        if (DebugTouchLogEnable)
            LAppPal::PrintLog("[APP]touchesMoved device{x:%.0f y:%.0f} screen{x:%.2f y:%.2f} view{x:%.2f y:%.2f}", pt.x, pt.y, screenX, screenY, viewX, viewY);

        touchMgr->touchesMoved(pt.x, pt.y);
    }
    LAppLive2DManager* live2DMgr = LAppLive2DManager::GetInstance();
    live2DMgr->OnDrag(viewX, viewY);
}

void LAppView::onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    // タッチ終了
    LAppLive2DManager* live2DMgr = LAppLive2DManager::GetInstance();
    live2DMgr->OnDrag(0, 0);

    if (touches.size() == 1)
    {
        // シングルタップ
        float x = deviceToScreen->TransformX(touchMgr->getX()); // 論理座標変換した座標を取得。
        float y = deviceToScreen->TransformY(touchMgr->getY()); // 論理座標変換した座標を取得。
        if (DebugTouchLogEnable) LAppPal::PrintLog("[APP]touchesEnded x:%.2f y:%.2f", x, y);
        live2DMgr->OnTap(x, y);
    }
}

void LAppView::updateViewMatrix(float dx, float dy, float cx, float cy, float scale)
{
    LAppLive2DManager* live2DMgr = LAppLive2DManager::GetInstance();

    // 拡大縮小
    viewMatrix->AdjustScale(cx, cy, scale);

    // 移動
    viewMatrix->AdjustTranslate(dx, dy);

    live2DMgr->SetViewMatrix(viewMatrix);
}

float LAppView::transformViewX(float deviceX)
{
    float screenX = deviceToScreen->TransformX(deviceX); // 論理座標変換した座標を取得。
    return viewMatrix->InvertTransformX(screenX); // 拡大、縮小、移動後の値。
}

float LAppView::transformViewY(float deviceY)
{
    float screenY = deviceToScreen->TransformY(deviceY); // 論理座標変換した座標を取得。
    return viewMatrix->InvertTransformY(screenY); // 拡大、縮小、移動後の値。
}

float LAppView::transformScreenX(float deviceX)
{
    return deviceToScreen->TransformX(deviceX);
}

float LAppView::transformScreenY(float deviceY)
{
    return deviceToScreen->TransformY(deviceY);
}

void LAppView::setDebugRectsNode(DrawNode* debugRects)
{
    _debugRects = debugRects;
}

void LAppView::drawDebugRects(LAppLive2DManager* manager) const
{
    const Color4F hitAreaColor = Color4F(1.0f, 0, 0, 0.2f);
    const Color4F userDataAreaColor = Color4F(0, 0, 1.0f, 0.2f);

    CubismMatrix44 projection;
    const Size window = Director::getInstance()->getWinSize();
    const CubismVector2 windowSize = CubismVector2(window.width, window.height);
    projection.Scale(1, window.width / window.height);

    if (viewMatrix != NULL)
    {
        projection.MultiplyByMatrix(viewMatrix);
    }

    for (csmUint32 i = 0; i < manager->GetModelCount(); ++i)
    {
        _debugRects->clear();
        LAppModel* model = manager->GetModel(i);
        const Csm::csmVector<Csm::csmRectF>& userDataAreas = model->GetUserDataAreas(projection, windowSize);
        for (csmUint32 j = 0; j < userDataAreas.GetSize(); ++j)
        {
            _debugRects->drawSolidRect(Vec2(userDataAreas[j].X, userDataAreas[j].Y)
                                       , Vec2(userDataAreas[j].GetRight(), userDataAreas[j].GetBottom())
                                       , userDataAreaColor);
        }

        const Csm::csmVector<Csm::csmRectF>& hitAreas = model->GetHitAreas(projection, windowSize);
        for (csmUint32 j = 0; j < hitAreas.GetSize(); ++j)
        {
            _debugRects->drawSolidRect(Vec2(hitAreas[j].X, hitAreas[j].Y)
                                       , Vec2(hitAreas[j].GetRight(), hitAreas[j].GetBottom())
                                       , hitAreaColor);
        }
    }
}

LAppView* LAppView::createDrawNode()
{
    LAppView* ret = new(std::nothrow) LAppView();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}
