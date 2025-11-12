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
#include "RenderTargetVK.h"
#include "UtilsVK.h"
#include "axmol/base/Logging.h"
#include "xxhash/xxhash.h"

namespace ax::rhi::vk
{

RenderTargetImpl::RenderTargetImpl(VkDevice device, bool defaultRenderTarget)
    : RenderTarget(defaultRenderTarget), _device(device)
{
    if (_defaultRenderTarget)
        _dirtyFlags = TargetBufferFlags::ALL;
}

RenderTargetImpl::~RenderTargetImpl()
{
    invalidate();

    // Note: we do not destroy cached renderpasses/framebuffers here to allow reuse across RT instances
    // If needed, an explicit shutdown phase can walk caches and destroy them.
}

void RenderTargetImpl::invalidate()
{
    for (auto& [_, pass] : _renderPassCache)
        vkDestroyRenderPass(_device, pass, nullptr);
    for (auto& [_, fb] : _framebufferCache)
        vkDestroyFramebuffer(_device, fb, nullptr);
    _renderPassCache.clear();
    _framebufferCache.clear();
    _renderPass  = nullptr;
    _framebuffer = nullptr;

    for (auto& v : _attachmentViews)
        v = VK_NULL_HANDLE;

    _attachmentViewsHash = 0;
    _attachmentsDirty    = true;

    // Mark all buffers dirty so next beginRenderPass re-collects attachments
    _dirtyFlags = TargetBufferFlags::ALL;
}

void RenderTargetImpl::beginRenderPass(VkCommandBuffer cmd,
                                       const RenderPassDesc& renderPassDesc,
                                       uint32_t width,
                                       uint32_t height) const
{
    // 1) Collect attachments for the current target
    do
    {
        if (_defaultRenderTarget)
        {
            // Default RT (swapchain): pull current image & depth from UtilsVK
            const auto colorTex = UtilsVK::getSwapchainColorAttachment();
            const auto depthTex = UtilsVK::getSwapchainDepthStencilAttachment();
            _attachmentViews.fill(VK_NULL_HANDLE);
            if (colorTex)
                _attachmentViews[0] = colorTex->internalHandle().view;
            if (depthTex)
                _attachmentViews[DepthViewIndex] = depthTex->internalHandle().view;

            // Update attachment hash based on current views
            _attachmentViewsHash = XXH64(&_attachmentViews[0], sizeof(_attachmentViews), 0);
            _attachmentsDirty    = true;  // views updated → may need FB/RP match
        }
        else
        {
            // Offscreen RT: only recompute attachments when marked dirty
            if (!_dirtyFlags)
                break;

            // Collect color attachments
            for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
            {
                if (bitmask::any(_dirtyFlags, getMRTColorFlag(i)))
                {
                    if (_color[i].texture)
                    {
                        auto* texImpl       = static_cast<TextureImpl*>(_color[i].texture);
                        _attachmentViews[i] = texImpl->internalHandle().view;
                    }
                    else
                        _attachmentViews[i] = VK_NULL_HANDLE;
                }
            }

            // Collect depth/stencil attachment
            if (bitmask::any(_dirtyFlags, TargetBufferFlags::DEPTH_AND_STENCIL))
            {
                if (_depthStencil.texture)
                {
                    auto* texImpl                    = static_cast<TextureImpl*>(_depthStencil.texture);
                    _attachmentViews[DepthViewIndex] = texImpl->internalHandle().view;
                }
                else
                    _attachmentViews[DepthViewIndex] = VK_NULL_HANDLE;
            }

            _attachmentsDirty    = true;
            _dirtyFlags          = TargetBufferFlags::NONE;
            _attachmentViewsHash = XXH64(&_attachmentViews[0], sizeof(_attachmentViews), 0);
        }
    } while (false);

    // 2) Lookup or create a compatible VkRenderPass based on desc + attachments
    VkRenderPass rp = ensureRenderPass(renderPassDesc);

    // 3) Lookup or create a framebuffer compatible with this render pass
    VkFramebuffer fb = ensureFramebuffer(rp);

    // 4) Build clear values according to flags and attachment order
    _clearValues.clear();

    // Push color clears in the same order as colorRefs
    for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
    {
        if (_attachmentViews[i] != VK_NULL_HANDLE)
        {
            const auto flag = getMRTColorFlag(i);
            if (bitmask::any(renderPassDesc.flags.clear, flag))
            {
                VkClearValue cv{};
                cv.color = {renderPassDesc.clearColorValue[0], renderPassDesc.clearColorValue[1],
                            renderPassDesc.clearColorValue[2], renderPassDesc.clearColorValue[3]};
                _clearValues.push_back(cv);
            }
        }
    }

    // Depth/stencil clear value (if any)
    if (_attachmentViews[DepthViewIndex] != VK_NULL_HANDLE &&
        bitmask::any(renderPassDesc.flags.clear, TargetBufferFlags::DEPTH_AND_STENCIL))
    {
        VkClearValue dsv{};
        dsv.depthStencil.depth   = renderPassDesc.clearDepthValue;
        dsv.depthStencil.stencil = static_cast<uint32_t>(renderPassDesc.clearStencilValue);
        _clearValues.push_back(dsv);
    }

    // 5) Begin render pass
    VkRenderPassBeginInfo rpBegin{};
    rpBegin.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBegin.renderPass        = rp;
    rpBegin.framebuffer       = fb;
    rpBegin.renderArea.offset = {0, 0};
    rpBegin.renderArea.extent = {width, height};
    rpBegin.clearValueCount   = static_cast<uint32_t>(_clearValues.size());
    rpBegin.pClearValues      = _clearValues.data();

    vkCmdBeginRenderPass(cmd, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);
}

VkFramebuffer RenderTargetImpl::ensureFramebuffer(VkRenderPass rp) const
{
    // If framebuffer is valid and attachments are not dirty, reuse it
    if (_framebuffer != VK_NULL_HANDLE && !_attachmentsDirty)
        return _framebuffer;

    // Key: combine renderpass handle and attachment views hash
    const uint64_t key = XXH64(&rp, sizeof(rp), _attachmentViewsHash);

    if (auto it = _framebufferCache.find(key); it != _framebufferCache.end())
    {
        _framebuffer = it->second;
    }
    else
    {
        // Collect current attachments in declared order (color0..MAX_COLOR-1, then depth)
        axstd::pod_vector<VkImageView> views;
        views.reserve(MAX_COLOR_ATTCHMENT + 1);
        for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
        {
            if (_attachmentViews[i] != VK_NULL_HANDLE)
                views.push_back(_attachmentViews[i]);
        }
        if (_attachmentViews[DepthViewIndex] != VK_NULL_HANDLE)
            views.push_back(_attachmentViews[DepthViewIndex]);

        // Determine size from color0 or depth (fallback)
        const auto color0 = getColorAttachment(0);
        uint32_t fbWidth  = color0.desc.width;
        uint32_t fbHeight = color0.desc.height;

        if ((fbWidth == 0 || fbHeight == 0) && _attachmentViews[DepthViewIndex] != VK_NULL_HANDLE)
        {
            const auto depthAtt = getDepthStencilAttachment();
            fbWidth             = depthAtt.desc.width;
            fbHeight            = depthAtt.desc.height;
        }

        VkFramebufferCreateInfo fbci{};
        fbci.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbci.renderPass      = rp;
        fbci.attachmentCount = static_cast<uint32_t>(views.size());
        fbci.pAttachments    = views.data();
        fbci.width           = fbWidth;
        fbci.height          = fbHeight;
        fbci.layers          = 1;

        VkResult vr = vkCreateFramebuffer(_device, &fbci, nullptr, &_framebuffer);
        AXASSERT(vr == VK_SUCCESS, "Failed to create VkFramebuffer");

        _framebufferCache.emplace(key, _framebuffer);
    }

    _attachmentsDirty = false;
    return _framebuffer;
}

VkRenderPass RenderTargetImpl::ensureRenderPass(const RenderPassDesc& desc) const
{
    // Reuse if valid and attachments unchanged
    if (_renderPass != VK_NULL_HANDLE && !_attachmentsDirty)
        return _renderPass;

    const uint64_t key = XXH64(&desc, sizeof(RenderPassDesc), _attachmentViewsHash);

    if (auto it = _renderPassCache.find(key); it != _renderPassCache.end())
    {
        _renderPass = it->second;
    }
    else
    {
        axstd::pod_vector<VkAttachmentDescription> attachments;
        axstd::pod_vector<VkAttachmentReference> colorRefs;
        VkAttachmentReference depthRef{};
        attachments.reserve(MAX_COLOR_ATTCHMENT + 1);
        colorRefs.reserve(MAX_COLOR_ATTCHMENT);

        const bool isDefaultRT = _defaultRenderTarget;

        // Build color attachment descriptions and references
        for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
        {
            if (_attachmentViews[i] != VK_NULL_HANDLE)
            {
                const auto&& attDesc = getColorAttachment(static_cast<int>(i)).desc;

                const bool clearColor   = bitmask::any(desc.flags.clear, getMRTColorFlag(i));
                const bool discardStart = bitmask::any(desc.flags.discardStart, getMRTColorFlag(i));
                const bool discardEnd   = bitmask::any(desc.flags.discardEnd, getMRTColorFlag(i));

                const VkAttachmentLoadOp loadOp =
                    clearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR
                               : (discardStart ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : VK_ATTACHMENT_LOAD_OP_LOAD);
                const VkAttachmentStoreOp storeOp =
                    discardEnd ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;

                VkAttachmentDescription ad{};
                ad.format         = UtilsVK::toVKFormatInfo(attDesc.pixelFormat)->format;
                ad.samples        = VK_SAMPLE_COUNT_1_BIT;
                ad.loadOp         = loadOp;
                ad.storeOp        = storeOp;
                ad.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                ad.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

                // initialLayout must NOT be UNDEFINED if loadOp == LOAD
                if (loadOp == VK_ATTACHMENT_LOAD_OP_LOAD)
                {
                    ad.initialLayout =
                        isDefaultRT ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                }
                else
                {
                    ad.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                }

                ad.finalLayout =
                    isDefaultRT ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                attachments.push_back(ad);

                VkAttachmentReference ref{};
                ref.attachment = static_cast<uint32_t>(colorRefs.size());
                ref.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                colorRefs.push_back(ref);
            }
        }

        // Depth/stencil attachment
        const bool hasDepth = (_attachmentViews[DepthViewIndex] != VK_NULL_HANDLE);
        if (hasDepth)
        {
            const auto&& dsDesc = getDepthStencilAttachment().desc;

            const bool clearDepth = bitmask::any(desc.flags.clear, TargetBufferFlags::DEPTH);
            const bool discardD0  = bitmask::any(desc.flags.discardStart, TargetBufferFlags::DEPTH);
            const bool discardD1  = bitmask::any(desc.flags.discardEnd, TargetBufferFlags::DEPTH);

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
            ad.format         = UtilsVK::toVKFormatInfo(dsDesc.pixelFormat)->format;
            ad.samples        = VK_SAMPLE_COUNT_1_BIT;
            ad.loadOp         = depthLoad;
            ad.storeOp        = depthStore;
            ad.stencilLoadOp  = stencilLoad;
            ad.stencilStoreOp = stencilStore;

            // If either depthLoad or stencilLoad is LOAD, initialLayout must NOT be UNDEFINED
            const bool needLoadInitial =
                (depthLoad == VK_ATTACHMENT_LOAD_OP_LOAD) || (stencilLoad == VK_ATTACHMENT_LOAD_OP_LOAD);

            ad.initialLayout =
                needLoadInitial ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;

            ad.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            attachments.push_back(ad);

            depthRef.attachment = static_cast<uint32_t>(attachments.size() - 1);
            depthRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        // Subpass description
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount    = static_cast<uint32_t>(colorRefs.size());
        subpass.pColorAttachments       = colorRefs.data();
        subpass.pDepthStencilAttachment = hasDepth ? &depthRef : nullptr;

        // Subpass dependency:
        // For LOAD cases, it's safer to include READ|WRITE on dst to ensure visibility.
        VkSubpassDependency dep{};
        dep.srcSubpass    = VK_SUBPASS_EXTERNAL;
        dep.dstSubpass    = 0;
        dep.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dep.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dep.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;  // previous frame writes
        dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo rpci{};
        rpci.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rpci.attachmentCount = static_cast<uint32_t>(attachments.size());
        rpci.pAttachments    = attachments.data();
        rpci.subpassCount    = 1;
        rpci.pSubpasses      = &subpass;
        rpci.dependencyCount = 1;
        rpci.pDependencies   = &dep;

        VkResult vr = vkCreateRenderPass(_device, &rpci, nullptr, &_renderPass);
        AXASSERT(vr == VK_SUCCESS, "Failed to create VkRenderPass");

        _renderPassCache.emplace(key, _renderPass);
    }

    return _renderPass;
}

RenderTargetImpl::Attachment RenderTargetImpl::getColorAttachment(int index) const
{
    TextureImpl* texImpl = _defaultRenderTarget ? UtilsVK::getSwapchainColorAttachment()
                                                : static_cast<TextureImpl*>(_color[index].texture);
    return texImpl ? Attachment{texImpl->internalHandle().view, texImpl->getDesc()} : Attachment{};
}

RenderTargetImpl::Attachment RenderTargetImpl::getDepthStencilAttachment() const
{
    TextureImpl* texImpl = _defaultRenderTarget ? UtilsVK::getSwapchainDepthStencilAttachment()
                                                : static_cast<TextureImpl*>(_depthStencil.texture);
    return texImpl ? Attachment{texImpl->internalHandle().view, texImpl->getDesc()} : Attachment{};
}

}  // namespace ax::rhi::vk
