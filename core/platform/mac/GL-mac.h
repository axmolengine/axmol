/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#ifndef __PLATFORM_MAC_CCGL_H__
#define __PLATFORM_MAC_CCGL_H__

#include "platform/PlatformConfig.h"
#if AX_TARGET_PLATFORM == AX_PLATFORM_MAC

#    import <OpenGL/gl3.h>
#    import <OpenGL/gl3ext.h>

#    define AX_GL_DEPTH24_STENCIL8 -1

#    define glClearDepthf glClearDepth
#    define glDepthRangef glDepthRange
#    define glReleaseShaderCompiler(xxx)

// GLES macros, only for compiling compatible
#    if !defined(GL_ETC1_RGB8_OES)
#        define GL_ETC1_RGB8_OES 0x8D64
#    endif

#    if !defined(GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG)
#        define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8C00
#        define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8C01
#        define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#        define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8C03
#        define GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG 0x9137
#        define GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG 0x9138
#        define GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT 0x8A54
#        define GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT 0x8A55
#        define GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT 0x8A56
#        define GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT 0x8A57
#        define GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG 0x93F0
#        define GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG 0x93F1
#    endif

#    if !defined(GL_COMPRESSED_RGB8_ETC2)
#        define GL_COMPRESSED_RGB8_ETC2 0x9274
#        define GL_COMPRESSED_SRGB8_ETC2 0x9275
#        define GL_COMPRESSED_RGBA8_ETC2_EAC 0x9278
#        define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC 0x9279
#    endif

#    if !defined(GL_ATC_RGB_AMD)
#        define GL_ATC_RGB_AMD 0x8C92
#        define GL_ATC_RGBA_EXPLICIT_ALPHA_AMD 0x8C93
#        define GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD 0x87EE
#    endif

#    if !defined(GL_COMPRESSED_RGBA_ASTC_4x4_KHR)
#        define GL_COMPRESSED_RGBA_ASTC_4x4_KHR 0x93B0
#        define GL_COMPRESSED_RGBA_ASTC_5x4_KHR 0x93B1
#        define GL_COMPRESSED_RGBA_ASTC_5x5_KHR 0x93B2
#        define GL_COMPRESSED_RGBA_ASTC_6x5_KHR 0x93B3
#        define GL_COMPRESSED_RGBA_ASTC_6x6_KHR 0x93B4
#        define GL_COMPRESSED_RGBA_ASTC_8x5_KHR 0x93B5
#        define GL_COMPRESSED_RGBA_ASTC_8x6_KHR 0x93B6
#        define GL_COMPRESSED_RGBA_ASTC_8x8_KHR 0x93B7
#        define GL_COMPRESSED_RGBA_ASTC_10x5_KHR 0x93B8
#        define GL_COMPRESSED_RGBA_ASTC_10x6_KHR 0x93B9
#        define GL_COMPRESSED_RGBA_ASTC_10x8_KHR 0x93BA
#        define GL_COMPRESSED_RGBA_ASTC_10x10_KHR 0x93BB
#        define GL_COMPRESSED_RGBA_ASTC_12x10_KHR 0x93BC
#        define GL_COMPRESSED_RGBA_ASTC_12x12_KHR 0x93BD
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR 0x93D0
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR 0x93D1
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR 0x93D2
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR 0x93D3
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR 0x93D4
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR 0x93D5
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR 0x93D6
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR 0x93D7
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR 0x93D8
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR 0x93D9
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR 0x93DA
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR 0x93DB
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR 0x93DC
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR 0x93DD
#    endif

#endif  // __PLATFORM_MAC_CCGL_H__

#endif  // s AX_TARGET_PLATFORM == AX_PLATFORM_MAC
