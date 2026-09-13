/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/scene/CameraBackgroundBrush.h"
#include "sceneext/SceneExtMacros.h"
#include "sceneext/WidgetReader/NodeReaderProtocol.h"
#include "sceneext/WidgetReader/NodeReaderDefine.h"

namespace ax::ext
{
class SCNIO_API GameNode3DReader : public ax::Object, public NodeReaderProtocol
{
    DECLARE_CLASS_NODE_READER_INFO

public:
    GameNode3DReader();
    ~GameNode3DReader();

    static GameNode3DReader* getInstance();
    static ax::CameraBackgroundBrush* getSceneBrushInstance();
    static void destroyInstance();

    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                                         flatbuffers::FlatBufferBuilder* builder);
    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* node3DOptions);
    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* node3DOptions);

protected:
    int getResourceType(std::string_view key);
};
}  // namespace ax::ext
