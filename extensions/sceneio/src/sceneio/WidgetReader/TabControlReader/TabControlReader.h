/****************************************************************************
 Copyright (c) 2016 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "sceneext/WidgetReader/NodeReaderProtocol.h"
#include "sceneext/WidgetReader/NodeReaderDefine.h"
#include "sceneio/WidgetReader/TabControlReader/CSTabControl_generated.h"

namespace ax::ext
{
class TabControlReader : public ax::Object, public ext::NodeReaderProtocol
{
    DECLARE_CLASS_NODE_READER_INFO

public:
    TabControlReader();
    ~TabControlReader();

    static TabControlReader* getInstance();
    static void destroyInstance();

    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(
        pugi::xml_node objectData,
        flatbuffers::FlatBufferBuilder* builder) override;

    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* nodeOption) override;

    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions) override;

private:
    static TabControlReader* _tabReaderInstance;
};

class TabHeaderReader : public ax::Object, public ext::NodeReaderProtocol
{
    DECLARE_CLASS_NODE_READER_INFO

public:
    TabHeaderReader();
    ~TabHeaderReader();

    static TabHeaderReader* getInstance();
    static void destroyInstance();

    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(
        pugi::xml_node objectData,
        flatbuffers::FlatBufferBuilder* builder) override;

    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* nodeOption) override;

    // CSArmatureNode
    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions) override;

private:
    static TabHeaderReader* _tabheaderReaderInstance;
    int getResourceType(std::string_view key);
};

class TabItemReader : public ax::Object, public ext::NodeReaderProtocol
{
    DECLARE_CLASS_NODE_READER_INFO

public:
    TabItemReader();
    ~TabItemReader();

    static TabItemReader* getInstance();
    static void destroyInstance();

    flatbuffers::Offset<flatbuffers::TabItemOption> createTabItemOptionWithFlatBuffers(
        pugi::xml_node objectData,
        flatbuffers::FlatBufferBuilder* builder);

    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* nodeOption) override;

    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions) override;

    virtual flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(
        pugi::xml_node objectData,
        flatbuffers::FlatBufferBuilder* builder) override;

private:
    static TabItemReader* _tabItemReaderInstance;
};
}  // namespace ax::ext
