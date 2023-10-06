/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
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
                                           Ref* target,
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
    , _updateHashLocked(false)
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

    /* tHashTimerEntry* element = nullptr;
     HASH_FIND_PTR(_hashForTimers, &target, element);*/
    tHashTimerEntry* element = nullptr;
    auto timerIt             = _hashForTimers.find(target);
    if (timerIt == _hashForTimers.end())
    {
        element = &_hashForTimers.emplace(target, _hashSelectorEntry{}).first->second;

        // Is this the 1st element ? Then set the pause level to all the selectors of this target
        element->paused = paused;
    }
    else
    {
        element = &timerIt->second;
        AXASSERT(timerIt->second.paused == paused, "element's paused should be paused!");
    }

    if (element->timers == nullptr)
    {
        element->timers = new Vector<Timer*>(10);
    }
    else
    {
        for (int i = 0; i < element->timers->size(); ++i)
        {
            TimerTargetCallback* timer = dynamic_cast<TimerTargetCallback*>(element->timers->at(i));

            if (timer && !timer->isExhausted() && key == timer->getKey())
            {
                AXLOG("CCScheduler#schedule. Reiniting timer with interval %.4f, repeat %u, delay %.4f", interval,
                      repeat, delay);
                timer->setupTimerWithInterval(interval, repeat, delay);
                return;
            }
        }
    }

    TimerTargetCallback* timer = new TimerTargetCallback();
    timer->initWithCallback(this, callback, target, key, interval, repeat, delay);
    element->timers->pushBack(timer);
    timer->release();
}

void Scheduler::unschedule(std::string_view key, void* target)
{
    // explicit handle nil arguments when removing an object
    if (target == nullptr || key.empty())
    {
        return;
    }

    // AXASSERT(target);
    // AXASSERT(selector);

    auto timerIt = _hashForTimers.find(target);
    if (timerIt != _hashForTimers.end())
    {
        auto& element = timerIt->second;
        for (int i = 0; i < element.timers->size(); ++i)
        {
            TimerTargetCallback* timer = dynamic_cast<TimerTargetCallback*>(element.timers->at(i));

            if (timer && key == timer->getKey())
            {
                if (timer == element.currentTimer && (!timer->isAborted()))
                {
                    timer->retain();
                    timer->setAborted();
                }

                element.timers->erase(i);

                // update timerIndex in case we are in tick:, looping over the actions
                if (element.timerIndex >= i)
                {
                    element.timerIndex--;
                }

                if (element.timers->size() == 0)
                {
                    if (_currentTarget == &element)
                    {
                        _currentTargetSalvaged = true;
                    }
                    else
                    {
                        delete element.timers;
                        _hashForTimers.erase(timerIt);
                    }
                }

                return;
            }
        }
    }
}

void Scheduler::priorityIn(axstd::pod_vector<tListEntry*>& list,
                           const ccSchedulerFunc& callback,
                           void* target,
                           int priority,
                           bool paused)
{
    tListEntry* listElement = new tListEntry();

    listElement->callback          = callback;
    listElement->target            = target;
    listElement->priority          = priority;
    listElement->paused            = paused;
    listElement->markedForDeletion = false;
    axstd::insert_sorted(list, listElement,
                         [](const tListEntry* lhs, const tListEntry* rhs) { return lhs->priority < rhs->priority; });

    auto& hashElement = _hashForUpdates.emplace(target, tHashUpdateEntry{}).first->second;
    hashElement.list  = &list;
    hashElement.entry = listElement;
}

void Scheduler::appendIn(axstd::pod_vector<tListEntry*>& list,
                         const ccSchedulerFunc& callback,
                         void* target,
                         bool paused)
{
    tListEntry* listElement = new tListEntry();

    listElement->callback          = callback;
    listElement->target            = target;
    listElement->paused            = paused;
    listElement->priority          = 0;
    listElement->markedForDeletion = false;

    list.emplace_back(listElement);

    auto& hashElement = _hashForUpdates.emplace(target, tHashUpdateEntry{}).first->second;
    hashElement.list  = &list;
    hashElement.entry = listElement;
}

