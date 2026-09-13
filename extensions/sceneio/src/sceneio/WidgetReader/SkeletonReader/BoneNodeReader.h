/****************************************************************************
Copyright (c) 2015-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "sceneext/WidgetReader/NodeReaderProtocol.h"
#include "sceneext/WidgetReader/NodeReaderDefine.h"

namespace ax::ext
{
class BoneNodeReader : public ax::Object, public ext::NodeReaderProtocol
{
    DECLARE_CLASS_NODE_READER_INFO

public:
    BoneNodeReader();
    ~BoneNodeReader();

    static BoneNodeReader* getInstance();
    static void destroyInstance();

    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(
        pugi::xml_node objectData,
        flatbuffers::FlatBufferBuilder* builder) override;
    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* boneOptions) override;

    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* boneOptions) override;
};
}  // namespace ax::ext
