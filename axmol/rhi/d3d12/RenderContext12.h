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
#include "axmol/rhi/d3d12/RenderPipeline12.h"
#include "axmol/rhi/d3d12/DepthStencilState12.h"
#include "axmol/rhi/d3d12/VertexLayout12.h"
#include "axmol/rhi/d3d12/Program12.h"
#include "axmol/rhi/d3d12/Buffer12.h"
#include "axmol/rhi/d3d12/Texture12.h"
#include "axmol/tlx/flat_set.hpp"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <array>
#include <vector>

namespace ax::rhi::d3d12
{

class DriverImpl;
class RenderTargetImpl;

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

struct GPUFence
{
    GPUFence() = default;

    GPUFence(const GPUFence&)            = delete;
    GPUFence& operator=(const GPUFence&) = delete;

    explicit operator bool() const { return this->handle && this->event; }

    uint64_t wait() const;

    ID3D12Fence* handle{nullptr};
    HANDLE event{nullptr};
    uint64_t value{0};
};

class RenderContextImpl : public RenderContext
{
public:
    static constexpr int SWAPCHAIN_BUFFER_COUNT = 3;

    static constexpr uint32_t VI_BINDING_INDEX            = 0;
    static constexpr uint32_t VI_INSTANCING_BINDING_INDEX = 1;

    // match axmol shaders
    static constexpr uint32_t VS_UBO_BINDING_INDEX = 0;
    static constexpr uint32_t FS_UBO_BINDING_INDEX = 1;

    static constexpr DynamicStateBits PIPELINE_REQUIRED_DYNAMIC_BITS =
        DynamicStateBits::Viewport | DynamicStateBits::Scissor | DynamicStateBits::StencilRef |
        DynamicStateBits::CullMode | DynamicStateBits::FrontFace;

    RenderContextImpl(DriverImpl* driver, void* surfaceContext);
    ~RenderContextImpl() override;

    RenderTarget* getScreenRenderTarget() const override { return (RenderTarget*)_screenRT; }

    bool updateSurface(void* surface, uint32_t width, uint32_t height) override;

    void setDepthStencilState(DepthStencilState* depthStencilState) override;
    void setRenderPipeline(RenderPipeline* renderPipeline) override;

    bool beginFrame() override;
    void beginRenderPass(RenderTarget* renderTarget, const RenderPassDesc& renderPassDesc) override;
    void updateDepthStencilState(const DepthStencilDesc& descriptor) override;
    void updatePipelineState(const RenderTarget* rt,
                             const PipelineDesc& pipelineDesc,
                             PrimitiveType primitiveType) override;

    void setViewport(int x, int y, unsigned int w, unsigned int h) override;
    void setCullMode(CullMode mode) override;
    void setWinding(Winding winding) override;
    void setScissorRect(bool isEnabled, float x, float y, float width, float height) override;

    void setVertexBuffer(Buffer* buffer) override;
    void setIndexBuffer(Buffer* buffer) override;
    void setInstanceBuffer(Buffer* buffer) override;

