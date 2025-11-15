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
#include "axmol/rhi/vulkan/RenderContextVK.h"
#include "axmol/rhi/vulkan/RenderTargetVK.h"
#include "axmol/rhi/vulkan/RenderPipelineVK.h"
#include "axmol/rhi/vulkan/DepthStencilStateVK.h"
#include "axmol/rhi/vulkan/VertexLayoutVK.h"
#include "axmol/rhi/vulkan/ProgramVK.h"
#include "axmol/rhi/vulkan/BufferVK.h"
#include "axmol/rhi/vulkan/TextureVK.h"
#include "axmol/rhi/vulkan/UtilsVK.h"
#include "axmol/rhi/vulkan/DriverVK.h"
#include "axmol/rhi/vulkan/SemaphorePoolVK.h"
#include "axmol/rhi/DriverBase.h"
#include "axmol/base/Logging.h"

#include <glad/vulkan.h>
#include <cassert>
#include <algorithm>

namespace ax::rhi::vk
{

/*
 * Helper: map PrimitiveType to VkPrimitiveTopology
 *
 * In the current Axmol Vulkan backend, LINE_LOOP is not supported in order
 * to reduce the number of PSOs (Pipeline State Objects). Vulkan does not
 * provide a native LINE_LOOP topology; it must be emulated using LINE_STRIP
 * with primitiveRestartEnable, which requires additional pipeline variants.
 *
 * If future contributors need LINE_LOOP support, they can:
 *   1. Extend this mapping to include LINE_LOOP → LINE_STRIP,
 *   2. Create additional pipelines with primitiveRestart enabled,
 *   3. Or contact the Axmol maintainers to discuss adding official support.
 *
 * By default, Axmol relies on TRIANGLE_LIST and other common topologies,
 * which cover the majority of rendering scenarios.
 */
static VkPrimitiveTopology toVkPrimitiveTopology(PrimitiveType type)
{
    switch (type)
    {
    case PrimitiveType::POINT:
        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    case PrimitiveType::LINE:
        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case PrimitiveType::LINE_LOOP:
        AXLOGE("axmol-vulkan RHI doesn't support LINE_LOOP");
        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;  // Vulkan has no LINE_LOOP
    case PrimitiveType::LINE_STRIP:
        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    case PrimitiveType::TRIANGLE:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case PrimitiveType::TRIANGLE_STRIP:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    default:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
}

// Helper: map IndexFormat to VkIndexType
static VkIndexType toVkIndexType(IndexFormat fmt)
{
    switch (fmt)
    {
    case IndexFormat::U_SHORT:
        return VK_INDEX_TYPE_UINT16;
    case IndexFormat::U_INT:
        return VK_INDEX_TYPE_UINT32;
    default:
        return VK_INDEX_TYPE_UINT32;
    }
}

// NOTE: This implementation assumes the existence of a Vulkan driver context that owns device, queues,
// swapchain, render pass, and descriptor management. Adapt integration points to your driver as needed.

RenderContextImpl::RenderContextImpl(DriverImpl* driver, VkSurfaceKHR surface)
{
    _driver        = driver;
    _surface       = surface;
    _graphicsQueue = driver->getGraphicsQueue();
    _presentQueue  = driver->getPresentQueue();
    _device        = driver->getDevice();

    // Create a command pool for allocating command buffers
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = driver->getGraphicsQueueFamily();
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VkResult vr               = vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool);
    AXASSERT(vr == VK_SUCCESS, "vkCreateCommandPool failed");

    auto& extent  = driver->getInitialSurfaceExtent();
    _screenWidth  = extent.width;
    _screenHeight = extent.height;

    createCommandBuffers();
    createDescriptorPool();
    rebuildSwapchain();

    // create sync objects
    constexpr VkSemaphoreCreateInfo semaphoreInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    constexpr VkFenceCreateInfo fenceInfo{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                          .flags = VK_FENCE_CREATE_SIGNALED_BIT};

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateFence(_device, &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS)
        {
            AXASSERT(false, "failed to create synchronization objects for a frame!");
        }

        if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_presentCompleteSemaphores[i]) != VK_SUCCESS)
        {
            AXASSERT(false, "failed to create synchronization objects for a frame!");
        }
    }

    // reserve descriptor sets
    _descriptorWritesPerFrame.reserve(16);
    _descriptorImageInfosPerFrame.reserve(16);

    // Create per-frame uniform ring buffers (capacity can be tuned)
    createUniformRingBuffers(1 * 1024 * 1024);  // 1 MB per frame
}

