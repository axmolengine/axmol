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

#if AX_TARGET_PLATFORM == AX_PLATFORM_WINRT
#    include <windows.ui.xaml.media.dxinterop.h>
#    include <windows.ui.xaml.controls.h>
#    include <windows.ui.core.h>
#    include <windows.foundation.h>
#    include <wrl/event.h>
#    include <wrl/implements.h>
#endif

namespace ax::rhi::d3d12
{

static constexpr DXGI_FORMAT AX_SWAPCHAIN_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

// Helper: map PrimitiveType to D3D12_PRIMITIVE_TOPOLOGY
static D3D12_PRIMITIVE_TOPOLOGY toD3DTopology(PrimitiveType type)
{
    switch (type)
    {
    case PrimitiveType::POINT:
        return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
    case PrimitiveType::LINE:
        return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    case PrimitiveType::LINE_STRIP:
        return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
    case PrimitiveType::TRIANGLE:
        return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case PrimitiveType::TRIANGLE_STRIP:
        return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    case PrimitiveType::LINE_LOOP:  // D3D12 has no native LINE_LOOP
        AXLOGW("D3D12 does not support LINE_LOOP; using LINESTRIP");
        return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
    default:
        return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
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

#if AX_TARGET_PLATFORM == AX_PLATFORM_WINRT

using ICoreDispatcher    = ABI::Windows::UI::Core::ICoreDispatcher;
using IDispatchedHandler = ABI::Windows::UI::Core::IDispatchedHandler;
using IAsyncAction       = ABI::Windows::Foundation::IAsyncAction;
using ISwapChainPanel    = ABI::Windows::UI::Xaml::Controls::ISwapChainPanel;
using IDependencyObject  = ABI::Windows::UI::Xaml::IDependencyObject;
using IUIElement         = ABI::Windows::UI::Xaml::IUIElement;

// Creates a COM/WinRT callback object for the specified interface type (_Ty)
// that is implemented with Free‑Threaded Marshaler (FtmBase) support.
//
// This helper wraps Microsoft::WRL::Callback with an Implements<> type that
// includes FtmBase, making the resulting object agile across threads.
// This is especially useful when passing the handler to APIs like
// ICoreDispatcher::RunAsync, which may invoke the callback on a different thread.
//
// Template parameters:
//   _Ty  - The COM/WinRT interface type to implement (e.g. ABI::Windows::UI::Core::IDispatchedHandler)
//   _Fty - The callable type (lambda, functor, etc.) providing the implementation
//
// Parameters:
//   func - A callable object implementing the interface's Invoke method
//
// Returns:
//   A Microsoft::WRL::ComPtr-compatible callback object implementing _Ty with FTM support.
template <typename _Ty, typename _Fty>
static auto makeFtmHandler(_Fty&& func)
{
    using Impl = Microsoft::WRL::Implements<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, _Ty,
                                            Microsoft::WRL::FtmBase>;
    return Microsoft::WRL::Callback<Impl>(std::forward<_Fty>(func));
}

template <typename _Fty>
static HRESULT runOnUIThread(const ComPtr<ICoreDispatcher>& dispatcher, _Fty&& func)
{
    using namespace ABI::Windows::UI::Core;

    boolean hasThreadAccess = FALSE;
    HRESULT hr              = dispatcher->get_HasThreadAccess(&hasThreadAccess);
    if (FAILED(hr))
        return hr;

    if (hasThreadAccess)
    {
        return func();
    }

    struct AutoHandle
    {
        explicit AutoHandle(HANDLE h) : _h(h) {}
        ~AutoHandle()
        {
            if (_h)
                ::CloseHandle(_h);
        }
        HANDLE get() const { return _h; }
        explicit operator bool() const { return _h != nullptr; }

    private:
        HANDLE _h;
    };

    AutoHandle waitEvent{::CreateEventExW(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS)};
    if (!waitEvent)
        return E_FAIL;

    HRESULT hr2 = E_FAIL;

    auto handler = makeFtmHandler<IDispatchedHandler>([&]() -> HRESULT {
        hr2 = func();
        ::SetEvent(waitEvent.get());
        return S_OK;
    });

    ComPtr<IAsyncAction> asyncAction;
    hr = dispatcher->RunAsync(CoreDispatcherPriority_Normal, handler.Get(), &asyncAction);
    if (FAILED(hr))
        return hr;

    auto waitResult = ::WaitForSingleObjectEx(waitEvent.get(), 10 * 1000, TRUE);
    if (waitResult != WAIT_OBJECT_0)
    {
        std::terminate();
        return E_FAIL;
    }

    return hr2;
}
#endif

RenderContextImpl::RenderContextImpl(DriverImpl* driver, void* surfaceContext) : _driver(driver)
{
    _device        = driver->getDevice();
    _graphicsQueue = driver->getGraphicsQueue();
    auto factory   = driver->getDXGIFactory();

    createCommandObjects();

    // Check tearing support
    BOOL allowTearing = FALSE;
    factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
    _swapchainFlags = allowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    // vsync control
    auto& contextAttrs = Application::getContextAttrs();
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
    HWND hwnd = (HWND)surfaceContext;
    GetClientRect(hwnd, &clientRect);
    _screenWidth  = clientRect.right - clientRect.left;
    _screenHeight = clientRect.bottom - clientRect.top;

    DXGI_SWAP_CHAIN_DESC1 desc1 = {};
    desc1.Width                 = _screenWidth;
    desc1.Height                = _screenHeight;
    desc1.Format                = AX_SWAPCHAIN_FORMAT;
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
        ComPtr<IUnknown> surfaceHold = reinterpret_cast<IUnknown*>(surfaceContext);
        ComPtr<ISwapChainPanel> swapChainPanel;
        hr = surfaceHold.As(&swapChainPanel);
        AX_BREAK_IF(FAILED(hr));

        ComPtr<IDependencyObject> swapChainPanelDependencyObject;
        hr = swapChainPanel.As(&swapChainPanelDependencyObject);
        AX_BREAK_IF(FAILED(hr));

        ComPtr<ICoreDispatcher> dispatcher;
        hr = swapChainPanelDependencyObject->get_Dispatcher(dispatcher.GetAddressOf());
        AX_BREAK_IF(FAILED(hr));

        ComPtr<ISwapChainPanelNative> swapChainPanelNative;
        hr = swapChainPanel.As(&swapChainPanelNative);
        AX_BREAK_IF(FAILED(hr));

        ABI::Windows::Foundation::Size panelSize;
        ComPtr<IUIElement> uiElement;
        hr = swapChainPanel.As(&uiElement);
        AX_BREAK_IF(FAILED(hr));

        Vec2 renderScale;
        hr = runOnUIThread(dispatcher, [&panelSize, &renderScale, uiElement, swapChainPanel] {
            HRESULT hr1 = uiElement->get_RenderSize(&panelSize);
            if (FAILED(hr1))
                return hr1;
            hr1 = swapChainPanel->get_CompositionScaleX(&renderScale.x);
            if (FAILED(hr1))
                return hr1;
            hr1 = swapChainPanel->get_CompositionScaleY(&renderScale.y);
            return hr1;
        });
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
        desc1.Format           = AX_SWAPCHAIN_FORMAT;
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

        hr = runOnUIThread(dispatcher, [swapChainPanelNative, swapchain1] {
            return swapChainPanelNative->SetSwapChain(swapchain1.Get());
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
    _screenRT = _driver->createDefaultRenderTarget();

    // Build swapchain attachments for screen RT
    static_cast<RenderTargetImpl*>(_screenRT)->rebuildAttachmentsForSwapchain(_swapchain.Get(), _screenWidth,
                                                                              _screenHeight);

    createUniformRingBuffers(1 * 1024 * 1024);  // 1 MB per frame
    _descriptorHeaps.reserve(16);

    bitmask::set(_inFlightDynamicDirtyBits[0], PIPELINE_REQUIRED_DYNAMIC_BITS);
    bitmask::set(_inFlightDynamicDirtyBits[1], PIPELINE_REQUIRED_DYNAMIC_BITS);
}

RenderContextImpl::~RenderContextImpl()
{
    _driver->waitDeviceIdle();

    // Ensure GPU idle then cleanup handles if needed
    if (_graphicsQueue)
    {
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            if (_fences[i])
            {
                _graphicsQueue->Signal(_fences[i].Get(), ++_fenceValues[i]);
                if (_fences[i]->GetCompletedValue() < _fenceValues[i])
                {
                    _fences[i]->SetEventOnCompletion(_fenceValues[i], _fenceEvents[i]);
                    WaitForSingleObject(_fenceEvents[i], INFINITE);
                }
            }
        }
    }

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        if (_fenceEvents[i])
        {
            CloseHandle(_fenceEvents[i]);
            _fenceEvents[i] = nullptr;
        }
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
        hr = _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fences[i]));
        AXASSERT(SUCCEEDED(hr), "CreateFence failed");
        _fenceValues[i] = 0;
        _fenceEvents[i] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        AXASSERT(_fenceEvents[i] != nullptr, "CreateEvent failed");
    }
}

bool RenderContextImpl::updateSurface(void* /*surface*/, uint32_t width, uint32_t height)
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
    _renderPipeline = static_cast<RenderPipelineImpl*>(renderPipeline);
}

