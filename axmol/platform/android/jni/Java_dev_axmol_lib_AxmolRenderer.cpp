/****************************************************************************
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

#include "axmol/base/IMEDispatcher.h"
#include "axmol/base/Director.h"
#include "axmol/base/EventType.h"
#include "axmol/base/EventCustom.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/platform/Application.h"
#include "axmol/platform/FileUtils.h"
#include <jni.h>

#include "axmol/base/text_utils.h"

using namespace ax;

extern "C" {

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolRenderer_nativeRender(JNIEnv*, jclass)
{
    ax::Director::getInstance()->mainLoop();
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolRenderer_nativeOnPause(JNIEnv*, jclass)
{
    if (Director::getInstance()->getRenderView())
    {
        Application::getInstance()->applicationDidEnterBackground();
        ax::EventCustom backgroundEvent(EVENT_COME_TO_BACKGROUND);
        ax::Director::getInstance()->getEventDispatcher()->dispatchEvent(&backgroundEvent, true);
    }
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolRenderer_nativeOnResume(JNIEnv*, jclass)
{
    if (Director::getInstance()->getRenderView())
    {
        Application::getInstance()->applicationWillEnterForeground();
        ax::EventCustom foregroundEvent(EVENT_COME_TO_FOREGROUND);
        ax::Director::getInstance()->getEventDispatcher()->dispatchEvent(&foregroundEvent, true);
    }
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolRenderer_nativeInsertText(JNIEnv* env, jclass, jstring text)
{
    std::string strValue = ax::text_utils::getStringUTFCharsJNI(env, text);
    const char* pszText  = strValue.c_str();
    ax::IMEDispatcher::sharedDispatcher()->dispatchInsertText(pszText, strlen(pszText));
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolRenderer_nativeDeleteBackward(JNIEnv*, jclass, jint numChars)
{
    ax::IMEDispatcher::sharedDispatcher()->dispatchDeleteBackward(numChars);
}

JNIEXPORT jstring JNICALL Java_dev_axmol_lib_AxmolRenderer_nativeGetContentText(JNIEnv* env, jclass)
{
    auto pszText = ax::IMEDispatcher::sharedDispatcher()->getContentText();
    return ax::text_utils::newStringUTFJNI(env, pszText);
}
}
