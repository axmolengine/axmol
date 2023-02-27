/****************************************************************************
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

#include "DrawPrimitivesTest.h"
#include "renderer/CCRenderer.h"
#include "renderer/CCCustomCommand.h"

USING_NS_AX;

using namespace std;

DrawPrimitivesTests::DrawPrimitivesTests()
{
    ADD_TEST_CASE(DrawNodeTest);
    ADD_TEST_CASE(Issue11942Test);
    ADD_TEST_CASE(BetterCircleRendering);
    ADD_TEST_CASE(Issue829Test);
}

string DrawPrimitivesBaseTest::title() const
{
    return "No title";
}

// DrawNodeTest
DrawNodeTest::DrawNodeTest()
{
    auto s = Director::getInstance()->getWinSize();

    auto draw = DrawNode::create();
    addChild(draw, 10);

    draw->drawPoint(Vec2(s.width / 2 - 120, s.height / 2 - 120), 10,
                    Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1));

    draw->drawPoint(Vec2(s.width / 2 + 120, s.height / 2 + 120), 10,
                    Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1));

    // draw 4 small points
    Vec2 position[] = {Vec2(60, 60), Vec2(70, 70), Vec2(60, 70), Vec2(70, 60)};
    draw->drawPoints(position, 4, 5, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1));

    // draw a line
    draw->drawLine(Vec2(0, 0), Vec2(s.width, s.height), Color4F(1.0, 0.0, 0.0, 0.5));

    // draw a rectangle
    draw->drawRect(Vec2(23, 23), Vec2(7, 7), Color4F(1, 1, 0, 1));

    draw->drawRect(Vec2(15, 30), Vec2(30, 15), Vec2(15, 0), Vec2(0, 15),
                   Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1));

    // draw a circle
    draw->drawCircle(VisibleRect::center() + Vec2(140, 0), 100, AX_DEGREES_TO_RADIANS(90), 50, true, 1.0f, 2.0f,
                     Color4F(1.0f, 0.0f, 0.0f, 0.5f));

    draw->drawCircle(VisibleRect::center() - Vec2(140, 0), 50, AX_DEGREES_TO_RADIANS(90), 30, false,
                     Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));

    // Draw some beziers
    draw->drawQuadBezier(Vec2(s.width - 150, s.height - 150), Vec2(s.width - 70, s.height - 10),
                         Vec2(s.width - 10, s.height - 10), 10,
                         Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    draw->drawQuadBezier(Vec2(0.0f, s.height), Vec2(s.width / 2, s.height / 2), Vec2(s.width, s.height), 50,
                         Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    draw->drawCubicBezier(VisibleRect::center(), Vec2(VisibleRect::center().x + 30, VisibleRect::center().y + 50),
                          Vec2(VisibleRect::center().x + 60, VisibleRect::center().y - 50), VisibleRect::right(), 100,
                          Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    draw->drawCubicBezier(Vec2(s.width - 250, 40.0f), Vec2(s.width - 70, 100.0f), Vec2(s.width - 30, 250.0f),
                          Vec2(s.width - 10, s.height - 50), 10,
                          Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    auto array = PointArray::create(20);
    array->addControlPoint(Vec2(0.0f, 0.0f));
    array->addControlPoint(Vec2(80.0f, 80.0f));
    array->addControlPoint(Vec2(s.width - 80, 80.0f));
    array->addControlPoint(Vec2(s.width - 80, s.height - 80));
    array->addControlPoint(Vec2(80.0f, s.height - 80));
    array->addControlPoint(Vec2(80.0f, 80.0f));
    array->addControlPoint(Vec2(s.width / 2, s.height / 2));
    draw->drawCardinalSpline(array, 0.5f, 50, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    auto array2 = PointArray::create(20);
    array2->addControlPoint(Vec2(s.width / 2, 30.0f));
    array2->addControlPoint(Vec2(s.width - 80, 30.0f));
    array2->addControlPoint(Vec2(s.width - 80, s.height - 80));
    array2->addControlPoint(Vec2(s.width / 2, s.height - 80));
    array2->addControlPoint(Vec2(s.width / 2, 30.0f));
    draw->drawCatmullRom(array2, 50, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    // open random color poly
    Vec2 vertices[] = {Vec2(0.0f, 0.0f), Vec2(50.0f, 50.0f), Vec2(100.0f, 50.0f), Vec2(100.0f, 100.0f),
                       Vec2(50.0f, 100.0f)};
    draw->drawPoly(vertices, 5, false, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));

    // closed random color poly
    Vec2 vertices2[] = {Vec2(30.0f, 130.0f), Vec2(30.0f, 230.0f), Vec2(50.0f, 200.0f)};
    draw->drawPoly(vertices2, 3, true, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));

    // Draw 10 circles
    for (int i = 0; i < 10; i++)
    {
        draw->drawDot(Vec2(s.width / 2, s.height / 2), 10 * (10 - i),
                      Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));
    }

    // Draw polygons
    Vec2 points[] = {Vec2(s.height / 4, 0.0f), Vec2(s.width, s.height / 5), Vec2(s.width / 3 * 2, s.height)};
    draw->drawPolygon(points, sizeof(points) / sizeof(points[0]), Color4F(1.0f, 0.0f, 0.0f, 0.5f), 4,
                      Color4F(0.0f, 0.0f, 1.0f, 0.5f));

    // star poly (triggers buggs)
    {
        const float o = 80;
        const float w = 20;
        const float h = 50;
        Vec2 star[]   = {
            Vec2(o + w, o - h), Vec2(o + w * 2, o),                  // lower spike
            Vec2(o + w * 2 + h, o + w), Vec2(o + w * 2, o + w * 2),  // right spike
            //              {o +w, o+w*2+h}, {o,o+w*2},                 // top spike
            //              {o -h, o+w}, {o,o},                         // left spike
        };

        draw->drawPolygon(star, sizeof(star) / sizeof(star[0]), Color4F(1.0f, 0.0f, 0.0f, 0.5f), 1,
                          Color4F(0.0f, 0.0f, 1.0f, 1.0f));
    }

    // star poly (doesn't trigger bug... order is important un tesselation is supported.
    {
        const float o = 180;
        const float w = 20;
        const float h = 50;
        Vec2 star[]   = {
            Vec2(o, o),
            Vec2(o + w, o - h),
            Vec2(o + w * 2, o),  // lower spike
            Vec2(o + w * 2 + h, o + w),
            Vec2(o + w * 2, o + w * 2),  // right spike
            Vec2(o + w, o + w * 2 + h),
            Vec2(o, o + w * 2),  // top spike
            Vec2(o - h, o + w),  // left spike
        };

        draw->drawPolygon(star, sizeof(star) / sizeof(star[0]), Color4F(1.0f, 0.0f, 0.0f, 0.5f), 1,
                          Color4F(0.0f, 0.0f, 1.0f, 1.0f));
    }

    // draw a solid polygon
    Vec2 vertices3[] = {Vec2(60.0f, 160.0f), Vec2(70.0f, 190.0f), Vec2(100.0f, 190.0f), Vec2(90.0f, 160.0f)};
    draw->drawSolidPoly(vertices3, 4, Color4F(1.0f, 1.0f, 0.0f, 1.0f));

    // draw a solid rectangle
    draw->drawSolidRect(Vec2(10.0f, 10.0f), Vec2(20.0f, 20.0f), Color4F(1.0f, 1.0f, 0.0f, 1.0f));

    // draw a solid circle
    draw->drawSolidCircle(VisibleRect::center() + Vec2(140.0f, 0.0f), 40, AX_DEGREES_TO_RADIANS(90), 50, 2.0f, 2.0f,
                          Color4F(0.0f, 1.0f, 0.0f, 1.0f));

    // Draw segment
    draw->drawSegment(Vec2(20.0f, s.height), Vec2(20.0f, s.height / 2), 10, Color4F(0.0f, 1.0f, 0.0f, 1.0f));

    draw->drawSegment(Vec2(10.0f, s.height / 2), Vec2(s.width / 2, s.height / 2), 40, Color4F(1.0f, 0.0f, 1.0f, 0.5f));

    // Draw triangle
    draw->drawTriangle(Vec2(10.0f, 10.0f), Vec2(70.0f, 30.0f), Vec2(100.0f, 140.0f),
                       Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    for (int i = 0; i < 100; i++)
    {
        draw->drawPoint(Vec2(i * 7.0f, 5.0f), (float)i / 5 + 1,
                        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));
    }

    auto draw1 = DrawNode::create();
    this->addChild(draw1, 10);
    draw1->setLineWidth(4);
    draw1->drawLine(Vec2(0.0f, s.height), Vec2(s.width, s.height - 20), Color4F::YELLOW);
    draw1->drawLine(Vec2(0.0f, 0.0f), Vec2(s.width, s.height - 20), Color4F::YELLOW);

    draw->runAction(RepeatForever::create(Sequence::create(FadeIn::create(1.2), FadeOut::create(1.2), NULL)));
    draw1->runAction(RepeatForever::create(Sequence::create(FadeIn::create(1.2), FadeOut::create(1.2), NULL)));
}

string DrawNodeTest::title() const
{
    return "Test DrawNode";
}

string DrawNodeTest::subtitle() const
{
    return "Testing DrawNode - batched draws. Concave polygons are BROKEN";
}

//
// Issue11942Test
//
Issue11942Test::Issue11942Test()
{
    // DrawNode 0 ------------------------------------------
    auto draw0 = DrawNode::create();
    addChild(draw0, 10);

    // draw a circle thickness 10 
    draw0->setLineWidth(10);
    draw0->drawCircle(VisibleRect::center() - Vec2(140.0f, 40.0f), 50, AX_DEGREES_TO_RADIANS(90), 30, false,
                      Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));
    draw0->setLineWidth(1); // thickness 10 will replaced with thickness 1 (also for all 'same' draw commands before!)
    draw0->drawCircle(VisibleRect::center() - Vec2(140.0f, -40.0f), 50, AX_DEGREES_TO_RADIANS(90), 30, false,
                      Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));


    // DrawNode 1 ------------------------------------------
    auto draw1 = DrawNode::create();
    addChild(draw1, 10);

    // draw a second circle thickness 1  
    draw1->setLineWidth(1);
    draw1->drawCircle(VisibleRect::center() + Vec2(140.0f, 40.0f), 50, AX_DEGREES_TO_RADIANS(90), 30, false,
                      Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));
    draw1->setLineWidth(20); // thickness 1 will replaced with thickness 10 (also for all 'same' draw commands before!)
    draw1->drawCircle(VisibleRect::center() + Vec2(140.0f, -40.0f), 50, AX_DEGREES_TO_RADIANS(90), 30, false,
                      Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));
}

string Issue11942Test::title() const
{
    return "GitHub Issue #11942 (axmol #137)";
}

string Issue11942Test::subtitle() const
{
    return "setLineWidth() change the WHOLE DrawNode object 'line with'";
}

//
// drawCircle new feature (better rendering)
//
BetterCircleRendering::BetterCircleRendering()
{
    //Add lines to see the correct "scale of the 'rings'" changing the window size

    auto draw = DrawNode::create();
    draw->setLineWidth(1);
    addChild(draw, 10);

    for (float y = 0; y < VisibleRect::top().y; y += 10)
    {
        draw->drawLine({VisibleRect::left().x, y}, {VisibleRect::right().x, y}, Color4B::GRAY);
    }
    initSliders();

    drawNode = DrawNode::create();
    addChild(drawNode, 10);

    lineWidth = 0;

    scheduleUpdate();
}

void BetterCircleRendering::changeLineWidth(ax::Ref* pSender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        ax::ui::Slider* sliderLineWidth = dynamic_cast<ax::ui::Slider*>(pSender);
        lineWidth                       = sliderLineWidth->getPercent();
        _lineWidthLabel->setString("setLineWidth(" + Value(lineWidth).asString() + ")");
    }
}

void BetterCircleRendering::changeThreshold(ax::Ref* pSender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        ax::ui::Slider* sliderThreshold = dynamic_cast<ax::ui::Slider*>(pSender);
        threshold                       = sliderThreshold->getPercent();
        _thresholdLabel->setString("drawCircle(pos, radius, ..., segments, ..., color, " + Value(threshold).asString() + ")");
    }
}

void BetterCircleRendering::initSliders()
{
    auto vsize             = Director::getInstance()->getVisibleSize();
    ax::ui::Slider* slider = ax::ui::Slider::create();
    slider->setPercent(0);
    slider->loadBarTexture("cocosui/sliderTrack.png");
    slider->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    slider->loadProgressBarTexture("cocosui/sliderProgress.png");
    slider->setPosition(Vec2(vsize.width / 2, vsize.height / 6));

    slider->addEventListener(AX_CALLBACK_2(BetterCircleRendering::changeThreshold, this));

    auto ttfConfig  = TTFConfig("fonts/arial.ttf", 8);
    _thresholdLabel = Label::createWithTTF(ttfConfig, "drawCircle(pos, radius, ..., segments, ..., color, 0)"); 
    addChild(_thresholdLabel, 20);
    _thresholdLabel->setPosition(Vec2(vsize.width / 2, vsize.height / 6 + 15));

    addChild(slider, 20);

    ax::ui::Slider* sliderLineWidth = ax::ui::Slider::create();
    sliderLineWidth->setPercent(0);
    sliderLineWidth->loadBarTexture("cocosui/sliderTrack.png");
    sliderLineWidth->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    sliderLineWidth->loadProgressBarTexture("cocosui/sliderProgress.png");
    sliderLineWidth->setPosition(Vec2(vsize.width / 2, vsize.height / 6 + 35));
    sliderLineWidth->addEventListener(AX_CALLBACK_2(BetterCircleRendering::changeLineWidth, this));

    _lineWidthLabel = Label::createWithTTF(ttfConfig, "setLineWidth(0)");

    addChild(_lineWidthLabel, 20);
    _lineWidthLabel->setPosition(Vec2(vsize.width / 2, vsize.height / 6 + 50));
    addChild(sliderLineWidth, 20);
}

 void BetterCircleRendering::update(float dt)
 {
    drawNode->clear();
    drawNode->setLineWidth(lineWidth);  // value from the slider

    // Old behavior => faster but badly rendering if line width > 5 (= rings)
    drawNode->drawCircle(VisibleRect::center() + Vec2(120.0f, 0.0f), 60, AX_DEGREES_TO_RADIANS(90), 36, false,
                         Color4F::RED);

    // New behavior => slower but good rendering if line width > 5
    auto color = Color4F::GREEN;
    if (lineWidth <= threshold)
    {
        color = Color4F::RED;  // using the faster rendering internal method of drawCircle (old behavior)
    }
    drawNode->drawCircle(VisibleRect::center() - Vec2(120.0f, 0.0f), 60, AX_DEGREES_TO_RADIANS(90), 36, false, color,
                         threshold);
 }

string BetterCircleRendering::title() const
 {
    return "Rendering drawCircle";
}

string BetterCircleRendering::subtitle() const
{
    return "Green: smoother rendering; Red: faster but badly rendering";
}

Issue829Test::Issue829Test()
{
    Vec2 vertices0[] = {{50.0, 20.0}, {100.0, 0.0}, {80.0, 50.0}, {100.0, 100.0},
                        {50.0, 80.0}, {0.0, 100.0}, {20.0, 50.0}, {0, 0}};

    Vec2 vertices4[] = {{362, 148}, {326, 241}, {295, 219}, {258, 88}, {440, 129},
                        {370, 196}, {372, 275}, {348, 257}, {364, 148}};

    Vec2* ver[] = {vertices0, vertices4};

    DrawNode* drawNode[sizeof(ver) + 1];
    for (int i = 0; i < sizeof(ver); i++)
    {
        drawNode[i] = DrawNode::create();
        addChild(drawNode[i]);
    }

    drawNode[0]->drawPolygon(vertices0, sizeof(vertices0) / sizeof(vertices0[0]), Color4F(0.0f, 0.0f, 0.7f, 0.5f), 3,
                             Color4F(0.0f, 0.0f, 1.0f, 1.0f));
    drawNode[0]->setPosition({20, 200});
    drawDirection(vertices0, sizeof(vertices0) / sizeof(vertices0[0]), drawNode[0]->getPosition());

    drawNode[4]->drawPolygon(vertices4, sizeof(vertices4) / sizeof(vertices4[0]), Color4F(0.0f, 0.0f, 0.7f, 0.5f), 3,
                             Color4F(0.0f, 0.0f, 1.0f, 1.0f));
    drawNode[4]->setPosition({-70, -20});
    drawDirection(vertices4, sizeof(vertices4) / sizeof(vertices4[0]), drawNode[4]->getPosition());

    {
        const float o = 80;
        const float w = 20;
        const float h = 50;
        {  // star
            auto drawNode1 = DrawNode::create();
            addChild(drawNode1);
            drawNode1->setPosition(300, 100);
            Vec2 star[] = {
                Vec2(o, o),
                Vec2(o + w, o - h),
                Vec2(o + w * 2, o),  // lower spike
                Vec2(o + w * 2 + h, o + w),
                Vec2(o + w * 2, o + w * 2),  // right spike
                Vec2(o + w, o + w * 2 + h),
                Vec2(o, o + w * 2),  // top spike
                Vec2(o - h, o + w),  // left spike
            };

            drawNode1->drawPolygon(star, sizeof(star) / sizeof(star[0]), Color4F(0.0f, 0.0f, 0.7f, 0.5f), 1,
                                   Color4F(0.0f, 0.0f, 1.0f, 1.0f));

            drawDirection(star, sizeof(star) / sizeof(star[0]), drawNode1->getPosition());
        }

        {  // wrong order
            auto drawNode1 = DrawNode::create();
            addChild(drawNode1);
            drawNode1->setPosition(-80, 20);
            Vec2 wrongOrder[] = {Vec2(o + w, o - h), Vec2(o + w * 2, o), Vec2(o + w * 2 + h, o + w),
                                 Vec2(o + w * 2, o + w * 2)};

            drawNode1->drawPolygon(wrongOrder, sizeof(wrongOrder) / sizeof(wrongOrder[0]),
                                   Color4F(0.0f, 0.0f, 0.7f, 0.5f), 1, Color4F(1.0f, 0.0f, 0.0f, 1.0f));

            drawDirection(wrongOrder, sizeof(wrongOrder) / sizeof(wrongOrder[0]), drawNode1->getPosition());
        }
        {  // correct order
            Vec2 correctOrder[] = {Vec2(o + w * 2, o), Vec2(o + w * 2 + h, o + w), Vec2(o + w * 2, o + w * 2),
                                   Vec2(o + w, o - h)};
            auto drawNode2      = DrawNode::create();
            addChild(drawNode2);
            drawNode2->setPosition({-10, 20});
            drawNode2->drawPolygon(correctOrder, sizeof(correctOrder) / sizeof(correctOrder[0]),
                                   Color4F(0.0f, 0.0f, 0.7f, 0.5f), 1, Color4F(0.0f, 1.0f, 0.0f, 1.0f));

            drawDirection(correctOrder, sizeof(correctOrder) / sizeof(correctOrder[0]), drawNode2->getPosition());
        }
    }
}

void Issue829Test::drawDirection(const Vec2* vec, const int size, Vec2 offset)
{
    for (size_t i = 0; i < size; i++)
    {
        auto label = Label::createWithTTF(std::to_string(i).c_str(), "fonts/Marker Felt.ttf", 10);
        this->addChild(label);
        label->setPosition(vec[i] + offset);
    }
}

string Issue829Test::title() const
{
    return "GitHub Issue #829";
}

string Issue829Test::subtitle() const
{
    return "Red polygon has wrong order!";
}
