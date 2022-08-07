/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axys1.github.io/

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

#include "base/CCIMEDispatcher.h"
#include "base/CCDirector.h"
#include "base/CCEventType.h"
#include "base/CCEventCustom.h"
#include "base/CCEventDispatcher.h"
#include "platform/CCApplication.h"
#include "platform/CCFileUtils.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>

#include "base/ccUTF8.h"

USING_NS_AX;

extern "C" {

JNIEXPORT void JNICALL Java_org_axys1_lib_AxysRenderer_nativeRender(JNIEnv*, jclass)
{
    axis::Director::getInstance()->mainLoop();
}

JNIEXPORT void JNICALL Java_org_axys1_lib_AxysRenderer_nativeOnPause(JNIEnv*, jclass)
{
    if (Director::getInstance()->getOpenGLView())
    {
        Application::getInstance()->applicationDidEnterBackground();
        axis::EventCustom backgroundEvent(EVENT_COME_TO_BACKGROUND);
        axis::Director::getInstance()->getEventDispatcher()->dispatchEvent(&backgroundEvent);
    }
}

JNIEXPORT void JNICALL Java_org_axys1_lib_AxysRenderer_nativeOnResume(JNIEnv*, jclass)
{
    static bool firstTime = true;
    if (Director::getInstance()->getOpenGLView())
    {
        // don't invoke at first to keep the same logic as iOS
        // can refer to https://github.com/cocos2d/cocos2d-x/issues/14206
        if (!firstTime)
            Application::getInstance()->applicationWillEnterForeground();

        axis::EventCustom foregroundEvent(EVENT_COME_TO_FOREGROUND);
        axis::Director::getInstance()->getEventDispatcher()->dispatchEvent(&foregroundEvent);

        firstTime = false;
    }
}

JNIEXPORT void JNICALL Java_org_axys1_lib_AxysRenderer_nativeInsertText(JNIEnv* env, jclass, jstring text)
{
    std::string strValue = axis::StringUtils::getStringUTFCharsJNI(env, text);
    const char* pszText  = strValue.c_str();
    axis::IMEDispatcher::sharedDispatcher()->dispatchInsertText(pszText, strlen(pszText));
}

JNIEXPORT void JNICALL Java_org_axys1_lib_AxysRenderer_nativeDeleteBackward(JNIEnv*, jclass)
{
    axis::IMEDispatcher::sharedDispatcher()->dispatchDeleteBackward();
}

JNIEXPORT jstring JNICALL Java_org_axys1_lib_AxysRenderer_nativeGetContentText(JNIEnv* env, jclass)
{
    auto pszText = axis::IMEDispatcher::sharedDispatcher()->getContentText();
    return axis::StringUtils::newStringUTFJNI(env, pszText);
}
}
