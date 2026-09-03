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
