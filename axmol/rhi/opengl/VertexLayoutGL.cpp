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

namespace ax::rhi::gl
{

VertexLayoutImpl::VertexLayoutImpl(VertexLayoutDesc&& desc) : VertexLayout(std::move(desc))
{
    setupVAO();
}

VertexLayoutImpl::~VertexLayoutImpl() 
{
    glDeleteVertexArrays(1, &_vao);
    _vao = 0;
}

void VertexLayoutImpl::apply(BufferImpl* vertexBuffer, BufferImpl* instanceBuffer) const
{
    glBindVertexArray(_vao);

    const auto& desc = getDesc();
    glBindVertexBuffer(0, vertexBuffer->getHandler(), 0, desc.getStride());

    if(instanceBuffer) {
        glBindVertexBuffer(1, instanceBuffer->getHandler(), 0, desc.getInstanceStride());
    }

    CHECK_GL_ERROR_DEBUG();
}

void VertexLayoutImpl::setupVAO()
{
    if (_vao)
        return;

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    const auto& bindings = getBindings();
    GLuint attribIndex   = 0;
    for (const auto& inputDesc : bindings)
    {
        GLuint bindingIndex = inputDesc.instanceStepRate ? 1 : 0;

        // glBindVertexBuffer(bindingIndex, inputDesc.vbo, inputDesc.offset, inputDesc.stride);

        if (UtilsGL::isFloatFormat(inputDesc.format))
            glVertexAttribFormat(attribIndex, UtilsGL::getGLAttributeSize(inputDesc.format),
                                 UtilsGL::toGLAttributeType(inputDesc.format), inputDesc.needToBeNormallized, 0);
        else
            glVertexAttribIFormat(attribIndex, UtilsGL::getGLAttributeSize(inputDesc.format),
                                  UtilsGL::toGLAttributeType(inputDesc.format), 0);

        glVertexAttribBinding(attribIndex, bindingIndex);
        glEnableVertexAttribArray(attribIndex);
        glVertexBindingDivisor(bindingIndex, inputDesc.instanceStepRate);

        // MAT4 情况：拆成4个vec4属性
        if (inputDesc.format == VertexFormat::MAT4)
        {
            for (GLuint i = 0; i < 4; ++i)
            {
                GLuint ai = attribIndex + i;
                glVertexAttribFormat(ai, 4, GL_FLOAT, GL_FALSE, inputDesc.offset + sizeof(float) * 4 * i);
                glVertexAttribBinding(ai, bindingIndex);
                glEnableVertexAttribArray(ai);
            }
            attribIndex += 4;
        }
        else
        {
            attribIndex++;
        }
    }

    glBindVertexArray(0);

    CHECK_GL_ERROR_DEBUG();
}

}  // namespace ax::rhi::gl
