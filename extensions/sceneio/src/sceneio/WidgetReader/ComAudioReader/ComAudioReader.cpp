/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "sceneio/WidgetReader/ComAudioReader/ComAudioReader.h"

#include "sceneext/ComAudio.h"
#include "sceneio/CSParseBinary_generated.h"
#include "sceneio/WidgetReader/NodeReader/NodeReader.h"

#include "flatbuffers/flatbuffers.h"

using namespace flatbuffers;

namespace ax::ext
{
ComAudioReader::ComAudioReader() {}

ComAudioReader::~ComAudioReader() {}

static ComAudioReader* _instanceComAudioReader = nullptr;

ComAudioReader* ComAudioReader::getInstance()
{
    if (!_instanceComAudioReader)
    {
        _instanceComAudioReader = new ComAudioReader();
    }

    return _instanceComAudioReader;
}

void ComAudioReader::destroyInstance()
{
    AX_SAFE_DELETE(_instanceComAudioReader);
}

Offset<Table> ComAudioReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                           flatbuffers::FlatBufferBuilder* builder)
{
    auto temp        = NodeReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto nodeOptions = *(Offset<WidgetOptions>*)(&temp);

    std::string name;
    bool enabled = false;
    bool loop    = false;
    float volume = 0;

    std::string path;
    std::string plist;
    int resourceType = 0;

    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        auto attriname = attribute.name();
        auto value     = attribute.value();

        if (attriname == "Loop")
        {
            loop = (value == "True") ? true : false;
        }
        else if (attriname == "Volume")
        {
            volume = atof(value.data());
        }
        else if (attriname == "Name")
        {
            name = value;
        }

        attribute = attribute.next_attribute();
    }

    // FileData
    auto child = objectData.first_child();
    while (child)
    {
        auto attriname = child.name();

        if (attriname == "FileData")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                attriname  = attribute.name();
                auto value = attribute.value();

                if (attriname == "Path")
                {
                    path = value;
                }
                else if (attriname == "Type")
                {
                    resourceType = 0;
                }
                else if (attriname == "Plist")
                {
                    plist = value;
                }

                attribute = attribute.next_attribute();
            }
        }

        child = child.next_sibling();
    }

    auto options = CreateComAudioOptions(
        *builder, nodeOptions, builder->CreateString(name), enabled, loop, volume,
        CreateResourceData(*builder, builder->CreateString(path), builder->CreateString(plist), resourceType));

    return *(Offset<Table>*)(&options);
}

void ComAudioReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* comAudioOptions)
{
    auto options = (ComAudioOptions*)comAudioOptions;

    auto nodeReader = NodeReader::getInstance();
    nodeReader->setPropsWithFlatBuffers(node, (Table*)(options->nodeOptions()));
}

Component* ComAudioReader::createComAudioWithFlatBuffers(const flatbuffers::Table* comAudioOptions)
{
    auto options = (ComAudioOptions*)comAudioOptions;

    Component* component = ComAudio::create();
    ComAudio* audio      = static_cast<ComAudio*>(component);

    auto fileNameData = options->fileNameData();

    int resourceType = fileNameData->resourceType();
    switch (resourceType)
    {
    case 0:
    {
        std::string path = fileNameData->path()->c_str();
        audio->setFile(path.c_str());
        break;
    }

    default:
        break;
    }

    bool loop = options->loop() != 0;
    audio->setLoop(loop);

    audio->setName(options->name()->c_str());

    return component;
}

Node* ComAudioReader::createNodeWithFlatBuffers(const flatbuffers::Table* /*nodeOptions*/)
{
    return nullptr;
}
}  // namespace ax::ext
