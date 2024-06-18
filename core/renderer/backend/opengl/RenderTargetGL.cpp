#include "RenderTargetGL.h"
#include "DriverGL.h"
#include "renderer/backend/opengl/MacrosGL.h"

NS_AX_BACKEND_BEGIN

RenderTargetGL::RenderTargetGL(bool defaultRenderTarget, DriverGL* driver) : RenderTarget(defaultRenderTarget)
{
    if (!defaultRenderTarget)
    {
        glGenFramebuffers(1, &_FBO);
    }
    else
    {
        _FBO = driver->getDefaultFBO();
    }
}
RenderTargetGL::~RenderTargetGL()
{
    if (!_defaultRenderTarget)
    {
        bindFrameBuffer();

        for (auto slot = 0; slot < MAX_COLOR_ATTCHMENT; ++slot)
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_TEXTURE_2D, 0, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

        unbindFrameBuffer();
        glDeleteFramebuffers(1, &_FBO);
        CHECK_GL_ERROR_DEBUG();
    }
}

void RenderTargetGL::bindFrameBuffer() const
{
    __gl->bindFrameBuffer(_FBO);
}

void RenderTargetGL::unbindFrameBuffer() const
{
    __gl->bindFrameBuffer(0);
}

void RenderTargetGL::update() const {
    if (!_dirty) return;
    if(!_defaultRenderTarget) {
        { // color attachments
            GLenum bufs[MAX_COLOR_ATTCHMENT] = {GL_NONE};
            for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
            {
                GLuint texture = 0;
                GLint level = 0;
                if (_color[i])
                {
                    texture = static_cast<GLuint>(_color[i].texture->getHandler());
                    level = _color[i].level;
                    bufs[i] = GL_COLOR_ATTACHMENT0 + i;
                }
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture, level);
            }
    #if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32 || AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
            glDrawBuffers(MAX_COLOR_ATTCHMENT, bufs);
    #endif
            CHECK_GL_ERROR_DEBUG();
        }

        // depth attacmhemt
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                               _depth.texture != nullptr ? _depth.texture->getHandler() : 0, _depth.level);
        CHECK_GL_ERROR_DEBUG();

        // stencil attachment
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
                               _stencil.texture != nullptr ? _stencil.texture->getHandler() : 0, _stencil.level);
        CHECK_GL_ERROR_DEBUG();
    }

    _dirty = false;
}

NS_AX_BACKEND_END
