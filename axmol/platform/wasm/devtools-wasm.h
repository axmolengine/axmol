#pragma once

#include "platform/PlatformConfig.h"
#if AX_TARGET_PLATFORM == AX_PLATFORM_WASM

#include "base/Scheduler.h"
#include "base/Director.h"

namespace ax
{

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

}

#endif // AX_TARGET_PLATFORM == AX_PLATFORM_WASM
