/****************************************************************************
Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/rhi/opengl/GraphicsPipelineGL.h"
#include "axmol/rhi/opengl/ShaderModuleGL.h"
#include "axmol/rhi/opengl/DepthStencilStateGL.h"
#include "axmol/rhi/opengl/ProgramGL.h"
#include "axmol/rhi/opengl/UtilsGL.h"
#include "axmol/rhi/opengl/MacrosGL.h"
#include "axmol/rhi/opengl/OpenGLState.h"

#include <assert.h>

namespace ax::rhi::gl
{

void GraphicsPipelineImpl::update(const RenderTarget*, const PipelineDesc& pipelineDesc)
{
    if (_programImpl != pipelineDesc.programState->getProgram())
    {
        AX_SAFE_RELEASE(_programImpl);
        _programImpl = static_cast<ProgramImpl*>(pipelineDesc.programState->getProgram());
        AX_SAFE_RETAIN(_programImpl);
    }

    updateBlendState(pipelineDesc.blendDesc);
}

void GraphicsPipelineImpl::updateBlendState(const BlendDesc& descriptor)
{
    auto blendEnabled                = descriptor.blendEnabled;
    auto rgbBlendOp                  = UtilsGL::toGLBlendOp(descriptor.rgbBlendOp);
    auto alphaBlendOp                = UtilsGL::toGLBlendOp(descriptor.alphaBlendOp);
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
        __state->enableBlend();

        __state->blendEquationSeparate(rgbBlendOp, alphaBlendOp);
        __state->blendFuncSeparate(sourceRGBBlendFactor, destinationRGBBlendFactor, sourceAlphaBlendFactor,
                                   destinationAlphaBlendFactor);
    }
    else
        __state->disableBlend();

    __state->colorMask(writeMaskRed, writeMaskGreen, writeMaskBlue, writeMaskAlpha);
}

GraphicsPipelineImpl::~GraphicsPipelineImpl()
{
    AX_SAFE_RELEASE(_programImpl);
}

}  // namespace ax::rhi::gl
