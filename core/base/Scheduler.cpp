/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#include "base/Scheduler.h"
#include "base/Macros.h"
#include "base/Director.h"
#include "base/ScriptSupport.h"

NS_AX_BEGIN

// implementation Timer

Timer::Timer()
    : _scheduler(nullptr)
    , _elapsed(-1)
    , _runForever(false)
    , _useDelay(false)
    , _timesExecuted(0)
    , _repeat(0)
    , _delay(0.0f)
    , _interval(0.0f)
    , _aborted(false)
{}

void Timer::setupTimerWithInterval(float seconds, unsigned int repeat, float delay)
{
    _elapsed       = -1;
    _interval      = seconds;
    _delay         = delay;
    _useDelay      = (_delay > 0.0f) ? true : false;
    _repeat        = repeat;
    _runForever    = (_repeat == AX_REPEAT_FOREVER) ? true : false;
    _timesExecuted = 0;
}

void Timer::update(float dt)
{
    if (_elapsed == -1)
    {
        _elapsed       = 0;
        _timesExecuted = 0;
        return;
    }

    // accumulate elapsed time
    _elapsed += dt;

    // deal with delay
    if (_useDelay)
    {
        if (_elapsed < _delay)
        {
            return;
        }
        _timesExecuted += 1;  // important to increment before call trigger
        trigger(_delay);
        _elapsed  = _elapsed - _delay;
        _useDelay = false;
        // after delay, the rest time should compare with interval
        if (isExhausted())
        {  // unschedule timer
            cancel();
            return;
        }
    }

    // if _interval == 0, should trigger once every frame
    float interval = (_interval > 0) ? _interval : _elapsed;
    while ((_elapsed >= interval) && !_aborted)
    {
        _timesExecuted += 1;  // important to increment before call trigger
        trigger(interval);
        _elapsed -= interval;

        if (isExhausted())
        {
            cancel();
            break;
        }

        if (_elapsed <= 0.f)
        {
            break;
        }
    }
}

bool Timer::isExhausted() const
{
    return !_runForever && _timesExecuted > _repeat;
}

// TimerTargetSelector

TimerTargetSelector::TimerTargetSelector() : _target(nullptr), _selector(nullptr) {}

bool TimerTargetSelector::initWithSelector(Scheduler* scheduler,
                                           SEL_SCHEDULE selector,
                                           Object* target,
                                           float seconds,
                                           unsigned int repeat,
                                           float delay)
{
    _scheduler = scheduler;
    _target    = target;
    _selector  = selector;
    setupTimerWithInterval(seconds, repeat, delay);
    return true;
}

void TimerTargetSelector::trigger(float dt)
{
    if (_target && _selector)
    {
        (_target->*_selector)(dt);
    }
}

void TimerTargetSelector::cancel()
{
    _scheduler->unschedule(_selector, _target);
}

// TimerTargetCallback

TimerTargetCallback::TimerTargetCallback() : _target(nullptr), _callback(nullptr) {}

bool TimerTargetCallback::initWithCallback(Scheduler* scheduler,
                                           const ccSchedulerFunc& callback,
                                           void* target,
                                           std::string_view key,
                                           float seconds,
                                           unsigned int repeat,
                                           float delay)
{
    _scheduler = scheduler;
    _target    = target;
    _callback  = callback;
    _key       = key;
    setupTimerWithInterval(seconds, repeat, delay);
    return true;
}

void TimerTargetCallback::trigger(float dt)
{
    if (_callback)
    {
        _callback(dt);
    }
}

void TimerTargetCallback::cancel()
{
    _scheduler->unschedule(_key, _target);
}

#if AX_ENABLE_SCRIPT_BINDING

// TimerScriptHandler

bool TimerScriptHandler::initWithScriptHandler(int handler, float seconds)
{
    _scriptHandler = handler;
    _elapsed       = -1;
    _interval      = seconds;

    return true;
}

void TimerScriptHandler::trigger(float dt)
{
    if (0 != _scriptHandler)
    {
        SchedulerScriptData data(_scriptHandler, dt);
        ScriptEvent event(kScheduleEvent, &data);
        ScriptEngineManager::sendEventToLua(event);
    }
}

void TimerScriptHandler::cancel() {}

#endif

// implementation of Scheduler

