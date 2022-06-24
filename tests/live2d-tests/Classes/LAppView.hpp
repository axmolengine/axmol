/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <CubismFramework.hpp>
#include <Math/CubismMatrix44.hpp>
#include <Math/CubismViewMatrix.hpp>
#include "TouchManager.h"
#include "2d/CCSprite.h"
#include "cocos2d.h"
#include <vector>
#include "LAppLive2DManager.hpp"

class LAppView : public cocos2d::DrawNode
{
public:
    LAppView();
    virtual ~LAppView() {}

    virtual void onEnter();
    virtual void onExit();

    virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags);
    void onDraw(const cocos2d::Mat4& transform, uint32_t flags);

    void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

    void updateViewMatrix(float dx, float dy, float cx, float cy, float scale);
    float transformViewX(float deviceX);
    float transformViewY(float deviceY);
    float transformScreenX(float deviceX);
    float transformScreenY(float deviceY);

    void setDebugRectsNode(DrawNode* debugRects);
    void drawDebugRects(LAppLive2DManager* manager) const;

    static LAppView* createDrawNode();

private:
    TouchManager* touchMgr;
    Csm::CubismMatrix44* deviceToScreen;
    Csm::CubismViewMatrix* viewMatrix;
    Csm::Rendering::CubismCommandBuffer_Cocos2dx _commandBuffer;
    DrawNode* _debugRects;
};
