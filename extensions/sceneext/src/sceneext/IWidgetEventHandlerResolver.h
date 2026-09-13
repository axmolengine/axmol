/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "sceneext/SceneExtMacros.h"
#include "axmol/ui/Widget.h"

namespace ax::ext
{

class SCNEXT_API IWidgetEventHandlerResolver
{
public:
    virtual ~IWidgetEventHandlerResolver()                                                         = 0;
    virtual ax::ui::Widget::PointerEventHandler resolvePointerEvent(std::string_view callBackName) = 0;
    virtual ax::ui::Widget::ClickEventHandler resolveClickEvent(std::string_view callBackName)     = 0;
    virtual ax::ui::Widget::WidgetEventCallback resolveEvent(std::string_view callBackName)        = 0;
};

}  // namespace ax::ext