// Priority level reserved for system services.
const int Scheduler::PRIORITY_SYSTEM = INT_MIN;

// Minimum priority level for user scheduling.
const int Scheduler::PRIORITY_NON_SYSTEM_MIN = PRIORITY_SYSTEM + 1;

Scheduler::Scheduler()
    : _timeScale(1.0f)
    , _currentTarget(nullptr)
    , _currentTargetSalvaged(false)
    , _indexMapLocked(false)
#if AX_ENABLE_SCRIPT_BINDING
    , _scriptHandlerEntries(20)
#endif
{
    // I don't expect to have more than 30 functions to all per frame
    _actionsToPerform.reserve(30);
}

Scheduler::~Scheduler()
{
    unscheduleAll();
}

void Scheduler::schedule(const ccSchedulerFunc& callback,
                         void* target,
                         float interval,
                         bool paused,
                         std::string_view key)
{
    this->schedule(callback, target, interval, AX_REPEAT_FOREVER, 0.0f, paused, key);
}

void Scheduler::schedule(const ccSchedulerFunc& callback,
                         void* target,
                         float interval,
                         unsigned int repeat,
                         float delay,
                         bool paused,
                         std::string_view key)
{
    AXASSERT(target, "Argument target must be non-nullptr");
    AXASSERT(!key.empty(), "key should not be empty!");

    auto timerIt = _timersMap.find(target);
    if (timerIt == _timersMap.end())
    {
        timerIt = _timersMap.emplace(target, TimerHandle{}).first;

        // Is this the 1st element ? Then set the pause level to all the selectors of this target
        timerIt->second.paused = paused;
    }
    else
    {
        AXASSERT(timerIt->second.paused == paused, "element's paused should be paused!");
    }

    auto& timers = timerIt->second.timers;
    if (timers.empty())
    {
        timers.reserve(10);
    }
    else
    {
        auto timerIt = std::find_if(timers.begin(), timers.end(), [&key](Timer* const itimer) {
            TimerTargetCallback* timer = dynamic_cast<TimerTargetCallback*>(itimer);
            return timer && !timer->isExhausted() && key == timer->getKey();
        });
        if (timerIt != timers.end())
        {
            AXLOG("Scheduler#schedule. Reiniting timer with interval %.4f, repeat %u, delay %.4f", interval, repeat,
                  delay);
            (*timerIt)->setupTimerWithInterval(interval, repeat, delay);
            return;
        }
    }

    TimerTargetCallback* timer = new TimerTargetCallback();
    timer->initWithCallback(this, callback, target, key, interval, repeat, delay);
    timers.pushBack(timer);
    timer->release();
}

void Scheduler::unschedule(std::string_view key, void* target)
{
    // explicit handle nil arguments when removing an object
    if (target == nullptr || key.empty())
    {
        return;
    }

    auto timerIt = _timersMap.find(target);
    if (timerIt != _timersMap.end())
    {
        auto& timerHandle = timerIt->second;
        for (int i = 0; i < timerHandle.timers.size(); ++i)
        {
            TimerTargetCallback* timer = dynamic_cast<TimerTargetCallback*>(timerHandle.timers[i]);

            if (timer && key == timer->getKey())
            {
                if (timer == timerHandle.currentTimer && (!timer->isAborted()))
                {
                    timer->retain();
                    timer->setAborted();
                }

                timerHandle.timers.erase(i);

                // update timerIndex in case we are in tick:, looping over the actions
                if (timerHandle.timerIndex >= i)
                {
                    timerHandle.timerIndex--;
                }

                if (timerHandle.timers.empty())
                {
                    if (_currentTarget == &timerHandle)
                    {
                        _currentTargetSalvaged = true;
                    }
                    else
                    {
                        _timersMap.erase(timerIt);
                    }
                }

                return;
            }
        }
    }
}

void Scheduler::priorityIn(axstd::pod_vector<SchedHandle*>& list,
                           const ccSchedulerFunc& callback,
                           void* target,
                           int priority,
                           bool paused)
{
    auto sched = new SchedHandle(&list, callback, target, priority, paused);
    axstd::insert_sorted(list, sched,
                         [](const SchedHandle* lhs, const SchedHandle* rhs) { return lhs->priority < rhs->priority; });

    _schedIndexMap.emplace(target, sched);
}

