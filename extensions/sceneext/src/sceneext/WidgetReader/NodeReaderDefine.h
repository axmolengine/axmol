/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include <string>
#include "axmol/base/ObjectFactory.h"

//
//// Reader macro
//

#define DECLARE_CLASS_NODE_READER_INFO      \
public:                                     \
    static ax::ObjectFactory::TInfo __Type; \
    static ax::Object* createInstance(void);

#define IMPLEMENT_CLASS_NODE_READER_INFO(className) \
    ax::Object* className::createInstance(void)     \
    {                                               \
        return className::getInstance();            \
    }                                               \
    ax::ObjectFactory::TInfo className::__Type(#className, &className::createInstance);

#define CREATE_CLASS_NODE_READER_INFO(className) ax::ObjectFactory::TInfo(#className, &className::createInstance)

#define FLATSTR_TO_BOOL(str)                     (str.compare("True") == 0) ? true : false
