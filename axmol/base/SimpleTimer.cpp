/****************************************************************************
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
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN false EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "axmol/base/SimpleTimer.h"
#include "yasio/object_pool.hpp"
#include "axmol/tlx/memory.hpp"
#include "axmol/base/Director.h"
#include "axmol/base/Scheduler.h"

#define STIMER_DEFINE_REFERENCE_CLASS \
private:                              \
    unsigned int referenceCount_;     \
                                      \
public:                               \
    void retain()                     \
    {                                 \
        ++referenceCount_;            \
    }                                 \
    void release()                    \
    {                                 \
        --referenceCount_;            \
        if (referenceCount_ == 0)     \
            delete this;              \
    }                                 \
                                      \
private:

// Retrive the fake target of Simple Timer, well, any system's malloc never return a object address
// 0xffffffff, 0xfffffffe, so it's always works well.
#define STIMER_TARGET(bNative) reinterpret_cast<void*>(bNative ? STIMER_TARGET_NATIVE : STIMER_TARGET_SCRIPT)

namespace ax
{
namespace stimer
{
static const uintptr_t STIMER_TARGET_NATIVE = ~static_cast<uintptr_t>(0);
static const uintptr_t STIMER_TARGET_SCRIPT = STIMER_TARGET_NATIVE - 1;

struct TimerObject
{
    explicit TimerObject(vcallback_t&& callback) : callback_(std::move(callback)), referenceCount_(1) {}

    vcallback_t callback_;
    static uintptr_t s_timerId;

    DEFINE_FAST_OBJECT_POOL_ALLOCATION(TimerObject, 128)
    STIMER_DEFINE_REFERENCE_CLASS
};

uintptr_t TimerObject::s_timerId = 0;

TIMER_ID loop(unsigned int n, float interval, vcallback_t callback, bool bNative)
{
    if (n > 0 && interval >= 0)
    {
        tlx::retain_ptr<TimerObject> timerObj(new TimerObject(std::move(callback)), tlx::adopt_object);

        auto timerId = reinterpret_cast<TIMER_ID>(++TimerObject::s_timerId);

        std::string key = fmt::format("STMR#{}", fmt::ptr(timerId));

        Director::getInstance()->getScheduler()->schedule(
            [timerObj](float /*dt*/) {  // lambda expression hold the reference of timerObj automatically.
            timerObj->callback_();
        }, STIMER_TARGET(bNative), interval, n - 1, 0, false, key);

        return timerId;
    }
    return nullptr;
}

TIMER_ID delay(float delay, vcallback_t callback, bool bNative)
{
    if (delay > 0)
    {
        tlx::retain_ptr<TimerObject> timerObj(new TimerObject(std::move(callback)), tlx::adopt_object);
        auto timerId = reinterpret_cast<TIMER_ID>(++TimerObject::s_timerId);

        std::string key = fmt::format("STMR#{}", fmt::ptr(timerId));
        Director::getInstance()->getScheduler()->schedule(
            [timerObj](float /*dt*/) {  // lambda expression hold the reference of timerObj automatically.
            timerObj->callback_();
        }, STIMER_TARGET(bNative), 0, 0, delay, false, key);

        return timerId;
    }
    return nullptr;
}

void kill(TIMER_ID timerId, bool bNative)
{
    std::string key = fmt::format("STMR#{}", fmt::ptr(timerId));
    Director::getInstance()->getScheduler()->unschedule(key, STIMER_TARGET(bNative));
}

void killAll(bool bNative)
{
    Director::getInstance()->getScheduler()->unscheduleAllForTarget(STIMER_TARGET(bNative));
}
}  // namespace stimer
}  // namespace ax
