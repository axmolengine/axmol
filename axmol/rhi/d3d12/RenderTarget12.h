/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors
 https://axmol.dev/
 ****************************************************************************/
#pragma once

#include "axmol/rhi/RenderTarget.h"
#include "Texture12.h"
#include <d3d12.h>
#include <dxgi1_5.h>
#include <wrl/client.h>
#include <array>

namespace ax::rhi::d3d12
{
class DriverImpl;

/**
 * @brief A D3D12-based RenderTarget implementation
 */
class RenderTargetImpl : public RenderTarget
{
public:
    enum
    {
        DepthViewIndex = MAX_COLOR_ATTCHMENT,
    };

    using Attachment = TextureImpl*;

    RenderTargetImpl(DriverImpl* driver, bool defaultRenderTarget);
    ~RenderTargetImpl();

    void invalidate();

    // Begin a render pass using this target
    void beginRenderPass(ID3D12GraphicsCommandList* cmd, const RenderPassDesc& desc, uint32_t width, uint32_t height);

    void endRenderPass(ID3D12GraphicsCommandList* cmd);

    Attachment getColorAttachment(int index) const;
    Attachment getDepthStencilAttachment() const;

    void rebuildAttachmentsForSwapchain(IDXGISwapChain4* swapchain, uint32_t width, uint32_t height);

private:
    void prepareAttachmentsForRendering(ID3D12GraphicsCommandList* cmd);

    DriverImpl* _driver{nullptr};

    // Current attachment descriptors
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, MAX_COLOR_ATTCHMENT + 1> _rtvHandles{};
    D3D12_CPU_DESCRIPTOR_HANDLE _dsvHandle{};

    std::array<TextureImpl*, MAX_COLOR_ATTCHMENT + 1> _attachmentTexPtrs{};
    bool _attachmentsDirty{true};
};

}  // namespace ax::rhi::d3d12
