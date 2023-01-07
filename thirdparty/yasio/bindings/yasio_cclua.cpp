//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any 
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2023 HALX99

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "yasio/bindings/yasio_cclua.hpp"
#include "yasio/bindings/lyasio.hpp"
#include "yasio/detail/object_pool.hpp"
#include "yasio/detail/ref_ptr.hpp"
#include "yasio/stl/string_view.hpp"

// A workaround to fix compile issue caused by `CCPlatformMacros.h` doesn't handle `__has_attribute` it properly
#  if !__has_attribute(format)
#    undef __has_attribute
#  endif
#include "cocos2d.h"
using namespace cocos2d;

namespace lyasio
{

namespace stimer
{
// The STIMER fake target: 0xfffffffe, well, any system's malloc never return a object address
// so it's always works well.
#define STIMER_TARGET_VALUE reinterpret_cast<void*>(~static_cast<uintptr_t>(0) - 1)

typedef void* TIMER_ID;
typedef std::function<void()> vcallback_t;

struct TimerObject
{
  TimerObject(vcallback_t&& callback) : callback_(std::move(callback)) {}

  vcallback_t callback_;
  static uintptr_t s_timerId;

  DEFINE_OBJECT_POOL_ALLOCATION(TimerObject, 128)
  YASIO__DEFINE_REFERENCE_CLASS
};
uintptr_t TimerObject::s_timerId = 0;

static TIMER_ID loop(unsigned int n, float interval, vcallback_t callback)
{
  if (n > 0 && interval >= 0)
  {
    yasio::gc::ref_ptr<TimerObject> timerObj(new TimerObject(std::move(callback)));

    auto timerId = reinterpret_cast<TIMER_ID>(++TimerObject::s_timerId);

    std::string key = StringUtils::format("LSTMR#%p", timerId);

    Director::getInstance()->getScheduler()->schedule(
        [timerObj](
            float /*dt*/) { // lambda expression hold the reference of timerObj automatically.
          timerObj->callback_();
        },
        STIMER_TARGET_VALUE, interval, n - 1, 0, false, key);

    return timerId;
  }
  return nullptr;
}

static TIMER_ID delay(float delay, vcallback_t callback)
{
  if (delay > 0)
  {
    yasio::gc::ref_ptr<TimerObject> timerObj(new TimerObject(std::move(callback)));
    auto timerId = reinterpret_cast<TIMER_ID>(++TimerObject::s_timerId);

    std::string key = StringUtils::format("LSTMR#%p", timerId);
    Director::getInstance()->getScheduler()->schedule(
        [timerObj](
            float /*dt*/) { // lambda expression hold the reference of timerObj automatically.
          timerObj->callback_();
        },
        STIMER_TARGET_VALUE, 0, 0, delay, false, key);

    return timerId;
  }
  return nullptr;
}

static void kill(TIMER_ID timerId)
{
  std::string key = StringUtils::format("LSTMR#%p", timerId);
  Director::getInstance()->getScheduler()->unschedule(key, STIMER_TARGET_VALUE);
}
YASIO_LUA_API void clear()
{
  Director::getInstance()->getScheduler()->unscheduleAllForTarget(STIMER_TARGET_VALUE);
}
} // namespace stimer
} // namespace lyasio

#if YASIO__HAS_CXX14

#  if !YASIO__HAS_CXX20
#    include "sol2/sol.hpp"
#  else
#    include "sol/sol.hpp"
#  endif

extern "C" {
struct lua_State;
YASIO_LUA_API int luaopen_yasio_cclua(lua_State* L)
{
  int n = luaopen_yasio(L);
  
  sol::stack_table yasio(L, -1);
  yasio.set_function("delay", lyasio::stimer::delay);
  yasio.set_function("loop", lyasio::stimer::loop);
  yasio.set_function("kill", lyasio::stimer::kill);
  
  return n;
}
}

#else

#  include "kaguya/kaguya.hpp"

extern "C" {
struct lua_State;
YASIO_LUA_API int luaopen_yasio_cclua(lua_State* L)
{
  luaopen_yasio(L);

  kaguya::State state(L);
  auto yasio = state.popFromStack();
  yasio.setField("delay", lyasio::stimer::delay);
  yasio.setField("loop", lyasio::stimer::loop);
  yasio.setField("kill", lyasio::stimer::kill);

  return yasio.push();
}
}
#endif
