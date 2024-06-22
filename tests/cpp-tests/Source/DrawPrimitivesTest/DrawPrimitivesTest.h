/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include <string>

DEFINE_TEST_SUITE(DrawPrimitivesTests);

class DrawPrimitivesBaseTest : public TestCase
{
public:
    virtual std::string title() const override;
};

class DrawNodeTest : public DrawPrimitivesBaseTest
{
public:
    CREATE_FUNC(DrawNodeTest);

    DrawNodeTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class DrawNodeBackwardsAPITest : public DrawPrimitivesBaseTest
{
public:
    CREATE_FUNC(DrawNodeBackwardsAPITest);

    DrawNodeBackwardsAPITest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class BetterCircleRendering : public DrawPrimitivesBaseTest
{
public:
    CREATE_FUNC(BetterCircleRendering);

    BetterCircleRendering();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);

    void initSliders();
    void changeThreshold(Object* pSender, ax::ui::Slider::EventType type);
    void changeLineWidth(Object* pSender, ax::ui::Slider::EventType type);

private:
    ax::DrawNode* drawNode;
    ax::Label* _lineWidthLabel;
    float lineWidth = 0;
    ax::Label* _thresholdLabel;
    float threshold = 0;
};

class Issue829Test : public DrawPrimitivesBaseTest
{
public:
    CREATE_FUNC(Issue829Test);

    Issue829Test();
    void drawDirection(const ax::Vec2* vec, const int size, ax::Vec2 offset);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class Issue1319Test : public DrawPrimitivesBaseTest
{
public:
    CREATE_FUNC(Issue1319Test);

    Issue1319Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class DrawNodeTestNewFeature1 : public DrawPrimitivesBaseTest
{
public:
    CREATE_FUNC(DrawNodeTestNewFeature1);

    DrawNodeTestNewFeature1();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);

    void initSliders();
    void changeStartAngle(Object* pSender, ax::ui::Slider::EventType type);
    void changeEndAngle(Object* pSender, ax::ui::Slider::EventType type);
    void changeAngle(Object* pSender, ax::ui::Slider::EventType type);

private:
    ax::DrawNode* drawNode;
    ax::Label* _StartAngleLabel;
    float startAngle = 0;
    ax::Label* _EndAngleLabel;
    float endAngle = 0;
    ax::Label* _AngleLabel;
    float angle = 0;
};
