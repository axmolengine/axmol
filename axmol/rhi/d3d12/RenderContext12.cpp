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
#include "axmol/rhi/d3d12/RenderContext12.h"
#include "axmol/rhi/d3d12/Driver12.h"
#include "axmol/rhi/d3d12/RenderTarget12.h"
#include "axmol/rhi/d3d12/RenderPipeline12.h"
#include "axmol/rhi/d3d12/DepthStencilState12.h"
#include "axmol/rhi/d3d12/VertexLayout12.h"
#include "axmol/rhi/d3d12/Program12.h"
#include "axmol/rhi/d3d12/Buffer12.h"
#include "axmol/rhi/d3d12/Texture12.h"
#include "axmol/base/Logging.h"
#include "axmol/math/MathUtil.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_WINRT
#    include "axmol/platform/winrt/SwapChainPanelUtil.h"
#endif

namespace ax::rhi::d3d12
{

static constexpr UINT MAX_ALLOW_DRAW_CALLS = 2000;

static constexpr D3D12_PRIMITIVE_TOPOLOGY PrimitiveTypeToD3DTopology[] = {
    /* POINT          */ D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
    /* LINE           */ D3D_PRIMITIVE_TOPOLOGY_LINELIST,
    /* LINE_LOOP      */ D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,  // no native LINE_LOOP
    /* LINE_STRIP     */ D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
    /* TRIANGLE       */ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
    /* TRIANGLE_STRIP */ D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
};

inline D3D12_PRIMITIVE_TOPOLOGY toD3DTopology(PrimitiveType type)
{
    return PrimitiveTypeToD3DTopology[static_cast<uint32_t>(type)];
}

static constexpr rhi::PrimitiveGroup kPrimitiveTypeToGroup[] = {
    /* POINT          */ rhi::PrimitiveGroup::Point,
    /* LINE           */ rhi::PrimitiveGroup::Line,
    /* LINE_LOOP      */ rhi::PrimitiveGroup::Line,
    /* LINE_STRIP     */ rhi::PrimitiveGroup::Line,
    /* TRIANGLE       */ rhi::PrimitiveGroup::Triangle,
    /* TRIANGLE_STRIP */ rhi::PrimitiveGroup::Triangle,
};

static inline rhi::PrimitiveGroup toPrimitiveGroup(PrimitiveType type)
{
    return kPrimitiveTypeToGroup[static_cast<uint32_t>(type)];
}

// Helper: map IndexFormat to DXGI_FORMAT
static DXGI_FORMAT toDxgiIndexFormat(IndexFormat fmt)
{
    switch (fmt)
    {
    case IndexFormat::U_SHORT:
        return DXGI_FORMAT_R16_UINT;
    case IndexFormat::U_INT:
        return DXGI_FORMAT_R32_UINT;
    default:
        return DXGI_FORMAT_R32_UINT;
    }
}

uint64_t GPUFence::wait() const
{
    const auto completeFenceValue = this->handle->GetCompletedValue();
    if (completeFenceValue < this->value)
    {
        this->handle->SetEventOnCompletion(this->value, this->event);
        WaitForSingleObject(this->event, INFINITE);
        return this->value;
    }
    return completeFenceValue;
}

RenderContextImpl::RenderContextImpl(DriverImpl* driver, SurfaceHandle surface) : _driver(driver)
{
    _device        = driver->getDevice();
    _graphicsQueue = driver->getGraphicsQueue();
    auto& factory  = driver->getDXGIFactory();

    createCommandObjects();

    // Check tearing support
    BOOL allowTearing = FALSE;
    ComPtr<IDXGIFactory5> factory5;
    if (SUCCEEDED(factory.As(&factory5)))
    {
        factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
    }
    _swapchainFlags = allowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    // vsync control
    auto& contextAttrs = Application::getContextAttrs();
    _renderScaleMode   = contextAttrs.renderScaleMode;
    if (contextAttrs.vsync)
    {
        _syncInterval = 1;
        _presentFlags = 0;
    }
    else
    {
        _syncInterval = 0;
        _presentFlags = DXGI_PRESENT_DO_NOT_WAIT;
        if (allowTearing)
            _presentFlags |= DXGI_PRESENT_ALLOW_TEARING;
    }

    HRESULT hr = E_FAIL;
    ComPtr<IDXGISwapChain> swapchain;

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    RECT clientRect;
    HWND hwnd = static_cast<HWND>(surface);
    GetClientRect(hwnd, &clientRect);
    _screenWidth  = clientRect.right - clientRect.left;
    _screenHeight = clientRect.bottom - clientRect.top;

    DXGI_SWAP_CHAIN_DESC1 desc1 = {};
    desc1.Width                 = _screenWidth;
    desc1.Height                = _screenHeight;
    desc1.Format                = DEFAULT_SWAPCHAIN_FORMAT;
    desc1.SampleDesc.Count      = 1;
    desc1.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc1.BufferCount           = SWAPCHAIN_BUFFER_COUNT;
    desc1.Flags                 = _swapchainFlags;
    desc1.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc = {};
    fsDesc.Windowed                        = TRUE;

    ComPtr<IDXGISwapChain1> swapchain1;

    hr = factory->CreateSwapChainForHwnd(_graphicsQueue.Get(), hwnd, &desc1, &fsDesc, nullptr, &swapchain1);
    if (SUCCEEDED(hr))
    {
        factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
        swapchain1.As(&swapchain);
    }

#elif AX_TARGET_PLATFORM == AX_PLATFORM_WINUWP
    do
    {
        // Wrap the raw COM pointer into a C++/WinRT runtime class
        winrt::SwapChainPanel swapChainPanel{nullptr};
        winrt::copy_from_abi(swapChainPanel, surface.ptr);

        auto dispatcher = swapChainPanel.Dispatcher();
        winrt::Size panelSize;
        winrt::Vector2 renderScale;

        hr = winrt::GetSwapChainPanelRenderMetrics(swapChainPanel, dispatcher, panelSize, renderScale);
        AX_BREAK_IF(FAILED(hr));

        DXGI_SWAP_CHAIN_DESC1 desc1 = {};
        if (_renderScaleMode == RenderScaleMode::Physical)
        {
            desc1.Width  = static_cast<UINT>(panelSize.Width * renderScale.x);
            desc1.Height = static_cast<UINT>(panelSize.Height * renderScale.y);
        }
        else
        {
            desc1.Width  = static_cast<UINT>(panelSize.Width);
            desc1.Height = static_cast<UINT>(panelSize.Height);
        }
        desc1.Format           = DEFAULT_SWAPCHAIN_FORMAT;
        desc1.SampleDesc.Count = 1;
        desc1.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc1.BufferCount      = SWAPCHAIN_BUFFER_COUNT;
        desc1.Scaling          = DXGI_SCALING_STRETCH;
        desc1.AlphaMode        = DXGI_ALPHA_MODE_IGNORE;
        desc1.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc1.Flags            = _swapchainFlags;

        ComPtr<IDXGISwapChain1> swapchain1;
        hr = factory->CreateSwapChainForComposition(_graphicsQueue.Get(), &desc1, nullptr, &swapchain1);
        AX_BREAK_IF(FAILED(hr));
        swapchain1.As(&swapchain);

        hr = winrt::RunOnUIThreadSync(dispatcher, [&swapChainPanel, swapchain1] {
            auto hr0 = swapChainPanel.as<ISwapChainPanelNative>()->SetSwapChain(swapchain1.Get());
            if (FAILED(hr0))
                throw winrt::hresult_error(hr0);
        });
        AX_BREAK_IF(FAILED(hr));

        DXGI_SWAP_CHAIN_DESC1 actualDesc = {};
        swapchain1->GetDesc1(&actualDesc);

        if (_renderScaleMode == RenderScaleMode::Physical)
        {
            DXGI_MATRIX_3X2_F scaleMatrix = {};
            scaleMatrix._11               = 1 / renderScale.x;
            scaleMatrix._22               = 1 / renderScale.y;

            ComPtr<IDXGISwapChain2> swapchain2;
            hr = swapchain1.As(&swapchain2);
            AX_BREAK_IF(FAILED(hr));
            hr = swapchain2->SetMatrixTransform(&scaleMatrix);
            AX_BREAK_IF(FAILED(hr));
        }

        _screenWidth  = actualDesc.Width;
        _screenHeight = actualDesc.Height;
    } while (false);
#endif

    if (FAILED(hr))
        assert(false);  // fatalError("CreateSwapChain", hr);

    swapchain.As(&_swapchain);

    // Create default screen render target
    _screenRT = new RenderTargetImpl(driver, true);

    // Build swapchain attachments for screen RT
    _screenRT->rebuildSwapchainBuffers(_swapchain.Get(), _screenWidth, _screenHeight);

    createUniformRingBuffers(1 * 1024 * 1024);  // 1 MB per frame

    createDescriptorHeaps();

    bitmask::set(_inFlightDynamicDirtyBits[0], PIPELINE_REQUIRED_DYNAMIC_BITS);
    bitmask::set(_inFlightDynamicDirtyBits[1], PIPELINE_REQUIRED_DYNAMIC_BITS);
}

RenderContextImpl::~RenderContextImpl()
{
    _driver->waitForGPU();

    AX_SAFE_RELEASE_NULL(_screenRT);
    AX_SAFE_RELEASE_NULL(_renderPipeline);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        if (_inflightFences[i].event)
        {
            CloseHandle(_inflightFences[i].event);
            _inflightFences[i].event = nullptr;
        }

        if (_inflightFences[i].handle)
        {
            SafeRelease(_inflightFences[i].handle);
        }

        if (_srvHeaps[i])
            _srvHeaps[i].Reset();
    }

