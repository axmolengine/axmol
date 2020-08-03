// STIMER V2.0
// Copyright (c) 2014-2019 x-studio365 - All Rights Reserved
// 
#ifndef _SIMPLE_TIMER_H_
#define _SIMPLE_TIMER_H_

#include "cocostudio/CocosStudioExport.h"
#include <functional>

namespace stimer {
    typedef void* TIMER_ID;
    typedef std::function<void(void)> vcallback_t;
    CC_STUDIOP_DLL TIMER_ID loop(unsigned int n, float interval, vcallback_t callback, bool bNative = true);
    CC_STUDIOP_DLL TIMER_ID delay(float delay, vcallback_t callback, bool bNative = true);
    CC_STUDIOP_DLL void kill(TIMER_ID timerId, bool bNative = true);
    CC_STUDIOP_DLL void killAll(bool bNative = true);
}

#endif
