/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.
 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/ui/HBox.h"

namespace ax
{

namespace ui
{

HBox::HBox() {}

HBox::~HBox() {}

HBox* HBox::create()
{
    HBox* widget = new HBox();
    if (widget->init())
    {
        widget->autorelease();
        return widget;
    }
    AX_SAFE_DELETE(widget);
    return nullptr;
}

HBox* HBox::create(const Vec2& size)
{
    HBox* widget = new HBox();
    if (widget->initWithSize(size))
    {
        widget->autorelease();
        return widget;
    }
    AX_SAFE_DELETE(widget);
    return nullptr;
}

bool HBox::init()
{
    if (LayoutGroup::init())
    {
        setLayoutType(LayoutGroup::Type::HORIZONTAL);
        return true;
    }
    return false;
}

bool HBox::initWithSize(const Vec2& size)
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
