/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include <memory>

#include <android/log.h>
#include <jni.h>

#include "AppDelegate.h"

#define LOG_TAG   "main"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

namespace
{
std::unique_ptr<AppDelegate> appDelegate;
}

void axmol_android_app_init(JNIEnv* env)
{
    LOGD("axmol_android_app_init");
    appDelegate.reset(new AppDelegate());
}
