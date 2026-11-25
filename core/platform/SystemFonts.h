/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "platform/PlatformConfig.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
#    include "platform/android/SystemFonts-android.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
#    include "platform/linux/SystemFonts-linux.h"
#else

#include <string_view>
#include <string>

namespace ax
{
  struct SystemFontRequest;
  struct SystemFontResult;

  class SystemFonts
  {
  public:
    static SystemFonts& getInstance();
    SystemFontResult findFont(const SystemFontRequest&);
  };
}

#endif
