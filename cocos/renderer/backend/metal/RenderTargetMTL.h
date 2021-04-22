#pragma once
#include "../RenderTarget.h"
#include "CommandBufferMTL.h"

CC_BACKEND_BEGIN

class RenderTargetMTL : public RenderTarget
{
public:
    struct Attachment {
        id<MTLTexture> texture = nil;
        int level = 0;
        explicit operator bool() const {
            return texture != nullptr;
        }
    };
    
    /*
    * generateFBO, false, use for screen framebuffer
    */
    RenderTargetMTL(bool defaultRenderTarget);
    ~RenderTargetMTL();

    void bindFrameBuffer() const override;
    void unbindFrameBuffer() const override;

    void setColorAttachment(ColorAttachment attachment) override;
    void setDepthAttachment(TextureBackend* attachment, int level = 0) override;
    void setStencilAttachment(TextureBackend* attachment, int level = 0) override;
    
    void applyRenderPassAttachments(const RenderPassParams& parmas, MTLRenderPassDescriptor* descriptor) const;
    
    Attachment getColorAttachment(int index) const;
    Attachment getDepthAttachment() const;
    Attachment getStencilAttachment() const;
    
    PixelFormat getColorAttachmentPixelFormat(int index) const;
    PixelFormat getDepthAttachmentPixelFormat() const;
    PixelFormat getStencilAttachmentPixelFormat() const;
public:

    // "Sidecar" textures used to implement automatic MSAA resolve.
    // id<MTLTexture> multisampledColor[MRT::TARGET_COUNT] = { 0 };
    // id<MTLTexture> multisampledDepth = nil;
    // MetalContext*, DeviceMTL*
};

CC_BACKEND_END
