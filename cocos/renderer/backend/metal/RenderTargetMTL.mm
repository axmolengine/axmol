#include "RenderTargetMTL.h"
#include "UtilsMTL.h"

CC_BACKEND_BEGIN

static MTLLoadAction getLoadAction(const RenderPassParams& params,
                                   TargetBufferFlags buffer) {
    const auto clearFlags = (TargetBufferFlags) params.flags.clear;
    const auto discardStartFlags = params.flags.discardStart;
    if (bitmask::any(clearFlags, buffer)) {
        return MTLLoadActionClear;
    } else if (bitmask::any(discardStartFlags, buffer)) {
        return MTLLoadActionDontCare;
    }
    return MTLLoadActionLoad;
}

static MTLStoreAction getStoreAction(const RenderPassParams& params,
                                     TargetBufferFlags buffer) {
    const auto discardEndFlags = params.flags.discardEnd;
    if (bitmask::any(discardEndFlags, buffer)) {
        return MTLStoreActionDontCare;
    }
    return MTLStoreActionStore;
}

RenderTargetMTL::RenderTargetMTL(bool defaultRenderTarget) : RenderTarget(defaultRenderTarget)
{
    
}
RenderTargetMTL::~RenderTargetMTL()
{
}

void RenderTargetMTL::bindFrameBuffer() const
{
}

void RenderTargetMTL::unbindFrameBuffer() const
{
}
void RenderTargetMTL::setColorAttachment(ColorAttachment attachment)
{
    RenderTarget::setColorAttachment(attachment);
    
    
}

void RenderTargetMTL::setDepthAttachment(TextureBackend* attachment, int level)
{
    RenderTarget::setDepthAttachment(attachment, level);
    
    
}

void RenderTargetMTL::setStencilAttachment(TextureBackend* attachment, int level)
{
    RenderTarget::setStencilAttachment(attachment, level);
    
}

void RenderTargetMTL::applyRenderPassAttachments(const RenderPassParams& params, MTLRenderPassDescriptor* descriptor) const
{
    const auto discardFlags = params.flags.discardEnd;
    
    for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; i++) {
        auto attachment = getColorAttachment(i);
        if (!attachment) {
            continue;
        }
        
#if 1
        descriptor.colorAttachments[i].texture = attachment.texture;
        descriptor.colorAttachments[i].level = attachment.level;
        // descriptor.colorAttachments[i].slice = attachment.layer;
        descriptor.colorAttachments[i].loadAction = getLoadAction(params, getMRTColorFlag(i));
        descriptor.colorAttachments[i].storeAction = getStoreAction(params, getMRTColorFlag(i));
        descriptor.colorAttachments[i].clearColor = MTLClearColorMake(
                                                                      params.clearColorValue[0], params.clearColorValue[1], params.clearColorValue[2], params.clearColorValue[3]);
#endif
#if 0
        if (multisampledColor[i]) {
            // We're rendering into our temporary MSAA texture and doing an automatic resolve.
            // We should not be attempting to load anything into the MSAA texture.
            assert(descriptor.colorAttachments[i].loadAction != MTLLoadActionLoad);
            
            descriptor.colorAttachments[i].texture = multisampledColor[i];
            descriptor.colorAttachments[i].level = 0;
            descriptor.colorAttachments[i].slice = 0;
            const bool discard = any(discardFlags & getMRTColorFlag(i));
            if (!discard) {
                descriptor.colorAttachments[i].resolveTexture = attachment.texture;
                descriptor.colorAttachments[i].resolveLevel = attachment.level;
                descriptor.colorAttachments[i].resolveSlice = attachment.layer;
                descriptor.colorAttachments[i].storeAction = MTLStoreActionMultisampleResolve;
            }
        }
#endif
    }
    
    auto depthAttachment = getDepthAttachment();
    if(depthAttachment){
        descriptor.depthAttachment.texture = depthAttachment.texture;
        descriptor.depthAttachment.level = depthAttachment.level;
        // descriptor.depthAttachment.slice = depthAttachment.layer;
        descriptor.depthAttachment.loadAction = getLoadAction(params, TargetBufferFlags::DEPTH);
        descriptor.depthAttachment.storeAction = getStoreAction(params, TargetBufferFlags::DEPTH);
        descriptor.depthAttachment.clearDepth = params.clearDepthValue;
    }
    
    auto stencilAttachment = getStencilAttachment();
    if(stencilAttachment) {
        descriptor.stencilAttachment.texture = stencilAttachment.texture;
        descriptor.stencilAttachment.level = depthAttachment.level;
        // descriptor.stencilAttachment.slice = depthAttachment.layer;
        descriptor.stencilAttachment.loadAction = getLoadAction(params, TargetBufferFlags::STENCIL);
        descriptor.stencilAttachment.storeAction = getStoreAction(params, TargetBufferFlags::STENCIL);
        descriptor.stencilAttachment.clearStencil= params.clearStencilValue;
    }
