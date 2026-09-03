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

#include "axmol/rhi/opengl/GraphicsContextGL.h"
#include "axmol/rhi/opengl/BufferGL.h"
#include "axmol/rhi/opengl/RenderPipelineGL.h"
#include "axmol/rhi/opengl/TextureGL.h"
#include "axmol/rhi/opengl/DepthStencilStateGL.h"
#include "axmol/rhi/opengl/ProgramGL.h"
#include "axmol/rhi/opengl/MacrosGL.h"
#include "axmol/rhi/opengl/UtilsGL.h"
#include "axmol/rhi/opengl/RenderTargetGL.h"
#include "axmol/rhi/opengl/GraphicsDeviceGL.h"
#include "axmol/rhi/opengl/VertexLayoutGL.h"
#include "axmol/rhi/opengl/ComputePipelineGL.h"
#include "axmol/rhi/SamplerRegistry.h"
#include "axmol/rhi/RHIUtils.h"

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

GraphicsContextImpl::GraphicsContextImpl(GraphicsDeviceImpl* driver)
{
    _screenRT = new RenderTargetImpl(driver, true);
}

GraphicsContextImpl::~GraphicsContextImpl()
{
    cleanResources();

    AX_SAFE_RELEASE_NULL(_screenRT);
    AX_SAFE_RELEASE_NULL(_renderPipeline);
}

bool GraphicsContextImpl::beginFrame()
{
    return true;
}

void GraphicsContextImpl::beginRenderPass(RenderTarget* rt, const RenderPassDesc& descriptor)
{
    auto rtGL = static_cast<RenderTargetImpl*>(rt);

    rtGL->bindFrameBuffer();
    rtGL->update();

    auto clearFlags = descriptor.flags.clear;

    // Disable scissor test before clearing in OpenGL.
    // In OpenGL, glClear is affected by the current scissor state, unlike other RHIs
    // (D3D, Metal, Vulkan) where clears always apply to the full render target.
    // To ensure consistent cross-platform behavior, we temporarily disable scissor
    // here so that beginRenderPass clears the entire framebuffer regardless of any
    // previously set scissor rect.
    // @see also: issue #1627, #3123
    __state->disableScissor();

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

void GraphicsContextImpl::setDepthStencilState(DepthStencilState* depthStencilState)
{
    _depthStencilStateImpl = static_cast<DepthStencilStateImpl*>(depthStencilState);
}

void GraphicsContextImpl::setRenderPipeline(RenderPipeline* renderPipeline)
{
    Object::assign(_renderPipeline, static_cast<RenderPipelineImpl*>(renderPipeline));
}

/**
 * Update depthStencil status, improvment: for metal backend cache it
 * @param depthStencilState Specifies the depth and stencil status
 */
void GraphicsContextImpl::updateDepthStencilState(const DepthStencilDesc& desc)
{
    _depthStencilStateImpl->update(desc);
}

/**
 * Update render pipeline status
 * @param depthStencilState Specifies the depth and stencil status
 */
void GraphicsContextImpl::updatePipelineState(const RenderTarget* rt,
                                              const PipelineDesc& desc,
                                              PrimitiveType primitiveType)
{
    GraphicsContext::updatePipelineState(rt, desc, primitiveType);

    _renderPipeline->update(rt, desc);

    _primitiveType = UtilsGL::toGLPrimitiveType(primitiveType);
}

void GraphicsContextImpl::setViewport(int x, int y, unsigned int w, unsigned int h)
{
    __state->viewport(_viewport.set(x, y, w, h));
}

void GraphicsContextImpl::setCullMode(CullMode mode)
{
    _cullMode = mode;
}

void GraphicsContextImpl::setWinding(Winding winding)
{
    __state->winding(UtilsGL::toGLFrontFace(winding));
}

void GraphicsContextImpl::setVertexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _vertexBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_vertexBuffer);
    _vertexBuffer = static_cast<BufferImpl*>(buffer);
}

void GraphicsContextImpl::setIndexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _indexBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_indexBuffer);
    _indexBuffer = static_cast<BufferImpl*>(buffer);
}

