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

/**
 * @brief D3D12-based RenderPipeline implementation
 *
 * Manage PSO And RootSignature
 */
class RenderPipelineImpl : public RenderPipeline
{
public:
    explicit RenderPipelineImpl(ID3D12Device* device);
    ~RenderPipelineImpl();

    void prepareUpdate(DepthStencilStateImpl* ds) { _dsState = ds; }

    void update(const RenderTarget*, const PipelineDesc& desc) override;

    ID3D12PipelineState* getPipelineState() const { return _activePSO.Get(); }
    ID3D12RootSignature* getRootSignature() const { return _activeRootSignature.Get(); }

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

    Microsoft::WRL::ComPtr<ID3D12RootSignature> _activeRootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> _activePSO;

    axstd::hash_map<uintptr_t, Microsoft::WRL::ComPtr<ID3D12PipelineState>> _psoCache;
    axstd::hash_map<uintptr_t, Microsoft::WRL::ComPtr<ID3D12RootSignature>> _rootSigCache;
};

}  // namespace ax::rhi::d3d12
