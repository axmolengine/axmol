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
#include "axmol/rhi/vulkan/BufferVK.h"
#include "axmol/rhi/vulkan/DriverVK.h"

namespace ax::rhi::vk
{

// BufferUsage -> VkBufferUsageFlags / VkMemoryPropertyFlags
static void translateUsage(BufferUsage in, VkBufferUsageFlags& outUsage, VkMemoryPropertyFlags& outMemProps)
{
    switch (in)
    {
    case BufferUsage::DYNAMIC:  // GPU read, CPU write
        outUsage =
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        outMemProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        break;

    case BufferUsage::STATIC:  // GPU read/write, updated via staging
        outUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                   VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        outMemProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        break;

    case BufferUsage::IMMUTABLE:  // GPU read, must provide initial data
        outUsage =
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        outMemProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        break;
    }
}

// BufferType -> VkBufferUsageFlags
static VkBufferUsageFlags translateBindFlag(BufferType t)
{
    switch (t)
    {
    case BufferType::VERTEX:
        return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    case BufferType::INDEX:
        return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

    case BufferType::UNIFORM:
        return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    case BufferType::PIXEL_PACK_BUFFER:
        // read backbuffer，map as staging or copy target
        return VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    default:
        AXLOGW("Unknown BufferType: {}", static_cast<uint32_t>(t));
        return 0;
    }
}

static inline std::size_t alignTo(std::size_t value, std::size_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

/* -------------------------------------------------- ctor */
BufferImpl::BufferImpl(DriverImpl* driver, std::size_t size, BufferType type, BufferUsage usage, const void* initial)
    : Buffer(size, type, usage), _driver(driver)
{
    translateUsage(usage, _usageFlags, _memoryProperties);
    _usageFlags |= translateBindFlag(type);

    _capacity = (type == BufferType::UNIFORM) ? alignTo(size, 16) : size;

    if (initial && size)
        _defaultData.assign(static_cast<const uint8_t*>(initial), static_cast<const uint8_t*>(initial) + size);

    if (usage != BufferUsage::IMMUTABLE || initial)
        createNativeBuffer(initial);
}

BufferImpl::~BufferImpl()
{
    if (_buffer != VK_NULL_HANDLE)
        _driver->queueDisposal(_buffer);
    if (_memory != VK_NULL_HANDLE)
        _driver->queueDisposal(_memory);
}

/* -------------------------------------------------- createNativeBuffer */
void BufferImpl::createNativeBuffer(const void* initial)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size        = _capacity;
    bufferInfo.usage       = _usageFlags;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    auto device = _driver->getDevice();
    if (vkCreateBuffer(device, &bufferInfo, nullptr, &_buffer) != VK_SUCCESS)
    {
        AXLOGE("Failed to create VkBuffer, size={}, alignedSize={}", _size, _capacity);
        assert(false && "Failed to create VkBuffer");
    }

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(device, _buffer, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memReq.size;
    allocInfo.memoryTypeIndex = _driver->findMemoryType(memReq.memoryTypeBits, _memoryProperties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &_memory) != VK_SUCCESS)
    {
        AXLOGE("Failed to allocate VkDeviceMemory");
        assert(false && "Failed to allocate VkDeviceMemory");
    }

    vkBindBufferMemory(device, _buffer, _memory, 0);

    if (initial)
        updateData(initial, _capacity);
}

/* -------------------------------------------------- updateData */
void BufferImpl::updateData(const void* data, std::size_t size)
{
    assert(size <= _capacity);
    assert(data);

    updateSubData(data, 0, size);
}

/* -------------------------------------------------- updateSubData */
void BufferImpl::updateSubData(const void* data, std::size_t offset, std::size_t size)
{
    assert(data && (offset + size <= _capacity));

    auto device = _driver->getDevice();

    if (_memoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        // Host visible memory: directly map and copy
        void* mapped = nullptr;
        vkMapMemory(device, _memory, offset, size, 0, &mapped);
        std::memcpy(static_cast<uint8_t*>(mapped), data, size);
        vkUnmapMemory(device, _memory);
    }
    else
    {
        // Device local memory: use staging buffer + isolate commands
        VkBuffer stagingBuf;
        VkDeviceMemory stagingMem;

        // Create staging buffer
        VkBufferCreateInfo bufInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufInfo.size        = size;
        bufInfo.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vkCreateBuffer(device, &bufInfo, nullptr, &stagingBuf);

        // Allocate staging memory (host visible + coherent)
        VkMemoryRequirements memReq{};
        vkGetBufferMemoryRequirements(device, stagingBuf, &memReq);
        VkMemoryAllocateInfo allocInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
        allocInfo.allocationSize  = memReq.size;
        allocInfo.memoryTypeIndex = _driver->findMemoryType(
            memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkAllocateMemory(device, &allocInfo, nullptr, &stagingMem);
        vkBindBufferMemory(device, stagingBuf, stagingMem, 0);

        // Map staging memory and copy data
        void* mapped = nullptr;
        vkMapMemory(device, stagingMem, 0, size, 0, &mapped);
        std::memcpy(mapped, data, size);
        vkUnmapMemory(device, stagingMem);

        // Record copy command
        auto submission = _driver->startIsolateSubmission();
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = offset;
        copyRegion.size      = size;
        vkCmdCopyBuffer(submission.cmd, stagingBuf, _buffer, 1, &copyRegion);
        _driver->finishIsolateSubmission(submission);

        // Destroy staging resources
        vkDestroyBuffer(device, stagingBuf, nullptr);
        vkFreeMemory(device, stagingMem, nullptr);
    }

    // Update default stored data
    if (_needDefaultStoredData)
    {
        if (_defaultData.size() < offset + size)
            _defaultData.resize(offset + size);
        std::memcpy(_defaultData.data() + offset, data, size);
    }
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

}  // namespace ax::rhi::vk
