/****************************************************************************
Copyright (c) 2010 cocos2d-x.org
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
#include "axmol/base/IMEDispatcher.h"
#include "axmol/base/Director.h"
#include "axmol/base/Scheduler.h"
#include "axmol/base/EventType.h"
#include "axmol/base/EventCustom.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/platform/Application.h"
#include "axmol/platform/android/RenderViewImpl-android.h"
#include "axmol/base/text_utils.h"
#include "axmol/platform/android/jni/JniHelper.h"
#include "axmol/rhi/DriverBase.h"
#include "axmol/renderer/TextureCache.h"
#include <android/log.h>
#include <android/native_window_jni.h>

using namespace ax;

static ANativeWindow* s_nativeWindow;

ANativeWindow* axmolGetANativeWindow()
{
    return s_nativeWindow;
}

static void axmolDispatchContextLost(bool isWarmStart)
{
#if AX_ENABLE_RESTART_APPLICATION_ON_CONTEXT_LOST
    auto director = ax::Director::getInstance();
    ax::EventCustom recreatedEvent(EVENT_APP_RESTARTING);
    director->getEventDispatcher()->dispatchEvent(&recreatedEvent, true);

    //  Pop to root scene, replace with an empty scene, and clear all cached data before restarting
    director->popToRootScene();
    auto rootScene = Scene::create();
    director->replaceScene(rootScene);
    director->purgeCachedData();

    JniHelper::callStaticVoidMethod("dev/axmol/lib/AxmolEngine", "restartProcess");
#endif

    if (isWarmStart)
    {
        auto director = ax::Director::getInstance();
        ax::EventCustom warmStartEvent(EVENT_APP_WARM_START);
        director->getEventDispatcher()->dispatchEvent(&warmStartEvent, true);
    }
}

extern "C" {

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeOnSurfaceCreated(JNIEnv* env,
                                                                             jclass,
                                                                             jobject surface,
                                                                             jint w,
                                                                             jint h,
                                                                             jboolean isWarmStart)
{
    if (s_nativeWindow)
    {
        ANativeWindow_release(s_nativeWindow);
    }
    s_nativeWindow  = ANativeWindow_fromSurface(env, surface);
    auto director   = ax::Director::getInstance();
    auto renderView = director->getRenderView();
    if (!renderView)
    {
        renderView = ax::RenderViewImpl::createWithRect(
            "axmol3", Rect{ax::Rect{0, 0, static_cast<float>(w), static_cast<float>(h)}});
        director->setRenderView(renderView);

        auto app = ax::Application::getInstance();
        ax::Application::getInstance()->run();
    }
    else
    {
#if AX_RENDER_API == AX_RENDER_API_VK
        director->getScheduler()->runOnAxmolThread(
            [renderView]() { static_cast<ax::RenderViewImpl*>(renderView)->recreateVkSurface(true); });
#elif AX_RENDER_API == AX_RENDER_API_GL
        axdrv->resetState();
        director->resetMatrixStack();
        ax::EventCustom recreatedEvent(EVENT_RENDERER_RECREATED);
        director->getEventDispatcher()->dispatchEvent(&recreatedEvent, true);
        director->setRenderDefaults();
#    if AX_ENABLE_CONTEXT_LOSS_RECOVERY
        ax::VolatileTextureMgr::reloadAllTextures();
        axmolDispatchContextLost(isWarmStart);
#    endif
#endif
    }
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeRenderFrame(JNIEnv*, jclass)
{
    ax::Director::getInstance()->renderFrame();
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeTouchesBegin(JNIEnv*, jclass, jint id, jfloat x, jfloat y)
{
    intptr_t idlong = id;
    ax::Director::getInstance()->getRenderView()->handleTouchesBegin(1, &idlong, &x, &y);
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeTouchesEnd(JNIEnv*, jclass, jint id, jfloat x, jfloat y)
{
    intptr_t idlong = id;
    ax::Director::getInstance()->getRenderView()->handleTouchesEnd(1, &idlong, &x, &y);
}

JNIEXPORT void JNICALL
Java_dev_axmol_lib_AxmolPlayer_nativeTouchesMove(JNIEnv* env, jclass, jintArray ids, jfloatArray xs, jfloatArray ys)
{
    int size = env->GetArrayLength(ids);
    jint id[size];
    jfloat x[size];
    jfloat y[size];

    env->GetIntArrayRegion(ids, 0, size, id);
    env->GetFloatArrayRegion(xs, 0, size, x);
    env->GetFloatArrayRegion(ys, 0, size, y);

    intptr_t idlong[size];
    for (int i = 0; i < size; i++)
        idlong[i] = id[i];

    ax::Director::getInstance()->getRenderView()->handleTouchesMove(size, idlong, x, y);
}

JNIEXPORT void JNICALL
Java_dev_axmol_lib_AxmolPlayer_nativeTouchesCancel(JNIEnv* env, jclass, jintArray ids, jfloatArray xs, jfloatArray ys)
{
    int size = env->GetArrayLength(ids);
    jint id[size];
    jfloat x[size];
    jfloat y[size];

    env->GetIntArrayRegion(ids, 0, size, id);
    env->GetFloatArrayRegion(xs, 0, size, x);
    env->GetFloatArrayRegion(ys, 0, size, y);

    intptr_t idlong[size];
    for (int i = 0; i < size; i++)
        idlong[i] = id[i];

    ax::Director::getInstance()->getRenderView()->handleTouchesCancel(size, idlong, x, y);
}

#define KEYCODE_BACK        0x04
#define KEYCODE_MENU        0x52
#define KEYCODE_DPAD_UP     0x13
#define KEYCODE_DPAD_DOWN   0x14
#define KEYCODE_DPAD_LEFT   0x15
#define KEYCODE_DPAD_RIGHT  0x16
#define KEYCODE_ENTER       0x42
#define KEYCODE_PLAY        0x7e
#define KEYCODE_DPAD_CENTER 0x17

static std::unordered_map<int, ax::EventKeyboard::KeyCode> g_keyCodeMap = {
    {KEYCODE_BACK, ax::EventKeyboard::KeyCode::KEY_ESCAPE},
    {KEYCODE_MENU, ax::EventKeyboard::KeyCode::KEY_MENU},
    {KEYCODE_DPAD_UP, ax::EventKeyboard::KeyCode::KEY_DPAD_UP},
    {KEYCODE_DPAD_DOWN, ax::EventKeyboard::KeyCode::KEY_DPAD_DOWN},
    {KEYCODE_DPAD_LEFT, ax::EventKeyboard::KeyCode::KEY_DPAD_LEFT},
    {KEYCODE_DPAD_RIGHT, ax::EventKeyboard::KeyCode::KEY_DPAD_RIGHT},
    {KEYCODE_ENTER, ax::EventKeyboard::KeyCode::KEY_ENTER},
    {KEYCODE_PLAY, ax::EventKeyboard::KeyCode::KEY_PLAY},
    {KEYCODE_DPAD_CENTER, ax::EventKeyboard::KeyCode::KEY_DPAD_CENTER},

};

JNIEXPORT jboolean JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeKeyEvent(JNIEnv*,
                                                                         jclass,
                                                                         jint keyCode,
                                                                         jboolean isPressed)
{
    auto iterKeyCode = g_keyCodeMap.find(keyCode);
    if (iterKeyCode == g_keyCodeMap.end())
    {
        return JNI_FALSE;
    }

    ax::EventKeyboard event(iterKeyCode->second, isPressed);
    ax::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    return JNI_TRUE;
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeOnPause(JNIEnv*, jclass)
{
    if (Director::getInstance()->getRenderView())
    {
        Application::getInstance()->applicationDidEnterBackground();
        ax::EventCustom backgroundEvent(EVENT_COME_TO_BACKGROUND);
        ax::Director::getInstance()->getEventDispatcher()->dispatchEvent(&backgroundEvent, true);
    }
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeOnResume(JNIEnv*, jclass)
{
    if (Director::getInstance()->getRenderView())
    {
        Application::getInstance()->applicationWillEnterForeground();
        ax::EventCustom foregroundEvent(EVENT_COME_TO_FOREGROUND);
        ax::Director::getInstance()->getEventDispatcher()->dispatchEvent(&foregroundEvent, true);
    }
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeInsertText(JNIEnv* env, jclass, jstring text)
{
    std::string strValue = ax::text_utils::getStringUTFCharsJNI(env, text);
    ax::IMEDispatcher::sharedDispatcher()->dispatchInsertText(strValue.c_str(), strValue.size());
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeDeleteBackward(JNIEnv*, jclass, jint numChars)
{
    ax::IMEDispatcher::sharedDispatcher()->dispatchDeleteBackward(numChars);
}

JNIEXPORT jstring JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeGetContentText(JNIEnv* env, jclass)
{
    auto pszText = ax::IMEDispatcher::sharedDispatcher()->getContentText();
    return ax::text_utils::newStringUTFJNI(env, pszText);
}
}