RenderContextImpl::~RenderContextImpl()
{
    vkDeviceWaitIdle(_device);

    destroyUniformRingBuffers();

    for (auto semaphore : _renderFinishedSemaphores)
        vkDestroySemaphore(_device, semaphore, nullptr);
    _renderFinishedSemaphores.clear();

    for (auto semaphore : _presentCompleteSemaphores)
        vkDestroySemaphore(_device, semaphore, nullptr);
    _presentCompleteSemaphores.fill(VK_NULL_HANDLE);

    for (auto fence : _inFlightFences)
        vkDestroyFence(_device, fence, nullptr);
    _inFlightFences.fill(VK_NULL_HANDLE);

    for (auto pool : _descriptorPools)
        vkDestroyDescriptorPool(_device, pool, nullptr);
    _descriptorPools.fill(VK_NULL_HANDLE);

    vkFreeCommandBuffers(_device, _commandPool, static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
    _commandBuffers.fill(VK_NULL_HANDLE);

    if (_commandPool)
    {
        vkDestroyCommandPool(_device, _commandPool, nullptr);
        _commandPool = VK_NULL_HANDLE;
    }

    UtilsVK::destroySwapchainAttachments();

    for (auto view : _swapchainImageViews)
        vkDestroyImageView(_device, view, nullptr);

    if (_swapchain != VK_NULL_HANDLE)
        vkDestroySwapchainKHR(_device, _swapchain, nullptr);

    _swapchainImageViews.clear();
    _swapchainImages.clear();
}

// Create per-frame uniform ring buffers with persistent mapping
void RenderContextImpl::createUniformRingBuffers(std::size_t capacityBytes)
{
    // Query minUniformBufferOffsetAlignment from physical device limits

    auto device = _driver->getDevice();

    VkPhysicalDeviceProperties props{};
    vkGetPhysicalDeviceProperties(_driver->getPhysical(), &props);
    std::size_t devAlign = std::max<std::size_t>(1, props.limits.minUniformBufferOffsetAlignment);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        UniformRingBuffer& ring = _uniformRings[i];
        ring.capacity           = capacityBytes;
        ring.align              = devAlign;

        // Create buffer
        VkBufferCreateInfo bci{};
        bci.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bci.size        = ring.capacity;
        bci.usage       = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;  // descriptor updates will use (buffer, offset, range)
        bci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        VkResult br     = vkCreateBuffer(device, &bci, nullptr, &ring.buffer);
        AXASSERT(br == VK_SUCCESS, "vkCreateBuffer (uniform ring) failed");

        // Allocate memory (HOST_VISIBLE | prefer COHERENT)
        VkMemoryRequirements memReq{};
        vkGetBufferMemoryRequirements(device, ring.buffer, &memReq);

        uint32_t typeIndex = _driver->findMemoryType(
            memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        VkMemoryAllocateInfo mai{};
        mai.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        mai.allocationSize  = memReq.size;
        mai.memoryTypeIndex = typeIndex;

        VkResult ar = vkAllocateMemory(device, &mai, nullptr, &ring.memory);
        AXASSERT(ar == VK_SUCCESS, "vkAllocateMemory (uniform ring) failed");

        VkResult brm = vkBindBufferMemory(device, ring.buffer, ring.memory, 0);
        AXASSERT(brm == VK_SUCCESS, "vkBindBufferMemory (uniform ring) failed");

        // Persistently map
        void* mapped = nullptr;
        VkResult mr  = vkMapMemory(device, ring.memory, 0, ring.capacity, 0, &mapped);
        AXASSERT(mr == VK_SUCCESS, "vkMapMemory (uniform ring) failed");
        ring.mapped     = static_cast<uint8_t*>(mapped);
        ring.writeHead  = 0;
        ring.isCoherent = true;  // we requested COHERENT, keep hint for flush decisions
    }
}

// Destroy per-frame uniform ring buffers
void RenderContextImpl::destroyUniformRingBuffers()
{
    vkDeviceWaitIdle(_device);

    auto device = _device;
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        UniformRingBuffer& ring = _uniformRings[i];
        if (ring.mapped)
        {
            vkUnmapMemory(device, ring.memory);
            ring.mapped = nullptr;
        }
        if (ring.buffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, ring.buffer, nullptr);
            ring.buffer = VK_NULL_HANDLE;
        }
        if (ring.memory != VK_NULL_HANDLE)
        {
            vkFreeMemory(device, ring.memory, nullptr);
            ring.memory = VK_NULL_HANDLE;
        }
    }
}

// Reset current frame ring buffer write head after its fence is signaled
void RenderContextImpl::resetUniformRingForCurrentFrame()
{
    UniformRingBuffer& ring = _uniformRings[_currentFrame];
    ring.writeHead          = 0;
}

// Allocate aligned slice from current frame's ring buffer
RenderContextImpl::UniformSlice RenderContextImpl::allocateUniformSlice(std::size_t size)
{
    UniformRingBuffer& ring = _uniformRings[_currentFrame];

    // Align allocation size to device requirement
    std::size_t aligned = (size + ring.align - 1) & ~(ring.align - 1);

    // Simple overflow check (can be replaced by grow or fallback)
    AXASSERT(ring.writeHead + aligned <= ring.capacity, "Uniform ring buffer overflow");

    UniformSlice s{};
    s.offset = ring.writeHead;
    s.size   = size;
    s.cpuPtr = ring.mapped + s.offset;

    ring.writeHead += aligned;
    return s;
}

