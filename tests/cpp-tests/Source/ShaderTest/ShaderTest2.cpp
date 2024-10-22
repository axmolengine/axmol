/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
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

#include "ShaderTest2.h"
#include "ShaderTest.h"
#include "../testResource.h"
#include "axmol.h"
#include "renderer/backend/DriverBase.h"
#include <tuple>

using namespace ax;

#define SET_UNIFORM(ps, name, value)                          \
    do                                                        \
    {                                                         \
        decltype(value) __v = value;                          \
        auto __loc          = (ps)->getUniformLocation(name); \
        (ps)->setUniform(__loc, &__v, sizeof(__v));           \
    } while (false)

#define SET_TEXTURE(ps, name, idx, value)            \
    do                                               \
    {                                                \
        auto* __v  = value;                          \
        auto __loc = (ps)->getUniformLocation(name); \
        (ps)->setTexture(__loc, idx, __v);           \
    } while (false)

Shader2Tests::Shader2Tests()
{
    ADD_TEST_CASE(EffectSpriteTest);
    ADD_TEST_CASE(EffectSpriteLamp);
}

//
// EffectSprite
//
static int tuple_sort(const std::tuple<ssize_t, Effect*, QuadCommand>& tuple1,
                      const std::tuple<ssize_t, Effect*, QuadCommand>& tuple2)
{
    return std::get<0>(tuple1) < std::get<0>(tuple2);
}

static void updateUniforms(backend::ProgramState* programState)
{
    float time = Director::getInstance()->getTotalFrames() * Director::getInstance()->getAnimationInterval();
    Vec4 uTime(time / 10.0f, time, time * 2.0f, time * 4.0f);
    Vec4 sinTime(time / 8.0f, time / 4.0f, time / 2.0f, sinf(time));
    Vec4 cosTime(time / 8.0f, time / 4.0f, time / 2.0f, cosf(time));

    SET_UNIFORM(programState, "u_Time", uTime);
    SET_UNIFORM(programState, "u_SinTime", sinTime);
    SET_UNIFORM(programState, "u_CosTime", cosTime);
}

class Effect;
class EffectSprite : public Sprite
{
public:
    static EffectSprite* create(std::string_view filename)
    {
        auto ret = new EffectSprite;
        if (ret->initWithFile(filename))
        {
            ret->autorelease();
            return ret;
        }
        AX_SAFE_RELEASE(ret);
        return nullptr;
    }

    void setEffect(Effect* effect)
    {
        if (_defaultEffect != effect)
        {
            effect->setTarget(this);

            AX_SAFE_RELEASE(_defaultEffect);
            _defaultEffect = effect;
            AX_SAFE_RETAIN(_defaultEffect);

            setProgramState(_defaultEffect->getProgramState());
        }
    }
    void addEffect(Effect* effect, ssize_t order)
    {
        effect->retain();
        effect->setTarget(this);

        _effects.emplace_back(std::make_tuple(order, effect, QuadCommand()));

        std::sort(std::begin(_effects), std::end(_effects), tuple_sort);
    }

