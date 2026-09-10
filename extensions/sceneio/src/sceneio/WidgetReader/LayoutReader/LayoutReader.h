/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "sceneio/WidgetReader/WidgetReader.h"
#include "sceneext/SceneExtMacros.h"

namespace ax::ext
{
class SCNIO_API LayoutReader : public WidgetReader
{

    DECLARE_CLASS_NODE_READER_INFO

public:
    LayoutReader();
    virtual ~LayoutReader();

    static LayoutReader* getInstance();
    static void destroyInstance();

    virtual void setPropsFromJsonDictionary(ax::ui::Widget* widget, const rapidjson::Value& options);
    virtual void setPropsFromBinary(ax::ui::Widget* widget, CocoLoader* cocoLoader, stExpCocoNode* pCocoNode);
    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                                         flatbuffers::FlatBufferBuilder* builder);
    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* layoutOptions);
    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* layoutOptions);

    int getResourceType(std::string_view key);
};

}  // namespace ax::ext
