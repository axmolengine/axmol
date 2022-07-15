#include "RenderTargetGL.h"
#include "DeviceGL.h"
#include "renderer/backend/opengl/MacrosGL.h"

NS_AX_BACKEND_BEGIN

RenderTargetGL::RenderTargetGL(bool defaultRenderTarget, DeviceGL* deviceGL) : RenderTarget(defaultRenderTarget)
{
    if (!defaultRenderTarget)
    {
        glGenFramebuffers(1, &_FBO);
    }
    else
    {
        _FBO = deviceGL->getDefaultFBO();
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
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
}

void RenderTargetGL::unbindFrameBuffer() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTargetGL::update() const {
    if (!_dirty) return;
    if(!_defaultRenderTarget) {
        if(bitmask::any(_flags, TargetBufferFlags::COLOR_ALL))
        { // color attachments
            GLenum bufs[MAX_COLOR_ATTCHMENT] = {GL_NONE};
            for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
            {
                if (bitmask::any(_flags, getMRTColorFlag(i)))
                {
                    auto textureInfo    = _color[i];
                    auto textureHandler = textureInfo.texture != nullptr ? textureInfo.texture->getHandler() : 0;
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureHandler,
                                        textureInfo.level);
                    bufs[i] = GL_COLOR_ATTACHMENT0 + i;
                }
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
