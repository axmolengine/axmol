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
#include "TextureVK.h"
#include <glad/vulkan.h>
#include <array>

namespace ax::rhi::vk
{
class RenderTargetImpl : public RenderTarget
{
public:
    enum
    {
        DepthViewIndex = MAX_COLOR_ATTCHMENT,
    };

    struct Attachment
    {
        TextureHandle tex{};
        TextureDesc desc{};
    };

    RenderTargetImpl(VkDevice device, bool defaultRenderTarget);
    ~RenderTargetImpl();

    // Destroy the current live framebuffer and mark attachments dirty
    void invalidate();

    // Begin a render pass using this target
    void beginRenderPass(VkCommandBuffer cmd, const RenderPassDesc& desc, uint32_t width, uint32_t height) const;
    Attachment getColorAttachment(int index) const;
    Attachment getDepthStencilAttachment() const;

    VkFramebuffer getFramebuffer() const { return _framebuffer; }

    VkRenderPass getVkRenderPass() const { return _renderPass; }

private:
    VkFramebuffer ensureFramebuffer(VkRenderPass rp) const;
    VkRenderPass ensureRenderPass(const RenderPassDesc& desc) const;

    VkDevice _device{VK_NULL_HANDLE};

    // Current attachment views for building renderpass/framebuffer
    mutable std::array<VkImageView, MAX_COLOR_ATTCHMENT + 1> _attachmentViews{};
    mutable uint64_t _attachmentViewsHash{0};

    mutable axstd::pod_vector<VkClearValue> _clearValues;

    mutable VkRenderPass _renderPass{VK_NULL_HANDLE};
    mutable VkFramebuffer _framebuffer{VK_NULL_HANDLE};

    // Caches keyed by (desc hash, attachment views hash)
    mutable axstd::hash_map<uint64_t, VkRenderPass> _renderPassCache;
    mutable axstd::hash_map<uint64_t, VkFramebuffer> _framebufferCache;

    mutable bool _attachmentsDirty{true};
};

}  // namespace ax::rhi::vk