void RenderContextImpl::createCommandBuffers()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = _commandPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;  // allocate all at once

    auto result = vkAllocateCommandBuffers(_device, &allocInfo, _commandBuffers.data());
    AXASSERT(result == VK_SUCCESS, "vkAllocateCommandBuffers failed");
}

void RenderContextImpl::createDescriptorPool()
{
    // Define the descriptor types and counts supported by the pool
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 128;  // Adjust as needed, enough to cover all UBOs per frame
    poolSizes[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = 128;  // Enough to cover all texture bindings

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.maxSets       = 128;  // Maximum number of descriptor sets that can be allocated
    poolInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    // Allow individual descriptor sets to be freed for flexible management

    for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        VkResult res = vkCreateDescriptorPool(_device, &poolInfo, nullptr, &_descriptorPools[i]);
        AXASSERT(res == VK_SUCCESS, "Failed to create descriptor pool");
    }
}

bool RenderContextImpl::resizeSwapchain(uint32_t width, uint32_t height)
{
    if (width == _screenWidth && height == _screenHeight)
        return true;

    _screenWidth    = width;
    _screenHeight   = height;
    _swapchainDirty = true;
    return true;
}

void RenderContextImpl::rebuildSwapchain()
{
    auto physical = _driver->getPhysical();

    // Query supported surface formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical, _surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical, _surface, &formatCount, formats.data());

    // Choose suitable surface format
    VkSurfaceFormatKHR surfaceFormat{.format = VK_FORMAT_UNDEFINED};
    PixelFormat pixelFormat{};
    int bestScore = -1;
    for (auto& sf : formats)
    {
        int score = 0;

        PixelFormat pf{};
        if (sf.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            if (sf.format == VK_FORMAT_R8G8B8A8_UNORM)
            {
                pf    = PixelFormat::RGBA8;
                score = 2;
            }
            else if (sf.format == VK_FORMAT_B8G8R8A8_UNORM)
            {
                pf    = PixelFormat::BGRA8;
                score = 1;
            }
        }

        if (score > bestScore)
        {
            bestScore     = score;
            surfaceFormat = sf;
            pixelFormat   = pf;
        }
    }

    if (surfaceFormat.format == VK_FORMAT_UNDEFINED)
    {
        throw std::runtime_error(
            "No suitable surface format found (need R8G8B8A8_UNORM or B8G8R8A8_UNORM with SRGB nonlinear).");
    }

    // Query supported present modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical, _surface, &presentModeCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical, _surface, &presentModeCount, presentModes.data());

    VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;  // guaranteed
    for (auto& pm : presentModes)
    {
        if (pm == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            chosenPresentMode = pm;
            break;
        }
    }

    // Query surface capabilities
    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, _surface, &caps);

    VkExtent2D extent = caps.currentExtent;
    if (extent.width == UINT32_MAX)
    {
        extent.width  = _screenWidth;
        extent.height = _screenHeight;
    }
    if (extent.width == 0 || extent.height == 0)
    {
        AXLOGE("axmol: Failed to create swapchain: extent width/height is 0");
    }

    uint32_t imageCount = caps.minImageCount + 1;
    if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
        imageCount = caps.maxImageCount;

    // Destroy old swapchain if exists
    if (_swapchain != VK_NULL_HANDLE)
    {
        for (auto view : _swapchainImageViews)
            vkDestroyImageView(_device, view, nullptr);
        vkDestroySwapchainKHR(_device, _swapchain, nullptr);
    }

    // Create new swapchain
    VkSwapchainCreateInfoKHR scInfo{};
    scInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    scInfo.surface          = _surface;
    scInfo.minImageCount    = imageCount;
    scInfo.imageFormat      = surfaceFormat.format;
    scInfo.imageColorSpace  = surfaceFormat.colorSpace;
    scInfo.imageExtent      = extent;
    scInfo.imageArrayLayers = 1;
    scInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    scInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    scInfo.preTransform     = caps.currentTransform;
    scInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    scInfo.presentMode      = chosenPresentMode;
    scInfo.clipped          = VK_TRUE;
    scInfo.oldSwapchain     = VK_NULL_HANDLE;

    VkResult vr = vkCreateSwapchainKHR(_device, &scInfo, nullptr, &_swapchain);
    AXASSERT(vr == VK_SUCCESS, "vkCreateSwapchainKHR failed");

    // Retrieve swapchain images
    uint32_t swapImageCount;
    vkGetSwapchainImagesKHR(_device, _swapchain, &swapImageCount, nullptr);
    _swapchainImages.resize(swapImageCount);
    vkGetSwapchainImagesKHR(_device, _swapchain, &swapImageCount, _swapchainImages.data());

    // Create image views
    _swapchainImageViews.resize(swapImageCount);
    for (size_t i = 0; i < _swapchainImages.size(); ++i)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image                           = _swapchainImages[i];
        viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format                          = surfaceFormat.format;
        viewInfo.components                      = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                                                    VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
        viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel   = 0;
        viewInfo.subresourceRange.levelCount     = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount     = 1;

        vr = vkCreateImageView(_device, &viewInfo, nullptr, &_swapchainImageViews[i]);
        AXASSERT(vr == VK_SUCCESS, "vkCreateImageView failed");
    }

    UtilsVK::rebuildSwapchainAttachments(_driver, _swapchainImages, _swapchainImageViews, extent, pixelFormat);

    // Create render finished semaphores
    if (!_renderFinishedSemaphores.empty())
        for (auto semaphore : _renderFinishedSemaphores)
            vkDestroySemaphore(_device, semaphore, nullptr);
    _renderFinishedSemaphores.resize(swapImageCount, VK_NULL_HANDLE);
    VkSemaphoreCreateInfo sci{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    for (uint32_t i = 0; i < swapImageCount; ++i)
    {
        VkResult r = vkCreateSemaphore(_device, &sci, nullptr, &_renderFinishedSemaphores[i]);
        AXASSERT(r == VK_SUCCESS, "vkCreateSemaphore failed");
    }

    // Sync screen size
    if (_screenWidth != extent.width || _screenHeight != extent.height)
    {
        _screenWidth  = extent.width;
        _screenHeight = extent.height;
    }
}

