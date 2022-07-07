#pragma once
#include "../RenderTarget.h"
#include "platform/CCGL.h"

CC_BACKEND_BEGIN

class DeviceGL;

class RenderTargetGL : public RenderTarget
{
public:
    /*
     * generateFBO, false, use for screen framebuffer
     */
    RenderTargetGL(bool defaultRenderTarget, DeviceGL* deviceGL);
    ~RenderTargetGL();

    void bindFrameBuffer() const;
    void unbindFrameBuffer() const;

    void update() const;

public:
    GLuint _FBO = 0;
};

CC_BACKEND_END
