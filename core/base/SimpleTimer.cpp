#include "SimpleTimer.h"
#include "yasio/object_pool.hpp"
#include "yasio/ref_ptr.hpp"
#include "axmol.h"

#define STIMER_DEFINE_REFERENCE_CLASS    \
private:                                 \
    unsigned int referenceCount_;        \
                                         \
public:                                  \
    void retain() { ++referenceCount_; } \
    void release()                       \
    {                                    \
        --referenceCount_;               \
        if (referenceCount_ == 0)        \
            delete this;                 \
    }                                    \
                                         \
private:

// Retrive the fake target of Simple Timer, well, any system's malloc never return a object address
// 0xffffffff, 0xfffffffe, so it's always works well.
#define STIMER_TARGET(bNative) reinterpret_cast<void*>(bNative ? STIMER_TARGET_NATIVE : STIMER_TARGET_SCRIPT)

NS_AX_BEGIN
namespace stimer
{
static const uintptr_t STIMER_TARGET_NATIVE = ~static_cast<uintptr_t>(0);
static const uintptr_t STIMER_TARGET_SCRIPT = STIMER_TARGET_NATIVE - 1;

struct TimerObject
{
    TimerObject(vcallback_t&& callback) : callback_(std::move(callback)), referenceCount_(1) {}

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
        yasio::ref_ptr<TimerObject> timerObj(new TimerObject(std::move(callback)));

        auto timerId = reinterpret_cast<TIMER_ID>(++TimerObject::s_timerId);

        std::string key = fmt::format("STMR#{}", fmt::ptr(timerId));

        Director::getInstance()->getScheduler()->schedule(
            [timerObj](float /*dt*/) {  // lambda expression hold the reference of timerObj automatically.
                timerObj->callback_();
            },
            STIMER_TARGET(bNative), interval, n - 1, 0, false, key);

        return timerId;
    }
    return nullptr;
}

TIMER_ID delay(float delay, vcallback_t callback, bool bNative)
{
    if (delay > 0)
    {
        yasio::ref_ptr<TimerObject> timerObj(new TimerObject(std::move(callback)));
        auto timerId = reinterpret_cast<TIMER_ID>(++TimerObject::s_timerId);

        std::string key = fmt::format("STMR#{}", fmt::ptr(timerId));
        Director::getInstance()->getScheduler()->schedule(
            [timerObj](float /*dt*/) {  // lambda expression hold the reference of timerObj automatically.
                timerObj->callback_();
            },
            STIMER_TARGET(bNative), 0, 0, delay, false, key);

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
NS_AX_END
