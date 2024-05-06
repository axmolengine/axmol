/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd..
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

#include "DrawNodeExTest.h"

#if defined(_WIN32)
#    pragma push_macro("TRANSPARENT")
#    undef TRANSPARENT
#endif

USING_NS_AX;
USING_NS_AX_EXT;

using namespace std;

Vec2 vertices1[] = {
    {45.750000f, 144.375000f},  {75.500000f, 136.875000f},  {75.500000f, 159.125000f},  {100.250000f, 161.375000f},
    {65.500000f, 181.375000f},  {102.250000f, 179.125000f}, {95.000000f, 215.125000f},  {129.331467f, 189.926208f},
    {131.371460f, 206.366196f}, {139.651474f, 192.446198f}, {161.851471f, 200.606201f}, {151.000000f, 220.375000f},
    {110.500000f, 244.375000f}, {153.750000f, 238.125000f}, {142.500000f, 253.875000f}, {220.750000f, 259.375000f},
    {250.500000f, 244.375000f}, {168.750000f, 241.875000f}, {182.250000f, 154.125000f}, {190.250000f, 227.375000f},
    {196.500000f, 197.375000f}, {208.750000f, 210.625000f}, {220.750000f, 194.375000f}, {208.750000f, 176.375000f},
    {253.250000f, 173.875000f}, {243.750000f, 154.125000f}, {213.750000f, 161.375000f}, {202.250000f, 139.875000f},
    {236.000000f, 131.875000f}, {218.500000f, 120.875000f}, {206.500000f, 125.625000f}, {184.500000f, 110.375000f},
    {157.000000f, 108.625000f}, {147.500000f, 96.625000f},  {153.750000f, 85.125000f},  {147.500000f, 75.375000f},
    {126.500000f, 74.125000f},  {110.500000f, 86.625000f},  {127.750000f, 85.125000f},  {135.250000f, 91.125000f},
    {135.250000f, 97.875000f},  {124.000000f, 93.875000f},  {115.500000f, 100.875000f}, {115.500000f, 111.875000f},
    {135.250000f, 108.625000f}, {151.000000f, 124.125000f}, {90.500000f, 131.875000f},  {113.250000f, 120.875000f},
    {88.000000f, 116.875000f},  {106.000000f, 103.875000f}, {88.000000f, 97.875000f}};

Vec2 vertices2[] = {
    {290.250000f, 98.1250000f}, {235.000000f, 90.8750000f}, {270.500000f, 109.875000f}, {235.000000f, 119.125000f},
    {275.250000f, 145.875000f}, {249.500000f, 145.875000f}, {249.500000f, 178.125000f}, {275.250000f, 187.375015f},
    {294.750488f, 168.333344f}, {311.250000f, 181.125000f}, {257.000000f, 213.625015f}, {338.500000f, 193.125000f},
    {300.000000f, 211.125015f}, {333.750000f, 211.125015f}, {368.250000f, 206.625000f}, {377.000000f, 178.125000f},
    {421.750000f, 170.125000f}, {416.250000f, 115.375000f}, {391.250000f, 157.875000f}, {338.500000f, 131.625000f},
    {362.750000f, 131.625000f}, {362.750000f, 106.875000f}, {306.500000f, 119.125000f}, {324.250000f, 85.1250000f},
    {227.500000f, 61.8750000}};

DrawNodeExTests::DrawNodeExTests()
{
    ADD_TEST_CASE(DrawNodePart1Test);
    ADD_TEST_CASE(DrawNodePart2Test);
    ADD_TEST_CASE(DrawNode2Test);
    ADD_TEST_CASE(IndividualThicknessTest);
    ADD_TEST_CASE(DrawPieTest);
//    ADD_TEST_CASE(DrawNode2PolygonTest);
    ADD_TEST_CASE(DrawNode2FilledPolygonTest);
}

string DrawNodeExBaseTest::title() const
{
    return "No title";
}

// DrawNodeTest
DrawNode2Test::DrawNode2Test()
{
    auto s = Director::getInstance()->getWinSize();

    auto draw = ax::extension::DrawNodeEx::create();
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

    auto array = ax::PointArray::create(20);
    array->addControlPoint(Vec2(0.0f, 0.0f));
    array->addControlPoint(Vec2(80.0f, 80.0f));
    array->addControlPoint(Vec2(s.width - 80, 80.0f));
    array->addControlPoint(Vec2(s.width - 80, s.height - 80));
    array->addControlPoint(Vec2(80.0f, s.height - 80));
    array->addControlPoint(Vec2(80.0f, 80.0f));
    array->addControlPoint(Vec2(s.width / 2, s.height / 2));
    draw->drawCardinalSpline(array, 0.5f, 50, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    auto array2 = ax::PointArray::create(20);
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

    auto draw1 = ax::extension::DrawNodeEx::create();
    this->addChild(draw1, 10);
    draw1->setLineWidth(4);
    draw1->drawLine(Vec2(0.0f, s.height), Vec2(s.width, s.height - 20), Color4F::YELLOW);
    draw1->drawLine(Vec2(0.0f, 0.0f), Vec2(s.width, s.height - 20), Color4F::YELLOW);

    draw->runAction(RepeatForever::create(Sequence::create(FadeIn::create(1.2), FadeOut::create(1.2), NULL)));
    draw1->runAction(RepeatForever::create(Sequence::create(FadeIn::create(1.2), FadeOut::create(1.2), NULL)));
}

string DrawNode2Test::title() const
{
    return "DrawNode vs DrawNodeEx";
}

string DrawNode2Test::subtitle() const
{
    return "";
}

//
//
//
IndividualThicknessTest::IndividualThicknessTest()
{
    // Add lines to see the correct "scale of the 'rings'" changing the window size

    auto draw = DrawNodeEx::create();
    draw->setLineWidth(1);
    addChild(draw, 10);

    for (float y = 0; y < VisibleRect::top().y; y += 10)
    {
        draw->drawLine({VisibleRect::left().x, y}, {VisibleRect::right().x, y}, Color4B::GRAY);
    }
    initSliders();

    drawNodeEx = DrawNodeEx::create();
    addChild(drawNodeEx, 10);

    auto s = Director::getInstance()->getWinSize();
    // auto draw1 = DrawNodeExt::create();
    // this->addChild(draw1, 10);
    //// draw1->setLineWidth(4);
    // draw1->drawLine(Vec2(0.0f, s.height), Vec2(s.width, s.height - 20), Color4F::YELLOW);
    // draw1->drawLine(Vec2(0.0f, 0.0f), Vec2(s.width, s.height - 20), Color4F::YELLOW);

    scheduleUpdate();
}

void IndividualThicknessTest::changeThickness(ax::Object* pSender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        ax::ui::Slider* sliderthickness = dynamic_cast<ax::ui::Slider*>(pSender);
        thickness                       = static_cast<float>(sliderthickness->getPercent()) / 10.0f;
        _thicknessLabel->setString("Thickness " + Value(thickness).asString());
    }
}

