/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2022 Bytedance Inc.

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
#pragma once

#include "platform/CCPlatformConfig.h"

#if AX_USE_GLAD

#    include "glad/gl.h"

#    undef GL_DEPTH_STENCIL
#    undef GL_DEPTH24_STENCIL8
#    undef GL_UNSIGNED_INT_24_8
#    undef glClearDepth
#    undef glMapBuffer
#    undef glUnmapBuffer
#    undef glBindVertexArray
#    undef glDeleteVertexArrays
#    undef glGenVertexArrays
#    if defined(GL_VERSION_ES_CM_1_0)
#        undef glIsRenderbuffer
#        undef glBindRenderbuffer
#        undef glDeleteRenderbuffers
#        undef glGenRenderbuffers
#        undef glRenderbufferStorage
#        undef glIsFramebuffer
#        undef glBindFramebuffer
#        undef glDeleteFramebuffers
#        undef glGenFramebuffers
#        undef glCheckFramebufferStatus
#        undef glFramebufferRenderbuffer
#        undef glFramebufferTexture2D
#        undef glGetFramebufferAttachmentParameteriv
#        undef glGenerateMipmap
#    endif

#    define GL_DEPTH_STENCIL GL_DEPTH_STENCIL_OES
#    define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
#    define GL_UNSIGNED_INT_24_8 GL_UNSIGNED_INT_24_8_OES
#    define glClearDepth glClearDepthf
#    define glMapBuffer glMapBufferOES
#    define glUnmapBuffer glUnmapBufferOES
#    define glBindVertexArray glBindVertexArrayOES
#    define glDeleteVertexArrays glDeleteVertexArraysOES
#    define glGenVertexArrays glGenVertexArraysOES
#    if defined(GL_VERSION_ES_CM_1_0)
#        define glIsRenderbuffer glIsRenderbufferOES
#        define glBindRenderbuffer glBindRenderbufferOES
#        define glDeleteRenderbuffers glDeleteRenderbuffersOES
#        define glGenRenderbuffers glGenRenderbuffersOES
#        define glRenderbufferStorage glRenderbufferStorageOES
#        define glIsFramebuffer glIsFramebufferOES
#        define glBindFramebuffer glBindFramebufferOES
#        define glDeleteFramebuffers glDeleteFramebuffersOES
#        define glGenFramebuffers glGenFramebuffersOES
#        define glCheckFramebufferStatus glCheckFramebufferStatusOES
#        define glFramebufferRenderbuffer glFramebufferRenderbufferOES
#        define glFramebufferTexture2D glFramebufferTexture2DOES
#        define glGetFramebufferAttachmentParameteriv glGetFramebufferAttachmentParameterivOES
#        define glGenerateMipmap glGenerateMipmapOES
#    endif

#else

#    define glClearDepth glClearDepthf
#    define glDeleteVertexArrays glDeleteVertexArraysOES
#    define glGenVertexArrays glGenVertexArraysOES
#    define glBindVertexArray glBindVertexArrayOES
#    define glMapBuffer glMapBufferOES
#    define glUnmapBuffer glUnmapBufferOES

#    define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
#    define GL_DEPTH_STENCIL GL_DEPTH_STENCIL_OES
#    define GL_UNSIGNED_INT_24_8 GL_UNSIGNED_INT_24_8_OES
#    define GL_WRITE_ONLY GL_WRITE_ONLY_OES

// GL_GLEXT_PROTOTYPES isn't defined in glplatform.h on android ndk r7
// we manually define it here
#    include <GLES2/gl2platform.h>
#    ifndef GL_GLEXT_PROTOTYPES
#        define GL_GLEXT_PROTOTYPES 1
#    endif

// normal process
#    include <GLES2/gl2.h>
#    include <GLES2/gl2ext.h>
// gl2.h doesn't define GLchar on Android
typedef char GLchar;
// android defines GL_BGRA_EXT but not GL_BRGA
#    ifndef GL_BGRA
#        define GL_BGRA 0x80E1
#    endif

// declare here while define in EGLView_android.cpp
extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOESEXT;
extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOESEXT;
extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOESEXT;

#    define glGenVertexArraysOES glGenVertexArraysOESEXT
#    define glBindVertexArrayOES glBindVertexArrayOESEXT
#    define glDeleteVertexArraysOES glDeleteVertexArraysOESEXT

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

// works on device which support OpenGLES 3.0
#    if !defined(GL_RG)
#        define GL_RG 0x8227
#    endif

#    if !defined(GL_RG8)
#        define GL_RG8 0x822B
#    endif

#endif
