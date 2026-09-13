/****************************************************************************
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#include "axmol/ui/CheckBox.h"

namespace ax
{

namespace ui
{

IMPLEMENT_CLASS_GUI_INFO(CheckBox)

CheckBox* CheckBox::create()
{
    CheckBox* widget = new CheckBox();
    if (widget->init())
    {
        widget->autorelease();
        return widget;
    }
    AX_SAFE_DELETE(widget);
    return nullptr;
}

CheckBox* CheckBox::create(std::string_view backGround,
                           std::string_view backGroundSelected,
                           std::string_view cross,
                           std::string_view backGroundDisabled,
                           std::string_view frontCrossDisabled,
                           TextureResType texType)
{
    CheckBox* pWidget = new CheckBox;
    if (pWidget->init(backGround, backGroundSelected, cross, backGroundDisabled, frontCrossDisabled, texType))
    {
        pWidget->autorelease();
        return pWidget;
    }
    AX_SAFE_DELETE(pWidget);
    return nullptr;
}

CheckBox* CheckBox::create(std::string_view backGround, std::string_view cross, TextureResType texType)
{
    CheckBox* pWidget = new CheckBox;
    if (pWidget->init(backGround, "", cross, "", "", texType))
    {
        pWidget->autorelease();
        return pWidget;
    }
    AX_SAFE_DELETE(pWidget);
    return nullptr;
}

void CheckBox::onPointerUp(PointerEvent* event)
{
    bool highlight = _highlight;

    AbstractCheckButton::onPointerUp(event);

    if (highlight)
    {
        if (_isSelected)
        {
            setSelected(false);
            dispatchSelectChangedEvent(false);
        }
        else
        {
            setSelected(true);
            dispatchSelectChangedEvent(true);
        }
    }
}

void CheckBox::dispatchSelectChangedEvent(bool selected)
{
    auto eventType = selected ? EventType::SELECTED : EventType::UNSELECTED;
    this->retain();
    if (_eventCallback)
    {
        _eventCallback(this, eventType);
    }
    if (_customEventCallback)
    {
        _customEventCallback(this, static_cast<int>(eventType));
    }
    this->release();
}

void CheckBox::addEventListener(const CheckBoxCallback& callback)
{
    _eventCallback = callback;
}

std::string CheckBox::getDescription() const
{
    return "CheckBox";
}

Widget* CheckBox::createCloneInstance()
{
    return CheckBox::create();
}

void CheckBox::copySpecialProperties(Widget* widget)
{
    CheckBox* checkBox = dynamic_cast<CheckBox*>(widget);
    if (checkBox)
    {
        AbstractCheckButton::copySpecialProperties(widget);
        _eventCallback = checkBox->_eventCallback;
    }
}

}  // namespace ui

}  // namespace ax
