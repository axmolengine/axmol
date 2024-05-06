/****************************************************************************
 * Copyright (c) 2021 @aismann; Peter Eismann, Germany; dreifrankensoft

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

//
// cocos2d
//
#ifndef __CHIPMUNKTESTBED_H__
#define __CHIPMUNKTESTBED_H__

#include "axmol.h"
#include "chipmunk/chipmunk.h"
#include "../BaseTest.h"
#include "extensions/axmol-ext.h"

#include "demo/ChipmunkDemo.h"

class ChipmunkTestBed : public TestCase
{
public:
    CREATE_FUNC(ChipmunkTestBed);

    ChipmunkTestBed();
    ~ChipmunkTestBed();

    void onEnter() override;
    void createResetButton();
    void reset(ax::Object* sender);

    void update(float dt) override;
    virtual void initPhysics();

    void onMouseDown(ax::Event* event);
    void onMouseUp(ax::Event* event);
    void onMouseMove(ax::Event* event);
    void DrawInfo();
    void updateInit(ChipmunkDemo tt);

    cpSpace* _space;                                              // strong ref
    ax::extension::PhysicsDebugNodeChipmunk2D* _debugLayer;  // weak ref
    int keyPresses;
    bool mousePresses = false;

private:
    ax::EventListenerMouse* _mouseListener;
    ax::Node* _trackNode;
    ax::DrawNode* draw;
    ax::Label* drawInfo;
};

class LogoSmashDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(LogoSmashDemo);
    std::string title() const override;
    void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class PlinkDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(PlinkDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;
    void initPhysics() override;
    virtual void update(float dt) override;
};

class TumbleDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(TumbleDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class PyramidStackDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(PyramidStackDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class PyramidToppleDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(PyramidToppleDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class ChainsDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(ChainsDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class OneWayDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(OneWayDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class PlanetDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(PlanetDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class TheoJansenDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(TheoJansenDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class TankDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(TankDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class BenchDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(BenchDemo);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class SpringiesDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(SpringiesDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class StickyDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(StickyDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class ShatterDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(ShatterDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class CraneDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(CraneDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class JointsDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(JointsDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class ConvexDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(ConvexDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class PumpDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(PumpDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class PlatformerPlayerDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(PlatformerPlayerDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class QueryDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(QueryDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class ContactGraphDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(ContactGraphDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class BuoyancyDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(BuoyancyDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class SliceDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(SliceDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class UnicycleDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(UnicycleDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class ExampleDemo : public ChipmunkTestBed
{
public:
    CREATE_FUNC(ExampleDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};
DEFINE_TEST_SUITE(ChipmunkTestBedTests);

#endif /* __CHIPMUNKTESTBED_H__ */
