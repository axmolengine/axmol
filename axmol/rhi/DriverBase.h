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

#include "axmol/rhi/RHITypes.h"
#include "axmol/rhi/RenderPassDesc.h"
#include "axmol/rhi/Texture.h"
#include "axmol/rhi/DepthStencilState.h"
#include "axmol/rhi/ShaderCache.h"

#include "axmol/base/Object.h"

namespace ax::rhi
{

class RenderContext;
class Buffer;
class ShaderModule;
class RenderPipeline;
class RenderPass;
class RenderTarget;

class Program;
class VertexLayout;

class SamplerCache;

struct VertexLayoutDesc;

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
    ASTC,
    VERTEX_ATTRIB_BINDING,  // GL330 / GLES30, need detect
};

/**
 * @addtogroup _rhi
 * @{
 */

struct DriverCaps
{
    int maxAttributes     = 0;  ///< Maximum attribute count.
    int maxTextureSize    = 0;  ///< Maximum texture size.
    int maxTextureUnits   = 0;  ///< Maximum texture unit.
    int maxSamplesAllowed = 0;  ///< Maximum sampler count.
};

/**
 * New or create resources from DriverBase.
 */
class AX_DLL DriverBase
{
public:
    struct Caps
    {};
    friend class ShaderCache;
    friend class SamplerCache;

    /**
     * Returns a shared instance of the DriverBase.
     */
    static DriverBase* getInstance();
    static void destroyInstance();

    virtual ~DriverBase() = default;

    /**
     * New a Render Context object, not auto released.
     * @param surfaceContext, current is win32 HWND or IUnkown*(swapChainPanel)
     * @return A RenderContext object.
     */
    virtual RenderContext* createRenderContext(void* surfaceContext) = 0;

    /**
     * New a Buffer object, not auto released.
     * @param size Specifies the size in bytes of the buffer object's new data store.
     * @param type Specifies the target buffer object. The symbolic constant must be BufferType::VERTEX or
     * BufferType::INDEX.
     * @param usage Specifies the expected usage pattern of the data store. The symbolic constant must be
     * BufferUsage::STATIC, BufferUsage::DYNAMIC.
     * @return A Buffer object.
     */
    virtual Buffer* createBuffer(size_t size, BufferType type, BufferUsage usage, const void* inital = nullptr) = 0;

    /**
     * New a Texture object, not auto released.
     * @param descriptor Specifies texture description.
     * @return A Texture object.
     */
    virtual Texture* createTexture(const TextureDesc& descriptor) = 0;

    virtual RenderTarget* createRenderTarget(Texture* colorAttachment        = nullptr,
                                             Texture* depthStencilAttachment = nullptr) = 0;

    virtual DepthStencilState* createDepthStencilState() = 0;

    /**
     * New a RenderPipeline object, not auto released.
     * @param descriptor Specifies render pipeline description.
     * @return A RenderPipeline object.
     */
    virtual RenderPipeline* createRenderPipeline() = 0;

    /**
     * Create an auto released Program.
     * @param vsData Specifes this is a vertex shader data.
     * @param fsData Specifes this is a fragment shader data.
     * @return A Program instance.
     */
    virtual Program* createProgram(Data vsData, Data fsData) = 0;

    virtual VertexLayout* createVertexLayout(VertexLayoutDesc&& desc);

    virtual void resetState() {};

    /// below is driver info
    /**
     * Get vendor device name.
     * @return Vendor device name.
     */
    virtual std::string getVendor() const = 0;

    /**
     * Get the full name of the vendor device.
     * @return The full name of the vendor device.
     */
    virtual std::string getRenderer() const = 0;

    /**
     * Get version name.
     * @return Version name.
     */
    virtual std::string getVersion() const = 0;

    virtual std::string getShaderVersion() const { return {}; }

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
    inline int getMaxTextureSize() const { return _caps.maxTextureSize; }

    /**
     * Get maximum attribute counts.
     * @return Maximum attribute counts.
     */
    inline int getMaxAttributes() const { return _caps.maxAttributes; }

    /**
     * Get maximum texture unit.
     * @return Maximum texture unit.
     */
    inline int getMaxTextureUnits() const { return _caps.maxTextureUnits; }

    /**
     * Get maximum sampler count.
     * @return Maximum sampler count.
     */
    inline int getMaxSamplesAllowed() const { return _caps.maxSamplesAllowed; }

    virtual void cleanPendingResources() {}

    virtual void waitForGPU() {};

protected:
    /**
     * New a shaderModule, not auto released.
     * @param stage Specifies whether is vertex shader or fragment shader.
     * @param chunk Specifies shader chunk.
     * @return A ShaderModule object.
     */
    virtual ShaderModule* createShaderModule(ShaderStage stage, Data& chunk) = 0;

    virtual SamplerHandle createSampler(const SamplerDesc& desc) = 0;
    virtual void destroySampler(SamplerHandle&)                  = 0;

    DriverCaps _caps;

private:
    static DriverBase* _instance;
};

// end of _rhi group
/// @}
}  // namespace ax::rhi

#define axdrv ax::rhi::DriverBase::getInstance()
