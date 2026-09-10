/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#pragma once

#include "axmol/platform/PlatformConfig.h"

#include "glad/gles2.h"

#undef GL_DEPTH_STENCIL
#undef GL_DEPTH24_STENCIL8
#undef GL_UNSIGNED_INT_24_8
#undef glClearDepth
#undef glMapBuffer
#undef glUnmapBuffer
#undef glBindVertexArray
#undef glDeleteVertexArrays
#undef glGenVertexArrays
#if defined(GL_VERSION_ES_CM_1_0)
#    undef glIsRenderbuffer
#    undef glBindRenderbuffer
#    undef glDeleteRenderbuffers
#    undef glGenRenderbuffers
#    undef glRenderbufferStorage
#    undef glIsFramebuffer
#    undef glBindFramebuffer
#    undef glDeleteFramebuffers
#    undef glGenFramebuffers
#    undef glCheckFramebufferStatus
#    undef glFramebufferRenderbuffer
#    undef glFramebufferTexture2D
#    undef glGetFramebufferAttachmentParameteriv
#    undef glGenerateMipmap
#endif

#define GL_DEPTH_STENCIL     GL_DEPTH_STENCIL_OES
#define GL_DEPTH24_STENCIL8  GL_DEPTH24_STENCIL8_OES
#define GL_UNSIGNED_INT_24_8 GL_UNSIGNED_INT_24_8_OES
#define glClearDepth         glClearDepthf
#define glMapBuffer          glMapBufferOES
#define glUnmapBuffer        glUnmapBufferOES
#define glBindVertexArray    glBindVertexArrayOES
#define glDeleteVertexArrays glDeleteVertexArraysOES
#define glGenVertexArrays    glGenVertexArraysOES
#if defined(GL_VERSION_ES_CM_1_0)
#    define glIsRenderbuffer                      glIsRenderbufferOES
#    define glBindRenderbuffer                    glBindRenderbufferOES
#    define glDeleteRenderbuffers                 glDeleteRenderbuffersOES
#    define glGenRenderbuffers                    glGenRenderbuffersOES
#    define glRenderbufferStorage                 glRenderbufferStorageOES
#    define glIsFramebuffer                       glIsFramebufferOES
#    define glBindFramebuffer                     glBindFramebufferOES
#    define glDeleteFramebuffers                  glDeleteFramebuffersOES
#    define glGenFramebuffers                     glGenFramebuffersOES
#    define glCheckFramebufferStatus              glCheckFramebufferStatusOES
#    define glFramebufferRenderbuffer             glFramebufferRenderbufferOES
#    define glFramebufferTexture2D                glFramebufferTexture2DOES
#    define glGetFramebufferAttachmentParameteriv glGetFramebufferAttachmentParameterivOES
#    define glGenerateMipmap                      glGenerateMipmapOES
#endif
