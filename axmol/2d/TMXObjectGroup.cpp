/****************************************************************************
Copyright (c) 2010      Neophit
Copyright (c) 2010      Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#include "axmol/2d/TMXObjectGroup.h"
#include "axmol/base/Macros.h"

namespace ax
{

// implementation TMXObjectGroup

TMXObjectGroup::TMXObjectGroup() : _groupName("") {}

TMXObjectGroup::~TMXObjectGroup()
{
    AXLOGV("deallocing TMXObjectGroup: {}", fmt::ptr(this));
}

ValueMap TMXObjectGroup::getObject(std::string_view objectName) const
{
    if (!_objects.empty())
    {
        for (const auto& v : _objects)
        {
            const ValueMap& dict = v.asValueMap();
            if (dict.find("name") != dict.end())
            {
                if (dict.at("name").asString() == objectName)
                    return dict;
            }
        }
    }

    // object not found
    return ValueMap();
}

Value TMXObjectGroup::getProperty(std::string_view propertyName) const
{
    if (_properties.find(propertyName) != _properties.end())
        return _properties.at(propertyName);

    return Value();
}

}  // namespace ax
