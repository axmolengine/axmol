/****************************************************************************
Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#include "RenderPipelineGL.h"
#include "ShaderModuleGL.h"
#include "DepthStencilStateGL.h"
#include "ProgramGL.h"
#include "UtilsGL.h"
#include "MacrosGL.h"
#include "OpenGLState.h"

#include <assert.h>

NS_AX_BACKEND_BEGIN

void RenderPipelineGL::update(const RenderTarget*, const PipelineDescriptor& pipelineDescriptor)
{
    if (_programGL != pipelineDescriptor.programState->getProgram())
    {
        AX_SAFE_RELEASE(_programGL);
        _programGL = static_cast<ProgramGL*>(pipelineDescriptor.programState->getProgram());
        AX_SAFE_RETAIN(_programGL);
    }

    updateBlendState(pipelineDescriptor.blendDescriptor);
}

void RenderPipelineGL::updateBlendState(const BlendDescriptor& descriptor)
{
    auto blendEnabled                = descriptor.blendEnabled;
    auto rgbBlendOperation           = UtilsGL::toGLBlendOperation(descriptor.rgbBlendOperation);
    auto alphaBlendOperation         = UtilsGL::toGLBlendOperation(descriptor.alphaBlendOperation);
    auto sourceRGBBlendFactor        = UtilsGL::toGLBlendFactor(descriptor.sourceRGBBlendFactor);
    auto destinationRGBBlendFactor   = UtilsGL::toGLBlendFactor(descriptor.destinationRGBBlendFactor);
    auto sourceAlphaBlendFactor      = UtilsGL::toGLBlendFactor(descriptor.sourceAlphaBlendFactor);
    auto destinationAlphaBlendFactor = UtilsGL::toGLBlendFactor(descriptor.destinationAlphaBlendFactor);
    GLboolean writeMaskRed           = bitmask::any(descriptor.writeMask, ColorWriteMask::RED);
    GLboolean writeMaskGreen         = bitmask::any(descriptor.writeMask, ColorWriteMask::GREEN);
    GLboolean writeMaskBlue          = bitmask::any(descriptor.writeMask, ColorWriteMask::BLUE);
    GLboolean writeMaskAlpha         = bitmask::any(descriptor.writeMask, ColorWriteMask::ALPHA);

    if (blendEnabled)
    {
        __gl->enableBlend();

        __gl->blendEquationSeparate(rgbBlendOperation, alphaBlendOperation);
        __gl->blendFuncSeparate(sourceRGBBlendFactor, destinationRGBBlendFactor, sourceAlphaBlendFactor,
                               destinationAlphaBlendFactor);
    }
    else
        __gl->disableBlend();

    __gl->colorMask(writeMaskRed, writeMaskGreen, writeMaskBlue, writeMaskAlpha);
}

RenderPipelineGL::~RenderPipelineGL()
{
    AX_SAFE_RELEASE(_programGL);
}

NS_AX_BACKEND_END
