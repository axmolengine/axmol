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

static constexpr uint32_t kPreferredSwapchainImageCount = 3;

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

static constexpr VkPrimitiveTopology kPrimitiveTopologyMap[] = {
    VK_PRIMITIVE_TOPOLOGY_POINT_LIST,     // POINT
    VK_PRIMITIVE_TOPOLOGY_LINE_LIST,      // LINE
    VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,     // LINE_LOOP (Not present in Vulkan, LINE_STRIP)
    VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,     // LINE_STRIP
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,  // TRIANGLE
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP  // TRIANGLE_STRIP
};

static VkPrimitiveTopology toVkPrimitiveTopology(PrimitiveType type)
{
    if (type == PrimitiveType::LINE_LOOP) [[unlikely]]
    {
        AXLOGE("axmol-vulkan RHI doesn't support LINE_LOOP");
    }
    return kPrimitiveTopologyMap[(size_t)type];
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

inline bool nearlyEqual(float a, float b, float eps = 1e-6f)
{
    return std::fabs(a - b) < eps;
}

inline bool operator==(const VkViewport& a, const VkViewport& b)
{
    return nearlyEqual(a.x, b.x) && nearlyEqual(a.y, b.y) && nearlyEqual(a.width, b.width) &&
           nearlyEqual(a.height, b.height) && nearlyEqual(a.minDepth, b.minDepth) &&
           nearlyEqual(a.maxDepth, b.maxDepth);
}

inline bool operator==(const VkRect2D& a, const VkRect2D& b)
{
    return a.offset.x == b.offset.x && a.offset.y == b.offset.y && a.extent.width == b.extent.width &&
           a.extent.height == b.extent.height;
}

static void destroySemphores(tlx::pod_vector<VkSemaphore>& semaphores, VkDevice device)
{
    for (auto semaphore : semaphores)
        vkDestroySemaphore(device, semaphore, nullptr);
    semaphores.clear();
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

    auto& extent  = driver->getSurfaceInitialExtent();
    _screenWidth  = extent.width;
    _screenHeight = extent.height;

    _screenRT = new RenderTargetImpl(_driver, true);

    createCommandBuffers();
#if !_AX_USE_DESCRIPTOR_CACHE
    createDescriptorPool();
#endif
    recreateSwapchain();

    // create frame fence objects
    constexpr VkFenceCreateInfo fenceInfo{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                          .flags = VK_FENCE_CREATE_SIGNALED_BIT};

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateFence(_device, &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS)
        {
            AXASSERT(false, "failed to create synchronization objects for a frame!");
        }
    }

    // reserve descriptor sets
    _descriptorWritesPerFrame.reserve(16);
    _descriptorImageInfosPerFrame.reserve(16);

    // Create per-frame uniform ring buffers (capacity can be tuned)
    createUniformRingBuffers(2 * 1024 * 1024);  // 2 MB per frame
}