void GraphicsContextImpl::setInstanceBuffer(Buffer* buffer)
{
    if (buffer == nullptr)
    {
        AX_SAFE_RELEASE_NULL(_instanceBuffer);
        return;
    }
    if (_instanceBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_instanceBuffer);
    _instanceBuffer = static_cast<BufferImpl*>(buffer);
}

void GraphicsContextImpl::drawArrays(size_t start, size_t count, bool wireframe)
{
    prepareDrawing();
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#else
    if (wireframe)
        _primitiveType = GL_LINES;
#endif
    glDrawArrays(_primitiveType, start, count);
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    cleanResources();
}

void GraphicsContextImpl::drawArraysInstanced(size_t start, size_t count, int instanceCount, bool wireframe)
{
    prepareDrawing();
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#else
    if (wireframe)
        _primitiveType = GL_LINES;
#endif
    glDrawArraysInstanced(_primitiveType, start, count, instanceCount);
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    cleanResources();
}

void GraphicsContextImpl::drawElements(IndexFormat indexType, size_t count, size_t offset, bool wireframe)
{
    prepareDrawing();
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#else
    if (wireframe)
        _primitiveType = GL_LINES;
#endif
    __state->bindBuffer(BufferType::ELEMENT_ARRAY_BUFFER, _indexBuffer->internalHandle());
    glDrawElements(_primitiveType, count, UtilsGL::toGLIndexType(indexType), (GLvoid*)offset);
    CHECK_GL_ERROR_DEBUG();
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    cleanResources();
}

void GraphicsContextImpl::drawElementsInstanced(IndexFormat indexType,
                                                size_t count,
                                                size_t offset,
                                                int instanceCount,
                                                bool wireframe)
{
    prepareDrawing();
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#else
    if (wireframe)
        _primitiveType = GL_LINES;
#endif
    __state->bindBuffer(BufferType::ELEMENT_ARRAY_BUFFER, _indexBuffer->internalHandle());
    glDrawElementsInstanced(_primitiveType, count, UtilsGL::toGLIndexType(indexType), (GLvoid*)offset, instanceCount);
    CHECK_GL_ERROR_DEBUG();
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    cleanResources();
}

void GraphicsContextImpl::endRenderPass()
{
    AX_SAFE_RELEASE_NULL(_indexBuffer);
    AX_SAFE_RELEASE_NULL(_vertexBuffer);
    AX_SAFE_RELEASE_NULL(_instanceBuffer);
}

void GraphicsContextImpl::endFrame() {}

void GraphicsContextImpl::submitCurrentFrameCommands(bool waitForCompletion)
{
    if (waitForCompletion)
        glFinish();
    else
        glFlush();
}

void GraphicsContextImpl::prepareDrawing() const
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

void GraphicsContextImpl::bindVertexBuffer(uint32_t& usedBits) const
{
    assert(_vertexLayout);

    auto vl = static_cast<VertexLayoutImpl*>(_vertexLayout);
    vl->apply(_vertexBuffer, _instanceBuffer, usedBits);
}

void GraphicsContextImpl::bindUniforms(ProgramImpl* program) const
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

        // Bind sampler objects selected by axslcc for GL/GLES combined texture uniforms.
        // These override the default texture sampler bound by TextureImpl::apply().
        if (!program->getActiveSamplerInfos().empty())
        {
            auto samplerReg = SamplerRegistry::getInstance();
            for (const auto& [bindingIndex, bindingSet] : _programState->getTextureBindingSets())
            {
                auto samplerId = program->getTextureSampler(bindingIndex);
                auto samplerLocation = program->getTextureSamplerLocation(bindingIndex);
                if (samplerLocation && samplerLocation.space == axslc::kCustomSamplerDescriptorSet)
                {
                    if (auto overrideId = _programState->getSamplerOverride(samplerLocation.binding))
                        samplerId = overrideId;
                }
                if (!samplerId)
                    continue;

                auto samplerHandle = samplerReg->getSampler(samplerId);
                if (!samplerHandle)
                    continue;

                auto glSampler = static_cast<GLuint>(samplerHandle);
                for (int slot : bindingSet.slots)
                    __state->bindSampler(slot, glSampler);
            }
        }

