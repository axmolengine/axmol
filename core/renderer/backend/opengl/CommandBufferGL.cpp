/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmolengine.github.io/

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

#include "CommandBufferGL.h"
#include "BufferGL.h"
#include "RenderPipelineGL.h"
#include "TextureGL.h"
#include "DepthStencilStateGL.h"
#include "ProgramGL.h"
#include "base/EventDispatcher.h"
#include "base/EventType.h"
#include "base/Director.h"
#include "MacrosGL.h"
#include "UtilsGL.h"
#include "RenderTargetGL.h"
#include "DriverGL.h"
#include <algorithm>

NS_AX_BACKEND_BEGIN

namespace
{
void applyTexture(TextureBackend* texture, int slot, int index)
{
    switch (texture->getTextureType())
    {
    case TextureType::TEXTURE_2D:
        static_cast<Texture2DGL*>(texture)->apply(slot, index);
        break;
    case TextureType::TEXTURE_CUBE:
        static_cast<TextureCubeGL*>(texture)->apply(slot, index);
        break;
    default:
        assert(false);
        return;
    }
}
}  // namespace

CommandBufferGL::CommandBufferGL() {}

CommandBufferGL::~CommandBufferGL()
{
    cleanResources();
}

bool CommandBufferGL::beginFrame()
{
    return true;
}

void CommandBufferGL::beginRenderPass(const RenderTarget* rt, const RenderPassDescriptor& descriptor)
{
    auto rtGL = static_cast<const RenderTargetGL*>(rt);

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
        __gl->enableDepthTest();

        __gl->depthMask(GL_TRUE);
        __gl->depthFunc(GL_ALWAYS);
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
            __gl->disableDepthTest();

        __gl->depthMask(oldDepthWrite);
        __gl->depthFunc(oldDepthFunc);
        glClearDepth(oldDepthClearValue);
    }

    CHECK_GL_ERROR_DEBUG();
}

void CommandBufferGL::setDepthStencilState(DepthStencilState* depthStencilState)
{
    _depthStencilStateGL = static_cast<DepthStencilStateGL*>(depthStencilState);
}

void CommandBufferGL::setRenderPipeline(RenderPipeline* renderPipeline)
{
    _renderPipeline = static_cast<RenderPipelineGL*>(renderPipeline);
}

/**
 * Update depthStencil status, improvment: for metal backend cache it
 * @param depthStencilState Specifies the depth and stencil status
 */
void CommandBufferGL::updateDepthStencilState(const DepthStencilDescriptor& descriptor)
{
    _depthStencilStateGL->update(descriptor);
}

/**
 * Update render pipeline status
 * @param depthStencilState Specifies the depth and stencil status
 */
void CommandBufferGL::updatePipelineState(const RenderTarget* rt, const PipelineDescriptor& descriptor)
{
    _renderPipeline->update(rt, descriptor);
}

void CommandBufferGL::setViewport(int x, int y, unsigned int w, unsigned int h)
{
    __gl->viewport(_viewPort.set(x, y, w, h));
}

void CommandBufferGL::setCullMode(CullMode mode)
{
    _cullMode = mode;
}

void CommandBufferGL::setWinding(Winding winding)
{
    __gl->winding(winding);
}

void CommandBufferGL::setIndexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _indexBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_indexBuffer);
    _indexBuffer = static_cast<BufferGL*>(buffer);
}

void CommandBufferGL::setInstanceBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _instanceTransformBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_instanceTransformBuffer);
    _instanceTransformBuffer = static_cast<BufferGL*>(buffer);
}

void CommandBufferGL::setVertexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _vertexBuffer == buffer)
        return;

    buffer->retain();
    AX_SAFE_RELEASE(_vertexBuffer);
    _vertexBuffer = static_cast<BufferGL*>(buffer);
}

void CommandBufferGL::setProgramState(ProgramState* programState)
{
    AX_SAFE_RETAIN(programState);
    AX_SAFE_RELEASE(_programState);
    _programState = programState;
}

void CommandBufferGL::drawArrays(PrimitiveType primitiveType, std::size_t start, std::size_t count, bool wireframe)
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