RenderContextImpl::~RenderContextImpl()
{
    vkDeviceWaitIdle(_device);

    AX_SAFE_RELEASE_NULL(_screenRT);
    AX_SAFE_RELEASE_NULL(_renderPipeline);

    destroyUniformRingBuffers();

    destroySemphores(_renderFinishedSemaphores, _device);
    destroySemphores(_presentCompleteSemaphores, _device);

    for (auto fence : _inFlightFences)
        vkDestroyFence(_device, fence, nullptr);
    _inFlightFences.fill({});
#if !_AX_USE_DESCRIPTOR_CACHE
    for (auto pool : _descriptorPools)
        vkDestroyDescriptorPool(_device, pool, nullptr);
    _descriptorPools.fill(VK_NULL_HANDLE);
#endif
    vkFreeCommandBuffers(_device, _commandPool, static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
    _commandBuffers.fill(VK_NULL_HANDLE);

    if (_commandPool)
    {
        vkDestroyCommandPool(_device, _commandPool, nullptr);
        _commandPool = VK_NULL_HANDLE;
    }

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
    UniformRingBuffer& ring = _uniformRings[_frameIndex];
    ring.writeHead          = 0;
}

// Allocate aligned slice from current frame's ring buffer
RenderContextImpl::UniformSlice RenderContextImpl::allocateUniformSlice(std::size_t size)
{
    UniformRingBuffer& ring = _uniformRings[_frameIndex];

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

#if !_AX_USE_DESCRIPTOR_CACHE
void RenderContextImpl::createDescriptorPool()
{
    // Define the descriptor types and counts supported by the pool
    constexpr VkDescriptorPoolSize poolSizes[] = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 64}, {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 64},
        /*{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 32},*/  // SSBO, unused currently
    };

    constexpr uint32_t MAX_DESCRIPTOR_SETS_PER_FRAME = 1024;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(std::size(poolSizes));
    poolInfo.pPoolSizes    = poolSizes;
    poolInfo.maxSets       = MAX_DESCRIPTOR_SETS_PER_FRAME;  // Maximum number of descriptor sets that can be allocated
    poolInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    // Allow individual descriptor sets to be freed for flexible management

    for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        VkResult res = vkCreateDescriptorPool(_device, &poolInfo, nullptr, &_descriptorPools[i]);
        AXASSERT(res == VK_SUCCESS, "Failed to create descriptor pool");
    }
}
#endif

bool RenderContextImpl::updateSurface(void* surface, uint32_t width, uint32_t height)
{
    if (width == _screenWidth && height == _screenHeight && _surface == surface)
        return true;

    _surface        = (VkSurfaceKHR)surface;
    _screenWidth    = width;
    _screenHeight   = height;
    _swapchainDirty = true;
    return true;
}

void RenderContextImpl::recreateSwapchain()
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

#if !defined(__ANDROID__)
    auto& contextAttrs = Application::getContextAttrs();
    if (!contextAttrs.vsync)
    {
        for (auto& pm : presentModes)
        {
            if (pm == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                chosenPresentMode = pm;
                break;
            }
        }
    }
#endif

    // Query surface capabilities
    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, _surface, &caps);

    VkSurfaceTransformFlagBitsKHR preTransform = caps.currentTransform;
#if defined(__ANDROID__)
    if (caps.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR ||
        caps.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR)
    {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
#endif

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

    const auto maxImageCount = caps.maxImageCount == 0 ? (std::numeric_limits<uint32_t>::max)() : caps.maxImageCount;
    auto preferredImageCount = std::clamp(kPreferredSwapchainImageCount, caps.minImageCount, maxImageCount);
    AXLOGI("Creating swapchain: {}x{}, format={}, presentMode={}, preferredImageCount={}", extent.width, extent.height,
           static_cast<int>(surfaceFormat.format), static_cast<int>(chosenPresentMode), preferredImageCount);

    // Destroy old swapchain and swapchain images if exists
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
    scInfo.minImageCount    = preferredImageCount;
    scInfo.imageFormat      = surfaceFormat.format;
    scInfo.imageColorSpace  = surfaceFormat.colorSpace;
    scInfo.imageExtent      = extent;
    scInfo.imageArrayLayers = 1;
    scInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    scInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    scInfo.preTransform     = preTransform;
    scInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    scInfo.presentMode      = chosenPresentMode;
    scInfo.clipped          = VK_TRUE;
    scInfo.oldSwapchain     = VK_NULL_HANDLE;

    VkResult vr = vkCreateSwapchainKHR(_device, &scInfo, nullptr, &_swapchain);
    if (vr == VK_ERROR_SURFACE_LOST_KHR)
        return;
    AXASSERT(vr == VK_SUCCESS, "vkCreateSwapchainKHR failed");

    // Retrieve swapchain images
    uint32_t swapImageCount{1};
    vkGetSwapchainImagesKHR(_device, _swapchain, &swapImageCount, nullptr);
    AXLOGI("vkGetSwapchainImagesKHR returned {} images", swapImageCount);

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

    _screenRT->rebuildSwapchainAttachments(_swapchainImages, _swapchainImageViews, extent, pixelFormat);

    // re-create render finished semaphores
    if (!_renderFinishedSemaphores.empty())
        destroySemphores(_renderFinishedSemaphores, _device);
    if (!_presentCompleteSemaphores.empty())
        destroySemphores(_presentCompleteSemaphores, _device);

    VkSemaphoreCreateInfo sci{};
    sci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    _renderFinishedSemaphores.resize(swapImageCount, VK_NULL_HANDLE);
    for (uint32_t i = 0; i < swapImageCount; ++i)
    {
        VkResult r = vkCreateSemaphore(_device, &sci, nullptr, &_renderFinishedSemaphores[i]);
        AXASSERT(r == VK_SUCCESS, "vkCreateSemaphore failed");
    }

    _presentCompleteSemaphores.resize(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        auto r = vkCreateSemaphore(_device, &sci, nullptr, &_presentCompleteSemaphores[i]);
        AXASSERT(r == VK_SUCCESS, "vkCreateSemaphore failed");
    }

    // Sync screen size
    if (_screenWidth != extent.width || _screenHeight != extent.height)
    {
        _screenWidth  = extent.width;
        _screenHeight = extent.height;
    }

    // Resets some state
    _frameIndex = 0;
    _imageIndex = 0;
    _lastError  = 0;
    _suboptimal = false;
}

