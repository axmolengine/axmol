/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include <vector>
#include <assert.h>
#include <glad/vulkan.h>
#include <vk_mem_alloc.h>
#include "axmol/rhi/Buffer.h"
#include "axmol/tlx/byte_buffer.hpp"
#include "axmol/rhi/RHITypes.h"  // for MAX_INFLIGHT_BUFFER

namespace ax::rhi::vk
{

class GraphicsDeviceImpl;

/**
 * @addtogroup _vk
 * @{
 */

/**
 * @brief A Vulkan-based Buffer implementation
 *
 */
class BufferImpl final : public Buffer
{
    friend class GraphicsContextImpl;

public:
    using Buffer::setLastFenceValue;
    /**
     * @param device   Vulkan logical device
     * @param physical Vulkan physical device (for memory properties)
     * @param size     request size of buffer
     * @param type     BufferType::VERTEX or BufferType::INDEX
     * @param usage    BufferUsage::STATIC / DYNAMIC / STREAM
     * @param initial  initial data
     */
    BufferImpl(GraphicsDeviceImpl*,
               size_t size,
               BufferType type,
               BufferUsage usage,
               const void* initial,
               uint32_t stride = 0);

    ~BufferImpl();

    void updateData(const void* data, size_t size) override;
    void updateSubData(const void* data, size_t offset, size_t size) override;
    void usingDefaultStoredData(bool needDefaultStoredData) override;

    VkBuffer internalHandle() const noexcept { return _buffer; }
    VkBufferUsageFlags getUsageFlags() const noexcept { return _usageFlags; }

private:
    void createNativeBuffer(const void* initial);

    // If this buffer is created as DYNAMIC (host visible), we support
    // per-frame backing: allocate MAX_INFLIGHT_BUFFER backing buffers and switch
    // among them based on the frame index provided by GraphicsDeviceImpl.
    void updateIndex();  // lazy switch to current frame backing

    tlx::byte_buffer _defaultData;
    bool _needDefaultStoredData = false;

    GraphicsDeviceImpl* _driver{nullptr};

    // Current active handle (may point to one of _dynamicBuffers or the single static buffer)
    VkBuffer _buffer{VK_NULL_HANDLE};
    VmaAllocation _memory{VK_NULL_HANDLE};
    void* _currentMappedData{nullptr};

    // When dynamic backing is used we keep all backings here
    tlx::pod_vector<VkBuffer> _dynamicBuffers;
    tlx::pod_vector<VmaAllocation> _dynamicMemories;
    tlx::pod_vector<void*> _dynamicMappedData;

    int _currentFrameIndex{0};
    uint32_t _lastSeenFrame{UINT32_MAX};

    VkBufferUsageFlags _usageFlags{VK_BUFFER_USAGE_VERTEX_BUFFER_BIT};
    VkMemoryPropertyFlags _memoryProperties{VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};
};

/** @} */

}  // namespace ax::rhi::vk