void IndividualThicknessTest::initSliders()
{

    // Layer => LayerRadialGradientTest   ############################################# Peter Eismann

    auto vsize             = Director::getInstance()->getVisibleSize();
    ax::ui::Slider* slider = ax::ui::Slider::create();
    slider->setPercent(0);
    slider->loadBarTexture("cocosui/sliderTrack.png");
    slider->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    slider->loadProgressBarTexture("cocosui/sliderProgress.png");
    slider->setPosition(Vec2(vsize.width / 2, vsize.height / 6));

    slider->addEventListener(AX_CALLBACK_2(IndividualThicknessTest::changeThickness, this));

    auto ttfConfig  = TTFConfig("fonts/arial.ttf", 8);
    _thicknessLabel = Label::createWithTTF(ttfConfig, "Thickness ");
    addChild(_thicknessLabel, 20);
    _thicknessLabel->setPosition(Vec2(vsize.width / 2, vsize.height / 6 + 15));

    addChild(slider, 20);
}

void IndividualThicknessTest::update(float dt)
{
    auto s = Director::getInstance()->getWinSize();

    drawNodeEx->clear();
    auto color = Color4F::GREEN;
    drawNodeEx->drawCircle(VisibleRect::center() /*- Vec2(120.0f, 0.0f)*/, 60, AX_DEGREES_TO_RADIANS(77), 36, false,
                           color, thickness);

    // drawNodeEx->drawCircle(VisibleRect::center() /*- Vec2(120.0f, 0.0f)*/, 60, AX_DEGREES_TO_RADIANS(77), 36, false,
    //                      Color4F::BLACK, 1);

    // drawNodeEx->drawLine(Vec2(0.0f, s.height), Vec2(s.width, s.height - 20), Color4F::YELLOW, thickness);
    // drawNodeEx->drawLine(Vec2(0.0f, 0.0f), Vec2(s.width, s.height - 20), Color4F::YELLOW, thickness);

    // draw a rectangles
    drawNodeEx->drawRect(Vec2(123, 123), Vec2(227, 227), Color4F(1, 1, 0, 1), thickness);
    // drawNodeEx->drawRect(Vec2(123, 123), Vec2(227, 227), Color4F::BLACK, 1);

    drawNodeEx->drawRect(Vec2(115, 130), Vec2(130, 115), Vec2(115, 100), Vec2(100, 115), Color4F::MAGENTA, thickness);
    // drawNodeEx->drawRect(Vec2(115, 130), Vec2(130, 115), Vec2(115, 100), Vec2(100, 115), Color4F::BLACK, 1);

    drawNodeEx->drawLine(Vec2(200.0f, s.height - 20), Vec2(s.width - 100, s.height - 20), Color4F::YELLOW, thickness);
    drawNodeEx->drawLine(Vec2(300.0f, 100.0f), Vec2(s.width - 200, s.height - 120), Color4F::GREEN, thickness);

    Vec2 vertices24[] = {
        {45.750000f, 144.375000f},  {75.500000f, 136.875000f},  {75.500000f, 159.125000f},  {100.250000f, 161.375000f},
        {65.500000f, 181.375000f},  {102.250000f, 179.125000f}, {95.000000f, 215.125000f},  {129.331467f, 189.926208f},
        {131.371460f, 206.366196f}, {139.651474f, 192.446198f}, {161.851471f, 200.606201f}, {151.000000f, 220.375000f},
        {110.500000f, 244.375000f}, {153.750000f, 238.125000f}, {142.500000f, 253.875000f}, {220.750000f, 259.375000f},
        {250.500000f, 244.375000f}, {168.750000f, 241.875000f}, {182.250000f, 154.125000f}, {190.250000f, 227.375000f},
        {196.500000f, 197.375000f}, {208.750000f, 210.625000f}, {220.750000f, 194.375000f}, {208.750000f, 176.375000f},
        {253.250000f, 173.875000f}, {243.750000f, 154.125000f}, {213.750000f, 161.375000f}, {202.250000f, 139.875000f},
        {236.000000f, 131.875000f}, {218.500000f, 120.875000f}, {206.500000f, 125.625000f}, {184.500000f, 110.375000f},
        {157.000000f, 108.625000f}, {147.500000f, 96.625000f},  {153.750000f, 85.125000f},  {147.500000f, 75.375000f},
        {126.500000f, 74.125000f},  {110.500000f, 86.625000f},  {127.750000f, 85.125000f},  {135.250000f, 91.125000f},
        {135.250000f, 97.875000f},  {124.000000f, 93.875000f},  {115.500000f, 100.875000f}, {115.500000f, 111.875000f},
        {135.250000f, 108.625000f}, {151.000000f, 124.125000f}, {90.500000f, 131.875000f},  {113.250000f, 120.875000f},
        {88.000000f, 116.875000f},  {106.000000f, 103.875000f}, {88.000000f, 97.875000f},
    };
    drawNodeEx->drawPolygon(vertices24, sizeof(vertices24) / sizeof(vertices24[0]), Color4B::TRANSPARENT, thickness,
                            Color4F::RED);
    // drawNodeEx->drawPolygon(vertices24, sizeof(vertices24) / sizeof(vertices24[0]), Color4B::TRANSPARENT, 0.5f,
    //                       Color4F::BLACK);

    // open random color poly
    Vec2 vertices[] = {Vec2(0.0f, 0.0f), Vec2(50.0f, 50.0f), Vec2(100.0f, 50.0f), Vec2(100.0f, 100.0f),
                       Vec2(50.0f, 100.0f)};
    drawNodeEx->drawPoly(vertices, 5, false, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f), thickness);

    // closed random color poly
    Vec2 vertices2[] = {Vec2(30.0f, 130.0f), Vec2(30.0f, 230.0f), Vec2(50.0f, 200.0f)};
    drawNodeEx->drawPoly(vertices2, 3, true, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f), thickness);

    // Draw some beziers
    // Draw some beziers
    drawNodeEx->drawQuadBezier(Vec2(s.width - 150, s.height - 150), Vec2(s.width - 70, s.height - 10),
                               Vec2(s.width - 10, s.height - 10), 10, Color4F::BLUE, thickness);

    drawNodeEx->drawQuadBezier(Vec2(0.0f + 100, s.height - 100), Vec2(s.width / 2, s.height / 2),
                               Vec2(s.width - 100, s.height - 100), 50, Color4F::RED, thickness);

    drawNodeEx->drawCubicBezier(VisibleRect::center(), Vec2(VisibleRect::center().x + 30, VisibleRect::center().y + 50),
                                Vec2(VisibleRect::center().x + 60, VisibleRect::center().y - 50), VisibleRect::right(),
                                100, Color4F::WHITE, thickness);

    drawNodeEx->drawCubicBezier(Vec2(s.width - 250, 40.0f), Vec2(s.width - 70, 100.0f), Vec2(s.width - 30, 250.0f),
                                Vec2(s.width - 10, s.height - 50), 10, Color4F::GRAY, thickness);

    auto array = ax::PointArray::create(20);
    array->addControlPoint(Vec2(0.0f, 0.0f));
    array->addControlPoint(Vec2(80.0f, 80.0f));
    array->addControlPoint(Vec2(s.width - 80, 80.0f));
    array->addControlPoint(Vec2(s.width - 80, s.height - 80));
    array->addControlPoint(Vec2(80.0f, s.height - 80));
    array->addControlPoint(Vec2(80.0f, 80.0f));
    array->addControlPoint(Vec2(s.width / 2, s.height / 2));
    drawNodeEx->drawCardinalSpline(array, 0.5f, 50, Color4F::MAGENTA, thickness);

    auto array2 = ax::PointArray::create(20);
    array2->addControlPoint(Vec2(s.width / 2, 30.0f));
    array2->addControlPoint(Vec2(s.width - 80, 30.0f));
    array2->addControlPoint(Vec2(s.width - 80, s.height - 80));
    array2->addControlPoint(Vec2(s.width / 2, s.height - 80));
    array2->addControlPoint(Vec2(s.width / 2, 30.0f));
    drawNodeEx->drawCatmullRom(array2, 50, Color4F::ORANGE, thickness);
}