void RenderContextImpl::setDepthStencilState(DepthStencilState* depthStencilState)
{
    _depthStencilState = static_cast<DepthStencilStateImpl*>(depthStencilState);
}

void RenderContextImpl::setRenderPipeline(RenderPipeline* renderPipeline)
{
    Object::assign(_renderPipeline, static_cast<RenderPipelineImpl*>(renderPipeline));
}

uint64_t RenderContextImpl::getCompletedFenceValue() const
{
    return _completedFenceValue;
}

bool RenderContextImpl::beginFrame()
{
    if (_swapchainDirty) [[unlikely]]
    {
        vkDeviceWaitIdle(_device);
        recreateSwapchain();
        static_cast<RenderTargetImpl*>(_screenRT)->invalidate();
        _swapchainDirty = false;
    }

    if (_lastError)
        return false;  // if error not cleared, skip frame

    // wait for previous frame to finish
    auto& currentFence = _inFlightFences[_frameIndex];
    vkWaitForFences(_device, 1, &currentFence, VK_TRUE, UINT64_MAX);
    vkResetFences(_device, 1, &currentFence);

    _completedFenceValue = currentFence.fenceValue;
    _driver->processDisposalQueue(_completedFenceValue);

    currentFence.fenceValue = ++_frameFenceValue;

    // Reset uniform ring write head for this frame
    resetUniformRingForCurrentFrame();

    const auto maxImageIndex = static_cast<uint32_t>(_swapchainImages.size());

    VkResult result = vkAcquireNextImageKHR(_device, _swapchain, UINT64_MAX, _presentCompleteSemaphores[_frameIndex],
                                            VK_NULL_HANDLE, &_imageIndex);
    if (!handleSwapchainResult(result, SwapchainOp::Acquire, _frameIndex))
        return false;

    AXASSERT(_imageIndex < maxImageIndex, "swapchain image index out of range!");

    _inFrame = true;

    _currentCmdBuffer = _commandBuffers[_frameIndex];
    vkResetCommandBuffer(_currentCmdBuffer, 0);

#if _AX_USE_DESCRIPTOR_CACHE
    auto& descriptorStates = _inFlightDescriptorStates[_frameIndex];
    for (auto& state : descriptorStates)
        _renderPipeline->recycleDescriptorState(state);
    descriptorStates.clear();
#else
    auto descriptorPool = _descriptorPools[_frameIndex];
    vkResetDescriptorPool(_device, descriptorPool, 0);  // safe: only reset current frame pool
#endif

    VkCommandBufferBeginInfo const binfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    VkResult vr = vkBeginCommandBuffer(_currentCmdBuffer, &binfo);
    AXASSERT(vr == VK_SUCCESS, "vkBeginCommandBuffer failed");

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
    rtImpl->beginRenderPass(_currentCmdBuffer, renderPassDesc, _renderTargetWidth, _renderTargetHeight, _imageIndex);
}

