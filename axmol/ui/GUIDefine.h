/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include <string>
#include "axmol/base/ObjectFactory.h"

///@cond DO_NOT_SHOW
//
//// Widget macro
//

#define DECLARE_CLASS_GUI_INFO              \
public:                                     \
    static ax::ObjectFactory::TInfo __Type; \
    static ax::Object* createInstance();

#define IMPLEMENT_CLASS_GUI_INFO(className) \
    ax::Object* className::createInstance() \
    {                                       \
        return className::create();         \
    }                                       \
    ax::ObjectFactory::TInfo className::__Type(#className, &className::createInstance);

#define CREATE_CLASS_GUI_INFO(className) ax::ObjectFactory::TInfo(#className, &className::createInstance)

//
//// Reader macro
//

#define DECLARE_CLASS_WIDGET_READER_INFO    \
public:                                     \
    static ax::ObjectFactory::TInfo __Type; \
    static ax::Object* createInstance();

#define IMPLEMENT_CLASS_WIDGET_READER_INFO(className) \
    ax::Object* className::createInstance()           \
    {                                                 \
        return className::getInstance();              \
    }                                                 \
    ax::ObjectFactory::TInfo className::__Type(#className, &className::createInstance);

#define CREATE_CLASS_WIDGET_READER_INFO(className) ax::ObjectFactory::TInfo(#className, &className::createInstance)

#define AX_VIDEOPLAYER_DEBUG_DRAW                  0

#define __LAYOUT_COMPONENT_NAME                    "__ui_layout"
///@endcond

namespace ax
{
struct AX_DLL ResourceData
{
    int type;
    std::string file;
    std::string plist;

    ResourceData()
    {
        type  = 0;
        file  = "";
        plist = "";
    }
    ResourceData(const ResourceData& rv)
    {
        type  = rv.type;
        file  = rv.file;
        plist = rv.plist;
    }
    ResourceData(ResourceData&& rv) noexcept
    {
        type  = rv.type;
        file  = std::move(rv.file);
        plist = std::move(rv.plist);
    }

    ResourceData(int iType, std::string sFile, std::string sPlist)
    {
        type  = iType;
        file  = std::move(sFile);
        plist = std::move(sPlist);
    }
    ResourceData& operator=(const ResourceData& rv)
    {
        type  = rv.type;
        file  = rv.file;
        plist = rv.plist;
        return *this;
    }
    ResourceData& operator=(ResourceData&& rv) noexcept
    {
        type  = rv.type;
        file  = std::move(rv.file);
        plist = std::move(rv.plist);
        return *this;
    }
};
}  // namespace ax