void RenderContextImpl::setDepthStencilState(DepthStencilState* depthStencilState)
{
    _depthStencilState = static_cast<DepthStencilStateImpl*>(depthStencilState);
}

void RenderContextImpl::setRenderPipeline(RenderPipeline* renderPipeline)
{
    _renderPipeline = static_cast<RenderPipelineImpl*>(renderPipeline);
}

bool RenderContextImpl::beginFrame()
{
    if (_swapchainDirty) [[unlikely]]
    {
        vkDeviceWaitIdle(_device);
        rebuildSwapchain();
        _swapchainDirty = false;
    }

    // beginFrame
    vkWaitForFences(_device, 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);

    // Reset uniform ring write head for this frame
    resetUniformRingForCurrentFrame();

    VkResult result = vkAcquireNextImageKHR(_device, _swapchain, UINT64_MAX, _presentCompleteSemaphores[_currentFrame],
                                            VK_NULL_HANDLE, &_currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        // Signal upper layer to recreate swapchain
        AXLOGW("axmol: swapchain is out of date (frame {}), need to recreate", _currentFrame);
        return false;
    }
    if (result == VK_SUBOPTIMAL_KHR && !_suboptimal)
    {
        _suboptimal = true;
        AXLOGW("axmol: Suboptimal swap chain.");
    }
    AXASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR || result == VK_TIMEOUT,
             "vkAcquireNextImageKHR failed");

    _inFrame = true;

    vkResetFences(_device, 1, &_inFlightFences[_currentFrame]);

    UtilsVK::setSwapchainCurrentImageIndex(_currentImageIndex);

    _currentCmdBuffer = _commandBuffers[_currentFrame];
    vkResetCommandBuffer(_currentCmdBuffer, 0);

    VkCommandBufferBeginInfo const binfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    VkResult vr = vkBeginCommandBuffer(_currentCmdBuffer, &binfo);
    AXASSERT(vr == VK_SUCCESS, "vkBeginCommandBuffer failed");

    auto descriptorPool = _descriptorPools[_currentFrame];
    vkResetDescriptorPool(_device, descriptorPool, 0);  // safe: only reset current frame pool

    return true;
}

void RenderContextImpl::beginRenderPass(RenderTarget* renderTarget, const RenderPassDesc& renderPassDesc)
{
    assert(_inFrame);
    auto rtImpl = static_cast<RenderTargetImpl*>(renderTarget);

    _currentRT = renderTarget;

    // Cache target size from first color attachment
    auto colorAttachment = rtImpl->getColorAttachment(0);
    _renderTargetWidth   = colorAttachment->getDesc().width;
    _renderTargetHeight  = colorAttachment->getDesc().height;

    // Delegate to RenderTargetImplVK: it will select/create VkRenderPass and VkFramebuffer
    rtImpl->beginRenderPass(_currentCmdBuffer, renderPassDesc, _renderTargetWidth, _renderTargetHeight);
}

void RenderContextImpl::endRenderPass()
{
    auto rtImpl = static_cast<RenderTargetImpl*>(_currentRT);
    rtImpl->endRenderPass(_currentCmdBuffer);

    // Reset state cache
    _programState = nullptr;
    _vertexLayout = nullptr;

    AX_SAFE_RELEASE_NULL(_indexBuffer);
    AX_SAFE_RELEASE_NULL(_vertexBuffer);
    AX_SAFE_RELEASE_NULL(_instanceBuffer);
}