    // Release retained buffers
    AX_SAFE_RELEASE_NULL(_vertexBuffer);
    AX_SAFE_RELEASE_NULL(_indexBuffer);
    AX_SAFE_RELEASE_NULL(_instanceBuffer);
}

void RenderContextImpl::createCommandObjects()
{
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        // Command allocator and list
        HRESULT hr =
            _device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocators[i]));
        AXASSERT(SUCCEEDED(hr), "CreateCommandAllocator failed");

        hr = _device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocators[i].Get(), nullptr,
                                        IID_PPV_ARGS(&_commandLists[i]));
        AXASSERT(SUCCEEDED(hr), "CreateGraphicsCommandList failed");
        _commandLists[i]->Close();

        // Fence + event
        hr = _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_inflightFences[i].handle));
        AXASSERT(SUCCEEDED(hr), "CreateFence failed");
        _inflightFences[i].event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        AXASSERT(!!_inflightFences[i], "CreateEvent failed");
    }
}

void RenderContextImpl::createDescriptorHeaps()
{
    const auto maxTextureUnits = static_cast<UINT>(_driver->getMaxTextureUnits());
    const auto maxDescriptors  = maxTextureUnits * MAX_ALLOW_DRAW_CALLS;
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                                           .NumDescriptors = maxDescriptors,
                                           .Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE};

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        _device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeaps[i]));
    }
}

