/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#include "axmol/platform/Common.h"
#include "axmol/platform/android/jni/JniHelper.h"
#include <android/log.h>
#include <stdio.h>
#include <jni.h>

namespace ax
{

AlertResult showAlert(std::string_view msg, std::string_view title, AlertStyle)
{
    JniHelper::callStaticVoidMethod("dev.axmol.lib.AxmolEngine", "showDialog", msg, title);
    return AlertResult::None;
}

}  // namespace ax
