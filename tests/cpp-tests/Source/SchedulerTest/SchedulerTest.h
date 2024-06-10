/****************************************************************************
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

#ifndef _SCHEDULER_TEST_H_
#define _SCHEDULER_TEST_H_

#include "axmol.h"
#include "extensions/axmol-ext.h"
#include "../BaseTest.h"

DEFINE_TEST_SUITE(SchedulerTests);

class SchedulerTestLayer : public TestCase
{
public:
};

class SchedulerAutoremove : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerAutoremove);

    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void autoremove(float dt);
    void tick(float dt);

private:
    float accum;
};

class SchedulerPauseResume : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerPauseResume);

    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void tick1(float dt);
    void tick2(float dt);
    using SchedulerTestLayer::pause;
    void pause(float dt);
};

class SchedulerPauseResumeAll : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerPauseResumeAll);

    SchedulerPauseResumeAll();
    virtual ~SchedulerPauseResumeAll();
    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void update(float delta) override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void tick1(float dt);
    void tick2(float dt);
    using SchedulerTestLayer::pause;
    void pause(float dt);
    using SchedulerTestLayer::resume;
    void resume(float dt);

private:
    std::set<void*> _pausedTargets;
};

class SchedulerPauseResumeAllUser : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerPauseResumeAllUser);

    SchedulerPauseResumeAllUser();
    virtual ~SchedulerPauseResumeAllUser();
    virtual void onEnter() override;
    virtual void onExit() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void tick1(float dt);
    void tick2(float dt);
    using SchedulerTestLayer::pause;
    void pause(float dt);
    using SchedulerTestLayer::resume;
    void resume(float dt);

private:
    std::set<void*> _pausedTargets;
};

class SchedulerUnscheduleAll : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerUnscheduleAll);

    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void tick1(float dt);
    void tick2(float dt);
    void tick3(float dt);
    void tick4(float dt);
    void unscheduleAll(float dt);
};

class SchedulerUnscheduleAllHard : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerUnscheduleAllHard);

    virtual void onEnter() override;
    virtual void onExit() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void tick1(float dt);
    void tick2(float dt);
    void tick3(float dt);
    void tick4(float dt);
    void unscheduleAll(float dt);

private:
    bool _actionManagerActive;
};

class SchedulerUnscheduleAllUserLevel : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerUnscheduleAllUserLevel);

    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void tick1(float dt);
    void tick2(float dt);
    void tick3(float dt);
    void tick4(float dt);
    void unscheduleAll(float dt);
};

class SchedulerSchedulesAndRemove : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerSchedulesAndRemove);

    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void tick1(float dt);
    void tick2(float dt);
    void tick3(float dt);
    void tick4(float dt);
    void scheduleAndUnschedule(float dt);
};

class SchedulerUpdate : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerUpdate);

    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void removeUpdates(float dt);
};

class SchedulerUpdateAndCustom : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerUpdateAndCustom);

    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void update(float dt) override;
    void tick(float dt);
    void stopSelectors(float dt);
};

class SchedulerUpdateFromCustom : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerUpdateFromCustom);

    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void update(float dt) override;
    void schedUpdate(float dt);
    void stopUpdate(float dt);
};

class TestNode : public ax::Node
{
public:
    CREATE_FUNC(TestNode);

    ~TestNode();

    void initWithString(std::string_view str, int priority);
    virtual void update(float dt) override;

private:
    std::string _string;
};

class RescheduleSelector : public SchedulerTestLayer
{
public:
    CREATE_FUNC(RescheduleSelector);

    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void schedUpdate(float dt);

private:
    float _interval;
    int _ticks;
};

class SchedulerDelayAndRepeat : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerDelayAndRepeat);

    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void update(float dt) override;
};

class SchedulerTimeScale : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerTimeScale);

    void onEnter() override;
    void onExit() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    ax::extension::ControlSlider* sliderCtl();
    void sliderAction(ax::Object* sender, ax::extension::Control::EventType controlEvent);
    ax::extension::ControlSlider* _sliderCtl;
};

class TwoSchedulers : public SchedulerTestLayer
{
public:
    CREATE_FUNC(TwoSchedulers);

    virtual ~TwoSchedulers();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void onEnter() override;
    ax::extension::ControlSlider* sliderCtl();
    void sliderAction(ax::Object* sender, ax::extension::Control::EventType controlEvent);
    ax::Scheduler* sched1;
    ax::Scheduler* sched2;
    ax::ActionManager* actionManager1;
    ax::ActionManager* actionManager2;

    ax::extension::ControlSlider* sliderCtl1;
    ax::extension::ControlSlider* sliderCtl2;
};

class SchedulerIssue2268 : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerIssue2268);

    ~SchedulerIssue2268();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void onEnter() override;
    void update(float dt) override;

private:
    ax::Node* testNode;
};

class SchedulerIssueWithReschedule : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerIssueWithReschedule);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void onEnter() override;
};

class ScheduleCallbackTest : public SchedulerTestLayer
{
public:
    CREATE_FUNC(ScheduleCallbackTest);

    ~ScheduleCallbackTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void onEnter() override;

    void callback(float dt);

private:
};

class ScheduleUpdatePriority : public SchedulerTestLayer
{
public:
    CREATE_FUNC(ScheduleUpdatePriority);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void onEnter() override;
    void onExit() override;

    virtual void update(float dt) override;

    bool onTouchBegan(ax::Touch* touch, ax::Event* event);
};

class SchedulerIssue10232 : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerIssue10232);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void onEnter() override;
    void update(float dt) override;
};

class SchedulerRemoveAllFunctionsToBePerformedInCocosThread : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerRemoveAllFunctionsToBePerformedInCocosThread);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void onEnter() override;
    void onExit() override;
    void update(float dt) override;

private:
    ax::Sprite* _sprite;
};

class SchedulerIssue17149 : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerIssue17149);
    SchedulerIssue17149();
    ~SchedulerIssue17149();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void onEnter() override;
    virtual void update(float dt) override;

private:
    class ClassA
    {
    public:
        ClassA();

        void update(float dt);

        int _member1;
        int _member2;
        int _member3;
    };

    class ClassB
    {
    public:
        ClassB();
        void update(float dt);

        int _member1;
        int _member2;
        int _member3;
    };

    void* _memoryPool;
};

class SchedulerRemoveEntryWhileUpdate : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerRemoveEntryWhileUpdate);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void onEnter() override;
    virtual void onExit() override;

private:
    class TestClass
    {
    public:
        TestClass(int index, TestClass* nextObj, ax::Scheduler* scheduler);
        void update(float dt);

    private:
        TestClass* _nextObj            = nullptr;
        int _index                     = 0;
        ax::Scheduler* _scheduler = nullptr;
        bool _cleanedUp                = false;
    };
    std::vector<TestClass*> _testvector;
};

class SchedulerRemoveSelectorDuringCall : public SchedulerTestLayer
{
public:
    CREATE_FUNC(SchedulerRemoveSelectorDuringCall);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void onEnter() override;
    virtual void onExit() override;

private:
    void callback(float);

private:
    bool _scheduled;
};

#endif