void Scheduler::appendIn(axstd::pod_vector<SchedHandle*>& list,
                         const ccSchedulerFunc& callback,
                         void* target,
                         bool paused)
{
    auto sched = new SchedHandle(&list, callback, target, 0, paused);
    list.emplace_back(sched);
    _schedIndexMap.emplace(target, sched);
}

void Scheduler::addToWaitList(const ccSchedulerFunc& callback, void* target, int priority, bool paused)
{
    auto sched = new SchedHandle(&_waitList, callback, target, priority, paused);
    _waitList.emplace_back(sched);
    _schedIndexMap.emplace(target, sched);
}

void Scheduler::activeWaitList()
{
    for (auto sched : _waitList)
    {
        if (sched->priority == 0)
        {
            sched->owner = &_updates0List;
            _updates0List.emplace_back(sched);
        }
        else if (sched->priority > 0)
        {
            sched->owner = &_updatesPosList;
            axstd::insert_sorted(_updatesPosList, sched, [](const SchedHandle* lhs, const SchedHandle* rhs) {
                return lhs->priority < rhs->priority;
            });
        }
        else
        {
            sched->owner = &_updatesNegList;
            axstd::insert_sorted(_updatesNegList, sched, [](const SchedHandle* lhs, const SchedHandle* rhs) {
                return lhs->priority < rhs->priority;
            });
        }
    }
    _waitList.clear();
}

void Scheduler::schedulePerFrame(const ccSchedulerFunc& callback, void* target, int priority, bool paused)
{
    auto updateIt = _schedIndexMap.find(target);
    if (updateIt != _schedIndexMap.end())
    {
        // change priority: should unschedule it first
        if (updateIt->second->priority != priority)
        {
            unscheduleUpdate(target);
        }
        else
        {
            // don't add it again
            return;
        }
    }

    if (!_indexMapLocked)
    {
        // most of the updates are going to be 0, that's way there
        // is an special list for updates with priority 0
        if (priority == 0)
        {
            appendIn(_updates0List, callback, target, paused);
        }
        else if (priority < 0)
        {
            priorityIn(_updatesNegList, callback, target, priority, paused);
        }
        else
        {
            // priority > 0
            priorityIn(_updatesPosList, callback, target, priority, paused);
        }
    }
    else
    {
        addToWaitList(callback, target, priority, paused);
    }
}

bool Scheduler::isScheduled(std::string_view key, const void* target) const
{
    AXASSERT(!key.empty(), "Argument key must not be empty");
    AXASSERT(target, "Argument target must be non-nullptr");

    auto timerIt = _timersMap.find(const_cast<void*>(target));

    if (timerIt == _timersMap.end())
        return false;

    auto&& timers    = timerIt->second.timers;
    const bool found = !timers.empty() && std::find_if(timers.begin(), timers.end(), [&key](Timer* const itimer) {
                                              auto timer = dynamic_cast<TimerTargetCallback*>(itimer);
                                              return (timer && !timer->isExhausted() && key == timer->getKey());
                                          }) != timers.end();

    return found;
}

void Scheduler::unscheduleUpdate(void* target)
{
    if (target == nullptr)
    {
        return;
    }

    auto updateIt = _schedIndexMap.find(target);
    if (updateIt != _schedIndexMap.end())
    {
        auto& sched = updateIt->second;

        if (!_indexMapLocked)
        {
            axstd::erase(*sched->owner, sched);
            delete sched;
        }
        else
        {
            sched->markedForDeletion = true;
            _updateDeleteVector.emplace_back(sched);
        }

        _schedIndexMap.erase(updateIt);
    }
}

void Scheduler::unscheduleAll()
{
    unscheduleAllWithMinPriority(PRIORITY_SYSTEM);
}

