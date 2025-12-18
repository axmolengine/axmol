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
#include "axmol/rhi/d3d12/DescriptorHeapAllocator12.h"
#include "axmol/rhi/d3d12/UploadBufferAllocator12.h"
#include "axmol/rhi/d3d12/ShaderModule12.h"
#include "axmol/rhi/DXUtils.h"
#include "axmol/tlx/byte_buffer.hpp"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <optional>
#include <string>
#include <mutex>
#include <deque>
#include <functional>
#include <array>

// Note: d3dcompiler_47.dll also works well in HLSL shader model 5.1
// DXC requires shader model 6.0
#include <dxcapi.h>

namespace ax::rhi
{
class RenderTarget;
}

namespace ax::rhi::d3d12
{

class RenderContextImpl;
class DriverImpl;
class TextureImpl;

struct DisposableResource
{
    enum class Type
    {
        Resource,
        ShaderResourceView,
        RenderTargetView,
        DepthStencilView,
        SamplerView,
    };

    Type type{};
    uint64_t fenceValue;
    union
    {
        ID3D12Resource* resource;
        DescriptorHandle* handle;
    };
};

class IsolateSubmission
{
    friend class DriverImpl;

public:
    IsolateSubmission() = default;

    IsolateSubmission(const IsolateSubmission&)            = delete;
    IsolateSubmission& operator=(const IsolateSubmission&) = delete;
    IsolateSubmission(IsolateSubmission&&)                 = delete;
    IsolateSubmission& operator=(IsolateSubmission&&)      = delete;

    explicit operator bool() const { return cmdList; }
    operator ID3D12GraphicsCommandList*() const { return cmdList.Get(); }
    ID3D12GraphicsCommandList* operator->() const noexcept { return cmdList.Get(); }

private:
    void create(ID3D12Device*);
    void reset();

    uint64_t submit(ComPtr<ID3D12CommandQueue>& queue);
    void waitGPU();

    void release();

    ComPtr<ID3D12CommandAllocator> allocator;
    ComPtr<ID3D12GraphicsCommandList> cmdList;
    ComPtr<ID3D12Fence> fence;
    HANDLE event{nullptr};
    UINT64 fenceValue = 0;
};

class DriverImpl : public DriverBase
{
    friend class RenderContextImpl;

public:
    static constexpr uint32_t MAX_VERTEX_ATTRIBS = 16;

    DriverImpl();
    ~DriverImpl();

    void init();

    RenderContext* createRenderContext(void* surfaceContext) override;
    Buffer* createBuffer(std::size_t size, BufferType type, BufferUsage usage, const void* initial) override;
    Texture* createTexture(const TextureDesc& descriptor) override;
    RenderTarget* createRenderTarget(Texture* colorAttachment, Texture* depthStencilAttachment) override;
    DepthStencilState* createDepthStencilState() override;
    RenderPipeline* createRenderPipeline() override;
    Program* createProgram(Data vsData, Data fsData) override;
    VertexLayout* createVertexLayout(VertexLayoutDesc&& desc) override;

    ShaderModule* createShaderModule(ShaderStage stage, Data&) override;
    SamplerHandle createSampler(const SamplerDesc& desc) override;
    void destroySampler(SamplerHandle&) override;

    std::string getVendor() const override;
    std::string getRenderer() const override;
    std::string getVersion() const override;
    std::string getShaderVersion() const override;

    bool checkForFeatureSupported(FeatureType feature) override;

    void cleanPendingResources() override;

    ID3D12Device* getDevice() const { return _device.Get(); }
    ID3D12CommandQueue* getGraphicsQueue() const { return _gfxQueue.Get(); }
    const ComPtr<IDXGIFactory4>& getDXGIFactory() const { return _dxgiFactory; }

    bool generateMipmaps(ID3D12GraphicsCommandList* cmd, ID3D12Resource* texture);

    IsolateSubmission& startIsolateSubmission();
    uint64_t finishIsolateSubmission(IsolateSubmission& submission, bool waitForCompletion = true);

    // New helpers
    DescriptorHandle* createSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc);
    DescriptorHandle* createUAV(ID3D12Resource* resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc);

