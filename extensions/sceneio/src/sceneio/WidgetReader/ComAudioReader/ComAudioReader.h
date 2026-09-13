/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/base/Object.h"
#include "axmol/scene/Component.h"
#include "axmol/scene/Node.h"
#include "sceneext/SceneExtMacros.h"
#include "sceneext/WidgetReader/NodeReaderProtocol.h"

namespace ax::ext
{
class SCNIO_API ComAudioReader : public ax::Object, public NodeReaderProtocol
{

public:
    ComAudioReader();
    ~ComAudioReader();

    static ComAudioReader* getInstance();
    static void destroyInstance();

    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(
        pugi::xml_node objectData,
        flatbuffers::FlatBufferBuilder* builder) override;
    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* comAudioOptions) override;
    ax::Component* createComAudioWithFlatBuffers(const flatbuffers::Table* comAudioOptions);
    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions) override;
};
}  // namespace ax::ext
