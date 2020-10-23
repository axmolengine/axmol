/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

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
#include "base/ccMacros.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventType.h"
#include "base/CCDirector.h"
#include "renderer/backend/opengl/UtilsGL.h"
#include "RenderTargetGL.h"
#include <algorithm>

CC_BACKEND_BEGIN

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
            return ;
        }
    }
}

CommandBufferGL::CommandBufferGL()
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_defaultFBO);
}

CommandBufferGL::~CommandBufferGL()
{
    cleanResources();
}

void CommandBufferGL::beginFrame()
{
}

void CommandBufferGL::beginRenderPass(const RenderTarget* rt, const RenderPassParams& descirptor)
{
    auto rtGL = static_cast<const RenderTargetGL*>(rt);
    
    rtGL->bindFrameBuffer();
 
    auto clearFlags = descirptor.flags.clear;

    // set clear color, depth and stencil
    GLbitfield mask = 0;
    if (bitmask::any(clearFlags, TargetBufferFlags::COLOR))
    {
        mask |= GL_COLOR_BUFFER_BIT;
        const auto& clearColor = descirptor.clearColorValue;
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    }

    CHECK_GL_ERROR_DEBUG();

    GLboolean oldDepthWrite = GL_FALSE;
    GLboolean oldDepthTest = GL_FALSE;
    GLfloat oldDepthClearValue = 0.f;
    GLint oldDepthFunc = GL_LESS;
    if (bitmask::any(clearFlags, TargetBufferFlags::DEPTH))
    {
        glGetBooleanv(GL_DEPTH_WRITEMASK, &oldDepthWrite);
        glGetBooleanv(GL_DEPTH_TEST, &oldDepthTest);
        glGetFloatv(GL_DEPTH_CLEAR_VALUE, &oldDepthClearValue);
        glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFunc);

        mask |= GL_DEPTH_BUFFER_BIT;
        glClearDepth(descirptor.clearDepthValue);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_ALWAYS);
    }

    CHECK_GL_ERROR_DEBUG();

    if (bitmask::any(clearFlags, TargetBufferFlags::STENCIL))
    {
        mask |= GL_STENCIL_BUFFER_BIT;
        glClearStencil(descirptor.clearStencilValue);
    }

    if (mask) glClear(mask);

    CHECK_GL_ERROR_DEBUG();

    // restore depth test
    if (bitmask::any(clearFlags, TargetBufferFlags::DEPTH))
    {
        if (!oldDepthTest)
            glDisable(GL_DEPTH_TEST);

        glDepthMask(oldDepthWrite);
        glDepthFunc(oldDepthFunc);
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
    glViewport(x, y, w, h);
    _viewPort.x = x;
    _viewPort.y = y;
    _viewPort.w = w;
    _viewPort.h = h;
}

void CommandBufferGL::setCullMode(CullMode mode)
{
    _cullMode = mode;
}

void CommandBufferGL::setWinding(Winding winding)
{
    glFrontFace(UtilsGL::toGLFrontFace(winding));
}

void CommandBufferGL::setIndexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr)
        return;
    
    buffer->retain();
    CC_SAFE_RELEASE(_indexBuffer);
    _indexBuffer = static_cast<BufferGL*>(buffer);
}

void CommandBufferGL::setVertexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _vertexBuffer == buffer)
        return;
    
    buffer->retain();
    _vertexBuffer = static_cast<BufferGL*>(buffer);
}

void CommandBufferGL::setProgramState(ProgramState* programState)
{
    CC_SAFE_RETAIN(programState);
    CC_SAFE_RELEASE(_programState);
    _programState = programState;
}

void CommandBufferGL::drawArrays(PrimitiveType primitiveType, std::size_t start,  std::size_t count)
{
    prepareDrawing();
    glDrawArrays(UtilsGL::toGLPrimitiveType(primitiveType), start, count);
    
    cleanResources();
}

void CommandBufferGL::drawElements(PrimitiveType primitiveType, IndexFormat indexType, std::size_t count, std::size_t offset)
{
    prepareDrawing();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer->getHandler());
    glDrawElements(UtilsGL::toGLPrimitiveType(primitiveType), count, UtilsGL::toGLIndexType(indexType), (GLvoid*)offset);
    CHECK_GL_ERROR_DEBUG();
    cleanResources();
}

void CommandBufferGL::endRenderPass()
{
}

void CommandBufferGL::endFrame()
{
}