    // Allocation
    DescriptorHandle* allocateDescriptor(DisposableResource::Type type);
    void deallocateDescriptor(DescriptorHandle* handle, DisposableResource::Type type);

    // Gets SRV/UAV/CBV heap
    ID3D12DescriptorHeap* getResourceViewHeap(const DescriptorHandle* h) const
    {
        return _srvAllocator->getDescriptorHeap(h);
    }
    ID3D12DescriptorHeap* getSamplerHeap() const { return _samplerAllocator->getDescriptorHeapByIndex(0); }
    ID3D12DescriptorHeap* getRtvHeap(const DescriptorHandle* h) const { return _rtvAllocator->getDescriptorHeap(h); }
    ID3D12DescriptorHeap* getDsvHeap(const DescriptorHandle* h) const { return _dsvAllocator->getDescriptorHeap(h); }

    UploadBufferAllocator* getUploadBufferAllocator() const { return _uploadBufferAllocator.get(); }

    UINT getSrvDescriptorStride() const { return _srvDescriptorStride; }
    UINT getSamplerDescriptorStride() const { return _samplerDescriptorStride; }

    bool compileShader(std::span<uint8_t> shaderCode, ShaderStage stage, D3D12BlobHandle& outHandle);

    void queueDisposal(ID3D12Resource*, uint64_t fenceValue);
    void queueDisposal(DescriptorHandle* handle, DisposableResource::Type type, uint64_t fenceValue);

    void processDisposalQueue(uint64_t completeFence);

    void waitForGPU() override;

protected:
    void queueDisposalInternal(DisposableResource&& res);

private:
    void initializeDevice();

    void createDescriptorAllocators();

    bool checkFormatSupport(DXGI_FORMAT format);
    bool detectDXCAvailability();

    RenderContextImpl* _currentRenderContext{nullptr};

    ComPtr<IDXGIFactory4> _dxgiFactory;
    ComPtr<IDXGIAdapter> _adapter;
    ComPtr<ID3D12Device> _device;
    ComPtr<ID3D12CommandQueue> _gfxQueue;

    ComPtr<ID3D12CommandQueue> _uploadQueue;

    ID3D12Fence* _idleFence{nullptr};
    HANDLE _idleEvent{nullptr};
    uint64_t _idleFenceValue{0};

    IsolateSubmission _isolateSubmission;

    // Descriptor heaps (RTV/DSV for render targets; SRV/UAV/CBV for general use if needed)
    std::unique_ptr<DescriptorHeapAllocator> _srvAllocator;
    std::unique_ptr<DescriptorHeapAllocator> _rtvAllocator;
    std::unique_ptr<DescriptorHeapAllocator> _dsvAllocator;
    std::unique_ptr<DescriptorHeapAllocator> _samplerAllocator;

    std::unique_ptr<UploadBufferAllocator> _uploadBufferAllocator;

    UINT _srvDescriptorStride{0};
    UINT _samplerDescriptorStride{0};

    // Disposal queue (optional) if you want fence-gated cleanup beyond ComPtr lifetime
    std::mutex _disposalMutex;
    std::vector<DisposableResource> _disposalQueue;

    D3D_FEATURE_LEVEL _featureLevel{D3D_FEATURE_LEVEL_11_0};
    DXGI_ADAPTER_DESC _adapterDesc{};
    std::optional<LARGE_INTEGER> _driverVersion;

    ComPtr<IDxcLibrary> _dxcLibrary;
    ComPtr<IDxcCompiler> _dxcCompiler;
    std::vector<LPCWSTR> _dxcArguments;

    bool _dxcAvailable{false};

    tlx::byte_buffer _shaderCompileBuffer;

    // lazy init helpers
    void ensureMipmapPipeline(bool isArray);
    D3D12BlobHandle compileMipmapCS(bool isArray);

    ComPtr<ID3D12RootSignature> _mipmapRootSig;
    ComPtr<ID3D12PipelineState> _mipmapPSO2D;
    ComPtr<ID3D12PipelineState> _mipmapPSOArray;
    ComPtr<ID3D12DescriptorHeap> _mipmapSrvHeap;
};

}  // namespace ax::rhi::d3d12
