#include "axmol/rhi/metal/RenderTargetMTL.h"
#include "axmol/rhi/metal/UtilsMTL.h"
#include "axmol/rhi/metal/TextureMTL.h"
#include "axmol/rhi/metal/RenderContextMTL.h"
#include "axmol/rhi/metal/DriverMTL.h"

namespace ax::rhi::mtl
{

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

RenderTargetImpl::RenderTargetImpl() : RenderTarget(false) {}
RenderTargetImpl::RenderTargetImpl(RenderContextImpl* context) : RenderTarget(true), _context(context) {}
RenderTargetImpl::~RenderTargetImpl() {}

void RenderTargetImpl::applyRenderPassAttachments(const RenderPassDesc& params, MTLRenderPassDescriptor* desc) const
{
    // const auto discardFlags = params.flags.discardEnd;
    auto clearFlags = params.flags.clear;

    for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; i++)
    {
        auto attachment = getColorAttachment(static_cast<int>(i));
        if (!attachment)
        {
            break;  // assume MRT must store contiguous
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

void RenderTargetImpl::rebuildSwapchainAttachments()
{
    AX_SAFE_RELEASE_NULL(_depthStencil.texture);

    auto mtlLayer = _context->getMetalLayer();

    auto width  = mtlLayer.drawableSize.width;
    auto height = mtlLayer.drawableSize.height;

    // depth-stencil
    TextureDesc depthDesc{};
    depthDesc.textureType  = TextureType::TEXTURE_2D;
    depthDesc.width        = static_cast<uint16_t>(width);
    depthDesc.height       = static_cast<uint16_t>(height);
    depthDesc.arraySize    = 1;
    depthDesc.mipLevels    = 1;
    depthDesc.pixelFormat  = PixelFormat::D24S8;
    depthDesc.textureUsage = TextureUsage::RENDER_TARGET;

    auto mtlDevice = static_cast<DriverImpl*>(DriverBase::getInstance())->getMTLDevice();
    auto tex       = new TextureImpl(mtlDevice, depthDesc);
    // ensure native texture
    tex->updateData(nullptr, width, height, 0);
    _depthStencil.texture = tex;

    _dirtyFlags = TargetBufferFlags::COLOR0 | TargetBufferFlags::DEPTH_AND_STENCIL;
}

RenderTargetImpl::Attachment RenderTargetImpl::getColorAttachment(int index) const
{
    if (isDefaultRenderTarget())
        return index == 0 ? Attachment{_context->acquireDrawable().texture, 0} : Attachment{};
    auto& rb = this->_color[index];
    return RenderTargetImpl::Attachment{
        static_cast<bool>(rb) ? static_cast<TextureImpl*>(rb.texture)->internalHandle() : nil, rb.level};
}

RenderTargetImpl::Attachment RenderTargetImpl::getDepthStencilAttachment() const
{
    if (isDefaultRenderTarget())
        return {static_cast<TextureImpl*>(_depthStencil.texture)->internalHandle(), 0};
    return RenderTargetImpl::Attachment{
        !!_depthStencil ? static_cast<TextureImpl*>(_depthStencil.texture)->internalHandle() : nil,
        _depthStencil.level};
}

PixelFormat RenderTargetImpl::getColorAttachmentPixelFormat(int index) const
{
    // !!!important
    // the default framebuffer pixel format is: MTLPixelFormatBGRA8Unorm
    if (isDefaultRenderTarget())
        return index == 0 ? PixelFormat::BGRA8 : PixelFormat::NONE;
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

}  // namespace ax::rhi::mtl
