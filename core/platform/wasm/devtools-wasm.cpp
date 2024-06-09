/****************************************************************************
Copyright (c) 2011      Laschweinski
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
