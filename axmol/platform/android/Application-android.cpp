/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#include "axmol/platform/android/jni/JniHelper.h"
#include "axmol/platform/Application.h"
#include "axmol/base/Director.h"
#include "axmol/base/Utils.h"
#include <android/log.h>
#include <jni.h>
#include <string.h>

#define LOG_TAG   "Application_android Debug"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

static const char* applicationHelperClassName = "dev.axmol.lib.AxmolEngine";

namespace ax
{
Application::Application()
{
    CCAssert(!s_axmolApp, "");
    s_axmolApp = this;
}

Application::~Application()
{
    CCAssert(this == s_axmolApp, "");
    s_axmolApp = nullptr;
}

int Application::run()
{
    // Initialize instance and axmol.
    if (!applicationDidFinishLaunching())
    {
        return -1;
    }

    return 0;
}

void Application::setAnimationInterval(float interval)
{
    JniHelper::callStaticVoidMethod("dev/axmol/lib/AxmolPlayer", "setAnimationInterval", interval);
}

const char* Application::getCurrentLanguageCode()
{
    static char code[3]  = {0};
    std::string language = JniHelper::callStaticStringMethod(applicationHelperClassName, "getCurrentLanguage");
    if (language.length() < 2)
        return "en";
    code[0] = language[0];
    code[1] = language[1];
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

void Application::postBoundaryTaskSignal()
{
    JniHelper::callStaticVoidMethod(applicationHelperClassName, "postBoundaryTaskSignal");
}

}  // namespace ax

#undef LOGD
#undef LOG_TAG
