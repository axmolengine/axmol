/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
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

#include "NewRendererTest.h"
#include <chrono>
#include <sstream>
#include "renderer/backend/DriverBase.h"

namespace
{
static uint64_t s_blur_program_id = 0;
static uint64_t s_sepia_program_id = 0;
}

using namespace ax;

class DurationRecorder
{
public:
    void startTick(std::string_view key) { _durations[key] = -now(); }

    int endTick(std::string_view key)
    {
        auto n   = now();
        auto itr = _durations.find(key);
        if (_durations.find(key) == _durations.end())
        {
            return -1;
        }
        else if (itr->second < 0)
        {
            itr->second = n + itr->second;
        }
        return static_cast<int>(itr->second);
    }

    inline int64_t now() const
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();
    }

    void reset() { _durations.clear(); }

private:
    hlookup::string_map<int64_t> _durations;
};

NewRendererTests::NewRendererTests()
{
     auto programManager = ProgramManager::getInstance();
     s_blur_program_id   = programManager->registerCustomProgram(positionTextureColor_vert,
                                                  "custom/example_Blur_fs"sv,
                                                VertexLayoutType::Sprite);
     s_sepia_program_id = programManager->registerCustomProgram(positionTextureColor_vert,
                                                "custom/example_Sepia_fs"sv,
                                          VertexLayoutType::Sprite);

    ADD_TEST_CASE(NewSpriteTest);
    ADD_TEST_CASE(GroupCommandTest);
    //    ADD_TEST_CASE(NewClippingNodeTest); // When depth and stencil are used together, ...
    ADD_TEST_CASE(NewDrawNodeTest);
    ADD_TEST_CASE(NewCullingTest);
    ADD_TEST_CASE(VBOFullTest);
    ADD_TEST_CASE(CaptureScreenTest);
    ADD_TEST_CASE(CaptureNodeTest);
    ADD_TEST_CASE(BugAutoCulling);
    ADD_TEST_CASE(RendererBatchQuadTri);
    ADD_TEST_CASE(RendererUniformBatch);
    ADD_TEST_CASE(RendererUniformBatch2);
    ADD_TEST_CASE(SpriteCreation);
    ADD_TEST_CASE(NonBatchSprites);
};

std::string MultiSceneTest::title() const
{
    return "New Renderer";
}

std::string MultiSceneTest::subtitle() const
{
    return "MultiSceneTest";
}

NewSpriteTest::NewSpriteTest()
{
    auto touchListener            = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesEnded = AX_CALLBACK_2(NewSpriteTest::onTouchesEnded, this);

    createSpriteTest();
    createNewSpriteTest();
}

NewSpriteTest::~NewSpriteTest() {}

void NewSpriteTest::createSpriteTest()
{
    Size winSize = Director::getInstance()->getWinSize();

    Sprite* parent = Sprite::create("Images/grossini.png");
    parent->setPosition(winSize.width / 4, winSize.height / 2);
    Sprite* child1 = Sprite::create("Images/grossinis_sister1.png");
    child1->setPosition(0.0f, -20.0f);
    Sprite* child2 = Sprite::create("Images/grossinis_sister2.png");
    child2->setPosition(20.0f, -20.0f);
    Sprite* child3 = Sprite::create("Images/grossinis_sister1.png");
    child3->setPosition(40.0f, -20.0f);
    Sprite* child4 = Sprite::create("Images/grossinis_sister2.png");
    child4->setPosition(60.0f, -20.0f);
    Sprite* child5 = Sprite::create("Images/grossinis_sister2.png");
    child5->setPosition(80.0f, -20.0f);
    Sprite* child6 = Sprite::create("Images/grossinis_sister2.png");
    child6->setPosition(100.0f, -20.0f);
    Sprite* child7 = Sprite::create("Images/grossinis_sister2.png");
    child7->setPosition(120.0f, -20.0f);

    parent->addChild(child1);
    parent->addChild(child2);
    parent->addChild(child3);
    parent->addChild(child4);
    parent->addChild(child5);
    parent->addChild(child6);
    parent->addChild(child7);
    addChild(parent);
}