string IndividualThicknessTest::title() const
{
    return "Individual line width";
}

string IndividualThicknessTest::subtitle() const
{
    return "";
}

DrawPieTest::DrawPieTest()
{
    drawNode = DrawNodeEx::create();
    addChild(drawNode, 10);

    initSliders();

    scheduleUpdate();
}

void DrawPieTest::changeEndAngle(ax::Object* pSender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        ax::ui::Slider* sEndAngle = dynamic_cast<ax::ui::Slider*>(pSender);
        endAngle                  = sEndAngle->getPercent() * 3.6;
        _EndAngleLabel->setString("endAngle: (" + Value(endAngle).asString() + ")");
    }
}

void DrawPieTest::changeStartAngle(ax::Object* pSender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        ax::ui::Slider* sStartAngle = dynamic_cast<ax::ui::Slider*>(pSender);
        startAngle                  = sStartAngle->getPercent() * 3.6;
        _StartAngleLabel->setString("startAngle: (" + Value(startAngle).asString() + ")");
    }
}

void DrawPieTest::changeRotation(ax::Object* pSender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        ax::ui::Slider* sStartAngle = dynamic_cast<ax::ui::Slider*>(pSender);
        rotation                    = sStartAngle->getPercent() * 3.6;
        _RotationLabel->setString("Rotation: (" + Value(rotation).asString() + ")");
    }
}

void DrawPieTest::changeThickness(ax::Object* pSender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        ax::ui::Slider* sStartAngle = dynamic_cast<ax::ui::Slider*>(pSender);
        thickness                   = sStartAngle->getPercent() * 0.1;
        _ThicknessLabel->setString("Thickness: (" + Value(thickness).asString() + ")");
    }
}

