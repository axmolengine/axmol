/****************************************************************************
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#include "axmol/platform/android/Application-android.h"
#include "axmol/platform/android/RenderView-android.h"
#include "axmol/base/Director.h"
#include "axmol/base/CustomEvent.h"
#include "axmol/base/EventType.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/renderer/TextureCache.h"
#include "axmol/platform/android/jni/JniHelper.h"

#include <android/log.h>
#include <android/api-level.h>
#include <jni.h>

#define LOG_TAG   "main"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

void axmol_android_app_init(JNIEnv* env) __attribute__((weak));

using namespace ax;

extern "C" {

// ndk break compatibility, refer to https://github.com/cocos2d/cocos2d-x/issues/16267 for detail information
// should remove it when using NDK r13 since NDK r13 will add back bsd_signal()
#if __ANDROID_API__ > 19
#    include <signal.h>
#    include <dlfcn.h>
typedef __sighandler_t (*bsd_signal_func_t)(int, __sighandler_t);
bsd_signal_func_t bsd_signal_func = NULL;

__sighandler_t bsd_signal(int s, __sighandler_t f)
{
    if (bsd_signal_func == NULL)
    {
        // For now (up to Android 7.0) this is always available
        bsd_signal_func = (bsd_signal_func_t)dlsym(RTLD_DEFAULT, "bsd_signal");

        if (bsd_signal_func == NULL)
        {
            __android_log_assert("", "bsd_signal_wrapper", "bsd_signal symbol not found!");
        }
    }
    return bsd_signal_func(s, f);
}
#endif  // __ANDROID_API__ > 19

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JniHelper::setJavaVM(vm);

    axmol_android_app_init(JniHelper::getEnv());

    return JNI_VERSION_1_4;
}
}
#undef LOGD
#undef LOG_TAG