void RenderContextImpl::endRenderPass()
{
    auto rtImpl = static_cast<RenderTargetImpl*>(_currentRT);
    rtImpl->endRenderPass(_currentCmdBuffer);

    // Reset state cache
    _programState  = nullptr;
    _vertexLayout  = nullptr;
    _boundPipeline = nullptr;

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
        UniformRingBuffer& ring = _uniformRings[_frameIndex];
        if (!ring.isCoherent && ring.writeHead > 0)
        {
            VkMappedMemoryRange range{VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE};
            range.memory = ring.memory;
            range.offset = 0;
            range.size   = ring.writeHead;
            vkFlushMappedMemoryRanges(_device, 1, &range);
        }
    }

    const VkPipelineStageFlags waitDestinationStageMask{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    // New render-finished semaphore from pool
    VkSemaphore submissionSemaphore = _renderFinishedSemaphores[_imageIndex];

    VkSubmitInfo submitInfo{};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &_currentCmdBuffer;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = &_presentCompleteSemaphores[_frameIndex];
    submitInfo.pWaitDstStageMask    = &waitDestinationStageMask;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = &submissionSemaphore;

    vr = vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFences[_frameIndex]);

    AXASSERT(vr == VK_SUCCESS, "vkQueueSubmit failed");

    // Present: wait on render-finished semaphore
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &submissionSemaphore;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &_swapchain;
    presentInfo.pImageIndices      = &_imageIndex;

    vr = vkQueuePresentKHR(_presentQueue, &presentInfo);
    handleSwapchainResult(vr, SwapchainOp::Present, 0);

    {
        for (auto& op : _postFrameOps)
            op();

        _postFrameOps.clear();
    }

    // Advance frame index for multi-frame-in-flight
    _frameIndex = (_frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;

    _driver->setFrameIndex(_frameIndex);

    _inFrame = false;
}

bool RenderContextImpl::handleSwapchainResult(VkResult result, SwapchainOp op, uint32_t frameIndex)
{
    if (result == VK_SUCCESS)
        return true;

    switch (result)
    {
    case VK_SUBOPTIMAL_KHR:
        if (!_suboptimal)
        {
            _suboptimal = true;
            if (op == SwapchainOp::Present)
                AXLOGW("axmol: vkQueuePresentKHR suboptimal swap chain.");
            else
                AXLOGW("axmol: vkAcquireNextImageKHR suboptimal swap chain.");
        }
        return true;

    case VK_ERROR_OUT_OF_DATE_KHR:
        if (op == SwapchainOp::Present)
        {
            AXLOGI("vkQueuePresentKHR: swapchain out of date when present");
        }
        else
        {
            AXLOGI("vkAcquireNextImageKHR: swapchain out of date");
            //_currentImageReadyIndex = prevSemaphoreIndex;  // revert
        }
        break;

    case VK_ERROR_SURFACE_LOST_KHR:
        if (op == SwapchainOp::Present)
        {
            AXLOGI("vkQueuePresentKHR: surface lost");
        }
        else
        {
            AXLOGI("vkAcquireNextImageKHR: surface lost");
            //_currentImageReadyIndex = prevSemaphoreIndex;  // revert
        }
        break;

    default:
        if (op == SwapchainOp::Present)
        {
            AXLOGE("axmol: vkQueuePresentKHR fail: {}", (unsigned int)result);
            AXASSERT(result && false, "vkQueuePresentKHR failed");
        }
        else
        {
            AXLOGE("axmol: vkAcquireNextImageKHR fail: {}", (unsigned int)result);
            AXASSERT(false, "vkAcquireNextImageKHR failed");
        }
        break;
    }
    _lastError = result;

    return false;
}

