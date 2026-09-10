/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/RenderTarget.h"
#include "axmol/rhi/vulkan/TextureVK.h"
#include <glad/vulkan.h>

namespace ax::rhi::vk
{
class GraphicsDeviceImpl;
class RenderTargetImpl : public RenderTarget
{
public:
    using Attachment = TextureImpl*;

    RenderTargetImpl(GraphicsDeviceImpl* driver, bool defaultRenderTarget);
    ~RenderTargetImpl();

    // Destroy the current live framebuffer and mark attachments dirty
    void cleanupResources() override;

    // Begin a render pass using this target
    void beginRenderPass(VkCommandBuffer cmd,
                         const RenderPassDesc& desc,
                         uint32_t width,
                         uint32_t height,
                         uint32_t imageIndex);

    void endRenderPass(VkCommandBuffer cmd);

    Attachment getColorAttachment(int index) const;
    Attachment getDepthStencilAttachment() const;

    VkFramebuffer getFramebuffer() const { return _framebuffer; }

    VkRenderPass getVkRenderPass() const { return _renderPass; }

    void rebuildSwapchainAttachments(const tlx::pod_vector<VkImage>& images,
                                     const VkExtent2D&,
                                     PixelFormat imagePF,
                                     VkFormat surfaceFormat);

    void setColorTexture(Texture* texture, int level = 0, int index = 0) override;

    uint32_t getColorAttachmentCount() const { return _numMRT; }

private:
    void updateRenderPass(const RenderPassDesc& desc, uint32_t imageIndex);
    void updateFramebuffer(VkCommandBuffer cmd, uint32_t imageIndex);

    void prepareAttachmentsForRendering(VkCommandBuffer cmd);

    GraphicsDeviceImpl* _driver{nullptr};

    // Current attachment views for building renderpass/framebuffer
    tlx::inlined_vector<VkImageView, INITIAL_COLOR_CAPACITY + 1> _attachmentViews{};

    // Seed values used to compute framebuffer/render pass hash per swapchain image
    // only used for screen render target
    tlx::inlined_vector<uint64_t, INITIAL_COLOR_CAPACITY> _renderHashSeeds{};

    tlx::pod_vector<VkClearValue> _clearValues;

    uint32_t _numMRT{0};  // number of color attachments, used for render pass creation

    uint64_t _activeHashSeed{0};

    VkRenderPass _renderPass{VK_NULL_HANDLE};    // active render pass
    VkFramebuffer _framebuffer{VK_NULL_HANDLE};  // active framebuffer

    tlx::pod_vector<VkImageView> _swapchainImageViews;

    // Caches keyed by (desc hash, attachment views hash)
    tlx::hash_map<uint64_t, VkRenderPass> _renderPassCache;
    tlx::hash_map<uint64_t, VkFramebuffer> _framebufferCache;
};

}  // namespace ax::rhi::vk