void CommandBufferGL::drawElements(PrimitiveType primitiveType,
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
    __gl->bindBuffer(BufferType::ELEMENT_ARRAY_BUFFER, _indexBuffer->getHandler());
    glDrawElements(UtilsGL::toGLPrimitiveType(primitiveType), count, UtilsGL::toGLIndexType(indexType),
                   (GLvoid*)offset);
    CHECK_GL_ERROR_DEBUG();
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    cleanResources();
}

void CommandBufferGL::drawElementsInstanced(PrimitiveType primitiveType,
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
    __gl->bindBuffer(BufferType::ELEMENT_ARRAY_BUFFER, _indexBuffer->getHandler());
    glDrawElementsInstanced(UtilsGL::toGLPrimitiveType(primitiveType), count, UtilsGL::toGLIndexType(indexType),
                            (GLvoid*)offset, instanceCount);
    CHECK_GL_ERROR_DEBUG();
#if !AX_GLES_PROFILE  // glPolygonMode is only supported in Desktop OpenGL
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    cleanResources();
}

void CommandBufferGL::endRenderPass()
{
    AX_SAFE_RELEASE_NULL(_indexBuffer);
    AX_SAFE_RELEASE_NULL(_vertexBuffer);
    AX_SAFE_RELEASE_NULL(_instanceTransformBuffer);
}

void CommandBufferGL::endFrame() {}

void CommandBufferGL::prepareDrawing() const
{
    const auto& program = _renderPipeline->getProgram();
    __gl->useProgram(program->getHandler());

    uint32_t usedBits{0};

    bindVertexBuffer(usedBits);
    bindInstanceBuffer(program, usedBits);
    __gl->disableUnusedVertexAttribs(usedBits);

    bindUniforms(program);

    // Set depth/stencil state.
    if (_depthStencilStateGL->isEnabled())
        _depthStencilStateGL->apply(_stencilReferenceValueFront, _stencilReferenceValueBack);
    else
        DepthStencilStateGL::reset();

    // Set cull mode.
    if (_cullMode != CullMode::NONE)
        __gl->enableCullFace(UtilsGL::toGLCullMode(_cullMode));
    else
        __gl->disableCullFace();
}

void CommandBufferGL::bindVertexBuffer(uint32_t& usedBits) const
{
    // Bind vertex buffers and set the attributes.
    auto vertexLayout = _programState->getVertexLayout();

    const auto& attributes = vertexLayout->getAttributes();
    if (!vertexLayout->isValid())
        return;

    // Bind VAO, engine share 1 VAO for all vertexLayouts aka vfmts
    // optimize proposal: create VAO per vertexLayout, just need bind VAO
    __gl->bindBuffer(BufferType::ARRAY_BUFFER, _vertexBuffer->getHandler());

    for (const auto& attributeInfo : attributes)
    {
        const auto& attribute = attributeInfo.second;
        __gl->enableVertexAttribArray(attribute.index);
        glVertexAttribPointer(attribute.index, UtilsGL::getGLAttributeSize(attribute.format),
                              UtilsGL::toGLAttributeType(attribute.format), attribute.needToBeNormallized,
                              vertexLayout->getStride(), (GLvoid*)attribute.offset);
        // non-instance attrib not use divisor, so clear to 0
        __gl->clearVertexAttribDivisor(attribute.index);
        usedBits |= (1 << attribute.index);
    }
}

void CommandBufferGL::bindInstanceBuffer(ProgramGL* program, uint32_t& usedBits) const
{
    // if we have an instance transform buffer pointer then we must be rendering in instance mode.
    if (_instanceTransformBuffer)
    {
        auto instanceLoc = program->getAttributeLocation(Attribute::INSTANCE);
        if (instanceLoc != -1)
        {
            __gl->bindBuffer(BufferType::ARRAY_BUFFER, _instanceTransformBuffer->getHandler());

            for (auto i = 0; i < 4; ++i)
            {
                auto elementLoc = instanceLoc + i;
                __gl->enableVertexAttribArray(elementLoc);
                glVertexAttribPointer(elementLoc, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16,
                                      (void*)(sizeof(float) * 4 * i));
                __gl->setVertexAttribDivisor(elementLoc);
                usedBits |= (1 << elementLoc);
            }
        }
    }
}

