/****************************************************************************
Copyright (c) 2015-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "sceneio/WidgetReader/SkeletonReader/BoneNodeReader.h"

namespace ax::ext
{
class SkeletonNodeReader : public BoneNodeReader
{
    DECLARE_CLASS_NODE_READER_INFO

public:
    SkeletonNodeReader();
    ~SkeletonNodeReader();

    static SkeletonNodeReader* getInstance();
    static void destroyInstance();

    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* boneOptions) override;
};
}  // namespace ax::ext
