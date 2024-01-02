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
#pragma once

#include "axmol.h"
#include "ui/CocosGUI.h"
#include "../BaseTest.h"

#include <string>

DEFINE_TEST_SUITE(DrawNodeExtTests);

class DrawNodeExtBaseTest : public TestCase
{
public:
    virtual std::string title() const override;
 //   ax::Vec2* vertices;
    ax::Vec2* makePolygons();
};

class DrawNode2Test : public DrawNodeExtBaseTest
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

class IndividualThicknessTest : public DrawNodeExtBaseTest
{
public:
    CREATE_FUNC(IndividualThicknessTest);

    IndividualThicknessTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);

    void initSliders();
    void changeThickness(ax::Ref* pSender, ax::ui::Slider::EventType type);
    // void changeLineWidth(Ref* pSender, ax::ui::Slider::EventType type);

private:
    ax::DrawNodeExt* drawNode;
    // ax::Label* _lineWidthLabel;
    // float lineWidth = 0;
    ax::Label* _thicknessLabel;
    float thickness = 1.0f;
};

class DrawNode2PolygonTest : public DrawNodeExtBaseTest
{
public:
    CREATE_FUNC(DrawNode2PolygonTest);

    DrawNode2PolygonTest();
    void drawDirection(const ax::Vec2* vec, const int size, ax::Vec2 offset);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class DrawNode2FilledPolygonTest : public DrawNodeExtBaseTest
{
public:
    CREATE_FUNC(DrawNode2FilledPolygonTest);

    DrawNode2FilledPolygonTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class DrawPieTest : public DrawNodeExtBaseTest
{
public:
    CREATE_FUNC(DrawPieTest);

    DrawPieTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);

    void initSliders();
    void changeStartAngle(ax::Ref* pSender, ax::ui::Slider::EventType type);
    void changeEndAngle(ax::Ref* pSender, ax::ui::Slider::EventType type);
    void changeRotation(ax::Ref* pSender, ax::ui::Slider::EventType type);
    void changeThickness(ax::Ref* pSender, ax::ui::Slider::EventType type);

private:
    ax::DrawNodeExt* drawNode;
    ax::Label* _StartAngleLabel;
    float startAngle = 0;
    ax::Label* _EndAngleLabel;
    float endAngle = 0;
    ax::Label* _RotationLabel;
    float rotation = 0;
    ax::Label* _ThicknessLabel;
    float thickness = 0;
};

class DrawNodePart1Test : public DrawNodeExtBaseTest
{
public:
    CREATE_FUNC(DrawNodePart1Test);

    DrawNodePart1Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);

    void sliderCallback(ax::Ref* sender, ax::ui::Slider::EventType type);
    void listviewCallback(ax::Ref* sender, ax::ui::ListView::EventType type);

    void drawAll();

private:
    ax::ui::Slider* createSlider();
    ax::ui::ListView* createListView();
    ax::DrawNodeExt* draw;
    ax::DrawNode* draw1;
    ax::Vec2* verticess;

    int _currentSeletedItemIndex = 0;
    float thickness              = 0.1f;
    bool isDirty                 = false;

    ax::Label* label;
};

class DrawNodePart2Test : public DrawNodeExtBaseTest
{
public:
    CREATE_FUNC(DrawNodePart2Test);

    DrawNodePart2Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt);

    void sliderCallback(ax::Ref* sender, ax::ui::Slider::EventType type);
    void listviewCallback(ax::Ref* sender, ax::ui::ListView::EventType type);

    void drawAll();

private:
    ax::ui::Slider* createSlider();
    ax::ui::ListView* createListView();
    ax::DrawNodeExt* draw;

    int _currentSeletedItemIndex = 0;
    int count                    = 1;
    bool isDirty                 = false;

    ax::Label* label;
};
