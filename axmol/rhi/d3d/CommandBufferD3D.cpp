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
#include "axmol/rhi/d3d/CommandBufferD3D.h"
#include "axmol/rhi/d3d/RenderTargetD3D.h"
#include "axmol/rhi/d3d/RenderPipelineD3D.h"
#include "axmol/rhi/d3d/DepthStencilStateD3D.h"
#include "axmol/rhi/d3d/BufferD3D.h"
#include "axmol/rhi/d3d/UtilsD3D.h"
#include "axmol/rhi/d3d/ProgramD3D.h"
#include "axmol/rhi/d3d/VertexLayoutD3D.h"
#include <dxgi1_2.h>
#include <VersionHelpers.h>
#include "axmol/base/Logging.h"

namespace ax::rhi::d3d
{
static D3D11_PRIMITIVE_TOPOLOGY toD3DPrimitiveTopology(PrimitiveType type, bool wireframe)
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

static BOOL _axmolIsWindows10BuildOrGreaterWin32(WORD build)
{
    OSVERSIONINFOEXW osvi = {sizeof(osvi), 10, 0, build};
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

static constexpr DXGI_FORMAT _AX_SWAPCHAIN_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

CommandBufferImpl::CommandBufferImpl(DriverImpl* driver, HWND hwnd)
{
    _driverImpl = driver;
    // TODO: listen window size changed, to call swapChain->ResizeBuffers ?
    // auto hwnd = Director::getInstance()->getRenderView()->getWin32Window();

    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    _screenWidth  = clientRect.right - clientRect.left;
    _screenHeight = clientRect.bottom - clientRect.top;

    auto context         = driver->getContext();
    ID3D11Device* device = driver->getDevice();

    ComPtr<IDXGIDevice> dxgiDevice;
    device->QueryInterface(__uuidof(IDXGIDevice), (void**)dxgiDevice.GetAddressOf());

    ComPtr<IDXGIAdapter> dxgiAdapter;
    dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());

    ComPtr<IDXGIFactory> factory;
    dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)factory.GetAddressOf());

    // create swapchain
    ComPtr<IDXGISwapChain> swapChain;
    ComPtr<IDXGIFactory2> factory2;

    DXGI_SWAP_EFFECT swapEffect = DXGI_SWAP_EFFECT_DISCARD;  // Default is blt mode

    if (SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(&factory2))))
    {
        // DXGI 1.2+ support Flip mode
        DXGI_SWAP_CHAIN_DESC1 desc1 = {};
        desc1.Width                 = _screenWidth;
        desc1.Height                = _screenHeight;
        desc1.Format                = _AX_SWAPCHAIN_FORMAT;
        desc1.SampleDesc.Count      = 1;  // Flip not support MSAA
        desc1.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc1.BufferCount           = 2;

        // choolse swapchain by OS version
        if (_axmolIsWindows10BuildOrGreaterWin32(0))
        {  // Win10+
            desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        }
        else
        {  // Win8 / Win7+PlatformUpdate
            desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        }

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc = {};
        fsDesc.Windowed                        = TRUE;

        ComPtr<IDXGISwapChain1> swapChain1;
        HRESULT hr = factory2->CreateSwapChainForHwnd(device, hwnd, &desc1, &fsDesc, nullptr, &swapChain1);

        if (SUCCEEDED(hr))
        {
            factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
            swapChain1.As(&swapChain);
        }
    }

    // Fallback to blt mode if flip mode fail
    if (!swapChain)
    {
        DXGI_SWAP_CHAIN_DESC scDesc               = {};
        scDesc.BufferCount                        = 1;
        scDesc.BufferDesc.Width                   = _screenWidth;
        scDesc.BufferDesc.Height                  = _screenHeight;
        scDesc.BufferDesc.Format                  = _AX_SWAPCHAIN_FORMAT;
        scDesc.BufferDesc.RefreshRate.Numerator   = 60;
        scDesc.BufferDesc.RefreshRate.Denominator = 1;
        scDesc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scDesc.OutputWindow                       = hwnd;
        scDesc.SampleDesc.Count                   = 1;
        scDesc.SampleDesc.Quality                 = 0;
        scDesc.Windowed                           = TRUE;
        scDesc.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;

        factory->CreateSwapChain(device, &scDesc, &swapChain);
    }

    _swapChain = swapChain.Detach();

    UtilsD3D::updateDefaultRenderTargetAttachments(_driverImpl, _swapChain);

    _nullSRVs.reserve(8);
}

