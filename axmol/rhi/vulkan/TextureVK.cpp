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
#include "axmol/rhi/vulkan/TextureVK.h"
#include "axmol/rhi/vulkan/UtilsVK.h"
#include "axmol/rhi/vulkan/DriverVK.h"
#include "axmol/rhi/SamplerCache.h"
#include "axmol/rhi/RHIUtils.h"
#include "axmol/base/Logging.h"
#include <cassert>
#include <cstring>
#include <algorithm>
#include <vector>

namespace ax::rhi::vk
{

static constexpr uint32_t LEVEL_INITIAL_CAPS = 16;
static constexpr uint32_t LAYER_INITIAL_CAPS = 8;

static constexpr VkMemoryPriorityAllocateInfoEXT IMAGE_MEMORY_PRIORITY_ALLOC_INFO{
    .sType    = VK_STRUCTURE_TYPE_MEMORY_PRIORITY_ALLOCATE_INFO_EXT,
    .pNext    = nullptr,
    .priority = 1.0f};

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------
static bool isLargeTexture(uint32_t width, uint32_t height, uint32_t mipLevels)
{
    // Estimate texture size
    VkDeviceSize estimatedSize = width * height * 4;  // RGBA8 approximation

    // Apply mipmap factor (approx 1.33x for full mip chain)
    if (mipLevels > 1)
        estimatedSize = estimatedSize * 4 / 3;

    // Consider dedicated allocation for textures larger than 64MB
    return estimatedSize > (64 * 1024 * 1024);
}

// Check if offset is block-aligned (required by spec for compressed subresource updates).
static inline bool isBlockAligned(uint32_t x, uint32_t y, uint32_t blockW, uint32_t blockH)
{
    return (x % blockW == 0) && (y % blockH == 0);
}

// Map PixelFormat to VkFormat via UtilsVK table
static inline VkFormat getVkFormat(PixelFormat pf)
{
    const auto* info = toVKFormatInfo(pf);
    return info ? info->format : VK_FORMAT_UNDEFINED;
}

// Transition an image subresource range between layouts (auto src/dst inference)
static void transitionImageLayout(VkCommandBuffer cmd,
                                  VkImage image,
                                  VkImageLayout oldLayout,
                                  VkImageLayout newLayout,
                                  const VkImageSubresourceRange& range)
{
    VkImageMemoryBarrier barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.oldLayout           = oldLayout;
    barrier.newLayout           = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image               = image;
    barrier.subresourceRange    = range;

    VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    switch (oldLayout)
    {
    case VK_IMAGE_LAYOUT_UNDEFINED:
        barrier.srcAccessMask = 0;
        srcStage              = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        srcStage              = VK_PIPELINE_STAGE_TRANSFER_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        srcStage              = VK_PIPELINE_STAGE_TRANSFER_BIT;
        break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        srcStage              = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        break;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        srcStage              = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        srcStage              = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        break;
    default:
        barrier.srcAccessMask = 0;
        srcStage              = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        break;
    }

    switch (newLayout)
    {
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        dstStage              = VK_PIPELINE_STAGE_TRANSFER_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        dstStage              = VK_PIPELINE_STAGE_TRANSFER_BIT;
        break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dstStage              = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        break;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dstStage              = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dstStage              = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        break;
    default:
        barrier.dstAccessMask = 0;
        dstStage              = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        break;
    }

    vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void TextureHandle::destroy(DriverImpl* driver, uint64_t fenceValue)
{
    if (view != VK_NULL_HANDLE)
    {
        driver->disposeImageView(view, fenceValue);
        view = VK_NULL_HANDLE;
    }
    if (image != VK_NULL_HANDLE)
    {
        driver->disposeImage(image, fenceValue);
        image = VK_NULL_HANDLE;
    }
    if (vmaMemory != VK_NULL_HANDLE)
    {
        driver->disposeVmaMemory(vmaMemory, fenceValue);
        vmaMemory = VK_NULL_HANDLE;
    }
}

// ------------------------------------------------------------
// ctor / dtor
// ------------------------------------------------------------
TextureImpl::TextureImpl(DriverImpl* driver, const TextureDesc& desc)
    : _driver(driver), _ownResources(true), _layoutTracker(LEVEL_INITIAL_CAPS, LAYER_INITIAL_CAPS)
{
    updateTextureDesc(desc);
}

TextureImpl::TextureImpl(DriverImpl* driver, VkImage existingImage, VkImageView existingImageView)
    : _driver(driver), _ownResources(false), _layoutTracker(LEVEL_INITIAL_CAPS, LAYER_INITIAL_CAPS)
{
    _nativeTexture.image = existingImage;
    _nativeTexture.view  = existingImageView;
    // Note: existingImage is owned externally (e.g., swapchain), we only wrap it.
}

TextureImpl::~TextureImpl()
{
    if (_ownResources)
    {
        _sampler = VK_NULL_HANDLE;  // SamplerCache handles sampler destruction
        _nativeTexture.destroy(_driver, _lastFenceValue);
    }
}

void TextureImpl::transitionLayout(VkCommandBuffer cmd, VkImageLayout newLayout)
{
    ensureNativeTexture();

    const VkImageLayout oldLayout = _layoutTracker.getLayout(0, 0);

    VkImageSubresourceRange range{};
    range.aspectMask     = (_desc.pixelFormat == PixelFormat::D24S8)
                               ? (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)
                               : VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel   = 0;
    range.levelCount     = _desc.mipLevels ? _desc.mipLevels : 1;
    range.baseArrayLayer = 0;
    range.layerCount     = _desc.arraySize ? _desc.arraySize : 1;

    transitionImageLayout(cmd, _nativeTexture.image, oldLayout, newLayout, range);

    _layoutTracker.setLayout(0, 0, newLayout);
}

VkImageLayout TextureImpl::getCurrentLayout() const
{
    return _layoutTracker.getLayout(0, 0);
}

void TextureImpl::setKnownLayout(VkImageLayout layout)
{
    _layoutTracker.setLayout(0, 0, layout);
}

// ------------------------------------------------------------
// updateSamplerDesc
// ------------------------------------------------------------
void TextureImpl::updateSamplerDesc(const SamplerDesc& sampler)
{
    _desc.samplerDesc = sampler;

    _sampler = static_cast<VkSampler>(SamplerCache::getInstance()->getSampler(sampler));
    assert(_sampler && "Gets vkCreateSampler failed");
}

// ------------------------------------------------------------
// updateTextureDesc
// ------------------------------------------------------------
void TextureImpl::updateTextureDesc(const TextureDesc& desc)
{
    _desc = desc;
    Texture::updateTextureDesc(desc);
    updateSamplerDesc(desc.samplerDesc);
}

// ------------------------------------------------------------
// updateData
// ------------------------------------------------------------
void TextureImpl::updateData(const void* data, int width, int height, int level, int layerIndex)
{
    updateSubData(0, 0, width, height, level, data, layerIndex);
}

// ------------------------------------------------------------
// updateSubData
// ------------------------------------------------------------
void TextureImpl::updateSubData(int xoffset,
                                int yoffset,
                                int width,
                                int height,
                                int level,
                                const void* data,
                                int layerIndex)
{
    ensureNativeTexture();
    if (!data)
        return;

    // Compute staging upload size via rowPitch/slicePitch
    const uint32_t rowPitch       = ax::rhi::RHIUtils::computeRowPitch(_desc.pixelFormat, width);
    const uint32_t slicePitch     = rowPitch * static_cast<uint32_t>(height);
    const VkDeviceSize uploadSize = slicePitch;

    // Create staging buffer
    VkBuffer stagingBuffer{};
    VkDeviceMemory stagingMemory{};

    VkBufferCreateInfo bufInfo{};
    bufInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufInfo.size        = uploadSize;
    bufInfo.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    auto device = _driver->getDevice();

    VmaAllocation allocation{};
    VmaAllocationCreateInfo vmaAllocCreateInfo{};
    vmaAllocCreateInfo.flags =
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    vmaAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    if (_driver->isMemoryPrioritySupported())
        vmaAllocCreateInfo.priority = 1.0f;
    auto res = vmaCreateBuffer(_driver->getVmaAllocator(), &bufInfo, &vmaAllocCreateInfo, &stagingBuffer, &allocation,
                               nullptr);
    VK_REQUIRE(res, "vkCreateBuffer (staging) failed");
    res = vmaCopyMemoryToAllocation(_driver->getVmaAllocator(), data, allocation, 0, static_cast<size_t>(uploadSize));

    // Record transfer commands
    auto submission = _driver->startIsolateSubmission();

    // Transition destination subresource to TRANSFER_DST
    VkImageSubresourceRange range{};
    range.aspectMask     = (_desc.pixelFormat == PixelFormat::D24S8)
                               ? (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)
                               : VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel   = static_cast<uint32_t>(level);
    range.levelCount     = 1;
    range.baseArrayLayer = static_cast<uint32_t>(layerIndex);
    range.layerCount     = 1;

    const auto oldLayout = _layoutTracker.getLayout(level, layerIndex);

    transitionImageLayout(submission, _nativeTexture.image, oldLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, range);

    // Copy staging to image
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    // For tightly packed data we can leave bufferRowLength and bufferImageHeight as 0.
    // For non-tight or special formats you may set bufferRowLength to the texel width of a row.
    region.bufferRowLength                 = 0;
    region.bufferImageHeight               = 0;
    region.imageSubresource.aspectMask     = range.aspectMask;
    region.imageSubresource.mipLevel       = range.baseMipLevel;
    region.imageSubresource.baseArrayLayer = range.baseArrayLayer;
    region.imageSubresource.layerCount     = range.layerCount;
    region.imageOffset                     = {xoffset, yoffset, 0};
    region.imageExtent                     = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};

    vkCmdCopyBufferToImage(submission, stagingBuffer, _nativeTexture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &region);

    // Transition to SHADER_READ_ONLY for sampling
    transitionImageLayout(submission, _nativeTexture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, range);

    _layoutTracker.setLayout(level, layerIndex, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (shouldGenMipmaps(_desc.textureType == TextureType::TEXTURE_2D ? level : 0))
        generateMipmaps(submission);

    _driver->finishIsolateSubmission(submission);

    vmaDestroyBuffer(_driver->getVmaAllocator(), stagingBuffer, allocation);
}

// ------------------------------------------------------------
// generateMipmaps
// ------------------------------------------------------------
void TextureImpl::generateMipmaps(VkCommandBuffer cmd)
{
    if (TextureUsage::RENDER_TARGET == _desc.textureUsage)
        return;  // not for render target

    const uint32_t mipLevels =
        (_desc.mipLevels != 0) ? _desc.mipLevels : ax::rhi::RHIUtils::computeMipLevels(_desc.width, _desc.height);
    if (mipLevels <= 1)
        return;

    int32_t mipWidth  = _desc.width;
    int32_t mipHeight = _desc.height;
    const uint32_t layerCount =
        (_desc.textureType == TextureType::TEXTURE_CUBE) ? 6u : static_cast<uint32_t>(_desc.arraySize);

    for (uint32_t i = 1; i < mipLevels; ++i)
    {
        // Transition src level (i-1) to TRANSFER_SRC_OPTIMAL
        VkImageSubresourceRange srcRange{};
        srcRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        srcRange.baseMipLevel   = i - 1;
        srcRange.levelCount     = 1;
        srcRange.baseArrayLayer = 0;
        srcRange.layerCount     = layerCount;

        transitionImageLayout(cmd, _nativeTexture.image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                              VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, srcRange);

        // Transition dest level (i) to TRANSFER_DST_OPTIMAL
        VkImageSubresourceRange dstRange{};
        dstRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        dstRange.baseMipLevel   = i;
        dstRange.levelCount     = 1;
        dstRange.baseArrayLayer = 0;
        dstRange.layerCount     = layerCount;

        transitionImageLayout(cmd, _nativeTexture.image,
                              VK_IMAGE_LAYOUT_UNDEFINED,  // first time write
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, dstRange);

        // Setup blit region
        VkImageBlit blit{};
        blit.srcOffsets[0]                 = {0, 0, 0};
        blit.srcOffsets[1]                 = {mipWidth, mipHeight, 1};
        blit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel       = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount     = layerCount;

        blit.dstOffsets[0]                 = {0, 0, 0};
        blit.dstOffsets[1]                 = {std::max(1, mipWidth / 2), std::max(1, mipHeight / 2), 1};
        blit.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel       = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount     = layerCount;

        // Perform blit
        vkCmdBlitImage(cmd, _nativeTexture.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _nativeTexture.image,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

        // Transition dest level to SHADER_READ_ONLY_OPTIMAL
        transitionImageLayout(cmd, _nativeTexture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, dstRange);

        // Transition src level back to SHADER_READ_ONLY_OPTIMAL
        transitionImageLayout(cmd, _nativeTexture.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, srcRange);

        // Update dimensions for next mip level
        mipWidth  = std::max(1, mipWidth / 2);
        mipHeight = std::max(1, mipHeight / 2);

        _layoutTracker.setLayout(i, 0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    _overrideMipLevels = mipLevels;
}

// ------------------------------------------------------------
// updateCompressedData
// ------------------------------------------------------------
void TextureImpl::updateCompressedData(const void* data,
                                       int width,
                                       int height,
                                       std::size_t dataSize,
                                       int level,
                                       int layerIndex)
{
    ensureNativeTexture();
    if (!data || width <= 0 || height <= 0)
        return;

    updateCompressedSubData(/*xoffset*/ 0, /*yoffset*/ 0, width, height, dataSize, level, data, layerIndex);
}

// ------------------------------------------------------------
// updateCompressedSubData
// ------------------------------------------------------------
void TextureImpl::updateCompressedSubData(int xoffset,
                                          int yoffset,
                                          int width,
                                          int height,
                                          std::size_t dataSize,
                                          int level,
                                          const void* data,
                                          int layerIndex)
{
    ensureNativeTexture();
    if (!data || width <= 0 || height <= 0)
        return;

    // Query block geometry and bytes for the compressed format.
    const auto& info = ax::rhi::RHIUtils::getFormatDesc(_desc.pixelFormat);
    assert(info.blockSize > 0 && "Unsupported compressed format");

    // Check block alignment
    assert(xoffset % info.blockWidth == 0 && "xoffset must be block aligned");
    assert(yoffset % info.blockHeight == 0 && "yoffset must be block aligned");

    // Compute expected size
    uint32_t blocksX      = (width + info.blockWidth - 1) / info.blockWidth;
    uint32_t blocksY      = (height + info.blockHeight - 1) / info.blockHeight;
    uint32_t expectedSize = blocksX * blocksY * info.blockSize;
    assert(expectedSize == dataSize && "Compressed data size mismatch");

    // Vulkan requires offsets to be aligned to block dimensions for compressed updates.
    // The width/height need not be multiples of the block-size; the last partial block is allowed.
    assert(isBlockAligned(static_cast<uint32_t>(xoffset), static_cast<uint32_t>(yoffset), info.blockWidth,
                          info.blockHeight) &&
           "Compressed upload offsets must be block-aligned");

    // Create staging buffer using VMA
    VkBuffer stagingBuffer          = VK_NULL_HANDLE;
    VmaAllocation stagingAllocation = VK_NULL_HANDLE;
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size        = expectedSize;
    bufferInfo.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // Configure VMA allocation for staging buffer
    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage                   = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;      // Prefer host-visible memory
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |  // Optimized for sequential writes
                            VMA_ALLOCATION_CREATE_MAPPED_BIT;                         // Keep memory persistently mapped

    // For staging buffers, we can use lower priority since they're temporary
    if (_driver->isMemoryPrioritySupported())
    {
        allocCreateInfo.priority = 0.1f;  // Low priority for staging memory
    }

    // Use dedicated allocation for very large staging buffers
    if (expectedSize > 16 * 1024 * 1024)
    {  // 16MB threshold
        allocCreateInfo.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    }

    VkDevice device           = _driver->getDevice();
    VmaAllocator vmaAllocator = _driver->getVmaAllocator();  // Assume driver provides VMA allocator

    VkResult res =
        vmaCreateBuffer(vmaAllocator, &bufferInfo, &allocCreateInfo, &stagingBuffer, &stagingAllocation, nullptr);
    VK_REQUIRE(res, "vmaCreateBuffer (staging) failed");

    res = vmaCopyMemoryToAllocation(vmaAllocator, data, stagingAllocation, 0, expectedSize);
    VK_REQUIRE(res, "vmaCopyMemoryToAllocation failed");

    // Record commands: transition destination subresource to TRANSFER_DST_OPTIMAL, copy, then to
    // SHADER_READ_ONLY_OPTIMAL.
    auto cmd = _driver->startIsolateSubmission();

    VkImageSubresourceRange range{};
    range.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel   = static_cast<uint32_t>(level);
    range.levelCount     = 1;
    range.baseArrayLayer = static_cast<uint32_t>(layerIndex);
    range.layerCount     = 1;

    const auto oldLayout = _layoutTracker.getLayout(level, layerIndex);

    transitionImageLayout(cmd, _nativeTexture.image, oldLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, range);

    // For compressed formats, bufferRowLength and bufferImageHeight are specified in texels (not bytes).
    // Setting them to 0 means tightly packed rows according to format rules.
    VkBufferImageCopy region{};
    region.bufferOffset                    = 0;
    region.bufferRowLength                 = 0;  // tight packing in texel units
    region.bufferImageHeight               = 0;  // tight packing in texel units
    region.imageSubresource.aspectMask     = range.aspectMask;
    region.imageSubresource.mipLevel       = range.baseMipLevel;
    region.imageSubresource.baseArrayLayer = range.baseArrayLayer;
    region.imageSubresource.layerCount     = range.layerCount;
    region.imageOffset                     = {xoffset, yoffset, 0};
    region.imageExtent                     = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};

    vkCmdCopyBufferToImage(cmd, stagingBuffer, _nativeTexture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    // !!!Vulkan requires baked mipmaps data for compressed texture
    if (shouldGenMipmaps())
    {
        AXLOGW(
            "Warning: Compressed textures do not support runtime mipmap generation. "
            "Please upload precomputed mip levels instead.");
    }

    // If you will generate mipmaps next, do not transition to SHADER_READ_ONLY here. Let generateMipmaps() handle
    // transitions.
    transitionImageLayout(cmd, _nativeTexture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, range);

    _layoutTracker.setLayout(level, layerIndex, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    _driver->finishIsolateSubmission(cmd);

    // Destroy staging buffer using VMA
    vmaDestroyBuffer(vmaAllocator, stagingBuffer, stagingAllocation);
}

// ------------------------------------------------------------
// updateFaceData
// ------------------------------------------------------------
void TextureImpl::updateFaceData(TextureCubeFace side, const void* data)
{
    assert(_desc.textureType == TextureType::TEXTURE_CUBE);
    updateSubData(0, 0, _desc.width, _desc.height, 0, data, static_cast<int>(side));
}

// ------------------------------------------------------------
// ensureNativeTexture
// ------------------------------------------------------------
void TextureImpl::ensureNativeTexture()
{
    if (_nativeTexture)
        return;

    const VkFormat vkFmt = getVkFormat(_desc.pixelFormat);
    if (vkFmt == VK_FORMAT_UNDEFINED)
    {
        AXLOGE("axmol: Vulkan does not support pixel format: {}", (int)_desc.pixelFormat);
        return;
    }

    const bool isCube = (_desc.textureType == TextureType::TEXTURE_CUBE);
    const uint32_t mipLevels =
        (_desc.mipLevels != 0) ? _desc.mipLevels : ax::rhi::RHIUtils::computeMipLevels(_desc.width, _desc.height);
    const uint32_t arrayLayers = isCube ? 6u : static_cast<uint32_t>(_desc.arraySize);

    VkImageCreateInfo imageInfo{};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.flags         = isCube ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width  = _desc.width;
    imageInfo.extent.height = _desc.height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels     = mipLevels;
    imageInfo.arrayLayers   = arrayLayers;
    imageInfo.format        = vkFmt;
    imageInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    // Set image usage flags
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (_desc.textureUsage == TextureUsage::RENDER_TARGET)
    {
        // If used as render target, add color attachment usage for non-depth formats
        if (_desc.pixelFormat != PixelFormat::D24S8)
            imageInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        else
            imageInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        // Optional: if future plan to use as input attachment in subpasses
        // imageInfo.usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    }
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples     = VK_SAMPLE_COUNT_1_BIT;

    auto device       = _driver->getDevice();
    auto vmaAllocator = _driver->getVmaAllocator();  // Assume driver provides VMA allocator

    // Use VMA to create image and allocate memory in one step
    VmaAllocationCreateInfo allocCreateInfo = {};

    // Textures are typically device-local (GPU only)
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

    // Set priority for memory allocation (optional)
    if (_driver->isMemoryPrioritySupported())
    {
        // VMA 7.0+ uses priority field directly
        allocCreateInfo.priority = 1.0f;  // High priority for textures
    }

    // Optional: Dedicated allocation for large textures
    if (isLargeTexture(_desc.width, _desc.height, mipLevels))
        allocCreateInfo.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

    VmaAllocationInfo allocationInfo{};
    VkResult res = vmaCreateImage(vmaAllocator, &imageInfo, &allocCreateInfo, &_nativeTexture.image,
                                  &_nativeTexture.vmaMemory, &allocationInfo);

    VK_REQUIRE(res, "vmaCreateImage failed");

    // Create image view
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = _nativeTexture.image;

    // For cube textures, arrayLayers must be multiple of 6
    if (isCube)
    {
        assert((arrayLayers % 6u) == 0u && "Cube array layers must be multiple of 6");
        // If exactly 6 layers -> single cube, else cube array
        viewInfo.viewType = (arrayLayers == 6u) ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
    }
    else
    {
        viewInfo.viewType = (_desc.arraySize > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D);
    }

    viewInfo.format                          = vkFmt;
    viewInfo.components                      = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B,
                                                VK_COMPONENT_SWIZZLE_A};
    viewInfo.subresourceRange.aspectMask     = (_desc.pixelFormat == PixelFormat::D24S8)
                                                   ? (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)
                                                   : VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = arrayLayers;

    res = vkCreateImageView(device, &viewInfo, nullptr, &_nativeTexture.view);
    VK_REQUIRE(res, "vkCreateImageView failed");
}

}  // namespace ax::rhi::vk
