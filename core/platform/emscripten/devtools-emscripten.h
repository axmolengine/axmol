#ifndef __DEVTOOLS_EMSCRIPTEN_H__
#define __DEVTOOLS_EMSCRIPTEN_H__

#include "platform/PlatformConfig.h"
#if AX_TARGET_PLATFORM == AX_PLATFORM_EMSCRIPTEN

#include "base/CCScheduler.h"
#include "base/CCDirector.h"

using namespace cocos2d;

class DevToolsImpl
{
public:
    DevToolsImpl();

    void update(float /*dt*/);

    void step();

    void pause();

    void resume();

    static DevToolsImpl* getInstance();

private:
    unsigned int _tick;
    Scheduler* _scheduler;
    Director* _director;
};

#endif // AX_TARGET_PLATFORM == AX_PLATFORM_EMSCRIPTEN
#endif // __DEVTOOLS_EMSCRIPTEN_H__