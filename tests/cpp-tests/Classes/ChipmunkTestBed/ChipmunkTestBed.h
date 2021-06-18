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
//#include "extensions/physics-nodes/Header.h";

#include "demo/ChipmunkDemo.h"


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
//extern ChipmunkDemo PlayeR;
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


class ChipmunkTestBed : public TestCase
{
public:
    CREATE_FUNC(ChipmunkTestBed);

    ChipmunkTestBed();
    ~ChipmunkTestBed();
    void onEnter() override;
    void createResetButton();
    void reset(cocos2d::Ref* sender);

    //void addNewSpriteAtPosition(cocos2d::Vec2 p);
    void update(float dt) override;
    virtual void initPhysics();
    //void toggleDebugCallback(cocos2d::Ref* sender);
    //void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    //virtual void onAcceleration(cocos2d::Acceleration* acc, cocos2d::Event* event);

    cpSpace* _space; // strong ref

private: 
    cocos2d::extension::PhysicsDebugNode* _debugLayer; // weak ref
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



class AddExample : public ChipmunkTestBed {
public:
    CREATE_FUNC(AddExample);
    virtual std::string title() const override;
    virtual void onEnter() override;

    void initPhysics() override;
    virtual void update(float dt) override;
};


DEFINE_TEST_SUITE(ChipmunkTestBedTests);

#endif /* __CHIPMUNKTESTBED_H__ */
