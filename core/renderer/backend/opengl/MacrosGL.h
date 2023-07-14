/****************************************************************************
 Copyright (c) 2021 Bytedance Inc.

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

#include "base/Macros.h"

#if !defined(_AX_DEBUG) || _AX_DEBUG == 0
#    define CHECK_GL_ERROR_DEBUG()
#else
#    define CHECK_GL_ERROR_DEBUG()                                                                       \
        do                                                                                               \
        {                                                                                                \
            GLenum __error = glGetError();                                                               \
            if (__error)                                                                                 \
            {                                                                                            \
                ax::log("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
            }                                                                                            \
        } while (false)
#    define CHECK_GL_ERROR_ABORT()                                                                       \
        do                                                                                               \
        {                                                                                                \
            GLenum __error = glGetError();                                                               \
            if (__error)                                                                                 \
            {                                                                                            \
                ax::log("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
                assert(false);                                                                           \
            }                                                                                            \
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
#    define AX_GL_ASSERT(gl_code)                               \
        do                                                      \
        {                                                       \
            gl_code;                                            \
            __gl_error_code = glGetError();                     \
            AX_ASSERT(__gl_error_code == GL_NO_ERROR, "Error"); \
        } while (0)
#endif

#define GL_ENABLE_DEPTH_TEST                                                  \
    do                                                                        \
    {                                                                         \
        if (!_glState._depthTest.has_value() || !_glState._depthTest.value()) \
        {                                                                     \
            glEnable(GL_DEPTH_TEST);                                          \
            _glState._depthTest = true;                                       \
        }                                                                     \
    } while (0)

#define GL_DISABLE_DEPTH_TEST                                                \
    do                                                                       \
    {                                                                        \
        if (!_glState._depthTest.has_value() || _glState._depthTest.value()) \
        {                                                                    \
            glDisable(GL_DEPTH_TEST);                                        \
            _glState._depthTest = false;                                     \
        }                                                                    \
    } while (0)

#define GL_ENABLE_BLENDING                                            \
    do                                                                \
    {                                                                 \
        if (!_glState._blend.has_value() || !_glState._blend.value()) \
        {                                                             \
            glEnable(GL_BLEND);                                       \
            _glState._blend = true;                                   \
        }                                                             \
    } while (0)

#define GL_DISABLE_BLENDING                                          \
    do                                                               \
    {                                                                \
        if (!_glState._blend.has_value() || _glState._blend.value()) \
        {                                                            \
            glDisable(GL_BLEND);                                     \
            _glState._blend = false;                                 \
        }                                                            \
    } while (0)

#define GL_ENABLE_SCISSOR_TEST                                            \
    do                                                                    \
    {                                                                     \
        if (!_glState._scissor.has_value() || !_glState._scissor.value()) \
        {                                                                 \
            glEnable(GL_SCISSOR_TEST);                                    \
            _glState._scissor = true;                                     \
        }                                                                 \
    } while (0)

#define GL_DISABLE_SCISSOR_TEST                                          \
    do                                                                   \
    {                                                                    \
        if (!_glState._scissor.has_value() || _glState._scissor.value()) \
        {                                                                \
            glDisable(GL_SCISSOR_TEST);                                  \
            _glState._scissor = false;                                   \
        }                                                                \
    } while (0)

#define GL_LINE_WIDTH(V)                                                          \
    do                                                                            \
    {                                                                             \
        if (!_glState._lineWidth.has_value() || _glState._lineWidth.value() != V) \
        {                                                                         \
            glLineWidth(V);                                                       \
            _glState._lineWidth = V;                                              \
        }                                                                         \
    } while (0)

#define GL_BIND_FRAMEBUFFER(I)                                                                \
    do                                                                                        \
    {                                                                                         \
        if (!_glState._frameBufferBind.has_value() || _glState._frameBufferBind.value() != I) \
        {                                                                                     \
            glBindFramebuffer(GL_FRAMEBUFFER, I);                                             \
            _glState._frameBufferBind = I;                                                    \
        }                                                                                     \
    } while (0)

#define GL_BIND_FRAMEBUFFER(I)                                                                \
    do                                                                                        \
    {                                                                                         \
        if (!_glState._frameBufferBind.has_value() || _glState._frameBufferBind.value() != I) \
        {                                                                                     \
            glBindFramebuffer(GL_FRAMEBUFFER, I);                                             \
            _glState._frameBufferBind = I;                                                    \
        }                                                                                     \
    } while (0)

#define GL_BLEND_EQUATION_SEPARATE(BLEND_RBG_OPER, BLEND_ALPHA_OPER)                                                 \
    do                                                                                                               \
    {                                                                                                                \
        if (!_glState._blendEquationSeparate.has_value() ||                                                          \
            _glState._blendEquationSeparate.value() != BlendEquationSeparateState{BLEND_RBG_OPER, BLEND_ALPHA_OPER}) \
        {                                                                                                            \
            glBlendEquationSeparate(BLEND_RBG_OPER, BLEND_ALPHA_OPER);                                               \
            _glState._blendEquationSeparate = BlendEquationSeparateState{BLEND_RBG_OPER, BLEND_ALPHA_OPER};          \
        }                                                                                                            \
    } while (0)

#define GL_BLEND_FUNC_SEPARATE(SRC_RBG_FACTOR, DST_RGB_FACTOR, SRC_ALPHA_FACTOR, DST_ALPHA_FACTOR)          \
    do                                                                                                      \
    {                                                                                                       \
        if (!_glState._blendFuncSeparate.has_value() ||                                                     \
            _glState._blendFuncSeparate.value() !=                                                          \
                BlendFuncSeparateState{SRC_RBG_FACTOR, DST_RGB_FACTOR, SRC_ALPHA_FACTOR, DST_ALPHA_FACTOR}) \
        {                                                                                                   \
            glBlendFuncSeparate(SRC_RBG_FACTOR, DST_RGB_FACTOR, SRC_ALPHA_FACTOR, DST_ALPHA_FACTOR);        \
            _glState._blendFuncSeparate =                                                                   \
                BlendFuncSeparateState{SRC_RBG_FACTOR, DST_RGB_FACTOR, SRC_ALPHA_FACTOR, DST_ALPHA_FACTOR}; \
        }                                                                                                   \
    } while (0)

#define GL_COLOR_MASK(RED_MASK, GREEN_MASK, BLUE_MASK, ALPHA_MASK)                                      \
    do                                                                                                  \
    {                                                                                                   \
        if (!_glState._colorMask.has_value() ||                                                         \
            _glState._colorMask.value() != ColorMaskState{RED_MASK, GREEN_MASK, BLUE_MASK, ALPHA_MASK}) \
        {                                                                                               \
            glColorMask(RED_MASK, GREEN_MASK, BLUE_MASK, ALPHA_MASK);                                   \
            _glState._colorMask = ColorMaskState{RED_MASK, GREEN_MASK, BLUE_MASK, ALPHA_MASK};          \
        }                                                                                               \
    } while (0)

#define GL_SET_DEPTH_MASK(V)                                                      \
    do                                                                            \
    {                                                                             \
        if (!_glState._depthMask.has_value() || _glState._depthMask.value() != V) \
        {                                                                         \
            glDepthMask(V);                                                       \
            _glState._depthMask = V;                                              \
        }                                                                         \
    } while (0)

#define GL_SET_DEPTH_FUNC(V)                                                      \
    do                                                                            \
    {                                                                             \
        if (!_glState._depthFunc.has_value() || _glState._depthFunc.value() != V) \
        {                                                                         \
            glDepthFunc(V);                                                       \
            _glState._depthFunc = V;                                              \
        }                                                                         \
    } while (0)

#define GL_ENABLE_STENCIL_TEST                                                    \
    do                                                                            \
    {                                                                             \
        if (!_glState._stencilTest.has_value() || !_glState._stencilTest.value()) \
        {                                                                         \
            glEnable(GL_STENCIL_TEST);                                            \
            _glState._stencilTest = true;                                         \
        }                                                                         \
    } while (0)

#define GL_DISABLE_STENCIL_TEST                                                  \
    do                                                                           \
    {                                                                            \
        if (!_glState._stencilTest.has_value() || _glState._stencilTest.value()) \
        {                                                                        \
            glDisable(GL_STENCIL_TEST);                                          \
            _glState._stencilTest = false;                                       \
        }                                                                        \
    } while (0)

#define GL_ENABLE_CULL_FACE                                                 \
    do                                                                      \
    {                                                                       \
        if (!_glState._cullFace.has_value() || !_glState._cullFace.value()) \
        {                                                                   \
            glEnable(GL_CULL_FACE);                                         \
            _glState._cullFace = true;                                      \
        }                                                                   \
    } while (0)

#define GL_DISABLE_CULL_FACE                                               \
    do                                                                     \
    {                                                                      \
        if (!_glState._cullFace.has_value() || _glState._cullFace.value()) \
        {                                                                  \
            glDisable(GL_CULL_FACE);                                       \
            _glState._cullFace = false;                                    \
        }                                                                  \
    } while (0)

#define GL_USE_PROGRAM(I)                                                             \
    do                                                                                \
    {                                                                                 \
        if (!_glState._programBind.has_value() || _glState._programBind.value() != I) \
        {                                                                             \
            glUseProgram(I);                                                          \
            _glState._programBind = I;                                                \
        }                                                                             \
    } while (0)

#define GL_STENCIL_FUNC(FUNC, REF, MASK)                                                                              \
    do                                                                                                                \
    {                                                                                                                 \
        if (!_glState._stencilFunc.has_value() || _glState._stencilFunc.value() != StencilFuncState{FUNC, REF, MASK}) \
        {                                                                                                             \
            glStencilFunc(FUNC, REF, MASK);                                                                           \
            _glState._stencilFunc = StencilFuncState{FUNC, REF, MASK};                                                \
        }                                                                                                             \
    } while (0)

#define GL_STENCIL_OP(FAIL, ZFAIL, ZPASS)                                             \
    do                                                                                \
    {                                                                                 \
        if (!_glState._stencilOp.has_value() ||                                       \
            _glState._stencilOp.value() != StencilOperationState{FAIL, ZFAIL, ZPASS}) \
        {                                                                             \
            glStencilOp(FAIL, ZFAIL, ZPASS);                                          \
            _glState._stencilOp = StencilOperationState{FAIL, ZFAIL, ZPASS};          \
        }                                                                             \
    } while (0)

#define GL_STENCIL_MASK(MASK)                                                            \
    do                                                                                   \
    {                                                                                    \
        if (!_glState._stencilMask.has_value() || _glState._stencilMask.value() != MASK) \
        {                                                                                \
            glStencilMask(MASK);                                                         \
            _glState._stencilMask = MASK;                                                \
        }                                                                                \
    } while (0)

#define GL_STENCIL_FUNC_FRONT(FUNC, REF, MASK)                                       \
    do                                                                               \
    {                                                                                \
        if (!_glState._stencilFuncFront.has_value() ||                               \
            _glState._stencilFuncFront.value() != StencilFuncState{FUNC, REF, MASK}) \
        {                                                                            \
            glStencilFuncSeparate(GL_FRONT, FUNC, REF, MASK);                        \
            _glState._stencilFuncFront = StencilFuncState{FUNC, REF, MASK};          \
        }                                                                            \
    } while (0)

#define GL_STENCIL_FUNC_BACK(FUNC, REF, MASK)                                       \
    do                                                                              \
    {                                                                               \
        if (!_glState._stencilFuncBack.has_value() ||                               \
            _glState._stencilFuncBack.value() != StencilFuncState{FUNC, REF, MASK}) \
        {                                                                           \
            glStencilFuncSeparate(GL_BACK, FUNC, REF, MASK);                        \
            _glState._stencilFuncBack = StencilFuncState{FUNC, REF, MASK};          \
        }                                                                           \
    } while (0)

#define GL_STENCIL_OP_FRONT(FAIL, ZFAIL, ZPASS)                                            \
    do                                                                                     \
    {                                                                                      \
        if (!_glState._stencilOpFront.has_value() ||                                       \
            _glState._stencilOpFront.value() != StencilOperationState{FAIL, ZFAIL, ZPASS}) \
        {                                                                                  \
            glStencilOpSeparate(GL_FRONT, FAIL, ZFAIL, ZPASS);                             \
            _glState._stencilOpFront = StencilOperationState{FAIL, ZFAIL, ZPASS};          \
        }                                                                                  \
    } while (0)

#define GL_STENCIL_OP_BACK(FAIL, ZFAIL, ZPASS)                                            \
    do                                                                                    \
    {                                                                                     \
        if (!_glState._stencilOpBack.has_value() ||                                       \
            _glState._stencilOpBack.value() != StencilOperationState{FAIL, ZFAIL, ZPASS}) \
        {                                                                                 \
            glStencilOpSeparate(GL_BACK, FAIL, ZFAIL, ZPASS);                             \
            _glState._stencilOpBack = StencilOperationState{FAIL, ZFAIL, ZPASS};          \
        }                                                                                 \
    } while (0)

#define GL_STENCIL_MASK_FRONT(MASK)                                                                \
    do                                                                                             \
    {                                                                                              \
        if (!_glState._stencilMaskFront.has_value() || _glState._stencilMaskFront.value() != MASK) \
        {                                                                                          \
            glStencilMaskSeparate(GL_FRONT, MASK);                                                 \
            _glState._stencilMaskFront = MASK;                                                     \
        }                                                                                          \
    } while (0)

#define GL_STENCIL_MASK_BACK(MASK)                                                               \
    do                                                                                           \
    {                                                                                            \
        if (!_glState._stencilMaskBack.has_value() || _glState._stencilMaskBack.value() != MASK) \
        {                                                                                        \
            glStencilMaskSeparate(GL_BACK, MASK);                                                \
            _glState._stencilMaskBack = MASK;                                                    \
        }                                                                                        \
    } while (0)

#define GL_SET_ACTIVE_TEXTURE(T)                                                          \
    do                                                                                    \
    {                                                                                     \
        if (!_glState._activeTexture.has_value() || _glState._activeTexture.value() != T) \
        {                                                                                 \
            glActiveTexture(T);                                                           \
            _glState._activeTexture = T;                                                  \
        }                                                                                 \
    } while (0)

#define GL_BIND_TEXTURE(TARGET, TEXTURE)                                                                              \
    do                                                                                                                \
    {                                                                                                                 \
        if (!_glState._textureBind.has_value() || _glState._textureBind.value() != TextureBindState{TARGET, TEXTURE}) \
        {                                                                                                             \
            glBindTexture(TARGET, TEXTURE);                                                                           \
            _glState._textureBind = TextureBindState{TARGET, TEXTURE};                                                \
        }                                                                                                             \
    } while (0)

#define GL_BIND_ARRAY_BUFFER(B)                                                       \
    do                                                                                \
    {                                                                                 \
        if (!_glState._arrayBuffer.has_value() || _glState._arrayBuffer.value() != B) \
        {                                                                             \
            glBindBuffer(GL_ARRAY_BUFFER, B);                                         \
            _glState._arrayBuffer = B;                                                \
        }                                                                             \
    } while (0)

#define GL_BIND_ELEMENT_ARRAY_BUFFER(B)                                                             \
    do                                                                                              \
    {                                                                                               \
        if (!_glState._elementArrayBuffer.has_value() || _glState._elementArrayBuffer.value() != B) \
        {                                                                                           \
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, B);                                               \
            _glState._elementArrayBuffer = B;                                                       \
        }                                                                                           \
    } while (0)
