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

#include "axmol/rhi/opengl/CommandBufferGL.h"
#include "axmol/rhi/opengl/BufferGL.h"
#include "axmol/rhi/opengl/RenderPipelineGL.h"
#include "axmol/rhi/opengl/TextureGL.h"
#include "axmol/rhi/opengl/DepthStencilStateGL.h"
#include "axmol/rhi/opengl/ProgramGL.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/EventType.h"
#include "axmol/base/Director.h"
#include "axmol/rhi/opengl/MacrosGL.h"
#include "axmol/rhi/opengl/UtilsGL.h"
#include "axmol/rhi/opengl/RenderTargetGL.h"
#include "axmol/rhi/opengl/DriverGL.h"
#include <algorithm>

namespace ax::rhi::gl {

// !!Note: WebGL 2.0 on (Open GELS 3.0) not support map buffer range
#if AX_TARGET_PLATFORM != AX_PLATFORM_WASM
#    define AX_HAVE_MAP_BUFFER_RANGE 1
#else
#    define AX_HAVE_MAP_BUFFER_RANGE 0
#endif

CommandBufferImpl::CommandBufferImpl() {}

CommandBufferImpl::~CommandBufferImpl()
{
    cleanResources();
}

bool CommandBufferImpl::beginFrame()
{
    return true;
}

void CommandBufferImpl::beginRenderPass(const RenderTarget* rt, const RenderPassDesc& descriptor)
{
    auto rtGL = static_cast<const RenderTargetImpl*>(rt);

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

void CommandBufferImpl::setDepthStencilState(DepthStencilState* depthStencilState)
{
    _depthStencilStateImpl = static_cast<DepthStencilStateImpl*>(depthStencilState);
}

void CommandBufferImpl::setRenderPipeline(RenderPipeline* renderPipeline)
{
    _renderPipeline = static_cast<RenderPipelineImpl*>(renderPipeline);
}

/**
 * Update depthStencil status, improvment: for metal backend cache it
 * @param depthStencilState Specifies the depth and stencil status
 */
void CommandBufferImpl::updateDepthStencilState(const DepthStencilDesc& descriptor)
{
    _depthStencilStateImpl->update(descriptor);
}

/**
 * Update render pipeline status
 * @param depthStencilState Specifies the depth and stencil status
 */
void CommandBufferImpl::updatePipelineState(const RenderTarget* rt, const PipelineDesc& descriptor)
{
    _renderPipeline->update(rt, descriptor);
}

void CommandBufferImpl::setViewport(int x, int y, unsigned int w, unsigned int h)
{
    __state->viewport(_viewPort.set(x, y, w, h));
}

void CommandBufferImpl::setCullMode(CullMode mode)
{
    _cullMode = mode;
}

void CommandBufferImpl::setWinding(Winding winding)
{
    __state->winding(winding);
}

void CommandBufferImpl::setProgramState(ProgramState* programState)
{
    AX_SAFE_RETAIN(programState);
    AX_SAFE_RELEASE(_programState);
    _programState = programState;
}

void CommandBufferImpl::setVertexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _vertexBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_vertexBuffer);
    _vertexBuffer = static_cast<BufferImpl*>(buffer);
}

void CommandBufferImpl::setIndexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _indexBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_indexBuffer);
    _indexBuffer = static_cast<BufferImpl*>(buffer);
}

void CommandBufferImpl::setInstanceBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _instanceBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_instanceBuffer);
    _instanceBuffer = static_cast<BufferImpl*>(buffer);
}

void CommandBufferImpl::drawArrays(PrimitiveType primitiveType, std::size_t start, std::size_t count, bool wireframe)
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

void CommandBufferImpl::drawArraysInstanced(PrimitiveType primitiveType, std::size_t start, std::size_t count, int instanceCount, bool wireframe)
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

void CommandBufferImpl::drawElements(PrimitiveType primitiveType,
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
    __state->bindBuffer(BufferType::ELEMENT_ARRAY_BUFFER, _indexBuffer->getHandler());
    glDrawElements(UtilsGL::toGLPrimitiveType(primitiveType), count, UtilsGL::toGLIndexType(indexType),
                   (GLvoid*)offset);
    CHECK_GL_ERROR_DEBUG();
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    cleanResources();
}

void CommandBufferImpl::drawElementsInstanced(PrimitiveType primitiveType,
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
    __state->bindBuffer(BufferType::ELEMENT_ARRAY_BUFFER, _indexBuffer->getHandler());
    glDrawElementsInstanced(UtilsGL::toGLPrimitiveType(primitiveType), count, UtilsGL::toGLIndexType(indexType),
                            (GLvoid*)offset, instanceCount);
    CHECK_GL_ERROR_DEBUG();
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    cleanResources();
}

void CommandBufferImpl::endRenderPass()
{
    AX_SAFE_RELEASE_NULL(_indexBuffer);
    AX_SAFE_RELEASE_NULL(_vertexBuffer);
    AX_SAFE_RELEASE_NULL(_instanceBuffer);
}

void CommandBufferImpl::endFrame()
{
}

