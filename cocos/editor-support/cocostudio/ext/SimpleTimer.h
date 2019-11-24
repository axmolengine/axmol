//
// Copyright (c) 2014-2018 x-studio365 - All Rights Reserved
//
#ifndef _SIMPLE_TIMER_H_
#define _SIMPLE_TIMER_H_

#include "editor-support/cocostudio/CocosStudioExport.h"
#include <functional>

namespace simple_timer {
    typedef void* TIMER_ID;
    typedef std::function<void(void)> vcallback_t;
    CC_STUDIOP_DLL TIMER_ID loop(unsigned int n, float interval, vcallback_t callback);
    CC_STUDIOP_DLL TIMER_ID delay(float delay, vcallback_t callback);
    CC_STUDIOP_DLL void kill(TIMER_ID timerId);
}

#endif
