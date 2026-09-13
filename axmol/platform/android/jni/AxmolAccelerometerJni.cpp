/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include <jni.h>
#include "axmol/base/Director.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/AccelerationEvent.h"

#define TG3_GRAVITY_EARTH (9.80665f)

using namespace ax;

extern "C" {
JNIEXPORT void JNICALL
Java_dev_axmol_lib_AxmolAccelerometer_onSensorChanged(JNIEnv*, jclass, jfloat x, jfloat y, jfloat z, jlong timeStamp)
{
    Acceleration a;
    a.x         = -((double)x / TG3_GRAVITY_EARTH);
    a.y         = -((double)y / TG3_GRAVITY_EARTH);
    a.z         = -((double)z / TG3_GRAVITY_EARTH);
    a.timestamp = (double)timeStamp / 1e9;

    AccelerationEvent event(a);
    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}
}
