/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "sceneio/WidgetReader/ScrollViewReader/ScrollViewReader.h"
#include "sceneext/SceneExtMacros.h"

namespace ax::ext
{
class SCNIO_API ListViewReader : public ScrollViewReader
{
    DECLARE_CLASS_NODE_READER_INFO

public:
    ListViewReader();
    virtual ~ListViewReader();

    static ListViewReader* getInstance();
    static void destroyInstance();

    virtual void setPropsFromJsonDictionary(ax::ui::Widget* widget, const rapidjson::Value& options);
    virtual void setPropsFromBinary(ax::ui::Widget* widget, CocoLoader* cocoLoader, stExpCocoNode* pCocoNode);
    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                                         flatbuffers::FlatBufferBuilder* builder);
    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* listViewOptions);
    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* listViewOptions);

    int getResourceType(std::string_view key);
};
}  // namespace ax::ext