void NewSpriteTest::createNewSpriteTest()
{
    Size winSize = Director::getInstance()->getWinSize();

    Sprite* parent = Sprite::create("Images/grossini.png");
    parent->setPosition(winSize.width * 2 / 3, winSize.height / 2);
    Sprite* child1 = Sprite::create("Images/grossinis_sister1.png");
    child1->setPosition(0.0f, -20.0f);
    Sprite* child2 = Sprite::create("Images/grossinis_sister2.png");
    child2->setPosition(20.0f, -20.0f);
    Sprite* child3 = Sprite::create("Images/grossinis_sister1.png");
    child3->setPosition(40.0f, -20.0f);
    Sprite* child4 = Sprite::create("Images/grossinis_sister2.png");
    child4->setPosition(60.0f, -20.0f);
    Sprite* child5 = Sprite::create("Images/grossinis_sister2.png");
    child5->setPosition(80.0f, -20.0f);
    Sprite* child6 = Sprite::create("Images/grossinis_sister2.png");
    child6->setPosition(100.0f, -20.0f);
    Sprite* child7 = Sprite::create("Images/grossinis_sister2.png");
    child7->setPosition(120.0f, -20.0f);

    parent->addChild(child1);
    parent->addChild(child2);
    parent->addChild(child3);
    parent->addChild(child4);
    parent->addChild(child5);
    parent->addChild(child6);
    parent->addChild(child7);
    addChild(parent);
}

void NewSpriteTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event) {}

std::string NewSpriteTest::title() const
{
    return "Renderer";
}

std::string NewSpriteTest::subtitle() const
{
    return "SpriteTest";
}

class SpriteInGroupCommand : public Sprite
{
public:
    static SpriteInGroupCommand* create(std::string_view filename);

    virtual void draw(Renderer* renderer, const Mat4& transform, uint32_t flags) override;
};

SpriteInGroupCommand* SpriteInGroupCommand::create(std::string_view filename)
{
    SpriteInGroupCommand* sprite = new SpriteInGroupCommand();
    sprite->initWithFile(filename);
    sprite->autorelease();
    return sprite;
}

void SpriteInGroupCommand::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    AXASSERT(renderer, "Render is null");
    auto * spriteWrapperCommand = renderer->getNextGroupCommand();
    spriteWrapperCommand->init(_globalZOrder);
    renderer->addCommand(spriteWrapperCommand);
    renderer->pushGroup(spriteWrapperCommand->getRenderQueueID());
    Sprite::draw(renderer, transform, flags);
    renderer->popGroup();
}

GroupCommandTest::GroupCommandTest()
{
    auto sprite  = SpriteInGroupCommand::create("Images/grossini.png");
    Size winSize = Director::getInstance()->getWinSize();
    sprite->setPosition(winSize.width / 2, winSize.height / 2);
    addChild(sprite);
}

GroupCommandTest::~GroupCommandTest() {}

std::string GroupCommandTest::title() const
{
    return "Renderer";
}

std::string GroupCommandTest::subtitle() const
{
    return "GroupCommandTest: You should see a sprite";
}

