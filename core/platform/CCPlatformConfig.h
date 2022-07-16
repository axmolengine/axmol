/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2017 Chukong Technologies
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2021 Bytedance Inc.

https://axis-project.github.io/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __BASE_AX_PLATFORM_CONFIG_H__
#define __BASE_AX_PLATFORM_CONFIG_H__
/// @cond DO_NOT_SHOW

/**
  Config of cocos2d-x project, per target platform.

  THIS FILE MUST NOT INCLUDE ANY OTHER FILE
*/

//////////////////////////////////////////////////////////////////////////
// pre configure
//////////////////////////////////////////////////////////////////////////

// define supported target platform macro which CC uses.
#define AX_PLATFORM_UNKNOWN 0
#define AX_PLATFORM_IOS 1
#define AX_PLATFORM_ANDROID 2
#define AX_PLATFORM_WIN32 3
// #define AX_PLATFORM_MARMALADE          4
#define AX_PLATFORM_LINUX 5
// #define AX_PLATFORM_BADA               6
// #define AX_PLATFORM_BLACKBERRY         7
#define AX_PLATFORM_MAC 8
// #define AX_PLATFORM_NACL               9
// #define AX_PLATFORM_EMSCRIPTEN        10
// #define AX_PLATFORM_TIZEN             11
// #define AX_PLATFORM_QT5               12
// #define AX_PLATFORM_WINRT             13

// Determine target platform by compile environment macro.
#define AX_TARGET_PLATFORM AX_PLATFORM_UNKNOWN

// Apple: Mac and iOS
#if defined(__APPLE__) && !defined(__ANDROID__)  // exclude android for binding generator.
#    include <TargetConditionals.h>
#    if TARGET_OS_IPHONE  // TARGET_OS_IPHONE includes TARGET_OS_IOS TARGET_OS_TV and TARGET_OS_WATCH. see
                          // TargetConditionals.h
#        undef AX_TARGET_PLATFORM
#        define AX_TARGET_PLATFORM AX_PLATFORM_IOS
#    elif TARGET_OS_MAC
#        undef AX_TARGET_PLATFORM
#        define AX_TARGET_PLATFORM AX_PLATFORM_MAC
#    endif
#endif

// android
#if defined(__ANDROID__)
#    undef AX_TARGET_PLATFORM
#    define AX_TARGET_PLATFORM AX_PLATFORM_ANDROID
#endif

// win32
#if defined(_WIN32) && defined(_WINDOWS)
#    undef AX_TARGET_PLATFORM
#    define AX_TARGET_PLATFORM AX_PLATFORM_WIN32
#endif

// linux
#if defined(LINUX) && !defined(__APPLE__)
#    undef AX_TARGET_PLATFORM
#    define AX_TARGET_PLATFORM AX_PLATFORM_LINUX
#endif

//////////////////////////////////////////////////////////////////////////
// post configure
//////////////////////////////////////////////////////////////////////////

// check user set platform
#if !AX_TARGET_PLATFORM
#    error "Cannot recognize the target platform; are you targeting an unsupported platform?"
#endif

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
#    ifndef __MINGW32__
#        pragma warning(disable : 4127)
#    endif
#endif  // AX_PLATFORM_WIN32

/*
windows: https://github.com/google/angle
mac: GL
iOS: GLES
other: GL
*/
#ifndef AX_USE_COMPAT_GL
#    define AX_USE_COMPAT_GL 0
#endif

#if ((AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) || (AX_TARGET_PLATFORM == AX_PLATFORM_IOS))
#    define AX_PLATFORM_MOBILE
#else
#    define AX_PLATFORM_PC
#endif

#if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
#    if !AX_USE_COMPAT_GL
#        define AX_USE_METAL
#    else
#        define AX_USE_GL
#    endif
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
#    if !AX_USE_COMPAT_GL
#        define AX_USE_METAL
#    else
#        define AX_USE_GLES
#    endif
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
#    define AX_USE_GLES
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
#    if !AX_USE_COMPAT_GL
#        define AX_USE_GL
#    else
#        define AX_USE_GLES
#    endif
#else
#    define AX_USE_GL
#endif

/// @endcond
#endif  // __BASE_AX_PLATFORM_CONFIG_H__