bool RenderContextImpl::updateSurface(SurfaceHandle /*surface*/, uint32_t width, uint32_t height)
{
    // For D3D12, surface is HWND; if resized, mark for swapchain resize
    if (_screenWidth == width && _screenHeight == height)
        return true;

    _screenWidth  = width;
    _screenHeight = height;

    _swapchainDirty = true;

    return true;
}

void RenderContextImpl::setDepthStencilState(DepthStencilState* depthStencilState)
{
    _depthStencilState = static_cast<DepthStencilStateImpl*>(depthStencilState);
}

void RenderContextImpl::setRenderPipeline(RenderPipeline* renderPipeline)
{
    Object::assign(_renderPipeline, static_cast<RenderPipelineImpl*>(renderPipeline));
}

uint64_t RenderContextImpl::getCompletedFenceValue() const
{
    return _completedFenceValue;
}

bool RenderContextImpl::beginFrame()
{
    // Wait fence of current frame
    auto& currentFence   = _inflightFences[_frameIndex];
    _completedFenceValue = currentFence.wait();
    _driver->processDisposalQueue(_completedFenceValue);

    if (!_frameCompletionOps.empty())
    {
        for (auto&& op : _frameCompletionOps)
            op(_completedFenceValue);
        _frameCompletionOps.clear();
    }

    currentFence.value = ++_frameFenceValue;

    if (_swapchainDirty)
    {
        auto rtImpl = static_cast<RenderTargetImpl*>(_screenRT);

        rtImpl->rebuildSwapchainBuffers(_swapchain.Get(), _screenWidth, _screenHeight, _swapchainFlags);

        _swapchainDirty = false;
    }

    // Reset offsets at the start of each frame
    _srvOffset[_frameIndex] = 0;

    resetUniformRingForCurrentFrame(_frameIndex);

    _imageIndex = _swapchain->GetCurrentBackBufferIndex();

    // Reset allocator and command list
    HRESULT hr = _commandAllocators[_frameIndex]->Reset();
    AXASSERT(SUCCEEDED(hr), "CommandAllocator Reset failed");
    _currentCmdList = _commandLists[_frameIndex].Get();
    hr              = _currentCmdList->Reset(_commandAllocators[_frameIndex].Get(), nullptr);
    AXASSERT(SUCCEEDED(hr), "CommandList Reset failed");

    _boundRootSig = nullptr;
    _boundPSO     = nullptr;

    bitmask::set(_inFlightDynamicDirtyBits[_frameIndex], PIPELINE_REQUIRED_DYNAMIC_BITS);

    // Sets descriptor heaps
    auto samplerHeap               = _driver->getSamplerHeap();
    auto srvHeap                   = _srvHeaps[_frameIndex].Get();
    ID3D12DescriptorHeap* heaps[2] = {srvHeap, samplerHeap};
    _currentCmdList->SetDescriptorHeaps(2, heaps);

    _inFrame = true;
    return true;
}

void RenderContextImpl::beginRenderPass(RenderTarget* renderTarget, const RenderPassDesc& descriptor)
{
    AXASSERT(_inFrame, "beginRenderPass must be called within a frame");
    auto rtImpl = static_cast<RenderTargetImpl*>(renderTarget);

    if (_currentRT != renderTarget)
    {
        _boundRootSig = nullptr;
        _boundPSO     = nullptr;
        _currentRT    = renderTarget;
    }

    // Get target size from color0
    auto colorAttachment = rtImpl->getColorAttachment(0);
    _renderTargetWidth   = colorAttachment->getDesc().width;
    _renderTargetHeight  = colorAttachment->getDesc().height;

    // Bind RTV/DSV and clear according to flags
    rtImpl->beginRenderPass(_currentCmdList, descriptor, _renderTargetWidth, _renderTargetHeight, _imageIndex);

    rtImpl->setLastFenceValue(_frameFenceValue);
}

