/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "sceneext/sceneext.h"
#include "sceneext/TriggerObj.h"
#include "sceneext/TriggerMng.h"
#include "sceneext/SceneExtMacros.h"

#include "axmol/base/ObjectFactory.h"

#define DECLARE_CLASS_INFO                \
public:                                   \
    static ax::ObjectFactory::TInfo Type; \
    static ax::Object* createInstance(void);

#define IMPLEMENT_CLASS_INFO(className)         \
    ax::Object* className::createInstance(void) \
    {                                           \
        auto ret = new className;               \
        ret->autorelease();                     \
        return ret;                             \
    }                                           \
    ax::ObjectFactory::TInfo className::Type(#className, &className::createInstance);

void SCNEXT_API sendEvent(unsigned int event);
