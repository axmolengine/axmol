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

#include "DeviceGL.h"
#include "RenderPipelineGL.h"
#include "BufferGL.h"
#include "ShaderModuleGL.h"
#include "CommandBufferGL.h"
#include "TextureGL.h"
#include "DepthStencilStateGL.h"
#include "ProgramGL.h"
#include "DeviceInfoGL.h"
#include "RenderTargetGL.h"

#include "renderer/backend/ProgramManager.h"

NS_AX_BACKEND_BEGIN

Device* Device::getInstance()
{
    if (!_instance)
        _instance = new DeviceGL();

    return _instance;
}

DeviceGL::DeviceGL()
{
    _deviceInfo = new DeviceInfoGL();
    if (!_deviceInfo->init())
    {
        delete _deviceInfo;
        _deviceInfo = nullptr;
    }

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_defaultFBO);
}

DeviceGL::~DeviceGL()
{
    ProgramManager::destroyInstance();
    delete _deviceInfo;
    _deviceInfo = nullptr;
}

GLint DeviceGL::getDefaultFBO() const
{
    return _defaultFBO;
}

CommandBuffer* DeviceGL::newCommandBuffer()
{
    return new CommandBufferGL();
}

Buffer* DeviceGL::newBuffer(std::size_t size, BufferType type, BufferUsage usage)
{
    return new BufferGL(size, type, usage);
}

TextureBackend* DeviceGL::newTexture(const TextureDescriptor& descriptor)
{
    switch (descriptor.textureType)
    {
    case TextureType::TEXTURE_2D:
        return new Texture2DGL(descriptor);
    case TextureType::TEXTURE_CUBE:
        return new TextureCubeGL(descriptor);
    default:
        return nullptr;
    }
}

RenderTarget* DeviceGL::newDefaultRenderTarget(TargetBufferFlags rtf)
{
    auto rtGL = new RenderTargetGL(true, this);
    rtGL->setTargetFlags(rtf);
    return rtGL;
}

RenderTarget* DeviceGL::newRenderTarget(TargetBufferFlags rtf,
                                        TextureBackend* colorAttachment,
                                        TextureBackend* depthAttachment,
                                        TextureBackend* stencilAttachhment)
{
    auto rtGL = new RenderTargetGL(false, this);
    rtGL->setTargetFlags(rtf);
    rtGL->bindFrameBuffer();
    RenderTarget::ColorAttachment colors{{colorAttachment, 0}};
    rtGL->setColorAttachment(colors);
    rtGL->setDepthAttachment(depthAttachment);
    rtGL->setStencilAttachment(stencilAttachhment);
    return rtGL;
}

ShaderModule* DeviceGL::newShaderModule(ShaderStage stage, std::string_view source)
{
    return new ShaderModuleGL(stage, source);
}

DepthStencilState* DeviceGL::newDepthStencilState()
{
    return new DepthStencilStateGL();
}

RenderPipeline* DeviceGL::newRenderPipeline()
{
    return new RenderPipelineGL();
}

Program* DeviceGL::newProgram(std::string_view vertexShader, std::string_view fragmentShader)
{
    return new ProgramGL(vertexShader, fragmentShader);
}

NS_AX_BACKEND_END
