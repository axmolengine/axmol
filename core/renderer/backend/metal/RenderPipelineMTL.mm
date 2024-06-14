/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#include "RenderPipelineMTL.h"
#include "DriverMTL.h"
#include "RenderTargetMTL.h"
#include "ShaderModuleMTL.h"
#include "DepthStencilStateMTL.h"
#include "UtilsMTL.h"
#include "ProgramMTL.h"
#include "xxhash.h"

NS_AX_BACKEND_BEGIN

namespace
{
MTLVertexStepFunction toMTLVertexStepFunction(VertexStepMode vertexStepMode)
{
    if (VertexStepMode::VERTEX == vertexStepMode)
        return MTLVertexStepFunctionPerVertex;
    else
        return MTLVertexStepFunctionPerInstance;
}

MTLVertexFormat toMTLVertexFormat(VertexFormat vertexFormat, bool needNormalize)
{
    MTLVertexFormat ret = MTLVertexFormatFloat4;
    switch (vertexFormat)
    {
    case VertexFormat::FLOAT4:
        ret = MTLVertexFormatFloat4;
        break;
    case VertexFormat::FLOAT3:
        ret = MTLVertexFormatFloat3;
        break;
    case VertexFormat::FLOAT2:
        ret = MTLVertexFormatFloat2;
        break;
    case VertexFormat::FLOAT:
        ret = MTLVertexFormatFloat;
        break;
    case VertexFormat::INT4:
        ret = MTLVertexFormatInt4;
        break;
    case VertexFormat::INT3:
        ret = MTLVertexFormatInt3;
        break;
    case VertexFormat::INT2:
        ret = MTLVertexFormatInt2;
        break;
    case VertexFormat::INT:
        ret = MTLVertexFormatInt;
        break;
    case VertexFormat::USHORT4:
        ret = MTLVertexFormatUShort4;
        break;
    case VertexFormat::USHORT2:
        ret = MTLVertexFormatUShort2;
        break;
    case VertexFormat::UBYTE4:
        if (needNormalize)
            ret = MTLVertexFormatUChar4Normalized;
        else
            ret = MTLVertexFormatUChar4;
        break;
    default:
        assert(false);
        break;
    }
    return ret;
}

MTLColorWriteMask toMTLColorWriteMask(ColorWriteMask mask)
{
    switch (mask)
    {
    case ColorWriteMask::NONE:
        return MTLColorWriteMaskNone;
    case ColorWriteMask::RED:
        return MTLColorWriteMaskRed;
    case ColorWriteMask::GREEN:
        return MTLColorWriteMaskGreen;
    case ColorWriteMask::BLUE:
        return MTLColorWriteMaskBlue;
    case ColorWriteMask::ALPHA:
        return MTLColorWriteMaskAlpha;
    case ColorWriteMask::ALL:
        return MTLColorWriteMaskAll;
    }
}

MTLBlendFactor toMTLBlendFactor(BlendFactor factor)
{
    switch (factor)
    {
    case BlendFactor::ZERO:
        return MTLBlendFactorZero;
    case BlendFactor::ONE:
        return MTLBlendFactorOne;
    case BlendFactor::SRC_COLOR:
        return MTLBlendFactorSourceColor;
    case BlendFactor::ONE_MINUS_SRC_COLOR:
        return MTLBlendFactorOneMinusSourceColor;
    case BlendFactor::SRC_ALPHA:
        return MTLBlendFactorSourceAlpha;
    case BlendFactor::ONE_MINUS_SRC_ALPHA:
        return MTLBlendFactorOneMinusSourceAlpha;
    case BlendFactor::DST_COLOR:
        return MTLBlendFactorDestinationColor;
    case BlendFactor::ONE_MINUS_DST_COLOR:
        return MTLBlendFactorOneMinusDestinationColor;
    case BlendFactor::DST_ALPHA:
        return MTLBlendFactorDestinationAlpha;
    case BlendFactor::ONE_MINUS_DST_ALPHA:
        return MTLBlendFactorOneMinusDestinationAlpha;
    case BlendFactor::SRC_ALPHA_SATURATE:
        return MTLBlendFactorSourceAlphaSaturated;
    case BlendFactor::BLEND_COLOR:
        return MTLBlendFactorBlendColor;
    default:
        return MTLBlendFactorZero;
    }
}

MTLBlendOperation toMTLBlendOperation(BlendOperation operation)
{
    switch (operation)
    {
    case BlendOperation::ADD:
        return MTLBlendOperationAdd;
    case BlendOperation::SUBTRACT:
        return MTLBlendOperationSubtract;
    case BlendOperation::REVERSE_SUBTRACT:
        return MTLBlendOperationReverseSubtract;
    default:
        return MTLBlendOperationAdd;
    }
}
}

