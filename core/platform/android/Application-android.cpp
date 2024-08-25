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
#include "platform/android/jni/JniHelper.h"
#include "platform/Application.h"
#include "base/Director.h"
#include "base/Utils.h"
#include <android/log.h>
#include <jni.h>
#include <cstring>

#define LOG_TAG "CCApplication_android Debug"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// FIXME: using ndk-r10c will cause the next function could not be found. It may be a bug of ndk-r10c.
// Here is the workaround method to fix the problem.
#ifdef __aarch64__
extern "C" size_t __ctype_get_mb_cur_max(void)
{
    return (size_t)sizeof(wchar_t);
}
#endif

static const char* applicationHelperClassName = "org.axmol.lib.AxmolEngine";

namespace ax
{

// sharedApplication pointer
Application* Application::sm_pSharedApplication = nullptr;

Application::Application()
{
    CCAssert(!sm_pSharedApplication, "");
    sm_pSharedApplication = this;
}

Application::~Application()
{
    CCAssert(this == sm_pSharedApplication, "");
    sm_pSharedApplication = nullptr;
}

int Application::run()
{
    // Initialize instance and cocos2d.
    if (!applicationDidFinishLaunching())
    {
        return 0;
    }

    return -1;
}

void Application::setAnimationInterval(float interval)
{
    JniHelper::callStaticVoidMethod("org/axmol/lib/AxmolRenderer", "setAnimationInterval", interval);
}

//////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////
Application* Application::getInstance()
{
    CCAssert(sm_pSharedApplication, "");
    return sm_pSharedApplication;
}

// @deprecated Use getInstance() instead
Application* Application::sharedApplication()
{
    return Application::getInstance();
}

const char* Application::getCurrentLanguageCode()
{
    static char code[3]  = {0};
    std::string language = JniHelper::callStaticStringMethod(applicationHelperClassName, "getCurrentLanguage");
    strncpy(code, language.c_str(), 2);
    code[2] = '\0';
    return code;
}

LanguageType Application::getCurrentLanguage()
{
    const char* code = getCurrentLanguageCode();

    return utils::getLanguageTypeByISO2(code);
}

Application::Platform Application::getTargetPlatform()
{
    return Platform::Android;
}

std::string Application::getVersion()
{
    return JniHelper::callStaticStringMethod(applicationHelperClassName, "getVersion");
}

bool Application::openURL(std::string_view url)
{
    return JniHelper::callStaticBooleanMethod(applicationHelperClassName, "openURL", url);
}

void Application::applicationScreenSizeChanged(int newWidth, int newHeight) {}

}

#undef LOGD
#undef LOG_TAG