NewClippingNodeTest::NewClippingNodeTest()
{
    auto s = Director::getInstance()->getWinSize();

    auto clipper = ClippingNode::create();
    clipper->setTag(kTagClipperNode);
    clipper->setContentSize(Size(200.0f, 200.0f));
    clipper->setAnchorPoint(Vec2(0.5f, 0.5f));
    clipper->setPosition(Vec2(s.width / 2, s.height / 2));

    clipper->runAction(RepeatForever::create(RotateBy::create(1, 45)));
    this->addChild(clipper);

    // TODO: Fix draw node as clip node
    //    auto stencil = NewDrawNode::create();
    //    Vec2 rectangle[4];
    //    rectangle[0] = Vec2(0, 0);
    //    rectangle[1] = Vec2(clipper->getContentSize().width, 0);
    //    rectangle[2] = Vec2(clipper->getContentSize().width, clipper->getContentSize().height);
    //    rectangle[3] = Vec2(0, clipper->getContentSize().height);
    //
    //    Color4F white(1, 1, 1, 1);
    //    stencil->drawPolygon(rectangle, 4, white, 1, white);
    //    clipper->setStencil(stencil);

    // Test with alpha Test
    clipper->setAlphaThreshold(0.05f);
    auto stencil = Sprite::create("Images/grossini.png");
    stencil->setPosition(s.width / 2, s.height / 2);
    clipper->setStencil(stencil);

    auto content = Sprite::create("Images/background2.png");
    content->setTag(kTagContentNode);
    content->setAnchorPoint(Vec2(0.5f, 0.5f));
    content->setPosition(Vec2(clipper->getContentSize().width / 2, clipper->getContentSize().height / 2));
    clipper->addChild(content);

    _scrolling = false;

    auto listener            = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = AX_CALLBACK_2(NewClippingNodeTest::onTouchesBegan, this);
    listener->onTouchesMoved = AX_CALLBACK_2(NewClippingNodeTest::onTouchesMoved, this);
    listener->onTouchesEnded = AX_CALLBACK_2(NewClippingNodeTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

NewClippingNodeTest::~NewClippingNodeTest() {}

std::string NewClippingNodeTest::title() const
{
    return "New Render";
}

std::string NewClippingNodeTest::subtitle() const
{
    return "ClipNode";
}

void NewClippingNodeTest::onTouchesBegan(const std::vector<Touch*>& touches, Event* event)
{
    Touch* touch = touches[0];
    auto clipper = this->getChildByTag(kTagClipperNode);
    Vec2 point   = clipper->convertToNodeSpace(Director::getInstance()->convertToGL(touch->getLocationInView()));
    auto rect    = Rect(0, 0, clipper->getContentSize().width, clipper->getContentSize().height);
    _scrolling   = rect.containsPoint(point);
    _lastPoint   = point;
}

void NewClippingNodeTest::onTouchesMoved(const std::vector<Touch*>& touches, Event* event)
{
    if (!_scrolling)
        return;
    Touch* touch = touches[0];
    auto clipper = this->getChildByTag(kTagClipperNode);
    auto point   = clipper->convertToNodeSpace(Director::getInstance()->convertToGL(touch->getLocationInView()));
    Vec2 diff    = point - _lastPoint;
    auto content = clipper->getChildByTag(kTagContentNode);
    content->setPosition(content->getPosition() + diff);
    _lastPoint = point;
}

void NewClippingNodeTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    if (!_scrolling)
        return;
    _scrolling = false;
}

/**
 * NewDrawNode
 */
NewDrawNodeTest::NewDrawNodeTest()
{
    auto s = Director::getInstance()->getWinSize();

    auto parent = Node::create();
    parent->setPosition(s.width / 2, s.height / 2);
    addChild(parent);

    auto rectNode = DrawNode::create();
    Vec2 rectangle[4];
    rectangle[0] = Vec2(-50, -50);
    rectangle[1] = Vec2(50, -50);
    rectangle[2] = Vec2(50, 50);
    rectangle[3] = Vec2(-50, 50);

    Color4F white(1, 1, 1, 1);
    rectNode->drawPolygon(rectangle, 4, white, 1, white);
    parent->addChild(rectNode);
}

NewDrawNodeTest::~NewDrawNodeTest() {}

std::string NewDrawNodeTest::title() const
{
    return "New Render";
}

std::string NewDrawNodeTest::subtitle() const
{
    return "DrawNode";
}

NewCullingTest::NewCullingTest()
{
    Size size   = Director::getInstance()->getWinSize();
    auto sprite = Sprite::create("Images/btn-about-normal-vertical.png");
    sprite->setRotation(5);
    sprite->setPosition(Vec2(size.width / 2, size.height / 3));
    sprite->setScale(2);
    addChild(sprite);

    auto sprite2 = Sprite::create("Images/btn-about-normal-vertical.png");
    sprite2->setRotation(-85);
    sprite2->setPosition(Vec2(size.width / 2, size.height * 2 / 3));
    sprite2->setScale(2);
    addChild(sprite2);

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = AX_CALLBACK_2(NewCullingTest::onTouchBegan, this);
    listener->onTouchMoved = AX_CALLBACK_2(NewCullingTest::onTouchMoved, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool NewCullingTest::onTouchBegan(Touch* touch, Event* event)
{
    auto pos = touch->getLocation();
    _lastPos = pos;
    return true;
}

void NewCullingTest::onTouchMoved(Touch* touch, Event* event)
{
    auto pos = touch->getLocation();

    auto offset = pos - _lastPos;

    auto layerPos = getPosition();
    auto newPos   = layerPos + offset;

    setPosition(newPos);
    _lastPos = pos;
}

NewCullingTest::~NewCullingTest() {}

std::string NewCullingTest::title() const
{
    return "New Render";
}

std::string NewCullingTest::subtitle() const
{
    return "Drag the layer to test the result of culling";
}

SpriteCreation::SpriteCreation()
{

    Size s       = Director::getInstance()->getWinSize();
    Node* parent = Node::create();
    parent->setPosition(s.width / 2, s.height / 2);
    addChild(parent);

#define KEY_CREATION "11"
#define KEY_DESTROYATION "22"

    labelCreate  = Label::createWithTTF(TTFConfig("fonts/arial.ttf"), "Sprite Creation: ..");
    labelDestory = Label::createWithTTF(TTFConfig("fonts/arial.ttf"), "Destroy Sprites: ..");

    MenuItemFont::setFontName("fonts/arial.ttf");
    MenuItemFont::setFontSize(65);
    auto decrease = MenuItemFont::create(" - ", AX_CALLBACK_1(SpriteCreation::delSpritesCallback, this));
    decrease->setColor(Color3B(0, 200, 20));
    auto increase = MenuItemFont::create(" + ", AX_CALLBACK_1(SpriteCreation::addSpritesCallback, this));
    increase->setColor(Color3B(0, 200, 20));

    auto menu = Menu::create(decrease, increase, nullptr);
    menu->alignItemsHorizontally();
    menu->setPosition(Vec2(s.width / 2, s.height - 105));
    addChild(menu, 1);

    TTFConfig ttfCount("fonts/Marker Felt.ttf", 30);
    _labelSpriteNum = Label::createWithTTF(ttfCount, "Label");
    _labelSpriteNum->setColor(Color3B(0, 200, 20));
    _labelSpriteNum->setPosition(Vec2(s.width / 2, s.height - 130));
    addChild(_labelSpriteNum);

    updateSpriteCountLabel(totalSprites);

    labelCreate->setPosition(0, -20);
    labelDestory->setPosition(0, -50);

    parent->addChild(labelCreate);
    parent->addChild(labelDestory);

    doTest();
}

void SpriteCreation::updateSpriteCountLabel(int x)
{
    totalSprites = std::max(1, x);
    std::stringstream ss;
    ss << totalSprites << " sprites";
    _labelSpriteNum->setString(ss.str());
}

void SpriteCreation::doTest()
{

    DurationRecorder perf;
    std::vector<std::string> predefineTextures = {"Images/concave.png",
                                                  "Images/atlastest.png",
                                                  "Images/grossini_dance_atlas-mono.png",
                                                  "Images/HelloWorld.png",
                                                  "Images/background1.png",
                                                  "Images/background2.png",
                                                  "Images/stone.png",
                                                  "Images/issue_17116.png",
                                                  "Images/sprite_polygon_crash.png",
                                                  "Images/bitmapFontTest3.png",
                                                  "Images/cocos-html5.png",
                                                  "Images/Fog.png",
                                                  "Images/poly_test_textures.png",
                                                  "Images/powered.png",
                                                  "Images/bug14017.png",
                                                  "Images/test-rgba1.png",
                                                  "Images/grossinis_heads.png",
                                                  "Images/cocos2dbanner.png"};

    std::vector<Sprite*> spriteCache;
    spriteCache.reserve(totalSprites);

    perf.startTick(KEY_CREATION);

    for (int i = 0; i < totalSprites; ++i)
    {
        auto* sprite = new Sprite();
        if (sprite == nullptr)
        {
            break;
        }
        if (!sprite->initWithFile(predefineTextures[i % predefineTextures.size()]))
        {
            delete sprite;
            break;
        }
        spriteCache.emplace_back(sprite);
    }

    auto creationDuration = perf.endTick(KEY_CREATION);
    perf.startTick(KEY_DESTROYATION);

    for (int i = 0; i < totalSprites; ++i)
    {
        spriteCache[i]->release();
    }
    auto destroyDuration = perf.endTick(KEY_DESTROYATION);
    std::stringstream ss;
    auto t1_ms = creationDuration * 1.0 / 1000000;
    ss << "Create " << spriteCache.size() << " sprites takes " << t1_ms << " ms, "
       << (int64_t)(spriteCache.size() * 1000 / t1_ms) << " sprites per second!";
    labelCreate->setString(ss.str());

    if (t1_ms < 100)
    {
        suggestDelta = (int)(0.5 * totalSprites);
    }
    else if (t1_ms < 1000)
    {
        suggestDelta = (int)(0.2 * totalSprites);
    }
    else if (t1_ms)
    {
        suggestDelta = (int)(0.1 * totalSprites);
    }

    suggestDelta = suggestDelta < 1000 ? 1000 : suggestDelta - suggestDelta % 1000;

    ss.str("");
    auto t2_ms = destroyDuration * 1.0 / 1000000;
    ss << "Destroy " << spriteCache.size() << " sprites takes " << t2_ms << " ms, "
       << (int64_t)(spriteCache.size() * 1000 / t2_ms) << " sprites per second!";
    labelDestory->setString(ss.str());

    spriteCache.clear();
}

void SpriteCreation::addSpritesCallback(ax::Object*)
{
    updateSpriteCountLabel(totalSprites + suggestDelta);
    doTest();
}

void SpriteCreation::delSpritesCallback(ax::Object*)
{
    updateSpriteCountLabel(totalSprites - suggestDelta);
    doTest();
}

SpriteCreation::~SpriteCreation() {}

std::string SpriteCreation::title() const
{
    return "Sprite Creation";
}

std::string SpriteCreation::subtitle() const
{
#if defined(_AX_DEBUG) && _AX_DEBUG == 1
    return "In debug mode";
#else
    return "In release mode";
#endif
}

VBOFullTest::VBOFullTest()
{
    Size s       = Director::getInstance()->getWinSize();
    Node* parent = Node::create();
    parent->setPosition(0, 0);
    addChild(parent);

    for (int i = 0; i < Renderer::VBO_SIZE / 3.9; ++i)
    {
        Sprite* sprite = Sprite::create("Images/grossini_dance_01.png");
        sprite->setScale(0.1f, 0.1f);
        float x = ((float)std::rand()) / RAND_MAX;
        float y = ((float)std::rand()) / RAND_MAX;
        sprite->setPosition(Vec2(x * s.width, y * s.height));
        parent->addChild(sprite);
    }
}

VBOFullTest::~VBOFullTest() {}

std::string VBOFullTest::title() const
{
    return "New Renderer";
}

std::string VBOFullTest::subtitle() const
{
    return "VBO full Test, everything should render normally";
}

CaptureScreenTest::CaptureScreenTest()
{
    Size s = Director::getInstance()->getWinSize();
    Vec2 left(s.width / 4, s.height / 2);
    Vec2 right(s.width / 4 * 3, s.height / 2);

    auto sp1 = Sprite::create("Images/grossini.png");
    sp1->setPosition(left);
    auto move1 = MoveBy::create(1, Vec2(s.width / 2, 0.0f));
    auto seq1  = RepeatForever::create(Sequence::create(move1, move1->reverse(), nullptr));
    addChild(sp1);
    sp1->runAction(seq1);
    auto sp2 = Sprite::create("Images/grossinis_sister1.png");
    sp2->setPosition(right);
    auto move2 = MoveBy::create(1, Vec2(-s.width / 2, 0.0f));
    auto seq2  = RepeatForever::create(Sequence::create(move2, move2->reverse(), nullptr));
    addChild(sp2);
    sp2->runAction(seq2);

    auto label1 = Label::createWithTTF(TTFConfig("fonts/arial.ttf"), "capture all");
    auto mi1    = MenuItemLabel::create(label1, AX_CALLBACK_1(CaptureScreenTest::onCaptured, this));
    auto menu   = Menu::create(mi1, nullptr);
    addChild(menu);
    menu->setPosition(s.width / 2, s.height / 4);

    _filename = "";
}

CaptureScreenTest::~CaptureScreenTest()
{
    Director::getInstance()->getTextureCache()->removeTextureForKey(_filename);
}

std::string CaptureScreenTest::title() const
{
    return "New Renderer";
}

std::string CaptureScreenTest::subtitle() const
{
    return "Capture screen test, press the menu items to capture the screen";
}

void CaptureScreenTest::onCaptured(Object*)
{
    Director::getInstance()->getTextureCache()->removeTextureForKey(_filename);
    removeChildByTag(childTag);
    _filename = "CaptureScreenTest.png";
    // retain it to avoid crash caused by invoking afterCaptured
    this->retain();
    utils::captureScreen(AX_CALLBACK_2(CaptureScreenTest::afterCaptured, this), _filename);
}

void CaptureScreenTest::afterCaptured(bool succeed, std::string_view outputFile)
{
    if (succeed)
    {
        auto sp = Sprite::create(outputFile);
        addChild(sp, 0, childTag);
        Size s = Director::getInstance()->getWinSize();
        sp->setPosition(s.width / 2, s.height / 2);
        sp->setScale(0.25);
        _filename = outputFile;
    }
    else
    {
        AXLOGW("Capture screen failed.");
    }

    // release it since it is retained in `CaptureScreenTest::onCaptured()`
    this->release();
}

CaptureNodeTest::CaptureNodeTest()
{
    Size s = Director::getInstance()->getWinSize();
    Vec2 left(s.width / 4, s.height / 2);
    Vec2 right(s.width / 4 * 3, s.height / 2);

    auto sp1 = Sprite::create("Images/grossini.png");
    sp1->setPosition(left);
    auto move1 = MoveBy::create(1, Vec2(s.width / 2, 0.0f));
    auto seq1  = RepeatForever::create(Sequence::create(move1, move1->reverse(), nullptr));
    addChild(sp1);
    sp1->runAction(seq1);
    auto sp2 = Sprite::create("Images/grossinis_sister1.png");
    sp2->setPosition(right);
    auto move2 = MoveBy::create(1, Vec2(-s.width / 2, 0.0f));
    auto seq2  = RepeatForever::create(Sequence::create(move2, move2->reverse(), nullptr));
    addChild(sp2);
    sp2->runAction(seq2);

    auto label1 = Label::createWithTTF(TTFConfig("fonts/arial.ttf"), "capture this scene");
    auto mi1    = MenuItemLabel::create(label1, AX_CALLBACK_1(CaptureNodeTest::onCaptured, this));
    auto menu   = Menu::create(mi1, nullptr);
    addChild(menu);
    menu->setPosition(s.width / 2, s.height / 4);

    _filename = "";
}

CaptureNodeTest::~CaptureNodeTest()
{
    Director::getInstance()->getTextureCache()->removeTextureForKey(_filename);
}

std::string CaptureNodeTest::title() const
{
    return "New Renderer";
}

std::string CaptureNodeTest::subtitle() const
{
    return "Capture node test, press the menu items to capture this scene with scale 0.5";
}

void CaptureNodeTest::onCaptured(Object*)
{
    Director::getInstance()->getTextureCache()->removeTextureForKey(_filename);
    removeChildByTag(childTag);

    _filename = FileUtils::getInstance()->getWritablePath() + "/CaptureNodeTest.png";

    // capture this
    auto callback = [&](RefPtr<Image> image) {
        // create a sprite with the captured image directly
        auto sp = Sprite::createWithTexture(Director::getInstance()->getTextureCache()->addImage(image, _filename));
        addChild(sp, 0, childTag);
        Size s = Director::getInstance()->getWinSize();
        sp->setPosition(s.width / 2, s.height / 2);

        // store to disk
        image->saveToFile(_filename, false);
    };

    auto callbackFunction = std::bind(callback, std::placeholders::_1);
    utils::captureNode(this, callbackFunction, 0.5);
}

BugAutoCulling::BugAutoCulling()
{
    Size s       = Director::getInstance()->getWinSize();
    auto fastmap = ax::FastTMXTiledMap::create("TileMaps/orthogonal-test2.tmx");
    this->addChild(fastmap);
    for (int i = 0; i < 30; i++)
    {
        auto sprite = Sprite::create("Images/grossini.png");
        sprite->setPosition(s.width / 2 + s.width / 10 * i, s.height / 2);
        this->addChild(sprite);
        auto label = Label::createWithTTF(TTFConfig("fonts/arial.ttf"), "Label");
        label->setPosition(s.width / 2 + s.width / 10 * i, s.height / 2);
        this->addChild(label);
    }
    this->scheduleOnce(
        [=](float) {
            auto camera = Director::getInstance()->getRunningScene()->getCameras().front();
            auto move   = MoveBy::create(2.0f, Vec2(2 * s.width, 0.0f));
            camera->runAction(Sequence::create(move, move->reverse(), nullptr));
        },
        1.0f, "lambda-autoculling-bug");
}

std::string BugAutoCulling::title() const
{
    return "Bug-AutoCulling";
}

std::string BugAutoCulling::subtitle() const
{
    return "Moving the camera to the right instead of moving the layer";
}

//
// RendererBatchQuadTri
//

RendererBatchQuadTri::RendererBatchQuadTri()
{
    Size s = Director::getInstance()->getWinSize();

    for (int i = 0; i < 250; i++)
    {
        int x = AXRANDOM_0_1() * s.width;
        int y = AXRANDOM_0_1() * s.height;

        auto label = LabelAtlas::create("This is a label", "fonts/tuffy_bold_italic-charmap.plist");
        label->setColor(Color3B::RED);
        label->setPosition(Vec2(x, y));
        addChild(label);

        auto sprite = Sprite::create("fonts/tuffy_bold_italic-charmap.png");
        sprite->setTextureRect(Rect(0.0f, 0.0f, 100.0f, 100.0f));
        sprite->setPosition(Vec2(x, y));
        sprite->setColor(Color3B::BLUE);
        addChild(sprite);
    }
}

std::string RendererBatchQuadTri::title() const
{
    return "RendererBatchQuadTri";
}

std::string RendererBatchQuadTri::subtitle() const
{
    return "QuadCommand and TriangleCommands are batched together";
}

//
//
// RendererUniformBatch
//

RendererUniformBatch::RendererUniformBatch()
{
    Size s = Director::getInstance()->getWinSize();

    auto blurState  = createBlurProgramState();
    auto sepiaState = createSepiaProgramState();

    auto x_inc = s.width / 20;
    auto y_inc = s.height / 6;

    for (int y = 0; y < 6; ++y)
    {
        for (int x = 0; x < 20; ++x)
        {
            auto sprite = Sprite::create("Images/grossini.png");
            sprite->setPosition(Vec2(x * x_inc, y * y_inc));
            sprite->setScale(0.4);
            addChild(sprite);

            if (y >= 4)
            {
                sprite->setProgramState(sepiaState);
            }
            else if (y >= 2)
            {
                sprite->setProgramState(blurState);
            }
        }
    }
}

ax::backend::ProgramState* RendererUniformBatch::createBlurProgramState()
{
    auto programState =
        new backend::ProgramState(ProgramManager::getInstance()->loadProgram(s_blur_program_id));
    programState->autorelease();

    backend::UniformLocation loc = programState->getUniformLocation("resolution");
    auto resolution              = Vec2(85, 121);
    programState->setUniform(loc, &resolution, sizeof(resolution));

    loc              = programState->getUniformLocation("blurRadius");
    float blurRadius = 10.0f;
    programState->setUniform(loc, &blurRadius, sizeof(blurRadius));

    loc             = programState->getUniformLocation("sampleNum");
    float sampleNum = 5.0f;
    programState->setUniform(loc, &sampleNum, sizeof(sampleNum));

    return programState;
}

ax::backend::ProgramState* RendererUniformBatch::createSepiaProgramState()
{
    auto programState = new backend::ProgramState(ProgramManager::getInstance()->loadProgram(s_sepia_program_id));
    programState->autorelease();
    return programState;
}

std::string RendererUniformBatch::title() const
{
    return "RendererUniformBatch";
}

std::string RendererUniformBatch::subtitle() const
{
    return "Only 9 draw calls should appear";
}

////
//// RendererUniformBatch2
////

RendererUniformBatch2::RendererUniformBatch2()
{
    Size s = Director::getInstance()->getWinSize();

    auto blurState  = createBlurProgramState();
    auto sepiaState = createSepiaProgramState();

    auto x_inc = s.width / 20;
    auto y_inc = s.height / 6;

    for (int y = 0; y < 6; ++y)
    {
        for (int x = 0; x < 20; ++x)
        {
            auto sprite = Sprite::create("Images/grossini.png");
            sprite->setPosition(Vec2(x * x_inc, y * y_inc));
            sprite->setScale(0.4);
            addChild(sprite);

            auto r = AXRANDOM_0_1();
            if (r < 0.33)
                sprite->setProgramState(sepiaState);
            else if (r < 0.66)
                sprite->setProgramState(blurState);
        }
    }
}

backend::ProgramState* RendererUniformBatch2::createBlurProgramState()
{
    auto programState =
        new backend::ProgramState(ProgramManager::getInstance()->loadProgram(s_blur_program_id));

    backend::UniformLocation loc = programState->getUniformLocation("resolution");
    auto resolution              = Vec2(85, 121);
    programState->setUniform(loc, &resolution, sizeof(resolution));

    loc              = programState->getUniformLocation("blurRadius");
    float blurRadius = 10.0f;
    programState->setUniform(loc, &blurRadius, sizeof(blurRadius));

    loc             = programState->getUniformLocation("sampleNum");
    float sampleNum = 5.0f;
    programState->setUniform(loc, &sampleNum, sizeof(sampleNum));

    return programState;
}

backend::ProgramState* RendererUniformBatch2::createSepiaProgramState()
{
    auto programState =
        new backend::ProgramState(ProgramManager::getInstance()->loadProgram(s_sepia_program_id));
    programState->autorelease();
    return programState;
}

std::string RendererUniformBatch2::title() const
{
    return "RendererUniformBatch 2";
}

std::string RendererUniformBatch2::subtitle() const
{
    return "Mixing different shader states should work ok";
}

NonBatchSprites::NonBatchSprites()
{
    Size s         = Director::getInstance()->getWinSize();
    _spritesAnchor = Node::create();
    _spritesAnchor->setPosition(0, 0);
    addChild(_spritesAnchor);

    _totalSprites = Label::createWithTTF(TTFConfig("fonts/arial.ttf"), "sprites");
    _totalSprites->setColor(Color3B::YELLOW);
    _totalSprites->enableOutline(Color4B::RED, 2);
    _totalSprites->setPosition(s.width / 2, s.height / 2);

    addChild(_totalSprites);

    scheduleUpdate();
}

void NonBatchSprites::createSprite()
{

    Size s         = Director::getInstance()->getWinSize();
    Sprite* sprite = nullptr;
    if (_spriteIndex % 2 == 0)
    {
        sprite = Sprite::create("Images/grossini_dance_05.png");
    }
    else
    {
        sprite = Sprite::create("Images/grossini_dance_01.png");
    }

    if (!sprite)
        return;
    auto r = rand_0_1() * 0.6 + 0.2;
    sprite->setScale(r, r);
    float x = ((float)std::rand()) / RAND_MAX;
    float y = ((float)std::rand()) / RAND_MAX;
    sprite->runAction(RepeatForever::create(RotateBy::create(1, 45)));

    sprite->setPosition(Vec2(x * s.width, y * s.height));
    _spritesAnchor->addChild(sprite);

    _spriteIndex++;
    std::stringstream ss;
    ss << _spriteIndex << " sprites";
    _totalSprites->setString(ss.str());
}

void NonBatchSprites::update(float dt)
{

    if (dt <= 1.0f / 28.0f && dt >= 1.0f / 31.0f)
    {
        _around30fps.hit();
    }
    else
    {
        _around30fps.cancel();
    }

    _maDt  = 0.7f * _maDt + 0.3f * dt;
    _rmaDt = 0.5f * _rmaDt + 0.5f * dt;
    if (_maDt <= DEST_DT_30FPS)
    {
        _contSlow.cancel();
        _contFast.hit();
        if (_contFast.ok())
        {
            auto t2    = DEST_DT_30FPS - _rmaDt;
            auto delta = (int)(t2 / _rmaDt * _spriteIndex * 0.1);
            delta      = std::min(20, std::max(1, delta));
            for (int i = 0; i < delta; i++)
            {
                createSprite();
            }
        }
    }
    else
    {
        _contSlow.hit();
        _contFast.cancel();
    }

    if (_contSlow.ok() || _around30fps.ok())
    {
        unscheduleUpdate();
        std::stringstream ss;
        ss << _spriteIndex << " sprites, DONE!";
        _totalSprites->setString(ss.str());
        _totalSprites->setScale(1.2);
    }
}

NonBatchSprites::~NonBatchSprites() {}

std::string NonBatchSprites::title() const
{
    return "Non Batched Sprites";
}

std::string NonBatchSprites::subtitle() const
{
#if defined(_AX_DEBUG) && _AX_DEBUG == 1
    return "DEBUG: simulate lots of sprites, drop to 30 fps";
#else
    return "RELEASE: simulate lots of sprites, drop to 30 fps";
#endif
}
