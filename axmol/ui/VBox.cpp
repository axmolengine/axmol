/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/ui/VBox.h"

namespace ax
{

namespace ui
{

VBox::VBox() {}

VBox::~VBox() {}

VBox* VBox::create()
{
    VBox* widget = new VBox();
    if (widget->init())
    {
        widget->autorelease();
        return widget;
    }
    AX_SAFE_DELETE(widget);
    return nullptr;
}

VBox* VBox::create(const Vec2& size)
{
    VBox* widget = new VBox();
    if (widget->initWithSize(size))
    {
        widget->autorelease();
        return widget;
    }
    AX_SAFE_DELETE(widget);
    return nullptr;
}

bool VBox::init()
{
    if (LayoutGroup::init())
    {
        setLayoutType(LayoutGroup::Type::VERTICAL);
        return true;
    }
    return false;
}

bool VBox::initWithSize(const Vec2& size)
{
    if (init())
    {
        setContentSize(size);
        return true;
    }
    return false;
}

}  // namespace ui

}  // namespace ax
