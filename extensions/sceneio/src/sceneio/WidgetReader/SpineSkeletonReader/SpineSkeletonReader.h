/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "sceneext/SceneExtMacros.h"
#include "sceneext/WidgetReader/NodeReaderProtocol.h"
#include "sceneext/WidgetReader/NodeReaderDefine.h"

#if defined(AX_ENABLE_EXT_SPINE)

namespace ax::ext
{
// x-studio spec
class SCNIO_API SpineSkeletonReader : public ax::Object, public NodeReaderProtocol
{
    DECLARE_CLASS_NODE_READER_INFO

public:
    SpineSkeletonReader();
    ~SpineSkeletonReader();

    static SpineSkeletonReader* getInstance();
    static void destroyInstance();

    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                                         flatbuffers::FlatBufferBuilder* builder);
    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* spriteOptions);
    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* spriteOptions);

    int getResourceType(std::string_view key);
};
}  // namespace ax::ext

#endif