void DrawPieTest::initSliders()
{
    rotation   = 324;
    endAngle   = 30;
    startAngle = 0;
    thickness  = 1;

    auto vsize                       = Director::getInstance()->getVisibleSize();
    ax::ui::Slider* sliderStartAngle = ax::ui::Slider::create();
    sliderStartAngle->setPercent(startAngle);
    sliderStartAngle->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    sliderStartAngle->loadBarTexture("cocosui/sliderTrack.png");
    sliderStartAngle->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    sliderStartAngle->loadProgressBarTexture("cocosui/sliderProgress.png");
    sliderStartAngle->setPosition(Vec2(20, vsize.height / 6));

    sliderStartAngle->addEventListener(AX_CALLBACK_2(DrawPieTest::changeStartAngle, this));

    auto ttfConfig   = TTFConfig("fonts/arial.ttf", 8);
    _StartAngleLabel = Label::createWithTTF(ttfConfig, "StartAngle (" + Value(startAngle).asString() + ")");
    _StartAngleLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    addChild(_StartAngleLabel, 20);
    _StartAngleLabel->setPosition(sliderStartAngle->getPosition() + Vec2(sliderStartAngle->getContentSize().x + 20, 0));

    addChild(sliderStartAngle, 20);

    ax::ui::Slider* sliderEndAngle = ax::ui::Slider::create();
    sliderEndAngle->setPercent(endAngle);
    sliderEndAngle->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    sliderEndAngle->loadBarTexture("cocosui/sliderTrack.png");
    sliderEndAngle->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    sliderEndAngle->loadProgressBarTexture("cocosui/sliderProgress.png");
    sliderEndAngle->setPosition(Vec2(20, vsize.height / 6 + 25));
    sliderEndAngle->addEventListener(AX_CALLBACK_2(DrawPieTest::changeEndAngle, this));

    _EndAngleLabel = Label::createWithTTF(ttfConfig, "endAngle (" + Value(endAngle).asString() + ")");
    _EndAngleLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    addChild(_EndAngleLabel, 20);
    _EndAngleLabel->setPosition(sliderEndAngle->getPosition() + Vec2(sliderEndAngle->getContentSize().x + 20, 0));
    addChild(sliderEndAngle, 20);

    ax::ui::Slider* sliderRotation = ax::ui::Slider::create();
    sliderRotation->setPercent(rotation);
    sliderRotation->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    sliderRotation->loadBarTexture("cocosui/sliderTrack.png");
    sliderRotation->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    sliderRotation->loadProgressBarTexture("cocosui/sliderProgress.png");
    sliderRotation->setPosition(Vec2(20, vsize.height / 6 + 50));
    sliderRotation->addEventListener(AX_CALLBACK_2(DrawPieTest::changeRotation, this));

    _RotationLabel = Label::createWithTTF(ttfConfig, "Rotation (" + Value(rotation).asString() + ")");
    _RotationLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    addChild(_RotationLabel, 20);
    _RotationLabel->setPosition(sliderRotation->getPosition() + Vec2(sliderRotation->getContentSize().x + 20, 0));
    addChild(sliderRotation, 20);

    ax::ui::Slider* sliderThickness = ax::ui::Slider::create();
    sliderThickness->setPercent(thickness);
    sliderThickness->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    sliderThickness->loadBarTexture("cocosui/sliderTrack.png");
    sliderThickness->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    sliderThickness->loadProgressBarTexture("cocosui/sliderProgress.png");
    sliderThickness->setPosition(Vec2(20, vsize.height / 6 + 75));
    sliderThickness->addEventListener(AX_CALLBACK_2(DrawPieTest::changeThickness, this));

    _ThicknessLabel = Label::createWithTTF(ttfConfig, "Thickness (" + Value(thickness).asString() + ")");
    _ThicknessLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    addChild(_ThicknessLabel, 20);
    _ThicknessLabel->setPosition(sliderThickness->getPosition() + Vec2(sliderThickness->getContentSize().x + 20, 0));
    addChild(sliderThickness, 20);
}

void DrawPieTest::update(float dt)
{
    drawNode->clear();
    drawNode->drawPie(VisibleRect::center() - Vec2(170.0f, -35.0f), 50, rotation, startAngle, endAngle, 1.0f, 1.0f,
                      Color4F::RED, Color4F::BLUE, drawNode->DrawMode::Fill, thickness);
    drawNode->drawPie(VisibleRect::center() - Vec2(60.0f, -35.0f), 50, rotation, startAngle, endAngle, 1.0f, 1.0f,
                      Color4F::TRANSPARENT, Color4F::BLUE, drawNode->DrawMode::Outline, thickness);
    drawNode->drawPie(VisibleRect::center() + Vec2(60.0f, 35.0f), 50, rotation, startAngle, endAngle, 1.0f, 1.0f,
                      Color4F::RED, Color4F::BLUE, drawNode->DrawMode::Line, thickness);
    drawNode->drawPie(VisibleRect::center() + Vec2(170.0f, 35.0f), 50, rotation, startAngle, endAngle, 1.0f, 1.0f,
                      Color4F::RED, Color4F::BLUE, drawNode->DrawMode::Semi, thickness);
}

string DrawPieTest::title() const
{
    return "drawPie";
}

string DrawPieTest::subtitle() const
{
    return "Filled, Outlined, Line, Semi";
}

DrawNode2PolygonTest::DrawNode2PolygonTest()
{
    auto director = Director::getInstance();
    // director->setClearColor(Color4F(0, 0, 0, 0));
    auto origin = director->getVisibleOrigin();
    auto size   = director->getVisibleSize();
    center      = Vec2(origin.x + size.width / 2 + 50, origin.y + size.height / 2);

    float o    = 80;
    float w    = 20;
    float h    = 50;
    drawNodeEx = DrawNodeEx::create();
    addChild(drawNodeEx);
  //  drawNodeEx->setPosition(center);

    {  // star

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

        drawNodeEx->setDNPosition(center);
        drawNodeEx->setDNCenter(star[0]);
        //   drawNodeEx->setDNRotation(rot);
        drawNodeEx->drawPolygon(star, sizeof(star) / sizeof(star[0]), Color4F(0.0f, 0.0f, 0.7f, 0.5f), 1,
                                Color4F::BLUE);
    }

    scheduleUpdate();
}

void DrawNode2PolygonTest::drawDirection(const Vec2* vec, const int size, Vec2 offset)
{
    for (size_t i = 0; i < size; i++)
    {
        auto label = Label::createWithTTF(std::to_string(i).c_str(), "fonts/Marker Felt.ttf", 10);
        this->addChild(label);
        label->setPosition(vec[i] + offset);
    }
}

string DrawNode2PolygonTest::title() const
{
    return "DrawNodeVersion and Rotation";
}

string DrawNode2PolygonTest::subtitle() const
{
    return "YELLOW=v1 ORANGE=v2";
}

void DrawNode2PolygonTest::update(float dt)
{
    float rot = 0.1;
    float o   = 80;
    float w   = 20;
    float h   = 50;
    {  // star

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

/*        drawNodeEx->setDNPosition(center);
        drawNodeEx->setDNCenter(star[0]);
        drawNodeEx->setDNRotation(rot)*/;
        drawNodeEx->drawPolygon(star, sizeof(star) / sizeof(star[0]), Color4F(0.0f, 0.0f, 0.7f, 0.5f), 1,
                                Color4F::BLUE);
    }
    {
        int x = 0;
        int y = 0;
        Vec2 vertices[4];
        drawNodeEx->setScale(0.5);
        Color4F color;
        for (int iy = 0; iy < 5; iy++)
        {
            x = 0;
            for (int ix = 0; ix < 13; ix++)
            {
                vertices[0] = Vec2(x + o + w, y + o - h);
                vertices[1] = Vec2(x + o + w * 2, y + o);
                vertices[2] = Vec2(x + o + w * 2 + h, y + o + w);
                vertices[3] = Vec2(x + o + w * 2, y + o + w * 2);

                if (AXRANDOM_0_1() > 0.5f)
                {
                    drawNodeEx->setIsConvex(true);
                    color = Color4F::YELLOW;
                }
                else
                {
                    drawNodeEx->setIsConvex(false);
                    color = Color4F::ORANGE;
                }
                //drawNodeEx->setDNRotation(rot);
                //drawNodeEx->setDNCenter(vertices[0]);
                //drawNodeEx->setDNPosition(Vec2(-70.f, 60.f));
                drawNodeEx->drawPolygon(vertices, 4, Color4F(0.7f, 0.7f, 0.7f, 0.5f), 1, color);
                drawNodeEx->setIsConvex(false);
                x += 70;
            }
            y += 80;
        }
    }

    rot += 0.1;
}

