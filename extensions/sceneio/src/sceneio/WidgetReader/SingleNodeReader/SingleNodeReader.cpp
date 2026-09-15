/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "sceneio/WidgetReader/SingleNodeReader/SingleNodeReader.h"

#include "sceneio/CSParseBinary_generated.h"
#include "sceneext/ActionTimeline/ActionTimeline.h"
#include "sceneio/WidgetReader/NodeReader/NodeReader.h"

#include "flatbuffers/flatbuffers.h"

using namespace flatbuffers;

namespace ax::ext
{
IMPLEMENT_CLASS_NODE_READER_INFO(SingleNodeReader)

SingleNodeReader::SingleNodeReader() {}

SingleNodeReader::~SingleNodeReader() {}

static SingleNodeReader* _instanceSingleNodeReader = nullptr;

SingleNodeReader* SingleNodeReader::getInstance()
{
    if (!_instanceSingleNodeReader)
    {
        _instanceSingleNodeReader = new SingleNodeReader();
    }

    return _instanceSingleNodeReader;
}

void SingleNodeReader::destroyInstance()
{
    AX_SAFE_DELETE(_instanceSingleNodeReader);
}

Offset<Table> SingleNodeReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                             flatbuffers::FlatBufferBuilder* builder)
{
    auto temp        = NodeReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto nodeOptions = *(Offset<WidgetOptions>*)(&temp);

    auto options = CreateSingleNodeOptions(*builder, nodeOptions);

    return *(Offset<Table>*)(&options);
}

void SingleNodeReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* singleNodeOptions)
{
    auto options = (SingleNodeOptions*)(singleNodeOptions);

    auto nodeReader = NodeReader::getInstance();
    nodeReader->setPropsWithFlatBuffers(node, (Table*)(options->nodeOptions()));
}

Node* SingleNodeReader::createNodeWithFlatBuffers(const flatbuffers::Table* singleNodeOptions)
{
    Node* node = Node::create();

    setPropsWithFlatBuffers(node, singleNodeOptions);

    return node;
}
}  // namespace ax::ext
