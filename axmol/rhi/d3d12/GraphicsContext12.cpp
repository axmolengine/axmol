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
#include "axmol/rhi/d3d12/GraphicsContext12.h"
#include "axmol/rhi/d3d12/GraphicsDevice12.h"
#include "axmol/rhi/d3d12/RenderTarget12.h"
#include "axmol/rhi/d3d12/RenderPipeline12.h"
#include "axmol/rhi/d3d12/DepthStencilState12.h"
#include "axmol/rhi/d3d12/VertexLayout12.h"
#include "axmol/rhi/d3d12/Program12.h"
#include "axmol/rhi/d3d12/Buffer12.h"
#include "axmol/rhi/d3d12/Texture12.h"
#include "axmol/rhi/d3d12/ComputePipeline12.h"
#include "axmol/rhi/SamplerRegistry.h"
#include "axmol/base/Logging.h"
#include "axmol/math/MathUtil.h"

#include <algorithm>

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

GraphicsContextImpl::GraphicsContextImpl(GraphicsDeviceImpl* driver, SurfaceHandle surface) : _driver(driver)
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

    // Effekseer's default GPU particle system can submit clear/spawn/update/render
    // constants for up to 256 emitters in one frame. Keep enough headroom for
    // those allocations and the rest of the scene without reusing an in-flight
    // slice from the same frame.
    createUniformRingBuffers(4 * 1024 * 1024);  // 4 MB per frame

    createDescriptorHeaps();

    for (auto& stateBits : _inFlightDynamicDirtyBits)
        bitmask::set(stateBits, PIPELINE_REQUIRED_DYNAMIC_BITS);
}

