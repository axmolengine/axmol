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
#include "axmol/rhi/d3d12/GraphicsDevice12.h"
#include "axmol/base/Logging.h"
#include <algorithm>
#include <limits>

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
    case BufferType::STORAGE:
        return D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
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
    case BufferType::STORAGE:
        return D3D12_RESOURCE_STATE_COMMON;
    case BufferType::PIXEL_PACK_BUFFER:
        // If used as copy destination for readback, start COMMON and transition when needed
        return D3D12_RESOURCE_STATE_COMMON;
    default:
        return D3D12_RESOURCE_STATE_COMMON;
    }
}

size_t BufferImpl::alignTo(size_t value, size_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

/* -------------------------------------------------- ctor */
BufferImpl::BufferImpl(GraphicsDeviceImpl* driver,
                       size_t size,
                       BufferType type,
                       BufferUsage usage,
                       const void* initial,
                       uint32_t stride)
    : Buffer(size, type, usage, stride), _driver(driver)
{
    AXASSERT(_driver, "GraphicsDeviceImpl must not be null");

    _resourceFlags = translateResourceFlags(type);
    _heapType      = (usage == BufferUsage::DYNAMIC) ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;

    // D3D12 forbids UAV access on UPLOAD heap resources; storage buffers must live in DEFAULT heap.
    if (type == BufferType::STORAGE)
    {
        AXASSERT(size > 0 && (size % sizeof(uint32_t)) == 0,
                 "D3D storage buffer size must be a non-zero multiple of 4 bytes");
        AXASSERT(stride == 0 || (size % stride) == 0, "Storage buffer size must be divisible by its logical stride");
        _heapType = D3D12_HEAP_TYPE_DEFAULT;
    }

    _capacity = (type == BufferType::UNIFORM) ? alignTo(size, 256) : size;  // CB size must be 256-byte aligned in D3D12

    if (initial && size)
        _defaultData.assign(static_cast<const uint8_t*>(initial), static_cast<const uint8_t*>(initial) + size);

    if (usage != BufferUsage::IMMUTABLE || initial)
        createNativeBuffer(initial);
}

BufferImpl::~BufferImpl()
{
    if (_srv)
        _driver->queueDisposal(_srv, DisposableResource::Type::ShaderResourceView, _lastFenceValue);
    if (_uav)
        _driver->queueDisposal(_uav, DisposableResource::Type::ShaderResourceView, _lastFenceValue);

    // If we allocated per-frame dynamic upload resources, detach and queue disposal for each.
    if (!_dynamicResources.empty())
    {
        for (auto& r : _dynamicResources)
        {
            // Detach and transfer ownership to disposal queue
            ID3D12Resource* raw = r.Detach();
            if (raw)
                _driver->queueDisposal(raw, _lastFenceValue);
        }
    }
    else
    {
        if (_resource)
            _driver->queueDisposal(_resource.Detach(), _lastFenceValue);
    }
}

void BufferImpl::createViews() const
{
    if (!_resource)
        return;

    auto* device = _driver->getDevice();

    if ((_resourceFlags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) && !_uav)
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
        uavDesc.ViewDimension       = D3D12_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.FirstElement = 0;
        if (_type == BufferType::STORAGE)
        {
            // axslcc emits RWByteAddressBuffer for portable writable storage buffers.
            uavDesc.Format                     = DXGI_FORMAT_R32_TYPELESS;
            uavDesc.Buffer.NumElements         = static_cast<UINT>(_capacity / sizeof(uint32_t));
            uavDesc.Buffer.StructureByteStride = 0;
            uavDesc.Buffer.Flags               = D3D12_BUFFER_UAV_FLAG_RAW;
        }
        else
        {
            uavDesc.Format             = DXGI_FORMAT_R32_UINT;
            uavDesc.Buffer.NumElements = static_cast<UINT>(_capacity / 4);
        }
        _uav = _driver->allocateDescriptor(DisposableResource::Type::ShaderResourceView);
        AXASSERT(_uav, "Failed to allocate D3D12 storage buffer UAV descriptor");
        if (_uav)
            device->CreateUnorderedAccessView(_resource.Get(), nullptr, &uavDesc, _uav->cpu);
    }

    if (!_srv)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.ViewDimension           = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Buffer.FirstElement     = 0;
        if (_type == BufferType::STORAGE)
        {
            // axslcc emits ByteAddressBuffer for portable read-only storage buffers.
            srvDesc.Format                     = DXGI_FORMAT_R32_TYPELESS;
            srvDesc.Buffer.NumElements         = static_cast<UINT>(_capacity / sizeof(uint32_t));
            srvDesc.Buffer.StructureByteStride = 0;
            srvDesc.Buffer.Flags               = D3D12_BUFFER_SRV_FLAG_RAW;
        }
        else if (_stride != 0)
        {
            srvDesc.Format                     = DXGI_FORMAT_UNKNOWN;
            srvDesc.Buffer.NumElements         = static_cast<UINT>(_capacity / _stride);
            srvDesc.Buffer.StructureByteStride = _stride;
        }
        else
        {
            srvDesc.Format                     = DXGI_FORMAT_R32_UINT;
            srvDesc.Buffer.NumElements         = static_cast<UINT>(_capacity / sizeof(uint32_t));
            srvDesc.Buffer.StructureByteStride = 0;
        }
        _srv = _driver->allocateDescriptor(DisposableResource::Type::ShaderResourceView);
        AXASSERT(_srv, "Failed to allocate D3D12 storage buffer SRV descriptor");
        if (_srv)
            device->CreateShaderResourceView(_resource.Get(), &srvDesc, _srv->cpu);
    }
}

