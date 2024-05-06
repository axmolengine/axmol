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

#ifndef _RENDERTEXTURE_TEST_H_
#define _RENDERTEXTURE_TEST_H_

#include "axmol.h"
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
    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void clearImage(ax::Object* pSender);
    void clearImageTransparent(ax::Object* sender);
    void saveImageWithPremultipliedAlpha(ax::Object* pSender);
    void saveImageWithNonPremultipliedAlpha(ax::Object* pSender);
    void addImage(ax::Object* sender);

private:
    ax::RenderTexture* _target;
    ax::Vector<ax::Sprite*> _brushs;
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

    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void renderScreenShot();

private:
    ax::SpriteBatchNode* mgr;

    ax::Sprite* sp1;
    ax::Sprite* sp2;
    ax::Sprite* sp3;
    ax::Sprite* sp4;
    ax::Sprite* sp5;
    ax::Sprite* sp6;
    ax::Sprite* sp7;
    ax::Sprite* sp8;
    ax::Sprite* sp9;
};

class RenderTextureTestDepthStencil : public RenderTextureTest
{
public:
    CREATE_FUNC(RenderTextureTestDepthStencil);
    RenderTextureTestDepthStencil();
    virtual ~RenderTextureTestDepthStencil();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

private:
//    ax::CallbackCommand _renderCmds[4];
    void onBeforeClear();
    void onBeforeStencil();
    void onBeforeDraw();
    void onAfterDraw();

private:
    ax::Renderer* _renderer;
    ax::backend::DepthStencilDescriptor _dsDesc;
    ax::RenderTexture* _rtx;
    ax::Sprite* _spriteDS;
    ax::Sprite* _spriteDraw;
};

class RenderTextureTargetNode : public RenderTextureTest
{
private:
    ax::Sprite *sprite1, *sprite2;
    ax::RenderTexture* renderTexture;

public:
    CREATE_FUNC(RenderTextureTargetNode);
    RenderTextureTargetNode();

    virtual void update(float t) override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void touched(ax::Object* sender);
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
    ax::RenderTexture* _rend;
    ax::Sprite* _spriteDraw;
};

class SpriteRenderTextureBug : public RenderTextureTest
{
public:
    class SimpleSprite : public ax::Sprite
    {
    public:
        static SimpleSprite* create(const char* filename, const ax::Rect& rect);
        SimpleSprite();
        ~SimpleSprite();
        virtual void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags);

    public:
        ax::RenderTexture* _rt;
    };

public:
    CREATE_FUNC(SpriteRenderTextureBug);
    SpriteRenderTextureBug();

    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    SimpleSprite* addNewSpriteWithCoords(const ax::Vec2& p);
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
