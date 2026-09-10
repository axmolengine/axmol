/****************************************************************************
Copyright (c) 2015-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#include "flatbuffers/flatbuffers.h"
#include "sceneio/WidgetReader/SkeletonReader/CSBoneBinary_generated.h"
#include "sceneio/WidgetReader/NodeReader/NodeReader.h"
#include "sceneio/CSParseBinary_generated.h"
#include "sceneio/WidgetReader/SkeletonReader/SkeletonNodeReader.h"
#include "sceneext/ActionTimeline/SkeletonNode.h"

USING_NS_TIMELINE;

using namespace flatbuffers;

namespace ax::ext
{

IMPLEMENT_CLASS_NODE_READER_INFO(SkeletonNodeReader)

SkeletonNodeReader::SkeletonNodeReader() {}

SkeletonNodeReader::~SkeletonNodeReader() {}

static SkeletonNodeReader* _instanceSkeletonNodeReader = nullptr;

SkeletonNodeReader* SkeletonNodeReader::getInstance()
{
    if (_instanceSkeletonNodeReader == nullptr)
    {
        _instanceSkeletonNodeReader = new SkeletonNodeReader();
    }
    return _instanceSkeletonNodeReader;
}

void SkeletonNodeReader::destroyInstance()
{
    AX_SAFE_DELETE(_instanceSkeletonNodeReader);
}

ax::Node* SkeletonNodeReader::createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions)
{
    auto bone = SkeletonNode::create();

    // self options
    auto options = (flatbuffers::BoneOptions*)nodeOptions;
    setPropsWithFlatBuffers(bone, (Table*)options);

    // super options (node)
    auto nodeReader = NodeReader::getInstance();
    nodeReader->setPropsWithFlatBuffers(bone, (Table*)options->nodeOptions());

    return bone;
}
}  // namespace ax::ext
