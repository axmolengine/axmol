// STIMER V2.0
// Copyright (c) 2014-2020 x-studio - All Rights Reserved
// 
#ifndef _SIMPLE_TIMER_H_
#define _SIMPLE_TIMER_H_

#include "base/ccMacros.h"
#include <functional>

NS_CC_BEGIN
namespace stimer {
    typedef void* TIMER_ID;
    typedef std::function<void(void)> vcallback_t;
    CC_DLL TIMER_ID loop(unsigned int n, float interval, vcallback_t callback, bool bNative = true);
    CC_DLL TIMER_ID delay(float delay, vcallback_t callback, bool bNative = true);
    CC_DLL void kill(TIMER_ID timerId, bool bNative = true);
    CC_DLL void killAll(bool bNative = true);
}
NS_CC_END

#endif
