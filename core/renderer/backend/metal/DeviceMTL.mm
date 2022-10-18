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

#include "DeviceMTL.h"
#include "CommandBufferMTL.h"
#include "BufferMTL.h"
#include "RenderPipelineMTL.h"
#include "ShaderModuleMTL.h"
#include "DepthStencilStateMTL.h"
#include "TextureMTL.h"
#include "ProgramMTL.h"
#include "DeviceInfoMTL.h"
#include "RenderTargetMTL.h"

#include "base/ccMacros.h"

#include "renderer/backend/ProgramManager.h"

NS_AX_BACKEND_BEGIN

CAMetalLayer* DeviceMTL::_metalLayer            = nil;
id<CAMetalDrawable> DeviceMTL::_currentDrawable = nil;

Device* Device::getInstance()
{
    if (!Device::_instance)
        Device::_instance = new DeviceMTL();

    return Device::_instance;
}

void DeviceMTL::setCAMetalLayer(CAMetalLayer* metalLayer)
{
    DeviceMTL::_metalLayer = metalLayer;
}

id<CAMetalDrawable> DeviceMTL::getCurrentDrawable()
{
    if (!DeviceMTL::_currentDrawable)
        DeviceMTL::_currentDrawable = [DeviceMTL::_metalLayer nextDrawable];

    return DeviceMTL::_currentDrawable;
}

void DeviceMTL::resetCurrentDrawable()
{
    DeviceMTL::_currentDrawable = nil;
}

DeviceMTL::DeviceMTL()
{
    _mtlDevice       = DeviceMTL::_metalLayer.device;
    _mtlCommandQueue = [_mtlDevice newCommandQueue];
    _deviceInfo      = new DeviceInfoMTL(_mtlDevice);
    if (!_deviceInfo->init())
    {
        delete _deviceInfo;
        _deviceInfo = nullptr;
    }
}

DeviceMTL::~DeviceMTL()
{
    ProgramManager::destroyInstance();
    delete _deviceInfo;
    _deviceInfo = nullptr;
}

CommandBuffer* DeviceMTL::newCommandBuffer()
{
    return new CommandBufferMTL(this);
}

Buffer* DeviceMTL::newBuffer(std::size_t size, BufferType type, BufferUsage usage)
{
    return new BufferMTL(_mtlDevice, size, type, usage);
}

TextureBackend* DeviceMTL::newTexture(const TextureDescriptor& descriptor)
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

RenderTarget* DeviceMTL::newDefaultRenderTarget(TargetBufferFlags rtf)
{
    auto rtGL = new RenderTargetMTL(true);
    rtGL->setTargetFlags(rtf);
    return rtGL;
}

RenderTarget* DeviceMTL::newRenderTarget(TargetBufferFlags rtf,
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

ShaderModule* DeviceMTL::newShaderModule(ShaderStage stage, std::string_view source)
{
    return new ShaderModuleMTL(_mtlDevice, stage, source);
}

DepthStencilState* DeviceMTL::newDepthStencilState()
{
    return new DepthStencilStateMTL(_mtlDevice);
}

RenderPipeline* DeviceMTL::newRenderPipeline()
{
    return new RenderPipelineMTL(_mtlDevice);
}

Program* DeviceMTL::newProgram(std::string_view vertexShader, std::string_view fragmentShader)
{
    return new ProgramMTL(vertexShader, fragmentShader);
}

void DeviceMTL::setFrameBufferOnly(bool frameBufferOnly)
{
    [DeviceMTL::_metalLayer setFramebufferOnly:frameBufferOnly];
}

NS_AX_BACKEND_END
