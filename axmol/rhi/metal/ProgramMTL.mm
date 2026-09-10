/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/rhi/metal/ProgramMTL.h"
#include "axmol/rhi/metal/ShaderModuleMTL.h"
#include "axmol/base/Macros.h"
#include "axmol/rhi/metal/GraphicsDeviceMTL.h"

namespace ax::rhi::mtl
{
namespace
{
// constexpr std::string_view metalSpecificDefine = "#define METAL\n"sv;
}

ProgramImpl::ProgramImpl(Data& vsData, Data& fsData) : Program(vsData, fsData) {}

ProgramImpl::ProgramImpl(Data& csData) : Program(csData) {}

}  // namespace ax::rhi::mtl
