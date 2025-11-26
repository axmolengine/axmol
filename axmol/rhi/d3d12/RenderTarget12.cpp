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
#include "axmol/rhi/d3d12/RenderTarget12.h"
#include "axmol/rhi/d3d12/RenderContext12.h"
#include "axmol/base/Logging.h"

namespace ax::rhi::d3d12
{

RenderTargetImpl::RenderTargetImpl(DriverImpl* driver, bool defaultRenderTarget)
    : RenderTarget(defaultRenderTarget), _driver(driver)
{
    if (_defaultRenderTarget)
        _dirtyFlags = TargetBufferFlags::ALL;
}

RenderTargetImpl::~RenderTargetImpl()
{
    _driver->waitDeviceIdle();
    invalidate();
}

void RenderTargetImpl::invalidate()
{
    _dirtyFlags = TargetBufferFlags::ALL;

    for (auto i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
    {
        if (_rtvsDescriptors[i])
        {
            _driver->queueDisposal(_rtvsDescriptors[i], DisposableResource::Type::RenderTargetView, _lastFenceValue);
            _rtvsDescriptors[i] = nullptr;
            _rtvHandles[i].ptr  = 0;
        }
        if (_color[i].texture)
        {
            _color[i].texture->release();
            _color[i].texture = nullptr;
        }
    }

    if (_dsvDescriptor)
    {
        _driver->queueDisposal(_dsvDescriptor, DisposableResource::Type::DepthStencilView, _lastFenceValue);
        _dsvDescriptor = nullptr;
        _dsvHandle.ptr = 0;
    }

    if (_depthStencil)
    {
        _depthStencil.texture->release();
        _depthStencil.texture = nullptr;
    }
}

void RenderTargetImpl::beginRenderPass(ID3D12GraphicsCommandList* cmd,
                                       const RenderPassDesc& renderPassDesc,
                                       uint32_t width,
                                       uint32_t height,
                                       uint32_t imageIndex)
{
    if (_dirtyFlags != TargetBufferFlags::NONE)
    {
        auto device = _driver->getDevice();

        // --- Handle user-defined render targets (MRT) ---
        for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
        {
            if (!bitmask::any(_dirtyFlags, getMRTColorFlag(i)))
                continue;

            if (_rtvsDescriptors[i])
                _driver->queueDisposal(_rtvsDescriptors[i], DisposableResource::Type::RenderTargetView,
                                       _lastFenceValue);

            if (_color[i].texture)
            {
                auto texImpl        = static_cast<TextureImpl*>(_color[i].texture);
                _rtvsDescriptors[i] = _driver->allocateDescriptor(DisposableResource::Type::RenderTargetView);
                device->CreateRenderTargetView(texImpl->internalHandle().resource.Get(), nullptr,
                                               _rtvsDescriptors[i]->cpu);

                _rtvHandles[i] = _rtvsDescriptors[i]->cpu;
            }
            else
            {
                _rtvsDescriptors[i] = nullptr;
                _rtvHandles[i].ptr  = 0;
            }
        }

        if (bitmask::any(_dirtyFlags, TargetBufferFlags::DEPTH_AND_STENCIL))
        {
            if (_dsvDescriptor)
                _driver->queueDisposal(_dsvDescriptor, DisposableResource::Type::DepthStencilView, _lastFenceValue);

            if (_depthStencil.texture)
            {
                auto texImpl   = static_cast<TextureImpl*>(_depthStencil.texture);
                _dsvDescriptor = _driver->allocateDescriptor(DisposableResource::Type::DepthStencilView);

                D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
                dsvDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
                dsvDesc.ViewDimension      = D3D12_DSV_DIMENSION_TEXTURE2D;
                dsvDesc.Texture2D.MipSlice = 0;
                device->CreateDepthStencilView(texImpl->internalHandle().resource.Get(), &dsvDesc, _dsvDescriptor->cpu);

                _dsvHandle = _dsvDescriptor->cpu;
            }
            else
            {
                _dsvDescriptor = nullptr;
                _dsvHandle.ptr = 0;
            }
        }

        if (_defaultRenderTarget)
        {
            _numRTVs = 1;
        }
        else
        {
            // Count active color attachments
            _numRTVs = 0;
            for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
            {
                if (_color[i])
                    ++_numRTVs;
                else
                    break;
            }
        }

        // Reset dirty flags after binding
        _dirtyFlags = TargetBufferFlags::NONE;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE* pRTVs{nullptr};
    D3D12_CPU_DESCRIPTOR_HANDLE* pDSV{nullptr};

    // Transition attachments to render target state if not default
    if (_defaultRenderTarget)
    {
        pRTVs = &_rtvHandles[imageIndex];

        auto texImpl = static_cast<TextureImpl*>(_color[imageIndex].texture);
        texImpl->transitionState(cmd, D3D12_RESOURCE_STATE_RENDER_TARGET);

        // Clear color attachments if requested
        if (bitmask::any(renderPassDesc.flags.clear, getMRTColorFlag(imageIndex)))
        {
            FLOAT clearColor[4] = {renderPassDesc.clearColorValue[0], renderPassDesc.clearColorValue[1],
                                   renderPassDesc.clearColorValue[2], renderPassDesc.clearColorValue[3]};
            cmd->ClearRenderTargetView(_rtvHandles[imageIndex], clearColor, 0, nullptr);
        }

        if (_depthStencil)
        {
            pDSV = &_dsvHandle;

            if (bitmask::any(renderPassDesc.flags.clear, TargetBufferFlags::DEPTH_AND_STENCIL))
            {
                cmd->ClearDepthStencilView(_dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
                                           renderPassDesc.clearDepthValue,
                                           static_cast<UINT8>(renderPassDesc.clearStencilValue), 0, nullptr);
            }
        }
    }
    else
    {
        pRTVs = _rtvHandles.data();

        // Clear color attachments if requested
        for (auto i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
        {
            TextureImpl* texImpl = static_cast<TextureImpl*>(_color[i].texture);
            if (!texImpl)
                break;
            texImpl->transitionState(cmd, D3D12_RESOURCE_STATE_RENDER_TARGET);
            if (bitmask::any(renderPassDesc.flags.clear, getMRTColorFlag(i)))
            {
                FLOAT clearColor[4] = {renderPassDesc.clearColorValue[0], renderPassDesc.clearColorValue[1],
                                       renderPassDesc.clearColorValue[2], renderPassDesc.clearColorValue[3]};
                cmd->ClearRenderTargetView(_rtvHandles[i], clearColor, 0, nullptr);
            }
        }
        if (_depthStencil)
        {
            pDSV = &_dsvHandle;
            if (bitmask::any(renderPassDesc.flags.clear, TargetBufferFlags::DEPTH_AND_STENCIL))
            {
                cmd->ClearDepthStencilView(_dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
                                           renderPassDesc.clearDepthValue,
                                           static_cast<UINT8>(renderPassDesc.clearStencilValue), 0, nullptr);
            }
        }
    }

    cmd->OMSetRenderTargets(_numRTVs, pRTVs, FALSE, pDSV);
}

void RenderTargetImpl::endRenderPass(ID3D12GraphicsCommandList* cmd, uint32_t imageIndex)
{
    if (_defaultRenderTarget)
    {
        auto texImpl = static_cast<TextureImpl*>(_color[imageIndex].texture);
        texImpl->transitionState(cmd, D3D12_RESOURCE_STATE_PRESENT);
    }
    else
    {
        for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
        {
            TextureImpl* texImpl = static_cast<TextureImpl*>(_color[i].texture);
            if (!texImpl)
                break;

            texImpl->transitionState(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        }
    }
}

void RenderTargetImpl::rebuildAttachmentsForSwapchain(IDXGISwapChain4* swapchain, uint32_t width, uint32_t height)
{
    if (!_defaultRenderTarget)
    {
        AXLOGW("Attempted to rebuild swapchain attachments on a non-default render target.");
        return;
    }

    static_assert(MAX_COLOR_ATTCHMENT >= RenderContextImpl::SWAPCHAIN_BUFFER_COUNT,
                  "RenderTargetImpl color attachment array too small for swapchain buffers");

    // destroy existing attachments if any
    for (int i = 0; i < RenderContextImpl::SWAPCHAIN_BUFFER_COUNT; ++i)
    {
        if (_color[i].texture)
            AX_SAFE_RELEASE_NULL(_color[i].texture);
    }

    if (_depthStencil.texture)
    {
        AX_SAFE_RELEASE_NULL(_depthStencil.texture);
    }

    // Create color attachments wrapping swapchain buffers
    const UINT colorAttachmentCount = RenderContextImpl::SWAPCHAIN_BUFFER_COUNT;
    for (UINT i = 0; i < colorAttachmentCount; ++i)
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
        HRESULT hr = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
        AXASSERT(SUCCEEDED(hr), "SwapChain GetBuffer failed");

        _color[i].texture = new TextureImpl(_driver, backBuffer);
        _color[i].level   = 0;
    }

    // Create depth-stencil attachment
    // Create a D24S8 texture as default depth-stencil
    TextureDesc depthDesc{};
    depthDesc.textureType  = TextureType::TEXTURE_2D;
    depthDesc.width        = static_cast<uint16_t>(width);
    depthDesc.height       = static_cast<uint16_t>(height);
    depthDesc.arraySize    = 1;
    depthDesc.mipLevels    = 1;
    depthDesc.pixelFormat  = PixelFormat::D24S8;
    depthDesc.textureUsage = TextureUsage::RENDER_TARGET;

    auto tex = new TextureImpl(_driver, depthDesc);
    tex->updateData(nullptr, width, height, 0);  // initialize resource

    // Create DSV in DSV heap
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format        = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags         = D3D12_DSV_FLAG_NONE;
    _depthStencil.texture = tex;
    _depthStencil.level   = 0;

    _dirtyFlags = TargetBufferFlags::ALL;
}

RenderTargetImpl::Attachment RenderTargetImpl::getColorAttachment(int index) const
{
    return static_cast<TextureImpl*>(_color[index].texture);
}

RenderTargetImpl::Attachment RenderTargetImpl::getDepthStencilAttachment() const
{
    return static_cast<TextureImpl*>(_depthStencil.texture);
}

}  // namespace ax::rhi::d3d12