CommandBufferImpl::~CommandBufferImpl()
{
    // cleanup GPU resources
    UtilsD3D::updateDefaultRenderTargetAttachments(nullptr, nullptr);
    _driverImpl->getContext()->OMSetRenderTargets(0, nullptr, nullptr);

    SafeRelease(_swapChain);

    if (_rasterState)
        _rasterState.Reset();
}

bool CommandBufferImpl::resizeSwapChain(uint32_t width, uint32_t height)
{
    if (!_swapChain || !_driverImpl || !_screenRT)
        return false;

    // Since the window size can be zero when minimized, delay rebuilding until it returns to normal state
    if (width == 0 || height == 0)
    {
        return true;
    }

    if (width == _screenWidth && height == _screenHeight)
        return true;

    // 2) Resize swapchain buffers
    UINT flags = 0;
    // if (_allowTearing)
    //     flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    auto impl = static_cast<RenderTargetImpl*>(const_cast<RenderTarget*>(_screenRT));
    impl->invalidate();

    HRESULT hr = _swapChain->ResizeBuffers(0, width, height, _AX_SWAPCHAIN_FORMAT, flags);

    UtilsD3D::updateDefaultRenderTargetAttachments(_driverImpl, _swapChain);

    if (FAILED(hr))
        return false;

    _screenWidth  = width;
    _screenHeight = height;

    return true;
}

void CommandBufferImpl::setDepthStencilState(DepthStencilState* depthStencilState)
{
    _depthStencilState = static_cast<DepthStencilStateImpl*>(depthStencilState);
}

void CommandBufferImpl::setRenderPipeline(RenderPipeline* renderPipeline)
{
    _renderPipeline = static_cast<RenderPipelineImpl*>(renderPipeline);
}

bool CommandBufferImpl::beginFrame()
{
    return true;
}

void CommandBufferImpl::beginRenderPass(const RenderTarget* renderTarget, const RenderPassDesc& renderPassDesc)
{
    auto context = _driverImpl->getContext();

    auto activeRT = static_cast<const RenderTargetImpl*>(renderTarget);
    if (_renderPassDesc == renderPassDesc && _currentRT == activeRT && !activeRT->isDirty())
    {
        ;
    }
    else
    {

        _currentRT      = activeRT;
        _renderPassDesc = renderPassDesc;
    }

    activeRT->update(context);
    activeRT->apply(context);

    auto colorAttachment = activeRT->getColorAttachment(0);
    _renderTargetWidth   = colorAttachment.desc.width;
    _renderTargetHeight  = colorAttachment.desc.height;

    auto clearFlags = renderPassDesc.flags.clear;

    // clear color
    if (bitmask::any(clearFlags, TargetBufferFlags::COLOR))
        context->ClearRenderTargetView(activeRT->getRTV(0), renderPassDesc.clearColorValue.data());

    // clear depth & stencil
    if (bitmask::any(clearFlags, TargetBufferFlags::DEPTH_AND_STENCIL) && activeRT->getDSV())
        context->ClearDepthStencilView(activeRT->getDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                                       renderPassDesc.clearDepthValue,
                                       static_cast<UINT8>(renderPassDesc.clearStencilValue));
}

void CommandBufferImpl::updateDepthStencilState(const DepthStencilDesc& desc)
{
    _depthStencilState->update(desc);
}

void CommandBufferImpl::updatePipelineState(const RenderTarget* rt, const PipelineDesc& desc)
{
    _renderPipeline->update(rt, desc);
}

void CommandBufferImpl::setViewport(int x, int y, unsigned int w, unsigned int h)
{
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX       = x;
    viewport.TopLeftY       = (int)(_renderTargetHeight - y - h);
    viewport.Width          = static_cast<FLOAT>(w);
    viewport.Height         = static_cast<FLOAT>(h);
    viewport.MinDepth       = 0.0f;
    viewport.MaxDepth       = 1.0f;

    _driverImpl->getContext()->RSSetViewports(1, &viewport);
}

void CommandBufferImpl::setCullMode(CullMode mode)
{
    if (_rasterDesc.cullMode != mode)
    {
        _rasterDesc.cullMode = mode;
        _rasterDesc.dirtyFlags |= RF_CULL_MODE;
    }
}

void CommandBufferImpl::setWinding(Winding winding)
{
    if (_rasterDesc.winding != winding)
    {
        _rasterDesc.winding = winding;
        _rasterDesc.dirtyFlags |= RF_WINDING;
    }
}

