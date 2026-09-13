/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/math/Vec3.h"
#include "sceneext/SceneExtMacros.h"
#include "sceneext/WidgetReader/NodeReaderProtocol.h"
#include "sceneext/WidgetReader/NodeReaderDefine.h"

namespace ax::ext
{
class SCNIO_API Node3DReader : public ax::Object, public NodeReaderProtocol
{
    DECLARE_CLASS_NODE_READER_INFO

public:
    Node3DReader();
    ~Node3DReader();

    static Node3DReader* getInstance();
    static void destroyInstance();

    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                                         flatbuffers::FlatBufferBuilder* builder);
    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* node3DOptions);
    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* node3DOptions);

protected:
    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffersForNode(
        pugi::xml_node objectData,
        flatbuffers::FlatBufferBuilder* builder);
    ax::Vec3 getVec3Attribute(pugi::xml_attribute attribute) const;
};
}  // namespace ax::ext
