/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#include "sceneio/WidgetReader/ParticleReader/ParticleReader.h"

#include "axmol/base/Types.h"
#include "axmol/base/Utils.h"
#include "axmol/2d/ParticleSystemQuad.h"
#include "axmol/platform/FileUtils.h"
#include "sceneio/CSParseBinary_generated.h"
#include "sceneio/WidgetReader/NodeReader/NodeReader.h"

#include "flatbuffers/flatbuffers.h"

using namespace flatbuffers;

namespace ax::ext
{
IMPLEMENT_CLASS_NODE_READER_INFO(ParticleReader)

ParticleReader::ParticleReader() {}

ParticleReader::~ParticleReader() {}

static ParticleReader* _instanceParticleReader = nullptr;

ParticleReader* ParticleReader::getInstance()
{
    if (!_instanceParticleReader)
    {
        _instanceParticleReader = new ParticleReader();
    }

    return _instanceParticleReader;
}

void ParticleReader::destroyInstance()
{
    AX_SAFE_DELETE(_instanceParticleReader);
}

Offset<Table> ParticleReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                           flatbuffers::FlatBufferBuilder* builder)
{
    auto temp        = NodeReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto nodeOptions = *(Offset<WidgetOptions>*)(&temp);

    std::string path;
    std::string plistFile;
    int resourceType = 0;

    ax::BlendFunc blendFunc = ax::BlendFunc::ALPHA_PREMULTIPLIED;

    // child elements
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
                    resourceType = 0;
                }
                else if (name == "Plist")
                {
                    plistFile = value;
                }

                attribute = attribute.next_attribute();
            }
        }
        else if (name == "BlendFunc")
        {
            auto attribute = child.first_attribute();

            while (attribute)
            {
                name       = attribute.name();
                auto value = attribute.value();

                if (name == "Src")
                {
                    blendFunc.src = utils::toBackendBlendFactor(atoi(value.data()));
                }
                else if (name == "Dst")
                {
                    blendFunc.dst = utils::toBackendBlendFactor(atoi(value.data()));
                }

                attribute = attribute.next_attribute();
            }
        }

        child = child.next_sibling();
    }

    flatbuffers::BlendFunc f_blendFunc(utils::toGLBlendFactor(blendFunc.src), utils::toGLBlendFactor(blendFunc.dst));

    auto options = CreateParticleSystemOptions(
        *builder, nodeOptions,
        CreateResourceData(*builder, builder->CreateString(path), builder->CreateString(plistFile), resourceType),
        &f_blendFunc);

    return *(Offset<Table>*)(&options);
}

void ParticleReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* particleOptions)
{
    auto particle = dynamic_cast<ParticleSystemQuad*>(node);
    auto options  = (ParticleSystemOptions*)particleOptions;

    auto f_blendFunc = options->blendFunc();
    if (particle && f_blendFunc)
    {
        ax::BlendFunc blendFunc = ax::BlendFunc::ALPHA_PREMULTIPLIED;
        blendFunc.src           = utils::toBackendBlendFactor(f_blendFunc->src());
        blendFunc.dst           = utils::toBackendBlendFactor(f_blendFunc->dst());
        particle->setBlendFunc(blendFunc);
    }

    auto nodeReader = NodeReader::getInstance();
    nodeReader->setPropsWithFlatBuffers(node, (Table*)options->nodeOptions());
}

Node* ParticleReader::createNodeWithFlatBuffers(const flatbuffers::Table* particleOptions)
{
    ParticleSystemQuad* particle = nullptr;

    auto options         = (ParticleSystemOptions*)particleOptions;
    auto fileNameDataDic = (options->fileNameData());

    bool fileExist = false;
    std::string errorFilePath;
    std::string path = fileNameDataDic->path()->c_str();
    int resourceType = fileNameDataDic->resourceType();
    switch (resourceType)
    {
    case 0:
    {
        if (FileUtils::getInstance()->isFileExist(path))
        {
            fileExist = true;
        }
        else
        {
            errorFilePath = path;
            fileExist     = false;
        }
        break;
    }

    default:
        break;
    }
    if (fileExist)
    {
        particle = ParticleSystemQuad::create(path);
        if (particle)
        {
            setPropsWithFlatBuffers(particle, (Table*)particleOptions);
            particle->setPositionType(ParticleSystem::PositionType::GROUPED);
        }
    }
    else
    {
        Node* node = Node::create();
        setPropsWithFlatBuffers(node, (Table*)particleOptions);
        return node;
    }

    return particle;
}

}  // namespace ax::ext
