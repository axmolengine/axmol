/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/d3d12/DescriptorHeapAllocator12.h"
#include "axmol/tlx/hlookup.hpp"
#include <stdint.h>
#include <vector>

namespace ax::rhi
{
class ProgramState;
}

namespace ax::rhi::d3d12
{
class GraphicsDeviceImpl;

class SamplerBatchCache
{
public:
    SamplerBatchCache() = default;
    ~SamplerBatchCache();

    SamplerBatchCache(const SamplerBatchCache&)            = delete;
    SamplerBatchCache& operator=(const SamplerBatchCache&) = delete;
    SamplerBatchCache(SamplerBatchCache&& other) noexcept;
    SamplerBatchCache& operator=(SamplerBatchCache&& other) noexcept;

    void initialize(GraphicsDeviceImpl* driver, uint32_t batchCount);

    const DescriptorHandle* get(const ::ax::rhi::ProgramState* programState);
    void reset();

private:
    GraphicsDeviceImpl* _driver{nullptr};
    uint32_t _batchCount{0};
    tlx::hash_map<uint64_t, DescriptorHandle*> _batchLookup;
    std::vector<DescriptorHandle*> _batches;
};
}  // namespace ax::rhi::d3d12
