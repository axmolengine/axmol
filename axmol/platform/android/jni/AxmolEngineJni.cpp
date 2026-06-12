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
#include "axmol/platform/android/jni/AxmolEngineJni.h"
#include <stdlib.h>
#include <jni.h>
#include <android/log.h>
#include <string>
#include "axmol/platform/android/jni/JniHelper.h"
#include "axmol/platform/android/FileUtils-android.h"
#include "axmol/platform/Application.h"
#include "axmol/rhi/DriverContext.h"
#include "axmol/base/EventType.h"
#include "axmol/base/CustomEvent.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/text_utils.h"

#define LOG_TAG   "Java_dev_axmol_lib_AxmolEngine.cpp"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

static const char* className = "dev.axmol.lib.AxmolEngine";

static EditTextCallback s_editTextCallback = nullptr;
static void* s_ctx                         = nullptr;

static std::string s_apkPath;

extern void _axmolPerformFrameBoundaryTasks();

extern "C" {

// This is the first call from java after JNI_OnLoad
JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolEngine_nativeInit(JNIEnv* env,
                                                                 jclass,
                                                                 jobject context,
                                                                 jobject assetManager)
{
    ax::JniHelper::setClassLoaderFrom(context);
    ax::FileUtilsAndroid::setAssetManagerFromJava(assetManager);

    auto app = ax::Application::getInstance();
    app->initContextAttrs();

    ax::rhi::DriverContext::makeCurrentDriver();
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolEngine_nativeSetEditTextDialogResult(JNIEnv* env,
                                                                                    jclass,
                                                                                    jbyteArray text)
{
    jsize size = env->GetArrayLength(text);

    if (size > 0)
    {
        jbyte* data  = (jbyte*)env->GetByteArrayElements(text, 0);
        char* buffer = (char*)malloc(size + 1);
        if (buffer != nullptr)
        {
            memcpy(buffer, data, size);
            buffer[size] = '\0';
            // pass data to edittext's delegate
            if (s_editTextCallback)
                s_editTextCallback(buffer, s_ctx);
            free(buffer);
        }
        env->ReleaseByteArrayElements(text, data, 0);
    }
    else
    {
        if (s_editTextCallback)
            s_editTextCallback("", s_ctx);
    }
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolEngine_nativePerformFrameBoundaryTasks(JNIEnv* env, jclass)
{
    _axmolPerformFrameBoundaryTasks();
}

JNIEXPORT int JNICALL Java_dev_axmol_lib_AxmolEngine_nativeGetRenderAPI(JNIEnv* env, jclass)
{
    return (int)ax::rhi::DriverContext::currentDriverType();
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolEngine_nativeRunOnAxmolThread(JNIEnv* env, jclass, jobject runnable)
{
    using jobject_type = std::remove_pointer_t<jobject>;
    struct jobject_delete
    {
        void operator()(jobject_type* __ptr) const _NOEXCEPT { ax::JniHelper::getEnv()->DeleteGlobalRef(__ptr); }
    };

    ax::Director::getInstance()->postTask(
        [wrap = std::make_shared<std::unique_ptr<jobject_type, jobject_delete>>(env->NewGlobalRef(runnable))] {
        auto curEnv = ax::JniHelper::getEnv();

        ax::JniMethodInfo mi;
        if (ax::JniHelper::getMethodInfo(mi, "java/lang/Runnable", "run", "()V"))
        {
            curEnv->CallVoidMethod(wrap.get()->get(), mi.methodID);
        }
    });
}

JNIEXPORT jintArray JNICALL Java_dev_axmol_lib_AxmolEngine_nativeGetGLContextAttrs(JNIEnv* env, jclass)
{
    auto app                 = ax::Application::getInstance();
    const auto& contextAttrs = ax::Application::getContextAttrs();

    int tmp[7] = {contextAttrs.redBits,           contextAttrs.greenBits, contextAttrs.blueBits,
                  contextAttrs.alphaBits,         contextAttrs.depthBits, contextAttrs.stencilBits,
                  contextAttrs.multisamplingCount};

    jintArray glContextAttrsJava = env->NewIntArray(7);
    env->SetIntArrayRegion(glContextAttrsJava, 0, 7, tmp);

    return glContextAttrsJava;
}
}

namespace ax
{
const char* getApkPath()
{
    if (s_apkPath.empty())
    {
        s_apkPath = JniHelper::callStaticStringMethod(className, "getAssetsPath");
    }

    return s_apkPath.c_str();
}

std::string getPackageNameJNI()
{
    return JniHelper::callStaticStringMethod(className, "getPackageName");
}

int getObbAssetFileDescriptorJNI(const char* path, int64_t* startOffset, int64_t* size)
{
    JniMethodInfo methodInfo;
    int fd = 0;

    if (JniHelper::getStaticMethodInfo(methodInfo, className, "getObbAssetFileDescriptor", "(Ljava/lang/String;)[J"))
    {
        jstring stringArg = methodInfo.env->NewStringUTF(path);
        jlongArray newArray =
            (jlongArray)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, stringArg);
        jsize theArrayLen = methodInfo.env->GetArrayLength(newArray);

        if (theArrayLen == 3)
        {
            jboolean copy = JNI_FALSE;
            jlong* array  = methodInfo.env->GetLongArrayElements(newArray, &copy);
            fd            = static_cast<int>(array[0]);
            *startOffset  = array[1];
            *size         = array[2];
            methodInfo.env->ReleaseLongArrayElements(newArray, array, 0);
        }

        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(stringArg);
    }

    return fd;
}

void conversionEncodingJNI(const char* src, int byteSize, const char* fromCharset, char* dst, const char* newCharset)
{
    JniMethodInfo methodInfo;

    if (JniHelper::getStaticMethodInfo(methodInfo, className, "conversionEncoding",
                                       "([BLjava/lang/String;Ljava/lang/String;)[B"))
    {
        jbyteArray strArray = methodInfo.env->NewByteArray(byteSize);
        methodInfo.env->SetByteArrayRegion(strArray, 0, byteSize, reinterpret_cast<const jbyte*>(src));

        jstring stringArg1 = methodInfo.env->NewStringUTF(fromCharset);
        jstring stringArg2 = methodInfo.env->NewStringUTF(newCharset);

        jbyteArray newArray = (jbyteArray)methodInfo.env->CallStaticObjectMethod(
            methodInfo.classID, methodInfo.methodID, strArray, stringArg1, stringArg2);
        jsize theArrayLen = methodInfo.env->GetArrayLength(newArray);
        methodInfo.env->GetByteArrayRegion(newArray, 0, theArrayLen, (jbyte*)dst);

        methodInfo.env->DeleteLocalRef(strArray);
        methodInfo.env->DeleteLocalRef(stringArg1);
        methodInfo.env->DeleteLocalRef(stringArg2);
        methodInfo.env->DeleteLocalRef(newArray);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

}  // namespace ax

#undef LOGD
#undef LOG_TAG
