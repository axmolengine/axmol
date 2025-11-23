/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/
 ****************************************************************************/
#pragma once

#include <vector>
#include <cassert>
#include <wrl/client.h>
#include <d3d12.h>

#include "axmol/rhi/Buffer.h"
#include "axmol/tlx/byte_buffer.hpp"

namespace ax::rhi::d3d12
{

using Microsoft::WRL::ComPtr;

class DriverImpl;

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
 */
class BufferImpl final : public Buffer
{
public:
    BufferImpl(DriverImpl* driver, std::size_t size, BufferType type, BufferUsage usage, const void* initial);
    ~BufferImpl();

    void updateData(const void* data, std::size_t size) override;
    void updateSubData(const void* data, std::size_t offset, std::size_t size) override;
    void usingDefaultStoredData(bool needDefaultStoredData) override;

    ID3D12Resource* internalResource() const noexcept { return _resource.Get(); }
    D3D12_RESOURCE_STATES currentState() const noexcept { return _resourceState; }
    D3D12_RESOURCE_FLAGS resourceFlags() const noexcept { return _resourceFlags; }

private:
    void createNativeBuffer(const void* initial);
    void copyFromUploadBuffer(const void* data, std::size_t offset, std::size_t size);
    void ensureUploadHeap(std::size_t size);
    static std::size_t alignTo(std::size_t value, std::size_t alignment);

private:
    size_t _capacity{0};

    axstd::byte_buffer _defaultData;
    bool _needDefaultStoredData = false;

    DriverImpl* _driver{nullptr};

    ComPtr<ID3D12Resource> _resource;      // main GPU buffer
    ComPtr<ID3D12Resource> _uploadBuffer;  // transient upload buffer (for DEFAULT heap writes)

    D3D12_HEAP_TYPE _heapType{D3D12_HEAP_TYPE_DEFAULT};
    D3D12_RESOURCE_STATES _resourceState{D3D12_RESOURCE_STATE_COMMON};
    D3D12_RESOURCE_FLAGS _resourceFlags{D3D12_RESOURCE_FLAG_NONE};
};

/** @} */

}  // namespace ax::rhi::d3d12