GraphicsContextImpl::~GraphicsContextImpl()
{
    _driver->waitForGPU();

    for (auto& pipelines : _inFlightComputePipelines)
        pipelines.clear();

    if (!_frameCompletionOps.empty())
    {
        for (auto&& op : _frameCompletionOps)
            op(_completedFenceValue);
        _frameCompletionOps.clear();
    }

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

void GraphicsContextImpl::createCommandObjects()
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

void GraphicsContextImpl::createDescriptorHeaps()
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

bool GraphicsContextImpl::updateSurface(SurfaceHandle /*surface*/, uint32_t width, uint32_t height)
{
    // For D3D12, surface is HWND; if resized, mark for swapchain resize
    if (_screenWidth == width && _screenHeight == height)
        return true;

    _screenWidth  = width;
    _screenHeight = height;

    _swapchainDirty = true;

    return true;
}

void GraphicsContextImpl::setDepthStencilState(DepthStencilState* depthStencilState)
{
    _depthStencilState = static_cast<DepthStencilStateImpl*>(depthStencilState);
}

void GraphicsContextImpl::setRenderPipeline(RenderPipeline* renderPipeline)
{
    Object::assign(_renderPipeline, static_cast<RenderPipelineImpl*>(renderPipeline));
}

uint64_t GraphicsContextImpl::getCompletedFenceValue() const
{
    return _completedFenceValue;
}

bool GraphicsContextImpl::beginFrame()
{
    // Wait fence of current frame
    auto& currentFence   = _inflightFences[_frameIndex];
    _completedFenceValue = currentFence.wait();
    _driver->processDisposalQueue(_completedFenceValue);

    // Release compute pipelines retained for this frame now that its fence has completed.
    _inFlightComputePipelines[_frameIndex].clear();

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

void GraphicsContextImpl::beginRenderPass(RenderTarget* renderTarget, const RenderPassDesc& descriptor)
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

void GraphicsContextImpl::endRenderPass()
{
    // Reset cached state objects
    static_cast<RenderTargetImpl*>(_currentRT)->endRenderPass(_currentCmdList, _imageIndex);

    _programState = nullptr;
    _vertexLayout = nullptr;

    AX_SAFE_RELEASE_NULL(_indexBuffer);
    AX_SAFE_RELEASE_NULL(_vertexBuffer);
    AX_SAFE_RELEASE_NULL(_instanceBuffer);
}

void GraphicsContextImpl::submitCurrentFrameCommands(bool /*waitForCompletion*/)
{
    if (!_inFrame || !_currentCmdList)
        return;

    HRESULT hr = _currentCmdList->Close();
    AXASSERT(SUCCEEDED(hr), "CommandList Close failed");

    ID3D12CommandList* lists[] = {_currentCmdList};
    _graphicsQueue->ExecuteCommandLists(1, lists);

    auto& currentFence = _inflightFences[_frameIndex];
    _graphicsQueue->Signal(currentFence.handle, currentFence.value);
    _completedFenceValue = currentFence.wait();
    _driver->processDisposalQueue(_completedFenceValue);

    currentFence.value = ++_frameFenceValue;

    hr = _commandAllocators[_frameIndex]->Reset();
    AXASSERT(SUCCEEDED(hr), "CommandAllocator Reset failed");
    hr = _currentCmdList->Reset(_commandAllocators[_frameIndex].Get(), nullptr);
    AXASSERT(SUCCEEDED(hr), "CommandList Reset failed");

    _boundRootSig = nullptr;
    _boundPSO     = nullptr;
    bitmask::set(_inFlightDynamicDirtyBits[_frameIndex], PIPELINE_REQUIRED_DYNAMIC_BITS);

    auto samplerHeap               = _driver->getSamplerHeap();
    auto srvHeap                   = _srvHeaps[_frameIndex].Get();
    ID3D12DescriptorHeap* heaps[2] = {srvHeap, samplerHeap};
    _currentCmdList->SetDescriptorHeaps(2, heaps);
}

void GraphicsContextImpl::endFrame()
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

        abort();
    }

    // Signal fence for this frame
    auto& currentFence = _inflightFences[_frameIndex];
    _graphicsQueue->Signal(currentFence.handle, currentFence.value);

    // Next frame index
    _frameIndex = (_frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;

    _driver->setFrameIndex(_frameIndex);
    _inFrame = false;
}

void GraphicsContextImpl::setViewport(int x, int y, unsigned int w, unsigned int h)
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

void GraphicsContextImpl::setScissorRect(bool enabled, float x, float y, float width, float height)
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

void GraphicsContextImpl::setCullMode(CullMode mode)
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

void GraphicsContextImpl::setWinding(Winding winding)
{
    BOOL isFrontCounterClockwise = winding == Winding::COUNTER_CLOCK_WISE ? TRUE : FALSE;
    if (isFrontCounterClockwise != _cachedFrontCounterClockwise)
    {
        _cachedFrontCounterClockwise = isFrontCounterClockwise;
        markDynamicStateDirty(DynamicStateBits::FrontFace);
    }
}

void GraphicsContextImpl::setStencilReferenceValue(uint32_t value)
{
    if (value != _stencilReferenceValue)
    {
        GraphicsContext::setStencilReferenceValue(value);
        markDynamicStateDirty(DynamicStateBits::StencilRef);
    }
}

void GraphicsContextImpl::applyPendingDynamicStates()
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

void GraphicsContextImpl::updateDepthStencilState(const DepthStencilDesc& desc)
{
    AXASSERT(_depthStencilState, "DepthStencilStateImpl not set");
    _depthStencilState->update(desc);
}

void GraphicsContextImpl::updatePipelineState(const RenderTarget* rt,
                                              const PipelineDesc& pipelineDesc,
                                              PrimitiveType primitiveType)
{
    GraphicsContext::updatePipelineState(rt, pipelineDesc, primitiveType);
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

    const auto customSamplerRootIndex = rootSigInfo->customSamplerRootIndex;
    if (customSamplerRootIndex != UINT_MAX)
    {
        auto* batch = _renderPipeline->getCustomSamplerBatch(_programState);
        AXASSERT(batch, "Failed to resolve D3D12 custom sampler descriptors");
        if (batch)
            _currentCmdList->SetGraphicsRootDescriptorTable(customSamplerRootIndex, batch->gpu);
    }
}

void GraphicsContextImpl::setVertexBuffer(Buffer* buffer)
{
    if (!buffer || _vertexBuffer == buffer)
        return;
    buffer->retain();
    AX_SAFE_RELEASE(_vertexBuffer);
    _vertexBuffer = static_cast<BufferImpl*>(buffer);
}

void GraphicsContextImpl::setIndexBuffer(Buffer* buffer)
{
    if (!buffer || _indexBuffer == buffer)
        return;
    buffer->retain();
    AX_SAFE_RELEASE(_indexBuffer);
    _indexBuffer = static_cast<BufferImpl*>(buffer);
}

void GraphicsContextImpl::setInstanceBuffer(Buffer* buffer)
{
    if (!buffer)
    {
        AX_SAFE_RELEASE_NULL(_instanceBuffer);
        return;
    }
    if (_instanceBuffer == buffer)
        return;
    buffer->retain();
    AX_SAFE_RELEASE(_instanceBuffer);
    _instanceBuffer = static_cast<BufferImpl*>(buffer);
}

void GraphicsContextImpl::drawArrays(size_t start, size_t count, bool /*wireframe*/)
{
    AXASSERT(_renderPipeline && _vertexBuffer, "Pipeline and vertex buffer must be set");

    prepareDrawing(_currentCmdList);

    _currentCmdList->DrawInstanced(static_cast<UINT>(count), 1, static_cast<UINT>(start), 0);
}

void GraphicsContextImpl::drawArraysInstanced(size_t start, size_t count, int instanceCount, bool /*wireframe*/)
{
    AXASSERT(_renderPipeline && _vertexBuffer, "Pipeline and vertex buffer must be set");

    prepareDrawing(_currentCmdList);

    _currentCmdList->DrawInstanced(static_cast<UINT>(count), static_cast<UINT>(instanceCount), static_cast<UINT>(start),
                                   0);
}

void GraphicsContextImpl::drawElements(IndexFormat indexType, size_t count, size_t offset, bool /*wireframe*/)
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

void GraphicsContextImpl::drawElementsInstanced(IndexFormat indexType,
                                                size_t count,
                                                size_t offset,
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

void GraphicsContextImpl::prepareDrawing(ID3D12GraphicsCommandList* cmd)
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
    auto& storageBindingSets = _programState->getStorageBufferBindingSets();
    if (!textureBindingSets.empty() || !storageBindingSets.empty())
    {
        const auto bindingStart = _srvOffset[_frameIndex];
        UINT slot               = 0;

        // Transition storage buffers from compute write state (UAV) to graphics
        // read state (SRV) before the draw consumes them.
        tlx::pod_vector<D3D12_RESOURCE_BARRIER> storageBarriers;
        constexpr D3D12_RESOURCE_STATES storageTargetState =
            D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        for (auto& [binding, bindingSet] : storageBindingSets)
        {
            if (!bindingSet.buffer)
                continue;
            auto bufferImpl = static_cast<BufferImpl*>(bindingSet.buffer);
            bufferImpl->setLastFenceValue(_frameFenceValue);
            if (bufferImpl->currentState() != storageTargetState)
            {
                D3D12_RESOURCE_BARRIER& bar = storageBarriers.emplace_back();
                bar.Type                    = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                bar.Transition.pResource    = bufferImpl->internalResource();
                bar.Transition.StateBefore  = bufferImpl->currentState();
                bar.Transition.StateAfter   = storageTargetState;
                bar.Transition.Subresource  = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                bufferImpl->_resourceState  = storageTargetState;
            }
        }
        if (!storageBarriers.empty())
            cmd->ResourceBarrier(static_cast<UINT>(storageBarriers.size()), storageBarriers.data());

        // Copy resources in exactly the same reflection order used to build
        // the root-signature descriptor ranges.
        auto* program = _programState->getProgram();
        for (const auto& storageInfo : program->getActiveStorageBufferInfos())
        {
            auto binding = storageBindingSets.find(storageInfo.binding);
            if (binding == storageBindingSets.end() || !binding->second.buffer)
                continue;
            auto bufferImpl = static_cast<BufferImpl*>(binding->second.buffer);
            auto srvHandle  = bufferImpl->getSRV();
            if (!srvHandle)
                continue;

            auto dstSrv = srvCpuStart;
            dstSrv.ptr += (bindingStart + slot) * srvStride;
            _device->CopyDescriptorsSimple(1, dstSrv, srvHandle->cpu, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            ++slot;
        }

        for (const auto& [_, textureInfo] : program->getActiveTextureInfos())
        {
            auto binding = textureBindingSets.find(textureInfo->location);
            if (binding == textureBindingSets.end())
                continue;
            auto& bindingSet = binding->second;
            const auto count = static_cast<int>(bindingSet.texs.size());
            for (int i = 0; i < count; ++i)
            {
                auto textureImpl = static_cast<TextureImpl*>(bindingSet.texs[i]);
                textureImpl->setLastFenceValue(_frameFenceValue);
                auto srvHandle = textureImpl->internalHandle().srv;
                assert(!!srvHandle);

                auto dstSrv = srvCpuStart;
                dstSrv.ptr += (bindingStart + slot) * srvStride;
                _device->CopyDescriptorsSimple(1, dstSrv, srvHandle->cpu, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                ++slot;
            }
        }

        if (slot > 0 && rootSigInfo->srvRootIndex != UINT_MAX)
        {
            auto srvGpuStart = srvHeap->GetGPUDescriptorHandleForHeapStart();
            srvGpuStart.ptr += static_cast<UINT64>(bindingStart) * srvStride;
            _currentCmdList->SetGraphicsRootDescriptorTable(rootSigInfo->srvRootIndex, srvGpuStart);
        }

        _srvOffset[_frameIndex] = bindingStart + slot;
    }
}

void GraphicsContextImpl::createUniformRingBuffers(size_t capacityBytes)
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

void GraphicsContextImpl::destroyUniformRingBuffers()
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
void GraphicsContextImpl::resetUniformRingForCurrentFrame(UINT frameIndex)
{
    AXASSERT(frameIndex < _uniformRings.size(), "Invalid frame index");
    auto& ring = _uniformRings[frameIndex];
    ring.reset();
}

// Allocate an aligned slice for the given frame
GraphicsContextImpl::UniformSlice GraphicsContextImpl::allocateUniformSlice(UINT frameIndex, size_t size)
{
    AXASSERT(frameIndex < _uniformRings.size(), "Invalid frame index");
    auto& ring = _uniformRings[frameIndex];
    AXASSERT(ring.valid(), "Uniform ring buffer not initialized");

    // Align size and head to 256-byte boundary to satisfy CBV requirements
    auto alignMask     = ring.align - 1;
    size_t alignedSize = (size + alignMask) & ~alignMask;
    size_t alignedHead = (ring.writeHead + alignMask) & ~alignMask;

    AXASSERT(alignedHead + alignedSize <= ring.capacity, "Uniform ring buffer overflow");

    UniformSlice slice;
    slice.offset = alignedHead;
    slice.size   = size;
    slice.cpuPtr = ring.mapped + alignedHead;
    slice.gpuVA  = ring.baseGpuVA + alignedHead;

    ring.writeHead = alignedHead + alignedSize;
    return slice;
}

void GraphicsContextImpl::readPixels(RenderTarget* rt, std::function<void(const PixelBufferDesc&)> callback)
{
    if (!rt)
    {
        callback({});
        return;
    }
    rt->retain();

    _frameCompletionOps.emplace_back([this, rt, callback = std::move(callback)](uint64_t) mutable {
        readPixelsInternal(rt, callback);

        rt->release();
    });
}

void GraphicsContextImpl::readPixelsInternal(RenderTarget* rt, std::function<void(const PixelBufferDesc&)>& callback)
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
        AXLOGE("GraphicsContextImpl::readPixelsInternal fail, hr={:x}", hr);
    }

    callback(pbd);
}

void GraphicsContextImpl::removeCachedPipelineObjects(Program* key)
{
    if (_renderPipeline)
        _renderPipeline->removeCachedObjects(key);
}

bool GraphicsContextImpl::dispatch(const ComputeDispatchDesc& desc)
{
    if (!_inFrame || !_currentCmdList)
        return false;
    if (!desc.programState || !desc.pipeline)
        return false;

    auto* pipelineProgram = desc.pipeline->getProgram();
    if (!pipelineProgram || pipelineProgram != desc.programState->getProgram())
    {
        AXASSERT(false, "ComputePipeline and ProgramState program mismatch");
        return false;
    }

    auto program = static_cast<ProgramImpl*>(pipelineProgram);
    if (!program || !program->getCSModule())
        return false;

    const auto& storageBindings = desc.programState->getStorageBufferBindingSets();
    for (const auto& storageInfo : program->getActiveStorageBufferInfos())
    {
        auto binding = storageBindings.find(storageInfo.binding);
        if (binding == storageBindings.end() || !binding->second.buffer)
        {
            AXLOGE("Missing D3D12 compute storage buffer binding {} ({})", storageInfo.binding, storageInfo.name);
            AXASSERT(false, "Missing D3D12 compute storage buffer binding");
            return false;
        }
        if (binding->second.access != storageInfo.access)
        {
            AXLOGE("D3D12 compute storage buffer binding {} has incompatible access", storageInfo.binding);
            AXASSERT(false, "D3D12 compute storage buffer access mismatch");
            return false;
        }

        auto bufferImpl = static_cast<BufferImpl*>(binding->second.buffer);
        if (!bufferImpl->internalResource() ||
            (storageInfo.sizeBytes != 0 && bufferImpl->getSize() < storageInfo.sizeBytes) ||
            (storageInfo.arrayStride != 0 && bufferImpl->getStride() != storageInfo.arrayStride))
        {
            AXLOGE("D3D12 compute storage buffer binding {} does not match its reflected resource",
                   storageInfo.binding);
            AXASSERT(false, "D3D12 compute storage buffer resource mismatch");
            return false;
        }

        const bool hasView = storageInfo.access == BufferAccess::READ_WRITE ? bufferImpl->getUAV() != nullptr
                                                                            : bufferImpl->getSRV() != nullptr;
        if (!hasView)
        {
            AXLOGE("D3D12 compute storage buffer binding {} has no native view", storageInfo.binding);
            AXASSERT(false, "D3D12 compute storage buffer has no native view");
            return false;
        }
    }

    auto* computePipeline = static_cast<ComputePipelineImpl*>(desc.pipeline);
    if (!computePipeline->getPipeline())
        return false;

    _programState = desc.programState;

    auto* cmd = _currentCmdList;
    cmd->SetComputeRootSignature(computePipeline->getRootSignature());
    cmd->SetPipelineState(computePipeline->getPipeline());

    // Compute uses the same command list: invalidate the graphics pipeline
    // cache so a later draw re-binds its root signature and PSO.
    _boundRootSig = nullptr;
    _boundPSO     = nullptr;

    // Bind uniform blocks via CBV root parameters.
    auto& callbackUniforms = desc.programState->getCallbackUniforms();
    for (auto& cb : callbackUniforms)
        cb.second(_programState, cb.first);

    const auto& cpuBuffer = desc.programState->getUniformBuffer();
    if (!cpuBuffer.empty())
    {
        for (auto& uboInfo : desc.programState->getActiveUniformBlockInfos())
        {
            auto s = allocateUniformSlice(_frameIndex, uboInfo.sizeBytes);
            ::memcpy(s.cpuPtr, cpuBuffer.data() + uboInfo.cpuOffset, uboInfo.sizeBytes);
            const auto rootIndex = computePipeline->cbvRootIndex(uboInfo.binding);
            AXASSERT(rootIndex != UINT_MAX, "Missing D3D12 compute CBV root parameter");
            if (rootIndex != UINT_MAX)
                cmd->SetComputeRootConstantBufferView(rootIndex, s.gpuVA);
        }
    }

    auto srvHeap         = _srvHeaps[_frameIndex].Get();
    auto srvCpuStart     = srvHeap->GetCPUDescriptorHandleForHeapStart();
    auto srvGpuStart     = srvHeap->GetGPUDescriptorHandleForHeapStart();
    const auto srvStride = _driver->getSrvDescriptorStride();

    const UINT bindingStart = _srvOffset[_frameIndex];
    UINT slot               = 0;

    // Copy SRV descriptors (read-only storage buffers, then textures) in
    // root-signature range order (ComputePipeline12::createRootSignature).
    for (const auto& storageInfo : program->getActiveStorageBufferInfos())
    {
        if (storageInfo.access != BufferAccess::READ_ONLY)
            continue;
        auto binding = storageBindings.find(storageInfo.binding);
        if (binding == storageBindings.end() || !binding->second.buffer)
            continue;
        auto bufferImpl = static_cast<BufferImpl*>(binding->second.buffer);
        bufferImpl->setLastFenceValue(_frameFenceValue);
        auto srvHandle = bufferImpl->getSRV();
        if (!srvHandle)
            continue;

        D3D12_CPU_DESCRIPTOR_HANDLE dst = srvCpuStart;
        dst.ptr += (bindingStart + slot) * srvStride;
        _device->CopyDescriptorsSimple(1, dst, srvHandle->cpu, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        ++slot;
    }
    const auto& textureBindings = desc.programState->getTextureBindingSets();
    for (const auto& [_, textureInfo] : program->getActiveTextureInfos())
    {
        auto binding = textureBindings.find(textureInfo->location);
        if (binding == textureBindings.end())
            continue;
        const auto& bindingSet = binding->second;
        for (auto tex : bindingSet.texs)
        {
            auto textureImpl = static_cast<TextureImpl*>(tex);
            textureImpl->setLastFenceValue(_frameFenceValue);
            auto srvHandle = textureImpl->internalHandle().srv;
            if (!srvHandle)
                continue;

            D3D12_CPU_DESCRIPTOR_HANDLE dst = srvCpuStart;
            dst.ptr += (bindingStart + slot) * srvStride;
            _device->CopyDescriptorsSimple(1, dst, srvHandle->cpu, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            ++slot;
        }
    }
    const UINT srvCount = slot;

    // Copy UAV descriptors (read-write storage buffers).
    for (const auto& storageInfo : program->getActiveStorageBufferInfos())
    {
        if (storageInfo.access != BufferAccess::READ_WRITE)
            continue;
        auto binding = storageBindings.find(storageInfo.binding);
        if (binding == storageBindings.end() || !binding->second.buffer)
            continue;
        auto bufferImpl = static_cast<BufferImpl*>(binding->second.buffer);
        bufferImpl->setLastFenceValue(_frameFenceValue);
        auto uavHandle = bufferImpl->getUAV();
        if (!uavHandle)
            continue;

        D3D12_CPU_DESCRIPTOR_HANDLE dst = srvCpuStart;
        dst.ptr += (bindingStart + slot) * srvStride;
        _device->CopyDescriptorsSimple(1, dst, uavHandle->cpu, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        ++slot;
    }
    const UINT uavCount = slot - srvCount;

    D3D12_GPU_DESCRIPTOR_HANDLE gpuBase = srvGpuStart;
    gpuBase.ptr += static_cast<UINT64>(bindingStart) * srvStride;

    if (computePipeline->srvRootIndex() != UINT_MAX && srvCount > 0)
        cmd->SetComputeRootDescriptorTable(computePipeline->srvRootIndex(), gpuBase);
    if (computePipeline->uavRootIndex() != UINT_MAX && uavCount > 0)
    {
        D3D12_GPU_DESCRIPTOR_HANDLE uavGpu = gpuBase;
        uavGpu.ptr += static_cast<UINT64>(srvCount) * srvStride;
        cmd->SetComputeRootDescriptorTable(computePipeline->uavRootIndex(), uavGpu);
    }

    // Samplers.
    if (computePipeline->samplerRootIndex() != UINT_MAX)
    {
        const auto samplerGpuStart = _driver->getSamplerHeap()->GetGPUDescriptorHandleForHeapStart();
        cmd->SetComputeRootDescriptorTable(computePipeline->samplerRootIndex(), samplerGpuStart);
    }
    if (computePipeline->customSamplerRootIndex() != UINT_MAX)
    {
        auto* batch = computePipeline->getCustomSamplerBatch(desc.programState);
        AXASSERT(batch, "Failed to resolve D3D12 compute custom sampler descriptors");
        if (batch)
            cmd->SetComputeRootDescriptorTable(computePipeline->customSamplerRootIndex(), batch->gpu);
    }

    // A texture uploaded by the regular texture path starts in pixel-shader
    // read state. Compute sampling needs the non-pixel bit as well, and the
    // combined state remains valid when the same texture is rendered later.
    constexpr D3D12_RESOURCE_STATES textureReadState =
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    for (const auto& [binding, bindingSet] : desc.programState->getTextureBindingSets())
    {
        for (auto* texture : bindingSet.texs)
        {
            if (!texture)
                continue;
            auto* textureImpl = static_cast<TextureImpl*>(texture);
            if (!textureImpl->internalHandle().resource)
                continue;
            textureImpl->setLastFenceValue(_frameFenceValue);
            if (textureImpl->getCurrentState() != textureReadState)
                textureImpl->transitionState(cmd, textureReadState);
        }
    }

    // Transition storage buffers to their bind state.
    tlx::pod_vector<D3D12_RESOURCE_BARRIER> barriers;
    for (const auto& [binding, bindingSet] : desc.programState->getStorageBufferBindingSets())
    {
        if (!bindingSet.buffer)
            continue;
        auto bufferImpl  = static_cast<BufferImpl*>(bindingSet.buffer);
        auto targetState = bindingSet.access == BufferAccess::READ_WRITE
                               ? D3D12_RESOURCE_STATE_UNORDERED_ACCESS
                               : D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        if (bufferImpl->currentState() != targetState)
        {
            D3D12_RESOURCE_BARRIER& bar = barriers.emplace_back();
            bar.Type                    = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            bar.Transition.pResource    = bufferImpl->internalResource();
            bar.Transition.StateBefore  = bufferImpl->currentState();
            bar.Transition.StateAfter   = targetState;
            bar.Transition.Subresource  = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            bufferImpl->_resourceState  = targetState;
        }
    }
    if (!barriers.empty())
        cmd->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

    cmd->Dispatch(desc.groupCountX, desc.groupCountY, desc.groupCountZ);

    // UAV barrier: make compute writes visible to subsequent compute/vertex/fragment reads.
    tlx::pod_vector<D3D12_RESOURCE_BARRIER> postBarriers;
    for (const auto& [binding, bindingSet] : desc.programState->getStorageBufferBindingSets())
    {
        if (!bindingSet.buffer || bindingSet.access != BufferAccess::READ_WRITE)
            continue;
        auto bufferImpl             = static_cast<BufferImpl*>(bindingSet.buffer);
        D3D12_RESOURCE_BARRIER& bar = postBarriers.emplace_back();
        bar.Type                    = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        bar.UAV.pResource           = bufferImpl->internalResource();
    }
    if (!postBarriers.empty())
        cmd->ResourceBarrier(static_cast<UINT>(postBarriers.size()), postBarriers.data());

    _srvOffset[_frameIndex] = bindingStart + srvCount + uavCount;

    // The D3D12 command list does not retain the PSO; keep the pipeline alive
    // until the GPU has finished executing this frame.
    _inFlightComputePipelines[_frameIndex].emplace_back(computePipeline);

    _programState = nullptr;
    return true;
}

bool GraphicsContextImpl::copyTexture(Texture* src, Texture* dst)
{
    if (!validateTextureCopy(src, dst) || !_inFrame || !_currentCmdList)
        return false;

    auto* srcImpl = static_cast<TextureImpl*>(src);
    auto* dstImpl = static_cast<TextureImpl*>(dst);

    auto srcResource = srcImpl->internalHandle().resource.Get();
    if (!srcResource)
        return false;

    if (!dstImpl->internalHandle().resource)
        dstImpl->updateData(nullptr, dst->getWidth(), dst->getHeight(), 0, 0);

    auto dstResource = dstImpl->internalHandle().resource.Get();
    if (!dstResource || srcResource == dstResource)
        return false;

    const auto srcDesc = srcResource->GetDesc();
    const auto dstDesc = dstResource->GetDesc();
    if (srcDesc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D ||
        dstDesc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || srcDesc.Width != dstDesc.Width ||
        srcDesc.Height != dstDesc.Height || srcDesc.Format != dstDesc.Format || srcDesc.SampleDesc.Count != 1 ||
        dstDesc.SampleDesc.Count != 1 || srcDesc.DepthOrArraySize != 1 || dstDesc.DepthOrArraySize != 1 ||
        srcDesc.MipLevels != 1 || dstDesc.MipLevels != 1)
        return false;

    const auto srcState = srcImpl->getCurrentState();
    const auto dstState = dstImpl->getCurrentState();
    srcImpl->transitionState(_currentCmdList, D3D12_RESOURCE_STATE_COPY_SOURCE);
    dstImpl->transitionState(_currentCmdList, D3D12_RESOURCE_STATE_COPY_DEST);

    D3D12_TEXTURE_COPY_LOCATION srcLocation{};
    srcLocation.pResource        = srcResource;
    srcLocation.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    srcLocation.SubresourceIndex = 0;

    D3D12_TEXTURE_COPY_LOCATION dstLocation{};
    dstLocation.pResource        = dstResource;
    dstLocation.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLocation.SubresourceIndex = 0;

    _currentCmdList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);

    srcImpl->transitionState(_currentCmdList, srcState);
    dstImpl->transitionState(_currentCmdList, dstState);
    srcImpl->setLastFenceValue(_frameFenceValue);
    dstImpl->setLastFenceValue(_frameFenceValue);
    return true;
}

bool GraphicsContextImpl::copyTexture(RenderTarget* src, Texture* dst)
{
    if (!src || !dst)
        return false;

    const size_t colorIndex = src->isDefaultRenderTarget() ? _imageIndex : 0;
    if (colorIndex >= src->_color.size() || src->_color[colorIndex].level != 0 || !src->_color[colorIndex].texture)
        return false;

    return copyTexture(src->_color[colorIndex].texture, dst);
}

}  // namespace ax::rhi::d3d12
