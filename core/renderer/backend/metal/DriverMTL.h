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

#include "../DriverBase.h"
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

NS_AX_BACKEND_BEGIN

enum class FeatureSet : int32_t
{
    Unknown                      = -1,
    FeatureSet_iOS_GPUFamily1_v1 = 0,
    FeatureSet_iOS_GPUFamily2_v1 = 1,

    FeatureSet_iOS_GPUFamily1_v2 = 2,
    FeatureSet_iOS_GPUFamily2_v2 = 3,
    FeatureSet_iOS_GPUFamily3_v1 = 4,

    FeatureSet_iOS_GPUFamily1_v3 = 5,
    FeatureSet_iOS_GPUFamily2_v3 = 6,
    FeatureSet_iOS_GPUFamily3_v2 = 7,

    FeatureSet_iOS_GPUFamily1_v4 = 8,
    FeatureSet_iOS_GPUFamily2_v4 = 9,
    FeatureSet_iOS_GPUFamily3_v3 = 10,
    FeatureSet_iOS_GPUFamily4_v1 = 11,

    FeatureSet_iOS_GPUFamily1_v5 = 12,
    FeatureSet_iOS_GPUFamily2_v5 = 13,
    FeatureSet_iOS_GPUFamily3_v4 = 14,
    FeatureSet_iOS_GPUFamily4_v2 = 15,

    FeatureSet_macOS_GPUFamily1_v1 = 10000,

    FeatureSet_macOS_GPUFamily1_v2         = 10001,
    FeatureSet_macOS_ReadWriteTextureTier2 = 10002,

    FeatureSet_macOS_GPUFamily1_v3 = 10003,

    FeatureSet_macOS_GPUFamily1_v4 = 10004,
    FeatureSet_macOS_GPUFamily2_v1 = 10005,
};


/**
 * @addtogroup _metal
 * @{
 */

/**
 * Create resources from MTLDevice.
 */
class DriverMTL : public DriverBase
{
public:
    /* The max vertex attribs, it's not how many device supports which may be lower. */
    static constexpr uint32_t MAX_VERTEX_ATTRIBS = 16;

    /* The vertex data buffers binding index start, the glslcc(SPIRV-Cross), default UBO binding index is 0,
    scope is per stage in MSL
     */
    static constexpr uint32_t VBO_BINDING_INDEX_START = 0;

    /* The vertex instancing buffer binding index */
    static constexpr uint32_t VBO_INSTANCING_BINDING_INDEX = VBO_BINDING_INDEX_START + 1;

    /* The default attribs binding index */
    static constexpr uint32_t DEFAULT_ATTRIBS_BINDING_INDEX = VBO_BINDING_INDEX_START + MAX_VERTEX_ATTRIBS;

    /**
     * Set CAMetalLayer.
     * @param metalLayer A CAMetalLayer object.
     */
    static void setCAMetalLayer(CAMetalLayer* metalLayer);

    /**
     * Invoke by engine internally at the beginning of rendering a new frame.
     */
    static void updateDrawable();

    /**
     * Get a CAMetalLayer.
     * @return A CAMetalLayer object.
     */
    static CAMetalLayer* getCAMetalLayer() { return DriverMTL::_metalLayer; }

    /**
     * Get available Drawable.
     * @return an available drawable.
     */
    static id<CAMetalDrawable> getCurrentDrawable();

    /**
     * Reset current drawable to nil.
     */
    static void resetCurrentDrawable();

    /// @name Constructor, Destructor and Initializers
    DriverMTL();
    ~DriverMTL();

    /// @name Setters & Getters
    /**
     * New a CommandBuffer object.
     * @return A CommandBuffer object.
     */
    CommandBuffer* newCommandBuffer() override;

    /**
     * New a Buffer object.
     * @param size Specifies the size in bytes of the buffer object's new data store.
     * @param type Specifies the target buffer object. The symbolic constant must be BufferType::VERTEX or
     * BufferType::INDEX.
     * @param usage Specifies the expected usage pattern of the data store. The symbolic constant must be
     * BufferUsage::STATIC, BufferUsage::DYNAMIC.
     * @return A Buffer object.
     */
    Buffer* newBuffer(std::size_t size, BufferType type, BufferUsage usage) override;

    /**
     * New a TextureBackend object.
     * @param descriptor Specifies texture description.
     * @return A TextureBackend object.
     */
    TextureBackend* newTexture(const TextureDescriptor& descriptor) override;

    RenderTarget* newDefaultRenderTarget() override;
    RenderTarget* newRenderTarget(TextureBackend* colorAttachment,
                                  TextureBackend* depthAttachment,
                                  TextureBackend* stencilAttachhment) override;

    /**
     * New a DepthStencilState object.
     */
    DepthStencilState* newDepthStencilState() override;

    /**
     * New a RenderPipeline object.
     * @param descriptor Specifies render pipeline description.
     * @return A RenderPipeline object.
     */
    RenderPipeline* newRenderPipeline() override;

    /**
     * This property controls whether or not the drawables'
     * MTLTextures may only be used for framebuffer attachments (YES) or
     * whether they may also be used for texture sampling and pixel
     * read/write operations (NO).
     * @param frameBufferOnly A value of YES allows CAMetalLayer to allocate the MTLTexture objects in ways that are
     * optimized for display purposes that makes them unsuitable for sampling. The recommended value for most
     * applications is YES.
     */
    void setFrameBufferOnly(bool frameBufferOnly) override;

    /**
     * New a Program, not auto release.
     * @param vertexShader Specifes this is a vertex shader source.
     * @param fragmentShader Specifes this is a fragment shader source.
     * @return A Program instance.
     */
    Program* newProgram(std::string_view vertexShader, std::string_view fragmentShader) override;

    /**
     * Get a MTLDevice object.
     * @return A MTLDevice object.
     */
    inline id<MTLDevice> getMTLDevice() const { return _mtlDevice; }

    /**
     * Get a MTLCommandQueue object.
     * @return A MTLCommandQueue object.
     */
    inline id<MTLCommandQueue> getMTLCommandQueue() const { return _mtlCommandQueue; }

    /// below is driver info
\
    /// @name Setters & Getters
    /**
     * Get vendor device name.
     * @return Vendor device name.
     */
    const char* getVendor() const override;

    /**
     * Get the full name of the vendor device.
     * @return The full name of the vendor device.
     */
    const char* getRenderer() const override;

    /**
     * Get featureSet name.
     * @return FeatureSet name.
     */
    const char* getVersion() const override;

    /**
     * Check if feature supported by Metal.
     * @param feature Specify feature to be query.
     * @return true if the feature is supported, false otherwise.
     */
    bool checkForFeatureSupported(FeatureType feature) override;

    static bool supportD24S8() { return _isDepth24Stencil8PixelFormatSupported; }

protected:
    /**
     * New a shaderModule.
     * @param stage Specifies whether is vertex shader or fragment shader.
     * @param source Specifies shader source.
     * @return A ShaderModule object.
     */
    ShaderModule* newShaderModule(ShaderStage stage, std::string_view source) override;

private:
    static CAMetalLayer* _metalLayer;
    static id<CAMetalDrawable> _currentDrawable;

    id<MTLDevice> _mtlDevice             = nil;
    id<MTLCommandQueue> _mtlCommandQueue = nil;

    std::string _deviceName;
    FeatureSet _featureSet = FeatureSet::Unknown;
    static bool _isDepth24Stencil8PixelFormatSupported;
};

// end of _metal group
/// @}
NS_AX_BACKEND_END
