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

#include "axmol/rhi/RenderTarget.h"
#include "axmol/rhi/vulkan/TextureVK.h"
#include <glad/vulkan.h>
#include <array>

namespace ax::rhi::vk
{
class DriverImpl;
class RenderTargetImpl : public RenderTarget
{
public:
    enum
    {
        DepthViewIndex = MAX_COLOR_ATTCHMENT,
    };

    using Attachment = TextureImpl*;

    RenderTargetImpl(DriverImpl* driver, bool defaultRenderTarget);
    ~RenderTargetImpl();

    // Destroy the current live framebuffer and mark attachments dirty
    void invalidate();

    // Begin a render pass using this target
    void beginRenderPass(VkCommandBuffer cmd, const RenderPassDesc& desc, uint32_t width, uint32_t height);

    void endRenderPass(VkCommandBuffer cmd);

    Attachment getColorAttachment(int index) const;
    Attachment getDepthStencilAttachment() const;

    VkFramebuffer getFramebuffer() const { return _framebuffer; }

    VkRenderPass getVkRenderPass() const { return _renderPass; }

private:
    void updateRenderPass(const RenderPassDesc& desc);
    void updateFramebuffer(VkCommandBuffer cmd);

    void prepareAttachmentsForRendering(VkCommandBuffer cmd);

    DriverImpl* _driver{nullptr};

    // Current attachment views for building renderpass/framebuffer
    std::array<VkImageView, MAX_COLOR_ATTCHMENT + 1> _attachmentViews{};
    std::array<TextureImpl*, MAX_COLOR_ATTCHMENT + 1> _attachmentTexPtrs;
    uint64_t _attachmentViewsHash{0};

    axstd::pod_vector<VkClearValue> _clearValues;

    VkRenderPass _renderPass{VK_NULL_HANDLE};    // active render pass
    VkFramebuffer _framebuffer{VK_NULL_HANDLE};  // active framebuffer

    // Caches keyed by (desc hash, attachment views hash)
    axstd::hash_map<uintptr_t, VkRenderPass> _renderPassCache;
    axstd::hash_map<uintptr_t, VkFramebuffer> _framebufferCache;

    bool _attachmentsDirty{true};
};

}  // namespace ax::rhi::vk
