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

#include "DriverMTL.h"
#include "CommandBufferMTL.h"
#include "BufferMTL.h"
#include "RenderPipelineMTL.h"
#include "ShaderModuleMTL.h"
#include "DepthStencilStateMTL.h"
#include "TextureMTL.h"
#include "ProgramMTL.h"
#include "RenderTargetMTL.h"
#include "UtilsMTL.h"
#include "base/Macros.h"

#include "renderer/backend/ProgramManager.h"

NS_AX_BACKEND_BEGIN

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

const char* featureSetToString(FeatureSet featureSet)
{
    switch (featureSet)
    {
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v1:
        return "iOS_GPUFamily1_v1";
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v1:
        return "iOS_GPUFamily2_v1";
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v2:
        return "iOS_GPUFamily1_v2";
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v2:
        return "iOS_GPUFamily2_v2";
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v3:
        return "iOS_GPUFamily1_v3";
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v3:
        return "iOS_GPUFamily2_v3";
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v4:
        return "iOS_GPUFamily1_v4";
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v4:
        return "iOS_GPUFamily2_v4";
    case FeatureSet::FeatureSet_iOS_GPUFamily1_v5:
        return "iOS_GPUFamily1_v5";
    case FeatureSet::FeatureSet_iOS_GPUFamily2_v5:
        return "iOS_GPUFamily2_v5";
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v1:
        return "iOS_GPUFamily3_v1";
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v2:
        return "iOS_GPUFamily3_v2";
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v3:
        return "iOS_GPUFamily3_v3";
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v1:
        return "iOS_GPUFamily4_v1";
    case FeatureSet::FeatureSet_iOS_GPUFamily3_v4:
        return "iOS_GPUFamily3_v4";
    case FeatureSet::FeatureSet_iOS_GPUFamily4_v2:
        return "iOS_GPUFamily4_v2";
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v1:
        return "macOS_GPUFamily1_v1";
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v2:
        return "macOS_GPUFamily1_v2";
    case FeatureSet::FeatureSet_macOS_ReadWriteTextureTier2:
        return "macOS_ReadWriteTextureTier2";
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v3:
        return "macOS_GPUFamily1_v3";
    case FeatureSet::FeatureSet_macOS_GPUFamily1_v4:
        return "macOS_GPUFamily1_v4";
    case FeatureSet::FeatureSet_macOS_GPUFamily2_v1:
        return "macOS_GPUFamily2_v1";
    default:
        break;
    }
    return "";
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

bool DriverMTL::_isDepth24Stencil8PixelFormatSupported = false;

CAMetalLayer* DriverMTL::_metalLayer            = nil;
id<CAMetalDrawable> DriverMTL::_currentDrawable = nil;

DriverBase* DriverBase::getInstance()
{
    if (!_instance)
        _instance = new DriverMTL();

    return _instance;
}

void DriverBase::destroyInstance() 
{
    AX_SAFE_DELETE(_instance);
}

void DriverMTL::setCAMetalLayer(CAMetalLayer* metalLayer)
{
    DriverMTL::_metalLayer = metalLayer;
}

id<CAMetalDrawable> DriverMTL::getCurrentDrawable()
{
    if (!DriverMTL::_currentDrawable)
        DriverMTL::_currentDrawable = [DriverMTL::_metalLayer nextDrawable];

    return DriverMTL::_currentDrawable;
}

void DriverMTL::resetCurrentDrawable()
{
    DriverMTL::_currentDrawable = nil;
}

DriverMTL::DriverMTL()
{
    _mtlDevice       = DriverMTL::_metalLayer.device;
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

DriverMTL::~DriverMTL()
{
    ProgramManager::destroyInstance();
}

CommandBuffer* DriverMTL::newCommandBuffer()
{
    return new CommandBufferMTL(this);
}

Buffer* DriverMTL::newBuffer(std::size_t size, BufferType type, BufferUsage usage)
{
    return new BufferMTL(_mtlDevice, size, type, usage);
}

TextureBackend* DriverMTL::newTexture(const TextureDescriptor& descriptor)
{
    switch (descriptor.textureType)
    {
    case TextureType::TEXTURE_2D:
        return new TextureMTL(_mtlDevice, descriptor);
    case TextureType::TEXTURE_CUBE:
        return new TextureCubeMTL(_mtlDevice, descriptor);
    default:
        AXASSERT(false, "invalidate texture type");
        return nullptr;
    }
}

RenderTarget* DriverMTL::newDefaultRenderTarget(TargetBufferFlags rtf)
{
    auto rtGL = new RenderTargetMTL(true);
    rtGL->setTargetFlags(rtf);
    return rtGL;
}

RenderTarget* DriverMTL::newRenderTarget(TargetBufferFlags rtf,
                                         TextureBackend* colorAttachment,
                                         TextureBackend* depthAttachment,
                                         TextureBackend* stencilAttachhment)
{
    auto rtMTL = new RenderTargetMTL(false);
    rtMTL->setTargetFlags(rtf);
    RenderTarget::ColorAttachment colors{{colorAttachment, 0}};
    rtMTL->setColorAttachment(colors);
    rtMTL->setDepthAttachment(depthAttachment);
    rtMTL->setStencilAttachment(stencilAttachhment);
    return rtMTL;
}

ShaderModule* DriverMTL::newShaderModule(ShaderStage stage, std::string_view source)
{
    return new ShaderModuleMTL(_mtlDevice, stage, source);
}

DepthStencilState* DriverMTL::newDepthStencilState()
{
    return new DepthStencilStateMTL(_mtlDevice);
}

RenderPipeline* DriverMTL::newRenderPipeline()
{
    return new RenderPipelineMTL(_mtlDevice);
}

Program* DriverMTL::newProgram(std::string_view vertexShader, std::string_view fragmentShader)
{
    return new ProgramMTL(vertexShader, fragmentShader);
}

void DriverMTL::setFrameBufferOnly(bool frameBufferOnly)
{
    [DriverMTL::_metalLayer setFramebufferOnly:frameBufferOnly];
}

const char* DriverMTL::getVendor() const
{
    return "";
}

const char* DriverMTL::getRenderer() const
{
    return _deviceName.c_str();
}

const char* DriverMTL::getVersion() const
{
    return featureSetToString(_featureSet);
}

bool DriverMTL::checkForFeatureSupported(FeatureType feature)
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

NS_AX_BACKEND_END