void RenderContextImpl::endFrame()
{
    VkResult vr = vkEndCommandBuffer(_currentCmdBuffer);
    AXASSERT(vr == VK_SUCCESS, "vkEndCommandBuffer failed");

    // If non-coherent, flush written uniform range before submit (optional)
    {
        UniformRingBuffer& ring = _uniformRings[_currentFrame];
        if (!ring.isCoherent && ring.writeHead > 0)
        {
            VkMappedMemoryRange range{VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE};
            range.memory = ring.memory;
            range.offset = 0;
            range.size   = ring.writeHead;
            vkFlushMappedMemoryRanges(_device, 1, &range);
        }
    }

    const VkSemaphore waitSemaphores[]               = {_presentCompleteSemaphores[_currentFrame]};
    const VkPipelineStageFlags waitSemaphoreStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    // New render-finished semaphore from pool
    VkSemaphore submissionSemaphore = _renderFinishedSemaphores[_currentImageIndex];

    VkSubmitInfo submitInfo{};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &_currentCmdBuffer;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = waitSemaphores;
    submitInfo.pWaitDstStageMask    = waitSemaphoreStages;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = &submissionSemaphore;

    vr = vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFences[_currentFrame]);
    AXASSERT(vr == VK_SUCCESS, "vkQueueSubmit failed");

    // Present: wait on render-finished semaphore
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &submissionSemaphore;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &_swapchain;
    presentInfo.pImageIndices      = &_currentImageIndex;

    vr = vkQueuePresentKHR(_presentQueue, &presentInfo);
    switch (vr)
    {
    case VK_SUCCESS:
        break;
    case VK_SUBOPTIMAL_KHR:
        if (!_suboptimal)
        {
            AXLOGW("axmol: Suboptimal swap chain.");
            _suboptimal = true;
        }
        break;
    case VK_ERROR_OUT_OF_DATE_KHR:
        AXLOGI("axmol: swapchain out of date");
        break;
    default:
        AXASSERT(vr && false, "vkQueuePresentKHR failed");
        break;
    }

    if (!_postFrameOps.empty())
    {
        for (auto& op : _postFrameOps)
            op();

        _postFrameOps.clear();
    }

    _driver->releaseDisposalResources();

    // Advance frame index for multi-frame-in-flight
    _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    _inFrame = false;
}

void RenderContextImpl::updateDepthStencilState(const DepthStencilDesc& desc)
{
    AXASSERT(_depthStencilState, "DepthStencilStateImpl not set");
    _depthStencilState->update(desc);
}

void RenderContextImpl::updatePipelineState(const RenderTarget* rt, const PipelineDesc& desc)
{
    RenderContext::updatePipelineState(rt, desc);
    AXASSERT(_renderPipeline, "RenderPipelineImpl not set");
    _renderPipeline->prepareUpdate(_depthStencilState);
    _renderPipeline->update(rt, desc);
}

void RenderContextImpl::setViewport(int x, int y, unsigned int w, unsigned int h)
{
    if (w == 0 || h == 0)
        return;

    VkViewport vp{};
    vp.x        = static_cast<float>(x);
    vp.y        = static_cast<float>(y + h);
    vp.width    = static_cast<float>(w);
    vp.height   = -static_cast<float>(h);
    vp.minDepth = 0.0f;
    vp.maxDepth = 1.0f;

    _cachedViewport = vp;
}

void RenderContextImpl::setScissorRect(bool isEnabled, float x, float y, float width, float height)
{
    VkRect2D rect{};
    if (isEnabled)
    {
        const float rtW = static_cast<float>(_renderTargetWidth);
        const float rtH = static_cast<float>(_renderTargetHeight);

        // Clamp and normalize coordinates
        const int32_t minX = static_cast<int32_t>(std::clamp(x, 0.f, rtW));
        const int32_t minY = static_cast<int32_t>(std::clamp(y, 0.f, rtH));
        const int32_t maxX = static_cast<int32_t>(std::clamp(x + width, 0.f, rtW));
        const int32_t maxY = static_cast<int32_t>(std::clamp(y + height, 0.f, rtH));

        rect.offset.x      = minX;
        rect.offset.y      = static_cast<int32_t>(rtH) - maxY;  // filp Y
        rect.extent.width  = static_cast<uint32_t>(std::max(0, maxX - minX));
        rect.extent.height = static_cast<uint32_t>(std::max(0, maxY - minY));
    }
    else
    {
        rect.offset = {0, 0};
        rect.extent = {_renderTargetWidth, _renderTargetHeight};
    }

    _scissorEnabled = isEnabled;
    _cachedScissor  = rect;
}

void RenderContextImpl::setCullMode(CullMode mode)
{
    switch (mode)
    {
    case CullMode::NONE:
        _cachedCullMode = VK_CULL_MODE_NONE;
        break;
    case CullMode::BACK:
        _cachedCullMode = VK_CULL_MODE_BACK_BIT;
        break;
    case CullMode::FRONT:
        _cachedCullMode = VK_CULL_MODE_FRONT_BIT;
        break;
    }
}

