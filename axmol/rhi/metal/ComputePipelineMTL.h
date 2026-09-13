/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/ComputePipeline.h"
#include <Metal/Metal.h>

namespace ax::rhi::mtl
{
class ProgramImpl;

/**
 * @brief A Metal compute pipeline (MTLComputePipelineState) for a compute program.
 */
class ComputePipelineImpl : public ComputePipeline
{
public:
    ComputePipelineImpl(id<MTLDevice> mtlDevice, ProgramImpl* program);
    ~ComputePipelineImpl();

    bool isValid() const override { return _mtlComputePipelineState != nil; }

    id<MTLComputePipelineState> getMTLComputePipelineState() const { return _mtlComputePipelineState; }

private:
    id<MTLComputePipelineState> _mtlComputePipelineState = nil;
};

}  // namespace ax::rhi::mtl