void RenderContextImpl::endRenderPass()
{
    // Reset cached state objects
    static_cast<RenderTargetImpl*>(_currentRT)->endRenderPass(_currentCmdList, _imageIndex);

    _programState = nullptr;
    _vertexLayout = nullptr;

    AX_SAFE_RELEASE_NULL(_indexBuffer);
    AX_SAFE_RELEASE_NULL(_vertexBuffer);
    AX_SAFE_RELEASE_NULL(_instanceBuffer);
}

void RenderContextImpl::endFrame()
{
    // Close and execute command list
    HRESULT hr = _currentCmdList->Close();
    AXASSERT(SUCCEEDED(hr), "CommandList Close failed");
    ID3D12CommandList* lists[] = {_currentCmdList};
    _graphicsQueue->ExecuteCommandLists(1, lists);

    // Present
    hr = _swapchain->Present(_syncInterval, _presentFlags);
    if (FAILED(hr))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            HRESULT reason = _device->GetDeviceRemovedReason();
            AXLOGE("D3D12 Device remove reason: {}", reason);
        }
        else
        {
            AXLOGE("SwapChain Present failed: hr=0x{:X}", hr);
        }

        std::abort();
    }

    // Signal fence for this frame
    auto& currentFence = _inflightFences[_frameIndex];
    _graphicsQueue->Signal(currentFence.handle, currentFence.value);

    // Next frame index
    _frameIndex = (_frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;

    _driver->setFrameIndex(_frameIndex);
    _inFrame = false;
}

void RenderContextImpl::setViewport(int x, int y, unsigned int w, unsigned int h)
{
    if (w == 0 || h == 0)
        return;

    D3D12_VIEWPORT vp{.MinDepth = 0.0f, .MaxDepth = 1.0f};
    vp.TopLeftX = static_cast<float>(x);
    vp.TopLeftY = static_cast<float>(y);
    vp.Width    = static_cast<float>(w);
    vp.Height   = static_cast<float>(h);

    // Avoid redundant state if equal
    if (!dxutils::viewportsEqual(_cachedViewport, vp))
    {
        _cachedViewport = vp;
        markDynamicStateDirty(DynamicStateBits::Viewport);
    }
}

void RenderContextImpl::setScissorRect(bool enabled, float x, float y, float width, float height)
{
    D3D12_RECT rect{};
    if (enabled)
    {
        const float rtW = static_cast<float>(_renderTargetWidth);
        const float rtH = static_cast<float>(_renderTargetHeight);
        const LONG l    = static_cast<LONG>(std::clamp(x, 0.f, rtW));
        const LONG r    = static_cast<LONG>(std::clamp(x + width, 0.f, rtW));
        const LONG t    = static_cast<LONG>(std::clamp(rtH - (y + height), 0.f, rtH));
        const LONG b    = static_cast<LONG>(std::clamp(rtH - y, 0.f, rtH));
        rect.left       = (std::min)(l, r);
        rect.top        = (std::min)(t, b);
        rect.right      = (std::max)(l, r);
        rect.bottom     = (std::max)(t, b);
    }
    else
    {
        rect.left   = 0;
        rect.top    = 0;
        rect.right  = static_cast<LONG>(_renderTargetWidth);
        rect.bottom = static_cast<LONG>(_renderTargetHeight);
    }

    if (!dxutils::rectsEqual(_cachedScissor, rect))
    {
        _cachedScissor = rect;
        markDynamicStateDirty(DynamicStateBits::Scissor);
    }
}

void RenderContextImpl::setCullMode(CullMode mode)
{
    D3D12_CULL_MODE nativeMode{D3D12_CULL_MODE_NONE};
    switch (mode)
    {
    case CullMode::NONE:
        nativeMode = D3D12_CULL_MODE_NONE;
        break;
    case CullMode::BACK:
        nativeMode = D3D12_CULL_MODE_BACK;
        break;
    case CullMode::FRONT:
        nativeMode = D3D12_CULL_MODE_FRONT;
        break;
    }

    if (_cachedCullMode != nativeMode)
    {
        _cachedCullMode = nativeMode;
        markDynamicStateDirty(DynamicStateBits::CullMode);
    }
}

void RenderContextImpl::setWinding(Winding winding)
{
    BOOL isFrontCounterClockwise = winding == Winding::COUNTER_CLOCK_WISE ? TRUE : FALSE;
    if (isFrontCounterClockwise != _cachedFrontCounterClockwise)
    {
        _cachedFrontCounterClockwise = isFrontCounterClockwise;
        markDynamicStateDirty(DynamicStateBits::FrontFace);
    }
}

