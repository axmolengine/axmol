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

#include "axmol/rhi/metal/DriverMTL.h"
#include "axmol/rhi/metal/CommandBufferMTL.h"
#include "axmol/rhi/metal/BufferMTL.h"
#include "axmol/rhi/metal/RenderPipelineMTL.h"
#include "axmol/rhi/metal/ShaderModuleMTL.h"
#include "axmol/rhi/metal/DepthStencilStateMTL.h"
#include "axmol/rhi/metal/TextureMTL.h"
#include "axmol/rhi/metal/ProgramMTL.h"
#include "axmol/rhi/metal/RenderTargetMTL.h"
#include "axmol/rhi/metal/UtilsMTL.h"
#include "axmol/base/Macros.h"

namespace ax::rhi {
DriverBase* DriverBase::getInstance()
{
    if (!_instance)
        _instance = new mtl::DriverImpl();

    return _instance;
}

void DriverBase::destroyInstance()
{
    if(_instance) delete _instance;
}
}

namespace ax::rhi::mtl {

inline FeatureSet operator--(FeatureSet& x)
{
    return x = (FeatureSet)(std::underlying_type<FeatureSet>::type(x) - 1);
}

namespace
{
int getMaxVertexAttributes(FeatureSet featureSet)
{
    int maxAttributes = 0;
    switch (featureSet)
    {
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v5:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v5:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v2:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v1:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v2:
    case FeatureSet::FeatureSet_macOS_ReadWriteTextureTier2:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v3:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v4:
    case FeatureSet::FeatureSet_macOS_GPUFamily2_v1:
        maxAttributes = 31;
        break;
    default:
        break;
    }
    return maxAttributes;
}

int getMaxTextureEntries(FeatureSet featureSet)
{
    int maxTextureEntries = 0;
    switch (featureSet)
    {
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v5:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v5:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v2:
        maxTextureEntries = 31;
        break;
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v1:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v2:
    case FeatureSet::FeatureSet_macOS_ReadWriteTextureTier2:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v3:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v4:
    case FeatureSet::FeatureSet_macOS_GPUFamily2_v1:
        maxTextureEntries = 128;
        break;
    default:
        break;
    }
    return maxTextureEntries;
}

int getMaxSamplerEntries(FeatureSet featureSet)
{
    int maxSamplerEntries = 0;
    switch (featureSet)
    {
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v5:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v5:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v2:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v1:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v2:
    case FeatureSet::FeatureSet_macOS_ReadWriteTextureTier2:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v3:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v4:
    case FeatureSet::FeatureSet_macOS_GPUFamily2_v1:
        maxSamplerEntries = 16;
        break;
    default:
        break;
    }
    return maxSamplerEntries;
}

int getMaxTextureWidthHeight(FeatureSet featureSet)
{
    int maxTextureSize = 0;
    switch (featureSet)
    {
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v1:
        maxTextureSize = 4096;
        break;
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v5:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v5:
        maxTextureSize = 8192;
        break;
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v2:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v1:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v2:
    case FeatureSet::FeatureSet_macOS_ReadWriteTextureTier2:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v3:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v4:
    case FeatureSet::FeatureSet_macOS_GPUFamily2_v1:
        maxTextureSize = 16384;
        break;
    default:
        break;
    }
    return maxTextureSize;
}

std::string_view featureSetToString(FeatureSet featureSet)
{
    switch (featureSet)
    {
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v1:
        return "iOS_GPUFamily1_v1"sv;
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v1:
        return "iOS_GPUFamily2_v1"sv;
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v2:
        return "iOS_GPUFamily1_v2"sv;
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v2:
        return "iOS_GPUFamily2_v2"sv;
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v3:
        return "iOS_GPUFamily1_v3"sv;
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v3:
        return "iOS_GPUFamily2_v3"sv;
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v4:
        return "iOS_GPUFamily1_v4"sv;
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v4:
        return "iOS_GPUFamily2_v4"sv;
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v5:
        return "iOS_GPUFamily1_v5"sv;
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v5:
        return "iOS_GPUFamily2_v5"sv;
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v1:
        return "iOS_GPUFamily3_v1"sv;
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v2:
        return "iOS_GPUFamily3_v2"sv;
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v3:
        return "iOS_GPUFamily3_v3"sv;
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v1:
        return "iOS_GPUFamily4_v1"sv;
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v4:
        return "iOS_GPUFamily3_v4"sv;
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v2:
        return "iOS_GPUFamily4_v2"sv;
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v1:
        return "macOS_GPUFamily1_v1"sv;
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v2:
        return "macOS_GPUFamily1_v2"sv;
    case FeatureSet::FeatureSet_macOS_ReadWriteTextureTier2:
        return "macOS_ReadWriteTextureTier2"sv;
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v3:
        return "macOS_GPUFamily1_v3"sv;
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v4:
        return "macOS_GPUFamily1_v4"sv;
    case FeatureSet::FeatureSet_macOS_GPUFamily2_v1:
        return "macOS_GPUFamily2_v1"sv;
    default:
        break;
    }
    return ""sv;
}

bool supportPVRTC(FeatureSet featureSet)
{
    switch (featureSet)
    {
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v5:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v5:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v2:
        return true;
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v1:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v2:
    case FeatureSet::FeatureSet_macOS_ReadWriteTextureTier2:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v3:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v4:
    case FeatureSet::FeatureSet_macOS_GPUFamily2_v1:
        return false;
    default:
        break;
    }
    return false;
}

bool supportEACETC(FeatureSet featureSet)
{
    switch (featureSet)
    {
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v5:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v5:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v2:
        return true;
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v1:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v2:
    case FeatureSet::FeatureSet_macOS_ReadWriteTextureTier2:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v3:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v4:
    case FeatureSet::FeatureSet_macOS_GPUFamily2_v1:
        return false;
    default:
        break;
    }
    return false;
}

bool supportASTC(FeatureSet featureSet)
{
    switch (featureSet)
    {
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v5:
        return false;
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v5:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v2:
        return true;
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v1:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v2:
    case FeatureSet::FeatureSet_macOS_ReadWriteTextureTier2:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v3:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v4:
    case FeatureSet::FeatureSet_macOS_GPUFamily2_v1:
        return false;
    default:
        break;
    }
    return false;
}

bool supportS3TC(FeatureSet featureSet)
{
    switch (featureSet)
    {
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v5:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v5:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v2:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v3:
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v1:
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v4:
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v2:
        return false;
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v1:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v2:
    case FeatureSet::FeatureSet_macOS_ReadWriteTextureTier2:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v3:
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v4:
    case FeatureSet::FeatureSet_macOS_GPUFamily2_v1:
        return true;
    default:
        break;
    }
    return false;
}
}

bool DriverImpl::_isDepth24Stencil8PixelFormatSupported = false;

CAMetalLayer* DriverImpl::_metalLayer            = nil;
id<CAMetalDrawable> DriverImpl::_currentDrawable = nil;

void DriverImpl::setCAMetalLayer(CAMetalLayer* metalLayer)
{
    DriverImpl::_metalLayer = metalLayer;
}

id<CAMetalDrawable> DriverImpl::getCurrentDrawable()
{
    if (!DriverImpl::_currentDrawable)
        DriverImpl::_currentDrawable = [DriverImpl::_metalLayer nextDrawable];

    return DriverImpl::_currentDrawable;
}

void DriverImpl::resetCurrentDrawable()
{
    DriverImpl::_currentDrawable = nil;
}

DriverImpl::DriverImpl()
{
    _mtlDevice       = DriverImpl::_metalLayer.device;
    _mtlCommandQueue = [_mtlDevice newCommandQueue];

    _deviceName = [_mtlDevice.name UTF8String];

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
    const FeatureSet minRequiredFeatureSet = FeatureSet::FeatureSet_iOS_GPUFamily1_v1;
    const FeatureSet maxKnownFeatureSet    = FeatureSet::FeatureSet_iOS_GPUFamily4_v2;
#else
    const FeatureSet minRequiredFeatureSet = FeatureSet::FeatureSet_macOS_GPUFamily1_v1;
    const FeatureSet maxKnownFeatureSet    = FeatureSet::FeatureSet_macOS_GPUFamily2_v1;
    _isDepth24Stencil8PixelFormatSupported = [_mtlDevice isDepth24Stencil8PixelFormatSupported];
#endif

    for (auto featureSet = maxKnownFeatureSet; featureSet >= minRequiredFeatureSet; --featureSet)
    {
        if ([_mtlDevice supportsFeatureSet:MTLFeatureSet(featureSet)])
        {
            _featureSet = featureSet;
            break;
        }
    }

    UtilsMTL::initGPUTextureFormats();

    _maxAttributes     = getMaxVertexAttributes(_featureSet);
    _maxSamplesAllowed = getMaxSamplerEntries(_featureSet);
    _maxTextureUnits   = getMaxTextureEntries(_featureSet);
    _maxTextureSize    = getMaxTextureWidthHeight(_featureSet);
}

DriverImpl::~DriverImpl()
{
}

CommandBuffer* DriverImpl::createCommandBuffer(void*)
{
    return new CommandBufferImpl(this);
}

Buffer* DriverImpl::createBuffer(std::size_t size, BufferType type, BufferUsage usage)
{
    return new BufferImpl(_mtlDevice, size, type, usage);
}

Texture* DriverImpl::createTexture(const TextureDesc& descriptor)
{
    return new TextureImpl(_mtlDevice, descriptor);
}

RenderTarget* DriverImpl::createDefaultRenderTarget()
{
    return new RenderTargetImpl(true);
}

RenderTarget* DriverImpl::createRenderTarget(Texture* colorAttachment,
                                         Texture* depthStencilAttachment)
{
    auto rtMTL = new RenderTargetImpl(false);
    RenderTarget::ColorAttachment colors{{colorAttachment, 0}};
    rtMTL->setColorAttachment(colors);
    rtMTL->setDepthStencilAttachment(depthStencilAttachment);
    return rtMTL;
}

DepthStencilState* DriverImpl::createDepthStencilState()
{
    return new DepthStencilStateImpl(_mtlDevice);
}

RenderPipeline* DriverImpl::createRenderPipeline()
{
    return new RenderPipelineImpl(_mtlDevice);
}

Program* DriverImpl::createProgram(std::string_view vertexShader, std::string_view fragmentShader)
{
    return new ProgramImpl(vertexShader, fragmentShader);
}

ShaderModule* DriverImpl::createShaderModule(ShaderStage stage, std::string_view source)
{
    return new ShaderModuleImpl(_mtlDevice, stage, source);
}

SamplerHandle DriverImpl::createSampler(const SamplerDesc& desc)
{
    MTLSamplerDescriptor* samplerDesc = [[MTLSamplerDescriptor alloc] init];

    // --- Min & Mag Filter ---
    switch (desc.minFilter)
    {
        case SamplerFilter::MIN_NEAREST:
            samplerDesc.minFilter = MTLSamplerMinMagFilterNearest;
            break;
        case SamplerFilter::MIN_LINEAR:
        case SamplerFilter::MIN_ANISOTROPIC:
            samplerDesc.minFilter = MTLSamplerMinMagFilterLinear;
            break;
    }

    samplerDesc.magFilter = (desc.magFilter == SamplerFilter::MAG_NEAREST)
                            ? MTLSamplerMinMagFilterNearest
                            : MTLSamplerMinMagFilterLinear;

    // --- Mip Filter ---
    switch (desc.mipFilter)
    {
        case SamplerFilter::MIP_NONE:
            samplerDesc.mipFilter = MTLSamplerMipFilterNotMipmapped;
            break;
        case SamplerFilter::MIP_NEAREST:
            samplerDesc.mipFilter = MTLSamplerMipFilterNearest;
            break;
        case SamplerFilter::MIP_LINEAR:
            samplerDesc.mipFilter = MTLSamplerMipFilterLinear;
            break;
    }

    // --- Address Modes ---
    auto toMTLAddressMode = [](SamplerAddressMode mode) -> MTLSamplerAddressMode {
        switch (mode)
        {
            case SamplerAddressMode::REPEAT: return MTLSamplerAddressModeRepeat;
            case SamplerAddressMode::MIRROR: return MTLSamplerAddressModeMirrorRepeat;
            case SamplerAddressMode::CLAMP: return MTLSamplerAddressModeClampToEdge;
            case SamplerAddressMode::BORDER: return MTLSamplerAddressModeClampToBorderColor;
        }
        return MTLSamplerAddressModeRepeat;
    };

    samplerDesc.sAddressMode = toMTLAddressMode(desc.sAddressMode);
    samplerDesc.tAddressMode = toMTLAddressMode(desc.tAddressMode);
    samplerDesc.rAddressMode = toMTLAddressMode(desc.wAddressMode);

    // --- Border Color ---
    if (desc.sAddressMode == SamplerAddressMode::BORDER ||
        desc.tAddressMode == SamplerAddressMode::BORDER ||
        desc.wAddressMode == SamplerAddressMode::BORDER)
    {
        samplerDesc.borderColor = MTLSamplerBorderColorTransparentBlack;
    }

    // --- Compare Function ---
    auto toMTLCompareFunc = [](CompareFunc func) ->MTLCompareFunction {
        if(func == CompareFunc::NEVER)
            return MTLCompareFunctionNever;
        switch (func)
        {
            case CompareFunc::LESS:          return MTLCompareFunctionLess;
            case CompareFunc::EQUAL:         return MTLCompareFunctionEqual;
            case CompareFunc::LESS_EQUAL:    return MTLCompareFunctionLessEqual;
            case CompareFunc::GREATER:       return MTLCompareFunctionGreater;
            case CompareFunc::NOT_EQUAL:     return MTLCompareFunctionNotEqual;
            case CompareFunc::GREATER_EQUAL: return MTLCompareFunctionGreaterEqual;
            case CompareFunc::ALWAYS:        return MTLCompareFunctionAlways;
            default:                         return MTLCompareFunctionNever;
        }
    };
    samplerDesc.compareFunction = toMTLCompareFunc(desc.compareFunc);

    // --- Anisotropy ---
    samplerDesc.maxAnisotropy = desc.anisotropy;

    // --- Create Sampler ---
    id<MTLSamplerState> sampler = [_mtlDevice newSamplerStateWithDescriptor:samplerDesc];
    [samplerDesc release];

    return (__bridge SamplerHandle)sampler;
}

void DriverImpl::destroySampler(SamplerHandle& sampler)
{
    if(sampler) {
        [reinterpret_cast<id<MTLSamplerState>>(sampler) release];
        sampler = nullptr;
    }
}

void DriverImpl::setFrameBufferOnly(bool frameBufferOnly)
{
    [DriverImpl::_metalLayer setFramebufferOnly:frameBufferOnly];
}

std::string DriverImpl::getVendor() const
{
    return "Apple Inc.";
}

std::string DriverImpl::getRenderer() const
{
    return _deviceName.c_str();
}

std::string DriverImpl::getVersion() const
{
    return std::string{featureSetToString(_featureSet)};
}

bool DriverImpl::checkForFeatureSupported(FeatureType feature)
{
    bool featureSupported = false;
    switch (feature)
    {
    case FeatureType::PVRTC:
        featureSupported = supportPVRTC(_featureSet);
        break;
    case FeatureType::ETC1:
    case FeatureType::ETC2:
        featureSupported = supportEACETC(_featureSet);
        break;
    case FeatureType::S3TC:
        featureSupported = supportS3TC(_featureSet);
        break;
    case FeatureType::IMG_FORMAT_BGRA8888:
        featureSupported = true;
        break;
    case FeatureType::PACKED_DEPTH_STENCIL:
        featureSupported = _isDepth24Stencil8PixelFormatSupported;
        break;
    case FeatureType::ASTC:
        featureSupported = supportASTC(_featureSet);
        break;
    default:
        break;
    }
    return featureSupported;
}

}
