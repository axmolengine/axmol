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

    void bindFrameBuffer() const override;
    void unbindFrameBuffer() const override;

    void setColorAttachment(ColorAttachment attachment) override;
    void setDepthAttachment(TextureBackend* attachment, int level = 0) override;
    void setStencilAttachment(TextureBackend* attachment, int level = 0) override;
public:
    GLuint _FBO = 0;
};

CC_BACKEND_END
