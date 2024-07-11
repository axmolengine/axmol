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
 
#include "CommandBufferGLES2.h"

#if !defined(__APPLE__) && AX_TARGET_PLATFORM != AX_PLATFORM_WINRT

#    include "platform/GL.h"

NS_AX_BACKEND_BEGIN

CommandBufferGLES2::CommandBufferGLES2()
{
    if (glDrawElementsInstancedEXT)
        glDrawElementsInstanced = glDrawElementsInstancedEXT;
    else if (glDrawElementsInstancedANGLE)
        glDrawElementsInstanced = glDrawElementsInstancedANGLE;

    if (glVertexAttribDivisorEXT)
        glVertexAttribDivisor = glVertexAttribDivisorEXT;
    else if (glVertexAttribDivisorANGLE)
        glVertexAttribDivisor = glVertexAttribDivisorANGLE;

    if (!glDrawElementsInstanced)
        AXLOGD("{}", "Device not support instancing draw");
}

void CommandBufferGLES2::drawElementsInstanced(PrimitiveType primitiveType,
                                               IndexFormat indexType,
                                               std::size_t count,
                                               std::size_t offset,
                                               int instanceCount,
                                               bool wireframe)
{
    if (!glDrawElementsInstanced)
        return;
    CommandBufferGL::drawElementsInstanced(primitiveType, indexType, count, offset, instanceCount, wireframe);
}

void CommandBufferGLES2::bindInstanceBuffer(ProgramGL* program, uint32_t& usedBits) const
{

    if (!glDrawElementsInstanced)
        return;
    CommandBufferGL::bindInstanceBuffer(program, usedBits);
}

NS_AX_BACKEND_END

#endif