void RenderContextImpl::updateDepthStencilState(const DepthStencilDesc& desc)
{
    AXASSERT(_depthStencilState, "DepthStencilStateImpl not set");
    _depthStencilState->update(desc);
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

    if (vp != _cachedViewport)
    {
        _cachedViewport = vp;
        markDynamicStateDirty(DynamicStateBits::Viewport);
    }
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

    if (_scissorEnabled != isEnabled || _cachedScissor != rect)
    {
        _scissorEnabled = isEnabled;
        _cachedScissor  = rect;
        markDynamicStateDirty(DynamicStateBits::Scissor);
    }
}

void RenderContextImpl::setStencilReferenceValue(uint32_t value)
{
    if (value != _stencilReferenceValue)
    {
        RenderContext::setStencilReferenceValue(value);
        markDynamicStateDirty(DynamicStateBits::StencilRef);
    }
}

void RenderContextImpl::setCullMode(CullMode mode)
{
    VkCullModeFlags nativeMode{0};
    switch (mode)
    {
    case CullMode::NONE:
        nativeMode = VK_CULL_MODE_NONE;
        break;
    case CullMode::BACK:
        nativeMode = VK_CULL_MODE_BACK_BIT;
        break;
    case CullMode::FRONT:
        nativeMode = VK_CULL_MODE_FRONT_BIT;
        break;
    }

    if (_extendedDynamicState.cullMode != nativeMode)
    {
        _extendedDynamicState.cullMode = nativeMode;
        markExtendedDynamicStateDirty(ExtendedDynamicStateBits::CullMode);
    }
}

void RenderContextImpl::setWinding(Winding winding)
{
    VkFrontFace frontFace = VkFrontFace::VK_FRONT_FACE_MAX_ENUM;
    switch (winding)
    {
    case Winding::CLOCK_WISE:
        frontFace = VK_FRONT_FACE_CLOCKWISE;
        break;
    case Winding::COUNTER_CLOCK_WISE:
        frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        break;
    }

    if (_extendedDynamicState.frontFace != frontFace)
    {
        _extendedDynamicState.frontFace = frontFace;
        markExtendedDynamicStateDirty(ExtendedDynamicStateBits::FrontFace);
    }
}