void RenderContextImpl::setStencilReferenceValue(uint32_t value)
{
    if (value != _stencilReferenceValue)
    {
        RenderContext::setStencilReferenceValue(value);
        markDynamicStateDirty(DynamicStateBits::StencilRef);
    }
}

void RenderContextImpl::applyPendingDynamicStates()
{
    if (bitmask::any(_inFlightDynamicDirtyBits[_frameIndex], DynamicStateBits::StencilRef))
    {
        _currentCmdList->OMSetStencilRef(_stencilReferenceValue);
        bitmask::clear(_inFlightDynamicDirtyBits[_frameIndex], DynamicStateBits::StencilRef);
    }

    if (bitmask::any(_inFlightDynamicDirtyBits[_frameIndex], DynamicStateBits::Viewport))
    {
        _currentCmdList->RSSetViewports(1, &_cachedViewport);
        bitmask::clear(_inFlightDynamicDirtyBits[_frameIndex], DynamicStateBits::Viewport);
    }

    if (bitmask::any(_inFlightDynamicDirtyBits[_frameIndex], DynamicStateBits::Scissor))
    {
        _currentCmdList->RSSetScissorRects(1, &_cachedScissor);
        bitmask::clear(_inFlightDynamicDirtyBits[_frameIndex], DynamicStateBits::Scissor);
    }
}

void RenderContextImpl::updateDepthStencilState(const DepthStencilDesc& desc)
{
    AXASSERT(_depthStencilState, "DepthStencilStateImpl not set");
    _depthStencilState->update(desc);
}

void RenderContextImpl::updatePipelineState(const RenderTarget* rt,
                                            const PipelineDesc& pipelineDesc,
                                            PrimitiveType primitiveType)
{
    RenderContext::updatePipelineState(rt, pipelineDesc, primitiveType);
    AXASSERT(_renderPipeline, "RenderPipelineImpl not set");
    _renderPipeline->prepareUpdate(_depthStencilState, _cachedCullMode, _cachedFrontCounterClockwise,
                                   toPrimitiveGroup(primitiveType));
    _renderPipeline->update(rt, pipelineDesc);

    _currentCmdList->IASetPrimitiveTopology(toD3DTopology(primitiveType));

    // Bind PSO & RootSignature
    uint32_t dirtyFlags = 0;
    auto rootSigInfo    = _renderPipeline->getRootSignature();
    auto rootSig        = rootSigInfo->rootSig.Get();
    if (_boundRootSig != rootSig)
    {
        _currentCmdList->SetGraphicsRootSignature(rootSig);
        _boundRootSig = rootSig;
        dirtyFlags |= 1;
    }

    auto pso = _renderPipeline->getPipelineState();
    if (pso != _boundPSO)
    {
        _currentCmdList->SetPipelineState(pso);
        dirtyFlags |= 2;
        _boundPSO = pso;
    }

    const auto samplerRootIndex = rootSigInfo->samplerRootIndex;
    if (dirtyFlags && samplerRootIndex != UINT_MAX)
    {
        const auto samplerGpuStart = _driver->getSamplerHeap()->GetGPUDescriptorHandleForHeapStart();
        _currentCmdList->SetGraphicsRootDescriptorTable(samplerRootIndex, samplerGpuStart);
    }
}

void RenderContextImpl::setVertexBuffer(Buffer* buffer)
{
    if (!buffer || _vertexBuffer == buffer)
        return;
    buffer->retain();
    AX_SAFE_RELEASE(_vertexBuffer);
    _vertexBuffer = static_cast<BufferImpl*>(buffer);
}

void RenderContextImpl::setIndexBuffer(Buffer* buffer)
{
    if (!buffer || _indexBuffer == buffer)
        return;
    buffer->retain();
    AX_SAFE_RELEASE(_indexBuffer);
    _indexBuffer = static_cast<BufferImpl*>(buffer);
}

void RenderContextImpl::setInstanceBuffer(Buffer* buffer)
{
    if (!buffer || _instanceBuffer == buffer)
        return;
    buffer->retain();
    AX_SAFE_RELEASE(_instanceBuffer);
    _instanceBuffer = static_cast<BufferImpl*>(buffer);
}

void RenderContextImpl::drawArrays(std::size_t start, std::size_t count, bool /*wireframe*/)
{
    AXASSERT(_renderPipeline && _vertexBuffer, "Pipeline and vertex buffer must be set");

    prepareDrawing(_currentCmdList);

    _currentCmdList->DrawInstanced(static_cast<UINT>(count), 1, static_cast<UINT>(start), 0);
}

void RenderContextImpl::drawArraysInstanced(std::size_t start, std::size_t count, int instanceCount, bool /*wireframe*/)
{
    AXASSERT(_renderPipeline && _vertexBuffer, "Pipeline and vertex buffer must be set");

    prepareDrawing(_currentCmdList);

    _currentCmdList->DrawInstanced(static_cast<UINT>(count), static_cast<UINT>(instanceCount), static_cast<UINT>(start),
                                   0);
}

