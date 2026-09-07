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
#include "axmol/rhi/d3d12/SamplerBatchCache12.h"
#include "axmol/rhi/d3d12/GraphicsDevice12.h"
#include "axmol/rhi/ProgramState.h"
#include "axmol/rhi/SamplerRegistry.h"
#include "axmol/tlx/hash.hpp"
#include "axmol/tlx/inlined_vector.hpp"
#include "axmol/base/Logging.h"

namespace ax::rhi::d3d12
{
SamplerBatchCache::~SamplerBatchCache()
{
    reset();
}

const DescriptorHandle* SamplerBatchCache::get(const ::ax::rhi::ProgramState* programState)
{
    if (!programState || !_driver || _batchCount == 0)
        return nullptr;

    tlx::inlined_vector<uint16_t, 8> samplerIds;
    for (const auto& sampler : programState->getProgram()->getActiveSamplerInfos())
    {
        if (sampler.presetIndex >= 0)
            continue;

        auto samplerId = programState->getSamplerOverride(sampler.binding);
        if (!samplerId)
            samplerId = sampler.samplerId;
        if (!samplerId)
            return nullptr;

        for (uint16_t i = 0; i < sampler.count; ++i)
            samplerIds.push_back(samplerId.value);
    }

    AXASSERT(samplerIds.size() == _batchCount, "D3D12 custom sampler descriptor count mismatch");
    if (samplerIds.size() != _batchCount)
        return nullptr;

    const auto hash = tlx::hash64_bytes(samplerIds.data(), samplerIds.size() * sizeof(uint16_t));
    if (auto lookup = _batchLookup.find(hash); lookup != _batchLookup.end())
        return lookup->second;

    auto* batch = _driver->getSamplerAllocator()->allocateBatch(_batchCount);
    if (!batch)
        return nullptr;

    const auto descriptorStride = _driver->getSamplerDescriptorStride();
    auto* registry              = SamplerRegistry::getInstance();
    for (size_t i = 0; i < samplerIds.size(); ++i)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE dst = batch->cpu;
        dst.ptr += static_cast<SIZE_T>(i) * static_cast<SIZE_T>(descriptorStride);
        _driver->writeSamplerDescriptor(registry->getSamplerDesc(SamplerId{samplerIds[i]}), dst);
    }

    _batchLookup.emplace(hash, batch);
    _batches.emplace_back(batch);
    return batch;
}

void SamplerBatchCache::reset()
{
    if (_driver)
    {
        for (auto* batch : _batches)
            _driver->getSamplerAllocator()->deallocateBatch(batch, _batchCount);
    }
    _batches.clear();
    _batchLookup.clear();
}
}  // namespace ax::rhi::d3d12
