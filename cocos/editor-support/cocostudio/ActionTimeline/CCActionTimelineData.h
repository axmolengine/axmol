#pragma once

#include "editor-support/cocostudio/CCComBase.h"
#include "2d/CCComponent.h"
#include "editor-support/cocostudio/CocosStudioExport.h"
#include "editor-support/cocostudio/ActionTimeline/CCTimelineMacro.h"

NS_TIMELINE_BEGIN

class CC_STUDIOP_DLL ActionTimelineData : public cocos2d::Ref
{
public:
    static ActionTimelineData* create(int actionTag);

    virtual void setActionTag(int actionTag) { _actionTag = actionTag; }
    virtual int getActionTag() const { return _actionTag; }
CC_CONSTRUCTOR_ACCESS:
    ActionTimelineData();
    virtual bool init(int actionTag);
protected:
    int _actionTag;
};

NS_TIMELINE_END
