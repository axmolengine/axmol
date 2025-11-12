/****************************************************************************
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

#include "axmol/rhi/DriverBase.h"
#include "axmol/platform/win32/ComPtr.h"

#include <d3d11.h>
#include <dxgi.h>

#include <optional>

namespace ax::rhi::d3d
{

/**
 * @addtogroup _d3d
 * @{
 */

/**
 * @brief A D3D11-based Driver implementation
 *
 */
class DriverImpl : public DriverBase
{
public:
    static constexpr D3D_FEATURE_LEVEL DEFAULT_REATURE_LEVELS[2] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    /// @name Constructor, Destructor and Initializers
    DriverImpl();
    ~DriverImpl();

    void init();

    /// @name Setters & Getters
    /**
     * Create a RenderContext object.
     * @return A RenderContext object.
     */
    RenderContext* createRenderContext(void* surfaceContext) override;

    /**
     * Create a Buffer object.
     * @param size Specifies the size in bytes of the buffer object's new data store.
     * @param type Specifies the target buffer object. The symbolic constant must be BufferType::VERTEX or
     * BufferType::INDEX.
     * @param usage Specifies the expected usage pattern of the data store. The symbolic constant must be
     * BufferUsage::STATIC, BufferUsage::DYNAMIC.
     * @return A Buffer object.
     */
    Buffer* createBuffer(std::size_t size, BufferType type, BufferUsage usage, const void* initial) override;

    /**
     * Create a Texture object.
     * @param descriptor Specifies texture description.
     * @return A Texture object.
     */
    Texture* createTexture(const TextureDesc& descriptor) override;

    RenderTarget* createDefaultRenderTarget() override;
    RenderTarget* createRenderTarget(Texture* colorAttachment, Texture* depthStencilAttachment) override;

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
     * Create a Program, not auto release.
     * @param vertexShader Specifes this is a vertex shader source.
     * @param fragmentShader Specifes this is a fragment shader source.
     * @return A Program instance.
     */
    Program* createProgram(std::string_view vertexShader, std::string_view fragmentShader) override;

    VertexLayout* createVertexLayout(VertexLayoutDesc&& desc) override;

    /// below is driver info

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

    const ComPtr<IDXGIFactory>& getDXGIFactory() const { return _dxgiFactory; }
    const ComPtr<IDXGIAdapter> getDXGIAdapter() const { return _dxgiAdapter; }

protected:
    /**
     * Create a shaderModule.
     * @param stage Specifies whether is vertex shader or fragment shader.
     * @param source Specifies shader source.
     * @return A ShaderModule object.
     */
    ShaderModule* createShaderModule(ShaderStage stage, std::string_view source) override;
    SamplerHandle createSampler(const SamplerDesc& desc) override;
    void destroySampler(SamplerHandle& h) override;

private:
    void initializeAdapter();
    void initializeDevice();
    HRESULT createD3DDevice(int requestDriverType, int createFlags);

    bool checkFormatSupport(DXGI_FORMAT format);

    ID3D11Device* _device         = nullptr;
    ID3D11DeviceContext* _context = nullptr;

    ComPtr<IDXGIFactory> _dxgiFactory;
    ComPtr<IDXGIAdapter> _dxgiAdapter;

    DXGI_ADAPTER_DESC _adapterDesc{};

    D3D_FEATURE_LEVEL _featureLevel{};

    // FeatureSet _featureSet = FeatureSet::Unknown;

    std::optional<LARGE_INTEGER> _driverVersion;
};

/** @} */

}  // namespace ax::rhi::d3d
