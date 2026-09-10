/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#include "sceneext/BatchNode.h"
#include "sceneext/Armature.h"
#include "sceneext/Skin.h"

#include "axmol/renderer/Renderer.h"
#include "axmol/renderer/GroupCommand.h"
#include "axmol/base/Director.h"

using namespace ax;

namespace ax::ext
{
BatchNode* BatchNode::create()
{
    BatchNode* batchNode = new BatchNode();
    if (batchNode->init())
    {
        batchNode->autorelease();
        return batchNode;
    }
    AX_SAFE_DELETE(batchNode);
    return nullptr;
}
BatchNode::BatchNode() /*: _groupCommand(nullptr)*/ {}
BatchNode::~BatchNode() {}

bool BatchNode::init()
{
    bool ret = Node::init();
    return ret;
}

void BatchNode::addChild(Node* child, int zOrder, int tag)
{
    Node::addChild(child, zOrder, tag);
    Armature* armature = dynamic_cast<Armature*>(child);
    if (armature != nullptr)
    {
        armature->setBatchNode(this);
    }
}

void BatchNode::addChild(ax::Node* child, int zOrder, std::string_view name)
{
    Node::addChild(child, zOrder, name);
    Armature* armature = dynamic_cast<Armature*>(child);
    if (armature != nullptr)
    {
        armature->setBatchNode(this);
    }
}

void BatchNode::removeChild(Node* child, bool cleanup)
{
    Armature* armature = dynamic_cast<Armature*>(child);
    if (armature != nullptr)
    {
        armature->setBatchNode(nullptr);
    }

    Node::removeChild(child, cleanup);
}

void BatchNode::visit(const SceneRenderState& state, const Mat4& parentTransform, uint32_t parentFlags)
{
    // quick return if not visible. children won't be drawn.
    if (!_visible)
    {
        return;
    }

    uint32_t flags = processParentFlags(state, parentTransform, parentFlags);

    if (isVisitableByCamera(state.cameraFlag))
    {
        sortAllChildren();
        draw(state, _modelViewTransform, flags);
    }
}

void BatchNode::draw(const SceneRenderState& state, const Mat4& transform, uint32_t flags)
{
    if (_children.empty())
    {
        return;
    }

    //    AX_NODE_DRAW_SETUP();

    bool pushed = false;
    for (auto&& object : _children)
    {
        Armature* armature = dynamic_cast<Armature*>(object);
        if (armature)
        {
            if (!pushed)
            {
                generateGroupCommand();
                pushed = true;
            }

            armature->visit(state, transform, flags);
        }
        else
        {
            state.getRenderer()->popGroup();
            pushed = false;

            ((Node*)object)->visit(state, transform, flags);
        }
    }
}

void BatchNode::generateGroupCommand()
{
    Renderer* renderer = Director::getInstance()->getRenderer();
    auto* groupCommand = renderer->getNextGroupCommand();
    groupCommand->init(_globalZOrder);
    renderer->addCommand(groupCommand);

    renderer->pushGroup(groupCommand->getRenderQueueID());
}

}  // namespace ax::ext
