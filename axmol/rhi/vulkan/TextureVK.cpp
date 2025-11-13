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
#include "TextureVK.h"
#include "UtilsVK.h"
#include "axmol/rhi/vulkan/DriverVK.h"
#include "axmol/rhi/SamplerCache.h"
#include "axmol/rhi/RHIUtils.h"
#include "axmol/base/Logging.h"
#include <glad/vulkan.h>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <vector>

namespace ax::rhi::vk
{

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

// Map PixelFormat to VkFormat via UtilsVK table
static inline VkFormat getVkFormat(PixelFormat pf)
{
    const auto* info = toVKFormatInfo(pf);
    return info ? info->format : VK_FORMAT_UNDEFINED;
}

// Transition an image subresource range between layouts
static void transitionImageLayout(VkCommandBuffer cmd,
                                  VkImage image,
                                  VkImageLayout oldLayout,
                                  VkImageLayout newLayout,
                                  const VkImageSubresourceRange& range)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
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
    default:
        srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
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
    default:
        dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        break;
    }

    vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

// ------------------------------------------------------------
// ctor / dtor
// ------------------------------------------------------------
TextureImpl::TextureImpl(DriverImpl* driver, const TextureDesc& desc) : _driver(driver), _ownResources(true)
{
    updateTextureDesc(desc);
}

TextureImpl::TextureImpl(DriverImpl* driver, VkImage existingImage, VkImageView existingImageView)
    : _driver(driver), _ownResources(false)
{
    _nativeTexture.image = existingImage;
    _nativeTexture.view  = existingImageView;
    // Note: existingImage is owned externally (e.g., swapchain), we only wrap it.
}

TextureImpl::~TextureImpl()
{
    if (_ownResources)
    {
        auto device = _driver->getDevice();
        _sampler    = VK_NULL_HANDLE;  // SamplerCache handles sampler destruction
        _nativeTexture.destroy(device);
    }
}

// ------------------------------------------------------------
// updateSamplerDesc
// ------------------------------------------------------------
void TextureImpl::updateSamplerDesc(const SamplerDesc& sampler)
{
    _desc.samplerDesc = sampler;

    _sampler = (VkSampler)SamplerCache::getInstance()->getSampler(sampler);
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
// updateData / updateCompressedData
// ------------------------------------------------------------
void TextureImpl::updateData(const void* data, int width, int height, int level, int layerIndex)
{
    updateSubData(0, 0, width, height, level, data, layerIndex);
}

void TextureImpl::updateCompressedData(const void* data,
                                       int width,
                                       int height,
                                       std::size_t dataSize,
                                       int level,
                                       int layerIndex)
{
    // For simplicity, treat compressed data similarly; for BC/ASTC use block-aware copy if needed.
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

    auto device  = _driver->getDevice();
    VkResult res = vkCreateBuffer(device, &bufInfo, nullptr, &stagingBuffer);
    assert(res == VK_SUCCESS && "vkCreateBuffer (staging) failed");

    VkMemoryRequirements bufMemReq{};
    vkGetBufferMemoryRequirements(device, stagingBuffer, &bufMemReq);

    VkMemoryAllocateInfo bufAlloc{};
    bufAlloc.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    bufAlloc.allocationSize  = bufMemReq.size;
    bufAlloc.memoryTypeIndex = _driver->findMemoryType(
        bufMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    res = vkAllocateMemory(device, &bufAlloc, nullptr, &stagingMemory);
    assert(res == VK_SUCCESS && "vkAllocateMemory (staging) failed");

    vkBindBufferMemory(device, stagingBuffer, stagingMemory, 0);

    // Copy user data into staging buffer
    void* mapped = nullptr;
    res          = vkMapMemory(device, stagingMemory, 0, uploadSize, 0, &mapped);
    assert(res == VK_SUCCESS && "vkMapMemory failed");
    std::memcpy(mapped, data, static_cast<size_t>(uploadSize));
    vkUnmapMemory(device, stagingMemory);

    // Record transfer commands
    VkCommandPool pool{};
    VkQueue queue{};
    VkCommandBuffer cmd = _driver->beginIsolateCommands();

    // Transition destination subresource to TRANSFER_DST
    VkImageSubresourceRange range{};
    range.aspectMask     = (_desc.pixelFormat == PixelFormat::D24S8)
                               ? (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)
                               : VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel   = static_cast<uint32_t>(level);
    range.levelCount     = 1;
    range.baseArrayLayer = static_cast<uint32_t>(layerIndex);
    range.layerCount     = 1;

    // Determine old layout: if we've uploaded before, assume SHADER_READ_ONLY, else UNDEFINED
    // if (!_initialized)
    //{
    //    transitionImageLayout(cmd, _nativeTexture.image, VK_IMAGE_LAYOUT_UNDEFINED,
    //                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, range);
    //    _initialized = true;
    //}

    transitionImageLayout(cmd, _nativeTexture.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          range);

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

    vkCmdCopyBufferToImage(cmd, stagingBuffer, _nativeTexture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    // Transition to SHADER_READ_ONLY for sampling
    transitionImageLayout(cmd, _nativeTexture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, range);

    _driver->endIsolateCommands(cmd);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingMemory, nullptr);

    // Optional: generate mipmaps here with a persistent cmd buffer; skipped in this one-time path.
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
    // This simplified path treats compressed like raw; for block-compressed, upload by blocks if needed.
    updateSubData(xoffset, yoffset, width, height, level, data, layerIndex);
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

    auto device   = _driver->getDevice();

    VkResult res = vkCreateImage(device, &imageInfo, nullptr, &_nativeTexture.image);
    assert(res == VK_SUCCESS && "vkCreateImage failed");

    VkMemoryRequirements memReq{};
    vkGetImageMemoryRequirements(device, _nativeTexture.image, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memReq.size;
    allocInfo.memoryTypeIndex = _driver->findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    res = vkAllocateMemory(device, &allocInfo, nullptr, &_nativeTexture.memory);
    assert(res == VK_SUCCESS && "vkAllocateMemory failed");

    vkBindImageMemory(device, _nativeTexture.image, _nativeTexture.memory, 0);

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
    assert(res == VK_SUCCESS && "vkCreateImageView failed");
}

// ------------------------------------------------------------
// generateMipmaps
// ------------------------------------------------------------
void TextureImpl::generateMipmaps(VkCommandBuffer cmd)
{
    if (_desc.pixelFormat == PixelFormat::D24S8)
        return;  // not for depth-stencil
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
        // Transition dest level to TRANSFER_DST
        VkImageSubresourceRange dstRange{};
        dstRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        dstRange.baseMipLevel   = i;
        dstRange.levelCount     = 1;
        dstRange.baseArrayLayer = 0;
        dstRange.layerCount     = layerCount;

        transitionImageLayout(cmd, _nativeTexture.image,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,  // if previously sampled
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, dstRange);

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

        vkCmdBlitImage(cmd, _nativeTexture.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _nativeTexture.image,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

        // Transition dest level to SHADER_READ
        transitionImageLayout(cmd, _nativeTexture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, dstRange);

        mipWidth  = std::max(1, mipWidth / 2);
        mipHeight = std::max(1, mipHeight / 2);
    }
}

}  // namespace ax::rhi::vk
