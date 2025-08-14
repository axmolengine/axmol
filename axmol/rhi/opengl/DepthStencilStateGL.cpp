/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

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

#include "axmol/rhi/opengl/DepthStencilStateGL.h"
#include "axmol/platform/GL.h"

#include "axmol/rhi/opengl/MacrosGL.h"
#include "axmol/rhi/opengl/UtilsGL.h"
#include "axmol/rhi/opengl/OpenGLState.h"

namespace ax::rhi::gl {

void DepthStencilStateImpl::reset()
{
    __state->disableDepthTest();
    __state->disableStencilTest();
}

void DepthStencilStateImpl::apply(unsigned int stencilReferenceValue) const
{
    const auto dsFlags = _dsDesc.flags;
    // depth test
    if (bitmask::any(dsFlags, DepthStencilFlags::DEPTH_TEST))
        __state->enableDepthTest();
    else
        __state->disableDepthTest();

    __state->depthMask(bitmask::any(dsFlags, DepthStencilFlags::DEPTH_WRITE));
    __state->depthFunc(UtilsGL::toGLComareFunction(_dsDesc.depthCompareFunction));

    // stencil test
    if (bitmask::any(dsFlags, DepthStencilFlags::STENCIL_TEST))
    {
        __state->enableStencilTest();

        if (_isBackFrontStencilEqual)
        {
            __state->stencilFunc(UtilsGL::toGLComareFunction(_dsDesc.frontFaceStencil.stencilCompareFunction),
                              (GLint)stencilReferenceValue, _dsDesc.frontFaceStencil.readMask);

            __state->stencilOp(UtilsGL::toGLStencilOperation(_dsDesc.frontFaceStencil.stencilFailureOperation),
                           UtilsGL::toGLStencilOperation(_dsDesc.frontFaceStencil.depthFailureOperation),
                           UtilsGL::toGLStencilOperation(_dsDesc.frontFaceStencil.depthStencilPassOperation));

            __state->stencilMask(_dsDesc.frontFaceStencil.writeMask);
        }
        else
        {
            __state->stencilFuncFront(UtilsGL::toGLComareFunction(_dsDesc.backFaceStencil.stencilCompareFunction),
                                   (GLint)stencilReferenceValue, _dsDesc.backFaceStencil.readMask);
            __state->stencilFuncBack(UtilsGL::toGLComareFunction(_dsDesc.frontFaceStencil.stencilCompareFunction),
                                  (GLint)stencilReferenceValue, _dsDesc.frontFaceStencil.readMask);

            __state->stencilOpFront(
                UtilsGL::toGLStencilOperation(_dsDesc.backFaceStencil.stencilFailureOperation),
                UtilsGL::toGLStencilOperation(_dsDesc.backFaceStencil.depthFailureOperation),
                UtilsGL::toGLStencilOperation(_dsDesc.backFaceStencil.depthStencilPassOperation));
            __state->stencilOpBack(
                UtilsGL::toGLStencilOperation(_dsDesc.frontFaceStencil.stencilFailureOperation),
                UtilsGL::toGLStencilOperation(_dsDesc.frontFaceStencil.depthFailureOperation),
                UtilsGL::toGLStencilOperation(_dsDesc.frontFaceStencil.depthStencilPassOperation));

            __state->stencilMaskBack(_dsDesc.backFaceStencil.writeMask);
            __state->stencilMaskFront(_dsDesc.frontFaceStencil.writeMask);
        }
    }
    else
        __state->disableStencilTest();

    CHECK_GL_ERROR_DEBUG();
}

}
