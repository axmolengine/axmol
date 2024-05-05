#pragma once

#include "cocostudio/ComBase.h"
#include "2d/Component.h"
#include "cocostudio/CocosStudioExport.h"
#include "TimelineMacro.h"

NS_TIMELINE_BEGIN

class CCS_DLL ActionTimelineData : public ax::Object
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
