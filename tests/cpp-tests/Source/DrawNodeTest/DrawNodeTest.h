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

#include "axmol/axmol.h"
#include "axmol/ui/CocosGUI.h"
#include "../BaseTest.h"
#include "axmol/2d/DrawNode.h"

DEFINE_TEST_SUITE(DrawNodeTests);

class DrawNodeBaseTest : public TestCase
{

protected:
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

    std::string drawMethods[drawMethodes::LAST] = {
        "drawLine",
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
        "draw(Colored)Triangle",
        "drawSegment",
        "drawSolidTriangle",
        "drawSolidCircle",
        "drawSolidPoly",
        "drawSolidRect",
        "drawStar",
        "drawSolidStar",
    };

public:
    DrawNodeBaseTest();

    void update(float dt) override;
    void onDrawImGui();
    void onExit() override;
    void onEnter() override;

    virtual std::string title() const override;
    void drawDirection(const ax::Vec2* vec, const int size, ax::Vec2 offset);

    int _color;    //
    float _count;  //
    bool _transparent;
    float _nodeScale;         // drawNode->getScale;
    ax::Vec2 _localePos;      // drawNode->_localPosition
    ax::Vec2 _localePivot;    // drawNode->_localPivot
    ax::Vec2 _localeScale;    // drawNode->_localScale
    float _localeThickScale;  // drawNode->_thicknessScale
    float thickness;
    float _localeRotation;  // drawNode->_localRotation
    float _angelStart;      // angle start
    float _angelEnd;        // angle end
    bool _drawOrder;
    bool _transform;
    int flagGUI        = -1;
    ax::Scene* _target = nullptr;

    // using from https://github.com/intmainreturn00/AwesomeNode/
    void generateDataPoints();

    ax::PointArray* pts  = nullptr;
    ax::PointArray* pts2 = nullptr;
    float defY, defY2, dev;
    const int n      = 50;
    const int grid   = 10;
    const int margin = 20;
    ax::Size screen;
    ax::Vec2 sixth;

protected:
    int _currentSeletedItemIndex = 0;

    // DrawNode stuff
    ax::DrawNode* drawNode = nullptr;
    ax::DrawNode* drawNodeArray[100];  // max 100

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

const int NUM_POINTS = 10000;
class DrawNodeJellyFishTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeJellyFishTest);

    DrawNodeJellyFishTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt) override;
    void CalculatePoint(float x, float y, float t, float& px, float& py);
    void DrawFrame();
    void UpdateJellyPoints();

private:
    ax::Vec2 dots[NUM_POINTS];
};

class DrawNodeMorphTest_SolidPolygon : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeMorphTest_SolidPolygon);

    DrawNodeMorphTest_SolidPolygon();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt) override;
    void onEnter() override;

private:
    ax::Vec2* verticesObj1[100];
    ax::Vec2* verticesObj2[100];
    ax::Vec2* verticesObjMorph[100];
    ax::Color color[100];
    float rad[100];
    bool state[100];
    int oldCount = 0;

    int segments = 40;
};

class DrawNodeMorphTest_Polygon : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeMorphTest_Polygon);

    DrawNodeMorphTest_Polygon();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt) override;
    void onEnter() override;

private:
    ax::Vec2* verticesObj1[100];
    ax::Vec2* verticesObj2[100];
    ax::Vec2* verticesObjMorph[100];
    ax::Color color[100];
    float rad[100];
    bool state[100];
    int oldCount = 0;

    int segments = 40;
};

class DrawNodeLineDrawTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeLineDrawTest);

    DrawNodeLineDrawTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void update(float dt) override;
    void onEnter() override;
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

    void update(float dt) override;
    void onEnter() override;
};

class DrawNodeMethodsTest : public DrawNodeBaseTest
{

public:
    CREATE_FUNC(DrawNodeMethodsTest);

    DrawNodeMethodsTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void update(float dt) override;
    void onEnter() override;

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

class DrawNodeAxmolTest2 : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeAxmolTest2);

    DrawNodeAxmolTest2();
    void onChangedRadioButtonSelect(ax::Object* sender, ax::ui::RadioButton::EventType type);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt) override;
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

    void update(float dt) override;
    void onEnter() override;
};

class DrawNodeSpLinesTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeSpLinesTest);

    DrawNodeSpLinesTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void update(float dt) override;
};

class DrawNodeCircleTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeCircleTest);

    DrawNodeCircleTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class DrawNodeSolidCircleTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeSolidCircleTest);

    DrawNodeSolidCircleTest();
    void showCircles();

    int objects = 100;

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

private:
    int primitive = 0;
    std::chrono::microseconds duration;
    ax::Label* autoTestLabel = nullptr;
    ax::Rect rect;
};

class DrawNodePolygonTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodePolygonTest);

    DrawNodePolygonTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class DrawNodeSpLinesOpenClosedTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeSpLinesOpenClosedTest);

    DrawNodeSpLinesOpenClosedTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void onPointerUp(ax::PointerEvent* event);
    void addNewControlPoint(ax::Vec2 p);
    void update(float dt) override;

private:
    ax::DrawNode* drawNodeCP = nullptr;
    std::vector<ax::Vec2> points;
    ax::PointArray* array;
};

class DrawNodeRoundRectTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeRoundRectTest);

    DrawNodeRoundRectTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class DrawNodeButtonTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeButtonTest);

    DrawNodeButtonTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class DrawNodeSideEffectTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeSideEffectTest);

    DrawNodeSideEffectTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void update(float dt) override;
    void onEnter() override;

    ax::DrawNode* issue3094;
};

#if defined(AX_PLATFORM_PC)

const int GRID_W = 800;
const int GRID_H = 600;
class DrawNodeGameOfLifeTest : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(DrawNodeGameOfLifeTest);

    DrawNodeGameOfLifeTest();
    ~DrawNodeGameOfLifeTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void update(float dt) override;

    int visibleSizeX;
    int visibleSizeY;

    bool* grid;
    bool* nextGrid;
    int* age;
    static const int colorCount = 10;
    ax::Color color[colorCount];
    tlx::pod_vector<ax::Vec2> arrea[colorCount];
};

class CandyMixEeffect : public DrawNodeBaseTest
{
public:
    CREATE_FUNC(CandyMixEeffect);

    CandyMixEeffect();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void update(float dt) override;
    void renderLine(float x1, float x2, float y, ax::Color color, float angle);

private:
    std::vector<ax::Vec2> points;
    ax::PointArray* array;
};
#endif
