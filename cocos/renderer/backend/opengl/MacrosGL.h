/****************************************************************************
 Copyright (c) 2021 Bytedance Inc.

 https://adxe.org

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

#include "base/ccMacros.h"

#if !defined(COCOS2D_DEBUG) || COCOS2D_DEBUG == 0
#define CHECK_GL_ERROR_DEBUG()
#else
#define CHECK_GL_ERROR_DEBUG() \
    do { \
        GLenum __error = glGetError(); \
        if(__error) { \
            cocos2d::log("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
        } \
    } while (false)
#define CHECK_GL_ERROR_ABORT() \
    do { \
        GLenum __error = glGetError(); \
        if(__error) { \
            cocos2d::log("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
            assert(false);\
        } \
    } while (false)
#endif


/**
 * GL assertion that can be used for any OpenGL function call.
 *
 * This macro will assert if an error is detected when executing
 * the specified GL code. This macro will do nothing in release
 * mode and is therefore safe to use for realtime/per-frame GL
 * function calls.
 */
#if defined(NDEBUG) || (defined(__APPLE__) && !defined(DEBUG))
#define CC_GL_ASSERT( gl_code ) gl_code
#else
#define CC_GL_ASSERT( gl_code ) do \
{ \
gl_code; \
__gl_error_code = glGetError(); \
CC_ASSERT(__gl_error_code == GL_NO_ERROR, "Error"); \
} while(0)
#endif
