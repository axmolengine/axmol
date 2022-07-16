#include "CCActionTimelineData.h"

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
