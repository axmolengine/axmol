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

#ifndef __cocos2d_tests__SpritePolygonTest__

#    include "../BaseTest.h"
#    include "ui/CocosGUI.h"

DEFINE_TEST_SUITE(SpritePolygonTest);

class SpritePolygonTestCase : public TestCase
{
protected:
    std::string _title;
    std::string _subtitle;
    bool _isDebugDraw;
    bool _isNeedDebugMenu;
    ax::Vector<ax::DrawNode*> _drawNodes;
    virtual std::string title() const override { return _title; };
    virtual std::string subtitle() const override { return _subtitle; };
    virtual bool init() override;
    virtual void onEnter() override;
    virtual void onExit() override;
    SpritePolygonTestCase();
    ~SpritePolygonTestCase();
    void updateDrawNode();
};

class SpritePolygonTestDemo : public SpritePolygonTestCase
{
protected:
    ax::Sprite* _polygonSprite;
    ax::Sprite* _polygonSprite_fix;
    ax::Sprite* _normalSprite;
    virtual bool init() override;
    virtual void initSprites(){};
    void initTouches();
};

class SpritePolygonTest1 : public SpritePolygonTestDemo
{
public:
    CREATE_FUNC(SpritePolygonTest1);
    SpritePolygonTest1();
    void initSprites() override;
};

class SpritePolygonTest2 : public SpritePolygonTestDemo
{
public:
    CREATE_FUNC(SpritePolygonTest2);
    SpritePolygonTest2();
    virtual void initSprites() override;
};

class SpritePolygonTestSlider : public SpritePolygonTestCase
{
protected:
    ax::Label* _epsilonLabel;
    int _tagIndex;
    ax::Sprite* makeSprite(std::string_view filename, const ax::Vec2& pos);
    virtual bool init() override;
    void initSliders();
    virtual void initSprites(){};
    void makeSprites(const std::string* list, const int count, const float y);
    void changeEpsilon(Object* pSender, ax::ui::Slider::EventType type);
    void updateLabel(const ax::Sprite* sp, const ax::PolygonInfo& pinfo);
};

class SpritePolygonTest3 : public SpritePolygonTestSlider
{
public:
    CREATE_FUNC(SpritePolygonTest3);
    SpritePolygonTest3();
    void initSprites() override;
};

class SpritePolygonTest4 : public SpritePolygonTestSlider
{
public:
    CREATE_FUNC(SpritePolygonTest4);
    SpritePolygonTest4();
    void initSprites() override;
};

class SpritePolygonTest5 : public SpritePolygonTestCase
{
public:
    CREATE_FUNC(SpritePolygonTest5);
    SpritePolygonTest5();

protected:
    virtual bool init() override;
    void initTouch();
    void loadDefaultSprites();
    void addSpritePolygon(const ax::Vec2& pos);
    void update(float dt) override;

private:
    ax::PolygonInfo _polygonInfo;
    int _tagIndex;
};

class SpritePolygonPerformance : public SpritePolygonTestCase
{
public:
    CREATE_FUNC(SpritePolygonPerformance);
    SpritePolygonPerformance();

protected:
    int _spriteCount;
    int _vertCount;
    int _triCount;
    int _pixelCount;
    float _elapsedTime;
    ax::Label* _perfLabel;
    int _continuousLowDt;
    float _continuousHighDtTime;
    float _waitingTime;

    int _posX;
    int _posY;
    int _leftX;
    int _rightX;
    bool goRight;
    bool ended;

    float prevDt;

    unsigned int _incVert;
    unsigned int _incTri;
    unsigned int _incPix;

    void updateLabel();
    virtual void update(float dt) override;
    virtual bool init() override;
    virtual Node* makeSprite();
    void incrementStats();
    virtual void initIncrementStats(){};
};

class SpritePolygonPerformanceTestDynamic : public SpritePolygonPerformance
{
public:
    CREATE_FUNC(SpritePolygonPerformanceTestDynamic);
    SpritePolygonPerformanceTestDynamic();

protected:
    ax::Sprite* makeSprite() override;
    ax::PolygonInfo _pinfo;
    virtual void initIncrementStats() override;
};

class SpritePerformanceTestDynamic : public SpritePolygonPerformance
{
public:
    CREATE_FUNC(SpritePerformanceTestDynamic);
    SpritePerformanceTestDynamic();

protected:
    virtual ax::Sprite* makeSprite() override;
    virtual void initIncrementStats() override;
};

class SpritePolygonTestNoCrash : public SpritePolygonTestDemo
{
public:
    CREATE_FUNC(SpritePolygonTestNoCrash);
    SpritePolygonTestNoCrash();
    virtual void initSprites() override;
};

class SpritePolygonTestTPIsland : public SpritePolygonTestDemo
{
public:
    CREATE_FUNC(SpritePolygonTestTPIsland);
    SpritePolygonTestTPIsland();
    virtual void initSprites() override;
};

class SpritePolygonTestAutoPolyIsland : public SpritePolygonTestDemo
{
public:
    CREATE_FUNC(SpritePolygonTestAutoPolyIsland);
    SpritePolygonTestAutoPolyIsland();
    virtual void initSprites() override;
};

class SpritePolygonTestFrameAnim : public SpritePolygonTestDemo
{
public:
    CREATE_FUNC(SpritePolygonTestFrameAnim);
    SpritePolygonTestFrameAnim();
    virtual void initSprites() override;
};

class Issue14017Test : public SpritePolygonTestDemo
{
public:
    CREATE_FUNC(Issue14017Test);
    Issue14017Test();
    virtual void initSprites() override;
};

class SpritePolygonTestPerformance : public SpritePolygonTestDemo
{
public:
    CREATE_FUNC(SpritePolygonTestPerformance);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    virtual void update(float dt) override;

protected:
    SpritePolygonTestPerformance();

    void createSprite();

    virtual ~SpritePolygonTestPerformance();
    class Ticker
    {
    public:
        Ticker(int m) : _max(m) {}
        void hit() { _cnt += 1; }
        void cancel() { _cnt = 0; }
        bool ok() { return _cnt >= _max; }

    private:
        int _cnt = 0;
        int _max = 0;
    };
    Node* _spritesAnchor       = nullptr;
    int _spriteIndex           = 0;
    float _maDt                = 1.0f / 60.0f;
    float _rmaDt               = 1.0f / 60.0f;
    const float DEST_DT_30FPS  = 1.0f / 30.0f;
    ax::Label* _totalSprites = nullptr;
    Ticker _contSlow           = Ticker(20);
    Ticker _contFast           = Ticker(2);
    Ticker _around30fps        = Ticker(60 * 3);
};

#endif /* defined(__cocos2d_tests__SpritePolygonTest__) */