    void drawArrays(std::size_t start, std::size_t count, bool wireframe) override;
    void drawArraysInstanced(std::size_t start, std::size_t count, int instanceCount, bool wireframe) override;
    void drawElements(IndexFormat indexType, std::size_t count, std::size_t offset, bool wireframe) override;
    void drawElementsInstanced(IndexFormat indexType,
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

    uint64_t getCompletedFenceValue() const override;

private:
    void createCommandObjects();
    void createDescriptorHeaps();

    void prepareDrawing(ID3D12GraphicsCommandList* cmd);

    void readPixelsInternal(RenderTarget* rt,
                            bool preserveAxisHint,
                            std::function<void(const PixelBufferDesc&)>& callback);

    void markDynamicStateDirty(DynamicStateBits bits) noexcept
    {
        auto&& apply = [this, bits]<std::size_t... _Idx>(std::index_sequence<_Idx...>) {
            (bitmask::set(_inFlightDynamicDirtyBits[_Idx], bits), ...);
        };
        apply(std::make_index_sequence<MAX_FRAMES_IN_FLIGHT>{});
    }

    void applyPendingDynamicStates();

    DriverImpl* _driver{nullptr};

    RenderTargetImpl* _screenRT{nullptr};

    ComPtr<IDXGISwapChain4> _swapchain;
    ComPtr<ID3D12Device> _device;
    ComPtr<ID3D12CommandQueue> _graphicsQueue;

    std::array<ComPtr<ID3D12CommandAllocator>, MAX_FRAMES_IN_FLIGHT> _commandAllocators;
    std::array<ComPtr<ID3D12GraphicsCommandList>, MAX_FRAMES_IN_FLIGHT> _commandLists;
    std::array<GPUFence, MAX_FRAMES_IN_FLIGHT> _inflightFences;

    uint64_t _frameFenceValue{0};
    uint64_t _completedFenceValue{0};

    ComPtr<ID3D12DescriptorHeap> _srvHeaps[MAX_FRAMES_IN_FLIGHT];

    UINT _srvOffset[MAX_FRAMES_IN_FLIGHT] = {};

    std::array<DynamicStateBits, MAX_FRAMES_IN_FLIGHT> _inFlightDynamicDirtyBits{DynamicStateBits::None};

    ID3D12GraphicsCommandList* _currentCmdList{nullptr};  // weak pointer

    ID3D12RootSignature* _boundRootSig{nullptr};
    ID3D12PipelineState* _boundPSO{nullptr};  // weak pointer

    uint32_t _imageIndex{0};
    int _frameIndex{0};

    uint32_t _renderTargetWidth{0};
    uint32_t _renderTargetHeight{0};
    uint32_t _screenWidth{0};
    uint32_t _screenHeight{0};

#pragma region Uniform ring buffer
    struct UniformSlice
    {
        std::size_t offset              = 0;        // byte offset from start of ring buffer
        std::size_t size                = 0;        // requested size (unaligned)
        uint8_t* cpuPtr                 = nullptr;  // persistently mapped CPU pointer to write data
        D3D12_GPU_VIRTUAL_ADDRESS gpuVA = 0;        // base GPU VA + offset (bind this)
    };

    struct UniformRingBuffer
    {
        // Upload heap resource (persistently mapped)
        ComPtr<ID3D12Resource> resource = nullptr;
        uint8_t* mapped                 = nullptr;

        // Capacity & allocation state
        std::size_t capacity  = 0;  // total bytes
        std::size_t writeHead = 0;  // current write offset

        // Alignment: CBV requires 256-byte alignment
        std::size_t align = 256;

        // Cached GPU VA base for fast slice addressing
        D3D12_GPU_VIRTUAL_ADDRESS baseGpuVA = 0;

        // Reset allocation pointer
        void reset() { writeHead = 0; }

        bool valid() const { return resource != nullptr && mapped != nullptr && capacity > 0; }
    };

    // Per-frame ring buffers
    std::array<UniformRingBuffer, MAX_FRAMES_IN_FLIGHT> _uniformRings{};

    // Public API
    void createUniformRingBuffers(std::size_t capacityBytes);
    void destroyUniformRingBuffers();
    void resetUniformRingForCurrentFrame(UINT frameIndex);
    UniformSlice allocateUniformSlice(UINT frameIndex, std::size_t size);
#pragma endregion

    DepthStencilStateImpl* _depthStencilState{nullptr};
    RenderPipelineImpl* _renderPipeline{nullptr};
    BufferImpl* _vertexBuffer{nullptr};
    BufferImpl* _indexBuffer{nullptr};
    BufferImpl* _instanceBuffer{nullptr};

    std::vector<std::function<void(uint64_t)>> _frameCompletionOps;

    D3D12_VIEWPORT _cachedViewport{};
    D3D12_RECT _cachedScissor{};
    D3D12_CULL_MODE _cachedCullMode{D3D12_CULL_MODE_NONE};
    BOOL _cachedFrontCounterClockwise{FALSE};

    UINT _syncInterval{0};
    UINT _presentFlags{0};
    UINT _swapchainFlags{0};

    RenderScaleMode _renderScaleMode{};

    bool _swapchainDirty{false};
    bool _inFrame{false};
};

}  // namespace ax::rhi::d3d12