RenderPipelineMTL::RenderPipelineMTL(id<MTLDevice> mtlDevice) : _mtlDevice(mtlDevice) {}

void RenderPipelineMTL::update(const RenderTarget* renderTarget, const PipelineDescriptor& pipelineDescriptor)
{
    struct
    {
        size_t vertexShaderHash;
        size_t fragmentShaderHash;
        unsigned int vertexLayoutInfo[32];
        backend::PixelFormat colorAttachment[MAX_COLOR_ATTCHMENT];
        backend::PixelFormat depthAttachment;
        backend::PixelFormat stencilAttachment;
        bool blendEnabled;
        unsigned int writeMask;
        unsigned int rgbBlendOperation;
        unsigned int alphaBlendOperation;
        unsigned int sourceRGBBlendFactor;
        unsigned int destinationRGBBlendFactor;
        unsigned int sourceAlphaBlendFactor;
        unsigned int destinationAlphaBlendFactor;
    } hashMe;

    memset(&hashMe, 0, sizeof(hashMe));
    const auto& blendDescriptor = pipelineDescriptor.blendDescriptor;
    chooseAttachmentFormat(renderTarget, _colorAttachmentsFormat, _depthAttachmentFormat, _stencilAttachmentFormat);
    auto program              = static_cast<ProgramMTL*>(pipelineDescriptor.programState->getProgram());
    hashMe.vertexShaderHash   = program->getVertexShader()->getHashValue();
    hashMe.fragmentShaderHash = program->getFragmentShader()->getHashValue();
    memcpy(&hashMe.colorAttachment, &_colorAttachmentsFormat, sizeof(_colorAttachmentsFormat));
    hashMe.depthAttachment             = _depthAttachmentFormat;
    hashMe.stencilAttachment           = _stencilAttachmentFormat;
    hashMe.blendEnabled                = blendDescriptor.blendEnabled;
    hashMe.writeMask                   = (unsigned int)blendDescriptor.writeMask;
    hashMe.rgbBlendOperation           = (unsigned int)blendDescriptor.rgbBlendOperation;
    hashMe.alphaBlendOperation         = (unsigned int)blendDescriptor.alphaBlendOperation;
    hashMe.sourceRGBBlendFactor        = (unsigned int)blendDescriptor.sourceRGBBlendFactor;
    hashMe.destinationRGBBlendFactor   = (unsigned int)blendDescriptor.destinationRGBBlendFactor;
    hashMe.sourceAlphaBlendFactor      = (unsigned int)blendDescriptor.sourceAlphaBlendFactor;
    hashMe.destinationAlphaBlendFactor = (unsigned int)blendDescriptor.destinationAlphaBlendFactor;
    int index                          = 0;
    auto vertexLayout                  = pipelineDescriptor.programState->getVertexLayout();
    const auto& attributes             = vertexLayout->getAttributes();
    for (const auto& it : attributes)
    {
        auto& attribute = it.second;
        /*
         stepFunction:1     stride:15       offest:10       format:5        needNormalized:1
         bit31           bit30 ~ bit16   bit15 ~ bit6    bit5 ~ bit1     bit0
         */
        hashMe.vertexLayoutInfo[index++] =
            ((unsigned int)vertexLayout->getVertexStepMode() & 0x1) << 31 |
            ((unsigned int)(vertexLayout->getStride() & 0x7FFF)) << 16 | ((unsigned int)attribute.offset & 0x3FF) << 6 |
            ((unsigned int)attribute.format & 0x1F) << 1 | ((unsigned int)attribute.needToBeNormallized & 0x1);
    }

    unsigned int hash = XXH32((const void*)&hashMe, sizeof(hashMe), 0);
    auto it           = _mtlStateCache.find(hash);
    if (it != _mtlStateCache.end())
    {
        _mtlRenderPipelineState = it->second;
        return;
    }

    _mtlRenderPipelineDescriptor = [[MTLRenderPipelineDescriptor alloc] init];

    setShaderModules(pipelineDescriptor);
    setVertexLayout(_mtlRenderPipelineDescriptor, pipelineDescriptor);

    setBlendStateAndFormat(pipelineDescriptor.blendDescriptor);

    NSError* error          = nil;
    _mtlRenderPipelineState = [_mtlDevice newRenderPipelineStateWithDescriptor:_mtlRenderPipelineDescriptor
                                                                         error:&error];
    if (error)
        NSLog(@"Can not create renderpipeline state: %@", error);

    [_mtlRenderPipelineDescriptor release];

    _mtlStateCache.emplace(hash, _mtlRenderPipelineState);
}

RenderPipelineMTL::~RenderPipelineMTL()
{
    for (auto& item : _mtlStateCache)
        [item.second release];
}

