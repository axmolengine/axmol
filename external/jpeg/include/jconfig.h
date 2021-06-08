#ifndef BUILDWARE_jpeg-turbo_REDIRECT_H
#define BUILDWARE_jpeg-turbo_REDIRECT_H

#if defined(_WIN32)
#  include "win32/jconfig.h"

#elif defined(__APPLE__)
#  include <TargetConditionals.h>

#  if TARGET_IPHONE_SIMULATOR == 1
#    include "ios-x64/jconfig.h"
#  elif TARGET_OS_IPHONE == 1
#    if defined(__arm64__)
#      include "ios-arm64/jconfig.h"
#    elif defined(__arm__)
#      include "ios-arm/jconfig.h"
#    endif
#  elif TARGET_OS_MAC == 1
#    include "mac/jconfig.h"
#  endif

#elif defined(__ANDROID__)

#  if defined(__aarch64__) || defined(__arm64__)
#    include "android-arm64/jconfig.h"
#  elif defined(__arm__)
#    include "android-arm/jconfig.h"
#  else
#    include "android-x86/jconfig.h"
#  endif

#elif defined(__linux__)
#  include "linux/jconfig.h"
#else
#  error "unsupported platform"
#endif

#endif
