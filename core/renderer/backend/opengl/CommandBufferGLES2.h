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
