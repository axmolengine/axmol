/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/ComputePipeline.h"
#include "axmol/rhi/d3d11/Program11.h"

namespace ax::rhi::d3d11
{
/**
 * @brief A D3D11 compute pipeline. D3D11 has no compute PSO; this object binds
 * a compute program for API symmetry with the other backends.
 */
class ComputePipelineImpl : public ComputePipeline
{
public:
    explicit ComputePipelineImpl(ProgramImpl* program) { setProgram(program); }
    ~ComputePipelineImpl() override = default;

    bool isValid() const override { return getProgram() != nullptr; }
};

}  // namespace ax::rhi::d3d11
