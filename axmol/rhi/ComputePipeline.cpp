/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/rhi/ComputePipeline.h"

namespace ax::rhi
{

ComputePipeline::~ComputePipeline()
{
    AX_SAFE_RELEASE(_program);
}

void ComputePipeline::setProgram(Program* program)
{
    Object::assign(_program, program);
}
}  // namespace ax::rhi
