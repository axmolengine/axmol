#include "RenderTargetGL.h"
#include "DeviceGL.h"
#include "renderer/backend/opengl/MacrosGL.h"

CC_BACKEND_BEGIN

RenderTargetGL::RenderTargetGL(bool defaultRenderTarget, DeviceGL* deviceGL) : RenderTarget(defaultRenderTarget)
{
    if (!defaultRenderTarget) {
        glGenFramebuffers(1, &_FBO);
    }
    else {
        _FBO = deviceGL->getDefaultFBO();
    }
}
RenderTargetGL::~RenderTargetGL()
{
    if (!_defaultRenderTarget) {
        bindFrameBuffer();

        for (auto slot = 0; slot < MAX_COLOR_ATTCHMENT; ++slot)
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0 + slot,
                GL_TEXTURE_2D,
                0,
                0);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D,
            0,
            0);

        glFramebufferTexture2D(GL_FRAMEBUFFER,
            GL_STENCIL_ATTACHMENT,
            GL_TEXTURE_2D,
            0,
            0);

        unbindFrameBuffer();
        glDeleteFramebuffers(1, &_FBO);
        CHECK_GL_ERROR_DEBUG();
    }
}

void RenderTargetGL::bindFrameBuffer() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
}

void RenderTargetGL::unbindFrameBuffer() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTargetGL::setColorAttachment(ColorAttachment attachment)
{
    RenderTarget::setColorAttachment(attachment);

    if (!_defaultRenderTarget && bitmask::any(_flags, TargetBufferFlags::COLOR_ALL)) {
        GLenum bufs[MAX_COLOR_ATTCHMENT] = { GL_NONE };
        for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i) {
            if (bitmask::any(_flags, getMRTColorFlag(i))) {
                auto textureInfo = attachment[i];
                auto textureHandler = textureInfo.texture != nullptr ? textureInfo.texture->getHandler() : 0;
                glFramebufferTexture2D(GL_FRAMEBUFFER,
                     GL_COLOR_ATTACHMENT0 + i,
                     GL_TEXTURE_2D,
                    textureHandler,
                    textureInfo.level);
                bufs[i] = GL_COLOR_ATTACHMENT0 + i;
            }
        }
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX
        glDrawBuffers(MAX_COLOR_ATTCHMENT, bufs);
#endif
        CHECK_GL_ERROR_DEBUG();
    }
}

void RenderTargetGL::setDepthAttachment(TextureBackend* attachment, int level)
{
    RenderTarget::setDepthAttachment(attachment, level);

    if (!_defaultRenderTarget) {
        glFramebufferTexture2D(GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D,
            attachment != nullptr ? attachment->getHandler() : 0,
            level);

        CHECK_GL_ERROR_DEBUG();
    }
}

void RenderTargetGL::setStencilAttachment(TextureBackend* attachment, int level)
{
    RenderTarget::setStencilAttachment(attachment, level);

    if (!_defaultRenderTarget) {
        glFramebufferTexture2D(GL_FRAMEBUFFER,
            GL_STENCIL_ATTACHMENT,
            GL_TEXTURE_2D,
            attachment != nullptr ? attachment->getHandler() : 0,
            level);

        CHECK_GL_ERROR_DEBUG();
    }
}

CC_BACKEND_END