DrawNode2FilledPolygonTest::DrawNode2FilledPolygonTest()
{

    DrawNodeEx* drawNode[3];
    for (int i = 0; i < 3; i++)
    {
        drawNode[i] = DrawNodeEx::create();
        addChild(drawNode[i]);
    }

    // draw a solid circle
    drawNode[1]->drawSolidCircle(VisibleRect::center() + Vec2(140.0f, 0.0f), 40, AX_DEGREES_TO_RADIANS(90), 30, 2.0f,
                                 2.0f, Color4F::BLUE);

    drawNode[1]->drawSolidCircle(VisibleRect::center() + Vec2(-40.0f, 0.0f), 40, AX_DEGREES_TO_RADIANS(90), 30, 2.0f,
                                 2.0f, Color4F::WHITE);

    drawNode[0]->setPosition(Vec2(-30, -20));
    // drawNodeEx[0]->drawPolygon(vertices24, sizeof(vertices24) / sizeof(vertices24[0]), Color4F::RED, 0.3f,
    //                          Color4F::GREEN);

    drawNode[2]->drawPolygon(vertices2, sizeof(vertices2) / sizeof(vertices2[0]), Color4F::GREEN, 0.3f,
                             Color4F::YELLOW);

    drawNode[2]->drawPolygon(vertices1, sizeof(vertices1) / sizeof(vertices1[0]), Color4F::TRANSPARENT, 5.3f,
                             Color4F::RED);
}

string DrawNode2FilledPolygonTest::title() const
{
    return "Filled Polygon Test #2";
}

string DrawNode2FilledPolygonTest::subtitle() const
{
    return "";
}

// DrawMethodesThicknessTest
DrawNodePart1Test::DrawNodePart1Test()
{
    _currentSeletedItemIndex = 0;

    auto director = Director::getInstance();
    director->setClearColor(Color4F(0, 0, 0, 0));
    auto origin = director->getVisibleOrigin();
    auto size   = director->getVisibleSize();
    Vec2 center(origin.x + size.width / 2 + 50, origin.y + size.height / 2);

    auto listview = createListView();
    listview->setPosition(Vec2(0.0f, 90.0f));
    addChild(listview);

    draw = DrawNodeEx::create();
    draw->setScale(0.5);
    draw->setPosition(center);
    addChild(draw);

    draw1 = DrawNode::create();
    draw1->setScale(0.5);
    draw1->setPosition(size / 4);
    addChild(draw1);

    auto thicknessSlider = createSlider();
    thicknessSlider->setPosition(Vec2(center.x, 60.0f));
    addChild(thicknessSlider);

    label = Label::createWithTTF("Value: ", "fonts/Marker Felt.ttf", 10);
    label->setPosition(Vec2(Vec2(center.x, 80.0f)));
    this->addChild(label, 1);
    isDirty = true;
    scheduleUpdate();
}

std::string DrawNodePart1Test::title() const
{
    return "Scale/Rotation/LineWidth/Position";
}

string DrawNodePart1Test::subtitle() const
{
    return "";
}

ax::ui::Slider* DrawNodePart1Test::createSlider()
{
    auto slider = ui::Slider::create();
    slider->setTouchEnabled(true);
    slider->loadBarTexture("cocosui/sliderTrack.png");
    slider->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    slider->loadProgressBarTexture("cocosui/sliderProgress.png");
    slider->addEventListener(AX_CALLBACK_2(DrawNodePart1Test::sliderCallback, this));
    slider->setTag(101);
    slider->setPercent(10);

    return slider;
}

void DrawNodePart1Test::listviewCallback(ax::Object* sender, ax::ui::ListView::EventType type)
{
    // clear all text to white
    auto listview = static_cast<ax::ui::ListView*>(sender);
    for (auto&& item : listview->getItems())
    {
        static_cast<ax::ui::Text*>(item)->setColor(ax::Color3B::WHITE);
    }
    _currentSeletedItemIndex = (int)listview->getCurSelectedIndex();
    listview->getItem(_currentSeletedItemIndex)->setColor(ax::Color3B::RED);
    isDirty = true;
}

void DrawNodePart1Test::sliderCallback(ax::Object* sender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        auto slider = dynamic_cast<ax::ui::Slider*>(sender);
        thickness   = slider->getPercent() * 0.1f;
    }
    isDirty = true;
}

