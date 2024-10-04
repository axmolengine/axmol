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

#    include "platform/wasm/Application-wasm.h"
#    include "platform/wasm/devtools-wasm.h"
#    include <unistd.h>
#    include <sys/time.h>
#    include <string>
#    include "base/Director.h"
#    include "base/Utils.h"
#    include "platform/FileUtils.h"
#    include "yasio/utils.hpp"
#    include <emscripten/emscripten.h>

extern void axmol_wasm_app_exit();

extern "C" {
//
void axmol_hdoc_visibilitychange(bool hidden)
{
    ax::EventCustom event(hidden ? EVENT_COME_TO_BACKGROUND : EVENT_COME_TO_FOREGROUND);
    ax::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event, true);
}

// webglcontextlost
void axmol_webglcontextlost()
{
    AXLOGI("receive event: webglcontextlost");
}

// webglcontextrestored
void axmol_webglcontextrestored()
{
    AXLOGI("receive event: webglcontextrestored");

    auto director = ax::Director::getInstance();
    ax::backend::DriverBase::getInstance()->resetState();
    director->resetMatrixStack();
    ax::EventCustom recreatedEvent(EVENT_RENDERER_RECREATED);
    director->getEventDispatcher()->dispatchEvent(&recreatedEvent, true);
    director->setGLDefaultValues();
#    if AX_ENABLE_CACHE_TEXTURE_DATA
    ax::VolatileTextureMgr::reloadAllTextures();
#    endif
}

void axmol_dev_pause()
{
    ax::DevToolsImpl::getInstance()->pause();
}

void axmol_dev_resume()
{
    ax::DevToolsImpl::getInstance()->resume();
}

void axmol_dev_step()
{
    ax::DevToolsImpl::getInstance()->step();
}
}

namespace ax
{

// sharedApplication pointer
Application* Application::sm_pSharedApplication = nullptr;

static int64_t NANOSECONDSPERSECOND = 1000000000LL;
static int64_t NANOSECONDSPERMICROSECOND = 1000000LL;
static int64_t FPS_CONTROL_THRESHOLD = static_cast<int64_t>(1.0f / 1200.0f * NANOSECONDSPERSECOND);

static int64_t s_animationInterval = static_cast<int64_t>(1/60.0 * NANOSECONDSPERSECOND);

static Director* __director;
static int64_t mLastTickInNanoSeconds = 0;

static void renderFrame() {
    auto director = __director;
    auto glview   = director->getGLView();

    director->mainLoop();
    glview->pollEvents();

    if (glview->windowShouldClose())
    {
        AXLOGI("shuting down axmol wasm app ...");
        emscripten_cancel_main_loop();  // Cancel current loop and set the cleanup one.

        if (glview->isOpenGLReady())
        {
            director->end();
            director->mainLoop();
        }
        glview->release();

        axmol_wasm_app_exit();
    }
}

static void updateFrame(void)
{
    renderFrame();

    /*
    * No need to use algorithm in default(60,90,120... FPS) situation,
    * since onDrawFrame() was called by system 60 times per second by default.
    */
    if (s_animationInterval > FPS_CONTROL_THRESHOLD) {
        auto interval = yasio::xhighp_clock() - mLastTickInNanoSeconds;

        if (interval < s_animationInterval) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(s_animationInterval - interval));
        }

        mLastTickInNanoSeconds = yasio::xhighp_clock();
    }
}

static void getCurrentLangISO2(char buf[16]) {
    // clang-format off
    EM_ASM_ARGS(
        {
            var lang = localStorage.getItem('localization_language');
            if (lang == null)
            {
                stringToUTF8(window.navigator.language.replace(/-.*/, ""), $0, 16);
            }
            else
            {
                stringToUTF8(lang, $0, 16);
            }
        },
        buf);
    // clang-format on
}

Application::Application()
{
    AX_ASSERT(!sm_pSharedApplication);
    sm_pSharedApplication = this;
}

Application::~Application()
{
    AX_ASSERT(this == sm_pSharedApplication);
    sm_pSharedApplication = nullptr;
}

int Application::run()
{
    initGLContextAttrs();
    // Initialize instance and cocos2d.
    if (!applicationDidFinishLaunching())
    {
        return 1;
    }

    __director = Director::getInstance();

    // Retain glview to avoid glview being released in the while loop
    __director->getGLView()->retain();

    /*
    The JavaScript environment will call that function at a specified number
    of frames per second. If called on the main browser thread, setting 0 or
    a negative value as the fps will use the browser’s requestAnimationFrame mechanism
    o call the main loop function. This is HIGHLY recommended if you are doing rendering,
    as the browser’s requestAnimationFrame will make sure you render at a proper smooth rate
    that lines up properly with the browser and monitor.
    */
    emscripten_set_main_loop(updateFrame, -1, false);

    return 0;
}

void Application::setAnimationInterval(float interval)
{
    s_animationInterval = static_cast<int64_t>(interval * NANOSECONDSPERSECOND);
}

Application::Platform Application::getTargetPlatform()
{
    return Platform::Wasm;
}

std::string Application::getVersion()
{
    return "";
}

bool Application::openURL(std::string_view url)
{
    EM_ASM_ARGS({ window.open(UTF8ToString($0)); }, url.data());

    return true;
}

//////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////
Application* Application::getInstance()
{
    AX_ASSERT(sm_pSharedApplication);
    return sm_pSharedApplication;
}

const char* Application::getCurrentLanguageCode()
{
    static char code[3] = {0};
    char pLanguageName[16];
    getCurrentLangISO2(pLanguageName);
    strncpy(code, pLanguageName, 2);
    code[2] = '\0';
    return code;
}

LanguageType Application::getCurrentLanguage()
{
    char pLanguageName[16];
    getCurrentLangISO2(pLanguageName);
    return utils::getLanguageTypeByISO2(pLanguageName);
}

}  // namespace ax

#endif  // AX_TARGET_PLATFORM == AX_PLATFORM_WASM
