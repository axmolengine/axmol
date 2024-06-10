/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
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
#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#include <algorithm>
#include <thread>

#include "platform/Application.h"
#include "platform/FileUtils.h"
#include "math/Math.h"
#include "base/Director.h"
#include "base/Utils.h"
#include "renderer/backend/metal/DriverMTL.h"

NS_AX_BEGIN

Application* Application::sm_pSharedApplication = nullptr;

Application::Application() : _animationInterval(16666667)
{
    AXASSERT(!sm_pSharedApplication, "sm_pSharedApplication already exist");
    sm_pSharedApplication = this;
}

Application::~Application()
{
    AXASSERT(this == sm_pSharedApplication, "sm_pSharedApplication != this");
    sm_pSharedApplication = 0;
}

int Application::run()
{
    initGLContextAttrs();
    if (!applicationDidFinishLaunching())
    {
        return 1;
    }

    std::chrono::steady_clock::time_point lastTime{};

    constexpr std::chrono::nanoseconds _1ms{1000000};

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
        auto waitDuration = _animationInterval - interval - _1ms;
        if (waitDuration.count() > 0)
            std::this_thread::sleep_for(waitDuration);
        else
            std::this_thread::yield();
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
    }

    glView->release();

    return 0;
}

void Application::setAnimationInterval(float interval)
{
    _animationInterval =
        std::chrono::nanoseconds{static_cast<std::chrono::nanoseconds::rep>(std::nano::den * interval)};
}

Application::Platform Application::getTargetPlatform()
{
    return Platform::OSX;
}

std::string Application::getVersion()
{
    NSString* version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
    if (version)
    {
        return [version UTF8String];
    }
    return "";
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////////////////////////////

Application* Application::getInstance()
{
    AXASSERT(sm_pSharedApplication, "sm_pSharedApplication not set");
    return sm_pSharedApplication;
}

const char* Application::getCurrentLanguageCode()
{
    static char code[3]       = {0};
    NSUserDefaults* defaults  = [NSUserDefaults standardUserDefaults];
    NSArray* languages        = [defaults objectForKey:@"AppleLanguages"];
    NSString* currentLanguage = [languages objectAtIndex:0];

    // get the current language code.(such as English is "en", Chinese is "zh" and so on)
    NSDictionary* temp     = [NSLocale componentsFromLocaleIdentifier:currentLanguage];
    NSString* languageCode = [temp objectForKey:NSLocaleLanguageCode];
    [languageCode getCString:code maxLength:3 encoding:NSASCIIStringEncoding];
    code[2] = '\0';
    return code;
}

LanguageType Application::getCurrentLanguage()
{
    // get the current language and country config
    NSUserDefaults* defaults  = [NSUserDefaults standardUserDefaults];
    NSArray* languages        = [defaults objectForKey:@"AppleLanguages"];
    NSString* currentLanguage = [languages objectAtIndex:0];

    // get the current language code.(such as English is "en", Chinese is "zh" and so on)
    NSDictionary* temp     = [NSLocale componentsFromLocaleIdentifier:currentLanguage];
    NSString* languageCode = [temp objectForKey:NSLocaleLanguageCode];

    return utils::getLanguageTypeByISO2([languageCode UTF8String]);
}

bool Application::openURL(std::string_view url)
{
    NSString* msg = [NSString stringWithCString:url.data() encoding:NSUTF8StringEncoding];
    NSURL* nsUrl  = [NSURL URLWithString:msg];
    return [[NSWorkspace sharedWorkspace] openURL:nsUrl];
}

void Application::setStartupScriptFilename(std::string_view startupScriptFile)
{
    _startupScriptFilename = startupScriptFile;
    std::replace(_startupScriptFilename.begin(), _startupScriptFilename.end(), '\\', '/');
}

std::string_view Application::getStartupScriptFilename()
{
    return _startupScriptFilename;
}

NS_AX_END