void CommandBufferImpl::setScissorRect(bool isEnabled, float x, float y, float width, float height)
{
    D3D11_RECT rect{};

    if (isEnabled)
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

    if (_rasterDesc.scissorEnable != isEnabled)
    {
        _rasterDesc.scissorEnable = isEnabled;
        _rasterDesc.dirtyFlags |= RF_SCISSOR;
    }

    _driverImpl->getContext()->RSSetScissorRects(1, &rect);
}

void CommandBufferImpl::updateRasterizerState()
{
    if (!_rasterDesc.dirtyFlags && _rasterState)
        return;
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

    ID3D11RasterizerState* rasterizerState = nullptr;
    HRESULT hr                             = _driverImpl->getDevice()->CreateRasterizerState(&desc, &rasterizerState);
    if (SUCCEEDED(hr))
    {
        _driverImpl->getContext()->RSSetState(rasterizerState);
        _rasterState = rasterizerState;
    }

    _rasterDesc.dirtyFlags = 0;
}

void CommandBufferImpl::setProgramState(ProgramState* programState)
{
    AX_SAFE_RETAIN(programState);
    AX_SAFE_RELEASE(_programState);
    _programState = programState;
}

void CommandBufferImpl::setVertexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _vertexBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_vertexBuffer);
    _vertexBuffer = static_cast<BufferImpl*>(buffer);
}

void CommandBufferImpl::setIndexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _indexBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_indexBuffer);
    _indexBuffer = static_cast<BufferImpl*>(buffer);
}

void CommandBufferImpl::setInstanceBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _instanceBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_instanceBuffer);
    _instanceBuffer = static_cast<BufferImpl*>(buffer);
}

void CommandBufferImpl::drawArrays(PrimitiveType primitiveType, std::size_t start, std::size_t count, bool wireframe)
{
    prepareDrawing();
    auto context = _driverImpl->getContext();
    context->IASetPrimitiveTopology(toD3DPrimitiveTopology(primitiveType, wireframe));
    context->Draw(static_cast<UINT>(count), static_cast<UINT>(start));
}

void CommandBufferImpl::drawArraysInstanced(PrimitiveType primitiveType,
                                            std::size_t start,
                                            std::size_t count,
                                            int instanceCount,
                                            bool wireframe)
{
    prepareDrawing();
    auto context = _driverImpl->getContext();
    context->IASetPrimitiveTopology(toD3DPrimitiveTopology(primitiveType, wireframe));
    context->DrawInstanced(static_cast<UINT>(count), static_cast<UINT>(instanceCount), static_cast<UINT>(start), 0);
}

void CommandBufferImpl::drawElements(PrimitiveType primitiveType,
                                     IndexFormat indexType,
                                     std::size_t count,
                                     std::size_t offset,
                                     bool wireframe)
{
    prepareDrawing();

    const DXGI_FORMAT dxgiFmt = (indexType == IndexFormat::U_SHORT) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    const UINT indexSize      = (indexType == IndexFormat::U_SHORT) ? 2u : 4u;

    assert((offset % indexSize) == 0);

    const UINT startIndex = static_cast<UINT>(offset / indexSize);
    const UINT indexCount = static_cast<UINT>(count);

    auto context = _driverImpl->getContext();

    context->IASetIndexBuffer(_indexBuffer->internalHandle(), dxgiFmt, 0);

    context->IASetPrimitiveTopology(toD3DPrimitiveTopology(primitiveType, wireframe));

    context->DrawIndexed(indexCount, startIndex, 0);
}

void CommandBufferImpl::drawElementsInstanced(PrimitiveType primitiveType,
                                              IndexFormat indexType,
                                              std::size_t count,
                                              std::size_t offset,
                                              int instanceCount,
                                              bool wireframe)
{
    prepareDrawing();

    const DXGI_FORMAT dxgiFmt = (indexType == IndexFormat::U_SHORT) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    const UINT indexSize      = (indexType == IndexFormat::U_SHORT) ? 2u : 4u;

    assert((offset % indexSize) == 0);

    const UINT startIndex = static_cast<UINT>(offset / indexSize);
    const UINT indexCount = static_cast<UINT>(count);

    auto context = _driverImpl->getContext();
    context->IASetIndexBuffer(_indexBuffer->internalHandle(), dxgiFmt, 0);

    context->IASetPrimitiveTopology(toD3DPrimitiveTopology(primitiveType, wireframe));
    context->DrawIndexedInstanced(static_cast<UINT>(count), static_cast<UINT>(instanceCount), startIndex, 0, 0);
}