void RenderContextImpl::drawElements(IndexFormat indexType, std::size_t count, std::size_t offset, bool /*wireframe*/)
{
    AXASSERT(_renderPipeline && _vertexBuffer && _indexBuffer, "Pipeline, vertex and index buffers must be set");

    prepareDrawing(_currentCmdList);

    _indexBuffer->setLastFenceValue(_frameFenceValue);

    // IB
    D3D12_INDEX_BUFFER_VIEW ibv{};
    ibv.BufferLocation = _indexBuffer->internalResource()->GetGPUVirtualAddress() + static_cast<UINT64>(offset);
    ibv.SizeInBytes    = static_cast<UINT>(_indexBuffer->getSize() - offset);
    ibv.Format         = toDxgiIndexFormat(indexType);
    _currentCmdList->IASetIndexBuffer(&ibv);

    _currentCmdList->DrawIndexedInstanced(static_cast<UINT>(count), 1, 0, 0, 0);
}

void RenderContextImpl::drawElementsInstanced(IndexFormat indexType,
                                              std::size_t count,
                                              std::size_t offset,
                                              int instanceCount,
                                              bool /*wireframe*/)
{
    AXASSERT(_renderPipeline && _vertexBuffer && _indexBuffer, "Pipeline, vertex and index buffers must be set");

    prepareDrawing(_currentCmdList);

    _indexBuffer->setLastFenceValue(_frameFenceValue);

    // IB
    D3D12_INDEX_BUFFER_VIEW ibv{};
    ibv.BufferLocation = _indexBuffer->internalResource()->GetGPUVirtualAddress() + static_cast<UINT64>(offset);
    ibv.SizeInBytes    = static_cast<UINT>(_indexBuffer->getSize() - offset);
    ibv.Format         = toDxgiIndexFormat(indexType);
    _currentCmdList->IASetIndexBuffer(&ibv);

    _currentCmdList->DrawIndexedInstanced(static_cast<UINT>(count), static_cast<UINT>(instanceCount), 0, 0, 0);
}