void RenderContextImpl::markExtendedDynamicStateDirty(ExtendedDynamicStateBits bits) noexcept
{
    if (_driver->isExtendedDynamicStateSupported())
    {
        auto&& apply = [this, bits]<std::size_t... _Idx>(std::index_sequence<_Idx...>) {
            (bitmask::set(_inFlightExtendedDynamicDirtyBits[_Idx], bits), ...);
        };
        apply(std::make_index_sequence<MAX_FRAMES_IN_FLIGHT>{});
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

void RenderContextImpl::updatePipelineState(const RenderTarget* rt,
                                            const PipelineDesc& desc,
                                            PrimitiveType primitiveType)
{
    auto primitiveTopology = toVkPrimitiveTopology(primitiveType);
    if (_extendedDynamicState.primitiveTopology != primitiveTopology)
    {
        _extendedDynamicState.primitiveTopology = primitiveTopology;
        markExtendedDynamicStateDirty(ExtendedDynamicStateBits::PrimitiveTopology);
    }

    RenderContext::updatePipelineState(rt, desc, primitiveType);
    AXASSERT(_renderPipeline, "RenderPipelineImpl not set");
    _renderPipeline->prepareUpdate(_depthStencilState);
    _renderPipeline->update(rt, desc, _extendedDynamicState);

    // Bind pipeline
    auto pipeline = _renderPipeline->getVkPipeline();
    if (_boundPipeline != pipeline)
    {
        vkCmdBindPipeline(_currentCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        _boundPipeline = pipeline;
        bitmask::set(_inFlightDynamicDirtyBits[_frameIndex], PIPELINE_ALL_DYNAMIC_BITS);

        if (_driver->isExtendedDynamicStateSupported())
            bitmask::set(_inFlightExtendedDynamicDirtyBits[_frameIndex], PIPELINE_ALL_EXTENDED_DYNAMIC_BITS);
    }
}

void RenderContextImpl::applyPendingDynamicStates()
{
    // Viewport
    if (bitmask::any(_inFlightDynamicDirtyBits[_frameIndex], DynamicStateBits::Viewport))
    {
        vkCmdSetViewport(_currentCmdBuffer, 0, 1, &_cachedViewport);
        bitmask::clear(_inFlightDynamicDirtyBits[_frameIndex], DynamicStateBits::Viewport);
    }

    // Scissor
    if (bitmask::any(_inFlightDynamicDirtyBits[_frameIndex], DynamicStateBits::Scissor))
    {
        vkCmdSetScissor(_currentCmdBuffer, 0, 1, &_cachedScissor);
        bitmask::clear(_inFlightDynamicDirtyBits[_frameIndex], DynamicStateBits::Scissor);
    }

    // Stencil reference if used
    if (bitmask::any(_inFlightDynamicDirtyBits[_frameIndex], DynamicStateBits::StencilRef))
    {
        vkCmdSetStencilReference(_currentCmdBuffer, VK_STENCIL_FRONT_AND_BACK, _stencilReferenceValue);
        bitmask::clear(_inFlightDynamicDirtyBits[_frameIndex], DynamicStateBits::StencilRef);
    }

    if (_driver->isExtendedDynamicStateSupported())
    {
        // CullMode and FrontFace via EXT dynamic state if available
        if (bitmask::any(_inFlightExtendedDynamicDirtyBits[_frameIndex], ExtendedDynamicStateBits::CullMode))
        {
            vkCmdSetCullModeEXT(_currentCmdBuffer, _extendedDynamicState.cullMode);
            bitmask::clear(_inFlightExtendedDynamicDirtyBits[_frameIndex], ExtendedDynamicStateBits::CullMode);
        }
        if (bitmask::any(_inFlightExtendedDynamicDirtyBits[_frameIndex], ExtendedDynamicStateBits::FrontFace))
        {
            vkCmdSetFrontFaceEXT(_currentCmdBuffer, _extendedDynamicState.frontFace);
            bitmask::clear(_inFlightExtendedDynamicDirtyBits[_frameIndex], ExtendedDynamicStateBits::FrontFace);
        }

        if (bitmask::any(_inFlightExtendedDynamicDirtyBits[_frameIndex], ExtendedDynamicStateBits::PrimitiveTopology))
        {
            vkCmdSetPrimitiveTopologyEXT(_currentCmdBuffer, _extendedDynamicState.primitiveTopology);
            bitmask::clear(_inFlightExtendedDynamicDirtyBits[_frameIndex], ExtendedDynamicStateBits::PrimitiveTopology);
        }
    }
}

void RenderContextImpl::removeCachedPipelines(VkRenderPass rp)
{
    if (_renderPipeline)
        _renderPipeline->removeCachedPipelines(rp);
}

void RenderContextImpl::prepareDrawing()
{
    AXASSERT(_programState, "ProgramState must be set before drawing");
    AXASSERT(_renderPipeline, "RenderPipelineImpl must be set before drawing");
    AXASSERT(_boundPipeline, "boundPipeline must be set before drawing");

    applyPendingDynamicStates();

    // Populate CPU-side uniforms via callbacks
    for (auto& cb : _programState->getCallbackUniforms())
        cb.second(_programState, cb.first);

    // Acquire descriptor sets for this frame, matching current pipeline layout
    VkPipelineLayout pipelineLayout = _renderPipeline->getVkPipelineLayout();
    const auto dslState             = _renderPipeline->getDescriptorSetLayoutState();

#if _AX_USE_DESCRIPTOR_CACHE
    auto& descriptorState = _inFlightDescriptorStates[_frameIndex].emplace_back();
    bool ok               = _renderPipeline->acquireDescriptorState(descriptorState, _frameIndex);
    AXASSERT(ok, "Failed to acquire descriptor sets");
    auto& descriptorSets = descriptorState.sets;
#else
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType               = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool      = _descriptorPools[_frameIndex];
    auto descriptorSetLayoutState = _renderPipeline->getDescriptorSetLayoutState();
    allocInfo.descriptorSetCount  = descriptorSetLayoutState->descriptorSetLayoutCount;
    allocInfo.pSetLayouts         = descriptorSetLayoutState->descriptorSetLayouts.data();

    std::array<VkDescriptorSet, RenderPipelineImpl::MAX_DESCRIPTOR_SETS> descriptorSets{};
    VkResult res = vkAllocateDescriptorSets(_device, &allocInfo, descriptorSets.data());
    AXASSERT(res == VK_SUCCESS, "Failed to allocate descriptor sets");
#endif

    assert(descriptorSets[RenderPipelineImpl::SET_INDEX_UBO]);

    // Prepare write lists sized to expected UBO + sampler descriptors
    auto& writes = _descriptorWritesPerFrame;
    writes.clear();
    writes.reserve(dslState->uniformDescriptorCount + dslState->samplerDescriptorCount);

    _descriptorBufferInfos.clear();

    auto& cpuBuffer = _programState->getUniformBuffer();
    if (!cpuBuffer.empty())
    {
        auto bufferPtr = cpuBuffer.data();
        for (auto& uboInfo : _programState->getActiveUniformBlockInfos())
        {
            UniformSlice s = allocateUniformSlice(uboInfo.sizeBytes);
            std::memcpy(s.cpuPtr, bufferPtr + uboInfo.cpuOffset, uboInfo.sizeBytes);

            VkWriteDescriptorSet& write        = writes.emplace_back();
            VkDescriptorBufferInfo& bufferInfo = _descriptorBufferInfos.emplace_back();

            bufferInfo.buffer = _uniformRings[_frameIndex].buffer;
            bufferInfo.offset = static_cast<VkDeviceSize>(s.offset);
            bufferInfo.range  = static_cast<VkDeviceSize>(uboInfo.sizeBytes);

            write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet          = descriptorSets[RenderPipelineImpl::SET_INDEX_UBO];  // renamed index
            write.dstBinding      = uboInfo.binding;
            write.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.descriptorCount = 1;
            write.pBufferInfo     = &bufferInfo;
        }
    }

    // --- Samplers (set=1, binding=N) ---
    auto& imageInfos = _descriptorImageInfosPerFrame;
    imageInfos.clear();
    imageInfos.reserve(dslState->samplerDescriptorCount);

    for (const auto& [bindingIndex, bindingSet] : _programState->getTextureBindingSets())
    {
        const auto& texs = bindingSet.texs;
        if (texs.empty())
            continue;

        // Remember current offset in imageInfos before appending
        const size_t offset = imageInfos.size();

        if (texs.size() == 1)
        {
            auto textureImpl      = static_cast<TextureImpl*>(texs[0]);
            auto& imageInfo       = imageInfos.emplace_back();
            imageInfo.sampler     = textureImpl->getSampler();
            imageInfo.imageView   = textureImpl->internalHandle().view;
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            textureImpl->setLastFenceValue(_frameFenceValue);
        }
        else
        {
            for (auto tex : texs)
            {
                auto textureImpl      = static_cast<TextureImpl*>(tex);
                auto& imageInfo       = imageInfos.emplace_back();
                imageInfo.sampler     = textureImpl->getSampler();
                imageInfo.imageView   = textureImpl->internalHandle().view;
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                textureImpl->setLastFenceValue(_frameFenceValue);
            }
        }

        VkWriteDescriptorSet& write = writes.emplace_back();
        write.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet                = descriptorSets[RenderPipelineImpl::SET_INDEX_SAMPLER];
        write.dstBinding            = bindingIndex;
        write.descriptorType        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount       = static_cast<uint32_t>(texs.size());
        write.pImageInfo            = imageInfos.data() + offset;
    }

    // Commit descriptor writes
    if (!writes.empty())
    {
        vkUpdateDescriptorSets(_device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    }

    // Bind descriptor sets: bind only the sets that exist
    vkCmdBindDescriptorSets(_currentCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0,
                            dslState->descriptorSetLayoutCount, descriptorSets.data(), 0, nullptr);

    // Bind vertex buffers
    _vertexBuffer->setLastFenceValue(_frameFenceValue);
    if (!_instanceBuffer)
    {
        VkBuffer buffers[]     = {_vertexBuffer->internalHandle()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(_currentCmdBuffer, 0, 1, buffers, offsets);
    }
    else
    {
        _instanceBuffer->setLastFenceValue(_frameFenceValue);
        VkBuffer buffers[]     = {_vertexBuffer->internalHandle(), _instanceBuffer->internalHandle()};
        VkDeviceSize offsets[] = {0, 0};
        vkCmdBindVertexBuffers(_currentCmdBuffer, 0, 2, buffers, offsets);
    }
}

void RenderContextImpl::drawArrays(std::size_t start, std::size_t count, bool /*wireframe*/)
{
    prepareDrawing();
    vkCmdDraw(_currentCmdBuffer, static_cast<uint32_t>(count), 1, static_cast<uint32_t>(start), 0);
}

void RenderContextImpl::drawArraysInstanced(std::size_t start, std::size_t count, int instanceCount, bool /*wireframe*/)
{
    prepareDrawing();
    vkCmdDraw(_currentCmdBuffer, static_cast<uint32_t>(count), static_cast<uint32_t>(instanceCount),
              static_cast<uint32_t>(start), 0);
}

void RenderContextImpl::drawElements(IndexFormat indexType, std::size_t count, std::size_t offset, bool /*wireframe*/)
{
    prepareDrawing();

    AXASSERT(_indexBuffer, "Index buffer must be set for drawElements");
    _indexBuffer->setLastFenceValue(_frameFenceValue);
    VkIndexType vkIndexType = toVkIndexType(indexType);
    vkCmdBindIndexBuffer(_currentCmdBuffer, _indexBuffer->internalHandle(), 0, vkIndexType);
    vkCmdDrawIndexed(_currentCmdBuffer, static_cast<uint32_t>(count), 1,
                     static_cast<uint32_t>(offset / (indexType == IndexFormat::U_SHORT ? 2u : 4u)), 0, 0);
}

void RenderContextImpl::drawElementsInstanced(IndexFormat indexType,
                                              std::size_t count,
                                              std::size_t offset,
                                              int instanceCount,
                                              bool /*wireframe*/)
{
    prepareDrawing();

    AXASSERT(_indexBuffer, "Index buffer must be set for drawElementsInstanced");
    _indexBuffer->setLastFenceValue(_frameFenceValue);
    VkIndexType vkIndexType = toVkIndexType(indexType);
    vkCmdBindIndexBuffer(_currentCmdBuffer, _indexBuffer->internalHandle(), 0, vkIndexType);
    vkCmdDrawIndexed(_currentCmdBuffer, static_cast<uint32_t>(count), static_cast<uint32_t>(instanceCount),
                     static_cast<uint32_t>(offset / (indexType == IndexFormat::U_SHORT ? 2u : 4u)), 0, 0);
}

void RenderContextImpl::readPixels(RenderTarget* rt,
                                   bool preserveAxisHint,
                                   std::function<void(const PixelBufferDesc&)> callback)
{
    if (!rt)
    {
        callback({});
        return;
    }
    rt->retain();

    _postFrameOps.emplace_back([this, rt, preserveAxisHint, callback = std::move(callback)]() mutable {
        readPixelsInternal(rt, preserveAxisHint, callback);
        rt->release();
    });
}

void RenderContextImpl::readPixelsInternal(RenderTarget* rt,
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
    vkWaitForFences(_device, 1, &_inFlightFences[_frameIndex], VK_TRUE, UINT64_MAX);

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

    vkCmdCopyImageToBuffer(submission, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, stagingBuf, 1, &copyRegion);

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
