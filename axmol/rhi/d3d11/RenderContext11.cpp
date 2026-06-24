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
#include "axmol/rhi/d3d11/RenderContext11.h"
#include "axmol/rhi/d3d11/RenderTarget11.h"
#include "axmol/rhi/d3d11/RenderPipeline11.h"
#include "axmol/rhi/d3d11/DepthStencilState11.h"
#include "axmol/rhi/d3d11/Buffer11.h"
#include "axmol/rhi/d3d11/Program11.h"
#include "axmol/rhi/d3d11/VertexLayout11.h"
#include "axmol/rhi/d3d11/Texture11.h"
#include "axmol/rhi/DriverContext.h"
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_5.h>
#include <VersionHelpers.h>
#include "axmol/base/Logging.h"
#include "axmol/platform/Application.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_WINRT
#    include "axmol/platform/winrt/SwapChainPanelUtil.h"
#endif

namespace ax::rhi::d3d11
{
static D3D11_PRIMITIVE_TOPOLOGY toD3DPrimitiveTopology(PrimitiveType type)
{
    switch (type)
    {
    case PrimitiveType::POINT:
        return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    case PrimitiveType::LINE:
        return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    case PrimitiveType::LINE_LOOP:
        return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;  // FIXME: LINE_LOOP
    case PrimitiveType::LINE_STRIP:
        return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
    case PrimitiveType::TRIANGLE:
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case PrimitiveType::TRIANGLE_STRIP:
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    default:
        return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
    }
}

static DXGI_FORMAT toDXGIFormat(IndexFormat format)
{
    switch (format)
    {
    case IndexFormat::U_SHORT:
        return DXGI_FORMAT_R16_UINT;
    case IndexFormat::U_INT:
        return DXGI_FORMAT_R32_UINT;
    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}

typedef LONG(WINAPI* PFN_RtlVerifyVersionInfo)(OSVERSIONINFOEXW*, ULONG, ULONGLONG);

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
static BOOL _axmolIsWindowsVersionOrGreaterWin32(WORD major, WORD minor, WORD build)
{
    OSVERSIONINFOEXW osvi = {sizeof(osvi), major, minor, build};
    DWORD mask            = VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER;
    ULONGLONG cond        = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
    cond                  = VerSetConditionMask(cond, VER_MINORVERSION, VER_GREATER_EQUAL);
    cond                  = VerSetConditionMask(cond, VER_BUILDNUMBER, VER_GREATER_EQUAL);
    // HACK: Use RtlVerifyVersionInfo instead of VerifyVersionInfoW as the
    //       latter lies unless the user knew to embed a non-default manifest
    //       announcing support for Windows 10 via supportedOS GUID

    static PFN_RtlVerifyVersionInfo RtlVerifyVersionInfo =
        (PFN_RtlVerifyVersionInfo)GetProcAddress(GetModuleHandleW(L"ntdll"), "RtlVerifyVersionInfo");
    return RtlVerifyVersionInfo(&osvi, mask, cond) == 0;
}

#    define axmolIsWindows10OrGreater() \
        _axmolIsWindowsVersionOrGreaterWin32(HIBYTE(_WIN32_WINNT_WIN10), LOBYTE(_WIN32_WINNT_WIN10), 0)
#endif

RenderContextImpl::RenderContextImpl(DriverImpl* driver, SurfaceHandle surface)
{
    _driver       = driver;
    _d3d11Context = driver->getContext();

    auto& contextAttrs = Application::getContextAttrs();
    _renderScaleMode   = contextAttrs.renderScaleMode;

    auto context         = driver->getContext();
    ID3D11Device* device = driver->getDevice();

    auto& factory  = driver->getDXGIFactory();
    auto& factory2 = driver->getDXGIFactory2();
    auto& adapter  = driver->getDXGIAdapter();

    ComPtr<IDXGISwapChain> swapChain;
    HRESULT hr = 0;
#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    RECT clientRect;
    auto hwnd = static_cast<HWND>(surface);
    GetClientRect(hwnd, &clientRect);
    _screenWidth  = clientRect.right - clientRect.left;
    _screenHeight = clientRect.bottom - clientRect.top;
#endif

    // Try create swapchain, we prefer DXGI 1.2 but support fallback to DXGI 1.1 for win32 apps
    do
    {
        AX_BREAK_IF(!factory2);

        // Check allow tearing feature support and set swapchain flags
        ComPtr<IDXGIFactory5> factory5;
        if (SUCCEEDED(factory.As(&factory5)))
            factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &_allowTearing, sizeof(_allowTearing));

        // Axmol-v3 D3D11 doesn't support change vsync runtime, so don't enable tearing if vsync is on.
        _swapChainFlags = _allowTearing && !contextAttrs.vsync ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
        DXGI_SWAP_CHAIN_DESC1 desc1 = {};
        desc1.Width                 = _screenWidth;
        desc1.Height                = _screenHeight;
        desc1.Format                = DEFAULT_SWAPCHAIN_FORMAT;
        desc1.SampleDesc.Count      = 1;  // Flip not support MSAA
        desc1.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc1.BufferCount           = 2;
        desc1.Flags                 = _swapChainFlags;

        // Choose swapchain effect by OS version
        // DXGI_SWAP_EFFECT_FLIP_DISCARD:
        //   Best performance, recommended on Windows 10 and later.
        //   Officially supported starting with Windows 10.
        // DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL:
        //   Official documentation states this is supported beginning with Windows 8.
        //   However, Windows 7 SP1 with the Platform Update (KB2670838) silently
        //   upgrades DXGI to version 1.2. This update is not explicitly mentioned
        //   in the API docs, but it brings hidden support for Flip Sequential.
        //   Evidence: dxgi.dll version changes from 6.1.x (Win7 base) to 6.2.x,
        //   and IDXGIFactory2 becomes available. Without the Platform Update,
        //   attempting to create a Flip Sequential swapchain on Win7 will fail
        //   with DXGI_ERROR_INVALID_CALL.
        // DXGI_SWAP_EFFECT_SEQUENTIAL / DISCARD (Blt model):
        //   The only swap effects available on vanilla Windows 7 (no Platform Update).
        //   Use these as a fallback when Flip modes are not supported.
        // refer to: https://learn.microsoft.com/en-us/windows/win32/api/dxgi/ne-dxgi-dxgi_swap_effect
        desc1.SwapEffect =
            axmolIsWindows10OrGreater() ? DXGI_SWAP_EFFECT_FLIP_DISCARD : DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc = {};
        fsDesc.Windowed                        = TRUE;

        ComPtr<IDXGISwapChain1> swapChain1;
        hr = factory2->CreateSwapChainForHwnd(device, hwnd, &desc1, &fsDesc, nullptr, &swapChain1);
        if (SUCCEEDED(hr))
        {
            factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
            swapChain1.As(&swapChain);
        }
#elif AX_TARGET_PLATFORM == AX_PLATFORM_WINUWP
        // Wrap the raw COM pointer into a C++/WinRT runtime class
        winrt::SwapChainPanel swapChainPanel{nullptr};
        winrt::copy_from_abi(swapChainPanel, surface.ptr);

        auto dispatcher = swapChainPanel.Dispatcher();
        winrt::Size panelSize;
        winrt::Vector2 renderScale;

        hr = winrt::GetSwapChainPanelRenderMetrics(swapChainPanel, dispatcher, panelSize, renderScale);
        AX_BREAK_IF(FAILED(hr));

        // create swapchain
        // The swapchain size can't be zero for WinRT, maybe SwapChainPanel::ActualWidth/Height * DPI
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
        desc1.SampleDesc.Count = 1;  // Flip not support MSAA
        desc1.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc1.BufferCount      = 2;

        desc1.Scaling    = DXGI_SCALING_STRETCH;
        desc1.AlphaMode  = DXGI_ALPHA_MODE_IGNORE;
        desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc1.Flags      = _swapChainFlags;

        ComPtr<IDXGISwapChain1> swapChain1;
        hr = factory2->CreateSwapChainForComposition(device, &desc1, nullptr, &swapChain1);
        AX_BREAK_IF(FAILED(hr));
        swapChain1.As(&swapChain);

        hr = winrt::RunOnUIThreadSync(dispatcher, [&swapChainPanel, swapChain1] {
            auto hr0 = swapChainPanel.as<ISwapChainPanelNative>()->SetSwapChain(swapChain1.Get());
            if (FAILED(hr0))
                throw winrt::hresult_error(hr0);
        });

        AX_BREAK_IF(FAILED(hr));

        DXGI_SWAP_CHAIN_DESC1 actualDesc = {};
        swapChain1->GetDesc1(&actualDesc);

        if (_renderScaleMode == RenderScaleMode::Physical)
        {
            // Setup a scale matrix for the swap chain
            DXGI_MATRIX_3X2_F scaleMatrix = {};
            scaleMatrix._11               = 1 / renderScale.x;
            scaleMatrix._22               = 1 / renderScale.y;

            ComPtr<IDXGISwapChain2> swapChain2;
            hr = swapChain1.As(&swapChain2);
            AX_BREAK_IF(FAILED(hr));

            hr = swapChain2->SetMatrixTransform(&scaleMatrix);
            AX_BREAK_IF(FAILED(hr));
        }

        _screenWidth  = actualDesc.Width;
        _screenHeight = actualDesc.Height;
#endif
    } while (false);

    // control vsync
    if (contextAttrs.vsync)
    {
        _syncInterval = 1;
        _presentFlags = 0;
    }
    else
    {
        _syncInterval = 0;
        _presentFlags = DXGI_PRESENT_DO_NOT_WAIT;
        if (_allowTearing)
            _presentFlags |= DXGI_PRESENT_ALLOW_TEARING;
    }

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    // Fallback to DXGI 1.1 blt mode if flip mode fail
    if (!swapChain)
    {
        // Note: Windows 7 RTM/SP1 without Platform Update doesn't support Flip mode, but it does support blt mode. So
        // we can fallback to blt mode for better compatibility.
        AXLOGW("CreateSwapChain: Fallback to DXGI 1.1 blt mode (vsync disabled to avoid frame stall)");

        // Force disable vsync on DXGI 1.1 (Win7 RTM/SP1) to avoid severe frame stall
        _syncInterval = 0;

        DXGI_SWAP_CHAIN_DESC scDesc               = {};
        scDesc.BufferCount                        = 2;
        scDesc.BufferDesc.Width                   = _screenWidth;
        scDesc.BufferDesc.Height                  = _screenHeight;
        scDesc.BufferDesc.Format                  = DEFAULT_SWAPCHAIN_FORMAT;
        scDesc.BufferDesc.RefreshRate.Numerator   = 0;
        scDesc.BufferDesc.RefreshRate.Denominator = 1;
        scDesc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scDesc.OutputWindow                       = hwnd;
        scDesc.SampleDesc.Count                   = 1;
        scDesc.SampleDesc.Quality                 = 0;
        scDesc.Windowed                           = TRUE;
        scDesc.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
        scDesc.Flags                              = 0;

        hr = factory->CreateSwapChain(device, &scDesc, &swapChain);
    }
#endif

    if (FAILED(hr))
        dxutils::fatalError("CreateSwapChain", hr);

    // Final swapchain setup
    _swapChain = swapChain.Detach();
    _screenRT  = new RenderTargetImpl(_driver, true);
    _screenRT->rebuildSwapchainBuffers(_swapChain, _screenWidth, _screenHeight);
    _nullSRVs.reserve(8);
}

RenderContextImpl::~RenderContextImpl()
{
    // cleanup GPU resources
    _d3d11Context->OMSetRenderTargets(0, nullptr, nullptr);

    AX_SAFE_RELEASE_NULL(_screenRT);
    AX_SAFE_RELEASE_NULL(_renderPipeline);

    SafeRelease(_swapChain);
    _rasterStateCache.clear();
}

bool RenderContextImpl::updateSurface(SurfaceHandle /*surface*/, uint32_t width, uint32_t height)
{
    if (!_swapChain || !_driver || !_screenRT)
        return false;

    // Since the window size can be zero when minimized, delay rebuilding until it returns to normal state
    if (width == 0 || height == 0)
    {
        return true;
    }

    if (width == _screenWidth && height == _screenHeight)
        return true;

    // Resize swapchain buffers
    if (!_screenRT->rebuildSwapchainBuffers(_swapChain, width, height, _swapChainFlags))
        return false;

    _screenWidth  = width;
    _screenHeight = height;

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

bool RenderContextImpl::beginFrame()
{
    return true;
}

void RenderContextImpl::beginRenderPass(RenderTarget* renderTarget, const RenderPassDesc& renderPassDesc)
{
    auto activeRT = static_cast<RenderTargetImpl*>(renderTarget);
    if (_renderPassDesc == renderPassDesc && _currentRT == activeRT && !activeRT->isDirty())
    {
        ;
    }
    else
    {

        _currentRT      = activeRT;
        _renderPassDesc = renderPassDesc;
    }

    activeRT->beginRenderPass(_d3d11Context);

    auto colorAttachment = activeRT->getColorAttachment(0);
    _renderTargetWidth   = colorAttachment.desc.width;
    _renderTargetHeight  = colorAttachment.desc.height;

    auto clearFlags = renderPassDesc.flags.clear;

    // clear color
    if (bitmask::any(clearFlags, TargetBufferFlags::COLOR))
        _d3d11Context->ClearRenderTargetView(activeRT->getRTV(0), renderPassDesc.clearColorValue.data());

    // clear depth & stencil
    if (bitmask::any(clearFlags, TargetBufferFlags::DEPTH_AND_STENCIL) && activeRT->getDSV())
        _d3d11Context->ClearDepthStencilView(activeRT->getDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                                             renderPassDesc.clearDepthValue,
                                             static_cast<UINT8>(renderPassDesc.clearStencilValue));
}

void RenderContextImpl::updateDepthStencilState(const DepthStencilDesc& desc)
{
    _depthStencilState->update(desc);
}

void RenderContextImpl::updatePipelineState(const RenderTarget* rt,
                                            const PipelineDesc& desc,
                                            PrimitiveType primitiveType)
{
    RenderContext::updatePipelineState(rt, desc, primitiveType);
    _renderPipeline->update(rt, desc);

    _d3d11Context->IASetPrimitiveTopology(toD3DPrimitiveTopology(primitiveType));
}

void RenderContextImpl::setCullMode(CullMode mode)
{
    if (_rasterDesc.cullMode != mode)
    {
        _rasterDesc.cullMode = mode;
        _dirtyStateFlags |= RenderStateFlag::RasterDesc;
    }
}

void RenderContextImpl::setWinding(Winding winding)
{
    if (_rasterDesc.winding != winding)
    {
        _rasterDesc.winding = winding;
        _dirtyStateFlags |= RenderStateFlag::RasterDesc;
    }
}

void RenderContextImpl::setViewport(int x, int y, unsigned int w, unsigned int h)
{
    D3D11_VIEWPORT vp = {.MinDepth = 0.0f, .MaxDepth = 1.0f};
    vp.TopLeftX       = static_cast<FLOAT>(x);
    vp.TopLeftY       = static_cast<FLOAT>(y);
    vp.Width          = static_cast<FLOAT>(w);
    vp.Height         = static_cast<FLOAT>(h);
    if (!dxutils::viewportsEqual(_viewport, vp))
    {
        _viewport = vp;
        _dirtyStateFlags |= RenderStateFlag::Viewport;
    }
}

void RenderContextImpl::setScissorRect(bool enabled, float x, float y, float width, float height)
{
    D3D11_RECT rect{};

    if (enabled)
    {
        const float rtW = static_cast<float>(_renderTargetWidth);
        const float rtH = static_cast<float>(_renderTargetHeight);
        y               = rtH - height - y;

        const int minX = static_cast<int>(std::clamp(x, 0.f, rtW));
        const int minY = static_cast<int>(std::clamp(y, 0.f, rtH));
        const int maxX = static_cast<int>(std::clamp(x + width, 0.f, rtW));
        const int maxY = static_cast<int>(std::clamp(y + height, 0.f, rtH));

        rect.left   = minX;
        rect.top    = minY;
        rect.right  = maxX;
        rect.bottom = maxY;

        if (rect.right < rect.left)
            rect.right = rect.left;
        if (rect.bottom < rect.top)
            rect.bottom = rect.top;
    }
    else
    {
        rect.left   = 0;
        rect.top    = 0;
        rect.right  = _renderTargetWidth;
        rect.bottom = _renderTargetHeight;
    }

    if (_rasterDesc.scissorEnable != enabled)
    {
        _rasterDesc.scissorEnable = enabled;
        _dirtyStateFlags |= RenderStateFlag::RasterDesc;
    }

    if (!dxutils::rectsEqual(_scissorRect, rect))
    {
        _scissorRect = rect;
        _dirtyStateFlags |= RenderStateFlag::ScissorRect;
    }
}

void RenderContextImpl::applyRenderStates()
{
    if (bitmask::any(RenderStateFlag::Viewport, _dirtyStateFlags))
    {
        _d3d11Context->RSSetViewports(1, &_viewport);
        _dirtyStateFlags &= ~RenderStateFlag::Viewport;
    }

    if (bitmask::any(RenderStateFlag::ScissorRect, _dirtyStateFlags))
    {
        _d3d11Context->RSSetScissorRects(1, &_scissorRect);
        _dirtyStateFlags &= ~RenderStateFlag::ScissorRect;
    }

    if (bitmask::any(RenderStateFlag::RasterDesc, _dirtyStateFlags))
    {
        const auto key = std::bit_cast<uint32_t>(_rasterDesc);
        auto it        = _rasterStateCache.find(key);
        if (it == _rasterStateCache.end()) [[unlikely]]
        {
            D3D11_RASTERIZER_DESC desc = {};
            desc.FillMode              = D3D11_FILL_SOLID;

            switch (_rasterDesc.cullMode)
            {
            case CullMode::NONE:
                desc.CullMode = D3D11_CULL_NONE;
                break;
            case CullMode::BACK:
                desc.CullMode = D3D11_CULL_BACK;
                break;
            case CullMode::FRONT:
                desc.CullMode = D3D11_CULL_FRONT;
                break;
            }

            desc.FrontCounterClockwise = (_rasterDesc.winding == Winding::COUNTER_CLOCK_WISE);

            desc.DepthClipEnable = TRUE;
            desc.ScissorEnable   = _rasterDesc.scissorEnable ? TRUE : FALSE;

            ComPtr<ID3D11RasterizerState> state;
            _AXASSERT_HR(_driver->getDevice()->CreateRasterizerState(&desc, state.GetAddressOf()));
            it = _rasterStateCache.emplace(key, std::move(state)).first;
        }

        _d3d11Context->RSSetState(it->second.Get());
        _dirtyStateFlags &= ~RenderStateFlag::RasterDesc;
    }
}

void RenderContextImpl::setVertexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _vertexBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_vertexBuffer);
    _vertexBuffer = static_cast<BufferImpl*>(buffer);
}

