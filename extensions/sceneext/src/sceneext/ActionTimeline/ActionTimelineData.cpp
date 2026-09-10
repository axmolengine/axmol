/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "ActionTimelineData.h"

NS_TIMELINE_BEGIN

// ActionTimelineData
ActionTimelineData* ActionTimelineData::create(int actionTag)
{
    ActionTimelineData* ret = new ActionTimelineData();
    if (ret->init(actionTag))
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

ActionTimelineData::ActionTimelineData() : _actionTag(0) {}

bool ActionTimelineData::init(int actionTag)
{
    _actionTag = actionTag;
    return true;
}

NS_TIMELINE_END
