/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software are
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
#include "axmol/rhi/vulkan/UtilsVK.h"
#include "axmol/rhi/RHITypes.h"  // MAX_INFLIGHT_BUFFER

namespace ax::rhi::vk
{
// Determine priority based on buffer usage
static float evalMemoryPriority(VkMemoryPropertyFlags properties, VkBufferUsageFlags usage)
{
    // Highest priority for GPU-written resources
    if (usage & (VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT))
    {
        return 1.0f;  // Highest priority for GPU storage
    }

    // Medium priority for frequently accessed resources
    if (usage &
        (VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT))
    {
        return 0.75f;
    }

    // Lower priority for CPU-written staging buffers
    if (properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        return 0.25f;  // Lower priority for host-visible memory
    }

    return 0.5f;  // Default medium priority
}

// Determine VMA memory usage based on buffer usage and memory properties
static VmaMemoryUsage getVmaMemoryUsage(BufferUsage usage, VkMemoryPropertyFlags memoryProps)
{
    switch (usage)
    {
    case BufferUsage::DYNAMIC:  // GPU read, CPU write
        return VMA_MEMORY_USAGE_AUTO_PREFER_HOST;

    case BufferUsage::STATIC:  // GPU read/write, updated via staging
        return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

    case BufferUsage::IMMUTABLE:  // GPU read, must provide initial data
        return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

    default:
        // Fallback based on memory properties
        if (memoryProps & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        {
            return VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        }
        else
        {
            return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        }
    }
}

// Determine VMA allocation flags based on buffer usage
static VmaAllocationCreateFlags getVmaAllocationFlags(BufferUsage usage, VkBufferUsageFlags bufferUsage)
{
    VmaAllocationCreateFlags flags = 0;

    switch (usage)
    {
    case BufferUsage::DYNAMIC:
        // For dynamic buffers that are frequently written by CPU, use sequential write optimization
        flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        // Keep memory mapped for faster updates
        flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
        break;

    case BufferUsage::STATIC:
    case BufferUsage::IMMUTABLE:
        // Static/immutable buffers are typically device-local, no special flags needed
        break;
    }

    // For staging buffers (transfer source), optimize for sequential writes
    if (bufferUsage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
    {
        flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    }

    return flags;
}

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
        // read backbuffer, map as staging or copy target
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

    // Create native buffers (single or per-frame backings depending on memory properties)
    if (usage != BufferUsage::IMMUTABLE || initial)
        createNativeBuffer(initial);
}

BufferImpl::~BufferImpl()
{
    // Clean up all VMA allocations
    auto& vmaAllocator = _driver->getVmaAllocator();

    // If per-frame dynamic backings were created, dispose all of them
    if (!_dynamicBuffers.empty())
    {
        for (size_t i = 0; i < _dynamicBuffers.size(); ++i)
        {
            if (_dynamicBuffers[i] != VK_NULL_HANDLE && _dynamicMemories[i] != nullptr)
            {
                _driver->disposeBuffer(_dynamicBuffers[i], _lastFenceValue);
                _driver->disposeVmaMemory(_dynamicMemories[i], _lastFenceValue);
            }
        }
    }
    else
    {
        // Single buffer allocation
        if (_buffer != VK_NULL_HANDLE && _memory != nullptr)
        {
            _driver->disposeBuffer(_buffer, _lastFenceValue);
            _driver->disposeVmaMemory(_memory, _lastFenceValue);
        }
    }
}

/* -------------------------------------------------- createNativeBuffer */
void BufferImpl::createNativeBuffer(const void* initial)
{
    auto device       = _driver->getDevice();
    auto vmaAllocator = _driver->getVmaAllocator();

    // If host-visible memory is requested (dynamic usage), allocate per-frame backings
    if (_memoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        // Create MAX_FRAMES_IN_FLIGHT separate host-visible buffers to avoid CPU overwriting GPU-in-flight data.
        _dynamicBuffers.resize(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);
        _dynamicMemories.resize(MAX_FRAMES_IN_FLIGHT, nullptr);
        _dynamicMappedData.resize(MAX_FRAMES_IN_FLIGHT, nullptr);

        // Setup VMA allocation parameters for dynamic buffers
        VmaAllocationCreateInfo allocCreateInfo = {};
        allocCreateInfo.usage                   = getVmaMemoryUsage(_usage, _memoryProperties);
        allocCreateInfo.flags                   = getVmaAllocationFlags(_usage, _usageFlags);

        // Set memory priority if supported
        if (_driver->isMemoryPrioritySupported())
        {
            allocCreateInfo.priority = evalMemoryPriority(_memoryProperties, _usageFlags);
        }

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size        = _capacity;
            bufferInfo.usage       = _usageFlags;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            VmaAllocationInfo allocationInfo{};
            VkResult res = vmaCreateBuffer(vmaAllocator, &bufferInfo, &allocCreateInfo, &_dynamicBuffers[i],
                                           &_dynamicMemories[i], &allocationInfo);
            VK_REQUIRE(res, "vmaCreateBuffer failed");

            // Store mapped pointer if allocation is mapped
            if (allocCreateInfo.flags & VMA_ALLOCATION_CREATE_MAPPED_BIT)
            {
                _dynamicMappedData[i] = allocationInfo.pMappedData;
            }
        }

        // Set active handle to nothing yet (will lazily switch on first write)
        _buffer            = VK_NULL_HANDLE;
        _memory            = nullptr;
        _currentMappedData = nullptr;
        _currentFrameIndex = -1;
    }
    else
    {
        // Single device-local buffer (static/immutable)
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size        = _capacity;
        bufferInfo.usage       = _usageFlags;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Setup VMA allocation parameters for static buffers
        VmaAllocationCreateInfo allocCreateInfo = {};
        allocCreateInfo.usage                   = getVmaMemoryUsage(_usage, _memoryProperties);
        allocCreateInfo.flags                   = getVmaAllocationFlags(_usage, _usageFlags);

        // Set memory priority if supported
        if (_driver->isMemoryPrioritySupported())
        {
            allocCreateInfo.priority = evalMemoryPriority(_memoryProperties, _usageFlags);
        }

        VmaAllocationInfo allocationInfo{};
        VkResult res =
            vmaCreateBuffer(vmaAllocator, &bufferInfo, &allocCreateInfo, &_buffer, &_memory, &allocationInfo);
        VK_REQUIRE(res, "vmaCreateBuffer failed");
    }

    // If initial data provided, write into the current backing
    if (initial)
        updateData(initial, _capacity);
}

/* -------------------------------------------------- updateIndex
   Lazy switch to the backing buffer corresponding to the current frame index
   provided by DriverImpl. This avoids O(N) per-frame iteration and performs
   the switch only when the buffer is actually updated.
*/
void BufferImpl::updateIndex()
{
    // Only applicable if we have multiple dynamic backings
    if (_dynamicBuffers.empty())
        return;

    // DriverImpl is expected to provide a frame index in range [0, MAX_FRAMES_IN_FLIGHT)
    int frameIndex = _driver->getFrameIndex();

    // If current backing already corresponds to this frame, nothing to do.
    if (_currentFrameIndex == frameIndex)
        return;

    // Sanity-check: ensure index fits our backing array.
    AXASSERT(frameIndex >= 0 && frameIndex < static_cast<int>(_dynamicBuffers.size()),
             "frame index out of range for dynamic buffers");

    // Switch active handles to the frame-specific backing
    _currentFrameIndex = frameIndex;
    _buffer            = _dynamicBuffers[frameIndex];
    _memory            = _dynamicMemories[frameIndex];
    _currentMappedData = _dynamicMappedData[frameIndex];
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

    auto device       = _driver->getDevice();
    auto vmaAllocator = _driver->getVmaAllocator();

    // If this buffer has host-visible per-frame backings, ensure we are
    // writing to the backing designated for the current frame.
    if (!_dynamicBuffers.empty())
    {
        updateIndex();
    }

    if (_memoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        // Host visible memory: directly map and copy
        if (_currentMappedData)
        {
            // If memory is already persistently mapped, use the mapped pointer
            ::memcpy(static_cast<uint8_t*>(_currentMappedData) + offset, data, size);

            // Flush the memory range if needed (VMA handles this automatically for mapped allocations)
            vmaFlushAllocation(vmaAllocator, _memory, offset, size);
        }
        else
        {
            VkResult res =
                vmaCopyMemoryToAllocation(vmaAllocator, static_cast<const uint8_t*>(data), _memory, offset, size);
            VK_REQUIRE(res, "vmaCopyMemoryToAllocation failed");
        }
    }
    else
    {
        // Device local memory: use staging buffer + isolate commands
        VkBuffer stagingBuffer          = VK_NULL_HANDLE;
        VmaAllocation stagingAllocation = VK_NULL_HANDLE;

        // Create staging buffer with VMA
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size        = size;
        bufferInfo.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocCreateInfo = {};
        allocCreateInfo.usage                   = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        allocCreateInfo.flags =
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        // Low priority for staging buffers
        if (_driver->isMemoryPrioritySupported())
        {
            allocCreateInfo.priority = 0.1f;
        }

        VmaAllocationInfo allocationInfo{};
        VkResult res = vmaCreateBuffer(vmaAllocator, &bufferInfo, &allocCreateInfo, &stagingBuffer, &stagingAllocation,
                                       &allocationInfo);
        VK_REQUIRE(res, "vmaCreateBuffer failed");

        // Copy data to staging buffer using VMA's convenient function
        res = vmaCopyMemoryToAllocation(vmaAllocator, data, stagingAllocation, 0, size);
        VK_REQUIRE(res, "vmaCopyMemoryToAllocation failed");

        // Record copy command
        auto submission = _driver->startIsolateSubmission();
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = offset;
        copyRegion.size      = size;
        vkCmdCopyBuffer(submission.cmd, stagingBuffer, _buffer, 1, &copyRegion);
        _driver->finishIsolateSubmission(submission);

        // Destroy staging buffer using VMA
        vmaDestroyBuffer(vmaAllocator, stagingBuffer, stagingAllocation);
    }

    // Update default stored data
    if (_needDefaultStoredData)
    {
        if (_defaultData.size() < offset + size)
            _defaultData.resize(offset + size);
        ::memcpy(_defaultData.data() + offset, data, size);
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
