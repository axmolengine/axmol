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

#include "axmol/rhi/metal/RenderPipelineMTL.h"
#include "axmol/rhi/metal/DriverMTL.h"
#include "axmol/rhi/metal/RenderTargetMTL.h"
#include "axmol/rhi/metal/ShaderModuleMTL.h"
#include "axmol/rhi/metal/DepthStencilStateMTL.h"
#include "axmol/rhi/metal/UtilsMTL.h"
#include "axmol/rhi/metal/ProgramMTL.h"
#include "xxhash.h"

namespace ax::rhi::mtl
{

namespace
{
static MTLVertexFormat toMTLVertexFormat(VertexFormat vertexFormat, bool needNormalize)
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

static MTLColorWriteMask toMTLColorWriteMask(ColorWriteMask mask)
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

static MTLBlendFactor toMTLBlendFactor(BlendFactor factor)
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

static MTLBlendOperation toMTLBlendOp(BlendOp operation)
{
    switch (operation)
    {
    case BlendOp::ADD:
        return MTLBlendOperationAdd;
    case BlendOp::SUBTRACT:
        return MTLBlendOperationSubtract;
    case BlendOp::REVERSE_SUBTRACT:
        return MTLBlendOperationReverseSubtract;
    default:
        return MTLBlendOperationAdd;
    }
}
}  // namespace

RenderPipelineImpl::RenderPipelineImpl(id<MTLDevice> mtlDevice) : _mtlDevice(mtlDevice) {}

void RenderPipelineImpl::update(const RenderTarget* renderTarget, const PipelineDesc& pipelineDesc)
{
    struct
    {
        size_t vertexShaderHash;
        size_t fragmentShaderHash;
        unsigned int vertexLayoutInfo[32];
        uint32_t rtHash;  // The RenderTarget pixel formats hash
        bool blendEnabled;
        unsigned int writeMask;
        unsigned int rgbBlendOp;
        unsigned int alphaBlendOp;
        unsigned int sourceRGBBlendFactor;
        unsigned int destinationRGBBlendFactor;
        unsigned int sourceAlphaBlendFactor;
        unsigned int destinationAlphaBlendFactor;
    } hashMe;

    auto rtImpl = static_cast<const RenderTargetImpl*>(renderTarget);

    memset(&hashMe, 0, sizeof(hashMe));
    const auto& blendDesc              = pipelineDesc.blendDesc;
    auto program                       = pipelineDesc.programState->getProgram();
    hashMe.vertexShaderHash            = program->getVSModule()->getHashValue();
    hashMe.fragmentShaderHash          = program->getFSModule()->getHashValue();
    hashMe.rtHash                      = rtImpl->getHash();
    hashMe.blendEnabled                = blendDesc.blendEnabled;
    hashMe.writeMask                   = (unsigned int)blendDesc.writeMask;
    hashMe.rgbBlendOp                  = (unsigned int)blendDesc.rgbBlendOp;
    hashMe.alphaBlendOp                = (unsigned int)blendDesc.alphaBlendOp;
    hashMe.sourceRGBBlendFactor        = (unsigned int)blendDesc.sourceRGBBlendFactor;
    hashMe.destinationRGBBlendFactor   = (unsigned int)blendDesc.destinationRGBBlendFactor;
    hashMe.sourceAlphaBlendFactor      = (unsigned int)blendDesc.sourceAlphaBlendFactor;
    hashMe.destinationAlphaBlendFactor = (unsigned int)blendDesc.destinationAlphaBlendFactor;
    int index                          = 0;
    auto vertexLayout                  = pipelineDesc.vertexLayout;
    for (const auto& bindingDesc : vertexLayout->getBindings())
    {
        /*
         stepFunction:1     stride:15       offest:10       format:5        needNormalized:1
         bit31           bit30 ~ bit16   bit15 ~ bit6    bit5 ~ bit1     bit0
         */
        hashMe.vertexLayoutInfo[index++] = ((unsigned int)bindingDesc.instanceStepRate & 0x1) << 31 |
                                           ((unsigned int)(vertexLayout->getStride() & 0x7FFF)) << 16 |
                                           ((unsigned int)bindingDesc.offset & 0x3FF) << 6 |
                                           ((unsigned int)bindingDesc.format & 0x1F) << 1 |
                                           ((unsigned int)bindingDesc.needToBeNormallized & 0x1);
    }

    unsigned int hash = XXH32((const void*)&hashMe, sizeof(hashMe), 0);
    auto it           = _mtlStateCache.find(hash);
    if (it != _mtlStateCache.end())
    {
        _mtlRenderPipelineState = it->second;
        return;
    }

    _mtlRenderPipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];

    setShaderModules(program);
    setVertexLayout(_mtlRenderPipelineDesc, pipelineDesc);

    setBlendStateAndFormat(pipelineDesc.blendDesc, rtImpl);

    NSError* error          = nil;
    _mtlRenderPipelineState = [_mtlDevice newRenderPipelineStateWithDescriptor:_mtlRenderPipelineDesc error:&error];
    if (error)
        NSLog(@"Can not create renderpipeline state: %@", error);

    [_mtlRenderPipelineDesc release];

    _mtlStateCache.emplace(hash, _mtlRenderPipelineState);
}

RenderPipelineImpl::~RenderPipelineImpl()
{
    for (auto& item : _mtlStateCache)
        [item.second release];
}

