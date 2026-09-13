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
// x-studio spec
class SCNIO_API RichTextReader : public WidgetReader
{
    DECLARE_CLASS_NODE_READER_INFO

public:
    RichTextReader();
    virtual ~RichTextReader();

    static RichTextReader* getInstance();
    static void destroyInstance();

    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(
        pugi::xml_node objectData,
        flatbuffers::FlatBufferBuilder* builder) override;
    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* textBMFontOptions) override;
    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* textBMFontOptions) override;
};
}  // namespace ax::ext
