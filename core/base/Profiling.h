/****************************************************************************
Copyright (c) 2010      Stuart Carnie
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#ifndef __SUPPORT_CCPROFILING_H__
#define __SUPPORT_CCPROFILING_H__
/// @cond DO_NOT_SHOW

#include <string>
#include <chrono>
#include "base/Config.h"
#include "base/Object.h"
#include "base/Map.h"

namespace ax
{

/**
 * @addtogroup global
 * @{
 */

class ProfilingTimer;

/** Profiler
 cocos2d builtin profiler.

 To use it, enable set the AX_ENABLE_PROFILERS=1 in the ccConfig.h file
 */

class AX_DLL Profiler
{
public:
    /**
     * @js NA
     * @lua NA
     */
    ~Profiler();
    /** display the timers
     * @js NA
     * @lua NA
     */
    void displayTimers();
    /**
     * @js NA
     * @lua NA
     */
    bool init();

public:
    /** returns the singleton
     * @js NA
     * @lua NA
     */
    static Profiler* getInstance();

    /** Creates and adds a new timer
     * @js NA
     * @lua NA
     */
    ProfilingTimer* createAndAddTimerWithName(const char* timerName);
    /** releases a timer
     * @js NA
     * @lua NA
     */
    void releaseTimer(const char* timerName);
    /** releases all timers
     * @js NA
     * @lua NA
     */
    void releaseAllTimers();

    Map<std::string, ProfilingTimer*> _activeTimers;
};

class ProfilingTimer : public Object
{
public:
    /**
     * @js NA
     * @lua NA
     */
    ProfilingTimer();
    /**
     * @js NA
     * @lua NA
     */
    ~ProfilingTimer();
    /**
     * @js NA
     * @lua NA
     */
    bool initWithName(const char* timerName);
    /**
     * @js NA
     * @lua NA
     */
    virtual std::string getDescription() const;
    /**
     * @js NA
     * @lua NA
     */
    const std::chrono::high_resolution_clock::time_point& getStartTime() { return _startTime; }

    /** resets the timer properties
     * @js NA
     * @lua NA
     */
    void reset();

    std::string _nameStr;
    std::chrono::high_resolution_clock::time_point _startTime;
    int32_t _averageTime1;
    int32_t _averageTime2;
    int32_t minTime;
    int32_t maxTime;
    int32_t totalTime;
    int32_t numberOfCalls;
};

extern void AX_DLL ProfilingBeginTimingBlock(const char* timerName);
extern void AX_DLL ProfilingEndTimingBlock(const char* timerName);
extern void AX_DLL ProfilingResetTimingBlock(const char* timerName);

/*
 * cocos2d profiling categories
 * used to enable / disable profilers with granularity
 */

extern bool kProfilerCategorySprite;
extern bool kProfilerCategoryBatchSprite;
extern bool kProfilerCategoryParticles;

// end of global group
/// @}

}

/// @endcond
#endif  // __SUPPORT_CCPROFILING_H__