bool RenderContextImpl::beginFrame()
{
    // Wait fence of current frame
    auto fence = _fences[_currentFrame];
    if (fence->GetCompletedValue() < _fenceValues[_currentFrame])
    {
        fence->SetEventOnCompletion(_fenceValues[_currentFrame], _fenceEvents[_currentFrame]);
        WaitForSingleObject(_fenceEvents[_currentFrame], INFINITE);
    }

    _driver->processDisposalQueue(1 << _currentFrame);

    if (_swapchainDirty)
    {
        auto rtImpl = static_cast<RenderTargetImpl*>(_screenRT);

        // Release references to the swapchain back buffer for we can ResizeBuffers
        rtImpl->invalidate();

        _driver->cleanPendingResources();

        _swapchain->ResizeBuffers(SWAPCHAIN_BUFFER_COUNT, _screenWidth, _screenHeight, AX_SWAPCHAIN_FORMAT,
                                  _swapchainFlags);
        rtImpl->rebuildAttachmentsForSwapchain(_swapchain.Get(), _screenWidth, _screenHeight);

        _swapchainDirty = false;
    }

    resetUniformRingForCurrentFrame(_currentFrame);

    _currentImageIndex = _swapchain->GetCurrentBackBufferIndex();

    // Reset allocator and command list
    HRESULT hr = _commandAllocators[_currentFrame]->Reset();
    AXASSERT(SUCCEEDED(hr), "CommandAllocator Reset failed");
    _currentCmdList = _commandLists[_currentFrame].Get();
    hr              = _currentCmdList->Reset(_commandAllocators[_currentFrame].Get(), nullptr);
    AXASSERT(SUCCEEDED(hr), "CommandList Reset failed");

    _inFrame = true;
    return true;
}

