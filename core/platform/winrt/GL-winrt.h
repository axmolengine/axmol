/****************************************************************************
Copyright (c) 2010 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.
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

#ifndef __CCGL_H__
#define __CCGL_H__

#define GL_BGRA GL_BGRA_EXT
#define glClearDepth glClearDepthf
#define GL_WRITE_ONLY GL_WRITE_ONLY_OES

#if AX_TARGET_PLATFORM == AX_PLATFORM_WINRT
#    define GL_GLEXT_PROTOTYPES 1

#    include "EGL/egl.h"
#    include "EGL/eglext.h"
#    include "EGL/eglplatform.h"
#    include "GLES2/gl2.h"
#    include "GLES2/gl2ext.h"
#    include "GLES3/gl3.h"

#    define glClearDepth glClearDepthf
#    define glDeleteVertexArrays glDeleteVertexArraysOES
#    define glGenVertexArrays glGenVertexArraysOES
#    define glBindVertexArray glBindVertexArrayOES
#    define glMapBuffer glMapBufferOES
#    define glUnmapBuffer glUnmapBufferOES
#    define glDrawBuffer(...)

#    define GL_WRITE_ONLY GL_WRITE_ONLY_OES

#    if !defined(GL_DEPTH_COMPONENT32)
#        define GL_DEPTH_COMPONENT32 0x81A7
#    endif

#endif

#endif  // __CCGL_H__
