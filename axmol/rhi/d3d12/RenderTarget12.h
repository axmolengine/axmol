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
#include "axmol/rhi/d3d12/Driver12.h"
#include "axmol/rhi/d3d12/Texture12.h"
#include <d3d12.h>
#include <dxgi1_5.h>
#include <optional>

namespace ax::rhi::d3d12
{
static constexpr DXGI_FORMAT DEFAULT_SWAPCHAIN_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

class DriverImpl;

/**
 * @brief A D3D12-based RenderTarget implementation
 */
class RenderTargetImpl : public RenderTarget
{
public:
    using Attachment = TextureImpl*;

    RenderTargetImpl(DriverImpl* driver, bool defaultRenderTarget);
    ~RenderTargetImpl();

    void cleanupResources() override;

    void setColorTexture(Texture* texture, int level = 0, int index = 0) override;

    // Begin a render pass using this target
    void beginRenderPass(ID3D12GraphicsCommandList* cmd,
                         const RenderPassDesc& desc,
                         uint32_t width,
                         uint32_t height,
                         uint32_t imageIndex);

    void endRenderPass(ID3D12GraphicsCommandList* cmd, uint32_t imageIndex);

    Attachment getColorAttachment(int index) const;
    Attachment getDepthStencilAttachment() const;

    bool rebuildSwapchainBuffers(IDXGISwapChain4* swapchain,
                                 uint32_t width,
                                 uint32_t height,
                                 std::optional<UINT> swapchainFlags = std::nullopt);

    void setLastFenceValue(uint64_t fenceValue) { _lastFenceValue = fenceValue; }
    uint64_t getLastFenceValue() const { return _lastFenceValue; }

private:
    uint64_t _lastFenceValue{0};
    DriverImpl* _driver{nullptr};

    tlx::inlined_vector<d3d12::DescriptorHandle*, INITIAL_COLOR_CAPACITY> _rtvsDescriptors{};
    DescriptorHandle* _dsvDescriptor{};

    // Current attachment descriptors
    tlx::inlined_vector<D3D12_CPU_DESCRIPTOR_HANDLE, INITIAL_COLOR_CAPACITY> _rtvHandles{};
    D3D12_CPU_DESCRIPTOR_HANDLE _dsvHandle{};

    uint32_t _numRTVs{0};
};

}  // namespace ax::rhi::d3d12
