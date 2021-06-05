#ifndef OPENSSLCONF_REDIRECT_H
#define OPENSSLCONF_REDIRECT_H

#if defined(_M_X64) || defined(_WIN64) || defined(__LP64__) || defined(_LP64) || defined(__x86_64) || defined(__arm64__) || defined(__aarch64__)
#  define OPENSSLCONF_REDIRECT_64BIT 1
#endif

#if defined(_WIN32)
#  include "win32/openssl/opensslconf.h"

#elif defined(__APPLE__)
#  include <TargetConditionals.h>

#  if TARGET_IPHONE_SIMULATOR == 1
#    include "ios-x64/openssl/opensslconf.h"
#  elif TARGET_OS_IPHONE == 1
#    if defined(__arm64__)
#      include "ios-arm64/openssl/opensslconf.h"
#    elif defined(__arm__)
#      include "ios-arm/openssl/opensslconf.h"
#    endif
#  elif TARGET_OS_MAC == 1
#    include "mac/openssl/opensslconf.h"
#  endif

#elif defined(__ANDROID__)

#  if defined(__aarch64__) || defined(__arm64__)
#    include "android-arm64/openssl/opensslconf.h"
#  elif defined(__arm__)
#    include "android-arm/openssl/opensslconf.h"
#  else
#    include "android-x86/openssl/opensslconf.h"
#  endif

#elif defined(__linux__)
#  include "linux/openssl/opensslconf.h"
#else
#  error "unsupported platform"
#endif

#endif
