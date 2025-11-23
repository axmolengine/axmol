/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors
 https://axmol.dev/
 ****************************************************************************/
#pragma once

#include "axmol/rhi/RenderTarget.h"
#include "axmol/rhi/d3d12/Driver12.h"
#include "axmol/rhi/d3d12/Texture12.h"
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
    void beginRenderPass(ID3D12GraphicsCommandList* cmd,
                         const RenderPassDesc& desc,
                         uint32_t width,
                         uint32_t height,
                         uint32_t imageIndex);

    void endRenderPass(ID3D12GraphicsCommandList* cmd, uint32_t imageIndex);

    Attachment getColorAttachment(int index) const;
    Attachment getDepthStencilAttachment() const;

    void rebuildAttachmentsForSwapchain(IDXGISwapChain4* swapchain, uint32_t width, uint32_t height);

private:
    void prepareAttachmentsForRendering(ID3D12GraphicsCommandList* cmd);

    DriverImpl* _driver{nullptr};

    std::array<d3d12::DescriptorHandle, MAX_COLOR_ATTCHMENT> _rtvsDescriptors{};
    DescriptorHandle _dsvDescriptor{};

    // Current attachment descriptors
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, MAX_COLOR_ATTCHMENT> _rtvHandles{};
    D3D12_CPU_DESCRIPTOR_HANDLE _dsvHandle{};

    uint32_t _numRTVs{0};
};

}  // namespace ax::rhi::d3d12