void RenderContextImpl::beginRenderPass(RenderTarget* renderTarget, const RenderPassDesc& descriptor)
{
    AXASSERT(_inFrame, "beginRenderPass must be called within a frame");
    auto rtImpl = static_cast<RenderTargetImpl*>(renderTarget);
    _currentRT  = renderTarget;

    // Get target size from color0
    auto colorAttachment = rtImpl->getColorAttachment(0);
    _renderTargetWidth   = colorAttachment->getDesc().width;
    _renderTargetHeight  = colorAttachment->getDesc().height;

    // Bind RTV/DSV and clear according to flags
    rtImpl->beginRenderPass(_currentCmdList, descriptor, _renderTargetWidth, _renderTargetHeight, _currentImageIndex);
}

void RenderContextImpl::endRenderPass()
{
    // Reset cached state objects
    static_cast<RenderTargetImpl*>(_currentRT)->endRenderPass(_currentCmdList, _currentImageIndex);

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
    AXASSERT(SUCCEEDED(hr), "SwapChain Present failed");

#ifdef NDEBUG
    (void)hr;
#else
    if (FAILED(hr))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            auto device    = static_cast<DriverImpl*>(DriverBase::getInstance())->getDevice();
            HRESULT reason = device->GetDeviceRemovedReason();
            AXLOGD("D3D12 Device remove reason: {}", reason);
        }
        // else if (hr == DXGI_ERROR_WAS_STILL_DRAWING)
        //{
        // }
    }