void CommandBufferGL::prepareDrawing() const
{   
    const auto& program = _renderPipeline->getProgram();
    glUseProgram(program->getHandler());
    
    bindVertexBuffer(program);
    setUniforms(program);

    // Set depth/stencil state.
    if (_depthStencilStateGL->isEnabled())
        _depthStencilStateGL->apply(_stencilReferenceValueFront, _stencilReferenceValueBack);
    else
        DepthStencilStateGL::reset();
    
    // Set cull mode.
    if (CullMode::NONE == _cullMode)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
        glCullFace(UtilsGL::toGLCullMode(_cullMode));
    }
}

void CommandBufferGL::bindVertexBuffer(ProgramGL *program) const
{
    // Bind vertex buffers and set the attributes.
    auto vertexLayout = _programState->getVertexLayout();
    
    if (!vertexLayout->isValid())
        return;
    
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer->getHandler());

    const auto& attributes = vertexLayout->getAttributes();
    for (const auto& attributeInfo : attributes)
    {
        const auto& attribute = attributeInfo.second;
        glEnableVertexAttribArray(attribute.index);
        glVertexAttribPointer(attribute.index,
            UtilsGL::getGLAttributeSize(attribute.format),
            UtilsGL::toGLAttributeType(attribute.format),
            attribute.needToBeNormallized,
            vertexLayout->getStride(),
            (GLvoid*)attribute.offset);
    }
}

void CommandBufferGL::setUniforms(ProgramGL* program) const
{
    if (_programState)
    {
        auto& uniformInfos = _programState->getProgram()->getAllActiveUniformInfo(ShaderStage::VERTEX);
        std::size_t bufferSize = 0;
        char* buffer = nullptr;
        _programState->getVertexUniformBuffer(&buffer, bufferSize);

        int i = 0;
        for(auto& iter : uniformInfos)
        {
            auto& uniformInfo = iter.second;
            if(uniformInfo.size <= 0)
                continue;

            int elementCount = uniformInfo.count;
            setUniform(uniformInfo.isArray,
                uniformInfo.location,
                elementCount,
                uniformInfo.type,
                (void*)(buffer + uniformInfo.bufferOffset));
        }
        
        const auto& textureInfo = _programState->getVertexTextureInfos();
        for(const auto& iter : textureInfo)
        {
            /* About mutli textures support
            *  a. sampler2DArray, sampler2D[2], bind BackendTexture one by one, not use GL_TEXTURE_2D_ARRAY, not used at all engine interanl
            *  b. texture slot, one BackendTexture, multi GPU texture handlers, used by etc1, restrict: textures must have same size
            *  c. Bind multi BackendTexture to 1 Shader Program, see the ShaderTest
            */
            auto& textures = iter.second.textures;
            auto& slots = iter.second.slots;
            auto& indexs = iter.second.indexs;
            auto location = iter.first;
#if CC_ENABLE_CACHE_TEXTURE_DATA
            location = iter.second.location;
#endif
            int i = 0;
            for (const auto& texture: textures)
            {
                applyTexture(texture, slots[i], indexs[i]);
                ++i;
            }
            
            auto arrayCount = slots.size();
            if (arrayCount == 1) // Most of the time， not use sampler2DArray, should be 1
                glUniform1i(location, slots[0]);
            else
                glUniform1iv(location, (uint32_t)arrayCount, (GLint*)slots.data());
        }
    }
}

