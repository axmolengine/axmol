/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#include "axmol.h"
#include "ui/CocosGUI.h"
#include "../BaseTest.h"
#include "DrawNodeEx/DrawNodeEx.h"


DEFINE_TEST_SUITE(DrawNodeExTests);

class DrawNodeExBaseTest : public TestCase
{
public:
    virtual std::string title() const override;
    void drawDirection(const ax::Vec2* vec, const int size, ax::Vec2 offset);
};

class DrawNodeExPictureTest : public DrawNodeExBaseTest
{
public:
    CREATE_FUNC(DrawNodeExPictureTest);

    DrawNodeExPictureTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);

private:
    ax::extension::DrawNodeEx* drawNodeEx;
    ax::any_buffer _abuf;
};

class DrawNodeExMorphTest : public DrawNodeExBaseTest
{
public:
    CREATE_FUNC(DrawNodeExMorphTest);

    DrawNodeExMorphTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);

private:
    ax::extension::DrawNodeEx* drawNodeEx;
    ax::any_buffer _abuf;
    ax::Vec2* verticesObj1;
    ax::Vec2* verticesObj2;
    ax::Vec2* verticesObjMorph;
    //   ax::Vec2* v;
    int segments = 0;

    bool state = false;
};

class DrawNodeExPerformanceTest : public DrawNodeExBaseTest
{
public:
    struct fireObj
    {
        float x;
        float y;
        float vx;
        float vy;
        ax::Color4F color;
        int life;
    };

    CREATE_FUNC(DrawNodeExPerformanceTest);

    DrawNodeExPerformanceTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);

    fireObj* createFireObjs(int count);

private:
    ax::Vec2* canon;
    ax::Vec2* projectile;
    ax::Vec2* wall;
    fireObj* ember;

    ax::extension::DrawNodeEx* drawNodeEx;
    ax::any_buffer _abuf;

    int ScreenWidth = 400;
    // DesktopWidth(0)
    int ScreenHeight = 400;
    // DesktopHeight(0)
    // variable assignments
    const int max = 750;
    // This sets the size of the ember array
    int fuse = 0;
    // countdown timer until next burst
    int old = 0;
    //     index to oldest ember
    int young = -1;
    // index to youngest ember
    int impulse = 20;
    // determines average explosion force
    float drag = 0.97;  // 0 < drag < 1;        smaller = more drag
    float gravity = 0.02;

    int burnTime = 150;
    //   determines average ember lifetime
    int fuseTime = 25;
    // determines average fuse time
    float tupi = 2 * M_PI, pow, alfa;
    float midx = ScreenWidth / 2;
    //      middle of screen
    float devx = midx * 0.75;
    //   maximum x burst deviation from center
    float nomy = ScreenHeight * 0.45;
    //  nominal y burst location
    float devy = ScreenHeight * 0.25;
    //  maximum y deviation from nomy
    int a, n, x, y, emberCount, cmix, c1, c2, shape;
};

class DrawNode2Test : public DrawNodeExBaseTest
{
public:
    CREATE_FUNC(DrawNode2Test);

    DrawNode2Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};
//
// class Issue11942Test : public DrawPrimitivesBaseTest
//{
// public:
//    CREATE_FUNC(Issue11942Test);
//
//    Issue11942Test();
//
//    virtual std::string title() const override;
//    virtual std::string subtitle() const override;
//};

class IndividualThicknessTest : public DrawNodeExBaseTest
{
public:
    CREATE_FUNC(IndividualThicknessTest);

    IndividualThicknessTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);

    void initSliders();
    void changeThickness(ax::Object* pSender, ax::ui::Slider::EventType type);
    // void changeLineWidth(Object* pSender, ax::ui::Slider::EventType type);

private:
    ax::extension::DrawNodeEx* drawNodeEx;
    // ax::Label* _lineWidthLabel;
    // float lineWidth = 0;
    ax::Label* _thicknessLabel;
    float thickness = 1.0f;
};

