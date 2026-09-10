/****************************************************************************
Copyright (c) 2014 cocos2d-x.org
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "sceneext/WidgetReader/NodeReaderProtocol.h"
#include "sceneext/WidgetReader/NodeReaderDefine.h"

/****************************************
* reader of CSArmatureNode
* @param register reader before use it:
CSLoader* instance = CSLoader::getInstance();
instance->registReaderObject("ArmatureNodeReader", (ObjectFactory::Instance)CustomSpriteReader::getInstance);
*@param Reader's name must be CLASSNAME#Reader, CLASSNAME is the dataobject's prefix: CLASSNAME#ObjectData
*
*
*****************************************/
namespace ax::ext
{
class ArmatureNodeReader : public ax::Object, public ax::ext::NodeReaderProtocol
{
    DECLARE_CLASS_NODE_READER_INFO

public:
    ArmatureNodeReader();
    ~ArmatureNodeReader();

    static ArmatureNodeReader* getInstance();
    static void destroyInstance();

    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(
        pugi::xml_node objectData,
        flatbuffers::FlatBufferBuilder* builder) override;
    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* textBMFontOptions) override;

    // CSArmatureNode
    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions) override;

private:
    std::string getArmatureName(std::string_view exporJsonPath);
};
}  // namespace ax::ext
