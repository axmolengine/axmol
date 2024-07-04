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