#define DEF_TO_INT(pointer, index)     (*((GLint*)(pointer) + index))
#define DEF_TO_FLOAT(pointer, index)   (*((GLfloat*)(pointer) + index))
void CommandBufferGL::setUniform(bool isArray, GLuint location, unsigned int size, GLenum uniformType, void* data) const
{
    GLsizei count = size;
    switch (uniformType)
    {
        case GL_INT:
        case GL_BOOL:
        case GL_SAMPLER_2D:
        case GL_SAMPLER_CUBE:
            if (isArray)
                glUniform1iv(location, count, (GLint*)data);
            else
                glUniform1i(location, DEF_TO_INT(data, 0));
            break;
        case GL_INT_VEC2:
        case GL_BOOL_VEC2:
            if (isArray)
                glUniform2iv(location, count, (GLint*)data);
            else
                glUniform2i(location, DEF_TO_INT(data, 0), DEF_TO_INT(data, 1));
            break;
        case GL_INT_VEC3:
        case GL_BOOL_VEC3:
            if (isArray)
                glUniform3iv(location, count, (GLint*)data);
            else
                glUniform3i(location,
                            DEF_TO_INT(data, 0),
                            DEF_TO_INT(data, 1),
                            DEF_TO_INT(data, 2));
            break;
        case GL_INT_VEC4:
        case GL_BOOL_VEC4:
            if (isArray)
                glUniform4iv(location, count, (GLint*)data);
            else
                glUniform4i(location,
                            DEF_TO_INT(data, 0),
                            DEF_TO_INT(data, 1),
                            DEF_TO_INT(data, 2),
                            DEF_TO_INT(data, 4));
            break;
        case GL_FLOAT:
            if (isArray)
                glUniform1fv(location, count, (GLfloat*)data);
            else
                glUniform1f(location, DEF_TO_FLOAT(data, 0));
            break;
        case GL_FLOAT_VEC2:
            if (isArray)
                glUniform2fv(location, count, (GLfloat*)data);
            else
                glUniform2f(location, DEF_TO_FLOAT(data, 0), DEF_TO_FLOAT(data, 1));
            break;
        case GL_FLOAT_VEC3:
            if (isArray)
                glUniform3fv(location, count, (GLfloat*)data);
            else
                glUniform3f(location,
                            DEF_TO_FLOAT(data, 0),
                            DEF_TO_FLOAT(data, 1),
                            DEF_TO_FLOAT(data, 2));
            break;
        case GL_FLOAT_VEC4:
            if (isArray)
                glUniform4fv(location, count, (GLfloat*)data);
            else
                glUniform4f(location,
                            DEF_TO_FLOAT(data, 0),
                            DEF_TO_FLOAT(data, 1),
                            DEF_TO_FLOAT(data, 2),
                            DEF_TO_FLOAT(data, 3));
            break;
        case GL_FLOAT_MAT2:
            glUniformMatrix2fv(location, count, GL_FALSE, (GLfloat*)data);
            break;
        case GL_FLOAT_MAT3:
            glUniformMatrix3fv(location, count, GL_FALSE, (GLfloat*)data);
            break;
        case GL_FLOAT_MAT4:
            glUniformMatrix4fv(location, count, GL_FALSE, (GLfloat*)data);
            break;
        break;
        
        default:
            CCASSERT(false, "invalidate Uniform data type");
        break;
    }
}

void CommandBufferGL::cleanResources()
{
    CC_SAFE_RELEASE_NULL(_indexBuffer);
    CC_SAFE_RELEASE_NULL(_programState);  
    CC_SAFE_RELEASE_NULL(_vertexBuffer);
}

void CommandBufferGL::setLineWidth(float lineWidth)
{
    if(lineWidth > 0.0f)
        glLineWidth(lineWidth);
    else
        glLineWidth(1.0f);
    
}


void CommandBufferGL::setScissorRect(bool isEnabled, float x, float y, float width, float height)
{
    if(isEnabled)
    {
        glEnable(GL_SCISSOR_TEST);
        glScissor(x, y, width, height);
    }
    else
    {
        glDisable(GL_SCISSOR_TEST);
    }
}
 
void CommandBufferGL::readPixels(RenderTarget* rt, std::function<void(const PixelBufferDescriptor&)> callback)
{
    PixelBufferDescriptor pbd;
    if(rt->isDefaultRenderTarget()) 
    { // read pixels from screen
        readPixels(rt, _viewPort.x, _viewPort.y, _viewPort.w, _viewPort.h, _viewPort.w * 4, pbd);
    }
    else {
        // we only readPixels from the COLOR0 attachment.
        auto colorAttachment = rt->_color[0].texture;
        if(colorAttachment) {
            readPixels(rt,
            0, 0, 
            colorAttachment->getWidth(),colorAttachment->getHeight(), 
            colorAttachment->getWidth() * 4,
            pbd);
        }
    }
    callback(pbd);
}

void CommandBufferGL::readPixels(RenderTarget* rt, int x, int y, uint32_t width, uint32_t height, uint32_t bytesPerRow, PixelBufferDescriptor& pbd)
{
    rt->bindFrameBuffer();

    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    auto bufferSize = bytesPerRow * height;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 && defined(GL_ES_VERSION_3_0)) || \
    (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && defined(GL_PIXEL_PACK_BUFFER))
    GLuint pbo;
    glGenBuffers(1, &pbo);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
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
    if (buffer && (wptr = pbd._data.resize(bufferSize))) {
        auto rptr = buffer + (height - 1) * bytesPerRow;
        for (int row = 0; row < height; ++row) {
            memcpy(wptr, rptr, bytesPerRow);
            wptr += bytesPerRow;
            rptr -= bytesPerRow;
        }
        pbd._width = width;
        pbd._height = height;
    }
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 && defined(GL_ES_VERSION_3_0)) || \
    (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && defined(GL_PIXEL_PACK_BUFFER))
    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glDeleteBuffers(1, &pbo);
#endif

    if (!rt->isDefaultRenderTarget())
        rt->unbindFrameBuffer();
}

CC_BACKEND_END
