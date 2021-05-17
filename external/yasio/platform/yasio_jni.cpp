//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any 
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2021 HALX99

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef YASIO__JNI_CPP
#define YASIO__JNI_CPP

/*
** yasio_jni.cpp: The yasio native interface for initialize c-ares on android.
org/yasio/AppGlobals.java provide 2 function:
 1. getApplicationContext(), for native only
 2. init(Context ctx), please call after your android app initialized.
*/
#include "yasio/detail/config.hpp"
#include "yasio/detail/logging.hpp"

#if defined(__ANDROID__) && defined(YASIO_HAVE_CARES)
static JavaVM* yasio__jvm;
static jclass yasio__appglobals_cls;
static jobject yasio__get_app_context(JNIEnv* env, jclass obj_cls, const char* funcName,
                                      const char* signature)
{
  jobject app_context = nullptr;
  do
  {
    if (obj_cls == nullptr)
      break;
    jmethodID android_get_app_mid = env->GetStaticMethodID(obj_cls, funcName, signature);
    if (android_get_app_mid == nullptr)
      break;
    app_context = env->CallStaticObjectMethod(obj_cls, android_get_app_mid);
  } while (false);

  if (env->ExceptionOccurred())
    env->ExceptionClear();

  return app_context;
}

static jobject yasio__get_app_context(JNIEnv* env, const char* className, const char* funcName,
                                      const char* signature)
{
  jobject app_context = nullptr;
  jclass obj_cls      = env->FindClass(className);
  if (obj_cls != nullptr)
  {
    app_context = yasio__get_app_context(env, obj_cls, funcName, signature);
    env->DeleteLocalRef(obj_cls);
  }
  if (env->ExceptionOccurred())
    env->ExceptionClear();
  return app_context;
}

extern "C" {
#  include "ares.h"

int yasio__ares_init_android()
{
  JNIEnv* env         = nullptr;
  int ret             = ARES_ENOTINITIALIZED;
  bool need_detatch   = false;
  jclass obj_cls      = nullptr;
  jobject app_context = nullptr;

  do
  {
    if (yasio__jvm == nullptr)
      break;

    jint res = yasio__jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (res == JNI_EDETACHED)
    {
      env          = nullptr;
      res          = yasio__jvm->AttachCurrentThread(&env, nullptr);
      need_detatch = true;
    }
    if (res != JNI_OK)
      break; // Get env failed

    /// Gets application context
    app_context = yasio__get_app_context(env, "android/app/AppGlobals", "getInitialApplication",
                                         "()Landroid/app/Application;");
    if (app_context == nullptr)
    {
      app_context = yasio__get_app_context(env, yasio__appglobals_cls, "getApplicationContext",
                                           "()Landroid/content/Context;");
      if (app_context == nullptr)
        break;
    }

    /// Gets connectivity_manager
    obj_cls = (env)->FindClass("android/content/Context");
    if (obj_cls == nullptr)
      break;
    jmethodID android_get_service_mid =
        (env)->GetMethodID(obj_cls, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    if (android_get_service_mid == nullptr)
      break;
    jfieldID cs_fid = env->GetStaticFieldID(obj_cls, "CONNECTIVITY_SERVICE", "Ljava/lang/String;");
    if (cs_fid == nullptr)
      break;
    jstring jstr_service_name = (jstring)env->GetStaticObjectField(obj_cls, cs_fid);
    if (jstr_service_name == nullptr)
      break;
    jobject connectivity_manager =
        env->CallObjectMethod(app_context, android_get_service_mid, jstr_service_name);
    if (connectivity_manager != nullptr)
    {
      ret = ::ares_library_init_android(connectivity_manager);
      env->DeleteLocalRef(connectivity_manager);
    }
    else
      YASIO_LOG(
          "[c-ares] Gets ConnectivityManager service failed, please ensure you have permission "
          "'ACCESS_NETWORK_STATE'");

    env->DeleteLocalRef(jstr_service_name);
    env->DeleteLocalRef(app_context);
    env->DeleteLocalRef(obj_cls);
    app_context = nullptr;
    obj_cls     = nullptr;
  } while (false);

  if (env != nullptr)
  {
    if (env->ExceptionOccurred())
      env->ExceptionClear();

    if (app_context != nullptr)
      env->DeleteLocalRef(app_context);
    if (obj_cls != nullptr)
      env->DeleteLocalRef(obj_cls);
  }

  if (need_detatch)
    yasio__jvm->DetachCurrentThread();

  return ret;
}

void yasio__jni_init(void* vm, void* env)
{
  yasio__jvm   = (JavaVM*)vm;
  JNIEnv* jenv = (JNIEnv*)env;
  if (jenv != nullptr)
  {
    // must find class at here,
    // see: https://developer.android.com/training/articles/perf-jni#faq_FindClass
    jclass obj_cls = jenv->FindClass("org/yasio/AppGlobals");
    if (obj_cls != nullptr)
    {
      yasio__appglobals_cls = (jclass)jenv->NewGlobalRef(obj_cls);
      jenv->DeleteLocalRef(obj_cls);
    }

    if (jenv->ExceptionOccurred())
      jenv->ExceptionClear();
  }
  ::ares_library_init_jvm(yasio__jvm);
}

int yasio__jni_onload(void* vm, void* /*reserved*/)
{
  JNIEnv* env = nullptr;
  jint res    = ((JavaVM*)vm)->GetEnv((void**)&env, JNI_VERSION_1_6);
  if (res == JNI_OK)
    yasio__jni_init(vm, env);

  return JNI_VERSION_1_6;
}
#  if defined(YASIO_BUILD_AS_SHARED) && !defined(YASIO_NO_JNI_ONLOAD)
jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) { return yasio__jni_onload(vm, reserved); }
#  endif
}
#endif

#endif
