/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/rhi/RHITypes.h"
#include "axmol/base/Object.h"
#include "axmol/base/CustomEventListener.h"
#include "axmol/platform/GL.h"
#include "axmol/rhi/Program.h"
#include "axmol/rhi/GraphicsCore.h"

#include <string>
#include <vector>
#include <unordered_map>

#include "axmol/tlx/vector.hpp"

namespace ax::rhi::gl
{

class ShaderModuleImpl;

/**
 * @addtogroup _opengl
 * @{
 */

/**
 * An OpenGL program.
 */
class ProgramImpl : public Program
{
public:
    /**
     * @param vertexShader Specifes the vertex shader source.
     * @param fragmentShader Specifes the fragment shader source.
     */
    ProgramImpl(Data& vsData, Data& fsData);

    /**
     * @param csData Specifies the compute shader source.
     */
    explicit ProgramImpl(Data& csData);

    ~ProgramImpl();

    /**
     * Get program object.
     * @return Program object.
     */
    inline GLuint internalHandle() const { return _program; }

    void bindUniformBuffers(const uint8_t* buffer, size_t bufferSize);

    bool isValid() const override;

private:
    void compileProgram();

    void deleteUniformBuffers();

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    void reloadProgram();
#endif

    GLuint _program = 0;

    // The ubo instances
    UniformBufferVector _uniformBuffers;

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    CustomEventListener* _backToForegroundListener = nullptr;
#endif
};
// end of _opengl group
/// @}
}  // namespace ax::rhi::gl
