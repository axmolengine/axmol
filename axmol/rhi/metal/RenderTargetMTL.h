#pragma once
#include "../RenderTarget.h"
#include "CommandBufferMTL.h"

namespace ax::rhi::mtl {

class RenderTargetImpl : public RenderTarget
{
public:
    struct Attachment
    {
        id<MTLTexture> texture = nil;
        int level              = 0;
        explicit operator bool() const { return texture != nullptr; }
    };

    /*
     * generateFBO, false, use for screen framebuffer
     */
    RenderTargetImpl(bool defaultRenderTarget);
    ~RenderTargetImpl();

    void applyRenderPassAttachments(const RenderPassDescriptor&, MTLRenderPassDescriptor*) const;

    Attachment getColorAttachment(int index) const;
    Attachment getDepthStencilAttachment() const;

    PixelFormat getColorAttachmentPixelFormat(int index) const;
    PixelFormat getDepthStencilAttachmentPixelFormat() const;
};

}
