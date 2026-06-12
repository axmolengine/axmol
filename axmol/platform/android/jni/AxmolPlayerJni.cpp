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
#include "axmol/base/InputSystem.h"
#include "axmol/base/Director.h"
#include "axmol/base/Scheduler.h"
#include "axmol/base/EventType.h"
#include "axmol/base/CustomEvent.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/platform/Application.h"
#include "axmol/platform/android/RenderView-android.h"
#include "axmol/base/text_utils.h"
#include "axmol/platform/android/jni/JniHelper.h"
#include "axmol/rhi/DriverContext.h"
#include "axmol/renderer/TextureCache.h"
#include "axmol/tlx/static_vector.hpp"

#include <android/log.h>
#include <android/native_window_jni.h>

using namespace ax;

static ANativeWindow* s_nativeWindow;

static constexpr int AX_MAX_TOUCHES = 10;

ANativeWindow* axmolGetANativeWindow()
{
    return s_nativeWindow;
}

static void axmolDispatchContextLost(bool isWarmStart)
{
#if AX_ENABLE_RESTART_APPLICATION_ON_CONTEXT_LOST
    auto director = ax::Director::getInstance();
    ax::CustomEvent recreatedEvent(EVENT_APP_RESTARTING);
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
        ax::CustomEvent warmStartEvent(EVENT_APP_WARM_START);
        director->getEventDispatcher()->dispatchEvent(&warmStartEvent, true);
    }
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

static std::unordered_map<int, ax::KeyboardEvent::KeyCode> g_keyCodeMap = {
    {KEYCODE_BACK, ax::KeyboardEvent::KeyCode::KEY_ESCAPE},
    {KEYCODE_MENU, ax::KeyboardEvent::KeyCode::KEY_MENU},
    {KEYCODE_DPAD_UP, ax::KeyboardEvent::KeyCode::KEY_DPAD_UP},
    {KEYCODE_DPAD_DOWN, ax::KeyboardEvent::KeyCode::KEY_DPAD_DOWN},
    {KEYCODE_DPAD_LEFT, ax::KeyboardEvent::KeyCode::KEY_DPAD_LEFT},
    {KEYCODE_DPAD_RIGHT, ax::KeyboardEvent::KeyCode::KEY_DPAD_RIGHT},
    {KEYCODE_ENTER, ax::KeyboardEvent::KeyCode::KEY_ENTER},
    {KEYCODE_PLAY, ax::KeyboardEvent::KeyCode::KEY_PLAY},
    {KEYCODE_DPAD_CENTER, ax::KeyboardEvent::KeyCode::KEY_DPAD_CENTER},

};

struct TouchPoint
{
    intptr_t id;
    float x;
    float y;
    float pressure;
};

