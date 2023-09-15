#include "platform/PlatformConfig.h"
#if AX_TARGET_PLATFORM == AX_PLATFORM_WASM

#include "platform/wasm/devtools-wasm.h"
#include <emscripten.h>
#include "base/UTF8.h"

NS_AX_BEGIN

DevToolsImpl::DevToolsImpl()
{
    _director = Director::getInstance();
    _scheduler = _director->getScheduler();
    _tick = 0;
}

void DevToolsImpl::update(float /*dt*/)
{
    // tick for 2 frames becuase delta time of the 1st frame after resume is forced to 0
    _tick ++;
    if (_tick >= 2)
    {
        _director->pause();
        _scheduler->unscheduleUpdate(this);
    }
}

void DevToolsImpl::step()
{
    _director->resume();
    _tick = 0;
    _scheduler->scheduleUpdate(this, 0, false);
}

void DevToolsImpl::pause()
{
    _director->pause();
}

void DevToolsImpl::resume()
{
    _director->resume();
}

DevToolsImpl* DevToolsImpl::getInstance()
{
    static DevToolsImpl instance;
    return &instance;
}

NS_AX_END

extern "C"
{
    void axmol_director_pause()
    {
        ax::DevToolsImpl::getInstance()->pause();
    }

    void axmol_director_resume()
    {
        ax::DevToolsImpl::getInstance()->resume();
    }

    void axmol_director_step()
    {
        ax::DevToolsImpl::getInstance()->step();
    }
}

#endif // AX_TARGET_PLATFORM == AX_PLATFORM_WASM
