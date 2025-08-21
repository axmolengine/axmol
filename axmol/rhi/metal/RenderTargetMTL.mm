#include "axmol/rhi/metal/RenderTargetMTL.h"
#include "axmol/rhi/metal/UtilsMTL.h"
#include "axmol/rhi/metal/TextureMTL.h"

namespace ax::rhi::mtl {

static MTLLoadAction getLoadAction(const RenderPassDesc& params, TargetBufferFlags buffer)
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

static MTLStoreAction getStoreAction(const RenderPassDesc& params, TargetBufferFlags buffer)
{
    const auto discardEndFlags = params.flags.discardEnd;
    if (bitmask::any(discardEndFlags, buffer))
    {
        return MTLStoreActionDontCare;
    }
    return MTLStoreActionStore;
}

RenderTargetImpl::RenderTargetImpl(bool defaultRenderTarget) : RenderTarget(defaultRenderTarget) {}
RenderTargetImpl::~RenderTargetImpl() {}

void RenderTargetImpl::applyRenderPassAttachments(const RenderPassDesc& params,
                                                 MTLRenderPassDescriptor* desc) const
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

        desc.colorAttachments[i].texture = attachment.texture;
        desc.colorAttachments[i].level   = attachment.level;
        // desc.colorAttachments[i].slice = attachment.layer;
        desc.colorAttachments[i].loadAction  = getLoadAction(params, MRTColorFlag);
        desc.colorAttachments[i].storeAction = getStoreAction(params, MRTColorFlag);
        if (bitmask::any(clearFlags, MRTColorFlag))
            desc.colorAttachments[i].clearColor =
                MTLClearColorMake(params.clearColorValue[0], params.clearColorValue[1], params.clearColorValue[2],
                                params.clearColorValue[3]);
    }

    // Sets descriptor depth and stencil params, should match RenderTargetImpl::chooseAttachmentFormat
    {
        auto depthStencilAttachment = getDepthStencilAttachment();
        if (depthStencilAttachment)
        {
            // depth
            desc.depthAttachment.texture = depthStencilAttachment.texture;
            desc.depthAttachment.level   = depthStencilAttachment.level;
            // desc.depthAttachment.slice = depthAttachment.layer;
            desc.depthAttachment.loadAction  = getLoadAction(params, TargetBufferFlags::DEPTH);
            desc.depthAttachment.storeAction = getStoreAction(params, TargetBufferFlags::DEPTH);
            if (bitmask::any(clearFlags, TargetBufferFlags::DEPTH))
                desc.depthAttachment.clearDepth = params.clearDepthValue;

            // depth
            desc.stencilAttachment.texture = depthStencilAttachment.texture;
            desc.stencilAttachment.level   = depthStencilAttachment.level;
            // desc.stencilAttachment.slice = depthAttachment.layer;
            desc.stencilAttachment.loadAction  = getLoadAction(params, TargetBufferFlags::STENCIL);
            desc.stencilAttachment.storeAction = getStoreAction(params, TargetBufferFlags::STENCIL);
            if (bitmask::any(clearFlags, TargetBufferFlags::STENCIL))
                desc.stencilAttachment.clearStencil = params.clearStencilValue;
        }
    }

    _dirtyFlags = TargetBufferFlags::NONE;
}

RenderTargetImpl::Attachment RenderTargetImpl::getColorAttachment(int index) const
{
    if (isDefaultRenderTarget() && index == 0)
        return {DriverImpl::getCurrentDrawable().texture, 0};
    auto& rb = this->_color[index];
    return RenderTargetImpl::Attachment{static_cast<bool>(rb) ? static_cast<TextureImpl*>(rb.texture)->internalHandle() : nil,
                                       rb.level};
}

RenderTargetImpl::Attachment RenderTargetImpl::getDepthStencilAttachment() const
{
    if (isDefaultRenderTarget())
        return {UtilsMTL::getDefaultDepthStencilTexture(), 0};
    auto& rb = this->_depthStencil;
    return RenderTargetImpl::Attachment{!!rb ? static_cast<TextureImpl*>(rb.texture)->internalHandle() : nil, rb.level};
}

PixelFormat RenderTargetImpl::getColorAttachmentPixelFormat(int index) const
{
    // !!!important
    // the default framebuffer pixel format is: MTLPixelFormatBGRA8Unorm
    if (isDefaultRenderTarget() && index == 0)
        return PixelFormat::BGRA8;
    auto& rb = this->_color[index];
    return rb ? rb.texture->getPixelFormat() : PixelFormat::NONE;
}

PixelFormat RenderTargetImpl::getDepthStencilAttachmentPixelFormat() const
{  // FIXME: axmol only support D24S8
    if (isDefaultRenderTarget())
        return PixelFormat::D24S8;
    if (_depthStencil)
        return _depthStencil.texture->getPixelFormat();
    return PixelFormat::NONE;
}

}
