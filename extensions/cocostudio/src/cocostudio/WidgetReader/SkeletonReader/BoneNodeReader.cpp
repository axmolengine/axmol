/****************************************************************************
Copyright (c) 2015-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

https://axmol.dev/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "flatbuffers/flatbuffers.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"
#include "cocostudio/CSParseBinary_generated.h"
#include "cocostudio/WidgetReader/SkeletonReader/CSBoneBinary_generated.h"
#include "cocostudio/WidgetReader/SkeletonReader/BoneNodeReader.h"
#include "cocostudio/ActionTimeline/BoneNode.h"
#include "base/Utils.h"

USING_NS_AX;
USING_NS_TIMELINE;
using namespace cocostudio;
using namespace flatbuffers;

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

    float length                 = 0;
    ax::BlendFunc blendFunc = ax::BlendFunc::ALPHA_PREMULTIPLIED;

    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view name  = attribute.name();
        std::string_view value = attribute.value();

        if (name == "Length")
        {
            length = atof(value.data());
        }
        attribute = attribute.next_attribute();
    }

    auto child = objectData.first_child();
    while (child)
    {
        std::string_view name = child.name();
        if (name == "BlendFunc")
        {
            pugi::xml_attribute battribute = child.first_attribute();

            while (battribute)
            {
                name              = battribute.name();
                std::string_view value = battribute.value();

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
        blendFunc.src                = utils::toBackendBlendFactor(f_blendFunc->src());
        blendFunc.dst                = utils::toBackendBlendFactor(f_blendFunc->dst());
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
