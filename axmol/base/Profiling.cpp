/****************************************************************************
Copyright (c) 2010      Stuart Carnie
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#include "axmol/base/Profiling.h"

using namespace std;

namespace ax
{

// Profiling Categories
/* set to false the categories that you don't want to profile */
bool kProfilerCategorySprite      = false;
bool kProfilerCategoryBatchSprite = false;
bool kProfilerCategoryParticles   = false;

static Profiler* g_sSharedProfiler = nullptr;

Profiler* Profiler::getInstance()
{
    if (!g_sSharedProfiler)
    {
        g_sSharedProfiler = new Profiler();
        g_sSharedProfiler->init();
    }

    return g_sSharedProfiler;
}

ProfilingTimer* Profiler::createAndAddTimerWithName(const char* timerName)
{
    ProfilingTimer* t = new ProfilingTimer();
    t->initWithName(timerName);
    _activeTimers.insert(timerName, t);
    t->release();

    return t;
}

void Profiler::releaseTimer(const char* timerName)
{
    _activeTimers.erase(timerName);
}

void Profiler::releaseAllTimers()
{
    _activeTimers.clear();
}

bool Profiler::init()
{
    return true;
}

Profiler::~Profiler() {}

void Profiler::displayTimers()
{
    for (auto&& iter : _activeTimers)
    {
        ProfilingTimer* timer = iter.second;
        AXLOGI("{}", timer->getDescription());
    }
}

// implementation of ProfilingTimer

ProfilingTimer::ProfilingTimer()
    : _averageTime1(0), _averageTime2(0), minTime(100000000), maxTime(0), totalTime(0), numberOfCalls(0)
{}

bool ProfilingTimer::initWithName(const char* timerName)
{
    _nameStr = timerName;
    return true;
}

ProfilingTimer::~ProfilingTimer() {}

std::string ProfilingTimer::getDescription() const
{
    std::string description =
        fmt::format("{} ::\tavg1: {},\tavg2: {},\tmin: {},\tmax: {},\ttotal: {:.2f}s,\tnr calls: {}", _nameStr,
                    _averageTime1, _averageTime2, minTime, maxTime, totalTime / 1000000., numberOfCalls);
    return description;
}

void ProfilingTimer::reset()
{
    numberOfCalls = 0;
    _averageTime1 = 0;
    _averageTime2 = 0;
    totalTime     = 0;
    minTime       = 100000000;
    maxTime       = 0;
    _startTime    = chrono::high_resolution_clock::now();
}

void ProfilingBeginTimingBlock(const char* timerName)
{
    Profiler* p           = Profiler::getInstance();
    ProfilingTimer* timer = p->_activeTimers.at(timerName);
    if (!timer)
    {
        timer = p->createAndAddTimerWithName(timerName);
    }

    timer->numberOfCalls++;

    // should be the last instruction in order to be more reliable
    timer->_startTime = chrono::high_resolution_clock::now();
}

void ProfilingEndTimingBlock(const char* timerName)
{
    // should be the 1st instruction in order to be more reliable
    auto now = chrono::high_resolution_clock::now();

    Profiler* p           = Profiler::getInstance();
    ProfilingTimer* timer = p->_activeTimers.at(timerName);

    AXASSERT(timer, "CCProfilingTimer  not found");

    int32_t duration =
        static_cast<int32_t>(chrono::duration_cast<chrono::microseconds>(now - timer->_startTime).count());

    timer->totalTime += duration;
    timer->_averageTime1 = (timer->_averageTime1 + duration) / 2.0f;
    timer->_averageTime2 = timer->totalTime / timer->numberOfCalls;
    timer->maxTime       = MAX(timer->maxTime, duration);
    timer->minTime       = MIN(timer->minTime, duration);
}

void ProfilingResetTimingBlock(const char* timerName)
{
    Profiler* p           = Profiler::getInstance();
    ProfilingTimer* timer = p->_activeTimers.at(timerName);

    AXASSERT(timer, "CCProfilingTimer not found");

    timer->reset();
}

}  // namespace ax
