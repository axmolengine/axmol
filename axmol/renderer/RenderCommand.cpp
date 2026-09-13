/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "axmol/renderer/RenderCommand.h"
#include "axmol/scene/Node.h"
#include "axmol/renderer/VertexLayoutManager.h"

namespace ax
{

RenderCommand::RenderCommand() {}

RenderCommand::~RenderCommand()
{
    if (_ownsPSVL) [[unlikely]]
    {
        releasePSVL();
    }
}

void RenderCommand::init(float globalZOrder, const ax::Mat4& transform, unsigned int flags, const SceneViewData& view)
{
    _globalOrder    = globalZOrder;
    _mv             = transform;
    _viewProjection = view.viewProjection;
    if (flags & Node::FLAGS_RENDER_AS_3D)
    {
        _depth = view.getDepthInView(transform);
        set3D(true);
    }
    else
    {
        set3D(false);
        _depth = 0;
    }
}

void RenderCommand::printID()
{
    AXLOGI("Command Depth: {}\n", _globalOrder);
}

void RenderCommand::setWeakPSVL(rhi::ProgramState* ps, rhi::VertexLayout* vl)
{
    // assert(ps && vl);

    if (_ownsPSVL) [[unlikely]]
    {
        AX_SAFE_RELEASE(_pipelineDesc.programState);
        AX_SAFE_RELEASE(_pipelineDesc.vertexLayout);
        _ownsPSVL = false;
    }
    if (ps)
        _pipelineDesc.programState = ps;
    if (vl)
        _pipelineDesc.vertexLayout = vl;
}

void RenderCommand::setOwnPSVL(rhi::ProgramState* ps, rhi::VertexLayout* vl, unsigned int adoptFlags)
{
    // assert(ps && vl);

    if (!(adoptFlags & ADOPT_FLAG_PS))
    {
        AX_SAFE_RETAIN(ps);
    }

    if (!(adoptFlags & ADOPT_FLAG_VL))
    {
        AX_SAFE_RETAIN(vl);
    }

    if (_ownsPSVL)
    {
        AX_SAFE_RELEASE(_pipelineDesc.programState);
        AX_SAFE_RELEASE(_pipelineDesc.vertexLayout);
    }

    _pipelineDesc.programState = ps;
    _pipelineDesc.vertexLayout = vl;
    _ownsPSVL                  = true;
}

void RenderCommand::releasePSVL()
{
    if (_ownsPSVL)
    {
        AX_SAFE_RELEASE(_pipelineDesc.programState);
        AX_SAFE_RELEASE(_pipelineDesc.vertexLayout);
        _ownsPSVL = false;
    }
}

bool RenderCommand::checkPSVL() const
{
    switch (_type)
    {
    case Type::QUAD_COMMAND:
    case Type::MESH_COMMAND:
    case Type::CUSTOM_COMMAND:
    case Type::TRIANGLES_COMMAND:
        return _pipelineDesc.programState && _pipelineDesc.vertexLayout;
    }

    return true;
}

}  // namespace ax
