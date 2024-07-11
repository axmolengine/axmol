/****************************************************************************

Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

https://axmol.dev/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

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

void RenderTargetGL::update() const
{
    if (!_dirtyFlags)
        return;
    if (!_defaultRenderTarget)
    {
        if (bitmask::any(_dirtyFlags, TargetBufferFlags::COLOR_ALL))
        {  // color attachments
            GLenum bufs[MAX_COLOR_ATTCHMENT] = {GL_NONE};
            for (size_t i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
            {
                auto textureInfo = _color[i];
                if (textureInfo.texture)
                    bufs[i] = GL_COLOR_ATTACHMENT0 + i;
                if (bitmask::any(_dirtyFlags, getMRTColorFlag(i)))
                    glFramebufferTexture2D(
                        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                        static_cast<GLuint>(textureInfo.texture ? textureInfo.texture->getHandler() : 0),
                        textureInfo.level);
            }
#if AX_GLES_PROFILE != 200
            glDrawBuffers(MAX_COLOR_ATTCHMENT, bufs);
#endif
            CHECK_GL_ERROR_DEBUG();
        }

        if (bitmask::any(_dirtyFlags, TargetBufferFlags::DEPTH))
        {
            // depth attacmhemt
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                                   _depth.texture != nullptr ? _depth.texture->getHandler() : 0, _depth.level);
            CHECK_GL_ERROR_DEBUG();
        }

        if (bitmask::any(_dirtyFlags, TargetBufferFlags::STENCIL))
        {
            // stencil attachment
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
                                   _stencil.texture != nullptr ? _stencil.texture->getHandler() : 0, _stencil.level);
            CHECK_GL_ERROR_DEBUG();
        }
    }

    _dirtyFlags = TargetBufferFlags::NONE;
}

NS_AX_BACKEND_END
