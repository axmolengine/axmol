/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

/// @cond DO_NOT_SHOW

#include "axmol/platform/PlatformConfig.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_MAC
#    include "axmol/platform/mac/Application-mac.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_IOS
#    include "axmol/platform/ios/Application-ios.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
#    include "axmol/platform/android/Application-android.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
#    include "axmol/platform/win32/Application-win32.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_WINRT
#    include "axmol/platform/winrt/Application-winrt.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
#    include "axmol/platform/linux/Application-linux.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_WASM
#    include "axmol/platform/wasm/Application-wasm.h"
#endif