void RenderPipelineMTL::setVertexLayout(MTLRenderPipelineDescriptor* mtlDescriptor,
                                        const PipelineDescriptor& descriptor)
{
    auto vertexLayout = descriptor.programState->getVertexLayout();
    if (!vertexLayout->isValid())
        return;

    int stride = static_cast<int>(vertexLayout->getStride());
    auto vertexDesc = mtlDescriptor.vertexDescriptor;
    vertexDesc.layouts[DriverMTL::DEFAULT_ATTRIBS_BINDING_INDEX].stride = stride;
    vertexDesc.layouts[DriverMTL::DEFAULT_ATTRIBS_BINDING_INDEX].stepFunction =
        toMTLVertexStepFunction(vertexLayout->getVertexStepMode());

    const auto& attributes = vertexLayout->getAttributes();


    for (const auto& it : attributes)
    {
        auto attribute = it.second;

        vertexDesc.attributes[attribute.index].format =
            toMTLVertexFormat(attribute.format, attribute.needToBeNormallized);
        vertexDesc.attributes[attribute.index].offset = attribute.offset;
        // Buffer index will always be 0;
        vertexDesc.attributes[attribute.index].bufferIndex = DriverMTL::DEFAULT_ATTRIBS_BINDING_INDEX;

    }
}

void RenderPipelineMTL::setBlendState(MTLRenderPipelineColorAttachmentDescriptor* colorAttachmentDescriptor,
                                      const BlendDescriptor& blendDescriptor)
{
    colorAttachmentDescriptor.blendingEnabled = blendDescriptor.blendEnabled;
    colorAttachmentDescriptor.writeMask       = toMTLColorWriteMask(blendDescriptor.writeMask);

    colorAttachmentDescriptor.rgbBlendOperation   = toMTLBlendOperation(blendDescriptor.rgbBlendOperation);
    colorAttachmentDescriptor.alphaBlendOperation = toMTLBlendOperation(blendDescriptor.alphaBlendOperation);

    colorAttachmentDescriptor.sourceRGBBlendFactor      = toMTLBlendFactor(blendDescriptor.sourceRGBBlendFactor);
    colorAttachmentDescriptor.destinationRGBBlendFactor = toMTLBlendFactor(blendDescriptor.destinationRGBBlendFactor);
    colorAttachmentDescriptor.sourceAlphaBlendFactor    = toMTLBlendFactor(blendDescriptor.sourceAlphaBlendFactor);
    colorAttachmentDescriptor.destinationAlphaBlendFactor =
        toMTLBlendFactor(blendDescriptor.destinationAlphaBlendFactor);
}

void RenderPipelineMTL::setShaderModules(const PipelineDescriptor& descriptor)
{
    auto vertexShaderModule = static_cast<ProgramMTL*>(descriptor.programState->getProgram())->getVertexShader();
    _mtlRenderPipelineDescriptor.vertexFunction = vertexShaderModule->getMTLFunction();

    auto fragShaderModule = static_cast<ProgramMTL*>(descriptor.programState->getProgram())->getFragmentShader();
    _mtlRenderPipelineDescriptor.fragmentFunction = fragShaderModule->getMTLFunction();
}

void RenderPipelineMTL::chooseAttachmentFormat(const RenderTarget* renderTarget,
                                               PixelFormat colorAttachmentsFormat[MAX_COLOR_ATTCHMENT],
                                               PixelFormat& depthFormat,
                                               PixelFormat& stencilFormat)
{
    // Choose color attachment format
    auto rtMTL   = static_cast<const RenderTargetMTL*>(renderTarget);
    for (auto i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
        colorAttachmentsFormat[i] = rtMTL->getColorAttachmentPixelFormat(i);

    depthFormat   = rtMTL->getDepthAttachmentPixelFormat();
    stencilFormat = rtMTL->getStencilAttachmentPixelFormat();
}

void RenderPipelineMTL::setBlendStateAndFormat(const BlendDescriptor& blendDescriptor)
{
    for (int i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
    {
        if (PixelFormat::NONE == _colorAttachmentsFormat[i])
        {
            _mtlRenderPipelineDescriptor.colorAttachments[i].pixelFormat = MTLPixelFormat::MTLPixelFormatInvalid;
            continue;
        }

        _mtlRenderPipelineDescriptor.colorAttachments[i].pixelFormat =
            UtilsMTL::toMTLPixelFormat(_colorAttachmentsFormat[i]);
        setBlendState(_mtlRenderPipelineDescriptor.colorAttachments[i], blendDescriptor);
    }
    _mtlRenderPipelineDescriptor.depthAttachmentPixelFormat   = UtilsMTL::toMTLPixelFormat(_depthAttachmentFormat);
    _mtlRenderPipelineDescriptor.stencilAttachmentPixelFormat = UtilsMTL::toMTLPixelFormat(_stencilAttachmentFormat);
}

NS_AX_BACKEND_END
