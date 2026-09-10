/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "sceneext/DictionaryHelper.h"
#include "sceneext/SceneExtMacros.h"

#include "pugixml/pugixml.hpp"
#include "pugixml/pugiext.hpp"

// protobuf present at cocos2d-x-3.3rc1@libccs19
namespace protocolbuffers
{
class NodeTree;
class ResourceData;  // please match in CSParseBinary.pb
}  // namespace protocolbuffers

namespace ax
{
namespace ui
{
class Widget;
}
}  // namespace ax

namespace ax::ext
{
class CocoLoader;
struct stExpCocoNode;

class SCNEXT_API WidgetReaderProtocol
{
public:
    virtual ~WidgetReaderProtocol();
    virtual void setPropsFromJsonDictionary(ax::ui::Widget* widget, const rapidjson::Value& options)          = 0;
    virtual void setPropsFromBinary(ax::ui::Widget* widget, CocoLoader* cocoLoader, stExpCocoNode* pCocoNode) = 0;
    virtual void setPropsFromProtocolBuffers(
        ax::ui::Widget* widget,
        const protocolbuffers::NodeTree& nodeTree);  // cocos2d-x-3.3rc1@libccs19 spec
    virtual void setPropsFromXML(ax::ui::Widget* widget,
                                 pugi::xml_node objectData);  // cocos2d-x-3.3rc1@libccs19 spec
};
}  // namespace ax::ext
