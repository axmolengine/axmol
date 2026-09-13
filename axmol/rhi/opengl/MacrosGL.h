/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/base/Macros.h"

#if !defined(_AX_DEBUG) || _AX_DEBUG == 0
#    define CHECK_GL_ERROR_DEBUG()
#else
#    define CHECK_GL_ERROR_DEBUG()                                                                         \
        do                                                                                                 \
        {                                                                                                  \
            GLenum __error = glGetError();                                                                 \
            if (__error)                                                                                   \
            {                                                                                              \
                AXLOGE("OpenGL error 0x{:04X} in {}({}) {}\n", __error, __FILE__, __LINE__, __FUNCTION__); \
            }                                                                                              \
        } while (false)
#    define CHECK_GL_ERROR_ABORT()                                                                         \
        do                                                                                                 \
        {                                                                                                  \
            GLenum __error = glGetError();                                                                 \
            if (__error)                                                                                   \
            {                                                                                              \
                AXLOGE("OpenGL error 0x{:04X} in {}({}) {}\n", __error, __FILE__, __LINE__, __FUNCTION__); \
                assert(false);                                                                             \
            }                                                                                              \
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
#    define AX_GL_ASSERT(gl_code) gl_code
#else
#    define AX_GL_ASSERT(gl_code)                                  \
        do                                                         \
        {                                                          \
            gl_code;                                               \
            __state_error_code = glGetError();                     \
            AX_ASSERT(__state_error_code == GL_NO_ERROR, "Error"); \
        } while (0)
#endif