void RenderContextImpl::prepareDrawing(ID3D12GraphicsCommandList* cmd)
{
    // callback uniforms
    auto& callbackUniforms = _programState->getCallbackUniforms();
    for (auto& cb : callbackUniforms)
        cb.second(_programState, cb.first);

    auto rootSigInfo = _renderPipeline->getRootSignature();

    applyPendingDynamicStates();

    _vertexBuffer->setLastFenceValue(_frameFenceValue);

    // bind vertex buffers
    if (!_instanceBuffer)
    {
        D3D12_VERTEX_BUFFER_VIEW vbv{};
        vbv.BufferLocation = _vertexBuffer->internalResource()->GetGPUVirtualAddress();
        vbv.SizeInBytes    = static_cast<UINT>(_vertexBuffer->getSize());
        vbv.StrideInBytes  = static_cast<UINT>(_vertexLayout->getStride());
        _currentCmdList->IASetVertexBuffers(0, 1, &vbv);
    }
    else
    {
        _instanceBuffer->setLastFenceValue(_frameFenceValue);
        D3D12_VERTEX_BUFFER_VIEW views[2]{};
        views[0].BufferLocation = _vertexBuffer->internalResource()->GetGPUVirtualAddress();
        views[0].SizeInBytes    = static_cast<UINT>(_vertexBuffer->getSize());
        views[0].StrideInBytes  = static_cast<UINT>(_vertexLayout->getStride());

        views[1].BufferLocation = _instanceBuffer->internalResource()->GetGPUVirtualAddress();
        views[1].SizeInBytes    = static_cast<UINT>(_instanceBuffer->getSize());
        views[1].StrideInBytes  = static_cast<UINT>(_vertexLayout->getInstanceStride());

        _currentCmdList->IASetVertexBuffers(0, 2, views);
    }

    // bind ubos
    const auto& cpuBuffer = _programState->getUniformBuffer();
    if (!cpuBuffer.empty())
    {
        for (auto& uboInfo : _programState->getActiveUniformBlockInfos())
        {
            auto s = allocateUniformSlice(_frameIndex, uboInfo.sizeBytes);
            ::memcpy(s.cpuPtr, cpuBuffer.data() + uboInfo.cpuOffset, uboInfo.sizeBytes);
            _currentCmdList->SetGraphicsRootConstantBufferView(uboInfo.binding, s.gpuVA);
        }
    }

    // --- bind textures ---
    auto srvHeap = _srvHeaps[_frameIndex].Get();

    // CPU start handles
    auto srvCpuStart = srvHeap->GetCPUDescriptorHandleForHeapStart();

    const auto srvStride     = _driver->getSrvDescriptorStride();
    auto& textureBindingSets = _programState->getTextureBindingSets();
    if (!textureBindingSets.empty())
    {
        const auto bindingStart = _srvOffset[_frameIndex];

        // Copy descriptors for each texture in the binding set
        int maxSlot = -1;
        for (auto& [bindingIndex, bindingSet] : textureBindingSets)
        {
            const auto count = static_cast<int>(bindingSet.texs.size());
            for (int i = 0; i < count; ++i)
            {
                const int slot = bindingIndex + i;
                if (maxSlot < slot)
                    maxSlot = slot;

                auto textureImpl = static_cast<TextureImpl*>(bindingSet.texs[i]);
                textureImpl->setLastFenceValue(_frameFenceValue);
                auto srvHandle = textureImpl->internalHandle().srv;
                assert(!!srvHandle);

                auto dstSrv = srvCpuStart;
                dstSrv.ptr += (bindingStart + slot) * srvStride;
                _device->CopyDescriptorsSimple(1, dstSrv, srvHandle->cpu, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }
        }

        // Bind GPU handles for this batch
        auto srvGpuStart = srvHeap->GetGPUDescriptorHandleForHeapStart();
        srvGpuStart.ptr += static_cast<UINT64>(bindingStart) * srvStride;
        _currentCmdList->SetGraphicsRootDescriptorTable(rootSigInfo->srvRootIndex, srvGpuStart);

        // Advance offsets for the next batch
        _srvOffset[_frameIndex] = bindingStart + static_cast<UINT>(maxSlot + 1);
    }
}

void RenderContextImpl::createUniformRingBuffers(std::size_t capacityBytes)
{
    // Enforce minimum alignment-friendly capacity
    if (capacityBytes == 0)
        capacityBytes = 1 << 20;  // default 1MB if caller passes 0

    for (auto& ring : _uniformRings)
    {
        // Describe buffer in Upload heap (CPU-write, GPU-read)
        D3D12_HEAP_PROPERTIES heapProps{};
        heapProps.Type                 = D3D12_HEAP_TYPE_UPLOAD;
        heapProps.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProps.VisibleNodeMask      = 1;
        heapProps.CreationNodeMask     = 1;

        D3D12_RESOURCE_DESC bufDesc{};
        bufDesc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufDesc.Alignment          = 0;
        bufDesc.Width              = capacityBytes;
        bufDesc.Height             = 1;
        bufDesc.DepthOrArraySize   = 1;
        bufDesc.MipLevels          = 1;
        bufDesc.Format             = DXGI_FORMAT_UNKNOWN;
        bufDesc.SampleDesc.Count   = 1;
        bufDesc.SampleDesc.Quality = 0;
        bufDesc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        bufDesc.Flags              = D3D12_RESOURCE_FLAG_NONE;

        Microsoft::WRL::ComPtr<ID3D12Resource> res;
        HRESULT hr = _device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufDesc,
                                                      D3D12_RESOURCE_STATE_GENERIC_READ,  // Upload heap default state
                                                      nullptr, IID_PPV_ARGS(&res));
        AXASSERT(SUCCEEDED(hr), "Failed to create uniform ring buffer");

        // Persistently map the buffer
        uint8_t* mapped = nullptr;
        D3D12_RANGE noRead{0, 0};  // we won't read from CPU
        hr = res->Map(0, &noRead, reinterpret_cast<void**>(&mapped));
        AXASSERT(SUCCEEDED(hr), "Failed to map uniform ring buffer");

        ring.resource  = res;
        ring.mapped    = mapped;
        ring.capacity  = capacityBytes;
        ring.writeHead = 0;
        ring.align     = 256;  // CBV min alignment
        ring.baseGpuVA = res->GetGPUVirtualAddress();
    }
}

void RenderContextImpl::destroyUniformRingBuffers()
{
    for (auto& ring : _uniformRings)
    {
        if (ring.resource)
        {
            // Unmap on destruction (optional for Upload heap, but clean)
            ring.resource->Unmap(0, nullptr);
            ring.resource.Reset();
        }
        ring.mapped    = nullptr;
        ring.capacity  = 0;
        ring.writeHead = 0;
        ring.baseGpuVA = 0;
    }
}

// Reset ring for the given frame (call after GPU finished that frame)
void RenderContextImpl::resetUniformRingForCurrentFrame(UINT frameIndex)
{
    AXASSERT(frameIndex < _uniformRings.size(), "Invalid frame index");
    auto& ring = _uniformRings[frameIndex];
    ring.reset();
}

