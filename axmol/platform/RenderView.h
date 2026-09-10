/****************************************************************************
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once
#include "axmol/platform/PlatformConfig.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_IOS
#    include "axmol/platform/ios/RenderView-ios.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
#    include "axmol/platform/android/RenderView-android.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_WINRT
#    include "axmol/platform/winrt/RenderView-winrt.h"
#else
#    include "axmol/platform/pc/RenderView-pc.h"
#endif
