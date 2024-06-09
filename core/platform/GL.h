/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

https://axmol.dev/

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

#ifndef __PLATFORM_CCGL_H__
#define __PLATFORM_CCGL_H__
/// @cond DO_NOT_SHOW

#include "platform/PlatformConfig.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
#    include "platform/android/GL-android.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
#    include "platform/win32/GL-win32.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_WINRT
#    include "platform/winrt/GL-winrt.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
#    include "platform/linux/GL-linux.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_WASM
#    include "platform/wasm/GL-wasm.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_IOS
#    if defined(AX_USE_GL)
#        include "platform/ios/GL-ios.h"
#    endif
#elif AX_TARGET_PLATFORM == AX_PLATFORM_MAC
#    if defined(AX_USE_GL)
#        include "platform/mac/GL-mac.h"
#    endif
#endif

#if defined(AX_USE_GL)
#    if !defined(GL_BGRA)
#        define GL_BGRA 0x80E1
#    endif

#    if !defined(GL_SRGB_ALPHA)
#        define GL_SRGB_ALPHA 0x8C42
#    endif

#    if !defined(GL_SRGB8_ALPHA8)
#        define GL_SRGB8_ALPHA8 0x8C43
#    endif

#    if !defined(GL_SRGB)
#        define GL_SRGB 0x8C40
#    endif

#    if !defined(GL_SRGB8)
#        define GL_SRGB8 0x8C41
#    endif

#    if !defined(GL_SLUMINANCE)
#        define GL_SLUMINANCE 0x8C46
#    endif

#    if !defined(GL_SLUMINANCE_ALPHA)
#        define GL_SLUMINANCE_ALPHA 0x8C44
#    endif

#    if !defined(GL_RGBA32F_EXT)
#        define GL_RGBA32F_EXT 0x8814
#    endif

#    if !defined(GL_LUMINANCE)
#        define GL_LUMINANCE 0x1909
#    endif
#    if !defined(GL_LUMINANCE_ALPHA)
#        define GL_LUMINANCE_ALPHA 0x190A
#    endif
#endif

/// @endcond
#endif /* __PLATFORM_CCPLATFORMDEFINE_H__*/
