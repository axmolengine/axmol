/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "axmol/rhi/vulkan/ProgramVK.h"
#include "axmol/rhi/vulkan/GraphicsDeviceVK.h"
#include "axmol/rhi/ShaderCache.h"
#include "axmol/base/Logging.h"

namespace ax::rhi::vk
{

ProgramImpl::ProgramImpl(Data& vsData, Data& fsData) : Program(vsData, fsData) {}

ProgramImpl::ProgramImpl(Data& csData) : Program(csData) {}

ProgramImpl::~ProgramImpl()
{
    auto gfxDevice = static_cast<GraphicsDeviceImpl*>(GraphicsCore::device());
    if (gfxDevice)
        gfxDevice->removeCachedPipelineObjects(this);
}

}  // namespace ax::rhi::vk