void RenderPipelineImpl::setVertexLayout(MTLRenderPipelineDescriptor* mtlDesc, const PipelineDesc& desc)
{
    auto vertexLayout = desc.vertexLayout;
    assert(vertexLayout);

    auto vertexDesc                                                            = mtlDesc.vertexDescriptor;
    vertexDesc.layouts[DriverImpl::DEFAULT_ATTRIBS_BINDING_INDEX].stride       = vertexLayout->getStride();
    vertexDesc.layouts[DriverImpl::DEFAULT_ATTRIBS_BINDING_INDEX].stepFunction = MTLVertexStepFunctionPerVertex;

    unsigned int instanceAttribCount = 0;
    for (const auto& bindingDesc : vertexLayout->getBindings())
    {
        if (bindingDesc.format != VertexFormat::MAT4)
        {
            auto attrib   = vertexDesc.attributes[bindingDesc.index];
            attrib.format = toMTLVertexFormat(bindingDesc.format, bindingDesc.needToBeNormallized);
            attrib.offset = bindingDesc.offset;
            if (!bindingDesc.instanceStepRate)
            {
                attrib.bufferIndex = DriverImpl::DEFAULT_ATTRIBS_BINDING_INDEX;
            }
            else
            {
                attrib.bufferIndex = DriverImpl::VBO_INSTANCING_BINDING_INDEX;
                ++instanceAttribCount;
                vertexDesc.layouts[DriverImpl::VBO_INSTANCING_BINDING_INDEX].stepRate = bindingDesc.instanceStepRate;
            }
        }
        else
        {
            const uint32_t colStride = sizeof(float) * 4;
            for (uint32_t col = 0; col < 4; ++col)
            {
                auto attrib   = vertexDesc.attributes[bindingDesc.index + col];
                attrib.format = MTLVertexFormatFloat4;
                attrib.offset = bindingDesc.offset + col * colStride;
                if (!bindingDesc.instanceStepRate)
                {
                    attrib.bufferIndex = DriverImpl::DEFAULT_ATTRIBS_BINDING_INDEX;
                }
                else
                {
                    attrib.bufferIndex = DriverImpl::VBO_INSTANCING_BINDING_INDEX;
                    vertexDesc.layouts[DriverImpl::VBO_INSTANCING_BINDING_INDEX].stepRate =
                        bindingDesc.instanceStepRate;
                    ++instanceAttribCount;
                }
            }
        }
    }

    if (instanceAttribCount)
    {
        vertexDesc.layouts[DriverImpl::VBO_INSTANCING_BINDING_INDEX].stride       = vertexLayout->getInstanceStride();
        vertexDesc.layouts[DriverImpl::VBO_INSTANCING_BINDING_INDEX].stepFunction = MTLVertexStepFunctionPerInstance;
    }
}

void RenderPipelineImpl::setBlendState(MTLRenderPipelineColorAttachmentDescriptor* colorAttachmentDesc,
                                       const BlendDesc& blendDesc)
{
    colorAttachmentDesc.blendingEnabled = blendDesc.blendEnabled;
    colorAttachmentDesc.writeMask       = toMTLColorWriteMask(blendDesc.writeMask);

    colorAttachmentDesc.rgbBlendOperation   = toMTLBlendOp(blendDesc.rgbBlendOp);
    colorAttachmentDesc.alphaBlendOperation = toMTLBlendOp(blendDesc.alphaBlendOp);

    colorAttachmentDesc.sourceRGBBlendFactor        = toMTLBlendFactor(blendDesc.sourceRGBBlendFactor);
    colorAttachmentDesc.destinationRGBBlendFactor   = toMTLBlendFactor(blendDesc.destinationRGBBlendFactor);
    colorAttachmentDesc.sourceAlphaBlendFactor      = toMTLBlendFactor(blendDesc.sourceAlphaBlendFactor);
    colorAttachmentDesc.destinationAlphaBlendFactor = toMTLBlendFactor(blendDesc.destinationAlphaBlendFactor);
}

void RenderPipelineImpl::setShaderModules(Program* program)
{
    auto vsModule                         = static_cast<ShaderModuleImpl*>(program->getVSModule());
    _mtlRenderPipelineDesc.vertexFunction = vsModule->getMTLFunction();

    auto fsModule                           = static_cast<ShaderModuleImpl*>(program->getFSModule());
    _mtlRenderPipelineDesc.fragmentFunction = fsModule->getMTLFunction();
}

void RenderPipelineImpl::setBlendStateAndFormat(const BlendDesc& blendDesc, const RenderTargetImpl* rt)
{
    auto& nativeColorFormats = rt->getNativeColorFormats();
    auto formatCount         = nativeColorFormats.size();
    for (size_t i = 0; i < formatCount; ++i)
    {
        _mtlRenderPipelineDesc.colorAttachments[i].pixelFormat = nativeColorFormats[i];
        setBlendState(_mtlRenderPipelineDesc.colorAttachments[i], blendDesc);
    }

    auto nativePF                                       = rt->getNativeDSFormat();
    _mtlRenderPipelineDesc.depthAttachmentPixelFormat   = nativePF;
    _mtlRenderPipelineDesc.stencilAttachmentPixelFormat = nativePF;
}

}  // namespace ax::rhi::mtl