#if AX_GL_HAS_COMPUTE
        // Bind storage buffers (SSBO) read by the GPU render vertex/fragment stages.
        for (const auto& [binding, bindingSet] : _programState->getStorageBufferBindingSets())
        {
            if (!bindingSet.buffer)
                continue;
            auto ssbo = static_cast<BufferImpl*>(bindingSet.buffer)->internalHandle();
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ssbo);
        }
#endif
    }
}

void GraphicsContextImpl::cleanResources()
{
    _programState = nullptr;
    _vertexLayout = nullptr;
}

void GraphicsContextImpl::setScissorRect(bool enabled, float x, float y, float width, float height)
{
    if (enabled)
    {
        __state->setScissor(x, y, width, height);
        __state->enableScissor();
    }
    else
    {
        __state->disableScissor();
    }
}

void GraphicsContextImpl::readPixels(RenderTarget* rt, std::function<void(const PixelBufferDesc&)> callback)
{
    PixelBufferDesc pbd;
    if (rt->isDefaultRenderTarget())
    {  // read pixels from screen
        readPixels(rt, _viewport.x, _viewport.y, _viewport.width, _viewport.height, _viewport.width * 4, pbd);
    }
    else
    {
        // we only readPixels from the COLOR0 attachment.
        auto colorAttachment = rt->_color[0].texture;
        if (colorAttachment)
        {
            readPixels(rt, 0, 0, colorAttachment->getWidth(), colorAttachment->getHeight(),
                       colorAttachment->getWidth() * 4, pbd);
        }
    }
    callback(pbd);
}

void GraphicsContextImpl::readPixels(RenderTarget* rt,
                                     int x,
                                     int y,
                                     uint32_t width,
                                     uint32_t height,
                                     uint32_t bytesPerRow,
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
        if (rt->isDefaultRenderTarget())
        {
            // flip Y for default RT (OpenGL has inverted Y)
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

bool GraphicsContextImpl::copyTexture(Texture* src, Texture* dst)
{
    if (!validateTextureCopy(src, dst))
        return false;

    auto* srcGL           = static_cast<TextureImpl*>(src);
    auto* dstGL           = static_cast<TextureImpl*>(dst);
    const auto srcTexture = srcGL->internalHandle();
    if (!srcTexture)
        return false;

    GLint previousFramebuffer   = 0;
    GLint previousActiveTexture = GL_TEXTURE0;
    GLint previousTexture       = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);
    glGetIntegerv(GL_ACTIVE_TEXTURE, &previousActiveTexture);

    __state->activeTexture(GL_TEXTURE0);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

    if (!dstGL->internalHandle())
        dstGL->updateData(nullptr, dst->getWidth(), dst->getHeight(), 0, 0);

    const auto dstTexture = dstGL->internalHandle();
    if (!dstTexture || srcTexture == dstTexture)
    {
        __state->bindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previousTexture));
        __state->activeTexture(static_cast<GLenum>(previousActiveTexture));
        return false;
    }

    GLuint copyFramebuffer = 0;
    glGenFramebuffers(1, &copyFramebuffer);
    __state->bindFrameBuffer(copyFramebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, srcTexture, 0);

    const bool framebufferComplete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    if (framebufferComplete)
    {
        __state->bindTexture(GL_TEXTURE_2D, dstTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, dst->getWidth(), dst->getHeight());
    }

    __state->bindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previousTexture));
    __state->activeTexture(static_cast<GLenum>(previousActiveTexture));
    __state->bindFrameBuffer(static_cast<GLuint>(previousFramebuffer));
    glDeleteFramebuffers(1, &copyFramebuffer);
    CHECK_GL_ERROR_DEBUG();

    return framebufferComplete;
}

