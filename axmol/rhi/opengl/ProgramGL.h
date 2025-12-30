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

#pragma once

#include "axmol/rhi/RHITypes.h"
#include "axmol/base/Object.h"
#include "axmol/base/EventListenerCustom.h"
#include "axmol/platform/GL.h"
#include "axmol/rhi/Program.h"
#include "axmol/rhi/DriverBase.h"

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

    ~ProgramImpl();

    /**
     * Get program object.
     * @return Program object.
     */
    inline GLuint internalHandle() const { return _program; }

    void bindUniformBuffers(const uint8_t* buffer, size_t bufferSize);

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
    EventListenerCustom* _backToForegroundListener = nullptr;
#endif
};
// end of _opengl group
/// @}
}  // namespace ax::rhi::gl