#endif

    // Signal fence for this frame
    _fenceValues[_currentFrame]++;
    _graphicsQueue->Signal(_fences[_currentFrame].Get(), _fenceValues[_currentFrame]);

    // Next frame index
    _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    _inFrame      = false;
}

void RenderContextImpl::setViewport(int x, int y, unsigned int w, unsigned int h)
{
    if (w == 0 || h == 0)
        return;

    D3D12_VIEWPORT vp{};
    vp.TopLeftX = static_cast<float>(x);
    vp.TopLeftY = static_cast<float>(y);
    vp.Width    = static_cast<float>(w);
    vp.Height   = static_cast<float>(h);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;

    // Avoid redundant state if equal
    const bool same = vp.TopLeftX == _cachedViewport.TopLeftX && vp.TopLeftY == _cachedViewport.TopLeftY &&
                      vp.Width == _cachedViewport.Width && vp.Height == _cachedViewport.Height &&
                      vp.MinDepth == _cachedViewport.MinDepth && vp.MaxDepth == _cachedViewport.MaxDepth;

    if (!same)
    {
        _cachedViewport = vp;
        markDynamicStateDirty(DynamicStateBits::Viewport);
    }
}

void RenderContextImpl::setScissorRect(bool isEnabled, float x, float y, float width, float height)
{
    D3D12_RECT rect{};
    if (isEnabled)
    {
        const LONG minX = static_cast<LONG>(std::max(0.f, x));
        const LONG minY = static_cast<LONG>(std::max(0.f, y));
        const LONG maxX = static_cast<LONG>(std::min<float>(x + width, static_cast<float>(_renderTargetWidth)));
        const LONG maxY = static_cast<LONG>(std::min<float>(y + height, static_cast<float>(_renderTargetHeight)));

        rect.left   = minX;
        rect.top    = minY;
        rect.right  = std::max<LONG>(minX, maxX);
        rect.bottom = std::max<LONG>(minY, maxY);
    }
    else
    {
        rect.left   = 0;
        rect.top    = 0;
        rect.right  = static_cast<LONG>(_renderTargetWidth);
        rect.bottom = static_cast<LONG>(_renderTargetHeight);
    }

    const bool changed = rect.left != _cachedScissor.left || rect.top != _cachedScissor.top ||
                         rect.right != _cachedScissor.right || rect.bottom != _cachedScissor.bottom;

    if (changed)
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
    if (bitmask::any(_inFlightDynamicDirtyBits[_currentFrame], DynamicStateBits::StencilRef))
    {
        _currentCmdList->OMSetStencilRef(_stencilReferenceValue);
        bitmask::clear(_inFlightDynamicDirtyBits[_currentFrame], DynamicStateBits::StencilRef);
    }

    if (bitmask::any(_inFlightDynamicDirtyBits[_currentFrame], DynamicStateBits::Viewport))
    {
        _currentCmdList->RSSetViewports(1, &_cachedViewport);
        bitmask::clear(_inFlightDynamicDirtyBits[_currentFrame], DynamicStateBits::Viewport);
    }

    if (bitmask::any(_inFlightDynamicDirtyBits[_currentFrame], DynamicStateBits::Scissor))
    {
        _currentCmdList->RSSetScissorRects(1, &_cachedScissor);
        bitmask::clear(_inFlightDynamicDirtyBits[_currentFrame], DynamicStateBits::Scissor);
    }
}

