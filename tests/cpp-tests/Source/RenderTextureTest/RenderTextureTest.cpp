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

#include "RenderTextureTest.h"

using namespace ax;
using namespace ax::ui;

RenderTextureTests::RenderTextureTests()
{
    ADD_TEST_CASE(RenderTextureSave);
    ADD_TEST_CASE(RenderTextureIssue937);
    ADD_TEST_CASE(RenderTextureZbuffer);
    ADD_TEST_CASE(RenderTextureTestDepthStencil);
    ADD_TEST_CASE(RenderTextureTargetNode);
    ADD_TEST_CASE(SpriteRenderTextureBug);
    ADD_TEST_CASE(RenderTexturePartTest);
    ADD_TEST_CASE(Issue16113Test);
    ADD_TEST_CASE(CameraTargetTextureTest);
};

/**
 * Implementation of RenderTextureSave
 */
RenderTextureSave::RenderTextureSave()
{
    auto s = _director->getCanvasSize();

    // create a render texture, this is what we are going to draw into
    _target = RenderTexture::create(_director->canvasToPixels(s), rhi::PixelFormat::RGBA8);
    _target->retain();
    _rtxPass = RefPtr<RenderTexturePass>(RenderTexturePass::obtain(_target), tlx::adopt_object);

    // create a sprite from the render texture to display it in the scene
    auto targetSprite = Sprite::createWithTexture(_target);
    targetSprite->setPosition(Vec2(s.width / 2, s.height / 2));
    targetSprite->setAnchorPoint(Anchors::center);
    this->addChild(targetSprite, -1);

    auto listener           = PointerEventListener::create();
    listener->onPointerMove = AX_CALLBACK_1(RenderTextureSave::onPointerMove, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    // Save Image menu
    MenuItemFont::setFontSize(16);
    auto item1 =
        MenuItemFont::create("Save Image PMA", AX_CALLBACK_1(RenderTextureSave::saveImageWithPremultipliedAlpha, this));
    auto item2 = MenuItemFont::create("Save Image Non-PMA",
                                      AX_CALLBACK_1(RenderTextureSave::saveImageWithNonPremultipliedAlpha, this));
    auto item3 = MenuItemFont::create("Add Image", AX_CALLBACK_1(RenderTextureSave::addImage, this));
    auto item4 = MenuItemFont::create("Clear to Random", AX_CALLBACK_1(RenderTextureSave::clearImage, this));
    auto item5 =
        MenuItemFont::create("Clear to Transparent", AX_CALLBACK_1(RenderTextureSave::clearImageTransparent, this));
    auto menu = Menu::create(item1, item2, item3, item4, item5, nullptr);
    this->addChild(menu);
    menu->alignItemsVertically();
    menu->setPosition(Vec2(VisibleRect::rightTop().x - 80, VisibleRect::rightTop().y - 100));
}

std::string RenderTextureSave::title() const
{
    return "Touch the screen";
}

std::string RenderTextureSave::subtitle() const
{
    return "Press 'Save Image' to create an snapshot of the render texture";
}

void RenderTextureSave::clearImage(ax::Object* sender)
{
    {
        _rtxPass->begin();
        _rtxPass->clear(ClearFlag::COLOR,
                        {.color = Color(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1())});
        _rtxPass->end();
    }
}

void RenderTextureSave::clearImageTransparent(ax::Object* sender)
{
    {
        _rtxPass->begin();
        _rtxPass->clear(ClearFlag::COLOR, {.color = Color(0, 0, 0, 0)});
        _rtxPass->end();
    }
}

void RenderTextureSave::saveImageWithPremultipliedAlpha(ax::Object* sender)
{
    static int counter = 0;

    char buf[20];
    auto fileName = fmt::format_to_z(buf, "image-pma-{}.png", counter);

    auto callback = [this](RenderTexture* rt, std::string_view path) {
        auto sprite = Sprite::create(path);
        addChild(sprite);
        sprite->setScale(0.3f);
        sprite->setPosition(Vec2(40.0f, 40.0f));
        sprite->setRotation(counter * 3);
        _target->release();
    };

    _target->retain();
    _target->saveToFile(fileName, Image::Format::PNG, true, callback);
    // Add this function to avoid crash if we switch to a new scene.
    Director::getInstance()->getRenderer()->render();
    AXLOGD("Image saved {}", fileName);

    counter++;
}

void RenderTextureSave::saveImageWithNonPremultipliedAlpha(ax::Object* sender)
{
    static int counter = 0;

    char buf[20];
    auto fileName = fmt::format_to_z(buf, "image-no-pma-{}.png", counter);

    auto callback = [this](RenderTexture* rt, std::string_view path) {
        auto sprite = Sprite::create(path);
        addChild(sprite);
        sprite->setScale(0.3f);
        sprite->setPosition(Vec2(40.0f, 40.0f));
        sprite->setRotation(counter * 3);
        rt->release();
    };

    _target->retain();
    _target->saveToFileAsNonPMA(fileName, Image::Format::PNG, true, callback);

    // Add this function to avoid crash if we switch to a new scene.
    Director::getInstance()->getRenderer()->render();
    AXLOGD("Image saved {}", fileName);

    counter++;
}

void RenderTextureSave::addImage(ax::Object* sender)
{
    auto s = Director::getInstance()->getCanvasSize();

    {
        _rtxPass->begin(getDefaultCamera());

        Sprite* sprite = Sprite::create("Images/test-rgba1.png");
        sprite->setPosition(
            sprite->getContentSize().width + AXRANDOM_0_1() * (s.width - sprite->getContentSize().width),
            sprite->getContentSize().height + AXRANDOM_0_1() * (s.height - sprite->getContentSize().height));
        sprite->visit();

        _rtxPass->end();
    }
}

RenderTextureSave::~RenderTextureSave()
{
    _target->release();
    Director::getInstance()->getTextureCache()->removeUnusedTextures();
}

void RenderTextureSave::onPointerMove(PointerEvent* event)
{
    if (!event->isPrimaryPressed())
        return;

    auto start = event->getLocation();
    auto end   = event->getPreviousLocation();

    {
        _rtxPass->begin(getDefaultCamera());

        // for extra points, we'll draw this smoothly from the last position and vary the sprite's
        // scale/rotation/offset
        float distance = start.getDistance(end);
        if (distance > 1)
        {
            int d = (int)distance;
            _brushs.clear();
            for (int i = 0; i < d; ++i)
            {
                Sprite* sprite = Sprite::create("Images/fire.png");
                sprite->setColor(Color32(255, 0, 0, 20));
                _brushs.pushBack(sprite);
            }
            for (int i = 0; i < d; i++)
            {
                float difx  = end.x - start.x;
                float dify  = end.y - start.y;
                float delta = (float)i / distance;
                auto brush  = _brushs.at(i);
                brush->setPosition(Vec2(start.x + (difx * delta), start.y + (dify * delta)));
                brush->setRotation(rand() % 360);
                float r = (float)(rand() % 50 / 50.f) + 0.25f;
                brush->setScale(r);
                /*_brush->setColor(Color32(AXRANDOM_0_1() * 127 + 128, 255, 255));*/
                // Use AXRANDOM_0_1() will cause error when loading libtests.so on android, I don't know why.
                brush->setColor(Color32(rand() % 127 + 128, 255, 255, brush->getOpacity()));
                // Call visit to draw the brush, don't call draw..
                brush->visit();
            }
        }

        _rtxPass->end();
    }

    return;
}

/**
 * Implementation of RenderTextureIssue937
 */

RenderTextureIssue937::RenderTextureIssue937()
{
    /*
     *     1    2
     * A: A1   A2
     *
     * B: B1   B2
     *
     *  A1: premulti sprite
     *  A2: premulti render
     *
     *  B1: non-premulti sprite
     *  B2: non-premulti render
     */
    auto background = LayerColor::create(Color32(200, 200, 200, 255));
    addChild(background);

    auto s            = Director::getInstance()->getCanvasSize();
    auto spr_premulti = Sprite::create("Images/fire.png");
    spr_premulti->setPosition(Vec2(s.width / 2 - 16, s.height / 2 + 16));

    auto spr_nonpremulti = Sprite::create("Images/fire.png");
    spr_nonpremulti->setPosition(Vec2(s.width / 2 - 16, s.height / 2 - 16));

    /* A2 & B2 setup */
    auto rend = RenderTexture::create(_director->canvasToPixels(Vec2(32, 64)), rhi::PixelFormat::RGBA8);

    if (nullptr == rend)
    {
        return;
    }

    // It's possible to modify the RenderTexture blending function by
    //        [[rend sprite] setBlendFunc:(BlendFunc) {BlendFactor::ONE, BlendFactor::ONE_MINUS_SRC_ALPHA}];
    {
        auto scope           = RefPtr<RenderTexturePass>(RenderTexturePass::obtain(rend), tlx::adopt_object);
        const auto pixelSize = Director::getInstance()->getCanvasSizeInPixels();
        scope->setViewport(scope->makeVirtualViewport(Vec2(s.width / 2 - 32, s.height / 2 - 32),
                                                      Rect(0, 0, s.width, s.height),
                                                      Rect(0, 0, pixelSize.width, pixelSize.height)));
        scope->begin(getDefaultCamera());
        spr_premulti->visit();
        spr_nonpremulti->visit();
        scope->end();
    }

    auto rendSprite = Sprite::createWithTexture(rend);
    rendSprite->setAnchorPoint(Anchors::center);
    rendSprite->setPosition(Vec2(s.width / 2 + 16, s.height / 2));

    addChild(spr_nonpremulti);
    addChild(spr_premulti);
    addChild(rendSprite);
}

std::string RenderTextureIssue937::title() const
{
    return "Testing issue #937";
}

std::string RenderTextureIssue937::subtitle() const
{
    return "All images should be equal...";
}

/**
 * Implementation of RenderTextureZbuffer
 */

RenderTextureZbuffer::RenderTextureZbuffer()
{
    auto listener           = PointerEventListener::create();
    listener->onPointerDown = AX_CALLBACK_1(RenderTextureZbuffer::onPointerDown, this);
    listener->onPointerMove = AX_CALLBACK_1(RenderTextureZbuffer::onPointerMove, this);
    listener->onPointerUp   = AX_CALLBACK_1(RenderTextureZbuffer::onPointerUp, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    auto size  = Director::getInstance()->getCanvasSize();
    auto label = Label::createWithTTF("vertexZ = 50", "fonts/Marker Felt.ttf", 64);
    label->setPosition(Vec2(size.width / 2, size.height * 0.25f));
    this->addChild(label);

    auto label2 = Label::createWithTTF("vertexZ = 0", "fonts/Marker Felt.ttf", 64);
    label2->setPosition(Vec2(size.width / 2, size.height * 0.5f));
    this->addChild(label2);

    auto label3 = Label::createWithTTF("vertexZ = -50", "fonts/Marker Felt.ttf", 64);
    label3->setPosition(Vec2(size.width / 2, size.height * 0.75f));
    this->addChild(label3);

    label->setPositionZ(50);
    label2->setPositionZ(0);
    label3->setPositionZ(-50);

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Images/bugs/circle.plist");
    mgr = SpriteBatchNode::create("Images/bugs/circle.png", 9);
    this->addChild(mgr);
    sp1 = Sprite::createWithSpriteFrameName("circle.png");
    sp2 = Sprite::createWithSpriteFrameName("circle.png");
    sp3 = Sprite::createWithSpriteFrameName("circle.png");
    sp4 = Sprite::createWithSpriteFrameName("circle.png");
    sp5 = Sprite::createWithSpriteFrameName("circle.png");
    sp6 = Sprite::createWithSpriteFrameName("circle.png");
    sp7 = Sprite::createWithSpriteFrameName("circle.png");
    sp8 = Sprite::createWithSpriteFrameName("circle.png");
    sp9 = Sprite::createWithSpriteFrameName("circle.png");

    mgr->addChild(sp1, 9);
    mgr->addChild(sp2, 8);
    mgr->addChild(sp3, 7);
    mgr->addChild(sp4, 6);
    mgr->addChild(sp5, 5);
    mgr->addChild(sp6, 4);
    mgr->addChild(sp7, 3);
    mgr->addChild(sp8, 2);
    mgr->addChild(sp9, 1);

    sp1->setPositionZ(400);
    sp2->setPositionZ(300);
    sp3->setPositionZ(200);
    sp4->setPositionZ(100);
    sp5->setPositionZ(0);
    sp6->setPositionZ(-100);
    sp7->setPositionZ(-200);
    sp8->setPositionZ(-300);
    sp9->setPositionZ(-400);

    sp9->setScale(2);
    sp9->setColor(Color32::yellow);
}

std::string RenderTextureZbuffer::title() const
{
    return "Testing Z Buffer in Render Texture";
}

std::string RenderTextureZbuffer::subtitle() const
{
    return "Touch screen. It should be green";
}

bool RenderTextureZbuffer::onPointerDown(PointerEvent* event)
{
    auto location = event->getLocation();

    sp1->setPosition(location);
    sp2->setPosition(location);
    sp3->setPosition(location);
    sp4->setPosition(location);
    sp5->setPosition(location);
    sp6->setPosition(location);
    sp7->setPosition(location);
    sp8->setPosition(location);
    sp9->setPosition(location);

    return true;
}

void RenderTextureZbuffer::onPointerMove(PointerEvent* event)
{
    if (!event->isCaptured())
        return;

    auto location = event->getLocation();

    sp1->setPosition(location);
    sp2->setPosition(location);
    sp3->setPosition(location);
    sp4->setPosition(location);
    sp5->setPosition(location);
    sp6->setPosition(location);
    sp7->setPosition(location);
    sp8->setPosition(location);
    sp9->setPosition(location);
}

void RenderTextureZbuffer::onPointerUp(PointerEvent* event)
{
    this->renderScreenShot();
}

void RenderTextureZbuffer::renderScreenShot()
{
    auto texture = RenderTexture::create(_director->canvasToPixels(Vec2(512, 512)));
    if (nullptr == texture)
    {
        return;
    }

    {
        auto renderer = _director->getRenderer();
        auto scope    = RefPtr<RenderTexturePass>(RenderTexturePass::obtain(texture), tlx::adopt_object);
        scope->begin(getDefaultCamera());
        this->visit(renderer, getNodeToParentTransform(), 0);
        scope->end();
    }

    _director->getRenderer()->render();

    auto sprite = Sprite::createWithTexture(texture);

    sprite->setPosition(Vec2(256, 256));
    sprite->setOpacity(182);
    sprite->setFlippedY(1);
    this->addChild(sprite, 999999);
    sprite->setColor(Color32::green);

    sprite->runAction(Sequence::create(FadeTo::create(2, 0), RemoveSelf::create(), nullptr));
}

RenderTexturePartTest::RenderTexturePartTest()
{
    auto sprite1     = Sprite::create("Images/grossini.png");
    auto sprite11    = Sprite::create("Images/grossini.png");
    auto sprite2     = Sprite::create("Images/grossinis_sister1.png");
    auto sprite22    = Sprite::create("Images/grossinis_sister1.png");
    Size size        = Director::getInstance()->getCanvasSize();
    Size sprite1Size = sprite1->getContentSize();
    sprite1->setPosition((size.width - sprite1Size.width) / 2 - 20, (size.height - sprite1Size.height) / 2 - 20);
    sprite11->setPosition(size.width / 2 + 20, (size.height - sprite1Size.height) / 2 - 20);

    sprite2->setPosition((size.width - sprite1Size.width) / 2 - 20, size.height / 2 + 20);
    sprite22->setPosition(size.width / 2 + 20, size.height / 2 + 20);

    addChild(sprite1);
    addChild(sprite11);
    addChild(sprite2);
    addChild(sprite22);

    _rend = RenderTexture::create(_director->canvasToPixels(Vec2(200, 200)), rhi::PixelFormat::RGBA8);
    _rend->retain();

    {
        auto scope           = RefPtr<RenderTexturePass>(RenderTexturePass::obtain(_rend), tlx::adopt_object);
        const auto pixelSize = Director::getInstance()->getCanvasSizeInPixels();
        scope->setViewport(scope->makeVirtualViewport(Vec2(size.width / 2 - 150, size.height / 2 - 150),
                                                      Rect(0, 0, size.width, size.height),
                                                      Rect(0, 0, pixelSize.width, pixelSize.height)));
        scope->begin(getDefaultCamera());
        scope->clear(ClearFlag::COLOR, {.color = Color(1, 0, 0, 1)});
        sprite1->visit();
        sprite11->visit();
        sprite2->visit();
        sprite22->visit();
        scope->end();
    }

    _spriteDraw                  = Sprite::createWithTexture(_rend);
    FiniteTimeAction* baseAction = MoveBy::create(1, Vec2(size.width, 0.0f));
    _spriteDraw->setPosition(0, size.height / 2);
    _spriteDraw->setScaleY(-1);
    _spriteDraw->runAction(RepeatForever::create(Sequence::create(baseAction, baseAction->reverse(), nullptr)));
    addChild(_spriteDraw);
}

RenderTexturePartTest::~RenderTexturePartTest()
{
    AX_SAFE_RELEASE(_rend);
}

std::string RenderTexturePartTest::title() const
{
    return "Render Texture Part Test";
}

std::string RenderTexturePartTest::subtitle() const
{
    return "Only Grabbing a sub region of fullscreen";
}

// RenderTextureTestDepthStencil

RenderTextureTestDepthStencil::RenderTextureTestDepthStencil()
{
    auto s = _director->getCanvasSize();

    _renderer = _director->getRenderer();

    /// store old depth stencil desc
    _dsDesc = _renderer->getDepthStencilDesc();

    _spriteDS = Sprite::create("Images/fire.png");
    _spriteDS->retain();
    _spriteDS->setPosition(Vec2(s.width * 0.25f, 0.0f));
    _spriteDS->setScale(10);

    _spriteDraw = Sprite::create("Images/fire.png");
    _spriteDraw->retain();
    _spriteDraw->setPosition(Vec2(s.width * 0.25f, 0.0f));
    _spriteDraw->setScale(10);
    //! move sprite half width and height, and draw only where not marked
    _spriteDraw->setPosition(_spriteDraw->getPosition() +
                             Vec2(_spriteDraw->getContentSize().width * _spriteDraw->getScale() * 0.5f,
                                  _spriteDraw->getContentSize().height * _spriteDraw->getScale() * 0.5f));

    _rtx     = RenderTexture::create(_director->canvasToPixels(s), rhi::PixelFormat::RGBA4, PixelFormat::D24S8);
    _rtxPass = RefPtr<RenderTexturePass>(RenderTexturePass::obtain(_rtx), tlx::adopt_object);

    auto rtxSprite = Sprite::createWithTexture(_rtx);
    rtxSprite->setPosition(Vec2(s.width * 0.5f, s.height * 0.5f));
    rtxSprite->setAnchorPoint(Anchors::center);

    this->addChild(rtxSprite);
}

RenderTextureTestDepthStencil::~RenderTextureTestDepthStencil()
{
    AX_SAFE_RELEASE(_spriteDraw);
    AX_SAFE_RELEASE(_spriteDS);

    // restore depth stencil desc
    _renderer->setDepthStencilDesc(_dsDesc);
    _renderer->setStencilTest(bitmask::any(_dsDesc.flags, DepthStencilFlags::STENCIL_TEST));
}

void RenderTextureTestDepthStencil::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    {
        _rtxPass->begin();
        _rtxPass->clear(ClearFlag::COLOR | ClearFlag::DEPTH | ClearFlag::STENCIL,
                        {.color = Color(0, 0, 0, 0), .depth = 0, .stencil = 0});

        //    _renderCmds[0].init(_globalZOrder);
        //    _renderCmds[0].func = AX_CALLBACK_0(RenderTextureTestDepthStencil::onBeforeClear, this);
        renderer->addCallbackCommand(AX_CALLBACK_0(RenderTextureTestDepthStencil::onBeforeClear, this), _globalZOrder);

        //    _renderCmds[1].init(_globalZOrder);
        //    _renderCmds[1].func = AX_CALLBACK_0(RenderTextureTestDepthStencil::onBeforeStencil, this);
        renderer->addCallbackCommand(AX_CALLBACK_0(RenderTextureTestDepthStencil::onBeforeStencil, this),
                                     _globalZOrder);

        _spriteDS->visit();

        //    _renderCmds[2].init(_globalZOrder);
        //    _renderCmds[2].func = AX_CALLBACK_0(RenderTextureTestDepthStencil::onBeforeDraw, this);
        renderer->addCallbackCommand(AX_CALLBACK_0(RenderTextureTestDepthStencil::onBeforeDraw, this), _globalZOrder);

        _spriteDraw->visit();

        //    _renderCmds[3].init(_globalZOrder);
        //    _renderCmds[3].func = AX_CALLBACK_0(RenderTextureTestDepthStencil::onAfterDraw, this);
        renderer->addCallbackCommand(AX_CALLBACK_0(RenderTextureTestDepthStencil::onAfterDraw, this), _globalZOrder);

        /// !!!end will set current render target to default renderTarget
        /// !!!all render target share one depthStencilDesc, TODO: optimize me?
        _rtxPass->end();
    }
}

void RenderTextureTestDepthStencil::onBeforeClear()
{
    _renderer->setStencilWriteMask(0xFF);
}

void RenderTextureTestDepthStencil::onBeforeStencil()
{
    //! mark sprite quad into stencil buffer
    _renderer->setStencilTest(true);
    _renderer->setStencilCompareFunc(rhi::CompareFunc::NEVER, 1, 0xFF);
    _renderer->setStencilOp(rhi::StencilOp::REPLACE, rhi::StencilOp::REPLACE, rhi::StencilOp::REPLACE);
}

void RenderTextureTestDepthStencil::onBeforeDraw()
{
    _renderer->setStencilCompareFunc(rhi::CompareFunc::NOT_EQUAL, 1, 0xFF);
}

void RenderTextureTestDepthStencil::onAfterDraw()
{
    _renderer->setStencilTest(false);
}

std::string RenderTextureTestDepthStencil::title() const
{
    return "Testing depthStencil attachment";
}

std::string RenderTextureTestDepthStencil::subtitle() const
{
    return "Circle should be missing 1/4 of its region";
}

// RenderTextureTest
RenderTextureTargetNode::RenderTextureTargetNode()
{
    /*
     *     1    2
     * A: A1   A2
     *
     * B: B1   B2
     *
     *  A1: premulti sprite
     *  A2: premulti render
     *
     *  B1: non-premulti sprite
     *  B2: non-premulti render
     */
    auto background = LayerColor::create(Color32(40, 40, 40, 255));
    addChild(background);

    // sprite 1
    _sprite1 = Sprite::create("Images/fire.png");

    // sprite 2
    _sprite2 = Sprite::create("Images/fire_rgba8888.pvr");

    auto s = Director::getInstance()->getCanvasSize();

    /* Create the render texture */
    _renderTexture = RenderTexture::create(_director->canvasToPixels(s), rhi::PixelFormat::RGBA4);
    _rtxPass       = RefPtr<RenderTexturePass>(RenderTexturePass::obtain(_renderTexture), tlx::adopt_object);

    /* store sprites in a container that we'll render into the RT each frame */
    _container = Node::create();
    _container->setContentSize(_renderTexture->getContentSize());
    _spriteCenterPosition = _container->getContentSize() / 2;

    _sprite1->setAnchorPoint(Anchors::center);
    _sprite1->setPosition(_spriteCenterPosition);
    _container->addChild(_sprite1);

    _sprite2->setAnchorPoint(Anchors::center);
    _sprite2->setPosition(_spriteCenterPosition);
    _container->addChild(_sprite2);

    /* create a sprite from the RT to display it */
    auto rtSprite = Sprite::createWithTexture(_renderTexture);
    rtSprite->setPosition(Vec2(s.width / 2, s.height / 2));
    rtSprite->setAnchorPoint(Anchors::center);
    addChild(rtSprite);

    scheduleUpdate();

    // Toggle clear on / off
    auto item = MenuItemFont::create("Clear On/Off", AX_CALLBACK_1(RenderTextureTargetNode::touched, this));
    auto menu = Menu::create(item, nullptr);
    addChild(menu);

    menu->setPosition(Vec2(s.width / 2, s.height / 2));
}

void RenderTextureTargetNode::touched(Object* sender)
{
    _shouldClear = !_shouldClear;
}

void RenderTextureTargetNode::update(float dt)
{
    static float time = 0;
    float r           = 80;
    _sprite1->setPosition(_spriteCenterPosition + Vec2(cosf(time * 2) * r, sinf(time * 2) * r));
    _sprite2->setPosition(_spriteCenterPosition + Vec2(sinf(time * 2) * r, cosf(time * 2) * r));

    time += dt;
}

void RenderTextureTargetNode::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    {
        _rtxPass->begin();
        if (_shouldClear)
            _rtxPass->clear(ClearFlag::COLOR, {.color = Color(0, 0, 0, 0)});
        _container->visit();
        _rtxPass->end();
    }

    RenderTextureTest::draw(renderer, transform, flags);
}