void Scheduler::unscheduleAllWithMinPriority(int minPriority)
{
    // Custom Selectors
    for (auto timerIt = _timersMap.begin(); timerIt != _timersMap.end();)
    {
        unscheduleAllForTarget(timerIt);
    }

    for (auto&& entry : _waitList)
    {
        unscheduleUpdate(entry->target);
    }

    // Updates selectors
    if (minPriority < 0)
    {
        for (auto&& entry : _updatesNegList)
        {
            if (entry->priority >= minPriority)
            {
                unscheduleUpdate(entry->target);
            }
        }
    }

    if (minPriority <= 0)
    {
        for (auto&& entry : _updates0List)
        {
            unscheduleUpdate(entry->target);
        }
    }

    for (auto&& entry : _updatesPosList)
    {
        if (entry->priority >= minPriority)
        {
            unscheduleUpdate(entry->target);
        }
    }
#if AX_ENABLE_SCRIPT_BINDING
    _scriptHandlerEntries.clear();
#endif
}

void Scheduler::unscheduleAllForTarget(void* target)
{
    // explicit nullptr handling
    if (target == nullptr)
    {
        return;
    }

    // Custom Selectors
    auto timerIt = _timersMap.find(target);
    if (timerIt != _timersMap.end())
        unscheduleAllForTarget(timerIt);
    else
        unscheduleUpdate(target);
}

void Scheduler::unscheduleAllForTarget(std::unordered_map<void*, TimerHandle>::iterator& timerIt)
{
    auto const target = timerIt->first;
    auto& timerHandle = timerIt->second;
    if (timerHandle.timers.contains(timerHandle.currentTimer) && (!timerHandle.currentTimer->isAborted()))
    {
        timerHandle.currentTimer->retain();
        timerHandle.currentTimer->setAborted();
    }
    timerHandle.timers.clear();

    if (_currentTarget == &timerHandle)
    {
        _currentTargetSalvaged = true;
        ++timerIt;
    }
    else
    {
        timerIt = _timersMap.erase(timerIt);
    }

    unscheduleUpdate(target);
}

#if AX_ENABLE_SCRIPT_BINDING
unsigned int Scheduler::scheduleScriptFunc(unsigned int handler, float interval, bool paused)
{
    SchedulerScriptHandlerEntry* entry = SchedulerScriptHandlerEntry::create(handler, interval, paused);
    _scriptHandlerEntries.pushBack(entry);
    return entry->getEntryId();
}

void Scheduler::unscheduleScriptEntry(unsigned int scheduleScriptEntryID)
{
    for (ssize_t i = _scriptHandlerEntries.size() - 1; i >= 0; i--)
    {
        SchedulerScriptHandlerEntry* entry = _scriptHandlerEntries.at(i);
        if (entry->getEntryId() == (int)scheduleScriptEntryID)
        {
            entry->markedForDeletion();
            break;
        }
    }
}

#endif

void Scheduler::resumeTarget(void* target)
{
    AXASSERT(target != nullptr, "target can't be nullptr!");

    // custom selectors
    auto timerIt = _timersMap.find(target);
    if (timerIt != _timersMap.end())
    {
        timerIt->second.paused = false;
    }

    // update selector
    auto updateIt = _schedIndexMap.find(target);
    if (updateIt != _schedIndexMap.end())
    {
        updateIt->second->paused = false;
    }
}

void Scheduler::pauseTarget(void* target)
{
    AXASSERT(target != nullptr, "target can't be nullptr!");

    // custom selectors
    auto timerIt = _timersMap.find(target);
    if (timerIt != _timersMap.end())
    {
        timerIt->second.paused = true;
    }

    // update selector
    auto updateIt = _schedIndexMap.find(target);
    if (updateIt != _schedIndexMap.end())
    {
        updateIt->second->paused = true;
    }
}

bool Scheduler::isTargetPaused(void* target)
{
    AXASSERT(target != nullptr, "target must be non nil");

    // Custom selectors
    auto timerIt = _timersMap.find(target);
    if (timerIt != _timersMap.end())
    {
        return timerIt->second.paused;
    }

    // We should check update selectors if target does not have custom selectors
    auto updateIt = _schedIndexMap.find(target);
    if (updateIt != _schedIndexMap.end())
    {
        return updateIt->second->paused;
    }

    return false;  // should never get here
}

std::set<void*> Scheduler::pauseAllTargets()
{
    return pauseAllTargetsWithMinPriority(PRIORITY_SYSTEM);
}

