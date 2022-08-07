/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axys1.github.io/

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
#include "renderer/CCCustomCommand.h"
#include <list>

NS_AX_BEGIN
namespace backend
{
class ProgramState;
}
NS_AX_END  // namespace axis

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

    virtual axis::Action* actionRotate();
    virtual axis::Action* actionScale();

    virtual axis::DrawNode* shape();
    virtual axis::Sprite* grossini();

    virtual axis::Node* stencil();
    virtual axis::ClippingNode* clipper();
    virtual axis::Node* content();
};

class ShapeTest : public BasicTest
{
public:
    CREATE_FUNC(ShapeTest);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    virtual axis::Node* stencil() override;
    virtual axis::Node* content() override;
};

class ShapeInvertedTest : public ShapeTest
{
public:
    CREATE_FUNC(ShapeInvertedTest);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual axis::ClippingNode* clipper() override;
};

class SpriteTest : public BasicTest
{
public:
    CREATE_FUNC(SpriteTest);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    virtual axis::Node* stencil() override;
    virtual axis::ClippingNode* clipper() override;
    virtual axis::Node* content() override;
};

class SpriteNoAlphaTest : public SpriteTest
{
public:
    CREATE_FUNC(SpriteNoAlphaTest);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual axis::ClippingNode* clipper() override;
};

class SpriteInvertedTest : public SpriteTest
{
public:
    CREATE_FUNC(SpriteInvertedTest);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual axis::ClippingNode* clipper() override;
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
    void pokeHoleAtPoint(axis::Vec2 point);
    void onTouchesBegan(const std::vector<axis::Touch*>& touches, axis::Event* event);

private:
    axis::ClippingNode* _outerClipper;
    axis::Node* _holes;
    axis::Node* _holesStencil;
};

class ScrollViewDemo : public BaseClippingNodeTest
{
public:
    CREATE_FUNC(ScrollViewDemo);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void setup() override;
    void onTouchesBegan(const std::vector<axis::Touch*>& touches, axis::Event* event);
    void onTouchesMoved(const std::vector<axis::Touch*>& touches, axis::Event* event);
    void onTouchesEnded(const std::vector<axis::Touch*>& touches, axis::Event* event);

private:
    bool _scrolling;
    axis::Vec2 _lastPoint;
};

//#if AXIS_DEBUG > 1

class RawStencilBufferTest : public BaseClippingNodeTest
{
public:
    CREATE_FUNC(RawStencilBufferTest);

    ~RawStencilBufferTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void setup() override;
    virtual void draw(axis::Renderer* renderer, const axis::Mat4& transform, uint32_t flags) override;

    virtual void setupStencilForClippingOnPlane(int plane);
    virtual void setupStencilForDrawingOnPlane(int plane);

protected:
    void onBeforeDrawClip(int planeIndex);
    void onBeforeDrawSprite(int planeIndex);
    void initCommands();

    std::vector<axis::CustomCommand> _renderCmds;
//    axis::CallbackCommand _enableStencilCallback;
//    axis::CallbackCommand _disableStencilCallback;
    axis::Vector<axis::Sprite*> _sprites;
    axis::Vector<axis::Sprite*> _spritesStencil;
    axis::backend::UniformLocation _locColor;
    axis::backend::UniformLocation _locMVPMatrix;
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
    axis::backend::UniformLocation _alphaMVPMatrix;
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

//#endif //AXIS_DEBUG > 1

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
