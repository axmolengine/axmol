#pragma once
#include "axmol/rhi/RenderTarget.h"
#include "axmol/platform/GL.h"
#include "axmol/rhi/opengl/OpenGLState.h"
#include "axmol/tlx/vector.hpp"

namespace ax::rhi::gl
{

class GraphicsDeviceImpl;

class RenderTargetImpl : public RenderTarget
{
public:
    /*
     * generateFBO, false, use for screen framebuffer
     */
    RenderTargetImpl(GraphicsDeviceImpl* driver, bool defaultRenderTarget);
    ~RenderTargetImpl();

    void setColorTexture(Texture* texture, int level = 0, int index = 0) override;

    void bindFrameBuffer() const;
    void unbindFrameBuffer() const;

    PixelFormat getColorAttachmentPixelFormat(int index = 0) const override;
    PixelFormat getDepthStencilAttachmentPixelFormat() const override;

    void update();

public:
    GLuint _FBO = 0;
    tlx::pod_vector<GLenum> _GLbufs;
    mutable PixelFormat _defaultColorAttachmentPixelFormat{PixelFormat::NONE};
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    CustomEventListener* _rendererRecreatedListener{nullptr};
#endif
};

}  // namespace ax::rhi::gl