std::set<void*> Scheduler::pauseAllTargetsWithMinPriority(int minPriority)
{
    std::set<void*> idsWithSelectors;

    // Custom Selectors
    for (auto& [target, timerHandle] : _timersMap)
    {
        timerHandle.paused = true;
        idsWithSelectors.insert(target);
    }

    // Updates selectors
    if (minPriority < 0)
    {
        for (auto&& entry : _updatesNegList)
        {
            if (entry->priority >= minPriority)
            {
                entry->paused = true;
                idsWithSelectors.insert(entry->target);
            }
        }
    }

    if (minPriority <= 0)
    {
        for (auto&& entry : _updates0List)
        {
            entry->paused = true;
            idsWithSelectors.insert(entry->target);
        }
    }

    for (auto&& entry : _updatesPosList)
    {
        if (entry->priority >= minPriority)
        {
            entry->paused = true;
            idsWithSelectors.insert(entry->target);
        }
    }

    return idsWithSelectors;
}

void Scheduler::resumeTargets(const std::set<void*>& targetsToResume)
{
    for (const auto& obj : targetsToResume)
    {
        this->resumeTarget(obj);
    }
}

void Scheduler::runOnAxmolThread(std::function<void()> action)
{
    std::lock_guard<std::mutex> lock(_performMutex);
    _actionsToPerform.emplace_back(std::move(action));
}

void Scheduler::removeAllPendingActions()
{
    std::unique_lock<std::mutex> lock(_performMutex);
    _actionsToPerform.clear();
}

// main loop
void Scheduler::update(float dt)
{
    // active waitlist
    if (!_waitList.empty())
        activeWaitList();

    _indexMapLocked = true;

    if (_timeScale != 1.0f)
    {
        dt *= _timeScale;
    }

    //
    // Selector callbacks
    //

    // Iterate over all the Updates' selectors
    // updates with priority < 0
    for (auto&& entry : _updatesNegList)
    {
        if ((!entry->paused) && (!entry->markedForDeletion))
        {
            entry->callback(dt);
        }
    }

    // updates with priority == 0
    for (auto&& entry : _updates0List)
    {
        if ((!entry->paused) && (!entry->markedForDeletion))
        {
            entry->callback(dt);
        }
    }

    // updates with priority > 0
    for (auto&& entry : _updatesPosList)
    {
        if ((!entry->paused) && (!entry->markedForDeletion))
        {
            entry->callback(dt);
        }
    }

    // Iterate over all the custom selectors
    for (auto it = _timersMap.begin(); it != _timersMap.end();)
    {
        auto elt               = &it->second;
        _currentTarget         = elt;
        _currentTargetSalvaged = false;

        if (!_currentTarget->paused)
        {
            // The 'timers' array may change while inside this loop
            for (elt->timerIndex = 0; elt->timerIndex < elt->timers.size(); ++(elt->timerIndex))
            {
                elt->currentTimer = elt->timers[elt->timerIndex];
                AXASSERT(!elt->currentTimer->isAborted(), "An aborted timer should not be updated");

                elt->currentTimer->update(dt);

                if (elt->currentTimer->isAborted())
                {
                    // The currentTimer told the remove itself. To prevent the timer from
                    // accidentally deallocating itself before finishing its step, we retained
                    // it. Now that step is done, it's safe to release it.
                    elt->currentTimer->release();
                }

                elt->currentTimer = nullptr;
            }
        }

        // only delete currentTarget if no actions were scheduled during the cycle (issue #481)
        if (_currentTargetSalvaged && _currentTarget->timers.empty())
        {
            it = _timersMap.erase(it);
        }
        else
            ++it;
    }

    // delete all updates that are removed in update
    for (auto&& sched : _updateDeleteVector)
    {
        axstd::erase(*sched->owner, sched);
        delete sched;
    }

    _updateDeleteVector.clear();

    _indexMapLocked = false;
    _currentTarget  = nullptr;

#if AX_ENABLE_SCRIPT_BINDING
    //
    // Script callbacks
    //

    // Iterate over all the script callbacks
    if (!_scriptHandlerEntries.empty())
    {
        for (auto i = _scriptHandlerEntries.size() - 1; i >= 0; i--)
        {
            SchedulerScriptHandlerEntry* eachEntry = _scriptHandlerEntries.at(i);
            if (eachEntry->isMarkedForDeletion())
            {
                _scriptHandlerEntries.erase(i);
            }
            else if (!eachEntry->isPaused())
            {
                eachEntry->getTimer()->update(dt);
            }
        }
    }
#endif
    //
    // Functions allocated from another thread
    //

    // Testing size is faster than locking / unlocking.
    // And almost never there will be functions scheduled to be called.
    if (!_actionsToPerform.empty())
    {
        _performMutex.lock();
        // fixed #4123: Save the callback functions, they must be invoked after '_performMutex.unlock()', otherwise if
        // new functions are added in callback, it will cause thread deadlock.
        auto temp = std::move(_actionsToPerform);
        _performMutex.unlock();

        for (const auto& function : temp)
        {
            function();
        }
    }
}

