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
#include "axmol/rhi/d3d12/Buffer12.h"
#include "axmol/rhi/d3d12/Driver12.h"
#include "axmol/base/Logging.h"

namespace ax::rhi::d3d12
{

// BufferType -> default state usage flags (for barriers and binding convenience)
static D3D12_RESOURCE_FLAGS translateResourceFlags(BufferType t)
{
    switch (t)
    {
    case BufferType::VERTEX:
    case BufferType::INDEX:
    case BufferType::UNIFORM:
        return D3D12_RESOURCE_FLAG_NONE;
    case BufferType::PIXEL_PACK_BUFFER:  // readback
        return D3D12_RESOURCE_FLAG_NONE;
    default:
        AXLOGW("Unknown BufferType: {}", static_cast<uint32_t>(t));
        return D3D12_RESOURCE_FLAG_NONE;
    }
}

static D3D12_RESOURCE_STATES translateInitialState(BufferType t, BufferUsage usage)
{
    if (usage == BufferUsage::DYNAMIC)
    {
        // Upload heap typically starts as GENERIC_READ (CPU visible)
        return D3D12_RESOURCE_STATE_GENERIC_READ;
    }

    // DEFAULT heap buffers start as COPY_DEST if they will receive data, otherwise COMMON
    switch (t)
    {
    case BufferType::VERTEX:
        return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    case BufferType::INDEX:
        return D3D12_RESOURCE_STATE_INDEX_BUFFER;
    case BufferType::UNIFORM:
        return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    case BufferType::PIXEL_PACK_BUFFER:
        // If used as copy destination for readback, start COMMON and transition when needed
        return D3D12_RESOURCE_STATE_COMMON;
    default:
        return D3D12_RESOURCE_STATE_COMMON;
    }
}

std::size_t BufferImpl::alignTo(std::size_t value, std::size_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

/* -------------------------------------------------- ctor */
BufferImpl::BufferImpl(DriverImpl* driver, std::size_t size, BufferType type, BufferUsage usage, const void* initial)
    : Buffer(size, type, usage), _driver(driver)
{
    AXASSERT(_driver, "DriverImpl must not be null");

    _resourceFlags = translateResourceFlags(type);
    _heapType      = (usage == BufferUsage::DYNAMIC) ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;

    _capacity = (type == BufferType::UNIFORM) ? alignTo(size, 256) : size;  // CB size must be 256-byte aligned in D3D12

    if (initial && size)
        _defaultData.assign(static_cast<const uint8_t*>(initial), static_cast<const uint8_t*>(initial) + size);

    if (usage != BufferUsage::IMMUTABLE || initial)
        createNativeBuffer(initial);
}

BufferImpl::~BufferImpl()
{
    if (_resource)
        _driver->queueDisposal(_resource.Detach(), _lastFenceValue);
}

/* -------------------------------------------------- createNativeBuffer */
void BufferImpl::createNativeBuffer(const void* initial)
{
    auto* device = _driver->getDevice();

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension           = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment           = 0;
    desc.Width               = static_cast<UINT64>(_capacity);
    desc.Height              = 1;
    desc.DepthOrArraySize    = 1;
    desc.MipLevels           = 1;
    desc.Format              = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count    = 1;
    desc.SampleDesc.Quality  = 0;
    desc.Layout              = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags               = _resourceFlags;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type                  = _heapType;
    heapProps.CPUPageProperty       = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference  = D3D12_MEMORY_POOL_UNKNOWN;
    heapProps.CreationNodeMask      = 1;
    heapProps.VisibleNodeMask       = 1;

    D3D12_RESOURCE_STATES initState =
        (_heapType == D3D12_HEAP_TYPE_UPLOAD) ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COMMON;

    HRESULT hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, initState, nullptr,
                                                 IID_PPV_ARGS(&_resource));
    AXASSERT(SUCCEEDED(hr), "Failed to create ID3D12Resource buffer");

    // AXLOGD("BufferImpl: Created resource: {} for {}", fmt::ptr(_resource.Get()), fmt::ptr(this));

    _resourceState = initState;

    // Initialize data if provided
    if (initial)
        updateData(initial, _size);
}

/* -------------------------------------------------- updateData */
void BufferImpl::updateData(const void* data, std::size_t size)
{
    AXASSERT(size <= _size, "updateData size overflow");
    AXASSERT(data, "updateData data is null");

    updateSubData(data, 0, size);
}

/* -------------------------------------------------- updateSubData */
void BufferImpl::updateSubData(const void* data, std::size_t offset, std::size_t size)
{
    AXASSERT(data && (offset + size <= _size), "updateSubData out of bounds");

    if (_heapType == D3D12_HEAP_TYPE_UPLOAD)
    {
        // Host visible: map and copy directly
        void* mapped = nullptr;
        D3D12_RANGE readRange{0, 0};  // we don't intend to read
        HRESULT hr = _resource->Map(0, &readRange, &mapped);
        AXASSERT(SUCCEEDED(hr), "Failed to map upload buffer");
        std::memcpy(static_cast<uint8_t*>(mapped) + offset, data, size);
        D3D12_RANGE written{offset, offset + size};
        _resource->Unmap(0, &written);
    }
    else
    {
        // Device local: use upload heap + CopyBufferRegion via command list
        copyFromUploadBuffer(data, offset, size);
    }

    // Update default stored data
    if (_needDefaultStoredData)
    {
        if (_defaultData.size() < offset + size)
            _defaultData.resize(offset + size);
        std::memcpy(_defaultData.data() + offset, data, size);
    }
}

/* -------------------------------------------------- copyFromUploadBuffer */
void BufferImpl::copyFromUploadBuffer(const void* data, std::size_t offset, std::size_t size)
{
    AXASSERT(data && size > 0, "copyFromUploadBuffer invalid args");

    // Allocate upload memory from allocator
    auto allocator = _driver->getUploadBufferAllocator();  // raw pointer
    auto span      = allocator->allocBytes(size);          // 256 alignment for safety

    // Copy data into upload memory
    std::memcpy(span.cpuPtr, data, size);

    // Record isolated copy commands
    auto& submission = _driver->startIsolateSubmission();

    // Transition destination buffer to COPY_DEST if needed
    if (_resourceState != D3D12_RESOURCE_STATE_COPY_DEST)
    {
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource   = _resource.Get();
        barrier.Transition.StateBefore = _resourceState;
        barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        submission->ResourceBarrier(1, &barrier);
        _resourceState = D3D12_RESOURCE_STATE_COPY_DEST;
    }

    // Copy from upload span to GPU buffer
    submission->CopyBufferRegion(_resource.Get(), static_cast<UINT64>(offset), span.heap, span.offset,
                                 static_cast<UINT64>(size));

    // Transition back to a usable state (vertex/index/constant/common)
    D3D12_RESOURCE_STATES targetState = translateInitialState(_type, _usage);
    if (_resourceState != targetState)
    {
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource   = _resource.Get();
        barrier.Transition.StateBefore = _resourceState;
        barrier.Transition.StateAfter  = targetState;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        submission->ResourceBarrier(1, &barrier);
        _resourceState = targetState;
    }

    // Submit & fence
    _driver->finishIsolateSubmission(submission, true);
    allocator->retireSync(span);
}

/* -------------------------------------------------- usingDefaultStoredData */
void BufferImpl::usingDefaultStoredData(bool needDefaultStoredData)
{
    _needDefaultStoredData = needDefaultStoredData;

    if (needDefaultStoredData && !_defaultData.empty())
    {
        // restore to default, write to GPU immediately
        updateData(_defaultData.data(), _defaultData.size());
    }
}

}  // namespace ax::rhi::d3d12
