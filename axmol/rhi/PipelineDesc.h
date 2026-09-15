/****************************************************************************
 Copyright (c) 2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/platform/PlatformMacros.h"
#include "axmol/rhi/DepthStencilState.h"
#include "axmol/rhi/Texture.h"
#include "axmol/rhi/VertexLayout.h"
#include "axmol/rhi/RenderPassDesc.h"
#include "axmol/rhi/ProgramState.h"

#include <string>

/**
 * @addtogroup rhi
 * @{
 */
namespace ax
{
namespace rhi
{
struct PipelineDesc
{
    ProgramState* programState{nullptr};
    VertexLayout* vertexLayout{nullptr};
    BlendDesc blendDesc;
};
}  // namespace rhi

// end of rhi group
/// @}

using PipelineDesc = rhi::PipelineDesc;

}  // namespace ax
