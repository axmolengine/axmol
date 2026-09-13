/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "sceneio/WidgetReader/WidgetReader.h"
#include "sceneext/SceneExtMacros.h"

namespace ax::ext
{
class SCNIO_API TextAtlasReader : public WidgetReader
{
    DECLARE_CLASS_NODE_READER_INFO

public:
    TextAtlasReader();
    virtual ~TextAtlasReader();

    static TextAtlasReader* getInstance();
    static void destroyInstance();

    void setPropsFromJsonDictionary(ax::ui::Widget* widget, const rapidjson::Value& options) override;
    void setPropsFromBinary(ax::ui::Widget* widget, CocoLoader* cocoLoader, stExpCocoNode* pCocoNode) override;
    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                                         flatbuffers::FlatBufferBuilder* builder);
    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* textAtlasOptions) override;
    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* textAtlasOptions) override;
};
}  // namespace ax::ext
