/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
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

#include "axmol/rhi/opengl/RenderContextGL.h"
#include "axmol/rhi/opengl/BufferGL.h"
#include "axmol/rhi/opengl/RenderPipelineGL.h"
#include "axmol/rhi/opengl/TextureGL.h"
#include "axmol/rhi/opengl/DepthStencilStateGL.h"
#include "axmol/rhi/opengl/ProgramGL.h"
#include "axmol/rhi/opengl/MacrosGL.h"
#include "axmol/rhi/opengl/UtilsGL.h"
#include "axmol/rhi/opengl/RenderTargetGL.h"
#include "axmol/rhi/opengl/DriverGL.h"
#include "axmol/rhi/opengl/VertexLayoutGL.h"

#include "axmol/base/EventDispatcher.h"
#include "axmol/base/EventType.h"
#include "axmol/base/Director.h"

#include <algorithm>

namespace ax::rhi::gl
{

// !!Note: WebGL 2.0 on (Open GELS 3.0) not support map buffer range
#if AX_TARGET_PLATFORM != AX_PLATFORM_WASM
#    define AX_HAVE_MAP_BUFFER_RANGE 1
#else
#    define AX_HAVE_MAP_BUFFER_RANGE 0
#endif

RenderContextImpl::RenderContextImpl(DriverImpl* driver)
{
    _screenRT = new RenderTargetImpl(driver, true);
}

RenderContextImpl::~RenderContextImpl()
{
    cleanResources();

    AX_SAFE_RELEASE_NULL(_screenRT);
    AX_SAFE_RELEASE_NULL(_renderPipeline);
}

bool RenderContextImpl::beginFrame()
{
    return true;
}

void RenderContextImpl::beginRenderPass(RenderTarget* rt, const RenderPassDesc& descriptor)
{
    auto rtGL = static_cast<RenderTargetImpl*>(rt);

    rtGL->bindFrameBuffer();
    rtGL->update();

    auto clearFlags = descriptor.flags.clear;

    // set clear color, depth and stencil
    GLbitfield mask = 0;
    if (bitmask::any(clearFlags, TargetBufferFlags::COLOR))
    {
        mask |= GL_COLOR_BUFFER_BIT;
        const auto& clearColor = descriptor.clearColorValue;
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    }

    CHECK_GL_ERROR_DEBUG();

    GLboolean oldDepthWrite    = GL_FALSE;
    GLboolean oldDepthTest     = GL_FALSE;
    GLfloat oldDepthClearValue = 0.f;
    GLint oldDepthFunc         = GL_LESS;
    if (bitmask::any(clearFlags, TargetBufferFlags::DEPTH))
    {
        glGetBooleanv(GL_DEPTH_WRITEMASK, &oldDepthWrite);
        glGetBooleanv(GL_DEPTH_TEST, &oldDepthTest);
        glGetFloatv(GL_DEPTH_CLEAR_VALUE, &oldDepthClearValue);
        glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFunc);

        mask |= GL_DEPTH_BUFFER_BIT;
        glClearDepth(descriptor.clearDepthValue);
        __state->enableDepthTest();

        __state->depthMask(GL_TRUE);
        __state->depthFunc(GL_ALWAYS);
    }

    CHECK_GL_ERROR_DEBUG();

    if (bitmask::any(clearFlags, TargetBufferFlags::STENCIL))
    {
        mask |= GL_STENCIL_BUFFER_BIT;
        glClearStencil(descriptor.clearStencilValue);
    }

    if (mask)
        glClear(mask);

    CHECK_GL_ERROR_DEBUG();

    // restore depth test
    if (bitmask::any(clearFlags, TargetBufferFlags::DEPTH))
    {
        if (!oldDepthTest)
            __state->disableDepthTest();

        __state->depthMask(oldDepthWrite);
        __state->depthFunc(oldDepthFunc);
        glClearDepth(oldDepthClearValue);
    }

    CHECK_GL_ERROR_DEBUG();
}

void RenderContextImpl::setDepthStencilState(DepthStencilState* depthStencilState)
{
    _depthStencilStateImpl = static_cast<DepthStencilStateImpl*>(depthStencilState);
}

void RenderContextImpl::setRenderPipeline(RenderPipeline* renderPipeline)
{
    Object::assign(_renderPipeline, static_cast<RenderPipelineImpl*>(renderPipeline));
}

/**
 * Update depthStencil status, improvment: for metal backend cache it
 * @param depthStencilState Specifies the depth and stencil status
 */
void RenderContextImpl::updateDepthStencilState(const DepthStencilDesc& desc)
{
    _depthStencilStateImpl->update(desc);
}

/**
 * Update render pipeline status
 * @param depthStencilState Specifies the depth and stencil status
 */
