#pragma once
#include "axmol/rhi/RenderTarget.h"
#include "axmol/rhi/metal/RenderContextMTL.h"

namespace ax::rhi::mtl
{
class RenderContextImpl;
class RenderTargetImpl : public RenderTarget
{
public:
    struct Attachment
    {
        id<MTLTexture> texture = nil;
        int level              = 0;
        explicit operator bool() const { return texture != nullptr; }
    };

    using NativeColorFormatArray = tlx::inlined_vector<MTLPixelFormat, INITIAL_COLOR_CAPACITY>;

    /*
     * generateFBO, false, use for screen framebuffer
     */
    RenderTargetImpl();
    RenderTargetImpl(RenderContextImpl* context);
    ~RenderTargetImpl();

    void applyRenderPassAttachments(const RenderPassDesc&, MTLRenderPassDescriptor*);

    Attachment getColorAttachment(int index) const;
    Attachment getDepthStencilAttachment() const;

    PixelFormat getColorAttachmentPixelFormat(int index) const;
    PixelFormat getDepthStencilAttachmentPixelFormat() const;

    void rebuildSwapchainAttachments();

    uint32_t getHash() const { return _hash; }

    const NativeColorFormatArray& getNativeColorFormats() const { return _nativeColorFormats; }
    MTLPixelFormat getNativeDSFormat() const { return _nativeDSFormat; }

private:
    RenderContextImpl* _context{nullptr};

    NativeColorFormatArray _nativeColorFormats;
    MTLPixelFormat _nativeDSFormat{MTLPixelFormatInvalid};

    // attachemnt pixel format hash
    uint32_t _hash{0};
};

}  // namespace ax::rhi::mtl
