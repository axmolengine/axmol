/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to deal in the Software without restriction, including the rights
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
#include "axmol/rhi/vulkan/RenderTargetVK.h"
#include "axmol/rhi/vulkan/DriverVK.h"
#include "axmol/rhi/vulkan/UtilsVK.h"
#include "axmol/base/Logging.h"
#include "axmol/tlx/hash.hpp"

namespace ax::rhi::vk
{

// Build a robust framebuffer cache key including:
// - RenderPass handle (value)
// - Ordered image view handles (contiguous color attachments, optional depth)
// - Attachment count
// - Framebuffer width/height
static uintptr_t makeFramebufferKeyHash(VkRenderPass rp,
                                        const tlx::pod_vector<VkImageView>& views,
                                        uint32_t width,
                                        uint32_t height)
{
    // Pack a small POD blob to hash deterministically.
    struct Header
    {
        uintptr_t rp;
        uint32_t count;
        uint32_t width;
        uint32_t height;
    } hdr{reinterpret_cast<uintptr_t>(rp), static_cast<uint32_t>(views.size()), width, height};

    // Hash header first, then hash the views array in order.
    uintptr_t h = tlx::hash_bytes(&hdr, sizeof(hdr), 0);
    if (!views.empty())
        h = tlx::hash_bytes(views.data(), views.size_bytes(), h);
    return h;
}

RenderTargetImpl::RenderTargetImpl(DriverImpl* driver, bool defaultRenderTarget)
    : RenderTarget(defaultRenderTarget), _driver(driver)
{
    _clearValues.reserve(MAX_COLOR_ATTCHMENT + 1);
}

RenderTargetImpl::~RenderTargetImpl()
{
    _driver->waitForGPU();

    invalidate();
    // We do not destroy cached renderpasses/framebuffers here to allow reuse across RT instances
}

void RenderTargetImpl::invalidate()
{
    // Conservative: wait idle before destroying caches to avoid "in use" errors.
    for (auto& [_, pass] : _renderPassCache)
        _driver->destroyRenderPass(pass);
    for (auto& [_, fb] : _framebufferCache)
        _driver->destroyFramebuffer(fb);
    _renderPassCache.clear();
    _framebufferCache.clear();
    _renderPass  = VK_NULL_HANDLE;
    _framebuffer = VK_NULL_HANDLE;

    _attachmentViews.fill(nullptr);

    _renderHashSeeds.fill(0);
    _activeHashSeed = 0;

    _dirtyFlags = TargetBufferFlags::ALL;
}

void RenderTargetImpl::rebuildSwapchainAttachments(const tlx::pod_vector<VkImage>& images,
                                                   const tlx::pod_vector<VkImageView>& imageViews,
                                                   const VkExtent2D& extent,
                                                   PixelFormat imagePF)
{
    if (images.empty() || imageViews.empty())
        return;

    VK_VERIFY(images.size() <= MAX_COLOR_ATTCHMENT, "Too many swapchain images");

    _dirtyFlags = TargetBufferFlags::DEPTH_AND_STENCIL;

    // destroy old attachments
    // colors
    for (auto tex : _color)
    {
        if (tex)
            AX_SAFE_RELEASE_NULL(tex.texture);
    }

    // depth-stencil
    if (_depthStencil)
        AX_SAFE_RELEASE_NULL(_depthStencil.texture);

    /// create new attachments

    // colors
    TextureDesc colorDesc{};
    colorDesc.textureType  = TextureType::TEXTURE_2D;
    colorDesc.width        = static_cast<uint16_t>(extent.width);
    colorDesc.height       = static_cast<uint16_t>(extent.height);
    colorDesc.arraySize    = 1;
    colorDesc.mipLevels    = 1;
    colorDesc.pixelFormat  = imagePF;
    colorDesc.textureUsage = TextureUsage::RENDER_TARGET;

    for (auto i = 0; i < images.size(); ++i)
    {
        VkImage swapchainImage = images[i];
        VkImageView imageView  = imageViews[i];
        // Wrap the swapchain VkImage as TextureImpl (color attachment)
        // Important: TextureImpl(VkImage) does not own the image memory; it should create a VkImageView for sampling.
        auto colorTex = new TextureImpl(_driver, swapchainImage, imageView);
        // Update descriptor (sampler, mip info, etc.). The TextureImpl should create view if missing.
        colorTex->updateTextureDesc(colorDesc);
        _color[i].texture = colorTex;
        _dirtyFlags |= getMRTColorFlag(i);
    }

    // depth-stencil
    TextureDesc depthDesc{};
    depthDesc.textureType  = TextureType::TEXTURE_2D;
    depthDesc.width        = static_cast<uint16_t>(extent.width);
    depthDesc.height       = static_cast<uint16_t>(extent.height);
    depthDesc.arraySize    = 1;
    depthDesc.mipLevels    = 1;
    depthDesc.pixelFormat  = PixelFormat::D24S8;
    depthDesc.textureUsage = TextureUsage::RENDER_TARGET;

    auto tex = new TextureImpl(_driver, depthDesc);
    // init image, imageView
    tex->updateData(nullptr, extent.width, extent.height, 0);
    _depthStencil.texture = tex;
}

void RenderTargetImpl::beginRenderPass(VkCommandBuffer cmd,
                                       const RenderPassDesc& renderPassDesc,
                                       uint32_t width,
                                       uint32_t height,
                                       uint32_t imageIndex)
{
    // 1) Collect attachment views and impl pointers

    if (_defaultRenderTarget)
    {
        AXASSERT(imageIndex < MAX_COLOR_ATTCHMENT, "image index out of range");

        auto oldDirtyFlags = _dirtyFlags;
        if (bitmask::any(_dirtyFlags, TargetBufferFlags::DEPTH_AND_STENCIL))
        {
            _dirtyFlags &= ~TargetBufferFlags::DEPTH_AND_STENCIL;
            if (_depthStencil)
                _attachmentViews[DepthViewIndex] =
                    static_cast<TextureImpl*>(_depthStencil.texture)->internalHandle().view;
            else
                AXASSERT(false, "swapchain depth-stencil can't be null");
        }

        const auto colorFlag = getMRTColorFlag(imageIndex);
        if (bitmask::any(_dirtyFlags, colorFlag))
        {
            _dirtyFlags &= ~colorFlag;
            if (_color[imageIndex])
                _attachmentViews[imageIndex] =
                    static_cast<TextureImpl*>(_color[imageIndex].texture)->internalHandle().view;
            else
                AXASSERT(false, "swapchain image can't be null");
        }

        if (oldDirtyFlags == _dirtyFlags) [[likely]]
        {  // both color[imageIndex] and depth-steicl not changed
            _activeHashSeed = _renderHashSeeds[imageIndex];
        }
        else
        {
            _activeHashSeed = _renderHashSeeds[imageIndex] =
                tlx::hash_bytes(&_attachmentViews[imageIndex], sizeof(VkImageView),
                                reinterpret_cast<uintptr_t>(_attachmentViews[DepthViewIndex]));
        }
    }
    else
    {  // Offscreen RenderTarget, update all attachment one-time
        if (_dirtyFlags != TargetBufferFlags::NONE) [[unlikely]]
        {
            // Unconditionally collect attachments for contiguous MRT from index 0
            for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
            {
                if (!bitmask::any(_dirtyFlags, getMRTColorFlag(i)))
                    continue;
                if (_color[i].texture)
                {
                    auto* texImpl       = static_cast<TextureImpl*>(_color[i].texture);
                    _attachmentViews[i] = texImpl->internalHandle().view;
                }
                else
                {
                    _attachmentViews[i] = VK_NULL_HANDLE;
                }
            }

            if (bitmask::any(_dirtyFlags, TargetBufferFlags::DEPTH_AND_STENCIL))
            {
                if (_depthStencil.texture)
                {
                    auto* texImpl                    = static_cast<TextureImpl*>(_depthStencil.texture);
                    _attachmentViews[DepthViewIndex] = texImpl->internalHandle().view;
                }
                else
                {
                    _attachmentViews[DepthViewIndex] = VK_NULL_HANDLE;
                }
            }

            _activeHashSeed = XXH64(&_attachmentViews[0], sizeof(_attachmentViews), 0);

            _dirtyFlags = TargetBufferFlags::NONE;
        }
    }

    // 2) Ensure render pass
    updateRenderPass(renderPassDesc, imageIndex);

    // 3) Ensure framebuffer
    updateFramebuffer(cmd, imageIndex);

    // 4) Clear values
    _clearValues.clear();

    if (_defaultRenderTarget)
    {
        VkClearValue& cv = _clearValues.emplace_back();
        if (bitmask::any(renderPassDesc.flags.clear, getMRTColorFlag(imageIndex)))
            cv.color = {renderPassDesc.clearColorValue[0], renderPassDesc.clearColorValue[1],
                        renderPassDesc.clearColorValue[2], renderPassDesc.clearColorValue[3]};
        else
            cv.color = {{0.f, 0.f, 0.f, 0.f}};
    }
    else
    {
        for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
        {
            if (_attachmentViews[i] != VK_NULL_HANDLE)
            {
                VkClearValue& cv = _clearValues.emplace_back();
                if (bitmask::any(renderPassDesc.flags.clear, getMRTColorFlag(i)))
                {
                    cv.color = {renderPassDesc.clearColorValue[0], renderPassDesc.clearColorValue[1],
                                renderPassDesc.clearColorValue[2], renderPassDesc.clearColorValue[3]};
                }
                else
                {
                    cv.color = {{0.f, 0.f, 0.f, 0.f}};
                }
            }
            else
            {
                break;  // contiguous color attachments assumption
            }
        }
    }

    if (_attachmentViews[DepthViewIndex] != VK_NULL_HANDLE)
    {
        VkClearValue& dsv = _clearValues.emplace_back();
        if (bitmask::any(renderPassDesc.flags.clear, TargetBufferFlags::DEPTH_AND_STENCIL))
        {
            dsv.depthStencil.depth   = renderPassDesc.clearDepthValue;
            dsv.depthStencil.stencil = static_cast<uint32_t>(renderPassDesc.clearStencilValue);
        }
        else
        {
            dsv.depthStencil.depth   = 1.0f;
            dsv.depthStencil.stencil = 0u;
        }
    }

    // 5) Transition to render layouts (non-default RT only)
    if (!_defaultRenderTarget)
        prepareAttachmentsForRendering(cmd);

    // 6) Begin render pass
    VkRenderPassBeginInfo rpBegin{};
    rpBegin.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBegin.renderPass        = _renderPass;
    rpBegin.framebuffer       = _framebuffer;
    rpBegin.renderArea.offset = {0, 0};
    rpBegin.renderArea.extent = {width, height};
    rpBegin.clearValueCount   = static_cast<uint32_t>(_clearValues.size());
    rpBegin.pClearValues      = _clearValues.data();

    vkCmdBeginRenderPass(cmd, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderTargetImpl::endRenderPass(VkCommandBuffer cmd)
{
    vkCmdEndRenderPass(cmd);

    if (!_defaultRenderTarget)
    {
        for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
        {
            if (!_color[i])
                break;
            static_cast<TextureImpl*>(_color[i].texture)->setKnownLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
        if (_depthStencil)
        {
            static_cast<TextureImpl*>(_depthStencil.texture)
                ->setKnownLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        }
    }
}

void RenderTargetImpl::updateFramebuffer(VkCommandBuffer /*cmd*/, uint32_t imageIndex)
{
    AXASSERT(_renderPass, "updateFramebuffer: RenderPass must set");

    auto key = tlx::hash_bytes(&_renderPass, sizeof(_renderPass), _activeHashSeed);
    if (auto it = _framebufferCache.find(key); it != _framebufferCache.end())
    {
        _framebuffer = it->second;
    }
    else
    {
        // Build ordered views vector (contiguous colors + optional depth)
        tlx::pod_vector<VkImageView> views;
        views.reserve(MAX_COLOR_ATTCHMENT + 1);
        if (_defaultRenderTarget)
        {
            AXASSERT(_attachmentViews[imageIndex], "swapchain color attachment view can't be null");
            views.push_back(_attachmentViews[imageIndex]);
        }
        else
        {
            for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
            {
                if (_attachmentViews[i] != VK_NULL_HANDLE)
                    views.push_back(_attachmentViews[i]);
                else
                    break;  // contiguous color attachments assumption
            }
        }
        if (_attachmentViews[DepthViewIndex] != VK_NULL_HANDLE)
            views.push_back(_attachmentViews[DepthViewIndex]);

        // Derive framebuffer extent from color0 or depth if color0 absent (depth-only)
        const auto color0 = getColorAttachment(0);
        auto& colorDesc   = color0->getDesc();
        uint32_t fbWidth  = colorDesc.width;
        uint32_t fbHeight = colorDesc.height;
        if ((fbWidth == 0 || fbHeight == 0) && _attachmentViews[DepthViewIndex] != VK_NULL_HANDLE)
        {
            const auto& dsDesc = getDepthStencilAttachment()->getDesc();
            fbWidth            = dsDesc.width;
            fbHeight           = dsDesc.height;
        }

        VkFramebufferCreateInfo fbci{};
        fbci.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbci.renderPass      = _renderPass;
        fbci.attachmentCount = static_cast<uint32_t>(views.size());
        fbci.pAttachments    = views.data();
        fbci.width           = fbWidth;
        fbci.height          = fbHeight;
        fbci.layers          = 1;

        VkResult vr = vkCreateFramebuffer(_driver->getDevice(), &fbci, nullptr, &_framebuffer);
        AXASSERT(vr == VK_SUCCESS, "Failed to create VkFramebuffer");

        _framebufferCache.emplace(key, _framebuffer);
    }
}

void RenderTargetImpl::updateRenderPass(const RenderPassDesc& desc, uint32_t imageIndex)
{
    const auto key = tlx::hash_bytes(&desc, sizeof(desc), _activeHashSeed);

    if (auto it = _renderPassCache.find(key); it != _renderPassCache.end())
    {
        _renderPass = it->second;
    }
    else
    {
        // Build attachment descriptions deterministically (contiguous colors + optional depth)
        tlx::pod_vector<VkAttachmentDescription> attachments;
        tlx::pod_vector<VkAttachmentReference> colorRefs;
        VkAttachmentReference depthRef{};
        attachments.reserve(MAX_COLOR_ATTCHMENT + 1);
        colorRefs.reserve(MAX_COLOR_ATTCHMENT);

        const bool isDefaultRT = _defaultRenderTarget;

        const auto appendAttachmentDesc = [this, &desc, isDefaultRT, &attachments, &colorRefs](int i) {
            auto attachment     = getColorAttachment(static_cast<int>(i));
            const auto& attDesc = attachment->getDesc();

            const bool clearColor   = bitmask::any(desc.flags.clear, getMRTColorFlag(i));
            const bool discardStart = bitmask::any(desc.flags.discardStart, getMRTColorFlag(i));
            const bool discardEnd   = bitmask::any(desc.flags.discardEnd, getMRTColorFlag(i));

            const VkAttachmentLoadOp loadOp =
                clearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR
                           : (discardStart ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : VK_ATTACHMENT_LOAD_OP_LOAD);
            const VkAttachmentStoreOp storeOp =
                discardEnd ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;

            VkAttachmentDescription ad{};
            ad.format         = UtilsVK::toVKFormat(attDesc.pixelFormat);
            ad.samples        = VK_SAMPLE_COUNT_1_BIT;
            ad.loadOp         = loadOp;
            ad.storeOp        = storeOp;
            ad.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            ad.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

            // Use UNDEFINED when not loading to minimize mismatch risk
            ad.initialLayout =
                (loadOp == VK_ATTACHMENT_LOAD_OP_LOAD)
                    ? (isDefaultRT ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                    : VK_IMAGE_LAYOUT_UNDEFINED;

            ad.finalLayout = isDefaultRT ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            attachments.push_back(ad);

            VkAttachmentReference ref{};
            ref.attachment = static_cast<uint32_t>(colorRefs.size());
            ref.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorRefs.push_back(ref);
        };

        // Color attachments
        if (isDefaultRT)
        {
            appendAttachmentDesc(imageIndex);
        }
        else
        {
            for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
            {
                if (_attachmentViews[i] == VK_NULL_HANDLE)
                    break;

                appendAttachmentDesc(i);
            }
        }

        // Depth/stencil attachment
        const bool hasDepth = (_attachmentViews[DepthViewIndex] != VK_NULL_HANDLE);
        if (hasDepth)
        {
            auto attachment    = getDepthStencilAttachment();
            const auto& dsDesc = attachment->getDesc();

            const bool clearDepth   = bitmask::any(desc.flags.clear, TargetBufferFlags::DEPTH);
            const bool discardD0    = bitmask::any(desc.flags.discardStart, TargetBufferFlags::DEPTH);
            const bool discardD1    = bitmask::any(desc.flags.discardEnd, TargetBufferFlags::DEPTH);
            const bool clearStencil = bitmask::any(desc.flags.clear, TargetBufferFlags::STENCIL);
            const bool discardS0    = bitmask::any(desc.flags.discardStart, TargetBufferFlags::STENCIL);
            const bool discardS1    = bitmask::any(desc.flags.discardEnd, TargetBufferFlags::STENCIL);

            const VkAttachmentLoadOp depthLoad =
                clearDepth ? VK_ATTACHMENT_LOAD_OP_CLEAR
                           : (discardD0 ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : VK_ATTACHMENT_LOAD_OP_LOAD);
            const VkAttachmentStoreOp depthStore =
                discardD1 ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;

            const VkAttachmentLoadOp stencilLoad =
                clearStencil ? VK_ATTACHMENT_LOAD_OP_CLEAR
                             : (discardS0 ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : VK_ATTACHMENT_LOAD_OP_LOAD);
            const VkAttachmentStoreOp stencilStore =
                discardS1 ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;

            VkAttachmentDescription ad{};
            ad.format         = UtilsVK::toVKFormat(dsDesc.pixelFormat);
            ad.samples        = VK_SAMPLE_COUNT_1_BIT;
            ad.loadOp         = depthLoad;
            ad.storeOp        = depthStore;
            ad.stencilLoadOp  = stencilLoad;
            ad.stencilStoreOp = stencilStore;

            const bool needLoadInitial =
                (depthLoad == VK_ATTACHMENT_LOAD_OP_LOAD) || (stencilLoad == VK_ATTACHMENT_LOAD_OP_LOAD);

            ad.initialLayout =
                needLoadInitial ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;

            ad.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            attachments.push_back(ad);

            depthRef.attachment = static_cast<uint32_t>(attachments.size() - 1);
            depthRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        // Subpass
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount    = static_cast<uint32_t>(colorRefs.size());
        subpass.pColorAttachments       = colorRefs.data();
        subpass.pDepthStencilAttachment = hasDepth ? &depthRef : nullptr;

        // Dependencies
        VkSubpassDependency deps[2]{};
        deps[0].srcSubpass    = VK_SUBPASS_EXTERNAL;
        deps[0].dstSubpass    = 0;
        deps[0].srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        deps[0].dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        deps[0].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        deps[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        uint32_t depCount = 1;
        if (hasDepth)
        {
            deps[1].srcSubpass    = VK_SUBPASS_EXTERNAL;
            deps[1].dstSubpass    = 0;
            deps[1].srcStageMask  = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            deps[1].dstStageMask  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            deps[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            deps[1].dstAccessMask =
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            depCount = 2;
        }

        VkRenderPassCreateInfo rpci{};
        rpci.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rpci.attachmentCount = static_cast<uint32_t>(attachments.size());
        rpci.pAttachments    = attachments.data();
        rpci.subpassCount    = 1;
        rpci.pSubpasses      = &subpass;
        rpci.dependencyCount = depCount;
        rpci.pDependencies   = deps;

        VkResult vr = vkCreateRenderPass(_driver->getDevice(), &rpci, nullptr, &_renderPass);
        AXASSERT(vr == VK_SUCCESS, "Failed to create VkRenderPass");

        // AXLOGI("vkCreateRenderPass: renderPass: {}, cache id:{}", fmt::ptr(_renderPass), key);

        _renderPassCache.emplace(key, _renderPass);
    }
}

void RenderTargetImpl::prepareAttachmentsForRendering(VkCommandBuffer cmd)
{
    if (_defaultRenderTarget) [[likely]]
        return;

    // Color -> ATTACHMENT_OPTIMAL (contiguous indices starting at 0)
    for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
    {
        if (!_color[i])
            break;
        static_cast<TextureImpl*>(_color[i].texture)->transitionLayout(cmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    }

    // Depth -> ATTACHMENT_OPTIMAL
    if (_depthStencil)
    {
        TextureImpl* texImpl = static_cast<TextureImpl*>(_depthStencil.texture);
        texImpl->transitionLayout(cmd, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    }
}

RenderTargetImpl::Attachment RenderTargetImpl::getColorAttachment(int index) const
{
    return static_cast<TextureImpl*>(_color[index].texture);
}

RenderTargetImpl::Attachment RenderTargetImpl::getDepthStencilAttachment() const
{
    return static_cast<TextureImpl*>(_depthStencil.texture);
}

}  // namespace ax::rhi::vk