#if 0
    if (multisampledDepth) {
        // We're rendering into our temporary MSAA texture and doing an automatic resolve.
        // We should not be attempting to load anything into the MSAA texture.
        assert(descriptor.depthAttachment.loadAction != MTLLoadActionLoad);
        
        descriptor.depthAttachment.texture = multisampledDepth;
        descriptor.depthAttachment.level = 0;
        descriptor.depthAttachment.slice = 0;
        const bool discard = any(discardFlags & TargetBufferFlags::DEPTH);
        if (!discard) {
            descriptor.depthAttachment.resolveTexture = depthAttachment.texture;
            descriptor.depthAttachment.resolveLevel = depthAttachment.level;
            descriptor.depthAttachment.resolveSlice = depthAttachment.layer;
            descriptor.depthAttachment.storeAction = MTLStoreActionMultisampleResolve;
        }
    }
#endif
}

RenderTargetMTL::Attachment RenderTargetMTL::getColorAttachment(int index) const
{
    if(isDefaultRenderTarget() && index == 0)
        return {DeviceMTL::getCurrentDrawable().texture, 0};
    auto& rb = this->_color[index];
    return RenderTargetMTL::Attachment{static_cast<bool>(rb) ? (id<MTLTexture>)(rb.texture->getHandler()) : nil, rb.level};
}

RenderTargetMTL::Attachment RenderTargetMTL::getDepthAttachment() const
{
    if(isDefaultRenderTarget())
        return {UtilsMTL::getDefaultDepthStencilTexture(), 0};
    auto& rb = this->_depth;
    return RenderTargetMTL::Attachment{!!rb ? (id<MTLTexture>)(rb.texture->getHandler()) : nil, rb.level};
}

RenderTargetMTL::Attachment RenderTargetMTL::getStencilAttachment() const
{
    if(isDefaultRenderTarget())
        return {UtilsMTL::getDefaultDepthStencilTexture(), 0};
    auto& rb = this->_stencil;
    return RenderTargetMTL::Attachment{!!rb ? (id<MTLTexture>)(rb.texture->getHandler()) : nil, rb.level};
}

PixelFormat RenderTargetMTL::getColorAttachmentPixelFormat(int index) const
{
    if(isDefaultRenderTarget() && index == 0)
        return PixelFormat::DEFAULT;
    auto& rb = this->_color[index];
    return rb ? rb.texture->getTextureFormat() : PixelFormat::DEFAULT;
}

PixelFormat RenderTargetMTL::getDepthAttachmentPixelFormat() const
{
    if(isDefaultRenderTarget() || !_depth)
        return PixelFormat::D24S8;
    return _depth.texture->getTextureFormat();
}

PixelFormat RenderTargetMTL::getStencilAttachmentPixelFormat() const
{
    if(isDefaultRenderTarget() || !_stencil)
        return PixelFormat::D24S8;
    return _stencil.texture->getTextureFormat();
}

CC_BACKEND_END
