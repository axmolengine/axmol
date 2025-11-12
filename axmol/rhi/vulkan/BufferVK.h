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
#include <glad/vulkan.h>

#include "axmol/rhi/Buffer.h"
#include "axmol/tlx/byte_buffer.hpp"

namespace ax::rhi::vk
{

class DriverImpl;

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
public:
    /**
     * @param device   Vulkan logical device
     * @param physical Vulkan physical device (for memory properties)
     * @param size     request size of buffer
     * @param type     BufferType::VERTEX or BufferType::INDEX
     * @param usage    BufferUsage::STATIC / DYNAMIC / STREAM
     * @param initial  initial data
     */
    BufferImpl(DriverImpl*, std::size_t size, BufferType type, BufferUsage usage, const void* initial);

    ~BufferImpl();

    void updateData(const void* data, std::size_t size) override;
    void updateSubData(const void* data, std::size_t offset, std::size_t size) override;
    void usingDefaultStoredData(bool needDefaultStoredData) override;

    VkBuffer internalHandle() const noexcept { return _buffer; }
    VkBufferUsageFlags getUsageFlags() const noexcept { return _usageFlags; }

private:
    void createNativeBuffer(const void* initial);

    size_t _capacity{0};

    axstd::byte_buffer _defaultData;
    bool _needDefaultStoredData = false;

    DriverImpl* _driver{nullptr};
    VkBuffer _buffer{VK_NULL_HANDLE};
    VkDeviceMemory _memory{VK_NULL_HANDLE};

    VkBufferUsageFlags _usageFlags{VK_BUFFER_USAGE_VERTEX_BUFFER_BIT};
    VkMemoryPropertyFlags _memoryProperties{VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};
};

/** @} */

}  // namespace ax::rhi::vk
