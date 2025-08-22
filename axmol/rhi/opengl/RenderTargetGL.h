#pragma once
#include "axmol/rhi/RenderTarget.h"
#include "axmol/platform/GL.h"
#include "axmol/rhi/opengl/OpenGLState.h"

namespace ax::rhi::gl {

class DriverImpl;

class RenderTargetImpl : public RenderTarget
{
public:
    /*
     * generateFBO, false, use for screen framebuffer
     */
    RenderTargetImpl(bool defaultRenderTarget, DriverImpl* driver);
    ~RenderTargetImpl();

    void bindFrameBuffer() const;
    void unbindFrameBuffer() const;

    void update() const;

public:
    GLuint _FBO = 0;
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    EventListenerCustom* _rendererRecreatedListener{nullptr};
#endif
};

}