void Scheduler::schedule(SEL_SCHEDULE selector,
                         Object* target,
                         float interval,
                         unsigned int repeat,
                         float delay,
                         bool paused)
{
    AXASSERT(target, "Argument target must be non-nullptr");

    auto timerIt = _timersMap.find(target);

    if (timerIt == _timersMap.end())
    {
        timerIt = _timersMap.emplace(target, TimerHandle{}).first;

        // Is this the 1st element ? Then set the pause level to all the selectors of this target
        timerIt->second.paused = paused;
    }
    else
    {
        AXASSERT(timerIt->second.paused == paused, "element's paused should be paused.");
    }

    auto&& timers = timerIt->second.timers;
    if (timers.empty())
    {
        timers.reserve(10);
    }
    else
    {
        auto timerIt = std::find_if(timers.begin(), timers.end(), [selector](Timer* const itimer) {
            TimerTargetSelector* timer = dynamic_cast<TimerTargetSelector*>(itimer);
            return timer && !timer->isExhausted() && selector == timer->getSelector();
        });
        if (timerIt != timers.end())
        {
            AXLOG("Scheduler#schedule. Reiniting timer with interval %.4f, repeat %u, delay %.4f", interval, repeat,
                  delay);
            (*timerIt)->setupTimerWithInterval(interval, repeat, delay);
            return;
        }
    }

    TimerTargetSelector* timer = new TimerTargetSelector();
    timer->initWithSelector(this, selector, target, interval, repeat, delay);
    timers.pushBack(timer);
    timer->release();
}

void Scheduler::schedule(SEL_SCHEDULE selector, Object* target, float interval, bool paused)
{
    this->schedule(selector, target, interval, AX_REPEAT_FOREVER, 0.0f, paused);
}

bool Scheduler::isScheduled(SEL_SCHEDULE selector, const Object* target) const
{
    AXASSERT(selector, "Argument selector must be non-nullptr");
    AXASSERT(target, "Argument target must be non-nullptr");

    auto timerIt = _timersMap.find(const_cast<Object*>(target));
    if (timerIt == _timersMap.end())
    {
        return false;
    }

    auto&& timers = timerIt->second.timers;
    const auto found =
        !timers.empty() && std::find_if(timers.begin(), timers.end(), [selector](Timer* const itimer) {
                               auto timer = dynamic_cast<TimerTargetSelector*>(itimer);
                               return (timer && !timer->isExhausted() && selector == timer->getSelector());
                           }) != timers.end();

    return found;
}

void Scheduler::unschedule(SEL_SCHEDULE selector, Object* target)
{
    // explicit handle nil arguments when removing an object
    if (target == nullptr || selector == nullptr)
    {
        return;
    }

    auto timerIt = _timersMap.find(target);
    if (timerIt != _timersMap.end())
    {
        auto& timerHandle = timerIt->second;
        auto&& timers     = timerHandle.timers;
        for (int i = 0; i < timers.size(); ++i)
        {
            TimerTargetSelector* timer = dynamic_cast<TimerTargetSelector*>(timers[i]);

            if (timer && selector == timer->getSelector())
            {
                if (timer == timerHandle.currentTimer && !timer->isAborted())
                {
                    timer->retain();
                    timer->setAborted();
                }

                timers.erase(i);

                // update timerIndex in case we are in tick:, looping over the actions
                if (timerHandle.timerIndex >= i)
                {
                    timerHandle.timerIndex--;
                }

                if (timers.empty())
                {
                    if (_currentTarget == &timerHandle)
                    {
                        _currentTargetSalvaged = true;
                    }
                    else
                    {
                        _timersMap.erase(timerIt);
                    }
                }

                return;
            }
        }
    }
}

NS_AX_END
