/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "axmol/rhi/metal/ComputePipelineMTL.h"
#include "axmol/rhi/metal/ProgramMTL.h"
#include "axmol/base/Logging.h"

namespace ax::rhi::mtl
{

ComputePipelineImpl::ComputePipelineImpl(id<MTLDevice> mtlDevice, ProgramImpl* program)
{
    setProgram(program);

    const auto& localSize = program->getComputeLocalSize();
    if (localSize[0] <= 0 || localSize[1] <= 0 || localSize[2] <= 0)
    {
        AXLOGE("Compute shader has invalid local workgroup size: {}x{}x{}", localSize[0], localSize[1], localSize[2]);
        return;
    }

    auto function = program->getMTLComputeFunction();
    if (function == nil)
        return;

    NSError* error           = nil;
    _mtlComputePipelineState = [mtlDevice newComputePipelineStateWithFunction:function error:&error];
    if (_mtlComputePipelineState == nil)
    {
        AXLOGE("Failed to create Metal compute pipeline: {}", [[error description] UTF8String]);
        return;
    }

    const auto totalThreads = localSize[0] * localSize[1] * localSize[2];
    if (totalThreads > _mtlComputePipelineState.maxTotalThreadsPerThreadgroup)
    {
        AXLOGE("Compute local workgroup size {} exceeds maxTotalThreadsPerThreadgroup {}", totalThreads,
               _mtlComputePipelineState.maxTotalThreadsPerThreadgroup);
        [_mtlComputePipelineState release];
        _mtlComputePipelineState = nil;
    }
}

ComputePipelineImpl::~ComputePipelineImpl()
{
    if (_mtlComputePipelineState)
    {
        [_mtlComputePipelineState release];
        _mtlComputePipelineState = nil;
    }
}

}  // namespace ax::rhi::mtl
