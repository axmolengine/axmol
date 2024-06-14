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

#pragma once

#include "Macros.h"
#include "Types.h"
#include "RenderPassDescriptor.h"
#include "Texture.h"
#include "DepthStencilState.h"
#include "ShaderCache.h"

#include "base/Object.h"

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

enum class FeatureType : uint32_t
{
    ETC1,
    ETC2,
    S3TC,
    AMD_COMPRESSED_ATC,
    PVRTC,
    IMG_FORMAT_BGRA8888,
    DISCARD_FRAMEBUFFER,
    PACKED_DEPTH_STENCIL,
    VAO,
    MAPBUFFER,
    DEPTH24,
    ASTC
};

/**
 * @addtogroup _backend
 * @{
 */

/**
 * New or create resources from DriverBase.
 */
class AX_DLL DriverBase
{
public:
    friend class ProgramManager;
    friend class ShaderCache;

    /**
     * Returns a shared instance of the DriverBase.
     */
    static DriverBase* getInstance();
    static void destroyInstance();

    virtual ~DriverBase() = default;

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

    virtual RenderTarget* newDefaultRenderTarget() = 0;

    virtual RenderTarget* newRenderTarget(TextureBackend* colorAttachment    = nullptr,
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

    virtual void resetState() {};

    /// below is driver info
     /**
     * Get vendor device name.
     * @return Vendor device name.
     */
    virtual const char* getVendor() const = 0;

    /**
     * Get the full name of the vendor device.
     * @return The full name of the vendor device.
     */
    virtual const char* getRenderer() const = 0;

    /**
     * Get version name.
     * @return Version name.
     */
    virtual const char* getVersion() const = 0;

    virtual const char* getShaderVersion() const { return ""; }

    /**
     * Check does device has extension.
     */
    virtual bool hasExtension(std::string_view /*extName*/) const { return false; };

    /**
    * Dump all extensions to string
    */
    virtual std::string dumpExtensions() const { return {}; };

    /**
     * Check if feature supported by device.
     * @param feature Specify feature to be query.
     * @return true if the feature is supported, false otherwise.
     */
    virtual bool checkForFeatureSupported(FeatureType feature) = 0;

    /**
     * Get maximum texture size.
     * @return Maximum texture size.
     */
    inline int getMaxTextureSize() const { return _maxTextureSize; }

    /**
     * Get maximum attribute counts.
     * @return Maximum attribute counts.
     */
    inline int getMaxAttributes() const { return _maxAttributes; }

    /**
     * Get maximum texture unit.
     * @return Maximum texture unit.
     */
    inline int getMaxTextureUnits() const { return _maxTextureUnits; }

    /**
     * Get maximum sampler count.
     * @return Maximum sampler count.
     */
    inline int getMaxSamplesAllowed() const { return _maxSamplesAllowed; }

protected:
    /**
     * New a shaderModule, not auto released.
     * @param stage Specifies whether is vertex shader or fragment shader.
     * @param source Specifies shader source.
     * @return A ShaderModule object.
     */
    virtual ShaderModule* newShaderModule(ShaderStage stage, std::string_view source) = 0;

    int _maxAttributes     = 0;  ///< Maximum attribute count.
    int _maxTextureSize    = 0;  ///< Maximum texture size.
    int _maxTextureUnits   = 0;  ///< Maximum texture unit.
    int _maxSamplesAllowed = 0;  ///< Maximum sampler count.

private:
    static DriverBase* _instance;
};

// end of _backend group
/// @}
NS_AX_BACKEND_END
