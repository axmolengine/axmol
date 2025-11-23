/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors
 https://axmol.dev/
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
    explicit RenderPipelineImpl(ID3D12Device* device);
    ~RenderPipelineImpl();

    void prepareUpdate(DepthStencilStateImpl* ds, D3D12_CULL_MODE cullMode, BOOL frontCCW)
    {
        _dsState                          = ds;
        _rasterDesc.CullMode              = cullMode;
        _rasterDesc.FrontCounterClockwise = frontCCW;
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
    ID3D12Device* _device{nullptr};

    const DepthStencilStateImpl* _dsState{nullptr};

    D3D12_BLEND_DESC _blendDesc{};
    D3D12_RASTERIZER_DESC _rasterDesc{};
    D3D12_INPUT_LAYOUT_DESC _inputLayout{};

    RootSignatureEntry* _activeRootSignature{nullptr};
    Microsoft::WRL::ComPtr<ID3D12PipelineState> _activePSO;

    axstd::hash_map<uintptr_t, Microsoft::WRL::ComPtr<ID3D12PipelineState>> _psoCache;
    axstd::hash_map<uintptr_t, RootSignatureEntry> _rootSigCache;
};

}  // namespace ax::rhi::d3d12