void Scheduler::schedulePerFrame(const ccSchedulerFunc& callback, void* target, int priority, bool paused)
{
    tHashUpdateEntry* hashElement = nullptr;
    // HASH_FIND_PTR(_hashForUpdates, &target, hashElement);
    auto updateIt = _hashForUpdates.find(target);
    if (updateIt != _hashForUpdates.end())
    {
        // change priority: should unschedule it first
        if (updateIt->second.entry->priority != priority)
        {
            unscheduleUpdate(target);
        }
        else
        {
            // don't add it again
            return;
        }
    }

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

bool Scheduler::isScheduled(std::string_view key, const void* target) const
{
    AXASSERT(!key.empty(), "Argument key must not be empty");
    AXASSERT(target, "Argument target must be non-nullptr");

    auto timerIt = _hashForTimers.find(target);

    if (timerIt == _hashForTimers.end())
    {
        return false;
    }

    auto timers = timerIt->second.timers;
    if (!timers)
    {
        return false;
    }

    for (int i = 0; i < timers->size(); ++i)
    {
        TimerTargetCallback* timer = static_cast<TimerTargetCallback*>(timers->at(i));

        if (timer && !timer->isExhausted() && key == timer->getKey())
        {
            return true;
        }
    }

    return false;
}

void Scheduler::removeUpdateFromHash(struct _listEntry* entry)
{
    auto updateIt = _hashForUpdates.find(entry->target);
    if (updateIt != _hashForUpdates.end())
    {
        auto& element = updateIt->second;
        // list entry
        axstd::erase(*element.list, entry);
        if (!_updateHashLocked)
            AX_SAFE_DELETE(element.entry);
        else
        {
            element.entry->markedForDeletion = true;
            _updateDeleteVector.emplace_back(element.entry);
        }

        // hash entry
        _hashForUpdates.erase(updateIt);
    }
}

void Scheduler::unscheduleUpdate(void* target)
{
    if (target == nullptr)
    {
        return;
    }

    tHashUpdateEntry* element = nullptr;
    auto updateIt             = _hashForUpdates.find(target);
    if (updateIt != _hashForUpdates.end())
        this->removeUpdateFromHash(updateIt->second.entry);
}

void Scheduler::unscheduleAll()
{
    unscheduleAllWithMinPriority(PRIORITY_SYSTEM);
}

void Scheduler::unscheduleAllWithMinPriority(int minPriority)
{
    // Custom Selectors
    for (auto& [_, element] : _hashForTimers)
    {
        unscheduleAllForTarget(&element);
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
    auto timerIt = _hashForTimers.find(target);
    if (timerIt != _hashForTimers.end())
    {
        auto& element = timerIt->second;
        if (element.timers->contains(element.currentTimer) && (!element.currentTimer->isAborted()))
        {
            element.currentTimer->retain();
            element.currentTimer->setAborted();
        }
        element.timers->clear();

        if (_currentTarget == &element)
        {
            _currentTargetSalvaged = true;
        }
        else
        {
            delete element.timers;
            _hashForTimers.erase(timerIt);
        }
    }

    // update selector
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
    auto timerIt = _hashForTimers.find(target);
    if (timerIt != _hashForTimers.end())
    {
        timerIt->second.paused = false;
    }

    // update selector
    auto updateIt = _hashForUpdates.find(target);
    if (updateIt != _hashForUpdates.end())
    {
        updateIt->second.entry->paused = false;
    }
}

void Scheduler::pauseTarget(void* target)
{
    AXASSERT(target != nullptr, "target can't be nullptr!");

    // custom selectors
    auto timerIt = _hashForTimers.find(target);
    if (timerIt != _hashForTimers.end())
    {
        timerIt->second.paused = false;
    }

    // update selector
    auto updateIt = _hashForUpdates.find(target);
    if (updateIt != _hashForUpdates.end())
    {
        updateIt->second.entry->paused = true;
    }
}

bool Scheduler::isTargetPaused(void* target)
{
    AXASSERT(target != nullptr, "target must be non nil");

    // Custom selectors
    auto timerIt = _hashForTimers.find(target);
    if (timerIt != _hashForTimers.end())
    {
        return timerIt->second.paused;
    }

    // We should check update selectors if target does not have custom selectors
    auto updateIt = _hashForUpdates.find(target);
    if (updateIt != _hashForUpdates.end())
    {
        return updateIt->second.entry->paused;
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
    for (auto& [target, element] : _hashForTimers)
    {
        element.paused = true;
        idsWithSelectors.insert((void*)target);
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
    _updateHashLocked = true;

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
    for (auto it = _hashForTimers.begin(); it != _hashForTimers.end();)
    {
        auto elt               = &it->second;
        _currentTarget         = elt;
        _currentTargetSalvaged = false;

        if (!_currentTarget->paused)
        {
            // The 'timers' array may change while inside this loop
            for (elt->timerIndex = 0; elt->timerIndex < elt->timers->size(); ++(elt->timerIndex))
            {
                elt->currentTimer = (Timer*)(elt->timers->at(elt->timerIndex));
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

        // elt, at this moment, is still valid
        // so it is safe to ask this here (issue #490)
        // elt = (tHashTimerEntry*)elt->hh.next;

        // only delete currentTarget if no actions were scheduled during the cycle (issue #481)
        if (_currentTargetSalvaged && _currentTarget->timers->size() == 0)
        {
            delete _currentTarget->timers;
            it = _hashForTimers.erase(it);
        }
        else
            ++it;
    }

    // delete all updates that are removed in update
    for (auto&& e : _updateDeleteVector)
        delete e;

    _updateDeleteVector.clear();

    _updateHashLocked = false;
    _currentTarget    = nullptr;

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
                         Ref* target,
                         float interval,
                         unsigned int repeat,
                         float delay,
                         bool paused)
{
    AXASSERT(target, "Argument target must be non-nullptr");

    auto timerIt = _hashForTimers.find(target);

    if (timerIt == _hashForTimers.end())
    {
        timerIt = _hashForTimers.emplace(target, tHashTimerEntry{}).first;

        // Is this the 1st element ? Then set the pause level to all the selectors of this target
        timerIt->second.paused = paused;
    }
    else
    {
        AXASSERT(timerIt->second.paused == paused, "element's paused should be paused.");
    }

    auto timers = timerIt->second.timers;
    if (!timers)
    {
        timers = timerIt->second.timers = new Vector<Timer*>(10);
    }
    else
    {
        for (int i = 0; i < timers->size(); ++i)
        {
            TimerTargetSelector* timer = static_cast<TimerTargetSelector*>(timers->at(i));

            if (timer && !timer->isExhausted() && selector == timer->getSelector())
            {
                AXLOG("CCScheduler#schedule. Reiniting timer with interval %.4f, repeat %u, delay %.4f", interval,
                      repeat, delay);
                timer->setupTimerWithInterval(interval, repeat, delay);
                return;
            }
        }
    }

    TimerTargetSelector* timer = new TimerTargetSelector();
    timer->initWithSelector(this, selector, target, interval, repeat, delay);
    timers->pushBack(timer);
    timer->release();
}

void Scheduler::schedule(SEL_SCHEDULE selector, Ref* target, float interval, bool paused)
{
    this->schedule(selector, target, interval, AX_REPEAT_FOREVER, 0.0f, paused);
}

bool Scheduler::isScheduled(SEL_SCHEDULE selector, const Ref* target) const
{
    AXASSERT(selector, "Argument selector must be non-nullptr");
    AXASSERT(target, "Argument target must be non-nullptr");

    auto timerIt = _hashForTimers.find(target);
    if (timerIt == _hashForTimers.end())
    {
        return false;
    }

    auto timers = timerIt->second.timers;
    if (!timers)
    {
        return false;
    }

    for (int i = 0; i < timers->size(); ++i)
    {
        TimerTargetSelector* timer = static_cast<TimerTargetSelector*>(timers->at(i));

        if (timer && !timer->isExhausted() && selector == timer->getSelector())
        {
            return true;
        }
    }

    return false;
}

void Scheduler::unschedule(SEL_SCHEDULE selector, Ref* target)
{
    // explicit handle nil arguments when removing an object
    if (target == nullptr || selector == nullptr)
    {
        return;
    }

    auto timerIt = _hashForTimers.find(target);
    if (timerIt != _hashForTimers.end())
    {
        auto& element = timerIt->second;
        auto timers   = element.timers;
        for (int i = 0; i < timers->size(); ++i)
        {
            TimerTargetSelector* timer = dynamic_cast<TimerTargetSelector*>(timers->at(i));

            if (timer && selector == timer->getSelector())
            {
                if (timer == element.currentTimer && !timer->isAborted())
                {
                    timer->retain();
                    timer->setAborted();
                }

                timers->erase(i);

                // update timerIndex in case we are in tick:, looping over the actions
                if (element.timerIndex >= i)
                {
                    element.timerIndex--;
                }

                if (timers->size() == 0)
                {
                    if (_currentTarget == &element)
                    {
                        _currentTargetSalvaged = true;
                    }
                    else
                    {
                        delete element.timers;
                        _hashForTimers.erase(timerIt);
                    }
                }

                return;
            }
        }
    }
}

NS_AX_END
