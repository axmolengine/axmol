/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "axmol/rhi/d3d12/SamplerBatchCache12.h"
#include "axmol/rhi/d3d12/GraphicsDevice12.h"
#include "axmol/rhi/ProgramState.h"
#include "axmol/rhi/SamplerRegistry.h"
#include "axmol/tlx/hash.hpp"
#include "axmol/tlx/inlined_vector.hpp"
#include "axmol/base/Logging.h"
#include <utility>

namespace ax::rhi::d3d12
{
SamplerBatchCache::~SamplerBatchCache()
{
    reset();
}

SamplerBatchCache::SamplerBatchCache(SamplerBatchCache&& other) noexcept
    : _driver(other._driver)
    , _batchCount(other._batchCount)
    , _batchLookup(std::move(other._batchLookup))
    , _batches(std::move(other._batches))
{
    other._driver     = nullptr;
    other._batchCount = 0;
}

SamplerBatchCache& SamplerBatchCache::operator=(SamplerBatchCache&& other) noexcept
{
    if (this != &other)
    {
        reset();
        _driver      = other._driver;
        _batchCount  = other._batchCount;
        _batchLookup = std::move(other._batchLookup);
        _batches     = std::move(other._batches);

        other._driver     = nullptr;
        other._batchCount = 0;
    }
    return *this;
}

void SamplerBatchCache::initialize(GraphicsDeviceImpl* driver, uint32_t batchCount)
{
    AXASSERT(_batches.empty(), "D3D12 sampler batch cache must be initialized before use");
    _driver     = driver;
    _batchCount = batchCount;
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
    _driver     = nullptr;
    _batchCount = 0;
}
}  // namespace ax::rhi::d3d12
