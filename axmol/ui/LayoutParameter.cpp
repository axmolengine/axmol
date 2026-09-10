/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/ui/LayoutParameter.h"
#include "axmol/ui/LayoutGroup.h"

namespace ax
{

namespace ui
{

const Margin Margin::ZERO = Margin(0, 0, 0, 0);

Margin::Margin(void) : left(0), top(0), right(0), bottom(0) {}

Margin::Margin(float l, float t, float r, float b) : left(l), top(t), right(r), bottom(b) {}

Margin::Margin(const Margin& other) : left(other.left), top(other.top), right(other.right), bottom(other.bottom) {}

Margin& Margin::operator=(const Margin& other)
{
    setMargin(other.left, other.top, other.right, other.bottom);
    return *this;
}

void Margin::setMargin(float l, float t, float r, float b)
{
    left   = l;
    top    = t;
    right  = r;
    bottom = b;
}

bool Margin::equals(const Margin& target) const
{
    return (left == target.left && top == target.top && right == target.right && bottom == target.bottom);
}

LayoutParameter* LayoutParameter::create()
{
    LayoutParameter* parameter = new LayoutParameter();
    parameter->autorelease();
    return parameter;
}

void LayoutParameter::setMargin(const Margin& margin)
{
    _margin = margin;
}

const Margin& LayoutParameter::getMargin() const
{
    return _margin;
}

LayoutParameter::Type LayoutParameter::getLayoutType() const
{
    return _layoutParameterType;
}

LayoutParameter* LayoutParameter::clone()
{
    LayoutParameter* clonedParameter = createCloneInstance();
    clonedParameter->copyProperties(this);
    return clonedParameter;
}

LayoutParameter* LayoutParameter::createCloneInstance()
{
    return LayoutParameter::create();
}

void LayoutParameter::copyProperties(LayoutParameter* model)
{
    _margin = model->_margin;
}

LinearLayoutParameter* LinearLayoutParameter::create()
{
    LinearLayoutParameter* parameter = new LinearLayoutParameter();
    parameter->autorelease();
    return parameter;
}

void LinearLayoutParameter::setGravity(LinearGravity gravity)
{
    _linearGravity = gravity;
}

LinearLayoutParameter::LinearGravity LinearLayoutParameter::getGravity() const
{
    return _linearGravity;
}

LayoutParameter* LinearLayoutParameter::createCloneInstance()
{
    return LinearLayoutParameter::create();
}

void LinearLayoutParameter::copyProperties(LayoutParameter* model)
{
    LayoutParameter::copyProperties(model);
    LinearLayoutParameter* parameter = dynamic_cast<LinearLayoutParameter*>(model);
    if (parameter)
    {
        setGravity(parameter->_linearGravity);
    }
}

RelativeLayoutParameter* RelativeLayoutParameter::create()
{
    RelativeLayoutParameter* parameter = new RelativeLayoutParameter();
    parameter->autorelease();
    return parameter;
}

void RelativeLayoutParameter::setAlign(RelativeAlign align)
{
    _relativeAlign = align;
}

RelativeLayoutParameter::RelativeAlign RelativeLayoutParameter::getAlign() const
{
    return _relativeAlign;
}

void RelativeLayoutParameter::setRelativeToWidgetName(std::string_view name)
{
    _relativeWidgetName = name;
}

std::string_view RelativeLayoutParameter::getRelativeToWidgetName() const
{
    return _relativeWidgetName;
}

void RelativeLayoutParameter::setRelativeName(std::string_view name)
{
    _relativeLayoutName = name;
}

std::string_view RelativeLayoutParameter::getRelativeName() const
{
    return _relativeLayoutName;
}

LayoutParameter* RelativeLayoutParameter::createCloneInstance()
{
    return RelativeLayoutParameter::create();
}

void RelativeLayoutParameter::copyProperties(LayoutParameter* model)
{
    LayoutParameter::copyProperties(model);
    RelativeLayoutParameter* parameter = dynamic_cast<RelativeLayoutParameter*>(model);
    if (parameter)
    {
        setAlign(parameter->_relativeAlign);
        setRelativeName(parameter->_relativeLayoutName);
        setRelativeToWidgetName(parameter->_relativeWidgetName);
    }
}

}  // namespace ui

}  // namespace ax