    void draw(Renderer* renderer, const Mat4& transform, uint32_t flags) override
    {
#if AX_USE_CULLING
        // Don't do calculate the culling if the transform was not updated
        _insideBounds =
            (flags & FLAGS_TRANSFORM_DIRTY) ? renderer->checkVisibility(transform, _contentSize) : _insideBounds;

        if (_insideBounds)
#endif
        {
            // negative effects: order < 0
            int idx = 0;
            for (auto&&effect : _effects)
            {

                if (std::get<0>(effect) >= 0)
                    break;
                auto* programState = std::get<1>(effect)->getProgramState();
                if (programState)
                {
                    QuadCommand& q = std::get<2>(effect);
                    q.init(_globalZOrder, _texture, _blendFunc, &_quad, 1, transform, flags);
                    updateUniforms(programState);
                    renderer->addCommand(&q);
                }
                idx++;
            }

            // normal effect: order == 0
            _trianglesCommand.init(_globalZOrder, _texture, _blendFunc, _polyInfo.triangles, transform, flags);

            updateUniforms(_trianglesCommand.getPipelineDescriptor().programState);
            renderer->addCommand(&_trianglesCommand);

            // positive effects: order >= 0
            for (auto&& it = std::begin(_effects) + idx; it != std::end(_effects); ++it)
            {
                QuadCommand& q     = std::get<2>(*it);
                auto* programState = std::get<1>(*it)->getProgramState();
                updateUniforms(programState);
                q.init(_globalZOrder, _texture, _blendFunc, &_quad, 1, transform, flags);
                q.getPipelineDescriptor().programState = programState;
                renderer->addCommand(&q);
                idx++;
            }
        }
    }

protected:
    EffectSprite() : _defaultEffect(nullptr) { _effects.reserve(2); }
    ~EffectSprite()
    {
        for (auto&&tuple : _effects)
        {
            std::get<1>(tuple)->release();
        }
        AX_SAFE_RELEASE(_defaultEffect);
    }

    std::vector<std::tuple<ssize_t, Effect*, QuadCommand>> _effects;
    Effect* _defaultEffect;
};

//
// Effect
//

bool Effect::initProgramState(std::string_view fragmentFilename)
{
    auto program      = ProgramManager::getInstance()->loadProgram(positionTextureColor_vert, fragmentFilename, VertexLayoutType::Sprite);
    auto programState = new backend::ProgramState(program);
    AX_SAFE_RELEASE(_programState);
    _programState = programState;

    return _programState != nullptr;
}

Effect::Effect() {}

Effect::~Effect()
{
    AX_SAFE_RELEASE_NULL(_programState);
}

// Blur
class EffectBlur : public Effect
{
public:
    CREATE_FUNC(EffectBlur);
    virtual void setTarget(EffectSprite* sprite) override;
    void setBlurRadius(float radius);
    void setBlurSampleNum(float num);

protected:
    bool init(float blurRadius = 10.0f, float sampleNum = 5.0f);

    float _blurRadius;
    float _blurSampleNum;
};

void EffectBlur::setTarget(EffectSprite* sprite)
{
    if (_programState == nullptr)
        return;

    Size size = sprite->getTexture()->getContentSizeInPixels();
    SET_UNIFORM(_programState, "resolution", size);
    SET_UNIFORM(_programState, "blurRadius", _blurRadius);
    SET_UNIFORM(_programState, "sampleNum", _blurSampleNum);
}

bool EffectBlur::init(float blurRadius, float sampleNum)
{
    initProgramState("custom/example_Blur_fs");
    _blurRadius    = blurRadius;
    _blurSampleNum = sampleNum;

    return true;
}

void EffectBlur::setBlurRadius(float radius)
{
    _blurRadius = radius;
}

void EffectBlur::setBlurSampleNum(float num)
{
    _blurSampleNum = num;
}

// Outline
class EffectOutline : public Effect
{
public:
    CREATE_FUNC(EffectOutline);

    bool init()
    {
        initProgramState("custom/example_Outline_fs");

        Vec3 color(1.0f, 0.2f, 0.3f);
        float radius    = 0.01f;
        float threshold = 1.75;

        SET_UNIFORM(_programState, "u_outlineColor", color);
        SET_UNIFORM(_programState, "u_radius", radius);
        SET_UNIFORM(_programState, "u_threshold", threshold);
        return true;
    }
};

// Noise
class EffectNoise : public Effect
{
public:
    CREATE_FUNC(EffectNoise);

protected:
    bool init()
    {
        initProgramState("custom/example_Noisy_fs");
        return true;
    }

    virtual void setTarget(EffectSprite* sprite) override
    {
        auto s = sprite->getTexture()->getContentSizeInPixels();
        SET_UNIFORM(_programState, "resolution", Vec2(s.width, s.height));
    }
};