void CommandBufferImpl::endRenderPass()
{
    AX_SAFE_RELEASE_NULL(_programState);
    AX_SAFE_RELEASE_NULL(_indexBuffer);
    AX_SAFE_RELEASE_NULL(_vertexBuffer);
    AX_SAFE_RELEASE_NULL(_instanceBuffer);

    // clean bound SRVs to resolve: D3D11 WARNING: ID3D11DeviceContext::OMSetRenderTargets: Resource being set to OM
    // RenderTarget slot 0 is still bound on input!
    if (_textureBounds)
    {
        _nullSRVs.resize(_textureBounds, nullptr);
        _driverImpl->getContext()->PSSetShaderResources(0, _textureBounds, _nullSRVs.data());
        _textureBounds = 0;
    }
}

void CommandBufferImpl::prepareDrawing()
{
    assert(_programState);
    updateRasterizerState();

    auto context = _driverImpl->getContext();

    auto& callbackUniforms = _programState->getCallbackUniforms();
    for (auto& cb : callbackUniforms)
        cb.second(_programState, cb.first);

    // bind shader
    auto program = static_cast<ProgramImpl*>(_programState->getProgram());
    context->VSSetShader(static_cast<ID3D11VertexShader*>(program->getVertexShader()->internalHandle()), nullptr, 0);
    context->PSSetShader(static_cast<ID3D11PixelShader*>(program->getFragmentShader()->internalHandle()), nullptr, 0);

    // bind vertex layout
    auto vertexLayout = static_cast<const VertexLayoutImpl*>(_programState->getVertexLayout());
    vertexLayout->apply(context, program);

    // bind vertexBuffer
    if (!_instanceBuffer)
    {
        ID3D11Buffer* vbs[] = {_vertexBuffer->internalHandle()};
        UINT strides[]      = {static_cast<UINT>(vertexLayout->getStride())};
        UINT offsets[]      = {0};
        context->IASetVertexBuffers(0, 1, vbs, strides, offsets);
    }
    else
    {
        ID3D11Buffer* vbs[] = {_vertexBuffer->internalHandle(), _instanceBuffer->internalHandle()};
        UINT strides[]      = {static_cast<UINT>(vertexLayout->getStride()),
                               static_cast<UINT>(vertexLayout->getInstanceStride())};
        UINT offsets[]      = {0, 0};
        context->IASetVertexBuffers(0, 2, vbs, strides, offsets);
    }

    // bind uniform buffer: glsl-optimizer is bound to index 1, axslcc: bound to 0
    constexpr int bindingIndex = DriverImpl::VBO_BINDING_INDEX_START;
    auto vertUB         = _programState->getVertexUniformBuffer();
    if (!vertUB.empty())
    {
        program->bindVertexUniformBuffer(context, vertUB.data(), vertUB.size(), bindingIndex);
    }

    // bind fragmentBuffer
    auto fragUB = _programState->getFragmentUniformBuffer();
    if (!fragUB.empty())
    {
        program->bindFragmentUniformBuffer(context, fragUB.data(), fragUB.size(), bindingIndex);
    }

    // bind texture
    _textureBounds = 0;
    for (const auto& [_, bindingInfo] : _programState->getTextureBindingInfos())
    {
        auto textureImpl    = static_cast<TextureImpl*>(bindingInfo.tex);
        auto& textureHandle = textureImpl->internalHandle();
        context->PSSetShaderResources(bindingInfo.slot, 1, &textureHandle.srv);
        auto samplerState = textureImpl->getSamplerState();
        context->PSSetSamplers(bindingInfo.slot, 1, &samplerState);
        ++_textureBounds;
    }

    // depth stencil
    _depthStencilState->apply(context, _stencilReferenceValue);
}

void CommandBufferImpl::endFrame()
{
    HRESULT hr = _swapChain->Present(1, 0 /*DXGI_PRESENT_DO_NOT_WAIT*/);

    if (FAILED(hr))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            auto device    = static_cast<DriverImpl*>(DriverBase::getInstance())->getDevice();
            HRESULT reason = device->GetDeviceRemovedReason();
            // AXLOGD("D3D11 Device remove reason: {}", reason);
        }
    }
}

void CommandBufferImpl::readPixels(RenderTarget* rt, std::function<void(const PixelBufferDesc&)> callback) {}

}  // namespace ax::rhi::d3d
