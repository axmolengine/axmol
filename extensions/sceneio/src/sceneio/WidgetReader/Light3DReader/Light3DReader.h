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
class SCNIO_API Light3DReader : public ax::Object, public NodeReaderProtocol
{
    DECLARE_CLASS_NODE_READER_INFO

public:
    Light3DReader();
    ~Light3DReader();

    static Light3DReader* getInstance();
    static void destroyInstance();

    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                                         flatbuffers::FlatBufferBuilder* builder);
    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* light3DOptions);
    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* light3DOptions);
};
}  // namespace ax::ext
