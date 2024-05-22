/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2017 Chukong Technologies
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

https://axmolengine.github.io/

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

#if __has_include(<winapifamily.h>)
#    include <winapifamily.h>
#endif

/**
  Config of cocos2d-x project, per target platform.

  THIS FILE MUST NOT INCLUDE ANY OTHER FILE
*/

//////////////////////////////////////////////////////////////////////////
// pre configure
//////////////////////////////////////////////////////////////////////////

// define supported target platform macro which CC uses.
#define AX_PLATFORM_UNKNOWN 0
#define AX_PLATFORM_WIN32   1
#define AX_PLATFORM_WINUWP  2
#define AX_PLATFORM_LINUX   3
#define AX_PLATFORM_OSX     4
#define AX_PLATFORM_ANDROID 5
#define AX_PLATFORM_IOS     6
#define AX_PLATFORM_TVOS    7
#define AX_PLATFORM_WASM    8

// alias platform macros
#define AX_PLATFORM_WINRT      AX_PLATFORM_WINUWP
#define AX_PLATFORM_MAC        AX_PLATFORM_OSX
#define AX_PLATFORM_EMSCRIPTEN AX_PLATFORM_WASM

// Determine target platform by compile environment macro.
#define AX_TARGET_PLATFORM AX_PLATFORM_UNKNOWN

// Apple: Mac and iOS
#if defined(__APPLE__) && !defined(__ANDROID__)  // exclude android for binding generator.
#    include <TargetConditionals.h>
#    if TARGET_OS_IPHONE  // TARGET_OS_IPHONE includes TARGET_OS_IOS TARGET_OS_TV and TARGET_OS_WATCH. see
                          // TargetConditionals.h
#        undef AX_TARGET_PLATFORM
#        define AX_TARGET_PLATFORM AX_PLATFORM_IOS
#        if TARGET_OS_TV && !defined(AX_TARGET_OS_TVOS)
#            define AX_TARGET_OS_TVOS 1
#        endif
#    elif TARGET_OS_MAC
#        undef AX_TARGET_PLATFORM
#        define AX_TARGET_PLATFORM AX_PLATFORM_MAC
#    endif
#endif

// win32
#if defined(_WIN32) && WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
#    undef AX_TARGET_PLATFORM
#    define AX_TARGET_PLATFORM AX_PLATFORM_WIN32
#endif

// WinRT (Windows 10.0 UWP App)
#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
// Windows Store or Universal Windows Platform
#    undef AX_TARGET_PLATFORM
#    define AX_TARGET_PLATFORM AX_PLATFORM_WINRT
#endif

// linux
#if defined(__linux__)
#    undef AX_TARGET_PLATFORM
#    define AX_TARGET_PLATFORM AX_PLATFORM_LINUX
#endif

#if defined(__EMSCRIPTEN__)
#    undef AX_TARGET_PLATFORM
#    define AX_TARGET_PLATFORM AX_PLATFORM_WASM
#endif

// android, override linux
#if defined(__ANDROID__) || defined(ANDROID)
#    undef AX_TARGET_PLATFORM
#    define AX_TARGET_PLATFORM AX_PLATFORM_ANDROID
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
The google/angle is library which translate native graphics API to GLES3+ APIs
repo: https://github.com/google/angle
windows: d3d9/d3d11/Desktop GL/Vulkan
macOS/iOS: Metal
Android: GLES/Vulkan
Linux: Desktop GL/Vulkan
*/
// 0: indicate: not use GLES
// mac/iOS/android use system builtin GL/GLES, not ANGLE
// Windows: use ANGLE GLES
#ifndef AX_GLES_PROFILE
#    if defined(__ANDROID__)
#        define AX_GLES_PROFILE 200
#    elif (AX_TARGET_PLATFORM == AX_PLATFORM_WINRT)
#        define AX_GLES_PROFILE 300
#    else
#        define AX_GLES_PROFILE 0
#    endif
#endif

#define AX_GLES_PROFILE_DEN 100

#if ((AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) || (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) || \
     (AX_TARGET_PLATFORM == AX_PLATFORM_WINRT) || (AX_TARGET_PLATFORM == AX_PLATFORM_WASM))
#    define AX_PLATFORM_MOBILE
#else
#    define AX_PLATFORM_PC
#endif

#if defined(__APPLE__)
#    if !defined(AX_USE_GL)
#        define AX_USE_METAL 1
#    endif
#else  // win32,linux,winuwp,android
#    if !defined(AX_USE_GL)
#        define AX_USE_GL 1
#    endif
#endif

/// @endcond
#endif  // __BASE_AX_PLATFORM_CONFIG_H__
