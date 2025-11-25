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

#include "axmol/rhi/RenderPipeline.h"
#include "axmol/tlx/hlookup.hpp"
#include <d3d12.h>
#include <wrl/client.h>
#include <unordered_map>

namespace ax::rhi::d3d12
{
class DepthStencilStateImpl;
class VertexLayoutImpl;
class ProgramImpl;
class DriverImpl;

struct RootSignatureEntry
{
    RootSignatureEntry()                                = default;
    RootSignatureEntry(RootSignatureEntry&&)            = default;
    RootSignatureEntry& operator=(RootSignatureEntry&&) = default;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSig;

    // RootParameter indices
    UINT vsUboRootIndex   = UINT_MAX;
    UINT fsUboRootIndex   = UINT_MAX;
    UINT srvRootIndex     = UINT_MAX;
    UINT samplerRootIndex = UINT_MAX;
};

/**
 * @brief D3D12-based RenderPipeline implementation
 *
 * Manage PSO And RootSignature
 */
class RenderPipelineImpl : public RenderPipeline
{
public:
    static constexpr int MAX_DESCRIPTOR_SETS = 2;
    static constexpr int SET_INDEX_UBO       = 0;
    static constexpr int SET_INDEX_SAMPLER   = 1;
    static constexpr int SET_INDEX_SRV       = 1;
    static constexpr int SAMPLER_ROOT_INDEX  = 0;

    explicit RenderPipelineImpl(DriverImpl* driver);
    ~RenderPipelineImpl();

    void prepareUpdate(DepthStencilStateImpl* ds,
                       D3D12_CULL_MODE cullMode,
                       BOOL frontCCW,
                       PrimitiveGroup primitiveGroup)
    {
        _dsState                          = ds;
        _rasterDesc.CullMode              = cullMode;
        _rasterDesc.FrontCounterClockwise = frontCCW;
        _primitiveGroup                   = primitiveGroup;
    }

    void update(const RenderTarget*, const PipelineDesc& desc) override;

    ID3D12PipelineState* getPipelineState() const { return _activePSO.Get(); }
    RootSignatureEntry* getRootSignature() const { return _activeRootSignature; }

private:
    void initializePipelineDefaults();

    void updateBlendState(const BlendDesc& blendDesc);
    void updateRootSignature(ProgramImpl* program);
    void updateGraphicsPipeline(const PipelineDesc& desc, ProgramImpl* program);

private:
    DriverImpl* _driver{nullptr};

    const DepthStencilStateImpl* _dsState{nullptr};

    PrimitiveGroup _primitiveGroup{};

    D3D12_BLEND_DESC _blendDesc{};
    D3D12_RASTERIZER_DESC _rasterDesc{};
    D3D12_INPUT_LAYOUT_DESC _inputLayout{};

    RootSignatureEntry* _activeRootSignature{nullptr};
    Microsoft::WRL::ComPtr<ID3D12PipelineState> _activePSO;

    axstd::hash_map<uintptr_t, Microsoft::WRL::ComPtr<ID3D12PipelineState>> _psoCache;
    axstd::hash_map<uintptr_t, RootSignatureEntry> _rootSigCache;
};

}  // namespace ax::rhi::d3d12
