/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) simdsoft.com, @HALX99.

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

#include "cocostudio/WidgetReader/SpriteReader/SpriteReader.h"

#include "base/Utils.h"
#include "2d/Sprite.h"
#include "2d/SpriteFrameCache.h"
#include "platform/FileUtils.h"

#include "cocostudio/CSParseBinary_generated.h"
#include "cocostudio/FlatBuffersSerialize.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"

#include "flatbuffers/flatbuffers.h"

#include "renderer/backend/ProgramManager.h"
#include "renderer/backend/ProgramState.h"

USING_NS_AX;
using namespace flatbuffers;

namespace cocostudio
{
IMPLEMENT_CLASS_NODE_READER_INFO(SpriteReader)

SpriteReader::SpriteReader() {}

SpriteReader::~SpriteReader() {}

static SpriteReader* _instanceSpriteReader = nullptr;

SpriteReader* SpriteReader::getInstance()
{
    if (!_instanceSpriteReader)
    {
        _instanceSpriteReader = new SpriteReader();
    }

    return _instanceSpriteReader;
}

void SpriteReader::purge()
{
    AX_SAFE_DELETE(_instanceSpriteReader);
}

void SpriteReader::destroyInstance()
{
    AX_SAFE_DELETE(_instanceSpriteReader);
}

Offset<Table> SpriteReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                         flatbuffers::FlatBufferBuilder* builder)
{
    auto temp        = NodeReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto nodeOptions = *(Offset<WidgetOptions>*)(&temp);

    std::string path;
    std::string plistFile;
    int resourceType           = 0;
    bool intelliShadingEnabled = false;
    ax::Vec3 hsv;
    ax::Vec3 filter;

    ax::BlendFunc blendFunc = ax::BlendFunc::ALPHA_PREMULTIPLIED;

    // attributes
    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view attriname = attribute.name();
        std::string_view value     = attribute.value();

        if (attriname == "IntelliShadingEnabled")
        {
            intelliShadingEnabled = (value == "True") ? true : false;
            break;
        }

        attribute = attribute.next_attribute();
    }

    // FileData
    auto child = objectData.first_child();
    while (child)
    {
        std::string_view name = child.name();
        pugi::xml_attribute attribute;
        if (name == "FileData")
        {
            std::string texture;
            std::string texturePng;

            attribute = child.first_attribute();

            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "Path")
                {
                    path = value;
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    plistFile = value;
                    texture   = value;
                }

                attribute = attribute.next_attribute();
            }

            if (resourceType == 1)
            {
                FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                fbs->_textures.emplace_back(builder->CreateString(texture));
            }
        }
        else if (name == "BlendFunc")
        {
            attribute = child.first_attribute();
            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

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
        else if (name == "HSV")
        {
            attribute = child.first_attribute();
            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "X")
                {
                    hsv.x = atof(value.data());
                }
                else if (name == "Y")
                {
                    hsv.y = atof(value.data());
                }
                else if (name == "Z")
                {
                    hsv.z = atof(value.data());
                }

                attribute = attribute.next_attribute();
            }
        }
        else if (name == "FilterRGB")
        {
            attribute = child.first_attribute();
            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "X")
                {
                    filter.x = atof(value.data());
                }
                else if (name == "Y")
                {
                    filter.y = atof(value.data());
                }
                else if (name == "Z")
                {
                    filter.z = atof(value.data());
                }

                attribute = attribute.next_attribute();
            }
        }

        child = child.next_sibling();
    }

    flatbuffers::BlendFunc f_blendFunc(utils::toGLBlendFactor(blendFunc.src), utils::toGLBlendFactor(blendFunc.dst));

    auto options = CreateSpriteOptions(
        *builder, nodeOptions,
        CreateResourceData(*builder, builder->CreateString(path), builder->CreateString(plistFile), resourceType),
        &f_blendFunc, intelliShadingEnabled, (const FVec3*)&hsv, (const FVec3*)&filter);

    return *(Offset<Table>*)(&options);
}

void SpriteReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* spriteOptions)
{
    Sprite* sprite = static_cast<Sprite*>(node);
    auto options   = (SpriteOptions*)spriteOptions;

    auto nodeReader = NodeReader::getInstance();
    nodeReader->setPropsWithFlatBuffers(node, (Table*)(options->nodeOptions()));

    auto fileNameDataDic = (options->fileNameData());

    int resourceType = fileNameDataDic->resourceType();
    std::string path = fileNameDataDic->path()->c_str();

    std::string errorFilePath;

    switch (resourceType)
    {
    case 0:
    {
        if (FileUtils::getInstance()->isFileExist(path))
        {
            sprite->setTexture(path);
        }
        else
        {
            errorFilePath = path;
        }
        break;
    }

    case 1:
    {
        std::string plist        = fileNameDataDic->plistFile()->c_str();
        SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->findFrame(path);
        if (spriteFrame)
        {
            sprite->setSpriteFrame(spriteFrame);
        }
        else
        {
            if (FileUtils::getInstance()->isFileExist(plist))
            {
                ValueMap value              = FileUtils::getInstance()->getValueMapFromFile(plist);
                ValueMap metadata           = value["metadata"].asValueMap();
                std::string textureFileName = metadata["textureFileName"].asString();
                if (!FileUtils::getInstance()->isFileExist(textureFileName))
                {
                    errorFilePath = textureFileName;
                }
            }
            else
            {
                errorFilePath = plist;
            }
        }
        break;
    }

    default:
        break;
    }

    auto f_blendFunc = options->blendFunc();
    if (f_blendFunc)
    {
        ax::BlendFunc blendFunc = ax::BlendFunc::ALPHA_PREMULTIPLIED;
        blendFunc.src                = utils::toBackendBlendFactor(f_blendFunc->src());
        blendFunc.dst                = utils::toBackendBlendFactor(f_blendFunc->dst());
        sprite->setBlendFunc(blendFunc);
    }

    auto nodeOptions = options->nodeOptions();

    uint8_t alpha = (uint8_t)nodeOptions->color()->a();
    uint8_t red   = (uint8_t)nodeOptions->color()->r();
    uint8_t green = (uint8_t)nodeOptions->color()->g();
    uint8_t blue  = (uint8_t)nodeOptions->color()->b();

    if (alpha != 255)
    {
        sprite->setOpacity(alpha);
    }
    if (red != 255 || green != 255 || blue != 255)
    {
        sprite->setColor(Color3B(red, green, blue));
    }

    bool flipX = nodeOptions->flipX() != 0;
    bool flipY = nodeOptions->flipY() != 0;

    if (flipX != false)
        sprite->setFlippedX(flipX);
    if (flipY != false)
        sprite->setFlippedY(flipY);

    if (options->intelliShadingEnabled())
    {
        auto hsv    = options->hsv();
        auto filter = options->filter();
        if (hsv != nullptr && filter != nullptr)
        {
            auto prog = ProgramManager::getInstance()->getBuiltinProgram(ProgramType::HSV);
            auto ps   = new backend::ProgramState(prog);
            sprite->setProgramState(ps, true);
            Vec3 axhsv{hsv->x(), hsv->y(), hsv->z()};
            ps->setUniform(ps->getUniformLocation("u_hsv"), &axhsv, sizeof(axhsv));
            ps->updateBatchId();
        }
    }
}

Node* SpriteReader::createNodeWithFlatBuffers(const flatbuffers::Table* spriteOptions)
{
    Sprite* sprite = Sprite::create();

    setPropsWithFlatBuffers(sprite, (Table*)spriteOptions);

    return sprite;
}

int SpriteReader::getResourceType(std::string_view key)
{
    if (key == "Normal" || key == "Default")
    {
        return 0;
    }

    FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
    if (fbs->_isSimulator)
    {
        if (key == "MarkedSubImage")
        {
            return 0;
        }
    }
    return 1;
}
}  // namespace cocostudio
