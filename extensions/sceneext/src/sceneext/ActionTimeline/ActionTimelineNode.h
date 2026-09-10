/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "sceneext/ActionTimeline/ActionTimeline.h"
#include "sceneext/ActionTimeline/TimelineMacro.h"
#include "sceneext/SceneExtMacros.h"

NS_TIMELINE_BEGIN

class ActionTimeline;

class SCNEXT_API ActionTimelineNode : public ax::Node
{
public:
    static ActionTimelineNode* create(ax::Node* root, ActionTimeline* action);

    ActionTimelineNode();
    ~ActionTimelineNode();

    bool init() override;
    bool init(ax::Node* root, ActionTimeline* action);

    virtual void setRoot(ax::Node* root);
    virtual ax::Node* getRoot();

    virtual void setActionTimeline(ActionTimeline* action);
    virtual ActionTimeline* getActionTimeline();

protected:
    ax::Node* _root;
    ActionTimeline* _action;
};

NS_TIMELINE_END
