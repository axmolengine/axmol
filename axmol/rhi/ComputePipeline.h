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

#include "axmol/base/Object.h"
#include "axmol/rhi/RHITypes.h"

namespace ax::rhi
{
/**
 * @addtogroup _rhi
 * @{
 */
class Program;

/**
 * @brief Compute pipeline (backed by a backend compute PSO / dispatch pipeline).
 *
 * Mirrors RenderPipeline for the graphics pipeline: a dispatch carries a
 * ComputePipeline (the compiled compute pipeline state) together with a
 * ProgramState (bound resources). The local workgroup size belongs to the
 * program (shader reflection), never to the dispatch.
 */
class ComputePipeline : public ax::Object
{
public:
    /**
     * Get the compute program this pipeline was created from.
     * @return The compute program.
     */
    Program* getProgram() const { return _program; }

    /**
     * Check whether the backend pipeline has been successfully created.
     * @return true if the native compute pipeline is ready to dispatch.
     */
    virtual bool isValid() const = 0;

protected:
    void setProgram(Program* program);

    virtual ~ComputePipeline();

private:
    Program* _program = nullptr;
};

// end of _rhi group
/// @}
}  // namespace ax::rhi
