#pragma once

#include <d3d12.h>
#include <stdint.h>
#include <wrl/client.h>
#include <vector>
#include <atomic>
#include <mutex>

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
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
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
    DescriptorHandle allocate();

    ID3D12DescriptorHeap* getDescriptorHeap(const DescriptorHandle& handle) const;

    // Free immediately (caller ensures GPU no longer uses it)
    void free(const DescriptorHandle& h);

    // Defer free to a frame/fence value
    void deferFree(const DescriptorHandle& h, uint64_t fenceValue);

    // Must be called to reap deferred frees when fence reaches value
    void reapDeferred(uint64_t completedFence);

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
    ID3D12Device* _device = nullptr;
    D3D12_DESCRIPTOR_HEAP_TYPE _type;
    bool _shaderVisible  = false;
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
