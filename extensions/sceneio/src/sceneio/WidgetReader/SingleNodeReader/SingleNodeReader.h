/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "sceneext/SceneExtMacros.h"
#include "sceneext/WidgetReader/NodeReaderProtocol.h"
#include "sceneext/WidgetReader/NodeReaderDefine.h"

namespace ax::ext
{
class SCNIO_API SingleNodeReader : public ax::Object, public NodeReaderProtocol
{
    DECLARE_CLASS_NODE_READER_INFO

public:
    SingleNodeReader();
    ~SingleNodeReader();

    static SingleNodeReader* getInstance();
    static void destroyInstance();

    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                                         flatbuffers::FlatBufferBuilder* builder);
    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* singleNodeOptions);
    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* singleNodeOptions);
};
}  // namespace ax::ext