// Edge Detect
class EffectEdgeDetect : public Effect
{
public:
    CREATE_FUNC(EffectEdgeDetect);

protected:
    bool init()
    {
        initProgramState("custom/example_EdgeDetection_fs");
        return true;
    }

    virtual void setTarget(EffectSprite* sprite) override
    {
        auto s = sprite->getTexture()->getContentSizeInPixels();
        SET_UNIFORM(_programState, "resolution", Vec2(s.width, s.height));
    }
};

// Grey
class EffectGreyScale : public Effect
{
public:
    CREATE_FUNC(EffectGreyScale);

protected:
    bool init()
    {
        initProgramState("custom/example_GreyScale_fs");
        return true;
    }
};

// Sepia
class EffectSepia : public Effect
{
public:
    CREATE_FUNC(EffectSepia);

protected:
    bool init()
    {
        initProgramState("custom/example_Sepia_fs");
        return true;
    }
};

// bloom
class EffectBloom : public Effect
{
public:
    CREATE_FUNC(EffectBloom);

protected:
    bool init()
    {
        initProgramState("custom/example_Bloom_fs");
        return true;
    }

    virtual void setTarget(EffectSprite* sprite) override
    {
        auto s = sprite->getTexture()->getContentSizeInPixels();
        SET_UNIFORM(_programState, "resolution", Vec2(s.width, s.height));
    }
};

// cel shading
class EffectCelShading : public Effect
{
public:
    CREATE_FUNC(EffectCelShading);

protected:
    bool init()
    {
        initProgramState("custom/example_CelShading_fs");
        return true;
    }

    virtual void setTarget(EffectSprite* sprite) override
    {
        auto s = sprite->getTexture()->getContentSizeInPixels();
        SET_UNIFORM(_programState, "resolution", Vec2(s.width, s.height));
    }
};

// Lens Flare
class EffectLensFlare : public Effect
{
public:
    CREATE_FUNC(EffectLensFlare);

protected:
    bool init()
    {
        initProgramState("custom/example_LensFlare_fs");
        return true;
    }

    virtual void setTarget(EffectSprite* sprite) override
    {
        auto s = sprite->getTexture()->getContentSizeInPixels();
        SET_UNIFORM(_programState, "textureResolution", Vec2(s.width, s.height));
        s = Director::getInstance()->getWinSize();
        SET_UNIFORM(_programState, "resolution", Vec2(s.width, s.height));
    }
};

class EffectNormalMapped : public Effect
{
public:
    CREATE_FUNC(EffectNormalMapped);
    static EffectNormalMapped* create(std::string_view normalMapFileName)
    {
        EffectNormalMapped* normalMappedSprite = new EffectNormalMapped();
        if (normalMappedSprite->init() && normalMappedSprite->initNormalMap(normalMapFileName))
        {

            normalMappedSprite->autorelease();
            return normalMappedSprite;
        }
        AX_SAFE_DELETE(normalMappedSprite);
        return nullptr;
    }
    void setKBump(float value);
    void setLightPos(const Vec3& pos);
    void setLightColor(const Color& color);
    float getKBump() const { return _kBump; }

protected:
    bool init();
    bool initNormalMap(std::string_view normalMapFileName);
    virtual void setTarget(EffectSprite* sprite) override;
    EffectSprite* _sprite;
    Vec3 _lightPos;
    Color _lightColor;
    float _kBump;
};

bool EffectNormalMapped::init()
{
    initProgramState("custom/Normal_fs");
    _kBump = 2;
    return true;
}
bool EffectNormalMapped::initNormalMap(std::string_view normalMapFileName)
{
    auto normalMapTexture = Director::getInstance()->getTextureCache()->addImage(normalMapFileName);
    SET_TEXTURE(_programState, "u_normalMap", 1, normalMapTexture->getBackendTexture());
    return true;
}
void EffectNormalMapped::setTarget(EffectSprite* sprite)
{
    _sprite = sprite;
    SET_UNIFORM(_programState, "u_kBump", _kBump);
    SET_UNIFORM(_programState, "u_contentSize", Vec2(sprite->getContentSize().width, sprite->getContentSize().height));
}

