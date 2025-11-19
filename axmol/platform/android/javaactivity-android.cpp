/****************************************************************************
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
#include "axmol/platform/android/Application-android.h"
#include "axmol/platform/android/RenderViewImpl-android.h"
#include "axmol/base/Director.h"
#include "axmol/base/EventCustom.h"
#include "axmol/base/EventType.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/renderer/TextureCache.h"
#include "axmol/platform/android/jni/JniHelper.h"
#include "axmol/rhi/opengl/DriverGL.h"

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

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolPlayer_nativeOnSurfaceChanged(JNIEnv*, jclass, jint w, jint h)
{
    auto renderView = ax::Director::getInstance()->getRenderView();
    if (renderView)
        renderView->updateRenderSurface(w, h, ax::RenderView::AllUpdates);
}
}
#undef LOGD
#undef LOG_TAG
