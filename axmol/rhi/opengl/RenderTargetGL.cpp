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

#include "axmol/rhi/opengl/RenderTargetGL.h"
#include "axmol/rhi/opengl/DriverGL.h"
#include "axmol/rhi/opengl/MacrosGL.h"
#include "axmol/rhi/opengl/TextureGL.h"

namespace ax::rhi::gl {

RenderTargetImpl::RenderTargetImpl(bool defaultRenderTarget, DriverImpl* driver) : RenderTarget(defaultRenderTarget)
{
    if (!defaultRenderTarget)
    {
        glGenFramebuffers(1, &_FBO);
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
        _rendererRecreatedListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom*) {
            glGenFramebuffers(1, &_FBO);
            _dirtyFlags = TargetBufferFlags::ALL;
        });
        Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_rendererRecreatedListener,
                                                                                         -1);
#endif
    }
    else
    {
        _FBO = driver->getDefaultFBO();
    }
}
RenderTargetImpl::~RenderTargetImpl()
{
    if (!_defaultRenderTarget)
    {
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
        Director::getInstance()->getEventDispatcher()->removeEventListener(_rendererRecreatedListener);
#endif

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

void RenderTargetImpl::bindFrameBuffer() const
{
    __state->bindFrameBuffer(_FBO);
}

void RenderTargetImpl::unbindFrameBuffer() const
{
    __state->bindFrameBuffer(0);
}

void RenderTargetImpl::update() const
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
                        textureInfo.texture ? static_cast<TextureImpl*>(textureInfo.texture)->internalHandle() : 0,
                        textureInfo.level);
            }

            glDrawBuffers(MAX_COLOR_ATTCHMENT, bufs);

            CHECK_GL_ERROR_DEBUG();
        }

        if (bitmask::any(_dirtyFlags, TargetBufferFlags::DEPTH_AND_STENCIL))
        {
            // depth stencil attachment
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
                                   _depthStencil.texture != nullptr
                                       ? static_cast<TextureImpl*>(_depthStencil.texture)->internalHandle()
                                       : 0,
                                   _depthStencil.level);
            CHECK_GL_ERROR_DEBUG();
        }
    }

    _dirtyFlags = TargetBufferFlags::NONE;
}

}
