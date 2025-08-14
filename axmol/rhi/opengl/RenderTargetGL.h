#pragma once
#include "../RenderTarget.h"
#include "platform/GL.h"
#include "OpenGLState.h"

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
#if AX_ENABLE_CACHE_TEXTURE_DATA
    EventListenerCustom* _rendererRecreatedListener{nullptr};
#endif
};

}
