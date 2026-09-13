/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/ui/RelativeBox.h"

namespace ax
{

namespace ui
{

RelativeBox::RelativeBox() {}

RelativeBox::~RelativeBox() {}

RelativeBox* RelativeBox::create()
{
    RelativeBox* widget = new RelativeBox();
    if (widget->init())
    {
        widget->autorelease();
        return widget;
    }
    AX_SAFE_DELETE(widget);
    return nullptr;
}

RelativeBox* RelativeBox::create(const Vec2& size)
{
    RelativeBox* widget = new RelativeBox();
    if (widget->initWithSize(size))
    {
        widget->autorelease();
        return widget;
    }
    AX_SAFE_DELETE(widget);
    return nullptr;
}

bool RelativeBox::init()
{
    if (Layout::init())
    {
        setLayoutType(Layout::Type::RELATIVE);
        return true;
    }
    return false;
}

bool RelativeBox::initWithSize(const Vec2& size)
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
