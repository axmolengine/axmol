/****************************************************************************
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
#pragma once

#include "axmol.h"
#include "ui/CocosGUI.h"
#include "../BaseTest.h"
#include "2d/DrawNode.h"

DEFINE_TEST_SUITE(DrawNodeTests);

class DrawNodeBaseTest : public TestCase
{

protected:

    enum sliderType
    {
        AngleStart = 0,
        AngleEnd,
        Rotation,
        Thickness,
        sliderTypeLast
    };
    enum drawMethodes
    {
        Line = 0,
        Rect,
        Circle,
        QuadBezier,
        CubicBezier,
        CardinalSpline,
        CatmullRom,
        Poly,
        Polygon,
        Dot,
        Point,
        Points,
        Triangle,
        Segment,
        SolidTriangle,
        SolidCircle,
        SolidPoly,
        SolidRect,
        Star,
        SolidStar,
        LAST
    };

    std::string drawMethods[drawMethodes::LAST] = { "drawLine",
        "drawRect",
        "drawCircle",
        "drawQuadBezier",
        "drawCubicBezier",
        "drawCardinalSpline",
        "drawCatmullRom",
        "drawPoly",
        "drawPolygon",
        "drawDot",
        "drawPoint",
        "drawPoints",
        "drawTriangle",
        "drawSegment",
        "drawSolidTriangle",
        "drawSolidCircle",
        "drawSolidPoly",
        "drawSolidRect",
        "drawStar",
        "drawSolidStar", };

public:

    DrawNodeBaseTest();

    void onChangedRadioButtonSelect(ax::ui::RadioButton* radioButton, ax::ui::RadioButton::EventType type);
    void listviewCallback(ax::Object* sender, ax::ui::ListView::EventType type);
    void setDrawOrder(Object* sender);
    void setTransform(Object* sender);

    void update(float dt);

    virtual std::string title() const override;
    void drawDirection(const ax::Vec2* vec, const int size, ax::Vec2 offset);

    void initSliders();

    void changeStartAngle(ax::Object* pSender, ax::ui::Slider::EventType type);
    void changeEndAngle(ax::Object* pSender, ax::ui::Slider::EventType type);
    void changeRotation(ax::Object* pSender, ax::ui::Slider::EventType type);
    void changeThickness(ax::Object* pSender, ax::ui::Slider::EventType type);

    // using from https://github.com/intmainreturn00/AwesomeNode/
    void generateDataPoints();

    ax::PointArray *pts = nullptr;
    ax::PointArray *pts2 = nullptr;
    float defY, defY2, dev;
    const int n = 50;
    const int grid = 10;
    const int margin = 20;
    ax::Size screen;
    ax::Vec2  sixth;

protected:

    int _currentSeletedItemIndex = 0;

    //UI stuff
    ax::ui::Slider* slider[sliderType::sliderTypeLast];
    ax::Label* sliderLabel[sliderType::sliderTypeLast];
    float sliderValue[sliderType::sliderTypeLast];


    ax::ui::RadioButtonGroup* _radioButtonGroup;
    ax::Layer* _uiLayer;
    ax::ui::Layout* _widget;
    int selectedRadioButton;

    ax::MenuItemFont* menuItemDrawOrder;
    ax::MenuItemFont* menuItemTransform;

    // DrawNode stuff
    ax::DrawNode* drawNode = nullptr;
    ax::DrawNode* drawNodeArray[10];
 

    // Window stuff
    ax::Vec2 origin;
    ax::Vec2 size;
    ax::Vec2 center;
};

class DrawNodePictureTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodePictureTest);

    DrawNodePictureTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt) override;
};

class DrawNodeMorphTest_SolidPolygon : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeMorphTest_SolidPolygon);

    DrawNodeMorphTest_SolidPolygon();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);
    void onEnter();

private:
    ax::Vec2* verticesObj1[10];
    ax::Vec2* verticesObj2[10];
    ax::Vec2* verticesObjMorph[10];
    ax::Color4F color[10];
    float rad[10];
    bool state[10];

    int segments = 40;
};

class DrawNodeMorphTest_Polygon : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeMorphTest_Polygon);

    DrawNodeMorphTest_Polygon();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);
    void onEnter();

private:
    ax::Vec2* verticesObj1[10];
    ax::Vec2* verticesObj2[10];
    ax::Vec2* verticesObjMorph[10];
    ax::Color4F color[10];
    float rad[10];
    bool state[10];

    int segments = 40;
};

class DrawNodeThicknessTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeThicknessTest);

    DrawNodeThicknessTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void update(float dt);
    void onEnter();

private:
    // ax::Label* _lineWidthLabel;
    // float lineWidth = 0;
    ax::Label* _thicknessLabel;
    float thickness = 1.0f;
};

class DrawNodeVersionsTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeVersionsTest);

    DrawNodeVersionsTest();
    void drawDirection(const ax::Vec2* vec, const int size, ax::Vec2 offset);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt) override;

private:
    ax::Vec2 center;
};

class DrawNodeFilledPolygonTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeFilledPolygonTest);

    DrawNodeFilledPolygonTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class DrawNodePieTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodePieTest);

    DrawNodePieTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void update(float dt);
    void onEnter();

};

class DrawNodeMethodsTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeMethodsTest);

    DrawNodeMethodsTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void update(float dt);
    void onEnter();

    void drawAll();

private:
    ax::ui::ListView* createListView();

    ax::Vec2* verticess;

    int count = 1;

    ax::Label* labelRound;
    ax::Label* labelSquare;
    ax::Label* labelButt;

    ax::ui::RadioButtonGroup* _radioButtonGroup;
    int selectedRadioButton;
};


class DrawNodeDrawInWrongOrder_Issue1888 : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeDrawInWrongOrder_Issue1888);

    DrawNodeDrawInWrongOrder_Issue1888();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt) override;

private:
    ax::Vec2* heart;
    const int totalFrames = 240;
};

class DrawNodeAxmolTest2 : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeAxmolTest2);

    DrawNodeAxmolTest2();
    void onChangedRadioButtonSelect(ax::ui::RadioButton* radioButton, ax::ui::RadioButton::EventType type);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);
    void drawAllv2(ax::DrawNode* drawNode, bool drawOrder);
    void drawAllv1(ax::DrawNode* drawNode);

private:
    ax::Vec2 s;

    ax::ui::RadioButtonGroup* _radioButtonGroup;
    int selectedRadioButton;
};

class DrawNodeIssueTester : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeIssueTester);

    DrawNodeIssueTester();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void update(float dt);
    void onEnter();

    void changeThreshold(Object* pSender, ax::ui::Slider::EventType type);
    void changeLineWidth(Object* pSender, ax::ui::Slider::EventType type);

private:
    ax::Label* _lineWidthLabel;
    float lineWidth = 0;
    ax::Label* _thresholdLabel;
    float threshold = 0;
};


class DrawNodeThicknessStressTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeThicknessStressTest);

    DrawNodeThicknessStressTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void update(float dt);
    void onEnter();

    void changeThreshold(Object* pSender, ax::ui::Slider::EventType type);
    void changeLineWidth(Object* pSender, ax::ui::Slider::EventType type);

private:
    ax::Label* _lineWidthLabel;
    float lineWidth = 0;
    ax::Label* _thresholdLabel;
    float threshold = 0;
};






class DrawNodeSpLinesTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeSpLinesTest);

    DrawNodeSpLinesTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void addNewControlPoint(ax::Vec2 p);
    void update(float dt);

private:
    ax::DrawNode* drawNodeCP = nullptr;
    std::vector<ax::Vec2> points;
    ax::PointArray* array;
};

#if defined(AX_PLATFORM_PC)
class CandyMixEeffect : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(CandyMixEeffect);

    CandyMixEeffect();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void update(float dt);
    void renderLine(float x1, float x2, float y, ax::Color4F color, float angle);

private:
    std::vector<ax::Vec2> points;
    ax::PointArray* array;

    ax::ui::RadioButtonGroup* _radioButtonGroup;
    int selectedRadioButton;
};
#endif
