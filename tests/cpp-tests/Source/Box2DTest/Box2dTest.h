/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2021 @aismann; Peter Eismann, Germany; dreifrankensoft

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

#ifndef _BOX2D_TEST_H_
#define _BOX2D_TEST_H_

#include "axmol.h"
#include "box2d/box2d.h"
#include "../BaseTest.h"
#include "extensions/axmol-ext.h"

DEFINE_TEST_SUITE(Box2DTests);

class Box2DTest : public TestCase
{
public:
    CREATE_FUNC(Box2DTest);

    virtual bool init() override;

    Box2DTest();
    ~Box2DTest();
    virtual std::string title() const override;

    void initPhysics();
    void createResetButton();

    void addNewSpriteAtPosition(ax::Vec2 p);
    void update(float dt) override;
    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);

    void toggleDebugCallback(ax::Object* sender);

private:
    b2World* world;

    ax::Texture2D* _spriteTexture;
    ax::DrawNode* drawBox2D;
    ax::extension::PhysicsDebugNodeBox2D g_debugDraw;
    bool showDebugDraw = true;
};

#endif
