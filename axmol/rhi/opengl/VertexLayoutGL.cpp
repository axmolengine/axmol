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
#include "axmol/rhi/opengl/VertexLayoutGL.h"
#include "axmol/rhi/opengl/ProgramGL.h"
#include "axmol/rhi/opengl/DriverGL.h"
#include "axmol/rhi/opengl/UtilsGL.h"
#include "axmol/rhi/opengl/MacrosGL.h"

/**
 * Vertex attribute binding (glVertexAttribBinding/Format) path is currently incomplete.
 * This code uses the legacy vertex format setup for compatibility.
 * TODO: Replace with full binding-based VAO configuration when binding index, offset/stride,
 *       divisor, and EBO setup are fully implemented.
 */
#define AX_ENABLE_VERTEX_ATTRIB_BINDING 0

namespace ax::rhi::gl
{

VertexLayoutImpl::VertexLayoutImpl(VertexLayoutDesc&& desc) : VertexLayout(std::move(desc))
{
    setupVAO();
}

VertexLayoutImpl::~VertexLayoutImpl()
{
#if AX_ENABLE_VERTEX_ATTRIB_BINDING
    glDeleteVertexArrays(1, &_vao);
    _vao      = 0;
    _usedBits = 0;
#endif
}

void VertexLayoutImpl::apply(BufferImpl* vertexBuffer, BufferImpl* instanceBuffer, uint32_t& usedBits) const
{
#if !AX_ENABLE_VERTEX_ATTRIB_BINDING

    // Bind vertex buffers and set the attributes.
    {
        // Bind VAO, engine share 1 VAO for all vertexLayouts aka vfmts
        // optimize proposal: create VAO per vertexLayout, just need bind VAO
        __state->bindBuffer(BufferType::ARRAY_BUFFER, vertexBuffer->internalHandle());

        for (const auto& inputDesc : getBindings())
        {
            if (inputDesc.instanceStepRate) [[unlikely]]
                continue;
            __state->enableVertexAttribArray(inputDesc.index);
            glVertexAttribPointer(inputDesc.index, UtilsGL::getGLAttributeSize(inputDesc.format),
                                  UtilsGL::toGLAttributeType(inputDesc.format), inputDesc.needToBeNormallized,
                                  _desc.getStride(), (GLvoid*)static_cast<uintptr_t>(inputDesc.offset));
            // non-instance attrib not use divisor, so clear to 0
            __state->clearVertexAttribDivisor(inputDesc.index);
            usedBits |= (1 << inputDesc.index);
        }
    }

    // Bind vertex instance buffer and set attributes
    if (instanceBuffer)
    {
        __state->bindBuffer(BufferType::ARRAY_BUFFER, instanceBuffer->internalHandle());
        for (const auto& inputDesc : getBindings())
        {
            if (!inputDesc.instanceStepRate) [[unlikely]]
                continue;
            switch (inputDesc.format)
            {
            case VertexFormat::MAT4:
                for (auto i = 0; i < 4; ++i)
                {
                    auto elementIndex = inputDesc.index + i;
                    __state->enableVertexAttribArray(elementIndex);
                    glVertexAttribPointer(elementIndex, 4, GL_FLOAT, GL_FALSE, _desc.getInstanceStride(),
                                          (void*)(sizeof(float) * 4 * i));
                    __state->setVertexAttribDivisor(elementIndex);
                    usedBits |= (1 << elementIndex);
                }
                break;
            default:
                __state->enableVertexAttribArray(inputDesc.index);
                glVertexAttribPointer(inputDesc.index, UtilsGL::getGLAttributeSize(inputDesc.format),
                                      UtilsGL::toGLAttributeType(inputDesc.format), inputDesc.needToBeNormallized,
                                      _desc.getInstanceStride(), (GLvoid*)static_cast<uintptr_t>(inputDesc.offset));
                __state->setVertexAttribDivisor(inputDesc.index);
                usedBits |= (1 << inputDesc.index);
            }
        }
    }
#else
    glBindVertexArray(_vao);

    const auto& desc = getDesc();
    glBindVertexBuffer(DriverImpl::VBO_BINDING_INDEX, vertexBuffer->internalHandle(), 0, desc.getStride());

    if (instanceBuffer)
        glBindVertexBuffer(DriverImpl::VBO_INSTANCING_BINDING_INDEX, instanceBuffer->internalHandle(), 0,
                           desc.getInstanceStride());

    // usedBits = _usedBits;
#endif

    CHECK_GL_ERROR_DEBUG();
}

void VertexLayoutImpl::setupVAO()
{
#if AX_ENABLE_VERTEX_ATTRIB_BINDING
    if (_vao)
        return;

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    const auto& bindings = getBindings();

    for (const auto& inputDesc : bindings)
    {
        GLuint bindingIndex =
            inputDesc.instanceStepRate ? DriverImpl::VBO_INSTANCING_BINDING_INDEX : DriverImpl::VBO_BINDING_INDEX;

        GLuint attribIndex = inputDesc.index;
        if (!inputDesc.instanceStepRate)
        {
#    if 0
            if (UtilsGL::isFloatFormat(inputDesc.format) || inputDesc.needToBeNormallized)
                glVertexAttribFormat(attribIndex, UtilsGL::getGLAttributeSize(inputDesc.format),
                                     UtilsGL::toGLAttributeType(inputDesc.format), true,
                                     inputDesc.offset);
            else
                glVertexAttribIFormat(attribIndex, UtilsGL::getGLAttributeSize(inputDesc.format),
                                      UtilsGL::toGLAttributeType(inputDesc.format), inputDesc.offset);
#    endif
            // we only support float attribs
            glVertexAttribFormat(attribIndex, UtilsGL::getGLAttributeSize(inputDesc.format),
                                 UtilsGL::toGLAttributeType(inputDesc.format), inputDesc.needToBeNormallized,
                                 inputDesc.offset);

            glVertexAttribBinding(attribIndex, bindingIndex);
            glEnableVertexAttribArray(attribIndex);
            glVertexBindingDivisor(bindingIndex, inputDesc.instanceStepRate);

            _usedBits = 1 << attribIndex;
        }
        else
        {
            switch (inputDesc.format)
            {
            case VertexFormat::MAT4:
                for (GLuint i = 0; i < 4; ++i)
                {
                    GLuint elementIndex = attribIndex + i;
                    glVertexAttribFormat(elementIndex, 4, GL_FLOAT, GL_FALSE, inputDesc.offset + sizeof(float) * 4 * i);
                    glVertexAttribBinding(elementIndex, bindingIndex);
                    glEnableVertexAttribArray(elementIndex);
                    _usedBits = 1 << elementIndex;
                }
                glVertexBindingDivisor(bindingIndex, inputDesc.instanceStepRate);
                break;
            default:
                glVertexAttribFormat(attribIndex, UtilsGL::getGLAttributeSize(inputDesc.format),
                                     UtilsGL::toGLAttributeType(inputDesc.format), inputDesc.needToBeNormallized,
                                     inputDesc.offset);
                glVertexAttribBinding(attribIndex, bindingIndex);
                glEnableVertexAttribArray(attribIndex);
                _usedBits = 1 << attribIndex;
                break;
            }
        }
    }

    glBindVertexArray(0);

    CHECK_GL_ERROR_DEBUG();
#endif
}

}  // namespace ax::rhi::gl
