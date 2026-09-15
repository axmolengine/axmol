/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "sceneext/ComBase.h"
#include "axmol/scene/Component.h"
#include "sceneext/SceneExtMacros.h"
#include "sceneext/ActionTimeline/TimelineMacro.h"

NS_TIMELINE_BEGIN

class SCNEXT_API ActionTimelineData : public ax::Object
{
public:
    static ActionTimelineData* create(int actionTag);

    virtual void setActionTag(int actionTag) { _actionTag = actionTag; }
    virtual int getActionTag() const { return _actionTag; }
    ActionTimelineData();
    virtual bool init(int actionTag);

protected:
    int _actionTag;
};

NS_TIMELINE_END