void RenderContextImpl::updateDepthStencilState(const DepthStencilDesc& desc)
{
    AXASSERT(_depthStencilState, "DepthStencilStateImpl not set");
    _depthStencilState->update(desc);
}

void RenderContextImpl::updatePipelineState(const RenderTarget* rt, const PipelineDesc& descriptor)
{
    RenderContext::updatePipelineState(rt, descriptor);
    AXASSERT(_renderPipeline, "RenderPipelineImpl not set");
    _renderPipeline->prepareUpdate(_depthStencilState, _cachedCullMode, _cachedFrontCounterClockwise);
    _renderPipeline->update(rt, descriptor);

    // Bind PSO & RootSignature
    auto pso = _renderPipeline->getPipelineState();
    // if (pso != _currentPSO)
    {
        auto rootSigInfo = _renderPipeline->getRootSignature();

        _currentCmdList->SetGraphicsRootSignature(rootSigInfo->rootSig.Get());
        _currentCmdList->SetPipelineState(_renderPipeline->getPipelineState());
        _currentPSO = pso;

        bitmask::set(_inFlightDynamicDirtyBits[_currentFrame], PIPELINE_REQUIRED_DYNAMIC_BITS);
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

void RenderContextImpl::drawArrays(PrimitiveType primitiveType,
                                   std::size_t start,
                                   std::size_t count,
                                   bool /*wireframe*/)
{
    AXASSERT(_renderPipeline && _vertexBuffer, "Pipeline and vertex buffer must be set");

    prepareDrawing(_currentCmdList);

    // Set primitive topology
    _currentCmdList->IASetPrimitiveTopology(toD3DTopology(primitiveType));
    // Draw
    _currentCmdList->DrawInstanced(static_cast<UINT>(count), 1, static_cast<UINT>(start), 0);
}

void RenderContextImpl::drawArraysInstanced(PrimitiveType primitiveType,
                                            std::size_t start,
                                            std::size_t count,
                                            int instanceCount,
                                            bool /*wireframe*/)
{
    AXASSERT(_renderPipeline && _vertexBuffer, "Pipeline and vertex buffer must be set");

    prepareDrawing(_currentCmdList);

    _currentCmdList->IASetPrimitiveTopology(toD3DTopology(primitiveType));

    _currentCmdList->DrawInstanced(static_cast<UINT>(count), static_cast<UINT>(instanceCount), static_cast<UINT>(start),
                                   0);
}

void RenderContextImpl::drawElements(PrimitiveType primitiveType,
                                     IndexFormat indexType,
                                     std::size_t count,
                                     std::size_t offset,
                                     bool /*wireframe*/)
{
    AXASSERT(_renderPipeline && _vertexBuffer && _indexBuffer, "Pipeline, vertex and index buffers must be set");

    _currentCmdList->IASetPrimitiveTopology(toD3DTopology(primitiveType));

    prepareDrawing(_currentCmdList);

    // IB
    D3D12_INDEX_BUFFER_VIEW ibv{};
    ibv.BufferLocation = _indexBuffer->internalResource()->GetGPUVirtualAddress() + static_cast<UINT64>(offset);
    ibv.SizeInBytes    = static_cast<UINT>(_indexBuffer->getSize() - offset);
    ibv.Format         = toDxgiIndexFormat(indexType);
    _currentCmdList->IASetIndexBuffer(&ibv);

    _currentCmdList->DrawIndexedInstanced(static_cast<UINT>(count), 1, 0, 0, 0);
}

void RenderContextImpl::drawElementsInstanced(PrimitiveType primitiveType,
                                              IndexFormat indexType,
                                              std::size_t count,
                                              std::size_t offset,
                                              int instanceCount,
                                              bool /*wireframe*/)
{
    AXASSERT(_renderPipeline && _vertexBuffer && _indexBuffer, "Pipeline, vertex and index buffers must be set");

    prepareDrawing(_currentCmdList);

    _currentCmdList->IASetPrimitiveTopology(toD3DTopology(primitiveType));

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

    // bind vertex buffers
    if (!_instanceBuffer)
    {
        D3D12_VERTEX_BUFFER_VIEW vbv{};
        vbv.BufferLocation = _vertexBuffer->internalResource()->GetGPUVirtualAddress();
        vbv.SizeInBytes    = static_cast<UINT>(_vertexBuffer->getSize());
        vbv.StrideInBytes  = static_cast<UINT>(_vertexLayout->getStride());
        cmd->IASetVertexBuffers(0, 1, &vbv);
    }
    else
    {
        D3D12_VERTEX_BUFFER_VIEW views[2]{};
        views[0].BufferLocation = _vertexBuffer->internalResource()->GetGPUVirtualAddress();
        views[0].SizeInBytes    = static_cast<UINT>(_vertexBuffer->getSize());
        views[0].StrideInBytes  = static_cast<UINT>(_vertexLayout->getStride());

        views[1].BufferLocation = _instanceBuffer->internalResource()->GetGPUVirtualAddress();
        views[1].SizeInBytes    = static_cast<UINT>(_instanceBuffer->getSize());
        views[1].StrideInBytes  = static_cast<UINT>(_vertexLayout->getInstanceStride());

        cmd->IASetVertexBuffers(0, 2, views);
    }

    // bind ubos
    // VS UBO
    auto vsData = _programState->getVertexUniformBuffer();
    if (!vsData.empty())
    {
        auto s = allocateUniformSlice(_currentFrame, vsData.size());
        std::memcpy(s.cpuPtr, vsData.data(), vsData.size());
        cmd->SetGraphicsRootConstantBufferView(rootSigInfo->vsUboRootIndex, s.gpuVA);
    }

    // FS UBO
    auto fsData = _programState->getFragmentUniformBuffer();
    if (!fsData.empty())
    {
        auto s = allocateUniformSlice(_currentFrame, fsData.size());
        std::memcpy(s.cpuPtr, fsData.data(), fsData.size());
        cmd->SetGraphicsRootConstantBufferView(rootSigInfo->fsUboRootIndex, s.gpuVA);
    }

    // --- bind textures & samplers ---
    _descriptorHeaps.clear();
    bool heapSet = false;
    for (const auto& [bindingIndex, bindingSet] : _programState->getTextureBindingSets())
    {
        const auto& texs = bindingSet.texs;
        if (texs.empty())
            continue;

        if (texs.size() == 1)
        {
            auto textureImpl = static_cast<TextureImpl*>(texs[0]);

            // SRV handle (already created in TextureImpl::ensureNativeTexture)
            const DescriptorHandle* srvHandle = textureImpl->internalHandle().srv;
            // Sampler handle (already created and returned by getSampler())
            const DescriptorHandle* samplerHandle = textureImpl->getSampler();

            if (!heapSet)
            {
                _descriptorHeaps.push_back(_driver->getSRVHeap(srvHandle));
                _descriptorHeaps.push_back(_driver->getSamplerHeap(samplerHandle));
                cmd->SetDescriptorHeaps(_descriptorHeaps.size(), _descriptorHeaps.data());
            }

            // Bind SRV descriptor table (RootParameter index for SRV table)
            cmd->SetGraphicsRootDescriptorTable(rootSigInfo->srvRootIndex, srvHandle->gpu);

            // Bind Sampler descriptor table (RootParameter index for Sampler table)
            cmd->SetGraphicsRootDescriptorTable(rootSigInfo->samplerRootIndex, samplerHandle->gpu);
        }
        else
        {  // TODO:
        }
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
                                   bool /*preserveAxisHint*/,
                                   std::function<void(const PixelBufferDesc&)> callback)
{
    AX_SAFE_RETAIN(rt);

    // Simplified: readback from swapchain backbuffer color0 by copying to readback resource
    PixelBufferDesc pbd{};
    auto* rtImpl = static_cast<RenderTargetImpl*>(rt);

    auto colorAttachment = rtImpl->getColorAttachment(0);
    if (!colorAttachment)
    {
        callback(pbd);
        AX_SAFE_RELEASE(rt);
        return;
    }

    // Ensure GPU work finished
    auto fence = _fences[_currentFrame];
    if (fence->GetCompletedValue() < _fenceValues[_currentFrame])
    {
        fence->SetEventOnCompletion(_fenceValues[_currentFrame], _fenceEvents[_currentFrame]);
        WaitForSingleObject(_fenceEvents[_currentFrame], INFINITE);
    }

    const auto& desc  = colorAttachment->getDesc();
    const UINT width  = desc.width;
    const UINT height = desc.height;

    // Create readback buffer (ROW_MAJOR)
    const UINT stride   = 4;  // RGBA8
    const UINT rowPitch = width * stride;
    const UINT bufSize  = rowPitch * height;

    Microsoft::WRL::ComPtr<ID3D12Resource> readback;
    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_READBACK;

    D3D12_RESOURCE_DESC bufDesc{};
    bufDesc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufDesc.Width            = bufSize;
    bufDesc.Height           = 1;
    bufDesc.DepthOrArraySize = 1;
    bufDesc.MipLevels        = 1;
    bufDesc.Format           = DXGI_FORMAT_UNKNOWN;
    bufDesc.SampleDesc.Count = 1;
    bufDesc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    HRESULT hr = _device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufDesc,
                                                  D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&readback));
    AXASSERT(SUCCEEDED(hr), "CreateCommittedResource READBACK failed");

    // Record copy from texture to readback via CopyTextureRegion
    auto cmd = _currentCmdList;
    // Ensure list is open; if not, open a tiny list (simplified assumption: we are between frames)
    // Transition source to COPY_SOURCE if needed
    colorAttachment->transitionState(cmd, D3D12_RESOURCE_STATE_COPY_SOURCE);

    D3D12_TEXTURE_COPY_LOCATION dst{};
    dst.pResource            = readback.Get();
    dst.Type                 = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    const auto nativeTexDesc = colorAttachment->internalHandle().resource->GetDesc();
    _device->GetCopyableFootprints(&nativeTexDesc, 0, 1, 0, &dst.PlacedFootprint, nullptr, nullptr, nullptr);

    D3D12_TEXTURE_COPY_LOCATION src{};
    src.pResource        = colorAttachment->internalHandle().resource.Get();
    src.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    src.SubresourceIndex = 0;

    cmd->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

    // Transition source back to sampling
    colorAttachment->transitionState(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    // Close and execute a small copy list if needed
    hr = cmd->Close();
    AXASSERT(SUCCEEDED(hr), "CommandList Close (readPixels) failed");
    ID3D12CommandList* lists[] = {cmd};
    _graphicsQueue->ExecuteCommandLists(1, lists);

    // Wait for copy completion
    _fenceValues[_currentFrame]++;
    _graphicsQueue->Signal(_fences[_currentFrame].Get(), _fenceValues[_currentFrame]);
    _fences[_currentFrame]->SetEventOnCompletion(_fenceValues[_currentFrame], _fenceEvents[_currentFrame]);
    WaitForSingleObject(_fenceEvents[_currentFrame], INFINITE);

    // Map and read data
    void* mapped = nullptr;
    D3D12_RANGE readRange{0, bufSize};
    readback->Map(0, &readRange, &mapped);

    pbd._width  = width;
    pbd._height = height;
    pbd._data.resize(bufSize);
    std::memcpy(pbd._data.data(), mapped, bufSize);

    D3D12_RANGE written{0, 0};
    readback->Unmap(0, &written);

    callback(pbd);
    AX_SAFE_RELEASE(rt);

    // Re-open command list for the frame (since we closed it)
    _commandAllocators[_currentFrame]->Reset();
    _currentCmdList->Reset(_commandAllocators[_currentFrame].Get(), nullptr);
}

}  // namespace ax::rhi::d3d12