extern "C" {

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeOnSurfaceCreated(JNIEnv* env,
                                                                             jclass,
                                                                             jobject surface,
                                                                             jint w,
                                                                             jint h,
                                                                             jboolean isWarmStart)
{
    if (s_nativeWindow)
        ANativeWindow_release(s_nativeWindow);

    s_nativeWindow = ANativeWindow_fromSurface(env, surface);
    if (s_nativeWindow == nullptr)
    {
        AXLOGW(
            "ANativeWindow_fromSurface failed: surface={}, "
            "windowSize=({} x {}), threadId={}",
            fmt::ptr(surface), static_cast<int>(w), static_cast<int>(h), (long)gettid());
        return;
    }

    AXLOGI(
        "ANativeWindow_fromSurface success: window={}, "
        "size=({} x {})",
        fmt::ptr(s_nativeWindow), static_cast<int>(w), static_cast<int>(h));

    auto director   = ax::Director::getInstance();
    auto renderView = director->getRenderView();
    if (!renderView)
    {
        renderView = ax::RenderView::createWithRect("axmol3",
                                                    Rect{ax::Rect{0, 0, static_cast<float>(w), static_cast<float>(h)}});
        director->setRenderView(renderView);

        auto axmolApp = ax::ApplicationCore::getInstance();
        axmolApp->run();
    }
    else
    {
        if (rhi::DriverContext::isVulkan())
        {
            static_cast<ax::RenderView*>(renderView)->recreateVkSurface(true);
        }
        else
        {
            axdrv->resetState();
            director->resetMatrixStack();
            ax::CustomEvent recreatedEvent(EVENT_RENDERER_RECREATED);
            director->getEventDispatcher()->dispatchEvent(&recreatedEvent, true);
            director->setRenderDefaults();
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
            ax::VolatileTextureMgr::reloadAllTextures();
            axmolDispatchContextLost(isWarmStart);
#endif
        }
    }
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeOnSurfaceChanged(JNIEnv*, jclass, jint w, jint h)
{
    auto director   = ax::Director::getInstance();
    auto renderView = director->getRenderView();
    if (renderView)
    {
        renderView->updateRenderSurface(w, h, ax::RenderView::AllUpdates);
    }
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeRenderFrame(JNIEnv*, jclass)
{
    ax::Director::getInstance()->renderFrame();
}

JNIEXPORT void JNICALL
Java_dev_axmol_lib_AxmolPlayer_nativeTouchBegin(JNIEnv* env, jclass, jint id, jfloat x, jfloat y, jfloat pressure)
{
    auto director = ax::Director::getInstance();

    director->postTask(
        [pos = Vec2{x, y}, state = ax::PointerInputState{.id       = static_cast<intptr_t>(id),
                                                         .pressure = static_cast<float>(pressure),
                                                         .type     = ax::PointerType::Touch}]() {
        ax::InputSystem::getInstance()->handlePointerDown(pos, state);
    },
        Director::TaskTiming::FrameBoundary);
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeOnWindowFocusChanged(JNIEnv* env,
                                                                                 jobject thiz,
                                                                                 jboolean has_focus)
{
    if (!has_focus)
    {
        ax::InputSystem::getInstance()->resetInput();
    }
}

JNIEXPORT void JNICALL
Java_dev_axmol_lib_AxmolPlayer_nativeTouchEnd(JNIEnv* env, jclass, jint id, jfloat x, jfloat y, jfloat pressure)
{
    auto director = ax::Director::getInstance();

    director->postTask(
        [pos = Vec2{x, y}, state = ax::PointerInputState{.id       = static_cast<intptr_t>(id),
                                                         .pressure = static_cast<float>(pressure),
                                                         .type     = ax::PointerType::Touch}]() {
        ax::InputSystem::getInstance()->handlePointerUp(pos, state);
    },
        Director::TaskTiming::FrameBoundary);
}

// ==============================================================================
// 2. Multi-Pointer Events (Zero-Copy)
// ==============================================================================
JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeTouchesMove(JNIEnv* env,
                                                                        jclass,
                                                                        jintArray jIds,
                                                                        jfloatArray jXs,
                                                                        jfloatArray jYs,
                                                                        jfloatArray jPressures,
                                                                        jint size)
{
    if (size <= 0)
        return;

    jint* ids         = (jint*)env->GetPrimitiveArrayCritical(jIds, nullptr);
    jfloat* xs        = (jfloat*)env->GetPrimitiveArrayCritical(jXs, nullptr);
    jfloat* ys        = (jfloat*)env->GetPrimitiveArrayCritical(jYs, nullptr);
    jfloat* pressures = (jfloat*)env->GetPrimitiveArrayCritical(jPressures, nullptr);

    size = std::min(size, AX_MAX_TOUCHES);

    tlx::static_vector<TouchPoint, AX_MAX_TOUCHES> touchPoints;
    if (ids && xs && ys && pressures)
    {
        for (int i = 0; i < size; ++i)
            touchPoints.push_back(
                {.id = static_cast<intptr_t>(ids[i]), .x = xs[i], .y = ys[i], .pressure = pressures[i]});
    }

    if (pressures)
        env->ReleasePrimitiveArrayCritical(jPressures, pressures, JNI_ABORT);
    if (ys)
        env->ReleasePrimitiveArrayCritical(jYs, ys, JNI_ABORT);
    if (xs)
        env->ReleasePrimitiveArrayCritical(jXs, xs, JNI_ABORT);
    if (ids)
        env->ReleasePrimitiveArrayCritical(jIds, ids, JNI_ABORT);

    ax::Director::getInstance()->postTask([touchPoints = std::move(touchPoints)]() {
        auto inputSys = ax::InputSystem::getInstance();
        for (auto& touchPoint : touchPoints)
        {
            auto state = ax::PointerInputState{
                .id = touchPoint.id, .pressure = touchPoint.pressure, .type = ax::PointerType::Touch};
            inputSys->handlePointerMove(Vec2(touchPoint.x, touchPoint.y), state);
        }
    }, Director::TaskTiming::FrameBoundary);
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeTouchesCancel(JNIEnv* env,
                                                                          jclass,
                                                                          jintArray jIds,
                                                                          jfloatArray jXs,
                                                                          jfloatArray jYs,
                                                                          jfloatArray jPressures,
                                                                          jint size)
{
    if (size <= 0)
        return;

    jint* ids         = (jint*)env->GetPrimitiveArrayCritical(jIds, nullptr);
    jfloat* xs        = (jfloat*)env->GetPrimitiveArrayCritical(jXs, nullptr);
    jfloat* ys        = (jfloat*)env->GetPrimitiveArrayCritical(jYs, nullptr);
    jfloat* pressures = (jfloat*)env->GetPrimitiveArrayCritical(jPressures, nullptr);

    size = std::min(size, AX_MAX_TOUCHES);

    tlx::static_vector<TouchPoint, AX_MAX_TOUCHES> touchPoints;
    if (ids && xs && ys && pressures)
    {
        for (int i = 0; i < size; ++i)
            touchPoints.push_back(
                {.id = static_cast<intptr_t>(ids[i]), .x = xs[i], .y = ys[i], .pressure = pressures[i]});
    }

    if (pressures)
        env->ReleasePrimitiveArrayCritical(jPressures, pressures, JNI_ABORT);
    if (ys)
        env->ReleasePrimitiveArrayCritical(jYs, ys, JNI_ABORT);
    if (xs)
        env->ReleasePrimitiveArrayCritical(jXs, xs, JNI_ABORT);
    if (ids)
        env->ReleasePrimitiveArrayCritical(jIds, ids, JNI_ABORT);

    ax::Director::getInstance()->postTask([touchPoints = std::move(touchPoints)]() {
        auto inputSys = ax::InputSystem::getInstance();
        for (auto& touchPoint : touchPoints)
        {
            auto state = ax::PointerInputState{
                .id = touchPoint.id, .pressure = touchPoint.pressure, .type = ax::PointerType::Touch};
            inputSys->handlePointerCancel(Vec2(touchPoint.x, touchPoint.y), state);
        }
    }, Director::TaskTiming::FrameBoundary);
}

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

    ax::InputSystem::getInstance()->handleKeyEvent(iterKeyCode->second,
                                                   isPressed ? InputPhase::KeyDown : InputPhase::KeyUp);
    return JNI_TRUE;
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeOnPause(JNIEnv*, jclass)
{
    if (Director::getInstance()->getRenderView())
    {
        Application::getInstance()->applicationDidEnterBackground();
        ax::CustomEvent backgroundEvent(EVENT_COME_TO_BACKGROUND);
        ax::Director::getInstance()->getEventDispatcher()->dispatchEvent(&backgroundEvent, true);
    }
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeOnResume(JNIEnv*, jclass)
{
    if (Director::getInstance()->getRenderView())
    {
        Application::getInstance()->applicationWillEnterForeground();
        ax::CustomEvent foregroundEvent(EVENT_COME_TO_FOREGROUND);
        ax::Director::getInstance()->getEventDispatcher()->dispatchEvent(&foregroundEvent, true);
    }
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeInsertText(JNIEnv* env, jclass, jstring text)
{
    std::string strValue = ax::text_utils::getStringUTFCharsJNI(env, text);
    ax::InputSystem::getInstance()->dispatchInsertText(strValue);
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeDeleteBackward(JNIEnv*, jclass, jint numChars)
{
    ax::InputSystem::getInstance()->dispatchDeleteBackward(static_cast<unsigned int>(numChars));
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeSoftInputShow(JNIEnv* env,
                                                                          jclass,
                                                                          jfloat x,
                                                                          jfloat y,
                                                                          jfloat width,
                                                                          jfloat height,
                                                                          jfloat duration)
{
    if (auto inputSys = ax::InputSystem::getInstance())
    {
        inputSys->onPlatformKeyboardWillShow(x, y, width, height, duration);
        inputSys->onPlatformKeyboardDidShow();
    }
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeSoftInputHide(JNIEnv*, jclass, float duration)
{
    if (auto inputSys = ax::InputSystem::getInstance())
    {
        inputSys->onPlatformKeyboardWillHide(duration);
        inputSys->onPlatformKeyboardDidHide();
    }
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativePerformEditAction(JNIEnv*, jclass, int action)
{
    ax::InputSystem::getInstance()->dispatchPerformEditAction(static_cast<ax::EditAction>(action));
}
}
