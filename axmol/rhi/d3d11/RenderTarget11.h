/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/RenderTarget.h"
#include <d3d11.h>
#include <dxgi.h>
#include <optional>

namespace ax::rhi::d3d11
{
/**
 * @addtogroup _d3d11
 * @{
 */

static constexpr DXGI_FORMAT DEFAULT_SWAPCHAIN_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

class GraphicsDeviceImpl;

/**
 * @brief A D3D11-based Shader RenderTarget implementation
 *
 */
class RenderTargetImpl : public RenderTarget
{
public:
    struct Attachment
    {
        ID3D11Texture2D* texure;
        TextureDesc desc;
    };
    RenderTargetImpl(GraphicsDeviceImpl* driver, bool defaultRenderTarget);
    ~RenderTargetImpl();

    void cleanupResources() override;

    void setColorTexture(Texture* texture, int level = 0, int index = 0) override;

    void beginRenderPass(ID3D11DeviceContext*);

    ID3D11RenderTargetView* getRTV(size_t idx) const { return _rtvs[idx]; }
    ID3D11DepthStencilView* getDSV() const { return _dsv; }

    Attachment getColorAttachment(int index) const;
    Attachment getDepthStencilAttachment() const;

    bool rebuildSwapchainBuffers(IDXGISwapChain* swapchain,
                                 uint32_t width,
                                 uint32_t height,
                                 std::optional<UINT> swapchainFlags = std::nullopt);

private:
    GraphicsDeviceImpl* _driver{nullptr};
    ID3D11Device* _device{nullptr};
    tlx::inlined_vector<ID3D11RenderTargetView*, INITIAL_COLOR_CAPACITY> _rtvs{};
    ID3D11DepthStencilView* _dsv{nullptr};
    uint32_t _rtvCuont{0};
};

/** @} */

}  // namespace ax::rhi::d3d11
