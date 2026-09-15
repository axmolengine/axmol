/****************************************************************************
 Copyright (c) 2015 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include <string>
#include "sceneext/ComBase.h"
#include "axmol/scene/Component.h"
#include "sceneext/SceneExtMacros.h"

namespace ax::ext
{
namespace timeline
{
class ActionTimelineData;
}
}  // namespace ax::ext

namespace ax::ext
{
class SCNEXT_API ComExtensionData : public ax::Component
{
    DECLARE_CLASS_COMPONENT_INFO
public:
    const static std::string COMPONENT_NAME;

    /**
     * @lua NA
     */
    void onEnter() override;
    /**
     * @lua NA
     */
    void onExit() override;
    /**
     * @lua NA
     */
    void onAdd() override;
    /**
     * @lua NA
     */
    void onRemove() override;
    static ComExtensionData* create();
    bool init() override;

    virtual void setCustomProperty(std::string_view customProperty) { _customProperty = customProperty; }
    virtual std::string getCustomProperty() const { return _customProperty; };

    virtual void setActionTag(int actionTag);
    virtual int getActionTag() const;

public:
    ComExtensionData();
    ~ComExtensionData();

protected:
    std::string _customProperty;
    ax::ext::timeline::ActionTimelineData* _timelineData;
};
}  // namespace ax::ext