bool GraphicsContextImpl::copyTexture(RenderTarget* src, Texture* dst)
{
    if (!src || !dst || dst->getTextureType() != TextureType::TEXTURE_2D || dst->getArraySize() != 1 ||
        dst->getMipLevels() != 1 || dst->getWidth() <= 0 || dst->getHeight() <= 0 ||
        dst->getPixelFormat() == PixelFormat::NONE || dst->getPixelFormat() == PixelFormat::D24S8)
        return false;

    const auto& formatDesc = RHIUtils::getFormatDesc(dst->getPixelFormat());
    if (formatDesc.blockWidth != 1 || formatDesc.blockHeight != 1)
        return false;

    if (!src->isDefaultRenderTarget())
    {
        if (src->_color.empty() || src->_color[0].level != 0 || !src->_color[0].texture)
            return false;
        return copyTexture(src->_color[0].texture, dst);
    }

    auto* srcGL = static_cast<RenderTargetImpl*>(src);
    auto* dstGL = static_cast<TextureImpl*>(dst);

    GLint previousFramebuffer   = 0;
    GLint previousActiveTexture = GL_TEXTURE0;
    GLint previousTexture       = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);
    glGetIntegerv(GL_ACTIVE_TEXTURE, &previousActiveTexture);

    __state->activeTexture(GL_TEXTURE0);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

    if (!dstGL->internalHandle())
        dstGL->updateData(nullptr, dst->getWidth(), dst->getHeight(), 0, 0);

    const auto dstTexture = dstGL->internalHandle();
    if (!dstTexture)
    {
        __state->bindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previousTexture));
        __state->activeTexture(static_cast<GLenum>(previousActiveTexture));
        return false;
    }

    srcGL->bindFrameBuffer();
    srcGL->update();

    const bool framebufferComplete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    if (framebufferComplete)
    {
        __state->bindTexture(GL_TEXTURE_2D, dstTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, dst->getWidth(), dst->getHeight());
    }

    __state->bindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previousTexture));
    __state->activeTexture(static_cast<GLenum>(previousActiveTexture));
    __state->bindFrameBuffer(static_cast<GLuint>(previousFramebuffer));
    CHECK_GL_ERROR_DEBUG();

    return framebufferComplete;
}

bool GraphicsContextImpl::dispatch(const ComputeDispatchDesc& desc)
{
#if AX_GL_HAS_COMPUTE
    if (!desc.programState || !desc.pipeline)
        return false;

    auto* pipelineProgram = desc.pipeline->getProgram();
    if (!pipelineProgram || pipelineProgram != desc.programState->getProgram())
    {
        AXASSERT(false, "ComputePipeline and ProgramState program mismatch");
        return false;
    }

    auto program = static_cast<ProgramImpl*>(pipelineProgram);
    if (!program || !program->getCSModule())
        return false;

    _programState = desc.programState;

    __state->useProgram(program->internalHandle());

    // Uniform buffers + textures + samplers (reuses the ProgramState bound above)
    bindUniforms(program);

    // Bind storage buffers (SSBO) at their reflected binding indices.
    for (const auto& [binding, bindingSet] : desc.programState->getStorageBufferBindingSets())
    {
        if (!bindingSet.buffer)
            continue;
        auto ssbo = static_cast<BufferImpl*>(bindingSet.buffer)->internalHandle();
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ssbo);
    }

    CHECK_GL_ERROR_DEBUG();

    dispatchCompute(desc.groupCountX, desc.groupCountY, desc.groupCountZ);

    // Make compute writes visible to subsequent compute/vertex/fragment reads.
    memoryBarrier(GL_ALL_BARRIER_BITS);

    // Unbind storage buffers to avoid stale SSBO bindings leaking into later draws.
    for (const auto& [binding, bindingSet] : desc.programState->getStorageBufferBindingSets())
    {
        if (!bindingSet.buffer)
            continue;
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, 0);
    }

    CHECK_GL_ERROR_DEBUG();

    cleanResources();
    return true;
#else
    AX_UNUSED_PARAM(desc);
    return false;
#endif
}

}  // namespace ax::rhi::gl
