/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/DXUtils.h"
#include <d3d12.h>
#include <stdint.h>
#include <vector>
#include <atomic>
#include <mutex>
#include "yasio/object_pool.hpp"

namespace ax::rhi::d3d12
{
struct DescriptorHandle
{
    // Persistent identity
    uint32_t blockIndex = 0;
    uint32_t slotIndex  = 0;

    // CPU & GPU handles for direct use
    D3D12_CPU_DESCRIPTOR_HANDLE cpu = {};
    D3D12_GPU_DESCRIPTOR_HANDLE gpu = {};

    // Validity
    bool shaderVisible = false;
    bool valid() const { return cpu.ptr != 0; }

    void reset() { memset(this, 0x0, sizeof(*this)); }
};

struct DescriptorHeapBlock
{
    ComPtr<ID3D12DescriptorHeap> heap;
    D3D12_DESCRIPTOR_HEAP_TYPE type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    bool shaderVisible              = false;

    UINT descriptorSize = 0;
    UINT capacity       = 0;
    std::vector<uint8_t> freeBits;  // bitset for slots
    uint32_t used{0};

    D3D12_CPU_DESCRIPTOR_HANDLE cpuStart = {};
    D3D12_GPU_DESCRIPTOR_HANDLE gpuStart = {};
};

class DescriptorHeapAllocator
{
public:
    DescriptorHeapAllocator(ID3D12Device* device,
                            D3D12_DESCRIPTOR_HEAP_TYPE type,
                            UINT initialCapacity,
                            bool shaderVisible);

    // Allocate a single slot; returns a persistent handle
    DescriptorHandle* allocate();

    // Allocate N consecutive slots; returns handle to the first slot
    // Returns nullptr if no contiguous range of N slots is available.
    DescriptorHandle* allocateBatch(uint32_t count);

    // Deallocate a batch of count consecutive slots starting at h.
    void deallocateBatch(DescriptorHandle* h, uint32_t count);

    // Deallocate immediately (caller ensures GPU no longer uses it)
    void deallocate(DescriptorHandle* h);

    ID3D12DescriptorHeap* getDescriptorHeap(const DescriptorHandle* handle) const;
    ID3D12DescriptorHeap* getDescriptorHeapByIndex(size_t index) const { return _blocks[index].heap.Get(); }

    void setAllowGrow(bool bval) { _allowGrow = bval; }

    // Recreate a new larger block (auto-called when full)
    void grow();

    // Utility
    UINT descriptorSize() const { return _descriptorSize; }
    D3D12_DESCRIPTOR_HEAP_TYPE type() const { return _type; }

    // Stats & debug
    struct Stats
    {
        uint32_t blocks;
        uint32_t capacity;
        uint32_t used;
    };
    Stats stats() const;

private:
    bool _allowGrow{true};
    yasio::object_pool<DescriptorHandle> _handlePool;
    ID3D12Device* _device = nullptr;
    D3D12_DESCRIPTOR_HEAP_TYPE _type;
    bool _shaderVisible = false;

    UINT _descriptorSize = 0;

    mutable std::mutex _mutex;
    std::vector<DescriptorHeapBlock> _blocks;

    // Growth policy
    UINT _nextCapacity = 0;  // doubled each grow

    // Deferred free list
    struct DeferredFree
    {
        DescriptorHandle h;
        uint64_t fence;
    };
    std::vector<DeferredFree> _deferred;
};

}  // namespace ax::rhi::d3d12
