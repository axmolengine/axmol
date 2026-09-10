/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
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
 * Mirrors GraphicsPipeline for the graphics pipeline: a dispatch carries a
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
