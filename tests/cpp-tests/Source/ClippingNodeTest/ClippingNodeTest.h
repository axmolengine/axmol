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

#include "../BaseTest.h"
#include "renderer/CustomCommand.h"
#include <list>

NS_AX_BEGIN
namespace backend
{
class ProgramState;
}
NS_AX_END  // namespace ax

DEFINE_TEST_SUITE(ClippingNodeTests);

class BaseClippingNodeTest : public TestCase
{
public:
    CREATE_FUNC(BaseClippingNodeTest);

    ~BaseClippingNodeTest();
    virtual bool init() override;

    virtual std::string title() const override;
    virtual void setup();
};

class BasicTest : public BaseClippingNodeTest
{
public:
    CREATE_FUNC(BasicTest);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void setup() override;

    virtual ax::Action* actionRotate();
    virtual ax::Action* actionScale();

    virtual ax::DrawNode* shape();
    virtual ax::Sprite* grossini();

    virtual ax::Node* stencil();
    virtual ax::ClippingNode* clipper();
    virtual ax::Node* content();
};

class ShapeTest : public BasicTest
{
public:
    CREATE_FUNC(ShapeTest);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    virtual ax::Node* stencil() override;
    virtual ax::Node* content() override;
};

class ShapeInvertedTest : public ShapeTest
{
public:
    CREATE_FUNC(ShapeInvertedTest);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual ax::ClippingNode* clipper() override;
};

class SpriteTest : public BasicTest
{
public:
    CREATE_FUNC(SpriteTest);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    virtual ax::Node* stencil() override;
    virtual ax::ClippingNode* clipper() override;
    virtual ax::Node* content() override;
};

class SpriteNoAlphaTest : public SpriteTest
{
public:
    CREATE_FUNC(SpriteNoAlphaTest);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual ax::ClippingNode* clipper() override;
};

class SpriteInvertedTest : public SpriteTest
{
public:
    CREATE_FUNC(SpriteInvertedTest);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual ax::ClippingNode* clipper() override;
};

class NestedTest : public BaseClippingNodeTest
{
public:
    CREATE_FUNC(NestedTest);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void setup() override;
};

class HoleDemo : public BaseClippingNodeTest
{
public:
    CREATE_FUNC(HoleDemo);

    ~HoleDemo();
    virtual void setup() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void pokeHoleAtPoint(ax::Vec2 point);
    void onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event);

private:
    ax::ClippingNode* _outerClipper;
    ax::Node* _holes;
    ax::Node* _holesStencil;
};


class ScrollViewDemo : public BaseClippingNodeTest
{
public:
    CREATE_FUNC(ScrollViewDemo);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void setup() override;
    void onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);

private:
    bool _scrolling;
    ax::Vec2 _lastPoint;
};

//#if _AX_DEBUG > 1

class RawStencilBufferTest : public BaseClippingNodeTest
{
public:
    CREATE_FUNC(RawStencilBufferTest);

    ~RawStencilBufferTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void setup() override;
    virtual void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

    virtual void setupStencilForClippingOnPlane(int plane);
    virtual void setupStencilForDrawingOnPlane(int plane);

protected:
    void onBeforeDrawClip(int planeIndex);
    void onBeforeDrawSprite(int planeIndex);
    void initCommands();

    std::vector<ax::CustomCommand> _renderCmds;
//    ax::CallbackCommand _enableStencilCallback;
//    ax::CallbackCommand _disableStencilCallback;
    ax::Vector<ax::Sprite*> _sprites;
    ax::Vector<ax::Sprite*> _spritesStencil;
    ax::backend::UniformLocation _locColor;
    ax::backend::UniformLocation _locMVPMatrix;
};

class RawStencilBufferTest2 : public RawStencilBufferTest
{
public:
    CREATE_FUNC(RawStencilBufferTest2);

    virtual std::string subtitle() const override;
    virtual void setupStencilForClippingOnPlane(int plane) override;
    virtual void setupStencilForDrawingOnPlane(int plane) override;
};

class RawStencilBufferTest3 : public RawStencilBufferTest
{
public:
    CREATE_FUNC(RawStencilBufferTest3);

    virtual std::string subtitle() const override;
    virtual void setupStencilForClippingOnPlane(int plane) override;
    virtual void setupStencilForDrawingOnPlane(int plane) override;
};

class RawStencilBufferTestAlphaTest : public RawStencilBufferTest
{
public:
    virtual void setup() override;
};

class RawStencilBufferTest4 : public RawStencilBufferTestAlphaTest
{
public:
    CREATE_FUNC(RawStencilBufferTest4);

    virtual std::string subtitle() const override;
    virtual void setupStencilForClippingOnPlane(int plane) override;
    virtual void setupStencilForDrawingOnPlane(int plane) override;

private:
    ax::backend::UniformLocation _alphaMVPMatrix;
};

class RawStencilBufferTest5 : public RawStencilBufferTestAlphaTest
{
public:
    CREATE_FUNC(RawStencilBufferTest5);

    virtual std::string subtitle() const override;
    virtual void setupStencilForClippingOnPlane(int plane) override;
    virtual void setupStencilForDrawingOnPlane(int plane) override;
};

class RawStencilBufferTest6 : public RawStencilBufferTestAlphaTest
{
public:
    CREATE_FUNC(RawStencilBufferTest6);

    virtual void setupStencilForClippingOnPlane(int plane) override;
    virtual void setupStencilForDrawingOnPlane(int plane) override;

    // override
    virtual void setup() override;
    virtual std::string subtitle() const override;
};

//#endif //_AX_DEBUG > 1

class ClippingToRenderTextureTest : public BaseClippingNodeTest
{
public:
    CREATE_FUNC(ClippingToRenderTextureTest);

    void expectedBehaviour();
    void reproduceBug();

    // override
    virtual void setup() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class ClippingRectangleNodeTest : public BaseClippingNodeTest
{
public:
    CREATE_FUNC(ClippingRectangleNodeTest);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void setup() override;
};

class ClippingNodePerformanceTest : public BaseClippingNodeTest
{
private:
    int _totalCount = 0;
public:
    CREATE_FUNC(ClippingNodePerformanceTest);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void setup() override;
};

class UniqueChildStencilTest : public BaseClippingNodeTest
{
public:
    CREATE_FUNC(UniqueChildStencilTest);

    ~UniqueChildStencilTest();
    virtual void setup() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void addChildStencils();

private:
    ax::ClippingNode* _outerClipper;
    ax::Node* _parentStencil;
};
