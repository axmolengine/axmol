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

#ifndef __PLATFORM_IOS_CCGL_H__
#define __PLATFORM_IOS_CCGL_H__

#include "platform/PlatformConfig.h"
#if AX_TARGET_PLATFORM == AX_PLATFORM_IOS

#    define glClearDepth glClearDepthf

#    include <OpenGLES/ES3/gl.h>
#    include <OpenGLES/ES3/glext.h>

/* gles3/gl */
#    if !defined(GL_SRGB8)
#        define GL_SRGB8 0x8C41
#    endif

#    if !defined(GL_SRGB8_ALPHA8)
#        define GL_SRGB8_ALPHA8 0x8C43
#    endif

#    if !defined(GL_COMPRESSED_RGB8_ETC2)
#        define GL_COMPRESSED_RGB8_ETC2 0x9274
#    endif

#    if !defined(GL_COMPRESSED_SRGB8_ETC2)
#        define GL_COMPRESSED_SRGB8_ETC2 0x9275
#    endif

#    if !defined(GL_COMPRESSED_RGBA8_ETC2_EAC)
#        define GL_COMPRESSED_RGBA8_ETC2_EAC 0x9278
#    endif

#    if !defined(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC)
#        define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC 0x9279
#    endif

/* gles2/glext */
#    ifndef GL_EXT_texture_compression_s3tc_srgb
#        define GL_EXT_texture_compression_s3tc_srgb 1
#        define GL_COMPRESSED_SRGB_S3TC_DXT1_EXT 0x8C4C
#        define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT 0x8C4D
#        define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT 0x8C4E
#        define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT 0x8C4F
#    endif /* GL_EXT_texture_compression_s3tc_srgb */

#    ifndef GL_ETC1_RGB8_OES
#        define GL_ETC1_RGB8_OES 0x8D64
#    endif

#    ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
#        define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#        define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#        define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#    endif

#    ifndef GL_ATC_RGB_AMD
#        define GL_ATC_RGB_AMD 0x8C92
#        define GL_ATC_RGBA_EXPLICIT_ALPHA_AMD 0x8C93
#        define GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD 0x87EE
#    endif

#    ifndef GL_COMPRESSED_RGBA_ASTC_4x4_KHR
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

// works on device which support OpenGLES 3.0
#    if !defined(GL_RG)
#        define GL_RG 0x8227
#    endif

#    if !defined(GL_RG8)
#        define GL_RG8 0x822B
#    endif

#endif  // AX_PLATFORM_IOS

#endif  // __PLATFORM_IOS_CCGL_H__
