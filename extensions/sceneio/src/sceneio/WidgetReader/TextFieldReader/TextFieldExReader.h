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
class SCNIO_API TextFieldExReader : public ax::Object, public NodeReaderProtocol
{
    DECLARE_CLASS_NODE_READER_INFO

public:
    TextFieldExReader();
    virtual ~TextFieldExReader();

    static TextFieldExReader* getInstance();
    static void destroyInstance();

    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                                         flatbuffers::FlatBufferBuilder* builder);
    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* textFieldOptions);
    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* textFieldOptions);
};
}  // namespace ax::ext