ax::ui::ListView* DrawNodePart1Test::createListView()
{
    auto listview = ax::ui::ListView::create();

    auto drawLine = ax::ui::Text::create();
    drawLine->setString("drawLine");
    drawLine->setTouchEnabled(true);
    listview->pushBackCustomItem(drawLine);

    auto drawRect = ax::ui::Text::create();
    drawRect->setString("drawRect");
    drawRect->setTouchEnabled(true);
    listview->pushBackCustomItem(drawRect);

    auto drawCircle = ax::ui::Text::create();
    drawCircle->setString("drawCircle");
    drawCircle->setTouchEnabled(true);
    listview->pushBackCustomItem(drawCircle);

    auto drawQuadBezier = ax::ui::Text::create();
    drawQuadBezier->setString("drawQuadBezier");
    drawQuadBezier->setTouchEnabled(true);
    listview->pushBackCustomItem(drawQuadBezier);

    auto drawCubicBezier = ax::ui::Text::create();
    drawCubicBezier->setString("drawCubicBezier");
    drawCubicBezier->setTouchEnabled(true);
    listview->pushBackCustomItem(drawCubicBezier);

    auto drawCardinalSpline = ax::ui::Text::create();
    drawCardinalSpline->setString("drawCardinalSpline");
    drawCardinalSpline->setTouchEnabled(true);
    listview->pushBackCustomItem(drawCardinalSpline);

    auto drawCatmullRom = ax::ui::Text::create();
    drawCatmullRom->setString("drawCatmullRom");
    drawCatmullRom->setTouchEnabled(true);
    listview->pushBackCustomItem(drawCatmullRom);

    auto drawPoly = ax::ui::Text::create();
    drawPoly->setString("drawPoly");
    drawPoly->setTouchEnabled(true);
    listview->pushBackCustomItem(drawPoly);

    auto drawPolygon = ax::ui::Text::create();
    drawPolygon->setString("drawPolygon");
    drawPolygon->setTouchEnabled(true);
    listview->pushBackCustomItem(drawPolygon);

    listview->setContentSize(drawLine->getContentSize() * 9);
    listview->setCurSelectedIndex(0);
    listview->setTouchEnabled(true);
    listview->addEventListener(
        (ui::ListView::ccListViewCallback)AX_CALLBACK_2(DrawNodePart1Test::listviewCallback, this));
    listview->setTag(100);

    listview->getItem(_currentSeletedItemIndex)->setColor(Color3B::RED);

    return listview;
}

void DrawNodePart1Test::update(float dt)
{
    // if (isDirty== true)
    {
        drawAll();
        isDirty = false;
    }
}

void DrawNodePart1Test::drawAll()
{
    isDirty               = false;
    static float rotation = 0.1;
    rotation += 0.1;
    auto s = Director::getInstance()->getWinSize();

    draw->clear();
    label->setString("Thickness: (" + Value(thickness).asString() + ")");

    switch (_currentSeletedItemIndex)
    {
    case 0:
    {
        draw->setRotation(rotation * 3);
        draw->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        draw->setScale(0.3);

        // draw->setPosition(0.1);
        Vec2 gear1 = {280.f, 320.f};
        Vec2 gear2 = {160.f, 320.f};
        Vec2 gear3 = {200.f, 200.f};
        Vec2 gear4 = {s.width - 200, s.height - 200};

        draw->drawLine(gear2, gear4, Color4F::RED, thickness);  // line
        draw->setDNCenter(gear1);
        draw->setDNRotation(rotation + 45);
        draw->drawStar(Vec2(gear1), 30, 60, 8, Color4F::BLUE, 4.0);
        draw->setDNRotation(-rotation);
        draw->setDNCenter(gear2);
        draw->drawSolidStar(gear2, 30, 60, 8, Color4F::GREEN, Color4F::YELLOW, 4.0);
 
        draw->resetDNValues();
        draw->drawLine(gear2, gear1, Color4F::RED, thickness);  // line
        draw->setDNCenter(gear4);
        draw->setDNRotation(rotation + 45);
        draw->drawStar(gear3, 30, 60, 18, Color4F::RED, 1.0);
        draw->drawLine(gear3, gear4, Color4F::YELLOW, thickness);  // line
        draw->resetDNValues();
        draw->setDNRotation(rotation - 45);
        draw->setDNCenter(gear4);
        draw->drawStar(gear4, 40, 60, 60, Color4F::GREEN, 1.0);
        draw->resetDNValues();
        isDirty = true;
        break;
    }
    case 1:
    {
        // drawRect
        draw->drawRect(Vec2(23, 23), Vec2(7, 7), Color4F(1, 1, 0, 1), thickness);
        draw->drawRect(Vec2(15, 30), Vec2(30, 15), Vec2(15, 0), Vec2(0, 15),
                       Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1), thickness);
        draw->drawRect(Vec2(123, 123), Vec2(227, 227), Color4F(1, 1, 0, 1), thickness);
        draw->drawRect(Vec2(115, 130), Vec2(130, 115), Vec2(115, 100), Vec2(100, 115), Color4F::MAGENTA, thickness);
        break;
    }
    case 2:
    {
        // drawCircle
        draw->drawCircle(VisibleRect::center() + Vec2(140, 0), 100, AX_DEGREES_TO_RADIANS(90), 50, true, 1.0f, 2.0f,
                         Color4F(1.0f, 0.0f, 0.0f, 0.5f), thickness);
        draw->drawCircle(VisibleRect::center() - Vec2(140, 0), 50, AX_DEGREES_TO_RADIANS(90), 30, true,
                         Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f), thickness);
        break;
    }
    case 3:
    {
        // drawCubicBezier
        draw->drawQuadBezier(Vec2(s.width - 150, s.height - 150), Vec2(s.width - 70, s.height - 10),
                             Vec2(s.width - 10, s.height - 10), 10, Color4F::BLUE, thickness);
        draw->drawQuadBezier(Vec2(0.0f + 100, s.height - 100), Vec2(s.width / 2, s.height / 2),
                             Vec2(s.width - 100, s.height - 100), 50, Color4F::RED, thickness);
        break;
    }
    case 4:
    {
        // drawCubicBezier
        draw->drawCubicBezier(VisibleRect::center(), Vec2(VisibleRect::center().x + 30, VisibleRect::center().y + 50),
                              Vec2(VisibleRect::center().x + 60, VisibleRect::center().y - 50), VisibleRect::right(),
                              100, Color4F::WHITE, thickness);
        draw->drawCubicBezier(Vec2(s.width - 250, 40.0f), Vec2(s.width - 70, 100.0f), Vec2(s.width - 30, 250.0f),
                              Vec2(s.width - 10, s.height - 50), 10, Color4F::GRAY, thickness);
        break;
    }
    case 5:
    {
        // drawCardinalSpline
        auto array = ax::PointArray::create(20);
        array->addControlPoint(Vec2(0.0f, 0.0f));
        array->addControlPoint(Vec2(80.0f, 80.0f));
        array->addControlPoint(Vec2(s.width - 80, 80.0f));
        array->addControlPoint(Vec2(s.width - 80, s.height - 80));
        array->addControlPoint(Vec2(80.0f, s.height - 80));
        array->addControlPoint(Vec2(80.0f, 80.0f));
        array->addControlPoint(Vec2(s.width / 2, s.height / 2));
        draw->drawCardinalSpline(array, 0.5f, 50, Color4F::MAGENTA, thickness);

        auto array2 = ax::PointArray::create(20);
        array2->addControlPoint(Vec2(s.width / 2, 80.0f));
        array2->addControlPoint(Vec2(s.width - 80, 80.0f));
        array2->addControlPoint(Vec2(s.width - 80, s.height - 80));
        array2->addControlPoint(Vec2(s.width / 2, s.height - 80));
        array2->addControlPoint(Vec2(s.width / 2, 80.0f));
        draw->drawCardinalSpline(array2, 5.0f, 50, Color4F::ORANGE, thickness);
        break;
    }
    case 6:
    {
        // drawCatmullRom
        auto array2 = ax::PointArray::create(20);
        array2->addControlPoint(Vec2(s.width / 2, 80.0f));
        array2->addControlPoint(Vec2(s.width - 80, 80.0f));
        array2->addControlPoint(Vec2(s.width - 80, s.height - 80));
        array2->addControlPoint(Vec2(s.width / 2, s.height - 80));
        array2->addControlPoint(Vec2(s.width / 2, 80.0f));
        draw->drawCatmullRom(array2, 50, Color4F::ORANGE, thickness);

        auto array = ax::PointArray::create(20);
        array->addControlPoint(Vec2(0.0f, 0.0f));
        array->addControlPoint(Vec2(80.0f, 80.0f));
        array->addControlPoint(Vec2(s.width - 80, 80.0f));
        array->addControlPoint(Vec2(s.width - 80, s.height - 80));
        array->addControlPoint(Vec2(80.0f, s.height - 80));
        array->addControlPoint(Vec2(80.0f, 80.0f));
        array->addControlPoint(Vec2(s.width / 2, s.height / 2));
        draw->drawCatmullRom(array, 50, Color4F::MAGENTA, thickness);
        break;
    }
    case 7:
    {
        // drawPoly
        // for (int n = 0; n < 10; n++)
        bool isReal = false;

        // draw->drawPoly(verticess, sizeof(verticess) / sizeof(verticess[0]), true,
        //                Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f), thickness);

        Vec2 vertices[5] = {Vec2(0.0f, 0.0f), Vec2(50.0f, 50.0f), Vec2(100.0f, 50.0f), Vec2(100.0f, 100.0f),
                            Vec2(50.0f, 100.0f)};
        draw->drawPoly(vertices, 5, false, Color4B::BLUE, thickness);

        Vec2 vertices2[3] = {Vec2(30.0f, 130.0f), Vec2(30.0f, 230.0f), Vec2(50.0f, 200.0f)};
        draw->drawPoly(vertices2, 3, true, Color4B::GREEN, thickness);

        draw->drawPoly(vertices1, sizeof(vertices1) / sizeof(vertices1[0]), true, Color4B::RED, thickness);

        break;
    }
    case 8:
    {
        // drawPolygon

        draw->setDNScale(Vec2(thickness, thickness));
        draw->setDNPosition(Vec2(0, 0));
        draw->setDNRotation(0);
        draw->drawPolygon(vertices1, sizeof(vertices1) / sizeof(vertices1[0]), Color4F::GREEN, thickness,
                          Color4F::YELLOW);
        draw->setDNPosition(Vec2(0, 0));
        draw->setDNRotation(thickness);
        draw->setDNScale(Vec2(thickness, thickness));
        draw->setDNCenter(vertices1[0]);
        draw->drawPolygon(vertices1, sizeof(vertices1) / sizeof(vertices1[0]), Color4F::GREEN, thickness,
                          Color4F::YELLOW);

        draw1->clear();
        draw1->setPosition(Vec2(200, 0));
        draw1->setScale(thickness);
        draw1->drawPolygon(vertices1, sizeof(vertices1) / sizeof(vertices1[0]), Color4F::RED, thickness,
                           Color4F::YELLOW);

        break;
    }
    default:
        break;
    }
}

