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
#include "axmol/rhi/RHIUtils.h"
#include "axmol/rhi/vulkan/UtilsVK.h"
#include "axmol/rhi/vulkan/DriverVK.h"
#include "axmol/base/Logging.h"
#include <glad/vulkan.h>
#include <cassert>
#include <cstdint>

namespace ax::rhi::vk
{

// -------------------------------------------------------------------------------------------------
// PixelFormatInfo mapping table (array style, aligned with PixelFormat enum order)
// -------------------------------------------------------------------------------------------------
static PixelFormatInfo s_pixelFormatInfos[] = {
    /* ---- format ----------------------- fmtSrv ----------------------- fmtDsv --------------------- fmtSrgb
       -------------------*/
    /* pvrtc v1 ------------------------------------------------------------ */
    {VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED},  // PVRTC4
    {VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED},  // PVRTC4A
    {VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED},  // PVRTC2
    {VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED},  // PVRTC2A

    /* etc ----------------------------------------------------------------- */
    {VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED},  // ETC1
    {VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED},  // ETC2
    {VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED},  // ETC2A

    /* s3tc / bc ----------------------------------------------------------- */
    {VK_FORMAT_BC1_RGBA_UNORM_BLOCK, VK_FORMAT_BC1_RGBA_UNORM_BLOCK, VK_FORMAT_UNDEFINED,
     VK_FORMAT_BC1_RGBA_SRGB_BLOCK},                                                                        // S3TC_DXT1
    {VK_FORMAT_BC2_UNORM_BLOCK, VK_FORMAT_BC2_UNORM_BLOCK, VK_FORMAT_UNDEFINED, VK_FORMAT_BC2_SRGB_BLOCK},  // S3TC_DXT3
    {VK_FORMAT_BC3_UNORM_BLOCK, VK_FORMAT_BC3_UNORM_BLOCK, VK_FORMAT_UNDEFINED, VK_FORMAT_BC3_SRGB_BLOCK},  // S3TC_DXT5

    /* atc ----------------------------------------------------------------- */
    {VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED},  // ATC
    {VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED},  // ATCE
    {VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED},  // ATCI

    /* astc ---------------------------------------------------------------- */
    {VK_FORMAT_ASTC_4x4_UNORM_BLOCK, VK_FORMAT_ASTC_4x4_UNORM_BLOCK, VK_FORMAT_UNDEFINED,
     VK_FORMAT_ASTC_4x4_SRGB_BLOCK},  // ASTC4×4
    {VK_FORMAT_ASTC_5x4_UNORM_BLOCK, VK_FORMAT_ASTC_5x4_UNORM_BLOCK, VK_FORMAT_UNDEFINED,
     VK_FORMAT_ASTC_5x4_SRGB_BLOCK},  // ASTC5×4
    {VK_FORMAT_ASTC_5x5_UNORM_BLOCK, VK_FORMAT_ASTC_5x5_UNORM_BLOCK, VK_FORMAT_UNDEFINED,
     VK_FORMAT_ASTC_5x5_SRGB_BLOCK},  // ASTC5×5
    {VK_FORMAT_ASTC_6x5_UNORM_BLOCK, VK_FORMAT_ASTC_6x5_UNORM_BLOCK, VK_FORMAT_UNDEFINED,
     VK_FORMAT_ASTC_6x5_SRGB_BLOCK},  // ASTC6×5
    {VK_FORMAT_ASTC_6x6_UNORM_BLOCK, VK_FORMAT_ASTC_6x6_UNORM_BLOCK, VK_FORMAT_UNDEFINED,
     VK_FORMAT_ASTC_6x6_SRGB_BLOCK},  // ASTC6×6
    {VK_FORMAT_ASTC_8x5_UNORM_BLOCK, VK_FORMAT_ASTC_8x5_UNORM_BLOCK, VK_FORMAT_UNDEFINED,
     VK_FORMAT_ASTC_8x5_SRGB_BLOCK},  // ASTC8×5
    {VK_FORMAT_ASTC_8x6_UNORM_BLOCK, VK_FORMAT_ASTC_8x6_UNORM_BLOCK, VK_FORMAT_UNDEFINED,
     VK_FORMAT_ASTC_8x6_SRGB_BLOCK},  // ASTC8×6
    {VK_FORMAT_ASTC_8x8_UNORM_BLOCK, VK_FORMAT_ASTC_8x8_UNORM_BLOCK, VK_FORMAT_UNDEFINED,
     VK_FORMAT_ASTC_8x8_SRGB_BLOCK},  // ASTC8×8
    {VK_FORMAT_ASTC_10x5_UNORM_BLOCK, VK_FORMAT_ASTC_10x5_UNORM_BLOCK, VK_FORMAT_UNDEFINED,
     VK_FORMAT_ASTC_10x5_SRGB_BLOCK},  // ASTC10×5
    {VK_FORMAT_ASTC_10x6_UNORM_BLOCK, VK_FORMAT_ASTC_10x6_UNORM_BLOCK, VK_FORMAT_UNDEFINED,
     VK_FORMAT_ASTC_10x6_SRGB_BLOCK},  // ASTC10×6
    {VK_FORMAT_ASTC_10x8_UNORM_BLOCK, VK_FORMAT_ASTC_10x8_UNORM_BLOCK, VK_FORMAT_UNDEFINED,
     VK_FORMAT_ASTC_10x8_SRGB_BLOCK},  // ASTC10×8
    {VK_FORMAT_ASTC_10x10_UNORM_BLOCK, VK_FORMAT_ASTC_10x10_UNORM_BLOCK, VK_FORMAT_UNDEFINED,
     VK_FORMAT_ASTC_10x10_SRGB_BLOCK},  // ASTC10×10
    {VK_FORMAT_ASTC_12x10_UNORM_BLOCK, VK_FORMAT_ASTC_12x10_UNORM_BLOCK, VK_FORMAT_UNDEFINED,
     VK_FORMAT_ASTC_12x10_SRGB_BLOCK},  // ASTC12×10
    {VK_FORMAT_ASTC_12x12_UNORM_BLOCK, VK_FORMAT_ASTC_12x12_UNORM_BLOCK, VK_FORMAT_UNDEFINED,
     VK_FORMAT_ASTC_12x12_SRGB_BLOCK},  // ASTC12×12

    /* uncompressed -------------------------------------------------------- */
    {VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_UNDEFINED, VK_FORMAT_R8G8B8A8_SRGB},  // RGBA8
    {VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_UNDEFINED, VK_FORMAT_B8G8R8A8_SRGB},  // BGRA8
    {VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED},                // RGB8 (NA)
    {VK_FORMAT_R5G6B5_UNORM_PACK16, VK_FORMAT_R5G6B5_UNORM_PACK16, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED},  // RGB565
    {VK_FORMAT_R4G4B4A4_UNORM_PACK16, VK_FORMAT_R4G4B4A4_UNORM_PACK16, VK_FORMAT_UNDEFINED,
     VK_FORMAT_UNDEFINED},  // RGBA4
    {VK_FORMAT_R5G5B5A1_UNORM_PACK16, VK_FORMAT_R5G5B5A1_UNORM_PACK16, VK_FORMAT_UNDEFINED,
     VK_FORMAT_UNDEFINED},  // RGB5A1

    {VK_FORMAT_R8_UNORM, VK_FORMAT_R8_UNORM, VK_FORMAT_UNDEFINED, VK_FORMAT_R8_SRGB},        // R8
    {VK_FORMAT_R8G8_UNORM, VK_FORMAT_R8G8_UNORM, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED},  // RG8
    {VK_FORMAT_R32G32B32A32_SFLOAT, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FORMAT_UNDEFINED,
     VK_FORMAT_UNDEFINED},  // RGBA32F

    /* depth-stencil ------------------------------------------------------- */
    {VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT,
     VK_FORMAT_UNDEFINED},  // D24S8
};

static_assert(AX_ARRAYSIZE(s_pixelFormatInfos) == static_cast<int>(PixelFormat::COUNT),
              "The Vulkan GPU texture format info table is incomplete!");

// -------------------------------------------------------------------------------------------------
// Default attachments
// vulkan s_swapchainColorAttachments not like off-screen MRT, store array for double/triple buffering
// -------------------------------------------------------------------------------------------------
static axstd::pod_vector<TextureImpl*> s_swapchainColorAttachments;
static uint32_t s_currentSwapchainImageIndex          = 0;
static TextureImpl* s_swapchainDepthStencilAttachment = nullptr;

inline namespace UtilsVK
{
// -------------------------------------------------------------------------------------------------
// Public API
// -------------------------------------------------------------------------------------------------
const PixelFormatInfo* toVKFormatInfo(PixelFormat pf)
{
    if (pf < PixelFormat::COUNT) [[likely]]
        return &s_pixelFormatInfos[(int)pf];
    return nullptr;
}

// Helper: create a depth-stencil image as TextureImpl
static TextureImpl* createDepthStencilAttachment(DriverImpl* driver, const VkExtent2D& extent)
{
    TextureDesc depthDesc{};
    depthDesc.textureType  = TextureType::TEXTURE_2D;
    depthDesc.width        = static_cast<uint16_t>(extent.width);
    depthDesc.height       = static_cast<uint16_t>(extent.height);
    depthDesc.arraySize    = 1;
    depthDesc.mipLevels    = 1;
    depthDesc.pixelFormat  = PixelFormat::D24S8;
    depthDesc.textureUsage = TextureUsage::RENDER_TARGET;

    auto tex = new TextureImpl(driver, depthDesc);
    // init image, imageView
    tex->updateData(nullptr, extent.width, extent.height, 0);
    return tex;
}

// Rebuild swapchain attachments from a swapchain image handle.
// Note: swapchainImage must be a VkImage (provided as void* to keep signature parity).
void rebuildSwapchainAttachments(DriverImpl* driver,
                                 const axstd::pod_vector<VkImage>& images,
                                 const axstd::pod_vector<VkImageView>& imageViews,
                                 const VkExtent2D& extent)
{
    // Destroy previous attachments
    destroySwapchainAttachments();

    if (!driver || images.empty() || imageViews.empty())
        return;

    VkDevice device           = driver->getDevice();
    VkPhysicalDevice physical = driver->getPhysicalDevice();

    TextureDesc colorDesc{};
    colorDesc.textureType  = TextureType::TEXTURE_2D;
    colorDesc.width        = static_cast<uint16_t>(extent.width);
    colorDesc.height       = static_cast<uint16_t>(extent.height);
    colorDesc.arraySize    = 1;
    colorDesc.mipLevels    = 1;
    colorDesc.pixelFormat  = PixelFormat::RGBA8;
    colorDesc.textureUsage = TextureUsage::RENDER_TARGET;

    for (auto i = 0; i < images.size(); ++i)
    {
        VkImage swapchainImage = images[i];
        VkImageView imageView  = imageViews[i];
        // Wrap the swapchain VkImage as TextureImpl (color attachment)
        // Important: TextureImpl(VkImage) does not own the image memory; it should create a VkImageView for sampling.
        auto colorTex = new TextureImpl(driver, swapchainImage, imageView);
        // Update descriptor (sampler, mip info, etc.). The TextureImpl should create view if missing.
        colorTex->updateTextureDesc(colorDesc);
        s_swapchainColorAttachments.push_back(colorTex);
    }

    // Create a matching depth-stencil attachment
    s_swapchainDepthStencilAttachment = createDepthStencilAttachment(driver, extent);
}

void destroySwapchainAttachments()
{
    if (!s_swapchainColorAttachments.empty())
    {
        for (auto tex : s_swapchainColorAttachments)
            delete tex;
        s_swapchainColorAttachments.clear();
    }
    if (s_swapchainDepthStencilAttachment)
    {
        delete s_swapchainDepthStencilAttachment;
        s_swapchainDepthStencilAttachment = nullptr;
    }
}

// Getters for default attachments
void setSwapchainCurrentImageIndex(uint32_t imageIndex)
{
    s_currentSwapchainImageIndex = imageIndex;
}

TextureImpl* getSwapchainColorAttachment()
{
    return s_swapchainColorAttachments[s_currentSwapchainImageIndex];
}

TextureImpl* getSwapchainDepthStencilAttachment()
{
    return s_swapchainDepthStencilAttachment;
}
}  // namespace UtilsVK
}  // namespace ax::rhi::vk