void RenderContextImpl::setIndexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _indexBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_indexBuffer);
    _indexBuffer = static_cast<BufferImpl*>(buffer);
}

void RenderContextImpl::setInstanceBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _instanceBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_instanceBuffer);
    _instanceBuffer = static_cast<BufferImpl*>(buffer);
}

void RenderContextImpl::drawArrays(size_t start, size_t count, bool /*wireframe*/)
{
    prepareDrawing();
    _d3d11Context->Draw(static_cast<UINT>(count), static_cast<UINT>(start));
}

void RenderContextImpl::drawArraysInstanced(size_t start, size_t count, int instanceCount, bool /*wireframe*/)
{
    prepareDrawing();
    _d3d11Context->DrawInstanced(static_cast<UINT>(count), static_cast<UINT>(instanceCount), static_cast<UINT>(start),
                                 0);
}

void RenderContextImpl::drawElements(IndexFormat indexType, size_t count, size_t offset, bool /*wireframe*/)
{
    prepareDrawing();

    const DXGI_FORMAT dxgiFmt = (indexType == IndexFormat::U_SHORT) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    const UINT indexSize      = (indexType == IndexFormat::U_SHORT) ? 2u : 4u;

    assert((offset % indexSize) == 0);

    const UINT startIndex = static_cast<UINT>(offset / indexSize);
    const UINT indexCount = static_cast<UINT>(count);

    _d3d11Context->IASetIndexBuffer(_indexBuffer->internalHandle(), dxgiFmt, 0);
    _d3d11Context->DrawIndexed(indexCount, startIndex, 0);
}