void EffectNormalMapped::setKBump(float value)
{
    _kBump = value;
    SET_UNIFORM(_programState, "u_kBump", _kBump);
}

void EffectNormalMapped::setLightPos(const Vec3& pos)
{
    _lightPos = pos;
    SET_UNIFORM(_programState, "u_lightPosInLocalSpace", Vec4(_lightPos.x, _lightPos.y, _lightPos.z, 1));
}

void EffectNormalMapped::setLightColor(const Color& color)
{
    _lightColor = color;
    SET_UNIFORM(_programState, "u_diffuseL", Vec3(_lightColor.r, _lightColor.g, _lightColor.b));
}

EffectSpriteTest::EffectSpriteTest() {}

bool EffectSpriteTest::init()
{
    if (ShaderTestDemo2::init())
    {

        auto layer = LayerColor::create(Color4B::BLUE);
        this->addChild(layer);

        auto s = Director::getInstance()->getWinSize();

        auto itemPrev = MenuItemImage::create("Images/b1.png", "Images/b2.png", [&](Object* sender) {
            _vectorIndex--;
            if (_vectorIndex < 0)
                _vectorIndex = _effects.size() - 1;
            _sprite->setEffect(_effects.at(_vectorIndex));
        });

        auto itemNext = MenuItemImage::create("Images/f1.png", "Images/f2.png", [&](Object* sender) {
            _vectorIndex++;
            if (_vectorIndex >= _effects.size())
                _vectorIndex = 0;
            _sprite->setEffect(_effects.at(_vectorIndex));
        });

        auto menu = Menu::create(itemPrev, itemNext, nullptr);
        menu->alignItemsHorizontally();
        menu->setScale(0.5);
        menu->setAnchorPoint(Vec2(0.0f, 0.0f));
        menu->setPosition(Vec2(s.width / 2, 70.0f));
        addChild(menu);

        _sprite = EffectSprite::create("Images/grossini.png");
        _sprite->setPosition(Vec2(0.0f, s.height / 2));
        addChild(_sprite);

        auto jump   = JumpBy::create(4, Vec2(s.width, 0.0f), 100, 4);
        auto rot    = RotateBy::create(4, 720);
        auto spawn  = Spawn::create(jump, rot, nullptr);
        auto rev    = spawn->reverse();
        auto seq    = Sequence::create(spawn, rev, nullptr);
        auto repeat = RepeatForever::create(seq);
        _sprite->runAction(repeat);

        // set the Effects
        _effects.pushBack(EffectBlur::create());
        _effects.pushBack(EffectOutline::create());
        _effects.pushBack(EffectNoise::create());
        _effects.pushBack(EffectEdgeDetect::create());
        _effects.pushBack(EffectGreyScale::create());
        _effects.pushBack(EffectSepia::create());
        _effects.pushBack(EffectBloom::create());
        _effects.pushBack(EffectCelShading::create());
        _effects.pushBack(EffectLensFlare::create());

        _vectorIndex = 0;
        _sprite->setEffect(_effects.at(_vectorIndex));

        //        _sprite->addEffect( _effects.at(8), -10 );
        //        _sprite->addEffect( _effects.at(1), 1 );

        return true;
    }
    return false;
}

EffectSpriteLamp::EffectSpriteLamp() {}

