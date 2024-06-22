/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org

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

#include "ComAudioReader.h"

#include "cocostudio/ComAudio.h"
#include "cocostudio/CSParseBinary_generated.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"

#include "flatbuffers/flatbuffers.h"

USING_NS_AX;
using namespace flatbuffers;

namespace cocostudio
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

void ComAudioReader::purge()
{
    AX_SAFE_DELETE(_instanceComAudioReader);
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
        std::string_view attriname = attribute.name();
        std::string_view value     = attribute.value();

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
        std::string_view attriname = child.name();

        if (attriname == "FileData")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                attriname         = attribute.name();
                std::string_view value = attribute.value();

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
}  // namespace cocostudio
