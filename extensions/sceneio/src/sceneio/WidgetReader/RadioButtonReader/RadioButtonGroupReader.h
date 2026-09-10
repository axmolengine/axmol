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
// x-studio spec
class SCNIO_API RadioButtonGroupReader : public WidgetReader
{
    DECLARE_CLASS_NODE_READER_INFO

public:
    RadioButtonGroupReader();
    virtual ~RadioButtonGroupReader();

    static RadioButtonGroupReader* getInstance();
    static void destroyInstance();

    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                                         flatbuffers::FlatBufferBuilder* builder);
    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table*);
    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table*);

    // int getResourceType(std::string_view key);
};
}  // namespace ax::ext