bool EffectSpriteLamp::init()
{
    if (ShaderTestDemo2::init())
    {

        auto s  = Director::getInstance()->getWinSize();
        _sprite = EffectSprite::create("Images/elephant1_Diffuse.png");
        // auto contentSize = _sprite->getContentSize();
        _sprite->setPosition(Vec2(s.width / 2, s.height / 2));
        addChild(_sprite);

        auto lampEffect = EffectNormalMapped::create("Images/elephant1_Normal.png");

        Vec3 pos(150, 150, 50);
        _lightSprite = Sprite::create("Images/ball.png");
        this->addChild(_lightSprite);
        _lightSprite->setPosition(Vec2(pos.x, s.height - pos.y));
        Mat4 mat = _sprite->getNodeToWorldTransform();
        Point lightPosInLocalSpace =
            PointApplyAffineTransform(Vec2(pos.x, pos.y), _sprite->getWorldToNodeAffineTransform());
        lampEffect->setLightColor(Color(1, 1, 1, 1));
        lampEffect->setLightPos(Vec3(lightPosInLocalSpace.x, lightPosInLocalSpace.y, 50.0f));
        lampEffect->setKBump(2);
        _sprite->setEffect(lampEffect);
        _effect                  = lampEffect;
        auto listener            = EventListenerTouchAllAtOnce::create();
        listener->onTouchesBegan = AX_CALLBACK_2(EffectSpriteLamp::onTouchesBegan, this);
        listener->onTouchesMoved = AX_CALLBACK_2(EffectSpriteLamp::onTouchesMoved, this);
        listener->onTouchesEnded = AX_CALLBACK_2(EffectSpriteLamp::onTouchesEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
        return true;
    }
    return false;
}

void EffectSpriteLamp::onTouchesBegan(const std::vector<Touch*>& touches, Event* unused_event)
{
    for (auto&&item : touches)
    {
        auto touch         = item;
        auto s             = Director::getInstance()->getWinSize();
        Point loc_winSpace = touch->getLocationInView();
        _lightSprite->setPosition(Vec2(loc_winSpace.x, s.height - loc_winSpace.y));
        Vec3 pos(loc_winSpace.x, loc_winSpace.y, 50);
        Mat4 mat = _sprite->getNodeToWorldTransform();
        Point lightPosInLocalSpace =
            PointApplyAffineTransform(Vec2(pos.x, pos.y), _sprite->getWorldToNodeAffineTransform());
        ((EffectNormalMapped*)_effect)->setLightPos(Vec3(lightPosInLocalSpace.x, lightPosInLocalSpace.y, 50.0f));
    }
}

void EffectSpriteLamp::onTouchesMoved(const std::vector<Touch*>& touches, Event* unused_event)
{
    for (auto&&item : touches)
    {
        auto touch         = item;
        auto s             = Director::getInstance()->getWinSize();
        Point loc_winSpace = touch->getLocationInView();
        _lightSprite->setPosition(Vec2(loc_winSpace.x, s.height - loc_winSpace.y));
        Vec3 pos(loc_winSpace.x, loc_winSpace.y, 50);
        Mat4 mat = _sprite->getNodeToWorldTransform();
        Point lightPosInLocalSpace =
            PointApplyAffineTransform(Vec2(pos.x, pos.y), _sprite->getWorldToNodeAffineTransform());
        ((EffectNormalMapped*)_effect)->setLightPos(Vec3(lightPosInLocalSpace.x, lightPosInLocalSpace.y, 50.0f));
    }
}

void EffectSpriteLamp::onTouchesEnded(const std::vector<Touch*>& touches, Event* unused_event)
{
    for (auto&&item : touches)
    {
        auto touch         = item;
        auto s             = Director::getInstance()->getWinSize();
        Point loc_winSpace = touch->getLocationInView();
        _lightSprite->setPosition(Vec2(loc_winSpace.x, s.height - loc_winSpace.y));
        Vec3 pos(loc_winSpace.x, loc_winSpace.y, 50);
        Mat4 mat = _sprite->getNodeToWorldTransform();
        Point lightPosInLocalSpace =
            PointApplyAffineTransform(Vec2(pos.x, pos.y), _sprite->getWorldToNodeAffineTransform());
        ((EffectNormalMapped*)_effect)->setLightPos(Vec3(lightPosInLocalSpace.x, lightPosInLocalSpace.y, 50.0f));
    }
}
