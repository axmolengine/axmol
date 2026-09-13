/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "axmol/platform/PlatformConfig.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)

#    include "axmol/ui/WebView/WebViewImpl-android.h"
#    include "axmol/ui/WebView/WebView-inl.h"

#elif (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)

#    include "axmol/ui/WebView/WebViewImpl-win32.h"
#    include "axmol/ui/WebView/WebView-inl.h"

#elif (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX) && defined(AX_HAVE_WEBKIT2GTK)

#    include "axmol/ui/WebView/WebViewImpl-linux.h"
#    include "axmol/ui/WebView/WebView-inl.h"

#endif
