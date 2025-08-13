#include "RenderTargetMTL.h"
#include "UtilsMTL.h"
#include "TextureMTL.h"

namespace ax::backend {

static MTLLoadAction getLoadAction(const RenderPassDescriptor& params, TargetBufferFlags buffer)
{
    const auto clearFlags        = (TargetBufferFlags)params.flags.clear;
    const auto discardStartFlags = params.flags.discardStart;
    if (bitmask::any(clearFlags, buffer))
    {
        return MTLLoadActionClear;
    }
    else if (bitmask::any(discardStartFlags, buffer))
    {
        return MTLLoadActionDontCare;
    }
    return MTLLoadActionLoad;
}

static MTLStoreAction getStoreAction(const RenderPassDescriptor& params, TargetBufferFlags buffer)
{
    const auto discardEndFlags = params.flags.discardEnd;
    if (bitmask::any(discardEndFlags, buffer))
    {
        return MTLStoreActionDontCare;
    }
    return MTLStoreActionStore;
}

RenderTargetMTL::RenderTargetMTL(bool defaultRenderTarget) : RenderTarget(defaultRenderTarget) {}
RenderTargetMTL::~RenderTargetMTL() {}

void RenderTargetMTL::applyRenderPassAttachments(const RenderPassDescriptor& params,
                                                 MTLRenderPassDescriptor* descriptor) const
{
    // const auto discardFlags = params.flags.discardEnd;
    auto clearFlags = params.flags.clear;

    for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; i++)
    {
        auto attachment = getColorAttachment(static_cast<int>(i));
        if (!attachment)
        {
            continue;
        }

        const auto MRTColorFlag = getMRTColorFlag(i);

        descriptor.colorAttachments[i].texture = attachment.texture;
        descriptor.colorAttachments[i].level   = attachment.level;
        // descriptor.colorAttachments[i].slice = attachment.layer;
        descriptor.colorAttachments[i].loadAction  = getLoadAction(params, MRTColorFlag);
        descriptor.colorAttachments[i].storeAction = getStoreAction(params, MRTColorFlag);
        if (bitmask::any(clearFlags, MRTColorFlag))
            descriptor.colorAttachments[i].clearColor =
                MTLClearColorMake(params.clearColorValue[0], params.clearColorValue[1], params.clearColorValue[2],
                                params.clearColorValue[3]);
    }

    // Sets descriptor depth and stencil params, should match RenderTargetMTL::chooseAttachmentFormat
    {
        auto depthStencilAttachment = getDepthStencilAttachment();
        if (depthStencilAttachment)
        {
            // depth
            descriptor.depthAttachment.texture = depthStencilAttachment.texture;
            descriptor.depthAttachment.level   = depthStencilAttachment.level;
            // descriptor.depthAttachment.slice = depthAttachment.layer;
            descriptor.depthAttachment.loadAction  = getLoadAction(params, TargetBufferFlags::DEPTH);
            descriptor.depthAttachment.storeAction = getStoreAction(params, TargetBufferFlags::DEPTH);
            if (bitmask::any(clearFlags, TargetBufferFlags::DEPTH))
                descriptor.depthAttachment.clearDepth = params.clearDepthValue;

            // depth
            descriptor.stencilAttachment.texture = depthStencilAttachment.texture;
            descriptor.stencilAttachment.level   = depthStencilAttachment.level;
            // descriptor.stencilAttachment.slice = depthAttachment.layer;
            descriptor.stencilAttachment.loadAction  = getLoadAction(params, TargetBufferFlags::STENCIL);
            descriptor.stencilAttachment.storeAction = getStoreAction(params, TargetBufferFlags::STENCIL);
            if (bitmask::any(clearFlags, TargetBufferFlags::STENCIL))
                descriptor.stencilAttachment.clearStencil = params.clearStencilValue;
        }
    }

    _dirtyFlags = TargetBufferFlags::NONE;
}

RenderTargetMTL::Attachment RenderTargetMTL::getColorAttachment(int index) const
{
    if (isDefaultRenderTarget() && index == 0)
        return {DriverMTL::getCurrentDrawable().texture, 0};
    auto& rb = this->_color[index];
    return RenderTargetMTL::Attachment{static_cast<bool>(rb) ? static_cast<TextureImpl*>(rb.texture)->internalHandle() : nil,
                                       rb.level};
}

RenderTargetMTL::Attachment RenderTargetMTL::getDepthStencilAttachment() const
{
    if (isDefaultRenderTarget())
        return {UtilsMTL::getDefaultDepthStencilTexture(), 0};
    auto& rb = this->_depthStencil;
    return RenderTargetMTL::Attachment{!!rb ? static_cast<TextureImpl*>(rb.texture)->internalHandle() : nil, rb.level};
}

PixelFormat RenderTargetMTL::getColorAttachmentPixelFormat(int index) const
{
    // !!!important
    // the default framebuffer pixel format is: MTLPixelFormatBGRA8Unorm
    if (isDefaultRenderTarget() && index == 0)
        return PixelFormat::BGRA8;
    auto& rb = this->_color[index];
    return rb ? rb.texture->getTextureFormat() : PixelFormat::NONE;
}

PixelFormat RenderTargetMTL::getDepthStencilAttachmentPixelFormat() const
{  // FIXME: axmol only support D24S8
    if (isDefaultRenderTarget())
        return PixelFormat::D24S8;
    if (_depthStencil)
        return _depthStencil.texture->getTextureFormat();
    return PixelFormat::NONE;
}

}