void RenderContextImpl::drawElementsInstanced(IndexFormat indexType,
                                              size_t count,
                                              size_t offset,
                                              int instanceCount,
                                              bool /*wireframe*/)
{
    prepareDrawing();

    const DXGI_FORMAT dxgiFmt = (indexType == IndexFormat::U_SHORT) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    const UINT indexSize      = (indexType == IndexFormat::U_SHORT) ? 2u : 4u;

    assert((offset % indexSize) == 0);

    const UINT startIndex = static_cast<UINT>(offset / indexSize);
    const UINT indexCount = static_cast<UINT>(count);

    _d3d11Context->IASetIndexBuffer(_indexBuffer->internalHandle(), dxgiFmt, 0);
    _d3d11Context->DrawIndexedInstanced(static_cast<UINT>(count), static_cast<UINT>(instanceCount), startIndex, 0, 0);
}

void RenderContextImpl::endRenderPass()
{
    _programState = nullptr;
    _vertexLayout = nullptr;
    AX_SAFE_RELEASE_NULL(_indexBuffer);
    AX_SAFE_RELEASE_NULL(_vertexBuffer);
    AX_SAFE_RELEASE_NULL(_instanceBuffer);

    // clean bound SRVs to resolve: D3D11 WARNING: ID3D11DeviceContext::OMSetRenderTargets: Resource being set to OM
    // RenderTarget slot 0 is still bound on input!
    if (_textureBounds)
    {
        _nullSRVs.resize(_textureBounds, nullptr);
        _d3d11Context->PSSetShaderResources(0, _textureBounds, _nullSRVs.data());
        _textureBounds = 0;
    }
}

