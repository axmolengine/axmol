/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

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

#include "DepthStencilStateGL.h"
#include "platform/GL.h"

#include "renderer/backend/opengl/MacrosGL.h"
#include "renderer/backend/opengl/UtilsGL.h"

NS_AX_BACKEND_BEGIN

void DepthStencilStateGL::reset()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
}

void DepthStencilStateGL::apply(unsigned int stencilReferenceValueFront, unsigned int stencilReferenceValueBack) const
{
    const auto dsFlags = _depthStencilInfo.flags;
    // depth test
    if (bitmask::any(dsFlags, DepthStencilFlags::DEPTH_TEST))
    {
        GL_ENABLE_DEPTH_TEST;
    }
    else
    {
        GL_DISABLE_DEPTH_TEST;
    }

    if (bitmask::any(dsFlags, DepthStencilFlags::DEPTH_WRITE))
        GL_SET_DEPTH_MASK(GL_TRUE);
    else
        GL_SET_DEPTH_MASK(GL_FALSE);

    GL_SET_DEPTH_FUNC(UtilsGL::toGLComareFunction(_depthStencilInfo.depthCompareFunction));

    // stencil test
    if (bitmask::any(dsFlags, DepthStencilFlags::STENCIL_TEST))
    {
        GL_ENABLE_STENCIL_TEST;

        if (_isBackFrontStencilEqual)
        {
            GL_STENCIL_FUNC(UtilsGL::toGLComareFunction(_depthStencilInfo.frontFaceStencil.stencilCompareFunction),
                            (GLint)stencilReferenceValueFront, _depthStencilInfo.frontFaceStencil.readMask);

            GL_STENCIL_OP(UtilsGL::toGLStencilOperation(_depthStencilInfo.frontFaceStencil.stencilFailureOperation),
                          UtilsGL::toGLStencilOperation(_depthStencilInfo.frontFaceStencil.depthFailureOperation),
                          UtilsGL::toGLStencilOperation(_depthStencilInfo.frontFaceStencil.depthStencilPassOperation));

            GL_STENCIL_MASK(_depthStencilInfo.frontFaceStencil.writeMask);
        }
        else
        {
            GL_STENCIL_FUNC_FRONT(UtilsGL::toGLComareFunction(_depthStencilInfo.backFaceStencil.stencilCompareFunction),
                                  (GLint)stencilReferenceValueBack, _depthStencilInfo.backFaceStencil.readMask);
            GL_STENCIL_FUNC_BACK(UtilsGL::toGLComareFunction(_depthStencilInfo.frontFaceStencil.stencilCompareFunction),
                                 (GLint)stencilReferenceValueFront, _depthStencilInfo.frontFaceStencil.readMask);

            GL_STENCIL_OP_FRONT(UtilsGL::toGLStencilOperation(_depthStencilInfo.backFaceStencil.stencilFailureOperation),
                UtilsGL::toGLStencilOperation(_depthStencilInfo.backFaceStencil.depthFailureOperation),
                UtilsGL::toGLStencilOperation(_depthStencilInfo.backFaceStencil.depthStencilPassOperation));
            GL_STENCIL_OP_BACK(UtilsGL::toGLStencilOperation(_depthStencilInfo.frontFaceStencil.stencilFailureOperation),
                UtilsGL::toGLStencilOperation(_depthStencilInfo.frontFaceStencil.depthFailureOperation),
                UtilsGL::toGLStencilOperation(_depthStencilInfo.frontFaceStencil.depthStencilPassOperation));

            GL_STENCIL_MASK_BACK(_depthStencilInfo.backFaceStencil.writeMask);
            GL_STENCIL_MASK_FRONT(_depthStencilInfo.frontFaceStencil.writeMask);
        }
    }
    else
        GL_DISABLE_STENCIL_TEST;

    CHECK_GL_ERROR_DEBUG();
}

NS_AX_BACKEND_END
