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

#include "axmol/rhi/RenderContext.h"
#include "axmol/rhi/d3d11/Driver11.h"
#include "axmol/rhi/DXUtils.h"

namespace ax::rhi::d3d11
{
/**
 * @addtogroup _d3d11
 * @{
 */

class BufferImpl;
class DepthStencilStateImpl;
class RenderPipelineImpl;
class RenderTargetImpl;

enum RasterFlag
{
    RF_CULL_MODE = 1,
    RF_WINDING   = 1 << 1,
    RF_SCISSOR   = 1 << 2
};

struct RasterStateDesc
{
    CullMode cullMode{CullMode::BACK};
    Winding winding{Winding::CLOCK_WISE};
    bool scissorEnable{FALSE};
    unsigned int dirtyFlags{0};
};

/**
 * @brief A D3D11-based RenderContext implementation
 *
 */
class RenderContextImpl : public RenderContext
{
public:
    /* The max vertex attribs, it's not how many device supports which may be lower. */
    static constexpr uint32_t MAX_VERTEX_ATTRIBS = 16;

    static constexpr uint32_t VI_BINDING_INDEX            = 0;
    static constexpr uint32_t VI_INSTANCING_BINDING_INDEX = 1;

    // match axmol shaders
    static constexpr uint32_t VS_UBO_BINDING_INDEX = 0;
    static constexpr uint32_t FS_UBO_BINDING_INDEX = 1;

    /// @name Constructor, Destructor and Initializers
    /**
     * @param driver The device for which d3d11DriverImpl object was created.
     * @param surface hwnd or IUnkown*(SwapChainPanel)
     */
    RenderContextImpl(DriverImpl* driver, SurfaceHandle surface);
    ~RenderContextImpl() override;

    RenderTarget* getScreenRenderTarget() const override { return (RenderTarget*)_screenRT; }

    bool updateSurface(SurfaceHandle surface, uint32_t width, uint32_t height) override;

    void setDepthStencilState(DepthStencilState* depthStencilState) override;

    void setRenderPipeline(RenderPipeline* renderPipeline) override;

    bool beginFrame() override;

    void beginRenderPass(RenderTarget* renderTarget, const RenderPassDesc& descriptor) override;

    void updateDepthStencilState(const DepthStencilDesc& descriptor) override;

    void updatePipelineState(const RenderTarget* rt,
                             const PipelineDesc& descriptor,
                             PrimitiveType primitiveType) override;

    void setViewport(int x, int y, unsigned int w, unsigned int h) override;

    void setCullMode(CullMode mode) override;

    void setWinding(Winding winding) override;

    void setVertexBuffer(Buffer* buffer) override;

    void setIndexBuffer(Buffer* buffer) override;

    void setInstanceBuffer(Buffer* buffer) override;

    void drawArrays(std::size_t start, std::size_t count, bool wireframe) override;
    void drawArraysInstanced(std::size_t start, std::size_t count, int instanceCount, bool wireframe = false) override;

    void drawElements(IndexFormat indexType, std::size_t count, std::size_t offset, bool wireframe) override;

    void drawElementsInstanced(IndexFormat indexType,
                               std::size_t count,
                               std::size_t offset,
                               int instanceCount,
                               bool wireframe = false) override;

    void endRenderPass() override;

    void endFrame() override;

    void setScissorRect(bool isEnabled, float x, float y, float width, float height) override;

    void readPixels(RenderTarget* rt,
                    bool preserveAxisHint,
                    std::function<void(const PixelBufferDesc&)> callback) override;

protected:
    void readPixels(RenderTarget* rt, UINT x, UINT y, UINT width, UINT height, PixelBufferDesc& pbd);

    void updateRasterizerState();

    void prepareDrawing();

    DriverImpl* _driverImpl{nullptr};
    ID3D11DeviceContext* _d3d11Context{nullptr};

    RenderTargetImpl* _screenRT{nullptr};
    IDXGISwapChain* _swapChain{nullptr};
    ID3D11Texture2D* _depthStencilTexture{nullptr};
    ComPtr<ID3D11RasterizerState> _rasterState{nullptr};
    RasterStateDesc _rasterDesc{};
    BufferImpl* _vertexBuffer{nullptr};
    BufferImpl* _indexBuffer{nullptr};
    BufferImpl* _instanceBuffer{nullptr};
    DepthStencilStateImpl* _depthStencilState{nullptr};
    RenderPipelineImpl* _renderPipeline{nullptr};
    UINT _renderTargetWidth{0};
    UINT _renderTargetHeight{0};
    UINT _screenWidth{0};
    UINT _screenHeight{0};
    RenderPassDesc _renderPassDesc{};

    tlx::pod_vector<ID3D11ShaderResourceView*> _nullSRVs;
    UINT _textureBounds{0};

    UINT _swapChainFlags{0};
    UINT _syncInterval{1};
    UINT _presentFlags{0};
    BOOL _allowTearing{FALSE};

    RenderScaleMode _renderScaleMode{};
};

/** @} */

}  // namespace ax::rhi::d3d11
