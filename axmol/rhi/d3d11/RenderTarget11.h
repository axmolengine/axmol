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

class DriverImpl;

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
    RenderTargetImpl(DriverImpl* driver, bool defaultRenderTarget);
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
    DriverImpl* _driver{nullptr};
    ID3D11Device* _device{nullptr};
    tlx::inlined_vector<ID3D11RenderTargetView*, INITIAL_COLOR_CAPACITY> _rtvs{};
    ID3D11DepthStencilView* _dsv{nullptr};
    uint32_t _rtvCuont{0};
};

/** @} */

}  // namespace ax::rhi::d3d11
