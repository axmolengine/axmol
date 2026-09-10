/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include <string>
#include "sceneext/DictionaryHelper.h"
#include "sceneext/SceneExtMacros.h"
#include "axmol/base/ObjectFactory.h"
#include "sceneext/CocoLoader.h"

#define DECLARE_CLASS_COMPONENT_INFO      \
public:                                   \
    static ax::ObjectFactory::TInfo Type; \
    static ax::Object* createInstance(void);

#define IMPLEMENT_CLASS_COMPONENT_INFO(className) \
    ax::Object* className::createInstance(void)   \
    {                                             \
        return className::create();               \
    }                                             \
    ax::ObjectFactory::TInfo className::Type(#className, &className::createInstance);

#define CREATE_CLASS_COMPONENT_INFO(className) ax::ObjectFactory::TInfo(#className, &className::createInstance)

struct SCNEXT_API SerData
{
    const rapidjson::Value* _rData;
    ax::ext::stExpCocoNode* _cocoNode;
    ax::ext::CocoLoader* _cocoLoader;
    SerData()
    {
        _rData      = NULL;
        _cocoNode   = NULL;
        _cocoLoader = NULL;
    }
};