const DescriptorHandle* BufferImpl::getSRV() const
{
    if (!_srv && _resource)
        createViews();
    return _srv;
}

const DescriptorHandle* BufferImpl::getUAV() const
{
    if (!_uav && _resource)
        createViews();
    return _uav;
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

    // If using UPLOAD heap for dynamic buffers, allocate per-frame upload resources
    if (_heapType == D3D12_HEAP_TYPE_UPLOAD)
    {
        _dynamicResources.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            ComPtr<ID3D12Resource> res;
            HRESULT hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, initState, nullptr,
                                                         IID_PPV_ARGS(&res));
            AXASSERT(SUCCEEDED(hr), "Failed to create upload buffer for dynamic backing");
            _dynamicResources[i] = res;
        }

        // leave _resource empty; we'll lazily switch to the proper per-frame resource on first write
        _resource.Reset();
        _currentFrameIndex = -1;
        _resourceState     = initState;
    }
    else
    {
        // Single device-local resource
        HRESULT hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, initState, nullptr,
                                                     IID_PPV_ARGS(&_resource));
        AXASSERT(SUCCEEDED(hr), "Failed to create ID3D12Resource buffer");

        _resourceState = initState;
    }

    // Initialize data if provided
    if (initial)
        updateData(initial, _size);
}

/* -------------------------------------------------- updateIndex
   Lazy switch to the per-frame upload resource corresponding to the current
   frame index obtained from GraphicsDeviceImpl. Avoids iterating all buffers each frame.
*/
void BufferImpl::updateIndex()
{
    if (_dynamicResources.empty())
        return;

    // driver->getFrameIndex() returns an int in [0, MAX_FRAMES_IN_FLIGHT)
    int frame = _driver->getFrameIndex();

    // If already switched to this frame's backing, nothing to do
    if (_currentFrameIndex == frame)
        return;

    // Sanity-check
    AXASSERT(frame >= 0 && frame < MAX_FRAMES_IN_FLIGHT, "Driver returned frame index out of expected range");

    // Switch active resource to the frame-specific upload resource
    _currentFrameIndex = frame;
    _resource          = _dynamicResources[frame];
    // resource state is known to be GENERIC_READ for upload heap
    _resourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
}

/* -------------------------------------------------- updateData */
void BufferImpl::updateData(const void* data, size_t size)
{
    AXASSERT(size <= _size, "updateData size overflow");
    AXASSERT(data, "updateData data is null");

    updateSubData(data, 0, size);
}

/* -------------------------------------------------- updateSubData */
void BufferImpl::updateSubData(const void* data, size_t offset, size_t size)
{
    AXASSERT(data && (offset + size <= _size), "updateSubData out of bounds");

    if (_heapType == D3D12_HEAP_TYPE_UPLOAD)
    {
        // For upload heap (dynamic), ensure we are writing to the per-frame upload resource.
        updateIndex();

        // Map and copy directly to the upload resource corresponding to current frame
        ID3D12Resource* res = _resource.Get();
        AXASSERT(res != nullptr, "Dynamic upload resource not set");

        void* mapped = nullptr;
        D3D12_RANGE readRange{0, 0};  // We don't intend to read from the resource
        HRESULT hr = res->Map(0, &readRange, &mapped);
        AXASSERT(SUCCEEDED(hr), "Failed to map upload buffer");
        ::memcpy(static_cast<uint8_t*>(mapped) + offset, data, size);
        D3D12_RANGE written{offset, offset + size};
        res->Unmap(0, &written);
    }
    else
    {
        // Device local: use upload allocator + copy via command list
        copyFromUploadBuffer(data, offset, size);
    }

    // Update default stored data
    if (_needDefaultStoredData)
    {
        if (_defaultData.size() < offset + size)
            _defaultData.resize(offset + size);
        ::memcpy(_defaultData.data() + offset, data, size);
    }
}

/* -------------------------------------------------- copyFromUploadBuffer */
void BufferImpl::copyFromUploadBuffer(const void* data, size_t offset, size_t size)
{
    AXASSERT(data && size > 0, "copyFromUploadBuffer invalid args");

    // Allocate upload memory from allocator
    auto allocator = _driver->getUploadBufferAllocator();  // raw pointer
    auto span      = allocator->allocBytes(size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

    // Copy data into upload memory
    ::memcpy(span.cpuPtr, data, size);

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