class DrawNode2PolygonTest : public DrawNodeExBaseTest
{
public:
    CREATE_FUNC(DrawNode2PolygonTest);

    DrawNode2PolygonTest();
    void drawDirection(const ax::Vec2* vec, const int size, ax::Vec2 offset);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);

private:
   ax::extension::DrawNodeEx* drawNodeEx;
   ax::Vec2 center;
};

class DrawNode2FilledPolygonTest : public DrawNodeExBaseTest
{
public:
    CREATE_FUNC(DrawNode2FilledPolygonTest);

    DrawNode2FilledPolygonTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class DrawPieTest : public DrawNodeExBaseTest
{
public:
    CREATE_FUNC(DrawPieTest);

    DrawPieTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);

    void initSliders();
    void changeStartAngle(ax::Object* pSender, ax::ui::Slider::EventType type);
    void changeEndAngle(ax::Object* pSender, ax::ui::Slider::EventType type);
    void changeRotation(ax::Object* pSender, ax::ui::Slider::EventType type);
    void changeThickness(ax::Object* pSender, ax::ui::Slider::EventType type);

private:
    ax::extension::DrawNodeEx* drawNode;
    ax::Label* _StartAngleLabel;
    float startAngle = 0;
    ax::Label* _EndAngleLabel;
    float endAngle = 0;
    ax::Label* _RotationLabel;
    float rotation = 0;
    ax::Label* _ThicknessLabel;
    float thickness = 0;
};

class DrawNodePart1Test : public DrawNodeExBaseTest
{
public:
    CREATE_FUNC(DrawNodePart1Test);

    DrawNodePart1Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);

    void sliderCallback(ax::Object* sender, ax::ui::Slider::EventType type);
    void listviewCallback(ax::Object* sender, ax::ui::ListView::EventType type);

    void drawAll();

private:
    ax::ui::Slider* createSlider();
    ax::ui::ListView* createListView();
    ax::extension::DrawNodeEx* draw;
    ax::DrawNode* draw1;
    ax::Vec2* verticess;

    int _currentSeletedItemIndex = 0;
    float thickness              = 0.1f;
    int count                    = 1;
    bool isDirty                 = false;

    ax::Label* label;
    ax::Label* label1;
    ax::Label* label2;
    ax::Label* label3;
};

class DrawNodePart2Test : public DrawNodeExBaseTest
{
public:
    CREATE_FUNC(DrawNodePart2Test);

    DrawNodePart2Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);

    void sliderCallback(ax::Object* sender, ax::ui::Slider::EventType type);
    void listviewCallback(ax::Object* sender, ax::ui::ListView::EventType type);

    void drawAll();

private:
    ax::ui::Slider* createSlider();
    ax::ui::ListView* createListView();
    ax::extension::DrawNodeEx* draw;

    int _currentSeletedItemIndex = 0;
    int count                    = 1;
    bool isDirty                 = false;

    ax::Label* label;
    ax::Label* label1;
    ax::Label* label2;
    ax::Label* label3;
};

class DrawNodeExHeartTest : public DrawNodeExBaseTest
{
public:
    CREATE_FUNC(DrawNodeExHeartTest);

    DrawNodeExHeartTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);

private:
    ax::extension::DrawNodeEx* drawNodeEx;
    ax::Vec2* heart;
    const int totalFrames = 240;
    ax::any_buffer _abuf;
};


class ISSUE_1888_DrawNodeDrawsInWrongOrder : public DrawNodeExBaseTest
{
public:
    CREATE_FUNC(ISSUE_1888_DrawNodeDrawsInWrongOrder);

    ISSUE_1888_DrawNodeDrawsInWrongOrder();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);

private:
    ax::extension::DrawNodeEx* drawNodeEx;
    ax::Vec2* heart;
    const int totalFrames = 240;
    ax::any_buffer _abuf;
};

