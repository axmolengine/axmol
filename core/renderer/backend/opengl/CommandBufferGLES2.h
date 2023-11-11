#pragma once

#include "CommandBufferGL.h"

#if !defined(__APPLE__) && AX_TARGET_PLATFORM != AX_PLATFORM_WINRT

NS_AX_BACKEND_BEGIN

/**
 * @addtogroup _opengl
 * @{
 */

/**
 * @brief Store encoded commands for the GPU to execute.
 * A command buffer stores encoded commands until the buffer is committed for execution by the GPU
 */
class CommandBufferGLES2 : public CommandBufferGL
{
public:
    CommandBufferGLES2();
    void drawElementsInstanced(PrimitiveType primitiveType,
                               IndexFormat indexType,
                               std::size_t count,
                               std::size_t offset,
                               int instanceCount,
                               bool wireframe = false) override;

    void bindInstanceBuffer(ProgramGL* program, uint32_t& usedBits) const override;

};

// end of _opengl group
/// @}
NS_AX_BACKEND_END

#endif
