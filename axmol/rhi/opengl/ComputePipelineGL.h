/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/ComputePipeline.h"
#include "axmol/rhi/opengl/ProgramGL.h"

namespace ax::rhi::gl
{
/**
 * @brief An OpenGL compute pipeline. OpenGL has no compute PSO; this object
 * binds a compute program for API symmetry with the other backends.
 */
class ComputePipelineImpl : public ComputePipeline
{
public:
    explicit ComputePipelineImpl(ProgramImpl* program) { setProgram(program); }
    ~ComputePipelineImpl() override = default;

    bool isValid() const override { return getProgram() != nullptr; }
};

}  // namespace ax::rhi::gl
