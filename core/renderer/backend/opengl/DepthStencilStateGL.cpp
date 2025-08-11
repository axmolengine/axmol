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

#include "DepthStencilStateGL.h"
#include "platform/GL.h"

#include "renderer/backend/opengl/MacrosGL.h"
#include "renderer/backend/opengl/UtilsGL.h"
#include "OpenGLState.h"

namespace ax::backend {

void DepthStencilStateGL::reset()
{
    __gl->disableDepthTest();
    __gl->disableStencilTest();
}

void DepthStencilStateGL::apply(unsigned int stencilReferenceValue) const
{
    const auto dsFlags = _dsDesc.flags;
    // depth test
    if (bitmask::any(dsFlags, DepthStencilFlags::DEPTH_TEST))
        __gl->enableDepthTest();
    else
        __gl->disableDepthTest();

    __gl->depthMask(bitmask::any(dsFlags, DepthStencilFlags::DEPTH_WRITE));
    __gl->depthFunc(UtilsGL::toGLComareFunction(_dsDesc.depthCompareFunction));

    // stencil test
    if (bitmask::any(dsFlags, DepthStencilFlags::STENCIL_TEST))
    {
        __gl->enableStencilTest();

        if (_isBackFrontStencilEqual)
        {
            __gl->stencilFunc(UtilsGL::toGLComareFunction(_dsDesc.frontFaceStencil.stencilCompareFunction),
                              (GLint)stencilReferenceValue, _dsDesc.frontFaceStencil.readMask);

            __gl->stencilOp(UtilsGL::toGLStencilOperation(_dsDesc.frontFaceStencil.stencilFailureOperation),
                           UtilsGL::toGLStencilOperation(_dsDesc.frontFaceStencil.depthFailureOperation),
                           UtilsGL::toGLStencilOperation(_dsDesc.frontFaceStencil.depthStencilPassOperation));

            __gl->stencilMask(_dsDesc.frontFaceStencil.writeMask);
        }
        else
        {
            __gl->stencilFuncFront(UtilsGL::toGLComareFunction(_dsDesc.backFaceStencil.stencilCompareFunction),
                                   (GLint)stencilReferenceValue, _dsDesc.backFaceStencil.readMask);
            __gl->stencilFuncBack(UtilsGL::toGLComareFunction(_dsDesc.frontFaceStencil.stencilCompareFunction),
                                  (GLint)stencilReferenceValue, _dsDesc.frontFaceStencil.readMask);

            __gl->stencilOpFront(
                UtilsGL::toGLStencilOperation(_dsDesc.backFaceStencil.stencilFailureOperation),
                UtilsGL::toGLStencilOperation(_dsDesc.backFaceStencil.depthFailureOperation),
                UtilsGL::toGLStencilOperation(_dsDesc.backFaceStencil.depthStencilPassOperation));
            __gl->stencilOpBack(
                UtilsGL::toGLStencilOperation(_dsDesc.frontFaceStencil.stencilFailureOperation),
                UtilsGL::toGLStencilOperation(_dsDesc.frontFaceStencil.depthFailureOperation),
                UtilsGL::toGLStencilOperation(_dsDesc.frontFaceStencil.depthStencilPassOperation));

            __gl->stencilMaskBack(_dsDesc.backFaceStencil.writeMask);
            __gl->stencilMaskFront(_dsDesc.frontFaceStencil.writeMask);
        }
    }
    else
        __gl->disableStencilTest();

    CHECK_GL_ERROR_DEBUG();
}

}
