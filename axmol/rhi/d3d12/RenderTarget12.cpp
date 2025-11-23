#include "axmol/rhi/d3d12/RenderTarget12.h"
#include "axmol/rhi/d3d12/Driver12.h"
#include "axmol/base/Logging.h"

namespace ax::rhi::d3d12
{

RenderTargetImpl::RenderTargetImpl(DriverImpl* driver, bool defaultRenderTarget)
    : RenderTarget(defaultRenderTarget), _driver(driver)
{
    if (_defaultRenderTarget)
        _dirtyFlags = TargetBufferFlags::ALL;
    _attachmentTexPtrs.fill(nullptr);
}

RenderTargetImpl::~RenderTargetImpl()
{
    _driver->waitDeviceIdle();
    invalidate();
}

void RenderTargetImpl::invalidate()
{
    _attachmentTexPtrs.fill(nullptr);
    _attachmentsDirty = true;
    _dirtyFlags       = TargetBufferFlags::ALL;
}

void RenderTargetImpl::beginRenderPass(ID3D12GraphicsCommandList* cmd,
                                       const RenderPassDesc& renderPassDesc,
                                       uint32_t width,
                                       uint32_t height)
{
    // Collect attachments
    if (_defaultRenderTarget)
    {
        auto colorTex = _driver->getSwapchainColorAttachment();
        auto depthTex = _driver->getSwapchainDepthStencilAttachment();

        _attachmentTexPtrs.fill(nullptr);

        if (colorTex)
        {
            _attachmentTexPtrs[0] = colorTex;
            //_rtvHandles[0]        = _driver->getRTVDescriptor(colorTex);
        }
        if (depthTex)
        {
            _attachmentTexPtrs[DepthViewIndex] = depthTex;
            //_dsvHandle                         = _driver->getDSVDescriptor(depthTex);
        }
    }
    else
    {
        for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
        {
            if (_color[i].texture)
            {
                auto* texImpl         = static_cast<TextureImpl*>(_color[i].texture);
                _attachmentTexPtrs[i] = texImpl;
                //_rtvHandles[i]        = _driver->getRTVDescriptor(texImpl);
            }
            else
            {
                _attachmentTexPtrs[i] = nullptr;
            }
        }

        if (_depthStencil.texture)
        {
            auto* texImpl                      = static_cast<TextureImpl*>(_depthStencil.texture);
            _attachmentTexPtrs[DepthViewIndex] = texImpl;
            //_dsvHandle                         = _driver->getDSVDescriptor(texImpl);
        }
        else
        {
            _attachmentTexPtrs[DepthViewIndex] = nullptr;
        }
    }

    // Bind render targets
    UINT numRTVs = 0;
    for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
    {
        if (_attachmentTexPtrs[i])
            ++numRTVs;
        else
            break;
    }

    if (_attachmentTexPtrs[DepthViewIndex])
        cmd->OMSetRenderTargets(numRTVs, _rtvHandles.data(), FALSE, &_dsvHandle);
    else
        cmd->OMSetRenderTargets(numRTVs, _rtvHandles.data(), FALSE, nullptr);

    // Clear attachments
    for (size_t i = 0; i < numRTVs; ++i)
    {
        if (bitmask::any(renderPassDesc.flags.clear, getMRTColorFlag(i)))
        {
            FLOAT clearColor[4] = {renderPassDesc.clearColorValue[0], renderPassDesc.clearColorValue[1],
                                   renderPassDesc.clearColorValue[2], renderPassDesc.clearColorValue[3]};
            cmd->ClearRenderTargetView(_rtvHandles[i], clearColor, 0, nullptr);
        }
    }

    if (_attachmentTexPtrs[DepthViewIndex])
    {
        if (bitmask::any(renderPassDesc.flags.clear, TargetBufferFlags::DEPTH_AND_STENCIL))
        {
            cmd->ClearDepthStencilView(_dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
                                       renderPassDesc.clearDepthValue,
                                       static_cast<UINT8>(renderPassDesc.clearStencilValue), 0, nullptr);
        }
    }

    if (!_defaultRenderTarget)
        prepareAttachmentsForRendering(cmd);
}

void RenderTargetImpl::endRenderPass(ID3D12GraphicsCommandList* /*cmd*/)
{
    // In D3D12, nothing explicit: render pass ends when command list finishes
    if (!_defaultRenderTarget)
    {
        for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
        {
            TextureImpl* texImpl = _attachmentTexPtrs[i];
            if (!texImpl)
                break;
            texImpl->setKnownState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        }
        if (_attachmentTexPtrs[DepthViewIndex])
        {
            _attachmentTexPtrs[DepthViewIndex]->setKnownState(D3D12_RESOURCE_STATE_DEPTH_WRITE);
        }
    }
}

void RenderTargetImpl::prepareAttachmentsForRendering(ID3D12GraphicsCommandList* cmd)
{
    if (_defaultRenderTarget)
        return;

    for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
    {
        TextureImpl* texImpl = _attachmentTexPtrs[i];
        if (!texImpl)
            break;
        texImpl->transitionState(cmd, D3D12_RESOURCE_STATE_RENDER_TARGET);
    }

    if (_attachmentTexPtrs[DepthViewIndex])
    {
        _attachmentTexPtrs[DepthViewIndex]->transitionState(cmd, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    }
}

void RenderTargetImpl::rebuildAttachmentsForSwapchain(IDXGISwapChain4* swapchain, uint32_t width, uint32_t height) {
    if (!_defaultRenderTarget) {
        AXLOGW("Attempted to rebuild swapchain attachments on a non-default render target.");
        return;
    }


}

RenderTargetImpl::Attachment RenderTargetImpl::getColorAttachment(int index) const
{
    return _defaultRenderTarget ? _driver->getSwapchainColorAttachment()
                                : static_cast<TextureImpl*>(_color[index].texture);
}

RenderTargetImpl::Attachment RenderTargetImpl::getDepthStencilAttachment() const
{
    return _defaultRenderTarget ? _driver->getSwapchainDepthStencilAttachment()
                                : static_cast<TextureImpl*>(_depthStencil.texture);
}

}  // namespace ax::rhi::d3d12
