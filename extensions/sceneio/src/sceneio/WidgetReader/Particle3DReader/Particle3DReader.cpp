/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/platform/FileUtils.h"
#include "sceneio/WidgetReader/Particle3DReader/Particle3DReader.h"

#include "sceneio/CSParseBinary_generated.h"
#include "sceneio/CSParse3DBinary_generated.h"
#include "sceneio/FlatBuffersSerialize.h"
#include "sceneio/WidgetReader/Node3DReader/Node3DReader.h"

#include "flatbuffers/flatbuffers.h"

#include "Particle3D/PU/PUParticleSystem3D.h"

using namespace flatbuffers;

namespace ax::ext
{
IMPLEMENT_CLASS_NODE_READER_INFO(Particle3DReader)

Particle3DReader::Particle3DReader() {}

Particle3DReader::~Particle3DReader() {}

static Particle3DReader* _instanceParticle3DReader = nullptr;

Particle3DReader* Particle3DReader::getInstance()
{
    if (!_instanceParticle3DReader)
    {
        _instanceParticle3DReader = new Particle3DReader();
    }

    return _instanceParticle3DReader;
}

void Particle3DReader::destroyInstance()
{
    AX_SAFE_DELETE(_instanceParticle3DReader);
}

Offset<Table> Particle3DReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                             flatbuffers::FlatBufferBuilder* builder)
{
    auto temp          = Node3DReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto node3DOptions = *(Offset<Node3DOption>*)(&temp);

    std::string path;
    int resourceType = 0;

    // FileData
    auto child = objectData.first_child();
    while (child)
    {
        auto name = child.name();

        if (name == "FileData")
        {
            auto attribute = child.first_attribute();

            while (attribute)
            {
                name       = attribute.name();
                auto value = attribute.value();

                if (name == "Path")
                {
                    path = value;
                }
                else if (name == "Type")
                {
                    if (value == "Normal")
                    {
                        resourceType = 1;
                    }
                    else if (value == "Default")
                    {
                        resourceType = 0;
                    }
                }

                attribute = attribute.next_attribute();
            }
        }

        child = child.next_sibling();
    }

    auto options = CreateParticle3DOptions(
        *builder, node3DOptions,
        CreateResourceData(*builder, builder->CreateString(path), builder->CreateString(""), resourceType));

    return *(Offset<Table>*)(&options);
}

void Particle3DReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* particle3DOptions)
{
    auto options = (Particle3DOptions*)particle3DOptions;

    auto node3DReader = Node3DReader::getInstance();
    node3DReader->setPropsWithFlatBuffers(node, (Table*)(options->node3DOption()));
}

Node* Particle3DReader::createNodeWithFlatBuffers(const flatbuffers::Table* particle3DOptions)
{
    auto options = (Particle3DOptions*)particle3DOptions;

    auto fileData    = options->fileData();
    std::string path = fileData->path()->c_str();

    PUParticleSystem3D* ret = PUParticleSystem3D::create();
    if (FileUtils::getInstance()->isFileExist(path))
    {
        ret->initWithFilePath(path);
    }

    setPropsWithFlatBuffers(ret, particle3DOptions);

    if (ret)
    {
        ret->startParticleSystem();
    }

    return ret;
}
}  // namespace ax::ext