// Allocate an aligned slice for the given frame
RenderContextImpl::UniformSlice RenderContextImpl::allocateUniformSlice(UINT frameIndex, std::size_t size)
{
    AXASSERT(frameIndex < _uniformRings.size(), "Invalid frame index");
    auto& ring = _uniformRings[frameIndex];
    AXASSERT(ring.valid(), "Uniform ring buffer not initialized");

    // Align size and head to 256-byte boundary to satisfy CBV requirements
    auto alignMask          = ring.align - 1;
    std::size_t alignedSize = (size + alignMask) & ~alignMask;
    std::size_t alignedHead = (ring.writeHead + alignMask) & ~alignMask;

    // Simple wrap-around strategy: reset if not enough room
    if (alignedHead + alignedSize > ring.capacity)
    {
        // In a robust system, you'd either assert, grow, or sub-allocate fallback.
        // Here we wrap to start and expect per-frame reset is used correctly.
        alignedHead = 0;
    }

    AXASSERT(alignedHead + alignedSize <= ring.capacity, "Uniform ring buffer overflow");

    UniformSlice slice;
    slice.offset = alignedHead;
    slice.size   = size;
    slice.cpuPtr = ring.mapped + alignedHead;
    slice.gpuVA  = ring.baseGpuVA + alignedHead;

    ring.writeHead = alignedHead + alignedSize;
    return slice;
}

void RenderContextImpl::readPixels(RenderTarget* rt,
                                   bool preserveAxisHint,
                                   std::function<void(const PixelBufferDesc&)> callback)
{
    if (!rt)
    {
        callback({});
        return;
    }
    rt->retain();

    _frameCompletionOps.emplace_back([this, rt, preserveAxisHint, callback = std::move(callback)](uint64_t) mutable {
        readPixelsInternal(rt, preserveAxisHint, callback);

        rt->release();
    });
}

void RenderContextImpl::readPixelsInternal(RenderTarget* rt,
                                           bool preserveAxisHint,
                                           std::function<void(const PixelBufferDesc&)>& callback)
{
    PixelBufferDesc pbd{};
    auto* rtImpl = static_cast<RenderTargetImpl*>(rt);

    auto colorAttachment = rtImpl->getColorAttachment(0);
    if (!colorAttachment)
    {
        callback(pbd);
        return;
    }

    const auto& desc  = colorAttachment->getDesc();
    const UINT width  = desc.width;
    const UINT height = desc.height;

    // Query footprint for readback
    const auto nativeTexDesc = colorAttachment->internalHandle().resource->GetDesc();
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
    UINT numRows          = 0;
    UINT64 rowSizeInBytes = 0;
    UINT64 totalBytes     = 0;
    _device->GetCopyableFootprints(&nativeTexDesc, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalBytes);

    // Create readback buffer sized to footprint
    Microsoft::WRL::ComPtr<ID3D12Resource> readback;
    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_READBACK;

    D3D12_RESOURCE_DESC bufDesc{};
    bufDesc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufDesc.Width            = totalBytes;  // must match footprint size
    bufDesc.Height           = 1;
    bufDesc.DepthOrArraySize = 1;
    bufDesc.MipLevels        = 1;
    bufDesc.Format           = DXGI_FORMAT_UNKNOWN;
    bufDesc.SampleDesc.Count = 1;
    bufDesc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    HRESULT hr = _device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufDesc,
                                                  D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&readback));
    AXASSERT(SUCCEEDED(hr), "CreateCommittedResource READBACK failed");

    // Record copy from texture to readback
    auto& cmd = _driver->startIsolateSubmission();
    colorAttachment->transitionState(cmd, D3D12_RESOURCE_STATE_COPY_SOURCE);

    D3D12_TEXTURE_COPY_LOCATION dst{};
    dst.pResource       = readback.Get();
    dst.Type            = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    dst.PlacedFootprint = footprint;

    D3D12_TEXTURE_COPY_LOCATION src{};
    src.pResource        = colorAttachment->internalHandle().resource.Get();
    src.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    src.SubresourceIndex = 0;

    cmd->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

    // Transition source back to sampling
    colorAttachment->transitionState(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    _driver->finishIsolateSubmission(cmd, true);

    // Map and read data
    void* mapped = nullptr;
    D3D12_RANGE readRange{0, totalBytes};
    hr = readback->Map(0, &readRange, &mapped);
    if (SUCCEEDED(hr) && mapped)
    {
        pbd._width  = width;
        pbd._height = height;
        pbd._data.resize(width * height * 4);

        BYTE* srcData = reinterpret_cast<BYTE*>(mapped);
        BYTE* dstData = pbd._data.data();

        // Copy row by row, removing padding
        for (UINT row = 0; row < height; ++row)
        {
            memcpy(dstData + row * width * 4, srcData + row * footprint.Footprint.RowPitch, width * 4);
        }

        D3D12_RANGE written{0, 0};
        readback->Unmap(0, &written);
    }
    else
    {
        AXLOGE("RenderContextImpl::readPixelsInternal fail, hr={:x}", hr);
    }

    callback(pbd);
}

void RenderContextImpl::removeCachedPipelineObjects(Program* key)
{
    if (_renderPipeline)
        _renderPipeline->removeCachedObjects(key);
}

}  // namespace ax::rhi::d3d12
