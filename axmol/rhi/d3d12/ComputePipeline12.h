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

#include "axmol/rhi/ComputePipeline.h"
#include "axmol/rhi/d3d12/DescriptorHeapAllocator12.h"
#include <d3d12.h>
#include <map>
#include <unordered_map>
#include <vector>

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

    uint32_t _customSamplerBatchCount = 0;
    std::map<std::vector<uint16_t>, DescriptorHandle*> _customSamplerBatches;
};

}  // namespace ax::rhi::d3d12
