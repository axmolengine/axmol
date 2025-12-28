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

#include <vector>
#include <cassert>
#include <d3d12.h>

#include "axmol/rhi/DXUtils.h"
#include "axmol/rhi/Buffer.h"
#include "axmol/tlx/byte_buffer.hpp"
#include "axmol/rhi/RHITypes.h"  // for MAX_FRAMES_IN_FLIGHT

namespace ax::rhi::d3d12
{

using Microsoft::WRL::ComPtr;

class DriverImpl;
class RenderContextImpl;

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
    friend class RenderContextImpl;

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
    static std::size_t alignTo(std::size_t value, std::size_t alignment);

    // For dynamic (UPLOAD heap) buffers we allocate per-frame ComPtr<ID3D12Resource>
    // and lazily switch to the one matching the current frame index retrieved from DriverImpl.
    void updateIndex();  // lazy switch to current frame backing

private:
    tlx::byte_buffer _defaultData;
    bool _needDefaultStoredData = false;

    DriverImpl* _driver{nullptr};

    ComPtr<ID3D12Resource> _resource;                       // main GPU buffer for static/default case or convenience
    std::vector<ComPtr<ID3D12Resource>> _dynamicResources;  // per-frame upload resources for DYNAMIC

    D3D12_HEAP_TYPE _heapType{D3D12_HEAP_TYPE_DEFAULT};
    D3D12_RESOURCE_STATES _resourceState{D3D12_RESOURCE_STATE_COMMON};
    D3D12_RESOURCE_FLAGS _resourceFlags{D3D12_RESOURCE_FLAG_NONE};

    // When using per-frame dynamic backings, current frame index (sentinel -1 = not set)
    int _currentFrameIndex{-1};
};

/** @} */

}  // namespace ax::rhi::d3d12