std::string RenderTextureTargetNode::title() const
{
    return "Testing Render Target Node";
}

std::string RenderTextureTargetNode::subtitle() const
{
    return "Sprites should be equal and move with each frame";
}

// SpriteRenderTextureBug

SpriteRenderTextureBug::SimpleSprite::SimpleSprite() : _rt(nullptr) {}
SpriteRenderTextureBug::SimpleSprite::~SimpleSprite()
{
    AX_SAFE_RELEASE(_rt);
}

SpriteRenderTextureBug::SimpleSprite* SpriteRenderTextureBug::SimpleSprite::create(const char* filename,
                                                                                   const Rect& rect)
{
    auto sprite = new SimpleSprite();
    if (sprite->initWithFile(filename, rect))
    {
        sprite->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(sprite);
    }

    return sprite;
}

void SpriteRenderTextureBug::SimpleSprite::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    if (_rt == nullptr)
    {
        auto s = _director->getCanvasSizeInPixels();
        _rt    = RenderTexture::create(s, rhi::PixelFormat::RGBA8);
        _rt->retain();
        _rtxPass = RefPtr<RenderTexturePass>(RenderTexturePass::obtain(_rt), tlx::adopt_object);
    }
    {
        _rtxPass->begin();
        _rtxPass->clear(ClearFlag::COLOR, {.color = Color(0, 0, 0, 1)});
        _rtxPass->end();
    }

    Sprite::draw(renderer, transform, flags);
}