void RenderContextImpl::prepareDrawing()
{
    assert(_programState);
    applyRenderStates();

    auto context = _driver->getContext();

    auto& callbackUniforms = _programState->getCallbackUniforms();
    for (auto& cb : callbackUniforms)
        cb.second(_programState, cb.first);

    // bind shader
    auto program = static_cast<ProgramImpl*>(_programState->getProgram());
    program->apply(context);

    // bind vertex layout
    auto vertexLayoutImpl = static_cast<VertexLayoutImpl*>(_vertexLayout);
    vertexLayoutImpl->apply(_d3d11Context, program);

    auto& vertexLayoutDesc = vertexLayoutImpl->getDesc();

    // bind vertexBuffer
    if (!_instanceBuffer)
    {
        ID3D11Buffer* vbs[] = {_vertexBuffer->internalHandle()};
        UINT strides[]      = {static_cast<UINT>(vertexLayoutDesc.getStride())};
        UINT offsets[]      = {0};
        _d3d11Context->IASetVertexBuffers(0, 1, vbs, strides, offsets);
    }
    else
    {
        ID3D11Buffer* vbs[] = {_vertexBuffer->internalHandle(), _instanceBuffer->internalHandle()};
        UINT strides[]      = {static_cast<UINT>(vertexLayoutDesc.getStride()),
                               static_cast<UINT>(vertexLayoutDesc.getInstanceStride())};
        UINT offsets[]      = {0, 0};
        _d3d11Context->IASetVertexBuffers(0, 2, vbs, strides, offsets);
    }

    auto& cpuBuffer = _programState->getUniformBuffer();
    program->bindUniformBuffers(_d3d11Context, cpuBuffer.data(), cpuBuffer.size());

    // bind texture
    _textureBounds = 0;
    for (const auto& [bindingIndex, bindingSet] : _programState->getTextureBindingSets())
    {
        auto& texs     = bindingSet.texs;
        auto arraySize = static_cast<UINT>(texs.size());
        for (UINT k = 0; k < arraySize; ++k)
        {
            const auto slot  = bindingIndex + k;
            auto textureImpl = static_cast<TextureImpl*>(texs[k]);
            context->PSSetShaderResources(slot, 1, &textureImpl->internalHandle().srv);
            auto samplerState = textureImpl->getSamplerState();
            context->PSSetSamplers(slot, 1, &samplerState);
            ++_textureBounds;
        }
    }

    // depth stencil
    _depthStencilState->apply(context, _stencilReferenceValue);
}

