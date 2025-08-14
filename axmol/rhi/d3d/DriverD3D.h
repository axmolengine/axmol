#pragma once

#include "axmol/rhi/DriverBase.h"
#include <optional>
#include <d3d11.h>

namespace ax::rhi::d3d {

/**
 * @addtogroup _d3d
 * @{
 */

/**
 * Create resources from MTLDevice.
 */
class DriverImpl : public DriverBase
{
public:
    /* The max vertex attribs, it's not how many device supports which may be lower. */
    static constexpr uint32_t MAX_VERTEX_ATTRIBS = 16;

    /* The vertex data buffers binding index start, the axslcc(SPIRV-Cross), default UBO binding index is 0,
    scope is per stage in MSL
     */
    static constexpr uint32_t VBO_BINDING_INDEX_START = 0;

    /* The vertex instancing buffer binding index */
    static constexpr uint32_t VBO_INSTANCING_BINDING_INDEX = VBO_BINDING_INDEX_START + 1;

    /* The default attribs binding index */
    static constexpr uint32_t DEFAULT_ATTRIBS_BINDING_INDEX = VBO_BINDING_INDEX_START + MAX_VERTEX_ATTRIBS;

    /// @name Constructor, Destructor and Initializers
    DriverImpl();
    ~DriverImpl();

    /// @name Setters & Getters
    /**
     * Create a CommandBuffer object.
     * @return A CommandBuffer object.
     */
    CommandBuffer* createCommandBuffer(void*) override;

    /**
     * Create a Buffer object.
     * @param size Specifies the size in bytes of the buffer object's new data store.
     * @param type Specifies the target buffer object. The symbolic constant must be BufferType::VERTEX or
     * BufferType::INDEX.
     * @param usage Specifies the expected usage pattern of the data store. The symbolic constant must be
     * BufferUsage::STATIC, BufferUsage::DYNAMIC.
     * @return A Buffer object.
     */
    Buffer* createBuffer(std::size_t size, BufferType type, BufferUsage usage) override;

    /**
     * Create a Texture object.
     * @param descriptor Specifies texture description.
     * @return A Texture object.
     */
    Texture* createTexture(const TextureDescriptor& descriptor) override;

    RenderTarget* createDefaultRenderTarget() override;
    RenderTarget* createRenderTarget(Texture* colorAttachment,
                                  Texture* depthStencilAttachment) override;

    /**
     * Create a DepthStencilState object.
     */
    DepthStencilState* createDepthStencilState() override;

    /**
     * Create a RenderPipeline object.
     * @param descriptor Specifies render pipeline description.
     * @return A RenderPipeline object.
     */
    RenderPipeline* createRenderPipeline() override;

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
     * Create a Program, not auto release.
     * @param vertexShader Specifes this is a vertex shader source.
     * @param fragmentShader Specifes this is a fragment shader source.
     * @return A Program instance.
     */
    Program* createProgram(std::string_view vertexShader, std::string_view fragmentShader) override;

    VertexLayout* createVertexLayout() override;

    /// below is driver info
\
    /// @name Setters & Getters
    /**
     * Get vendor device name.
     * @return Vendor device name.
     */
    std::string getVendor() const override;

    /**
     * Get the full name of the vendor device.
     * @return The full name of the vendor device.
     */
    std::string getRenderer() const override;

    /**
     * Get featureSet name.
     * @return FeatureSet name.
     */
    std::string getVersion() const override;

    std::string getShaderVersion() const override;

    D3D_FEATURE_LEVEL getFeatureLevel() const { return _featureLevel; }

    /**
     * Check if feature supported by Metal.
     * @param feature Specify feature to be query.
     * @return true if the feature is supported, false otherwise.
     */
    bool checkForFeatureSupported(FeatureType feature) override;


    inline ID3D11Device* getDevice() const { return _device; }

    inline ID3D11DeviceContext* getContext() const { return _context; }

    static bool supportD24S8() { return _isDepth24Stencil8PixelFormatSupported; }

protected:
    /**
     * Create a shaderModule.
     * @param stage Specifies whether is vertex shader or fragment shader.
     * @param source Specifies shader source.
     * @return A ShaderModule object.
     */
    ShaderModule* createShaderModule(ShaderStage stage, std::string_view source) override;

private:

    ID3D11Device* _device = nullptr;
    ID3D11DeviceContext* _context = nullptr;

    DXGI_ADAPTER_DESC _adapterDesc;

    //FeatureSet _featureSet = FeatureSet::Unknown;
    static bool _isDepth24Stencil8PixelFormatSupported;

    std::optional<LARGE_INTEGER> _driverVersion;

    D3D_FEATURE_LEVEL _featureLevel{};
};

// end of _metal group
/// @}
}