void RenderContextImpl::updatePipelineState(const RenderTarget* rt,
                                            const PipelineDesc& desc,
                                            PrimitiveGroup primitiveGroup)
{
    RenderContext::updatePipelineState(rt, desc, primitiveGroup);

    _renderPipeline->update(rt, desc);
}

void RenderContextImpl::setViewport(int x, int y, unsigned int w, unsigned int h)
{
    __state->viewport(_viewport.set(x, y, w, h));
}

void RenderContextImpl::setCullMode(CullMode mode)
{
    _cullMode = mode;
}

void RenderContextImpl::setWinding(Winding winding)
{
    __state->winding(UtilsGL::toGLFrontFace(winding));
}

void RenderContextImpl::setVertexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _vertexBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_vertexBuffer);
    _vertexBuffer = static_cast<BufferImpl*>(buffer);
}

void RenderContextImpl::setIndexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _indexBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_indexBuffer);
    _indexBuffer = static_cast<BufferImpl*>(buffer);
}

void RenderContextImpl::setInstanceBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _instanceBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_instanceBuffer);
    _instanceBuffer = static_cast<BufferImpl*>(buffer);
}

void RenderContextImpl::drawArrays(PrimitiveType primitiveType, std::size_t start, std::size_t count, bool wireframe)
{
    prepareDrawing();
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#else
    if (wireframe)
        primitiveType = PrimitiveType::LINE;
#endif
    glDrawArrays(UtilsGL::toGLPrimitiveType(primitiveType), start, count);
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    cleanResources();
}

void RenderContextImpl::drawArraysInstanced(PrimitiveType primitiveType,
                                            std::size_t start,
                                            std::size_t count,
                                            int instanceCount,
                                            bool wireframe)
{
    prepareDrawing();
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#else
    if (wireframe)
        primitiveType = PrimitiveType::LINE;
#endif
    glDrawArraysInstanced(UtilsGL::toGLPrimitiveType(primitiveType), start, count, instanceCount);
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    cleanResources();
}

void RenderContextImpl::drawElements(PrimitiveType primitiveType,
                                     IndexFormat indexType,
                                     std::size_t count,
                                     std::size_t offset,
                                     bool wireframe)
{
    prepareDrawing();
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#else
    if (wireframe)
        primitiveType = PrimitiveType::LINE;
#endif
    __state->bindBuffer(BufferType::ELEMENT_ARRAY_BUFFER, _indexBuffer->internalHandle());
    glDrawElements(UtilsGL::toGLPrimitiveType(primitiveType), count, UtilsGL::toGLIndexType(indexType),
                   (GLvoid*)offset);
    CHECK_GL_ERROR_DEBUG();
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    cleanResources();
}

void RenderContextImpl::drawElementsInstanced(PrimitiveType primitiveType,
                                              IndexFormat indexType,
                                              std::size_t count,
                                              std::size_t offset,
                                              int instanceCount,
                                              bool wireframe)
{
    prepareDrawing();
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#else
    if (wireframe)
        primitiveType = PrimitiveType::LINE;
#endif
    __state->bindBuffer(BufferType::ELEMENT_ARRAY_BUFFER, _indexBuffer->internalHandle());
    glDrawElementsInstanced(UtilsGL::toGLPrimitiveType(primitiveType), count, UtilsGL::toGLIndexType(indexType),
                            (GLvoid*)offset, instanceCount);
    CHECK_GL_ERROR_DEBUG();
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    cleanResources();
}

void RenderContextImpl::endRenderPass()
{
    AX_SAFE_RELEASE_NULL(_indexBuffer);
    AX_SAFE_RELEASE_NULL(_vertexBuffer);
    AX_SAFE_RELEASE_NULL(_instanceBuffer);
}

void RenderContextImpl::endFrame() {}

void RenderContextImpl::prepareDrawing() const
{
    const auto& program = _renderPipeline->getProgram();
    __state->useProgram(program->internalHandle());

    uint32_t usedBits{0};

    bindVertexBuffer(usedBits);
    __state->disableUnusedVertexAttribs(usedBits);

    bindUniforms(program);

    // Set depth/stencil state.
    if (_depthStencilStateImpl->isEnabled())
        _depthStencilStateImpl->apply(_stencilReferenceValue);
    else
        DepthStencilStateImpl::reset();

    // Set cull mode.
    if (_cullMode != CullMode::NONE)
        __state->enableCullFace(UtilsGL::toGLCullMode(_cullMode));
    else
        __state->disableCullFace();
}

void RenderContextImpl::bindVertexBuffer(uint32_t& usedBits) const
{
    assert(_vertexLayout);

    auto vl = static_cast<VertexLayoutImpl*>(_vertexLayout);
    vl->apply(_vertexBuffer, _instanceBuffer, usedBits);
}