void RenderContextImpl::endFrame()
{
    HRESULT hr = _swapChain->Present(_syncInterval, _presentFlags);
#ifdef NDEBUG
    (void)hr;
#else
    if (FAILED(hr))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            auto device    = static_cast<DriverImpl*>(axdrv)->getDevice();
            HRESULT reason = device->GetDeviceRemovedReason();
            AXLOGD("D3D11 Device remove reason: {}", reason);
        }
        // else if (hr == DXGI_ERROR_WAS_STILL_DRAWING)
        //{
        // }
    }
#endif
}

void RenderContextImpl::readPixels(RenderTarget* rt, std::function<void(const PixelBufferDesc&)> callback)
{
    PixelBufferDesc pbd;

    if (rt->isDefaultRenderTarget())
    {
        D3D11_VIEWPORT vp;
        UINT numViewports = 1;
        _d3d11Context->RSGetViewports(&numViewports, &vp);
        uint32_t width  = static_cast<uint32_t>(vp.Width);
        uint32_t height = static_cast<uint32_t>(vp.Height);
        uint32_t x      = static_cast<uint32_t>(vp.TopLeftX);
        uint32_t y      = static_cast<uint32_t>(vp.TopLeftY);

        // FIXME: After maximizing the window, the viewport not hold whole render target color attachment
        readPixels(rt, x, y, width, height, pbd);
    }
    else
    {
        auto colorAttachment = rt->_color[0].texture;
        if (colorAttachment)
        {
            uint32_t width  = colorAttachment->getWidth();
            uint32_t height = colorAttachment->getHeight();
            readPixels(rt, 0, 0, width, height, pbd);
        }
    }

    callback(pbd);
}

