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

#pragma once

/// @cond DO_NOT_SHOW

#include "axmol/platform/PlatformConfig.h"

#if AX_ENABLE_GL

#    if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
#        include "glad/gles2.h"
#    elif AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
#        if AX_GLES_PROFILE
#            include "glad/gles2.h"
#        else
#            include "glad/gl.h"
#        endif
#    elif AX_TARGET_PLATFORM == AX_PLATFORM_WINRT
#        define GL_GLEXT_PROTOTYPES 1
#        include "EGL/egl.h"
#        include "EGL/eglext.h"
#        include "EGL/eglplatform.h"
#        include "GLES2/gl2.h"
#        include "GLES2/gl2ext.h"
#        include "GLES3/gl3.h"
#    elif AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
#        include "glad/gl.h"
#    elif AX_TARGET_PLATFORM == AX_PLATFORM_WASM
#        include "glad/gles2.h"
#    elif AX_TARGET_PLATFORM == AX_PLATFORM_IOS
#        include <OpenGLES/ES3/gl.h>
#        include <OpenGLES/ES3/glext.h>
#    elif AX_TARGET_PLATFORM == AX_PLATFORM_MAC
#        include <OpenGL/gl3.h>
#        include <OpenGL/gl3ext.h>
#    endif

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

#    if !defined(GL_ETC1_RGB8)
#        define GL_ETC1_RGB8 0x8D64
#    endif

#    ifndef GL_WRITE_ONLY
#        define GL_WRITE_ONLY GL_WRITE_ONLY_OES
#    endif

#    ifndef GL_DEPTH_COMPONENT32
#        define GL_DEPTH_COMPONENT32 GL_DEPTH_COMPONENT32_OES
#    endif

#    if !defined(GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG)
#        define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG        0x8C00
#        define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG        0x8C01
#        define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG       0x8C02
#        define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG       0x8C03
#        define GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG       0x9137
#        define GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG       0x9138
#        define GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT       0x8A54
#        define GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT       0x8A55
#        define GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT 0x8A56
#        define GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT 0x8A57
#        define GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG 0x93F0
#        define GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG 0x93F1
#    endif

#    if !defined(GL_COMPRESSED_RGB8_ETC2)
#        define GL_COMPRESSED_RGB8_ETC2             0x9274
#        define GL_COMPRESSED_SRGB8_ETC2            0x9275
#        define GL_COMPRESSED_RGBA8_ETC2_EAC        0x9278
#        define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC 0x9279
#    endif

#    if !defined(GL_ATC_RGB_AMD)
#        define GL_ATC_RGB_AMD                     0x8C92
#        define GL_ATC_RGBA_EXPLICIT_ALPHA_AMD     0x8C93
#        define GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD 0x87EE
#    endif

#    if !defined(GL_COMPRESSED_RGBA_ASTC_4x4_KHR)
#        define GL_COMPRESSED_RGBA_ASTC_4x4_KHR           0x93B0
#        define GL_COMPRESSED_RGBA_ASTC_5x4_KHR           0x93B1
#        define GL_COMPRESSED_RGBA_ASTC_5x5_KHR           0x93B2
#        define GL_COMPRESSED_RGBA_ASTC_6x5_KHR           0x93B3
#        define GL_COMPRESSED_RGBA_ASTC_6x6_KHR           0x93B4
#        define GL_COMPRESSED_RGBA_ASTC_8x5_KHR           0x93B5
#        define GL_COMPRESSED_RGBA_ASTC_8x6_KHR           0x93B6
#        define GL_COMPRESSED_RGBA_ASTC_8x8_KHR           0x93B7
#        define GL_COMPRESSED_RGBA_ASTC_10x5_KHR          0x93B8
#        define GL_COMPRESSED_RGBA_ASTC_10x6_KHR          0x93B9
#        define GL_COMPRESSED_RGBA_ASTC_10x8_KHR          0x93BA
#        define GL_COMPRESSED_RGBA_ASTC_10x10_KHR         0x93BB
#        define GL_COMPRESSED_RGBA_ASTC_12x10_KHR         0x93BC
#        define GL_COMPRESSED_RGBA_ASTC_12x12_KHR         0x93BD
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR   0x93D0
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR   0x93D1
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR   0x93D2
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR   0x93D3
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR   0x93D4
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR   0x93D5
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR   0x93D6
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR   0x93D7
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR  0x93D8
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR  0x93D9
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR  0x93DA
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR 0x93DB
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR 0x93DC
#        define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR 0x93DD
#    endif

/* gles2/glext */
#    ifndef GL_EXT_texture_compression_s3tc_srgb
#        define GL_EXT_texture_compression_s3tc_srgb   1
#        define GL_COMPRESSED_SRGB_S3TC_DXT1_EXT       0x8C4C
#        define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT 0x8C4D
#        define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT 0x8C4E
#        define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT 0x8C4F
#    endif /* GL_EXT_texture_compression_s3tc_srgb */

#    ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
#        define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#        define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#        define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#    endif

#    ifndef glClearDepth
#        define glClearDepth glClearDepthf
#    endif

#    ifndef glMapBuffer
#        define glMapBuffer glMapBufferOES
#    endif

#    ifndef glDrawBuffer
#        define glDrawBuffer(...)
#    endif

#endif
