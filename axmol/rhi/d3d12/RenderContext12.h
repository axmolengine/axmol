#pragma once
#include "axmol/rhi/RenderContext.h"
#include "axmol/rhi/d3d12/RenderPipeline12.h"
#include "axmol/rhi/d3d12/RenderTarget12.h"
#include "axmol/rhi/d3d12/DepthStencilState12.h"
#include "axmol/rhi/d3d12/VertexLayout12.h"
#include "axmol/rhi/d3d12/Program12.h"
#include "axmol/rhi/d3d12/Buffer12.h"
#include "axmol/rhi/d3d12/Texture12.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <array>
#include <vector>

namespace ax::rhi::d3d12
{

class DriverImpl;

enum class DynamicStateBits : uint32_t
{
    None       = 0,
    Viewport   = 1 << 0,
    Scissor    = 1 << 1,
    StencilRef = 1 << 2,
    CullMode   = 1 << 3,
    FrontFace  = 1 << 4,
};
AX_ENABLE_BITMASK_OPS(DynamicStateBits);

class RenderContextImpl : public RenderContext
{
public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    RenderContextImpl(DriverImpl* driver, void* surfaceContext);
    ~RenderContextImpl() override;

    bool updateSurface(void* surface, uint32_t width, uint32_t height) override;

    void setDepthStencilState(DepthStencilState* depthStencilState) override;
    void setRenderPipeline(RenderPipeline* renderPipeline) override;

    bool beginFrame() override;
    void beginRenderPass(RenderTarget* renderTarget, const RenderPassDesc& descriptor) override;
    void updateDepthStencilState(const DepthStencilDesc& descriptor) override;
    void updatePipelineState(const RenderTarget* rt, const PipelineDesc& descriptor) override;

    void setViewport(int x, int y, unsigned int w, unsigned int h) override;
    void setCullMode(CullMode mode) override;
    void setWinding(Winding winding) override;
    void setScissorRect(bool isEnabled, float x, float y, float width, float height) override;

    void setVertexBuffer(Buffer* buffer) override;
    void setIndexBuffer(Buffer* buffer) override;
    void setInstanceBuffer(Buffer* buffer) override;

    void drawArrays(PrimitiveType primitiveType, std::size_t start, std::size_t count, bool wireframe) override;
    void drawArraysInstanced(PrimitiveType primitiveType,
                             std::size_t start,
                             std::size_t count,
                             int instanceCount,
                             bool wireframe) override;
    void drawElements(PrimitiveType primitiveType,
                      IndexFormat indexType,
                      std::size_t count,
                      std::size_t offset,
                      bool wireframe) override;
    void drawElementsInstanced(PrimitiveType primitiveType,
                               IndexFormat indexType,
                               std::size_t count,
                               std::size_t offset,
                               int instanceCount,
                               bool wireframe) override;

    void endRenderPass() override;
    void endFrame() override;

    void readPixels(RenderTarget* rt,
                    bool preserveAxisHint,
                    std::function<void(const PixelBufferDesc&)> callback) override;

    void setStencilReferenceValue(uint32_t value) override;

    uint32_t getCurrentFrame() const { return _currentFrame; }

private:
    void createCommandObjects();

    void prepareDrawing(ID3D12GraphicsCommandList* cmd);

    DriverImpl* _driver{nullptr};

    Microsoft::WRL::ComPtr<IDXGISwapChain4> _swapchain;
    Microsoft::WRL::ComPtr<ID3D12Device> _device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> _graphicsQueue;

    std::array<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>, MAX_FRAMES_IN_FLIGHT> _commandAllocators;
    std::array<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>, MAX_FRAMES_IN_FLIGHT> _commandLists;
    std::array<Microsoft::WRL::ComPtr<ID3D12Fence>, MAX_FRAMES_IN_FLIGHT> _fences;
    std::array<HANDLE, MAX_FRAMES_IN_FLIGHT> _fenceEvents{};
    std::array<uint64_t, MAX_FRAMES_IN_FLIGHT> _fenceValues{};

    uint32_t _currentFrame{0};
    uint32_t _renderTargetWidth{0};
    uint32_t _renderTargetHeight{0};
    uint32_t _screenWidth{0};
    uint32_t _screenHeight{0};

    DepthStencilStateImpl* _depthStencilState{nullptr};
    RenderPipelineImpl* _renderPipeline{nullptr};
    BufferImpl* _vertexBuffer{nullptr};
    BufferImpl* _indexBuffer{nullptr};
    BufferImpl* _instanceBuffer{nullptr};

    D3D12_VIEWPORT _cachedViewport{};
    D3D12_RECT _cachedScissor{};
    D3D12_CULL_MODE _cachedCullMode{D3D12_CULL_MODE_NONE};

    UINT _syncInterval{0};
    UINT _presentFlags{0};
    UINT _swapchainFlags{0};

    bool _scissorEnabled{false};

    bool _inFrame{false};
};

}  // namespace ax::rhi::d3d12
