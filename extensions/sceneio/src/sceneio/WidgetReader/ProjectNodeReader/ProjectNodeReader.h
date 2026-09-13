/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/base/Object.h"
#include "sceneext/SceneExtMacros.h"
#include "sceneext/WidgetReader/NodeReaderProtocol.h"

namespace ax::ext
{
class SCNIO_API ProjectNodeReader : public ax::Object, public NodeReaderProtocol
{

public:
    ProjectNodeReader();
    ~ProjectNodeReader();

    static ProjectNodeReader* getInstance();
    static void destroyInstance();

    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                                         flatbuffers::FlatBufferBuilder* builder);

    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* projectNodeOptions);
    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions);
};
}  // namespace ax::ext
