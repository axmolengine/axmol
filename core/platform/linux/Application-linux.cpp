/****************************************************************************
Copyright (c) 2011      Laschweinski
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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
#include "platform/linux/Application-linux.h"
#include <unistd.h>
#include <sys/time.h>
#include <string>
#include <thread>
#include "base/Director.h"
#include "base/Utils.h"
#include "platform/FileUtils.h"

NS_AX_BEGIN

// sharedApplication pointer
Application* Application::sm_pSharedApplication = nullptr;

Application::Application() : _animationInterval(16666667)
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
        return 0;
    }

    std::chrono::steady_clock::time_point lastTime{};

    auto director = Director::getInstance();
    auto glView   = director->getGLView();

    // Retain glView to avoid glView being released in the while loop
    glView->retain();

    while (!glView->windowShouldClose())
    {
        lastTime = std::chrono::steady_clock::now();

        director->mainLoop();
        glView->pollEvents();

        auto interval = std::chrono::steady_clock::now() - lastTime;
        if (interval < _animationInterval)
        {
            auto waitDuration = _animationInterval - interval;
            std::this_thread::sleep_for(waitDuration);
        }
        else
        {
            std::this_thread::yield();
        }
    }
    /* Only work on Desktop
     *  Director::mainLoop is really one frame logic
     *  when we want to close the window, we should call Director::end();
     *  then call Director::mainLoop to do release of internal resources
     */
    if (glView->isOpenGLReady())
    {
        director->end();
        director->mainLoop();
        director = nullptr;
    }
    glView->release();
    return EXIT_SUCCESS;
}

void Application::setAnimationInterval(float interval)
{
    // TODO do something else
    _animationInterval =
        std::chrono::nanoseconds{static_cast<std::chrono::nanoseconds::rep>(std::nano::den * interval)};
}

Application::Platform Application::getTargetPlatform()
{
    return Platform::Linux;
}

std::string Application::getVersion()
{
    return "";
}

bool Application::openURL(std::string_view url)
{
    std::string op = std::string("xdg-open '").append(url).append("'");
    return system(op.c_str()) == 0;
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
    char* pLanguageName = getenv("LANG");
    if (!pLanguageName)
        return "en";
    strtok(pLanguageName, "_");
    if (!pLanguageName)
        return "en";
    strncpy(code, pLanguageName, 2);
    code[2] = '\0';
    return code;
}

LanguageType Application::getCurrentLanguage()
{
    char* pLanguageName = getenv("LANG");
    if (!pLanguageName)
    {
        return LanguageType::ENGLISH;
    }
    strtok(pLanguageName, "_");
    if (!pLanguageName)
    {
        return LanguageType::ENGLISH;
    }

    return utils::getLanguageTypeByISO2(pLanguageName);
}

NS_AX_END
