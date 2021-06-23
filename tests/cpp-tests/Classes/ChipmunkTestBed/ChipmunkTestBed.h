/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
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

#include "cocos2d.h"
#include "chipmunk/chipmunk.h"
#include "../BaseTest.h"
#include "extensions/cocos-ext.h"

#include "demo/ChipmunkDemo.h"

extern ChipmunkDemo Example; // Use as template for new Chipmunk2D demos

extern ChipmunkDemo LogoSmash;
extern ChipmunkDemo PyramidStack;
extern ChipmunkDemo Plink;
extern ChipmunkDemo BouncyHexagons;
extern ChipmunkDemo Tumble;
extern ChipmunkDemo PyramidTopple;
extern ChipmunkDemo Planet;
extern ChipmunkDemo Springies;
extern ChipmunkDemo Pump;
extern ChipmunkDemo TheoJansen;
extern ChipmunkDemo Query;
extern ChipmunkDemo OneWay;
extern ChipmunkDemo PlatformerPlayer;
extern ChipmunkDemo Joints;
extern ChipmunkDemo Tank;
extern ChipmunkDemo Chains;
extern ChipmunkDemo Crane;
extern ChipmunkDemo Buoyancy;
extern ChipmunkDemo ContactGraph;
extern ChipmunkDemo Slice;
extern ChipmunkDemo Convex;
extern ChipmunkDemo Unicycle;
extern ChipmunkDemo Sticky;
extern ChipmunkDemo Shatter;
extern ChipmunkDemo GJK;

extern ChipmunkDemo bench_list[];
extern int bench_count;

class KeyboardNotificationLayer;

class ChipmunkTestBed : public TestCase, public cocos2d::IMEDelegate  {
public:
    CREATE_FUNC(ChipmunkTestBed);

    ChipmunkTestBed();
    ~ChipmunkTestBed();

    void onEnter() override;
    void createResetButton();
    void reset(cocos2d::Ref* sender);

    void update(float dt) override;
    virtual void initPhysics();

    //void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
    //void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);

    void onMouseDown(cocos2d::Event* event);
    void onMouseUp(cocos2d::Event* event);
    void onMouseMove(cocos2d::Event* event);
    void onMouseScroll(cocos2d::Event* event);

    cpSpace* _space; // strong ref

    cocos2d::Size orgSize; 

private: 
    cocos2d::extension::PhysicsDebugNode* _debugLayer; // weak ref
    cocos2d::EventListenerMouse* _mouseListener;
    cocos2d::Label* label;
    cocos2d::Node* _trackNode;
};

class LogoSmashDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(LogoSmashDemo);
    std::string title() const override;
    void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class PlinkDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(PlinkDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;
    void initPhysics() override;
    virtual void update(float dt) override;
};


class TumbleDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(TumbleDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class PyramidStackDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(PyramidStackDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class PyramidToppleDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(PyramidToppleDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class ChainsDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(ChainsDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class OneWayDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(OneWayDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class PlanetDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(PlanetDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class TheoJansenDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(TheoJansenDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class TankDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(TankDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class BouncyHexagonsDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(BouncyHexagonsDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class SpringiesDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(SpringiesDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class StickyDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(StickyDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class ShatterDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(ShatterDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class CraneDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(CraneDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class JointsDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(JointsDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class ConvexDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(ConvexDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class PumpDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(PumpDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};

class PlatformerPlayerDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(PlatformerPlayerDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};







class QueryDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(QueryDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};


class ContactGraphDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(ContactGraphDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};


class BuoyancyDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(BuoyancyDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};


class SliceDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(SliceDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};


class UnicycleDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(UnicycleDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};





class ExampleDemo : public ChipmunkTestBed {
public:
    CREATE_FUNC(ExampleDemo);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};




    

DEFINE_TEST_SUITE(ChipmunkTestBedTests);

#endif /* __CHIPMUNKTESTBED_H__ */
