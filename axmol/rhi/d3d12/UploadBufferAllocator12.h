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

#include <d3d12.h>
#include <wrl/client.h>
#include <deque>
#include <stdint.h>

namespace ax::rhi::d3d12
{

using Microsoft::WRL::ComPtr;

// Returned span of upload memory within the ring or a one-off staging resource.
struct UploadSpan
{
    // CPU write pointer to the start of the allocated region.
    uint8_t* cpuPtr = nullptr;

    // GPU-visible offset within the source resource (ring heap or one-off).
    uint64_t offset = 0;

    // Allocated size (aligned).
    uint64_t size = 0;

    // Source resource to use in Copy* commands.
    ID3D12Resource* heap = nullptr;

    // True if this allocation uses the persistent ring heap; false if it is a one-off staging resource.
    bool isRing = false;

    // (Optional) keep an owning reference when not using the ring.
    ComPtr<ID3D12Resource> oneOff;
    // For one-off, we map/unmap per allocation. For ring, it's persistently mapped.
};

// A simple fence-backed ring-buffer allocator for D3D12 UPLOAD memory.
// - Persistent ring heap (mapped once).
// - 256-byte alignment by default for texture row pitch compatibility.
// - Fallback to one-off staging buffer for large allocations.
// - Track in-flight ranges with a completion serial (typically a fence value).
class UploadBufferAllocator
{
public:
    // Create a ring heap of ringSize bytes (e.g., 16MB or 32MB).
    UploadBufferAllocator(ID3D12Device* device, uint64_t ringSize);
    ~UploadBufferAllocator();

    UploadBufferAllocator(const UploadBufferAllocator&)            = delete;
    UploadBufferAllocator& operator=(const UploadBufferAllocator&) = delete;

    // Allocate 'size' bytes with 'alignment' (default 256).
    // Returns a span that is either inside the ring (isRing=true) or a one-off staging allocation (isRing=false).
    UploadSpan allocBytes(uint64_t size, uint64_t alignment = 256);

    // Convenience: allocate a texture footprint sized region with 256 alignment.
    // 'totalBytes' from GetCopyableFootprints, and returns the UploadSpan12 plus the chosen offset.
    UploadSpan allocTextureFootprint(uint64_t totalBytes);

    // Retire sync
    void retireSync(const UploadSpan& span);

    // Mark that a previously allocated (and submitted) range [startOffset, size] will be completed at
    // 'completionSerial'.
    void markSubmitted(const UploadSpan& span, uint64_t completionSerial);

    // Retire completed ranges up to 'lastCompletedSerial' (advance read head to free space).
    void retire(uint64_t lastCompletedSerial);

    // Reset the allocator (keeps the ring heap; discards in-flight tracking). Only call when GPU is idle or after full
    // retire.
    void reset();

    // Accessors
    ID3D12Resource* ringHeap() const { return mRingHeap.Get(); }
    uint8_t* ringCPU() const { return mMapped; }
    uint64_t capacity() const { return mCapacity; }
    uint64_t writeHead() const { return mWrite; }
    uint64_t readHead() const { return mRead; }

private:
    struct InFlightRange
    {
        uint64_t start;
        uint64_t size;
        uint64_t serial;  // completion serial/fence value
    };

    // Internal helpers
    static uint64_t alignUp(uint64_t v, uint64_t a);
    UploadSpan allocFromRing(uint64_t size, uint64_t alignment);
    UploadSpan allocOneOff(uint64_t size);

    // D3D12 objects
    ComPtr<ID3D12Resource> mRingHeap = nullptr;
    uint8_t* mMapped                 = nullptr;

    // Ring state
    uint64_t mCapacity = 0;
    uint64_t mWrite    = 0;  // next write position
    uint64_t mRead     = 0;  // earliest free position (after retiring)

    std::deque<InFlightRange> mInFlight;
    ID3D12Device* mDevice = nullptr;
};

}  // namespace ax::rhi::d3d12