// DrawFilledPrimitivesTest
DrawNodePart2Test::DrawNodePart2Test()
{
    _currentSeletedItemIndex = 0;

    auto director = Director::getInstance();
    director->setClearColor(Color4F(0, 0, 0, 0));
    auto origin = director->getVisibleOrigin();
    auto size   = director->getVisibleSize();
    Vec2 center(origin.x + size.width / 2 + 50, origin.y + size.height / 2);

    auto listview = createListView();
    listview->setPosition(Vec2(0.0f, 90.0f));
    addChild(listview);

    draw = DrawNodeEx::create();
    draw->setScale(0.5);
    draw->setPosition(size / 4);
    addChild(draw);

    auto thicknessSlider = createSlider();
    thicknessSlider->setPosition(Vec2(center.x, 60.0f));
    addChild(thicknessSlider);

    label = Label::createWithTTF("Count: ", "fonts/Marker Felt.ttf", 10);
    label->setPosition(Vec2(Vec2(center.x, 80.0f)));
    this->addChild(label, 1);

    scheduleUpdate();
}

std::string DrawNodePart2Test::title() const
{
    return "DrawNode #2 Test";
}

string DrawNodePart2Test::subtitle() const
{
    return "";
}

ax::ui::Slider* DrawNodePart2Test::createSlider()
{
    auto slider = ui::Slider::create();
    slider->setTouchEnabled(true);
    slider->loadBarTexture("cocosui/sliderTrack.png");
    slider->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    slider->loadProgressBarTexture("cocosui/sliderProgress.png");
    slider->addEventListener(AX_CALLBACK_2(DrawNodePart2Test::sliderCallback, this));
    //     slider->setRotation(90);
    slider->setTag(101);
    slider->setPercent(10);

    return slider;
}

void DrawNodePart2Test::listviewCallback(ax::Object* sender, ax::ui::ListView::EventType type)
{
    // clear all text to white
    auto listview = static_cast<ax::ui::ListView*>(sender);
    for (auto&& item : listview->getItems())
    {
        static_cast<ax::ui::Text*>(item)->setColor(ax::Color3B::WHITE);
    }
    _currentSeletedItemIndex = (int)listview->getCurSelectedIndex();
    listview->getItem(_currentSeletedItemIndex)->setColor(ax::Color3B::RED);
}

void DrawNodePart2Test::sliderCallback(ax::Object* sender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        auto slider = dynamic_cast<ax::ui::Slider*>(sender);
        count       = slider->getPercent() * 10;
    }
}

