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

#ifndef _RENDERTEXTURE_TEST_H_
#define _RENDERTEXTURE_TEST_H_

#include "cocos2d.h"
#include "renderer/backend/DepthStencilState.h"
#include "../BaseTest.h"

DEFINE_TEST_SUITE(RenderTextureTests);

class RenderTextureTest : public TestCase
{};

class RenderTextureSave : public RenderTextureTest
{
public:
    CREATE_FUNC(RenderTextureSave);
    RenderTextureSave();
    ~RenderTextureSave();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void onTouchesMoved(const std::vector<axis::Touch*>& touches, axis::Event* event);
    void clearImage(axis::Ref* pSender);
    void clearImageTransparent(axis::Ref* sender);
    void saveImageWithPremultipliedAlpha(axis::Ref* pSender);
    void saveImageWithNonPremultipliedAlpha(axis::Ref* pSender);
    void addImage(axis::Ref* sender);

private:
    axis::RenderTexture* _target;
    axis::Vector<axis::Sprite*> _brushs;
};

class RenderTextureIssue937 : public RenderTextureTest
{
public:
    CREATE_FUNC(RenderTextureIssue937);
    RenderTextureIssue937();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class RenderTextureZbuffer : public RenderTextureTest
{
public:
    CREATE_FUNC(RenderTextureZbuffer);
    RenderTextureZbuffer();

    void onTouchesMoved(const std::vector<axis::Touch*>& touches, axis::Event* event);
    void onTouchesBegan(const std::vector<axis::Touch*>& touches, axis::Event* event);
    void onTouchesEnded(const std::vector<axis::Touch*>& touches, axis::Event* event);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void renderScreenShot();

private:
    axis::SpriteBatchNode* mgr;

    axis::Sprite* sp1;
    axis::Sprite* sp2;
    axis::Sprite* sp3;
    axis::Sprite* sp4;
    axis::Sprite* sp5;
    axis::Sprite* sp6;
    axis::Sprite* sp7;
    axis::Sprite* sp8;
    axis::Sprite* sp9;
};

class RenderTextureTestDepthStencil : public RenderTextureTest
{
public:
    CREATE_FUNC(RenderTextureTestDepthStencil);
    RenderTextureTestDepthStencil();
    virtual ~RenderTextureTestDepthStencil();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void draw(axis::Renderer* renderer, const axis::Mat4& transform, uint32_t flags) override;

private:
//    axis::CallbackCommand _renderCmds[4];
    void onBeforeClear();
    void onBeforeStencil();
    void onBeforeDraw();
    void onAfterDraw();

private:
    axis::Renderer* _renderer;
    axis::backend::DepthStencilDescriptor _dsDesc;
    axis::RenderTexture* _rtx;
    axis::Sprite* _spriteDS;
    axis::Sprite* _spriteDraw;
};

class RenderTextureTargetNode : public RenderTextureTest
{
private:
    axis::Sprite *sprite1, *sprite2;
    axis::RenderTexture* renderTexture;

public:
    CREATE_FUNC(RenderTextureTargetNode);
    RenderTextureTargetNode();

    virtual void update(float t) override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void touched(axis::Ref* sender);
};

class RenderTexturePartTest : public RenderTextureTest
{
public:
    CREATE_FUNC(RenderTexturePartTest);
    RenderTexturePartTest();
    virtual ~RenderTexturePartTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

private:
    axis::RenderTexture* _rend;
    axis::Sprite* _spriteDraw;
};

class SpriteRenderTextureBug : public RenderTextureTest
{
public:
    class SimpleSprite : public axis::Sprite
    {
    public:
        static SimpleSprite* create(const char* filename, const axis::Rect& rect);
        SimpleSprite();
        ~SimpleSprite();
        virtual void draw(axis::Renderer* renderer, const axis::Mat4& transform, uint32_t flags);

    public:
        axis::RenderTexture* _rt;
    };

public:
    CREATE_FUNC(SpriteRenderTextureBug);
    SpriteRenderTextureBug();

    void onTouchesEnded(const std::vector<axis::Touch*>& touches, axis::Event* event);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    SimpleSprite* addNewSpriteWithCoords(const axis::Vec2& p);
};

class Issue16113Test : public RenderTextureTest
{
public:
    CREATE_FUNC(Issue16113Test);
    Issue16113Test();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

#endif
