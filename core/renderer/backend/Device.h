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

#pragma once

#include "Macros.h"
#include "Types.h"
#include "RenderPassDescriptor.h"
#include "Texture.h"
#include "DepthStencilState.h"
#include "ShaderCache.h"
#include "DeviceInfo.h"

#include "base/CCRef.h"

#include <string>

NS_AX_BACKEND_BEGIN

class CommandBuffer;
class Buffer;
class ShaderModule;
class RenderPipeline;
class RenderPass;
class RenderTarget;

class ProgramManager;
class Program;

/**
 * @addtogroup _backend
 * @{
 */

/**
 * New or create resources from Device.
 */
class AX_DLL Device : public ax::Ref
{
public:
    friend class ProgramManager;
    friend class ShaderCache;

    /**
     * Returns a shared instance of the device.
     */
    static Device* getInstance();

    virtual ~Device() = default;

    /**
     * New a CommandBuffer object, not auto released.
     * @return A CommandBuffer object.
     */
    virtual CommandBuffer* newCommandBuffer() = 0;

    /**
     * New a Buffer object, not auto released.
     * @param size Specifies the size in bytes of the buffer object's new data store.
     * @param type Specifies the target buffer object. The symbolic constant must be BufferType::VERTEX or
     * BufferType::INDEX.
     * @param usage Specifies the expected usage pattern of the data store. The symbolic constant must be
     * BufferUsage::STATIC, BufferUsage::DYNAMIC.
     * @return A Buffer object.
     */
    virtual Buffer* newBuffer(size_t size, BufferType type, BufferUsage usage) = 0;

    /**
     * New a TextureBackend object, not auto released.
     * @param descriptor Specifies texture description.
     * @return A TextureBackend object.
     */
    virtual TextureBackend* newTexture(const TextureDescriptor& descriptor) = 0;

    virtual RenderTarget* newDefaultRenderTarget(TargetBufferFlags rtf) = 0;

    virtual RenderTarget* newRenderTarget(TargetBufferFlags rtf,
                                          TextureBackend* colorAttachment    = nullptr,
                                          TextureBackend* depthAttachment    = nullptr,
                                          TextureBackend* stencilAttachhment = nullptr) = 0;

    virtual DepthStencilState* newDepthStencilState() = 0;

    /**
     * New a RenderPipeline object, not auto released.
     * @param descriptor Specifies render pipeline description.
     * @return A RenderPipeline object.
     */
    virtual RenderPipeline* newRenderPipeline() = 0;

    /**
     * This property controls whether or not the drawables'
     * metal textures may only be used for framebuffer attachments (YES) or
     * whether they may also be used for texture sampling and pixel
     * read/write operations (NO).
     * @param frameBufferOnly A value of YES allows CAMetalLayer to allocate the MTLTexture objects in ways that are
     * optimized for display purposes that makes them unsuitable for sampling. The recommended value for most
     * applications is YES.
     * @note This interface is specificaly designed for metal.
     */
    virtual void setFrameBufferOnly(bool frameBufferOnly) = 0;

    /**
     * Create an auto released Program.
     * @param vertexShader Specifes this is a vertex shader source.
     * @param fragmentShader Specifes this is a fragment shader source.
     * @return A Program instance.
     */
    virtual Program* newProgram(std::string_view vertexShader, std::string_view fragmentShader) = 0;

    /**
     * Get a DeviceInfo object.
     * @return A DeviceInfo object.
     */
    inline DeviceInfo* getDeviceInfo() const { return _deviceInfo; }

protected:
    /**
     * New a shaderModule, not auto released.
     * @param stage Specifies whether is vertex shader or fragment shader.
     * @param source Specifies shader source.
     * @return A ShaderModule object.
     */
    virtual ShaderModule* newShaderModule(ShaderStage stage, std::string_view source) = 0;

    DeviceInfo* _deviceInfo = nullptr;  ///< Device information.

private:
    static Device* _instance;
};

// end of _backend group
/// @}
NS_AX_BACKEND_END