void RenderContextImpl::bindUniforms(ProgramImpl* program) const
{
    if (_programState)
    {
        assert(program == _programState->getProgram());

        auto& callbacks = _programState->getCallbackUniforms();
        for (auto&& cb : callbacks)
            cb.second(_programState, cb.first);

        auto& buffer = _programState->getUniformBuffer();
        program->bindUniformBuffers(buffer.data(), buffer.size());

        CHECK_GL_ERROR_DEBUG();

        for (const auto& [_, bindingSet] : _programState->getTextureBindingSets())
        {
            auto& slots          = bindingSet.slots;
            auto& texs           = bindingSet.texs;
            const auto arraySize = slots.size();
            if (!arraySize) [[unlikely]]
                continue;

            if (arraySize == 1)
            {  // perform bind for 'uniform sampler2D u_tex;' or 'uniform sampler2DArray u_texs;'
                static_cast<TextureImpl*>(texs[0])->apply(slots[0]);
                glUniform1i(bindingSet.runtimeLocation, slots[0]);
            }
            else
            {  // perform bind for 'uniform sampler2D u_details[4];' in shader
                for (size_t i = 0; i < arraySize; ++i)
                    static_cast<TextureImpl*>(texs[i])->apply(slots[i]);
                glUniform1iv(bindingSet.runtimeLocation, static_cast<GLsizei>(arraySize),
                             static_cast<const GLint*>(slots.data()));
            }

            CHECK_GL_ERROR_DEBUG();
        }
    }
}

void RenderContextImpl::cleanResources()
{
    _programState = nullptr;
    _vertexLayout = nullptr;
}

void RenderContextImpl::setScissorRect(bool isEnabled, float x, float y, float width, float height)
{
    if (isEnabled)
        __state->enableScissor(x, y, width, height);
    else
        __state->disableScissor();
}

void RenderContextImpl::readPixels(RenderTarget* rt,
                                   bool preserveAxisHint,
                                   std::function<void(const PixelBufferDesc&)> callback)
{
    PixelBufferDesc pbd;
    if (rt->isDefaultRenderTarget())
    {  // read pixels from screen
        readPixels(rt, _viewport.x, _viewport.y, _viewport.width, _viewport.height, _viewport.width * 4,
                   preserveAxisHint, pbd);
    }
    else
    {
        // we only readPixels from the COLOR0 attachment.
        auto colorAttachment = rt->_color[0].texture;
        if (colorAttachment)
        {
            readPixels(rt, 0, 0, colorAttachment->getWidth(), colorAttachment->getHeight(),
                       colorAttachment->getWidth() * 4, preserveAxisHint, pbd);
        }
    }
    callback(pbd);
}

void RenderContextImpl::readPixels(RenderTarget* rt,
                                   int x,
                                   int y,
                                   uint32_t width,
                                   uint32_t height,
                                   uint32_t bytesPerRow,
                                   bool preserveAxisHint,
                                   PixelBufferDesc& pbd)
{
    auto rtGL = static_cast<RenderTargetImpl*>(rt);
    rtGL->bindFrameBuffer();

    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    auto bufferSize = bytesPerRow * height;
#if AX_HAVE_MAP_BUFFER_RANGE
    GLuint pbo;
    glGenBuffers(1, &pbo);
    __state->bindBuffer(BufferType::PIXEL_PACK_BUFFER, pbo);
    glBufferData(GL_PIXEL_PACK_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    auto buffer_ptr = (uint8_t*)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, bufferSize, GL_MAP_READ_BIT);
#else
    tlx::byte_buffer buffer(static_cast<size_t>(bufferSize), 0);
    auto buffer_ptr = buffer.data();
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer_ptr);
#endif
    CHECK_GL_ERROR_DEBUG();

    if (buffer_ptr)
    {
        if (!preserveAxisHint)
        {
            // we need to flip the buffer vertically to match our API
            uint8_t* wptr = pbd._data.resize(bufferSize);
            if (wptr)
            {
                auto rptr = buffer_ptr + (height - 1) * bytesPerRow;
                for (int row = 0; row < height; ++row)
                {
                    memcpy(wptr, rptr, bytesPerRow);
                    wptr += bytesPerRow;
                    rptr -= bytesPerRow;
                }
                pbd._width  = width;
                pbd._height = height;
            }
        }
        else
        {
            // for cache for restore on EGL context resume, don't need flip
            pbd._width  = width;
            pbd._height = height;
#if AX_HAVE_MAP_BUFFER_RANGE
            pbd._data.copy(buffer_ptr, static_cast<ssize_t>(bufferSize));
#else
            static_cast<tlx::byte_buffer&>(pbd._data).swap(buffer);
#endif
        }
    }
#if AX_HAVE_MAP_BUFFER_RANGE
    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    __state->bindBuffer(BufferType::PIXEL_PACK_BUFFER, 0);
    glDeleteBuffers(1, &pbo);
#endif

    if (!rtGL->isDefaultRenderTarget())
        rtGL->unbindFrameBuffer();
}

}  // namespace ax::rhi::gl