ax::ui::ListView* DrawNodePart2Test::createListView()
{
    auto listview = ax::ui::ListView::create();

    auto drawLine = ax::ui::Text::create();
    drawLine->setString("drawDot");
    drawLine->setTouchEnabled(true);
    listview->pushBackCustomItem(drawLine);

    auto drawSegment = ax::ui::Text::create();
    drawSegment->setString("drawPoint");
    drawSegment->setTouchEnabled(true);
    listview->pushBackCustomItem(drawSegment);

    auto drawTriangle = ax::ui::Text::create();
    drawTriangle->setString("drawPoints");
    drawTriangle->setTouchEnabled(true);
    listview->pushBackCustomItem(drawTriangle);

    auto drawRect = ax::ui::Text::create();
    drawRect->setString("drawTriangle");
    drawRect->setTouchEnabled(true);
    listview->pushBackCustomItem(drawRect);

    auto drawCircle = ax::ui::Text::create();
    drawCircle->setString("drawSegment");
    drawCircle->setTouchEnabled(true);
    listview->pushBackCustomItem(drawCircle);

    auto drawQuadBezier = ax::ui::Text::create();
    drawQuadBezier->setString("drawSolidCircle");
    drawQuadBezier->setTouchEnabled(true);
    listview->pushBackCustomItem(drawQuadBezier);

    auto drawCubicBezier = ax::ui::Text::create();
    drawCubicBezier->setString("drawSolidPoly");
    drawCubicBezier->setTouchEnabled(true);
    listview->pushBackCustomItem(drawCubicBezier);

    auto drawCardinalSpline = ax::ui::Text::create();
    drawCardinalSpline->setString("drawSolidRect");
    drawCardinalSpline->setTouchEnabled(true);
    listview->pushBackCustomItem(drawCardinalSpline);

    listview->setContentSize(drawLine->getContentSize() * 8);
    listview->setCurSelectedIndex(0);
    listview->setTouchEnabled(true);
    listview->addEventListener(
        (ui::ListView::ccListViewCallback)AX_CALLBACK_2(DrawNodePart2Test::listviewCallback, this));
    listview->setTag(100);

    listview->getItem(_currentSeletedItemIndex)->setColor(Color3B::RED);

    return listview;
}

void DrawNodePart2Test::update(float dt)
{

    auto s = Director::getInstance()->getWinSize();

    draw->clear();

    label->setString("Count: (" + Value(count).asString() + ")");
    switch (_currentSeletedItemIndex)
    {
    case 0:
    {
        // Draw 10 circles/dots
        for (int i = 0; i < count / 100; i++)
        {
            draw->drawDot(Vec2(100.f + AXRANDOM_0_1() * VisibleRect::rightTop().x,
                               100.f + AXRANDOM_0_1() * VisibleRect::rightTop().y),
                          AXRANDOM_0_1() * 20 + 20, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));
        }
        break;
    }
    case 1:
    {
        for (int i = 0; i < 100; i++)
        {
            draw->drawPoint(Vec2(i * 7.0f, 50.0f), (float)i / 5 + 1,
                            Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));
        }
        break;
    }
    case 2:
    {
        // drawPoints
        for (int i = 0; i < count; i++)
        {
            Vec2 position[] = {
                {60 + AXRANDOM_0_1() * VisibleRect::rightTop().x, 60 + AXRANDOM_0_1() * VisibleRect::rightTop().y},
                {70 + AXRANDOM_0_1() * VisibleRect::rightTop().x, 70 + AXRANDOM_0_1() * VisibleRect::rightTop().y},
                {60 + AXRANDOM_0_1() * VisibleRect::rightTop().x, 60 + AXRANDOM_0_1() * VisibleRect::rightTop().y},
                {70 + AXRANDOM_0_1() * VisibleRect::rightTop().x, 70 + AXRANDOM_0_1() * VisibleRect::rightTop().y}};
            draw->drawPoints(position, 4, 5, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1));
        }

        break;
    }
    case 3:
    {
        // drawTriangle
        for (int i = 0; i < count; i++)
        {
            Vec2 pos = Vec2(AXRANDOM_0_1() * VisibleRect::rightTop().x, AXRANDOM_0_1() * VisibleRect::rightTop().y);
            draw->drawTriangle(pos + Vec2(10.0f, 10.0f), pos + Vec2(70.0f, 30.0f), pos + Vec2(100.0f, 140.0f),
                               Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));
        }
        break;
    }
    case 4:
    {
        // Draw segment
        draw->drawSegment(Vec2(20.0f, s.height), Vec2(20.0f, s.height / 2), 10,
                          Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

        draw->drawSegment(Vec2(10.0f, s.height / 2), Vec2(s.width / 2, s.height / 2), 40,
                          Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));
        break;
    }
    case 5:
    {
        // draw a solid circle
        for (int i = 0; i < count; i++)
        {
            draw->drawSolidCircle(
                Vec2(AXRANDOM_0_1() * VisibleRect::rightTop().x, AXRANDOM_0_1() * VisibleRect::rightTop().y),
                AXRANDOM_0_1() * 40.f + 40.f, AX_DEGREES_TO_RADIANS(90), 50, 2.0f, 2.0f,
                Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));
        }
        break;
    }
    case 6:
    {
        // draw a solid poly
        draw->resetDNValues();
        draw->setDNPosition(vertices1[0]);
        draw->setDNRotation(count);
        //    draw->setDNScale(Vec2(count, count));
        draw->setDNCenter(vertices1[0]);
        draw->drawPoly(vertices1, sizeof(vertices1) / sizeof(vertices1[0]), true, Color4F::GREEN, count);
        draw->resetDNValues();
        break;
    }
    case 7:
    {
        // draw a solid rectangle
        for (int i = 0; i < count; i++)
        {
            Vec2 pos = Vec2(AXRANDOM_0_1() * VisibleRect::rightTop().x, AXRANDOM_0_1() * VisibleRect::rightTop().y);
            draw->drawSolidRect(pos, pos + Vec2(20.0f, 20.0f),
                                Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));
        }
        break;
    }
    default:
        break;
    }
}

#if defined(_WIN32)
#    pragma pop_macro("TRANSPARENT")
#endif
