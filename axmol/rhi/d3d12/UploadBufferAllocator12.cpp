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
#include "axmol/rhi/d3d12/UploadBufferAllocator12.h"
#include <algorithm>
#include <assert.h>

namespace ax::rhi::d3d12
{

static D3D12_RESOURCE_DESC MakeUploadBufferDesc(uint64_t size)
{
    D3D12_RESOURCE_DESC desc{};
    desc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment          = 0;
    desc.Width              = size;
    desc.Height             = 1;
    desc.DepthOrArraySize   = 1;
    desc.MipLevels          = 1;
    desc.Format             = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count   = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags              = D3D12_RESOURCE_FLAG_NONE;
    return desc;
}

static D3D12_HEAP_PROPERTIES MakeUploadHeapProps()
{
    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type                 = D3D12_HEAP_TYPE_UPLOAD;
    heapProps.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProps.CreationNodeMask     = 1;
    heapProps.VisibleNodeMask      = 1;
    return heapProps;
}

uint64_t UploadBufferAllocator::alignUp(uint64_t v, uint64_t a)
{
    return (v + (a - 1)) & ~(a - 1);
}

UploadBufferAllocator::UploadBufferAllocator(ID3D12Device* device, uint64_t ringSize)
    : mDevice(device), mCapacity(ringSize)
{
    assert(device && "ID3D12Device must be valid");
    assert(ringSize > 0 && "Ring size must be > 0");

    auto heapProps = MakeUploadHeapProps();
    auto desc      = MakeUploadBufferDesc(ringSize);

    HRESULT hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                                 D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mRingHeap));
    assert(SUCCEEDED(hr) && "Failed to create ring upload heap");

    D3D12_RANGE noRead{0, 0};
    hr = mRingHeap->Map(0, &noRead, reinterpret_cast<void**>(&mMapped));
    assert(SUCCEEDED(hr) && "Failed to map ring upload heap");

    mWrite = 0;
    mRead  = 0;
}

UploadBufferAllocator::~UploadBufferAllocator()
{
    if (mRingHeap && mMapped)
    {
        mRingHeap->Unmap(0, nullptr);
        mMapped = nullptr;
    }
    mInFlight.clear();
}

UploadSpan UploadBufferAllocator::allocBytes(uint64_t size, uint64_t alignment)
{
    // Try ring first (unless the request is larger than capacity).
    if (size <= mCapacity)
    {
        UploadSpan s = allocFromRing(size, alignment);
        if (s.heap)
            return s;
        // If ring has no space now, caller should retire and retry, or we can fallback.
        // Fallback is useful to avoid stalling.
        // Note: large numbers of fallbacks may increase resource churn.
        return allocOneOff(size);
    }
    // Too large for ring: directly allocate a one-off staging buffer.
    return allocOneOff(size);
}

UploadSpan UploadBufferAllocator::allocTextureFootprint(uint64_t totalBytes)
{
    // Texture footprints should be aligned to 256 (row pitch requirement).
    return allocBytes(totalBytes, 256);
}

UploadSpan UploadBufferAllocator::allocFromRing(uint64_t size, uint64_t alignment)
{
    UploadSpan span{};
    if (!mRingHeap || !mMapped)
        return span;

    const uint64_t aligned = alignUp(size, alignment);

    // Case 1: space remains at the tail [write..capacity)
    if (mWrite + aligned <= mCapacity)
    {
        span.cpuPtr = mMapped + mWrite;
        span.offset = mWrite;
        span.size   = aligned;
        span.heap   = mRingHeap.Get();
        span.isRing = true;

        mWrite += aligned;
        return span;
    }

    // Case 2: wrap to 0 if it won't overlap unread region [0..read)
    if (aligned <= mRead)
    {
        span.cpuPtr = mMapped + 0;
        span.offset = 0;
        span.size   = aligned;
        span.heap   = mRingHeap.Get();
        span.isRing = true;

        mWrite = aligned;  // wrapped
        return span;
    }

    // No space currently available in ring.
    return UploadSpan{};
}

UploadSpan UploadBufferAllocator::allocOneOff(uint64_t size)
{
    UploadSpan span{};
    assert(mDevice && "Device must be valid for one-off allocation");

    auto heapProps = MakeUploadHeapProps();
    auto desc      = MakeUploadBufferDesc(alignUp(size, 4));  // basic 4-byte alignment

    ComPtr<ID3D12Resource> res;
    HRESULT hr = mDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                                  D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&res));
    assert(SUCCEEDED(hr) && "Failed to create one-off upload buffer");

    uint8_t* mapped = nullptr;
    D3D12_RANGE noRead{0, 0};
    hr = res->Map(0, &noRead, reinterpret_cast<void**>(&mapped));
    assert(SUCCEEDED(hr) && "Failed to map one-off upload buffer");

    span.cpuPtr = mapped;
    span.offset = 0;
    span.size   = desc.Width;
    span.heap   = res.Get();
    span.isRing = false;
    span.oneOff = res;
    return span;
}

void UploadBufferAllocator::markSubmitted(const UploadSpan& span, uint64_t completionSerial)
{
    // Only track ring allocations; one-off allocations don't need ring retirement.
    // Callers may release one-off COMPtr when done (after commands are submitted).
    if (!mRingHeap || !span.isRing)
        return;

    // Record the in-flight range and its completion serial.
    InFlightRange r{span.offset, span.size, completionSerial};
    mInFlight.push_back(r);
}

void UploadBufferAllocator::retireSync(const UploadSpan& span)
{
    if (span.isRing)
    {
        // ring buffer:
        if (span.offset == mRead)
        {
            mRead = span.offset + span.size;
            if (mRead >= mCapacity)
                mRead = 0;
        }
        else
        {
            reset();
        }
    }
    else if (span.oneOff)
    {
        // one-off staging buffer: just unmap and let ComPtr release
        span.oneOff->Unmap(0, nullptr);
    }
}

void UploadBufferAllocator::retire(uint64_t lastCompletedSerial)
{
    // Advance read head over completed ranges in submission order.
    // If completion is out-of-order, we only advance when the front matches 'read'.
    while (!mInFlight.empty())
    {
        const auto& front = mInFlight.front();
        if (front.serial > lastCompletedSerial)
            break;

        // Advance read pointer if this block starts at read head (contiguous).
        if (front.start == mRead)
        {
            mRead = front.start + front.size;
            if (mRead >= mCapacity)
                mRead = 0;
            mInFlight.pop_front();

            // Merge subsequent contiguous ranges that are also completed to reduce fragmentation.
            while (!mInFlight.empty())
            {
                const auto& next = mInFlight.front();
                if (next.serial > lastCompletedSerial)
                    break;
                // Check contiguous forward segment respecting wrap
                bool contiguous =
                    (mRead != 0 && next.start == mRead) || (mRead == 0 && next.start == 0);  // handle wrap boundary
                if (!contiguous)
                    break;

                mRead = next.start + next.size;
                if (mRead >= mCapacity)
                    mRead = 0;
                mInFlight.pop_front();
            }
        }
        else
        {
            // Front doesn't start at read head; cannot advance safely yet.
            // We need to wait for the segment at 'mRead' to be completed.
            break;
        }
    }
}

void UploadBufferAllocator::reset()
{
    mInFlight.clear();
    mWrite = 0;
    mRead  = 0;
}

}  // namespace ax::rhi::d3d12
