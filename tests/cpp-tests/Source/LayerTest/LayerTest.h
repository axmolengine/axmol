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

#ifndef _LAYER_TEST_H_
#define _LAYER_TEST_H_

#include "../BaseTest.h"
#include "core/ui/UISlider.h"
#include "../core/ui/UIListView.h"

DEFINE_TEST_SUITE(LayerTests);

class LayerTest : public TestCase
{
public:
    virtual std::string title() const override;

protected:
    std::string _title;
};

class LayerTestCascadingOpacityA : public LayerTest
{
public:
    CREATE_FUNC(LayerTestCascadingOpacityA);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class LayerTestCascadingOpacityB : public LayerTest
{
public:
    CREATE_FUNC(LayerTestCascadingOpacityB);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class LayerTestCascadingOpacityC : public LayerTest
{
public:
    CREATE_FUNC(LayerTestCascadingOpacityC);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class LayerTestCascadingColorA : public LayerTest
{
public:
    CREATE_FUNC(LayerTestCascadingColorA);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class LayerTestCascadingColorB : public LayerTest
{
public:
    CREATE_FUNC(LayerTestCascadingColorB);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class LayerTestCascadingColorC : public LayerTest
{
public:
    CREATE_FUNC(LayerTestCascadingColorC);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class LayerTest1 : public LayerTest
{
public:
    CREATE_FUNC(LayerTest1);

    virtual void onEnter() override;
    virtual std::string subtitle() const override;

    void updateSize(ax::Vec2& touchLocation);

    void onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);
};

class LayerTest2 : public LayerTest
{
public:
    CREATE_FUNC(LayerTest2);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class LayerTestBlend : public LayerTest
{
public:
    CREATE_FUNC(LayerTestBlend);

    LayerTestBlend();
    void newBlend(float dt);
    virtual std::string subtitle() const override;
};

class LayerGradientTest : public LayerTest
{
public:
    CREATE_FUNC(LayerGradientTest);
    LayerGradientTest();
    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void toggleItem(ax::Object* sender);
};

class LayerGradientTest2 : public LayerTest
{
public:
    CREATE_FUNC(LayerGradientTest2);
    LayerGradientTest2();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LayerIgnoreAnchorPointPos : public LayerTest
{
public:
    CREATE_FUNC(LayerIgnoreAnchorPointPos);
    virtual void onEnter() override;
    void onToggle(ax::Object* pObject);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LayerIgnoreAnchorPointRot : public LayerTest
{
public:
    CREATE_FUNC(LayerIgnoreAnchorPointRot);
    virtual void onEnter() override;
    void onToggle(ax::Object* pObject);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LayerIgnoreAnchorPointScale : public LayerTest
{
public:
    CREATE_FUNC(LayerIgnoreAnchorPointScale);
    virtual void onEnter() override;
    void onToggle(ax::Object* pObject);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LayerExtendedBlendOpacityTest : public LayerTest
{
public:
    CREATE_FUNC(LayerExtendedBlendOpacityTest);
    LayerExtendedBlendOpacityTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LayerBug3162A : public LayerTest
{
public:
    CREATE_FUNC(LayerBug3162A);
    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void step(float dt);

private:
    ax::LayerColor* _layer[3];
};

class LayerBug3162B : public LayerTest
{
public:
    CREATE_FUNC(LayerBug3162B);
    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void step(float dt);

private:
    ax::LayerColor* _layer[3];
};

class LayerColorOccludeBug : public LayerTest
{
public:
    CREATE_FUNC(LayerColorOccludeBug);
    virtual void onEnter() override;
    virtual void onExit() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

private:
    ax::LayerColor* _layer;
};

class LayerRadialGradientTest : public LayerTest
{
public:
    CREATE_FUNC(LayerRadialGradientTest);
    virtual void onEnter() override;
    virtual std::string title() const override;

    void sliderCallback(ax::Object* sender, ax::ui::Slider::EventType type);
    void listviewCallback(ax::Object* sender, ax::ui::ListView::EventType type);

private:
    ax::ui::Slider* createSlider();
    ax::ui::ListView* createListView();

    ax::LayerRadialGradient* _layer;
    int _currentSeletedItemIndex;
};

#endif
