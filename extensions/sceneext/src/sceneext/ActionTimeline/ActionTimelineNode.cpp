/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "sceneext/ActionTimeline/ActionTimelineNode.h"

using namespace ax;

NS_TIMELINE_BEGIN

ActionTimelineNode* ActionTimelineNode::create(Node* root, ActionTimeline* action)
{
    ActionTimelineNode* ret = new ActionTimelineNode();
    if (ret->init(root, action))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

ActionTimelineNode::ActionTimelineNode() : _root(nullptr), _action(nullptr) {}

ActionTimelineNode::~ActionTimelineNode() {}

bool ActionTimelineNode::init()
{
    return Node::init();
}

bool ActionTimelineNode::init(Node* root, ActionTimeline* action)
{
    _root   = root;
    _action = action;

    if (_root)
    {
        _root->removeFromParent();
        addChild(_root);
    }

    return true;
}

void ActionTimelineNode::setRoot(ax::Node* root)
{
    _root = root;
}
ax::Node* ActionTimelineNode::getRoot()
{
    return _root;
}

void ActionTimelineNode::setActionTimeline(ActionTimeline* action)
{
    _action = action;
}
ActionTimeline* ActionTimelineNode::getActionTimeline()
{
    return _action;
}

NS_TIMELINE_END
