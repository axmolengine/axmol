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
#    include <emscripten/emscripten.h>

extern "C" {
//
void axmol_hdoc_visibilitychange(bool hidden)
{
    ax::EventCustom event(hidden ? EVENT_COME_TO_BACKGROUND : EVENT_COME_TO_FOREGROUND);
    ax::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event, true);
}

// webglcontextlost
void axmol_wgl_context_lost()
{
    AXLOGI("receive event: webglcontextlost");
}

// webglcontextrestored
void axmol_wgl_context_restored()
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

Application::Application() : _animationSpeed(60)
{
    AX_ASSERT(!sm_pSharedApplication);
    sm_pSharedApplication = this;
}

Application::~Application()
{
    AX_ASSERT(this == sm_pSharedApplication);
    sm_pSharedApplication = nullptr;
}

extern "C" void mainLoopIter(void)
{
    auto director = Director::getInstance();
    auto glview   = director->getGLView();

    director->mainLoop();
    glview->pollEvents();
}

int Application::run()
{
    initGLContextAttrs();
    // Initialize instance and cocos2d.
    if (!applicationDidFinishLaunching())
    {
        return 1;
    }

    auto director = Director::getInstance();
    auto glview   = director->getGLView();

    // Retain glview to avoid glview being released in the while loop
    glview->retain();

    // emscripten_set_main_loop(&mainLoopIter, 0, 1);
    emscripten_set_main_loop(&mainLoopIter, _animationSpeed, 1);
    // TODO: ? does these cleanup really run?
    /* Only work on Desktop
     *  Director::mainLoop is really one frame logic
     *  when we want to close the window, we should call Director::end();
     *  then call Director::mainLoop to do release of internal resources
     */
    if (glview->isOpenGLReady())
    {
        director->end();
        director->mainLoop();
        director = nullptr;
    }
    glview->release();
    return 0;
}

void Application::setAnimationInterval(float interval)
{
    _animationSpeed = 1.0f / interval;
}

void Application::setResourceRootPath(const std::string& rootResDir)
{
    _resourceRootPath = rootResDir;
    if (_resourceRootPath[_resourceRootPath.length() - 1] != '/')
    {
        _resourceRootPath += '/';
    }
    FileUtils* pFileUtils                = FileUtils::getInstance();
    std::vector<std::string> searchPaths = pFileUtils->getSearchPaths();
    searchPaths.insert(searchPaths.begin(), _resourceRootPath);
    pFileUtils->setSearchPaths(searchPaths);
}

const std::string& Application::getResourceRootPath()
{
    return _resourceRootPath;
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

// @deprecated Use getInstance() instead
Application* Application::sharedApplication()
{
    return Application::getInstance();
}

const char* Application::getCurrentLanguageCode()
{
    static char code[3] = {0};
    char pLanguageName[16];

    EM_ASM_ARGS(
        {
            var lang = localStorage.getItem('localization_language');
            if (lang == null)
            {
                stringToUTF8(window.navigator.language.replace(/ - .*/, ""), $0, 16);
            }
            else
            {
                stringToUTF8(lang, $0, 16);
            }
        },
        pLanguageName);
    strncpy(code, pLanguageName, 2);
    code[2] = '\0';
    return code;
}

LanguageType Application::getCurrentLanguage()
{
    char pLanguageName[16];

    EM_ASM_ARGS(
        {
            var lang = localStorage.getItem('localization_language');
            if (lang == null)
            {
                stringToUTF8(window.navigator.language.replace(/ - .*/, ""), $0, 16);
            }
            else
            {
                stringToUTF8(lang, $0, 16);
            }
        },
        pLanguageName);

    return utils::getLanguageTypeByISO2(pLanguageName);
}

}  // namespace ax

#endif  // AX_TARGET_PLATFORM == AX_PLATFORM_WASM
