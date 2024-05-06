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

#ifndef _PARTICLE_TEST_H_
#define _PARTICLE_TEST_H_

#include "../BaseTest.h"

DEFINE_TEST_SUITE(ParticleTests);

class ParticleDemo : public TestCase
{
protected:
    ax::ParticleSystemQuad* _emitter;
    ax::Sprite* _background;
    ax::LayerColor* _color;

public:
    ~ParticleDemo();

    virtual void onEnter() override;

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void toggleCallback(ax::Object* sender);

    void onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);

    virtual void update(float dt) override;
    void setEmitterPosition();
};

class DemoFirework : public ParticleDemo
{
public:
    CREATE_FUNC(DemoFirework);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoFire : public ParticleDemo
{
public:
    CREATE_FUNC(DemoFire);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoSun : public ParticleDemo
{
public:
    CREATE_FUNC(DemoSun);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoGalaxy : public ParticleDemo
{
public:
    CREATE_FUNC(DemoGalaxy);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoFlower : public ParticleDemo
{
public:
    CREATE_FUNC(DemoFlower);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoBigFlower : public ParticleDemo
{
public:
    CREATE_FUNC(DemoBigFlower);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoRotFlower : public ParticleDemo
{
public:
    CREATE_FUNC(DemoRotFlower);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoMeteor : public ParticleDemo
{
public:
    CREATE_FUNC(DemoMeteor);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoSpiral : public ParticleDemo
{
public:
    CREATE_FUNC(DemoSpiral);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoExplosion : public ParticleDemo
{
public:
    CREATE_FUNC(DemoExplosion);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoSmoke : public ParticleDemo
{
public:
    CREATE_FUNC(DemoSmoke);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoSnow : public ParticleDemo
{
public:
    CREATE_FUNC(DemoSnow);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoRain : public ParticleDemo
{
public:
    CREATE_FUNC(DemoRain);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoModernArt : public ParticleDemo
{
public:
    CREATE_FUNC(DemoModernArt);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoRing : public ParticleDemo
{
public:
    CREATE_FUNC(DemoRing);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class ParallaxParticle : public ParticleDemo
{
public:
    CREATE_FUNC(ParallaxParticle);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoFixedFPS : public ParticleDemo
{
public:
    CREATE_FUNC(DemoFixedFPS);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoTimeScale: public ParticleDemo
{
public:
    CREATE_FUNC(DemoTimeScale);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
    virtual void update(float dt) override;

    float elapsedTime = 0.0F;
};

class DemoSpawnFadeIn : public ParticleDemo
{
public:
    CREATE_FUNC(DemoSpawnFadeIn);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoScaleFadeIn : public ParticleDemo
{
public:
    CREATE_FUNC(DemoScaleFadeIn);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoSimulation : public ParticleDemo
{
public:
    CREATE_FUNC(DemoSimulation);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoSpawnRotation : public ParticleDemo
{
public:
    CREATE_FUNC(DemoSpawnRotation);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoHSV : public ParticleDemo
{
public:
    CREATE_FUNC(DemoHSV);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoLifeAnimation : public ParticleDemo
{
public:
    CREATE_FUNC(DemoLifeAnimation);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoLifeAnimationAtlas : public ParticleDemo
{
public:
    CREATE_FUNC(DemoLifeAnimationAtlas);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoLifeAnimationReversed : public ParticleDemo
{
public:
    CREATE_FUNC(DemoLifeAnimationReversed);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoLoopAnimation : public ParticleDemo
{
public:
    CREATE_FUNC(DemoLoopAnimation);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoLoopAnimationReversed : public ParticleDemo
{
public:
    CREATE_FUNC(DemoLoopAnimationReversed);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoEmitterAnimation : public ParticleDemo
{
public:
    CREATE_FUNC(DemoEmitterAnimation);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoEmitterAnimationDescriptor : public ParticleDemo
{
public:
    CREATE_FUNC(DemoEmitterAnimationDescriptor);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoLoopAnimationMultiDescriptor : public ParticleDemo
{
public:
    CREATE_FUNC(DemoLoopAnimationMultiDescriptor);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoEmissionShapePoint : public ParticleDemo
{
public:
    CREATE_FUNC(DemoEmissionShapePoint);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoEmissionShapeRect : public ParticleDemo
{
public:
    CREATE_FUNC(DemoEmissionShapeRect);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoEmissionShapeRectTorus : public ParticleDemo
{
public:
    CREATE_FUNC(DemoEmissionShapeRectTorus);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoEmissionShapeCircle : public ParticleDemo
{
public:
    CREATE_FUNC(DemoEmissionShapeCircle);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoEmissionShapeCircleBiasEdge : public ParticleDemo
{
public:
    CREATE_FUNC(DemoEmissionShapeCircleBiasEdge);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoEmissionShapeCircleBiasCenter : public ParticleDemo
{
public:
    CREATE_FUNC(DemoEmissionShapeCircleBiasCenter);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoEmissionShapeTorus : public ParticleDemo
{
public:
    CREATE_FUNC(DemoEmissionShapeTorus);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoEmissionShapeTorusBiasEdge : public ParticleDemo
{
public:
    CREATE_FUNC(DemoEmissionShapeTorusBiasEdge);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoEmissionShapeTorusBiasInner : public ParticleDemo
{
public:
    CREATE_FUNC(DemoEmissionShapeTorusBiasInner);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoEmissionShapeCone : public ParticleDemo
{
public:
    CREATE_FUNC(DemoEmissionShapeCone);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoEmissionShapeConeTorus : public ParticleDemo
{
public:
    CREATE_FUNC(DemoEmissionShapeConeTorus);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoEmissionShapeAlphaMask : public ParticleDemo
{
public:
    CREATE_FUNC(DemoEmissionShapeAlphaMask);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoParticleFromFile : public ParticleDemo
{
public:
    static DemoParticleFromFile* create(std::string_view file)
    {
        auto ret = new DemoParticleFromFile;
        if (ret->init())
        {
            ret->_title = file;
            ret->autorelease();
        }
        else
        {
            delete ret;
            ret = nullptr;
        }

        return ret;
    }

    std::string _title;

    virtual void onEnter() override;
    virtual std::string subtitle() const override { return _title; }
};

class RadiusMode1 : public ParticleDemo
{
public:
    CREATE_FUNC(RadiusMode1);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class RadiusMode2 : public ParticleDemo
{
public:
    CREATE_FUNC(RadiusMode2);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class Issue704 : public ParticleDemo
{
public:
    CREATE_FUNC(Issue704);
    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class Issue870 : public ParticleDemo
{
public:
    CREATE_FUNC(Issue870);
    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void updateQuads(float dt);

private:
    int _index;
};

class Issue1201 : public ParticleDemo
{
public:
    CREATE_FUNC(Issue1201);
    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class ParticleBatchHybrid : public ParticleDemo
{
public:
    CREATE_FUNC(ParticleBatchHybrid);
    virtual void onEnter() override;
    void switchRender(float dt);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

private:
    ax::Node* _parent1;
    ax::Node* _parent2;
};

class ParticleBatchMultipleEmitters : public ParticleDemo
{
public:
    CREATE_FUNC(ParticleBatchMultipleEmitters);

    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class ParticleReorder : public ParticleDemo
{
public:
    CREATE_FUNC(ParticleReorder);
    virtual void onEnter() override;
    void reorderParticles(float dt);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

private:
    unsigned int _order;
};

class MultipleParticleSystems : public ParticleDemo
{
public:
    CREATE_FUNC(MultipleParticleSystems);
    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void update(float dt) override;
};

class MultipleParticleSystemsBatched : public ParticleDemo
{
public:
    CREATE_FUNC(MultipleParticleSystemsBatched);
    virtual void onEnter() override;
    virtual void update(float dt) override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class AddAndDeleteParticleSystems : public ParticleDemo
{
public:
    CREATE_FUNC(AddAndDeleteParticleSystems);
    virtual void onEnter() override;
    virtual void update(float dt) override;
    void removeSystem(float dt);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

private:
    ax::ParticleBatchNode* _batchNode;
};

class ReorderParticleSystems : public ParticleDemo
{
public:
    CREATE_FUNC(ReorderParticleSystems);
    virtual void onEnter() override;
    void reorderSystem(float time);
    virtual void update(float dt) override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

private:
    ax::ParticleBatchNode* _batchNode;
};

class PremultipliedAlphaTest : public ParticleDemo
{
    bool _hasEmitter;

public:
    CREATE_FUNC(PremultipliedAlphaTest);
    virtual void onEnter() override;
    void readdParticle(float delta);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class PremultipliedAlphaTest2 : public ParticleDemo
{
public:
    CREATE_FUNC(PremultipliedAlphaTest2);
    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class Issue3990 : public ParticleDemo
{
public:
    CREATE_FUNC(Issue3990);
    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class ParticleVisibleTest : public ParticleDemo
{
public:
    CREATE_FUNC(ParticleVisibleTest);
    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void callback(float delta);
};

class ParticleAutoBatching : public ParticleDemo
{
public:
    CREATE_FUNC(ParticleAutoBatching);
    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class ParticleResetTotalParticles : public ParticleDemo
{
public:
    CREATE_FUNC(ParticleResetTotalParticles);
    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class ParticleIssue12310 : public ParticleDemo
{
public:
    CREATE_FUNC(ParticleIssue12310);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class DemoPause : public ParticleDemo
{
public:
    CREATE_FUNC(DemoPause);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
    void pauseEmitter(float time);
};

class ParticleSpriteFrame : public ParticleDemo
{
public:
    CREATE_FUNC(ParticleSpriteFrame);
    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

#endif