void CommandBufferImpl::prepareDrawing() const
{
    const auto& program = _renderPipeline->getProgram();
    __state->useProgram(program->getHandler());

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

void CommandBufferImpl::bindVertexBuffer(uint32_t& usedBits) const
{
    auto vertexLayout = _programState->getVertexLayout();

    if (!vertexLayout->isValid())
        return;

    // Bind vertex buffers and set the attributes.
    {
        // Bind VAO, engine share 1 VAO for all vertexLayouts aka vfmts
        // optimize proposal: create VAO per vertexLayout, just need bind VAO
        __state->bindBuffer(BufferType::ARRAY_BUFFER, _vertexBuffer->getHandler());

        const auto& attributes = vertexLayout->getBindings();
        for (const auto& attributeInfo : attributes)
        {
            const auto& attribute = attributeInfo.second;
            __state->enableVertexAttribArray(attribute.index);
            glVertexAttribPointer(attribute.index, UtilsGL::getGLAttributeSize(attribute.format),
                                    UtilsGL::toGLAttributeType(attribute.format), attribute.needToBeNormallized,
                                    vertexLayout->getStride(), (GLvoid*)static_cast<uintptr_t>(attribute.offset));
            // non-instance attrib not use divisor, so clear to 0
            __state->clearVertexAttribDivisor(attribute.index);
            usedBits |= (1 << attribute.index);
        }
    }

    // Bind vertex instance buffer and set attributes
    if (_instanceBuffer)
    {
        __state->bindBuffer(BufferType::ARRAY_BUFFER, _instanceBuffer->getHandler());
        const auto instanceStride = vertexLayout->getInstanceStride();
        const auto& attributes    = vertexLayout->getInstanceBindings();
        for (const auto& attributeInfo : attributes)
        {
            const auto& attribute = attributeInfo.second;
            switch (attribute.format)
            {
            case VertexFormat::MAT4:
                for (auto i = 0; i < 4; ++i)
                {
                    auto elementLoc = attribute.index + i;
                    __state->enableVertexAttribArray(elementLoc);
                    glVertexAttribPointer(elementLoc, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16,
                                            (void*)(sizeof(float) * 4 * i));
                    __state->setVertexAttribDivisor(elementLoc);
                    usedBits |= (1 << elementLoc);
                }
                break;
            default:
                __state->enableVertexAttribArray(attribute.index);
                glVertexAttribPointer(attribute.index, UtilsGL::getGLAttributeSize(attribute.format),
                                        UtilsGL::toGLAttributeType(attribute.format), attribute.needToBeNormallized,
                                        instanceStride, (GLvoid*)static_cast<uintptr_t>(attribute.offset));
                __state->setVertexAttribDivisor(attribute.index);
                usedBits |= (1 << attribute.index);
            }
        }
    }
}

void CommandBufferImpl::bindUniforms(ProgramImpl* program) const
{
    if (_programState)
    {
        assert(program == _programState->getProgram());

        auto& callbacks = _programState->getCallbackUniforms();
        for (auto&& cb : callbacks)
            cb.second(_programState, cb.first);

        auto&& buffer            = _programState->getUniformBuffer();
        program->bindUniformBuffers(buffer.data(), buffer.size());

        for (auto& [location, bindingInfo] : _programState->getTextureBindingInfos())
        {
            static_cast<TextureImpl*>(bindingInfo.tex)->apply(bindingInfo.slot);
            glUniform1i(location, bindingInfo.slot);
        }
    }
}

void CommandBufferImpl::cleanResources()
{
    AX_SAFE_RELEASE_NULL(_programState);
}

void CommandBufferImpl::setScissorRect(bool isEnabled, float x, float y, float width, float height)
{
    if (isEnabled)
        __state->enableScissor(x, y, width, height);
    else
        __state->disableScissor();
}

void CommandBufferImpl::readPixels(RenderTarget* rt, std::function<void(const PixelBufferDesc&)> callback)
{
    PixelBufferDesc pbd;
    if (rt->isDefaultRenderTarget())
    {  // read pixels from screen
        readPixels(rt, _viewPort.x, _viewPort.y, _viewPort.width, _viewPort.height, _viewPort.width * 4, false, pbd);
    }
    else
    {
        // we only readPixels from the COLOR0 attachment.
        auto colorAttachment = rt->_color[0].texture;
        if (colorAttachment)
        {
            readPixels(rt, 0, 0, colorAttachment->getWidth(), colorAttachment->getHeight(),
                       colorAttachment->getWidth() * 4, false, pbd);
        }
    }
    callback(pbd);
}

void CommandBufferImpl::readPixels(RenderTarget* rt,
                                 int x,
                                 int y,
                                 uint32_t width,
                                 uint32_t height,
                                 uint32_t bytesPerRow,
                                 bool eglCacheHint,
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
    axstd::byte_buffer buffer(static_cast<size_t>(bufferSize), 0);
    auto buffer_ptr = buffer.data();
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer_ptr);
#endif
    CHECK_GL_ERROR_DEBUG();

    if (buffer_ptr)
    {
        if (!eglCacheHint)
        {
            // we need to flip the buffer vertically to match our API
            uint8_t* wptr = nullptr;
            if (wptr = pbd._data.resize(bufferSize))
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
            static_cast<axstd::byte_buffer&>(pbd._data).swap(buffer);
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

}
