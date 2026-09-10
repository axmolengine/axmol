/****************************************************************************
 Copyright (c) 2015 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "sceneext/ComExtensionData.h"
#include "sceneext/ActionTimeline/ActionTimelineData.h"

namespace ax::ext
{
IMPLEMENT_CLASS_COMPONENT_INFO(ComExtensionData)

const std::string ComExtensionData::COMPONENT_NAME = "ComExtensionData";

ComExtensionData::ComExtensionData() : _customProperty(""), _timelineData(nullptr)
{
    _name = COMPONENT_NAME;
}

ComExtensionData::~ComExtensionData()
{
    AX_SAFE_RELEASE(_timelineData);
}

ComExtensionData* ComExtensionData::create()
{
    ComExtensionData* ret = new ComExtensionData();
    if (ret->init())
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

bool ComExtensionData::init()
{
    _timelineData = ::ax::ext::timeline::ActionTimelineData::create(0);
    AX_SAFE_RETAIN(_timelineData);

    return true;
}

void ComExtensionData::onEnter() {}

void ComExtensionData::onExit()
{
    onRemove();
}

void ComExtensionData::onAdd() {}

void ComExtensionData::onRemove() {}

void ComExtensionData::setActionTag(int actionTag)
{
    _timelineData->setActionTag(actionTag);
}

int ComExtensionData::getActionTag() const
{
    return _timelineData->getActionTag();
}

}  // namespace ax::ext