SpriteRenderTextureBug::SpriteRenderTextureBug()
{
    auto listener         = PointerEventListener::create();
    listener->onPointerUp = AX_CALLBACK_1(SpriteRenderTextureBug::onPointerUp, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    auto s = Director::getInstance()->getCanvasSize();
    addNewSpriteWithCoords(Vec2(s.width / 2, s.height / 2));
}

SpriteRenderTextureBug::SimpleSprite* SpriteRenderTextureBug::addNewSpriteWithCoords(const Vec2& p)
{
    int idx = AXRANDOM_0_1() * 1400 / 100;
    int x   = (idx % 5) * 85;
    int y   = (idx / 5) * 121;

    auto sprite = SpriteRenderTextureBug::SimpleSprite::create("Images/grossini_dance_atlas.png", Rect(x, y, 85, 121));
    addChild(sprite);

    sprite->setPosition(p);

    FiniteTimeAction* action = nullptr;
    float rd                 = AXRANDOM_0_1();

    if (rd < 0.20)
        action = ScaleBy::create(3, 2);
    else if (rd < 0.40)
        action = RotateBy::create(3, 360);
    else if (rd < 0.60)
        action = Blink::create(1, 3);
    else if (rd < 0.8)
        action = TintBy::create(2, 0, -255, -255);
    else
        action = FadeOut::create(2);

    auto action_back = action->reverse();
    auto seq         = Sequence::create(action, action_back, nullptr);

    sprite->runAction(RepeatForever::create(seq));

    // return sprite;
    return nullptr;
}

void SpriteRenderTextureBug::onPointerUp(PointerEvent* event)
{
    {
        auto location = event->getLocation();
        addNewSpriteWithCoords(location);
    }
}

std::string SpriteRenderTextureBug::title() const
{
    return "SpriteRenderTextureBug";
}

std::string SpriteRenderTextureBug::subtitle() const
{
    return "Touch the screen. Sprite should appear on under the touch";
}

//
// Issue16113Test
//
Issue16113Test::Issue16113Test()
{
    auto s = Director::getInstance()->getCanvasSize();

    // Save Image menu
    MenuItemFont::setFontSize(16);
    auto item1 = MenuItemFont::create("Save Image", [&](Object* ref) {
        auto canvasSize = Director::getInstance()->getVisibleSize();
        auto text       = Label::createWithTTF("hello world", "fonts/Marker Felt.ttf", 40);
        text->setTextColor(Color32::red);
        auto target = RenderTexture::create(_director->canvasToPixels(canvasSize), rhi::PixelFormat::RGBA8);
        {
            auto scope = RefPtr<RenderTexturePass>(RenderTexturePass::obtain(target), tlx::adopt_object);
            scope->begin();
            scope->clear(ClearFlag::COLOR, {.color = Color(0, 0, 0, 0)});
            text->setPosition(canvasSize.width / 2, canvasSize.height / 2);
            text->Node::visit();
            scope->end();
        }
        auto callback = [this](RenderTexture* rt, std::string_view path) { rt->release(); };
        target->retain();
        target->saveToFile("issue16113.png", Image::Format::PNG, true, callback);
    });
    auto menu  = Menu::create(item1, nullptr);
    this->addChild(menu);
    menu->setPosition(s.width / 2, s.height / 2);
}

std::string Issue16113Test::title() const
{
    return "Github Issue 16113";
}

std::string Issue16113Test::subtitle() const
{
    return "aaa.png file without white border on iOS";
}

CameraTargetTextureTest::CameraTargetTextureTest()
{
    auto s = _director->getCanvasSize();

    auto background = LayerColor::create(Color32(64, 64, 64, 255));
    this->addChild(background, -1);

    // Sprite visible only in RT (CameraFlag::USER1)
    auto targetSprite = Sprite::create("Images/grossinis_sister1.png");
    targetSprite->setPosition(Vec2(s.width / 2, s.height / 2));
    targetSprite->setCameraMask((unsigned short)CameraFlag::USER1);
    targetSprite->runAction(RepeatForever::create(RotateBy::create(2, 360)));
    this->addChild(targetSprite);

    // RenderTexture filled automatically by Scene::render() via camera targetTexture
    _rt = RenderTexture::create(_director->canvasToPixels(s), rhi::PixelFormat::RGBA8);

    // Camera with targetTexture: renders only USER1-flagged nodes to RT
    _captureCamera = Camera::createOrthographic(s.width, s.height, -1024, 1024);
    _captureCamera->setCameraFlag(CameraFlag::USER1);
    _captureCamera->setPosition3D(Vec3(s.width / 2, s.height / 2, 0));
    _captureCamera->setTargetTexture(_rt);
    this->addChild(_captureCamera);

    // Sprite displaying the RT content (DEFAULT flag, rendered by default camera)
    auto rtSprite = Sprite::createWithTexture(_rt);
    rtSprite->setPosition(Vec2(s.width * 0.15f, s.height * 0.15f));
    rtSprite->setScale(0.3f);
    rtSprite->setAnchorPoint(Anchors::center);
    this->addChild(rtSprite);
}

std::string CameraTargetTextureTest::title() const
{
    return "Camera Target Texture";
}

std::string CameraTargetTextureTest::subtitle() const
{
    return "Camera::setTargetTexture driven by Scene::render() camera loop";
}
