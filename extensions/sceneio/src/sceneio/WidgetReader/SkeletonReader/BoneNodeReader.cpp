/****************************************************************************
Copyright (c) 2015-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#include "flatbuffers/flatbuffers.h"
#include "sceneio/WidgetReader/NodeReader/NodeReader.h"
#include "sceneio/CSParseBinary_generated.h"
#include "sceneio/WidgetReader/SkeletonReader/CSBoneBinary_generated.h"
#include "sceneio/WidgetReader/SkeletonReader/BoneNodeReader.h"
#include "sceneext/ActionTimeline/BoneNode.h"
#include "axmol/base/Utils.h"

USING_NS_TIMELINE;

using namespace flatbuffers;

namespace ax::ext
{

IMPLEMENT_CLASS_NODE_READER_INFO(BoneNodeReader)

BoneNodeReader::BoneNodeReader() {}

BoneNodeReader::~BoneNodeReader() {}

static BoneNodeReader* _instanceBoneNodeReader = nullptr;

BoneNodeReader* BoneNodeReader::getInstance()
{
    if (_instanceBoneNodeReader == nullptr)
    {
        _instanceBoneNodeReader = new BoneNodeReader();
    }
    return _instanceBoneNodeReader;
}

void BoneNodeReader::destroyInstance()
{
    AX_SAFE_DELETE(_instanceBoneNodeReader);
}

Offset<Table> BoneNodeReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                           flatbuffers::FlatBufferBuilder* builder)
{

    auto temp        = NodeReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto nodeOptions = *(Offset<WidgetOptions>*)(&temp);

    float length            = 0;
    ax::BlendFunc blendFunc = ax::BlendFunc::ALPHA_PREMULTIPLIED;

    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        auto name  = attribute.name();
        auto value = attribute.value();

        if (name == "Length")
        {
            length = atof(value.data());
        }
        attribute = attribute.next_attribute();
    }

    auto child = objectData.first_child();
    while (child)
    {
        auto name = child.name();
        if (name == "BlendFunc")
        {
            pugi::xml_attribute battribute = child.first_attribute();

            while (battribute)
            {
                name       = battribute.name();
                auto value = battribute.value();

                if (name == "Src")
                {
                    blendFunc.src = utils::toBackendBlendFactor(atoi(value.data()));
                }
                else if (name == "Dst")
                {
                    blendFunc.dst = utils::toBackendBlendFactor(atoi(value.data()));
                }

                battribute = battribute.next_attribute();
            }
        }

        child = child.next_sibling();
    }
    flatbuffers::BlendFunc f_blendFunc(utils::toGLBlendFactor(blendFunc.src), utils::toGLBlendFactor(blendFunc.dst));

    auto options = CreateBoneOptions(*builder, nodeOptions, length, &f_blendFunc);

    return *(Offset<Table>*)(&options);
}

void BoneNodeReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* nodeOptions)
{

    auto* bone   = static_cast<BoneNode*>(node);
    auto options = (flatbuffers::BoneOptions*)nodeOptions;

    float length = options->length();
    bone->setDebugDrawLength(length);

    auto f_blendFunc = options->blendFunc();
    if (f_blendFunc)
    {
        ax::BlendFunc blendFunc = ax::BlendFunc::ALPHA_PREMULTIPLIED;
        blendFunc.src           = utils::toBackendBlendFactor(f_blendFunc->src());
        blendFunc.dst           = utils::toBackendBlendFactor(f_blendFunc->dst());
        bone->setBlendFunc(blendFunc);
    }
}

ax::Node* BoneNodeReader::createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions)
{
    auto bone = BoneNode::create();

    // self options
    auto options = (flatbuffers::BoneOptions*)nodeOptions;
    setPropsWithFlatBuffers(bone, (Table*)options);

    // super options (node)
    auto nodeReader = NodeReader::getInstance();
    nodeReader->setPropsWithFlatBuffers(bone, (Table*)options->nodeOptions());

    return bone;
}

}  // namespace ax::ext
