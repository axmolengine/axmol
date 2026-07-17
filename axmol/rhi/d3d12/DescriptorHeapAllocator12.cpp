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
#include "axmol/rhi/d3d12/DescriptorHeapAllocator12.h"

namespace ax::rhi::d3d12
{
static void initializeHeapBlock(ID3D12Device* device,
                                D3D12_DESCRIPTOR_HEAP_TYPE type,
                                UINT capacity,
                                bool shaderVisible,
                                DescriptorHeapBlock& b)
{
    b.type          = type;
    b.shaderVisible = shaderVisible;

    D3D12_DESCRIPTOR_HEAP_DESC desc{};
    desc.Type           = type;
    desc.NumDescriptors = capacity;
    desc.Flags          = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&b.heap));

    b.descriptorSize = device->GetDescriptorHandleIncrementSize(type);
    b.capacity       = capacity;
    b.freeBits.resize((capacity + 7) / 8, 0xFF);  // all free

    b.cpuStart = b.heap->GetCPUDescriptorHandleForHeapStart();
    if (shaderVisible)
        b.gpuStart = b.heap->GetGPUDescriptorHandleForHeapStart();
}

DescriptorHeapAllocator::DescriptorHeapAllocator(ID3D12Device* device,
                                                 D3D12_DESCRIPTOR_HEAP_TYPE type,
                                                 UINT initialCapacity,
                                                 bool shaderVisible)
    : _handlePool(initialCapacity), _device(device), _type(type), _shaderVisible(shaderVisible)
{
    _descriptorSize = device->GetDescriptorHandleIncrementSize(type);
    initializeHeapBlock(device, type, initialCapacity, shaderVisible, _blocks.emplace_back());
    _nextCapacity = std::max<UINT>(initialCapacity, 64);
}

void DescriptorHeapAllocator::grow()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _nextCapacity = std::max<UINT>(_nextCapacity * 2, _nextCapacity + 64);
    initializeHeapBlock(_device, _type, _nextCapacity, _shaderVisible, _blocks.emplace_back());
}

static bool tryAcquireSlot(DescriptorHeapBlock& b, uint32_t& outIndex)
{
    // Linear scan; for production consider a freelist or find-first-set
    for (uint32_t i = 0; i < b.capacity; ++i)
    {
        uint32_t byte = i >> 3;
        uint8_t mask  = uint8_t(1u << (i & 7));
        if (b.freeBits[byte] & mask)
        {
            b.freeBits[byte] &= ~mask;
            ++b.used;
            outIndex = i;
            return true;
        }
    }
    return false;
}

static bool tryAcquireContiguous(DescriptorHeapBlock& b, uint32_t count, uint32_t& outBase)
{
    if (count == 0 || count > b.capacity)
        return false;

    uint32_t run = 0;
    for (uint32_t i = 0; i < b.capacity; ++i)
    {
        uint32_t byte = i >> 3;
        uint8_t mask  = uint8_t(1u << (i & 7));
        if (b.freeBits[byte] & mask)
        {
            if (run == 0)
                outBase = i;
            ++run;
            if (run >= count)
            {
                for (uint32_t j = outBase; j < outBase + count; ++j)
                {
                    uint32_t bj = j >> 3;
                    b.freeBits[bj] &= ~uint8_t(1u << (j & 7));
                }
                b.used += count;
                return true;
            }
        }
        else
        {
            run = 0;
        }
    }
    return false;
}

DescriptorHandle* DescriptorHeapAllocator::allocate()
{
    std::lock_guard<std::mutex> lock(_mutex);

    for (uint32_t bi = 0; bi < _blocks.size(); ++bi)
    {
        auto& b      = _blocks[bi];
        uint32_t idx = 0;
        if (tryAcquireSlot(b, idx))
        {
            DescriptorHandle* h = (DescriptorHandle*)_handlePool.allocate();
            h->blockIndex       = bi;
            h->slotIndex        = idx;
            h->shaderVisible    = b.shaderVisible;

            h->cpu = b.cpuStart;
            h->cpu.ptr += SIZE_T(idx) * SIZE_T(b.descriptorSize);

            if (b.shaderVisible)
            {
                h->gpu = b.gpuStart;
                h->gpu.ptr += UINT64(idx) * UINT64(b.descriptorSize);
            }
            return h;
        }
    }

    if (!_allowGrow)
        throw std::runtime_error("DescriptorHeapAllocator out of memory!");

    // No free slot; grow and retry once
    grow();
    auto& b      = _blocks.back();
    uint32_t idx = 0;
    if (!tryAcquireSlot(b, idx))
        return {};  // catastrophic, should not happen

    DescriptorHandle* h = (DescriptorHandle*)_handlePool.allocate();
    h->blockIndex       = uint32_t(_blocks.size() - 1);
    h->slotIndex        = idx;
    h->shaderVisible    = b.shaderVisible;

    h->cpu = b.cpuStart;
    h->cpu.ptr += SIZE_T(idx) * SIZE_T(b.descriptorSize);
    if (b.shaderVisible)
    {
        h->gpu = b.gpuStart;
        h->gpu.ptr += UINT64(idx) * UINT64(b.descriptorSize);
    }
    return h;
}

