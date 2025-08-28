/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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
#include "axmol/renderer/RenderCommand.h"
#include "axmol/2d/Camera.h"
#include "axmol/2d/Node.h"
#include "axmol/renderer/VertexLayoutManager.h"

namespace ax
{

RenderCommand::RenderCommand() {}

RenderCommand::~RenderCommand() {}

void RenderCommand::init(float globalZOrder, const ax::Mat4& transform, unsigned int flags)
{
    _globalOrder = globalZOrder;
    if (flags & Node::FLAGS_RENDER_AS_3D)
    {
        if (Camera::getVisitingCamera())
            _depth = Camera::getVisitingCamera()->getDepthInView(transform);

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
