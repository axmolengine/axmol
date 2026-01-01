#pragma once
#include "axmol/rhi/RenderTarget.h"
#include "axmol/platform/GL.h"
#include "axmol/rhi/opengl/OpenGLState.h"

namespace ax::rhi::gl
{

class DriverImpl;

class RenderTargetImpl : public RenderTarget
{
public:
    /*
     * generateFBO, false, use for screen framebuffer
     */
    RenderTargetImpl(DriverImpl* driver, bool defaultRenderTarget);
    ~RenderTargetImpl();

    void setColorTexture(Texture* texture, int level = 0, int index = 0) override;

    void bindFrameBuffer() const;
    void unbindFrameBuffer() const;

    void update();

public:
    GLuint _FBO = 0;
    tlx::pod_vector<GLenum> _GLbufs;
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    EventListenerCustom* _rendererRecreatedListener{nullptr};
#endif
};

}  // namespace ax::rhi::gl