DescriptorHandle* DescriptorHeapAllocator::allocateBatch(uint32_t count)
{
    if (count == 0)
        return {};

    std::lock_guard<std::mutex> lock(_mutex);

    for (uint32_t bi = 0; bi < _blocks.size(); ++bi)
    {
        auto& b      = _blocks[bi];
        uint32_t base = 0;
        if (tryAcquireContiguous(b, count, base))
        {
            DescriptorHandle* h = (DescriptorHandle*)_handlePool.allocate();
            h->blockIndex       = bi;
            h->slotIndex        = base;
            h->shaderVisible    = b.shaderVisible;

            h->cpu = b.cpuStart;
            h->cpu.ptr += SIZE_T(base) * SIZE_T(b.descriptorSize);

            if (b.shaderVisible)
            {
                h->gpu = b.gpuStart;
                h->gpu.ptr += UINT64(base) * UINT64(b.descriptorSize);
            }
            return h;
        }
    }

    if (!_allowGrow)
        return {};

    grow();
    auto& b      = _blocks.back();
    uint32_t base = 0;
    if (!tryAcquireContiguous(b, count, base))
        return {};

    DescriptorHandle* h = (DescriptorHandle*)_handlePool.allocate();
    h->blockIndex       = uint32_t(_blocks.size() - 1);
    h->slotIndex        = base;
    h->shaderVisible    = b.shaderVisible;

    h->cpu = b.cpuStart;
    h->cpu.ptr += SIZE_T(base) * SIZE_T(b.descriptorSize);
    if (b.shaderVisible)
    {
        h->gpu = b.gpuStart;
        h->gpu.ptr += UINT64(base) * UINT64(b.descriptorSize);
    }
    return h;
}

void DescriptorHeapAllocator::deallocateBatch(DescriptorHandle* h, uint32_t count)
{
    if (!h->valid() || count == 0)
        return;
    std::lock_guard<std::mutex> lock(_mutex);
    if (h->blockIndex >= _blocks.size())
        return;
    auto& b = _blocks[h->blockIndex];
    for (uint32_t i = 0; i < count; ++i)
    {
        uint32_t idx   = h->slotIndex + i;
        if (idx >= b.capacity)
            return;
        uint32_t byte = idx >> 3;
        uint8_t mask  = uint8_t(1u << (idx & 7));
        if (b.freeBits[byte] & mask)
            return; // already free
        b.freeBits[byte] |= mask;
        --b.used;
    }
    _handlePool.deallocate(h);
}

ID3D12DescriptorHeap* DescriptorHeapAllocator::getDescriptorHeap(const DescriptorHandle* handle) const
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (handle->blockIndex >= _blocks.size())
        return nullptr;
    return _blocks[handle->blockIndex].heap.Get();
}

void DescriptorHeapAllocator::deallocate(DescriptorHandle* h)
{
    if (!h->valid())
        return;
    std::lock_guard<std::mutex> lock(_mutex);
    if (h->blockIndex >= _blocks.size())
        return;
    auto& b = _blocks[h->blockIndex];
    if (h->slotIndex >= b.capacity)
        return;

    uint32_t byte = h->slotIndex >> 3;
    uint8_t mask  = uint8_t(1u << (h->slotIndex & 7));
    if (b.freeBits[byte] & mask)
    {
        // double free guard: already free
        return;
    }
    b.freeBits[byte] |= mask;
    --b.used;

    _handlePool.deallocate(h);
}

DescriptorHeapAllocator::Stats DescriptorHeapAllocator::stats() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    Stats s{};
    s.blocks = uint32_t(_blocks.size());
    for (auto& b : _blocks)
    {
        s.capacity += b.capacity;
        s.used += b.used;
    }
    return s;
}

}  // namespace ax::rhi::d3d12