void RenderContextImpl::readPixels(RenderTarget* rt, UINT x, UINT y, UINT width, UINT height, PixelBufferDesc& pbd)
{
    auto tex = static_cast<RenderTargetImpl*>(rt)->getColorAttachment(0).texure;
    assert(tex);

    ID3D11Device* device = _driver->getDevice();

    // D3D11_CPU_ACCESS_READ not allow as render target color attachment
    // so we need create a staging texture for CPU read
    D3D11_TEXTURE2D_DESC desc;
    tex->GetDesc(&desc);
    // desc.Width          = width;
    // desc.Height         = height;
    desc.MipLevels      = 1;
    desc.ArraySize      = 1;
    desc.Usage          = D3D11_USAGE_STAGING;
    desc.BindFlags      = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.MiscFlags      = 0;

    ID3D11Texture2D* stagingTex = nullptr;
    HRESULT hr                  = device->CreateTexture2D(&desc, nullptr, &stagingTex);
    if (FAILED(hr))
        return;

    UINT texW = desc.Width;
    UINT texH = desc.Height;

    x      = std::min<UINT>(x, texW);
    y      = std::min<UINT>(y, texH);
    width  = std::min<UINT>(width, texW - x);
    height = std::min<UINT>(height, texH - y);

    D3D11_BOX srcBox = {x, y, 0, x + width, y + height, 1};

    _d3d11Context->CopySubresourceRegion(stagingTex, 0, 0, 0, 0, tex, 0, &srcBox);

    D3D11_MAPPED_SUBRESOURCE mapped;
    hr = _d3d11Context->Map(stagingTex, 0, D3D11_MAP_READ, 0, &mapped);
    if (FAILED(hr))
    {
        stagingTex->Release();
        return;
    }

    const UINT bytesPerRow = width * 4;
    size_t bufferSize      = bytesPerRow * height;
    uint8_t* dst           = pbd._data.resize(bufferSize);
    uint8_t* src           = reinterpret_cast<uint8_t*>(mapped.pData);

    for (uint32_t row = 0; row < height; ++row)
    {
        memcpy(dst + row * bytesPerRow, src + row * mapped.RowPitch, bytesPerRow);
    }

    pbd._width  = width;
    pbd._height = height;

    _d3d11Context->Unmap(stagingTex, 0);
    SafeRelease(stagingTex);
}

}  // namespace ax::rhi::d3d11
