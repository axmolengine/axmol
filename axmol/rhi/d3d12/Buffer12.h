/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include <vector>
#include <assert.h>
#include <d3d12.h>

#include "axmol/rhi/DXUtils.h"
#include "axmol/rhi/Buffer.h"
#include "axmol/tlx/byte_buffer.hpp"
#include "axmol/rhi/RHITypes.h"  // for MAX_FRAMES_IN_FLIGHT

namespace ax::rhi::d3d12
{

using Microsoft::WRL::ComPtr;

class GraphicsDeviceImpl;
class GraphicsContextImpl;
struct DescriptorHandle;

/**
 * @addtogroup _d3d12
 * @{
 */

/**
 * @brief A D3D12-based Buffer implementation
 *
 * Notes:
 * - STATIC/IMMUTABLE live in DEFAULT heap, written via upload buffer + command list copy
 * - DYNAMIC lives in UPLOAD heap, mapped for CPU writes
 *
 * For dynamic buffers we allocate per-frame backing in the UPLOAD heap to avoid CPU
 * overwriting GPU-in-flight memory when multiple frames are in flight.
 */
class BufferImpl final : public Buffer
{
    friend class GraphicsContextImpl;

public:
    BufferImpl(GraphicsDeviceImpl* driver,
               size_t size,
               BufferType type,
               BufferUsage usage,
               const void* initial,
               uint32_t stride = 0);
    ~BufferImpl();

    void updateData(const void* data, size_t size) override;
    void updateSubData(const void* data, size_t offset, size_t size) override;
    void usingDefaultStoredData(bool needDefaultStoredData) override;

    ID3D12Resource* internalResource() const noexcept { return _resource.Get(); }
    D3D12_RESOURCE_STATES currentState() const noexcept { return _resourceState; }
    D3D12_RESOURCE_FLAGS resourceFlags() const noexcept { return _resourceFlags; }

    /**
     * Get (and lazily create) a shader-resource view descriptor for this storage buffer.
     * @return The CPU descriptor handle, or null if unavailable.
     */
    const DescriptorHandle* getSRV() const;

    /**
     * Get (and lazily create) an unordered-access view descriptor for this storage buffer.
     * @return The CPU descriptor handle, or null if unavailable.
     */
    const DescriptorHandle* getUAV() const;

private:
    void createNativeBuffer(const void* initial);
    void copyFromUploadBuffer(const void* data, size_t offset, size_t size);
    static size_t alignTo(size_t value, size_t alignment);
    void createViews() const;

    // For dynamic (UPLOAD heap) buffers we allocate per-frame ComPtr<ID3D12Resource>
    // and lazily switch to the one matching the current frame index retrieved from GraphicsDeviceImpl.
    void updateIndex();  // lazy switch to current frame backing

private:
    tlx::byte_buffer _defaultData;
    bool _needDefaultStoredData = false;

    GraphicsDeviceImpl* _driver{nullptr};

    ComPtr<ID3D12Resource> _resource;                       // main GPU buffer for static/default case or convenience
    std::vector<ComPtr<ID3D12Resource>> _dynamicResources;  // per-frame upload resources for DYNAMIC

    D3D12_HEAP_TYPE _heapType{D3D12_HEAP_TYPE_DEFAULT};
    D3D12_RESOURCE_STATES _resourceState{D3D12_RESOURCE_STATE_COMMON};
    D3D12_RESOURCE_FLAGS _resourceFlags{D3D12_RESOURCE_FLAG_NONE};

    mutable DescriptorHandle* _srv{nullptr};
    mutable DescriptorHandle* _uav{nullptr};

    // When using per-frame dynamic backings, current frame index (sentinel -1 = not set)
    int _currentFrameIndex{-1};
};

/** @} */

}  // namespace ax::rhi::d3d12
