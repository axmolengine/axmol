#include "CommandBufferGLES2.h"

#if defined(__ANDROID__)

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

    if (!glDrawElementsInstancedEXT)
    {
        AXLOG("%s", "Device not support instancing draw");
    }
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

void CommandBufferGLES2::bindInstanceBuffer(ProgramGL* program, bool* usedList) const
{

    if (!glDrawElementsInstanced)
        return;
    CommandBufferGL::bindInstanceBuffer(program, usedList);
}

NS_AX_BACKEND_END

#endif
