/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/ComputePipeline.h"
#include "axmol/rhi/d3d12/DescriptorHeapAllocator12.h"
#include "axmol/rhi/d3d12/SamplerBatchCache12.h"
#include <d3d12.h>
#include <unordered_map>

namespace ax::rhi
{
class ProgramState;
}

namespace ax::rhi::d3d12
{
class ProgramImpl;
class GraphicsDeviceImpl;

/**
 * @brief A D3D12 compute pipeline (root signature + PSO) for a compute program.
 *
 * Root parameters:
 *   - CBV root parameters per uniform block (space 0)
 *   - SRV descriptor table (space 1) for sampled textures and read-only storage buffers
 *   - UAV descriptor table (space 1) for read-write storage buffers
 *   - Sampler descriptor tables (space 1 presets, space 2 custom)
 */
class ComputePipelineImpl : public ComputePipeline
{
public:
    ComputePipelineImpl(GraphicsDeviceImpl* driver, ProgramImpl* program);
    ~ComputePipelineImpl();

    bool isValid() const override { return _pso.Get() != nullptr && _rootSig.Get() != nullptr; }

    ID3D12PipelineState* getPipeline() const { return _pso.Get(); }
    ID3D12RootSignature* getRootSignature() const { return _rootSig.Get(); }

    UINT srvRootIndex() const { return _srvRootIndex; }
    UINT uavRootIndex() const { return _uavRootIndex; }
    UINT samplerRootIndex() const { return _samplerRootIndex; }
    UINT customSamplerRootIndex() const { return _customSamplerRootIndex; }
    UINT cbvRootIndex(int binding) const
    {
        auto it = _cbvRootIndices.find(binding);
        return it != _cbvRootIndices.end() ? it->second : UINT_MAX;
    }
    const DescriptorHandle* getCustomSamplerBatch(const ::ax::rhi::ProgramState* programState);

private:
    bool createRootSignature(ProgramImpl* program);
    void createPipeline(ProgramImpl* program);

    GraphicsDeviceImpl* _driver{nullptr};
    Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSig;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> _pso;

    UINT _srvRootIndex           = UINT_MAX;
    UINT _uavRootIndex           = UINT_MAX;
    UINT _samplerRootIndex       = UINT_MAX;
    UINT _customSamplerRootIndex = UINT_MAX;
    std::unordered_map<int, UINT> _cbvRootIndices;

    SamplerBatchCache _customSamplerBatches;
};

}  // namespace ax::rhi::d3d12