void CommandBufferGL::bindUniforms(ProgramGL* program) const
{
    if (_programState)
    {
        assert(program == _programState->getProgram());

        auto& callbacks = _programState->getCallbackUniforms();
        for (auto&& cb : callbacks)
            cb.second(_programState, cb.first);

        auto& uniformInfos = program->getAllActiveUniformInfo(ShaderStage::VERTEX);

        std::size_t bufferSize = 0;
        auto buffer            = _programState->getVertexUniformBuffer(bufferSize);
        program->bindUniformBuffers(buffer, bufferSize);

        const auto& textureInfo = _programState->getVertexTextureInfos();
        for (const auto& iter : textureInfo)
        {
            /* About mutli textures support
             *  a. sampler2DArray, sampler2D[2], bind BackendTexture one by one, not use GL_TEXTURE_2D_ARRAY, not used
             * at all engine interanl b. texture slot, one BackendTexture, multi GPU texture handlers, used by etc1,
             * restrict: textures must have same size c. Bind multi BackendTexture to 1 Shader Program, see the
             * ShaderTest
             */
            auto& textures = iter.second.textures;
            auto& slots    = iter.second.slots;
            auto& indexs   = iter.second.indexs;
            auto location  = iter.first;
#if AX_ENABLE_CACHE_TEXTURE_DATA
            location = iter.second.location;
#endif
            int i = 0;
            for (const auto& texture : textures)
            {
                applyTexture(texture, slots[i], indexs[i]);
                ++i;
            }

            auto arrayCount = slots.size();
            if (arrayCount == 1)  // Most of the time， not use sampler2DArray, should be 1
                glUniform1i(location, slots[0]);
            else
                glUniform1iv(location, static_cast<GLsizei>(arrayCount), static_cast<const GLint*>(slots.data()));
        }
    }
}

void CommandBufferGL::cleanResources()
{
    AX_SAFE_RELEASE_NULL(_programState);
}

void CommandBufferGL::setScissorRect(bool isEnabled, float x, float y, float width, float height)
{
    if (isEnabled)
        __gl->enableScissor(x, y, width, height);
    else
        __gl->disableScissor();
}

void CommandBufferGL::readPixels(RenderTarget* rt, std::function<void(const PixelBufferDescriptor&)> callback)
{
    PixelBufferDescriptor pbd;
    if (rt->isDefaultRenderTarget())
    {  // read pixels from screen
        readPixels(rt, _viewPort.x, _viewPort.y, _viewPort.width, _viewPort.height, _viewPort.width * 4, pbd);
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

void CommandBufferGL::readPixels(RenderTarget* rt,
                                 int x,
                                 int y,
                                 uint32_t width,
                                 uint32_t height,
                                 uint32_t bytesPerRow,
                                 PixelBufferDescriptor& pbd)
{
    auto rtGL = static_cast<RenderTargetGL*>(rt);
    rtGL->bindFrameBuffer();

    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    auto bufferSize = bytesPerRow * height;
#if AX_GLES_PROFILE != 200
    GLuint pbo;
    glGenBuffers(1, &pbo);
    __gl->bindBuffer(BufferType::PIXEL_PACK_BUFFER, pbo);
    glBufferData(GL_PIXEL_PACK_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    auto buffer = (uint8_t*)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, bufferSize, GL_MAP_READ_BIT);
#else
    std::unique_ptr<uint8_t[]> bufferStorage(new uint8_t[bufferSize]);
    auto buffer = bufferStorage.get();
    memset(buffer, 0, bufferSize);
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
#endif
    uint8_t* wptr = nullptr;
    if (buffer && (wptr = pbd._data.resize(bufferSize)))
    {
        auto rptr = buffer + (height - 1) * bytesPerRow;
        for (int row = 0; row < height; ++row)
        {
            memcpy(wptr, rptr, bytesPerRow);
            wptr += bytesPerRow;
            rptr -= bytesPerRow;
        }
        pbd._width  = width;
        pbd._height = height;
    }
#if AX_GLES_PROFILE != 200
    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    __gl->bindBuffer(BufferType::PIXEL_PACK_BUFFER, 0);
    glDeleteBuffers(1, &pbo);
#endif

    if (!rtGL->isDefaultRenderTarget())
        rtGL->unbindFrameBuffer();
}

NS_AX_BACKEND_END