void RenderContextImpl::setWinding(Winding winding)
{
    switch (winding)
    {
    case Winding::CLOCK_WISE:
        _cachedFrontFace = VK_FRONT_FACE_CLOCKWISE;
        break;
    case Winding::COUNTER_CLOCK_WISE:
        _cachedFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        break;
    }
}

void RenderContextImpl::setVertexBuffer(Buffer* buffer)
{
    if (!buffer || _vertexBuffer == buffer)
        return;
    buffer->retain();
    AX_SAFE_RELEASE(_vertexBuffer);
    _vertexBuffer = static_cast<BufferImpl*>(buffer);
}

void RenderContextImpl::setIndexBuffer(Buffer* buffer)
{
    if (!buffer || _indexBuffer == buffer)
        return;
    buffer->retain();
    AX_SAFE_RELEASE(_indexBuffer);
    _indexBuffer = static_cast<BufferImpl*>(buffer);
}

void RenderContextImpl::setInstanceBuffer(Buffer* buffer)
{
    if (!buffer || _instanceBuffer == buffer)
        return;
    buffer->retain();
    AX_SAFE_RELEASE(_instanceBuffer);
    _instanceBuffer = static_cast<BufferImpl*>(buffer);
}

void RenderContextImpl::prepareDrawing()
{
    AXASSERT(_programState, "ProgramState must be set before drawing");
    AXASSERT(_renderPipeline, "RenderPipelineImpl must be set before drawing");

    // Populate CPU-side uniforms via callbacks
    for (auto& cb : _programState->getCallbackUniforms())
        cb.second(_programState, cb.first);

    // Bind pipeline
    vkCmdBindPipeline(_currentCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _renderPipeline->getVkPipeline());

    // Allocate descriptor sets (set=0 UBOs, set=1 samplers)
    VkPipelineLayout pipelineLayout = _renderPipeline->getVkPipelineLayout();
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType                  = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool         = _descriptorPools[_currentFrame];
    VkDescriptorSetLayout layouts[2] = {_renderPipeline->getDescriptorSetLayout(0),
                                        _renderPipeline->getDescriptorSetLayout(1)};
    allocInfo.descriptorSetCount     = _renderPipeline->getDescriptorSetLayoutCount();
    allocInfo.pSetLayouts            = layouts;

    VkDescriptorSet descriptorSets[2];
    VkResult res = vkAllocateDescriptorSets(_device, &allocInfo, descriptorSets);
    AXASSERT(res == VK_SUCCESS, "Failed to allocate descriptor sets");

    auto& writes = _descriptorWritesPerFrame;
    writes.clear();

    VkDescriptorBufferInfo bufferInfos[2] = {};

    // --- Vertex UBO (set=0, binding=0) ---
    auto vertUB = _programState->getVertexUniformBuffer();
    if (!vertUB.empty())
    {
        // Allocate slice from per-frame ring and copy data
        UniformSlice s = allocateUniformSlice(vertUB.size());
        std::memcpy(s.cpuPtr, vertUB.data(), vertUB.size());

        VkWriteDescriptorSet& write        = writes.emplace_back();
        VkDescriptorBufferInfo& bufferInfo = bufferInfos[0];

        // Bind slice via descriptor write (UNIFORM_BUFFER with offset/range)
        bufferInfo.buffer = _uniformRings[_currentFrame].buffer;
        bufferInfo.offset = static_cast<VkDeviceSize>(s.offset);
        bufferInfo.range  = static_cast<VkDeviceSize>(vertUB.size());

        write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet          = descriptorSets[0];
        write.dstBinding      = VS_UBO_BINDING_INDEX;
        write.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo     = &bufferInfo;
    }

    // --- Fragment UBO (set=0, binding=1) ---
    auto fragUB = _programState->getFragmentUniformBuffer();
    if (!fragUB.empty())
    {
        UniformSlice s = allocateUniformSlice(fragUB.size());
        std::memcpy(s.cpuPtr, fragUB.data(), fragUB.size());

        VkWriteDescriptorSet& write        = writes.emplace_back();
        VkDescriptorBufferInfo& bufferInfo = bufferInfos[1];

        bufferInfo.buffer = _uniformRings[_currentFrame].buffer;
        bufferInfo.offset = static_cast<VkDeviceSize>(s.offset);
        bufferInfo.range  = static_cast<VkDeviceSize>(fragUB.size());

        write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet          = descriptorSets[0];
        write.dstBinding      = FS_UBO_BINDING_INDEX;
        write.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo     = &bufferInfo;
    }

    // --- Samplers (set=1, binding=N) ---
    auto& imageInfos = _descriptorImageInfosPerFrame;
    imageInfos.clear();
    for (const auto& [bindingIndex, bindingSet] : _programState->getTextureBindingSets())
    {
        auto& texs = bindingSet.texs;
        for (uint32_t k = 0; k < texs.size(); ++k)
        {
            auto textureImpl = static_cast<TextureImpl*>(texs[k]);

            auto& imageInfo       = imageInfos.emplace_back();
            imageInfo.sampler     = textureImpl->getSampler();
            imageInfo.imageView   = textureImpl->internalHandle().view;
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            VkWriteDescriptorSet& write = writes.emplace_back();
            write.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet                = descriptorSets[1];
            write.dstBinding            = bindingIndex + k;  // preserve shader binding order
            write.descriptorType        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write.descriptorCount       = 1;
            write.pImageInfo            = &imageInfo;
        }
    }

    // Commit descriptor writes
    vkUpdateDescriptorSets(_device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);

    // Bind descriptor sets (no dynamic offsets needed since we baked offset/range in descriptor writes)
    vkCmdBindDescriptorSets(_currentCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0,
                            allocInfo.descriptorSetCount, descriptorSets, 0, nullptr);

    // === Bind vertex buffers ===
    if (!_instanceBuffer)
    {
        VkBuffer buffers[]     = {_vertexBuffer->internalHandle()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(_currentCmdBuffer, 0, 1, buffers, offsets);
    }
    else
    {
        VkBuffer buffers[]     = {_vertexBuffer->internalHandle(), _instanceBuffer->internalHandle()};
        VkDeviceSize offsets[] = {0, 0};
        vkCmdBindVertexBuffers(_currentCmdBuffer, 0, 2, buffers, offsets);
    }

    // ---------- VkPipeline dynamic states -----------
    vkCmdSetViewport(_currentCmdBuffer, 0, 1, &_cachedViewport);

    if (_scissorEnabled)
        vkCmdSetScissor(_currentCmdBuffer, 0, 1, &_cachedScissor);
    else
    {
        VkRect2D fullRect{{0, 0}, {_renderTargetWidth, _renderTargetHeight}};
        vkCmdSetScissor(_currentCmdBuffer, 0, 1, &fullRect);
    }

    vkCmdSetStencilReference(_currentCmdBuffer, VK_STENCIL_FACE_FRONT_AND_BACK, _stencilReferenceValue);
    vkCmdSetCullModeEXT(_currentCmdBuffer, _cachedCullMode);
    vkCmdSetFrontFaceEXT(_currentCmdBuffer, _cachedFrontFace);
    // --------- end dynamic states ----------
}

void RenderContextImpl::drawArrays(PrimitiveType primitiveType,
                                   std::size_t start,
                                   std::size_t count,
                                   bool /*wireframe*/)
{
    prepareDrawing();
    vkCmdSetPrimitiveTopology(_currentCmdBuffer, toVkPrimitiveTopology(primitiveType));
    vkCmdDraw(_currentCmdBuffer, static_cast<uint32_t>(count), 1, static_cast<uint32_t>(start), 0);
}

void RenderContextImpl::drawArraysInstanced(PrimitiveType primitiveType,
                                            std::size_t start,
                                            std::size_t count,
                                            int instanceCount,
                                            bool /*wireframe*/)
{
    prepareDrawing();
    vkCmdSetPrimitiveTopology(_currentCmdBuffer, toVkPrimitiveTopology(primitiveType));
    vkCmdDraw(_currentCmdBuffer, static_cast<uint32_t>(count), static_cast<uint32_t>(instanceCount),
              static_cast<uint32_t>(start), 0);
}

void RenderContextImpl::drawElements(PrimitiveType primitiveType,
                                     IndexFormat indexType,
                                     std::size_t count,
                                     std::size_t offset,
                                     bool /*wireframe*/)
{
    prepareDrawing();

    AXASSERT(_indexBuffer, "Index buffer must be set for drawElements");
    VkIndexType vkIndexType = toVkIndexType(indexType);
    vkCmdBindIndexBuffer(_currentCmdBuffer, _indexBuffer->internalHandle(), 0, vkIndexType);

    vkCmdSetPrimitiveTopology(_currentCmdBuffer, toVkPrimitiveTopology(primitiveType));
    vkCmdDrawIndexed(_currentCmdBuffer, static_cast<uint32_t>(count), 1,
                     static_cast<uint32_t>(offset / (indexType == IndexFormat::U_SHORT ? 2u : 4u)), 0, 0);
}

void RenderContextImpl::drawElementsInstanced(PrimitiveType primitiveType,
                                              IndexFormat indexType,
                                              std::size_t count,
                                              std::size_t offset,
                                              int instanceCount,
                                              bool /*wireframe*/)
{
    prepareDrawing();

    AXASSERT(_indexBuffer, "Index buffer must be set for drawElementsInstanced");
    VkIndexType vkIndexType = toVkIndexType(indexType);
    vkCmdBindIndexBuffer(_currentCmdBuffer, _indexBuffer->internalHandle(), 0, vkIndexType);

    vkCmdSetPrimitiveTopology(_currentCmdBuffer, toVkPrimitiveTopology(primitiveType));
    vkCmdDrawIndexed(_currentCmdBuffer, static_cast<uint32_t>(count), static_cast<uint32_t>(instanceCount),
                     static_cast<uint32_t>(offset / (indexType == IndexFormat::U_SHORT ? 2u : 4u)), 0, 0);
}

void RenderContextImpl::readPixels(RenderTarget* rt,
                                   bool preserveAxisHint,
                                   std::function<void(const PixelBufferDesc&)> callback)
{
    AX_SAFE_RETAIN(rt);

    _postFrameOps.emplace_back([this, rt, preserveAxisHint, callback = std::move(callback)]() mutable {
        readPixelsImpl(rt, preserveAxisHint, callback);
    });
}

void RenderContextImpl::readPixelsImpl(RenderTarget* rt,
                                       bool /*preserveAxisHint*/,
                                       std::function<void(const PixelBufferDesc&)>& callback)
{
    PixelBufferDesc pbd{};
    auto* rtImpl = static_cast<RenderTargetImpl*>(rt);

    auto colorAttachment = rtImpl->getColorAttachment(0);
    if (!colorAttachment)
    {
        callback(pbd);
        return;
    }

    // ensure last rendering commands submission finished
    vkWaitForFences(_device, 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);

    auto& colorDesc = colorAttachment->getDesc();

    const uint32_t width  = colorDesc.width;
    const uint32_t height = colorDesc.height;
    const VkFormat format = UtilsVK::toVKFormat(colorDesc.pixelFormat);

    // Basic stride for RGBA8
    const uint32_t pixelStride    = 4;
    const VkDeviceSize bufferSize = static_cast<VkDeviceSize>(width) * static_cast<VkDeviceSize>(height) * pixelStride;

    // Create HOST_VISIBLE | COHERENT staging buffer
    VkBuffer stagingBuf       = VK_NULL_HANDLE;
    VkDeviceMemory stagingMem = VK_NULL_HANDLE;

    VkBufferCreateInfo bufInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufInfo.size        = bufferSize;
    bufInfo.usage       = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    AXASSERT(vkCreateBuffer(_device, &bufInfo, nullptr, &stagingBuf) == VK_SUCCESS, "vkCreateBuffer failed");

    VkMemoryRequirements memReq{};
    vkGetBufferMemoryRequirements(_device, stagingBuf, &memReq);

    uint32_t typeIndex = _driver->findMemoryType(
        memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VkMemoryAllocateInfo allocInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocInfo.allocationSize  = memReq.size;
    allocInfo.memoryTypeIndex = typeIndex;
    AXASSERT(vkAllocateMemory(_device, &allocInfo, nullptr, &stagingMem) == VK_SUCCESS, "vkAllocateMemory failed");
    AXASSERT(vkBindBufferMemory(_device, stagingBuf, stagingMem, 0) == VK_SUCCESS, "vkBindBufferMemory failed");


    auto submission = _driver->startIsolateSubmission();

    VkCommandBufferBeginInfo const binfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    const VkImage srcImage = colorAttachment->internalHandle().image;

    // Read the true current layout from tracker
    VkImageLayout currentLayout = colorAttachment->getCurrentLayout();
    if (rtImpl->isDefaultRenderTarget() && currentLayout == VK_IMAGE_LAYOUT_UNDEFINED)
        currentLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Transition to TRANSFER_SRC_OPTIMAL using TextureImpl
    colorAttachment->transitionLayout(submission, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    // Copy image -> buffer
    VkBufferImageCopy copyRegion{};
    copyRegion.bufferOffset                    = 0;
    copyRegion.bufferRowLength                 = 0;  // tightly packed
    copyRegion.bufferImageHeight               = 0;
    copyRegion.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.imageSubresource.mipLevel       = 0;
    copyRegion.imageSubresource.baseArrayLayer = 0;
    copyRegion.imageSubresource.layerCount     = 1;
    copyRegion.imageOffset                     = {0, 0, 0};
    copyRegion.imageExtent                     = {width, height, 1};

    vkCmdCopyImageToBuffer(submission, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, stagingBuf, 1,
                           &copyRegion);

    // Restore to original layout using TextureImpl
    colorAttachment->transitionLayout(submission, currentLayout);

    // Submit and wait
    _driver->finishIsolateSubmission(submission);

    // Map and copy out
    void* mapped = nullptr;
    AXASSERT(vkMapMemory(_device, stagingMem, 0, bufferSize, 0, &mapped) == VK_SUCCESS, "vkMapMemory failed");

    pbd._width  = width;
    pbd._height = height;
    pbd._data.resize(static_cast<size_t>(bufferSize));
    std::memcpy(pbd._data.data(), mapped, static_cast<size_t>(bufferSize));

    vkUnmapMemory(_device, stagingMem);

    // Cleanup
    vkDestroyBuffer(_device, stagingBuf, nullptr);
    vkFreeMemory(_device, stagingMem, nullptr);

    callback(pbd);
}

}  // namespace ax::rhi::vk
