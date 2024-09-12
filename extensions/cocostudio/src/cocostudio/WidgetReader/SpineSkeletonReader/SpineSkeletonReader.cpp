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

#include "SpineSkeletonReader.h"

#if defined(AX_ENABLE_EXT_SPINE)
#    include "cocostudio/SpineSkeletonDataCache.h"

#    include "2d/Sprite.h"
#    include "2d/SpriteFrameCache.h"
#    include "platform/FileUtils.h"

#    include "cocostudio/CSParseBinary_generated.h"
#    include "cocostudio/FlatBuffersSerialize.h"
#    include "cocostudio/WidgetReader/NodeReader/NodeReader.h"

#    include "flatbuffers/flatbuffers.h"

using namespace ax;
using namespace flatbuffers;

namespace cocostudio
{
IMPLEMENT_CLASS_NODE_READER_INFO(SpineSkeletonReader)

SpineSkeletonReader::SpineSkeletonReader() {}

SpineSkeletonReader::~SpineSkeletonReader() {}

static SpineSkeletonReader* _instanceSpriteReader = nullptr;

SpineSkeletonReader* SpineSkeletonReader::getInstance()
{
    if (!_instanceSpriteReader)
    {
        _instanceSpriteReader = new SpineSkeletonReader();
    }

    return _instanceSpriteReader;
}

void SpineSkeletonReader::destroyInstance()
{
    AX_SAFE_DELETE(_instanceSpriteReader);
}

Offset<Table> SpineSkeletonReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                                flatbuffers::FlatBufferBuilder* builder)
{
    auto temp        = NodeReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto nodeOptions = *(Offset<WidgetOptions>*)(&temp);

    std::string dataFile;
    std::string atlasFile;
    std::string animation;
    std::string skin = "default";
    bool loop        = false;
    float timeScale  = 1.0f;

    ax::BlendFunc blendFunc = ax::BlendFunc::ALPHA_PREMULTIPLIED;

    // attributes
    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view name  = attribute.name();
        std::string_view value = attribute.value();

        if (name == "DataFile")
        {
            dataFile = std::move(value);
        }
        else if (name == "AtlasFile")
        {
            atlasFile = std::move(value);
        }
        else if (name == "Animation")
        {
            animation = std::move(value);
        }
        else if (name == "Skin")
        {
            skin = std::move(value);
        }
        else if (name == "Loop")
        {
            loop = (value == "True" ? true : false);
        }
        else if (name == "TimeScale")
        {
            timeScale = atof(value.data());
        }

        attribute = attribute.next_attribute();
    }
#    if 0
        // FileData
        auto child = objectData.first_child();
        while (child)
        {
            std::string_view name = child.name();

            if (name == "DataFile")
            {
                std::string texture;
                std::string texturePng;

                auto attribute =  child.first_attribute();

                while (attribute)
                {
                    name = attribute.name();
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
                        texture = value;
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
                auto attribute =  child.first_attribute();

                while (attribute)
                {
                    name = attribute.name();
                    std::string_view value = attribute.value();

                    if (name == "Src")
                    {
                        blendFunc.src = atoi(value.data());
                    }
                    else if (name == "Dst")
                    {
                        blendFunc.dst = atoi(value.data());
                    }

                    attribute = attribute.next_attribute();
                }
            }

            child = child.next_sibling();
        }
#    endif

    flatbuffers::BlendFunc f_blendFunc(utils::toGLBlendFactor(blendFunc.src), utils::toGLBlendFactor(blendFunc.dst));

    auto options = CreateSpineSkeletonOptions(*builder, nodeOptions, builder->CreateString(dataFile),
                                              builder->CreateString(atlasFile), builder->CreateString(animation),
                                              builder->CreateString(skin), loop, timeScale);

    return *(Offset<Table>*)(&options);
}

void SpineSkeletonReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* spriteOptions)
{
    Node** ppResult = (Node**)(node);

    auto options = (SpineSkeletonOptions*)spriteOptions;

    auto sharedData = SpineSkeletonDataCache::getInstance()->addData(options->dataFile()->c_str(),
                                                                     options->atlasFile()->c_str(), 1.0f /*TODO:*/);
    auto implNode   = spine::SkeletonAnimation::createWithData(sharedData->data);
    implNode->setAnimation(0, options->animation()->c_str(), options->loop());
    implNode->setSkin(options->animation()->c_str());

    *ppResult = implNode;

    auto nodeReader = NodeReader::getInstance();
    nodeReader->setPropsWithFlatBuffers(implNode, (Table*)(options->nodeOptions()));

    // set Node Options
    auto nodeOptions = options->nodeOptions();

    uint8_t alpha = (uint8_t)nodeOptions->color()->a();
    uint8_t red   = (uint8_t)nodeOptions->color()->r();
    uint8_t green = (uint8_t)nodeOptions->color()->g();
    uint8_t blue  = (uint8_t)nodeOptions->color()->b();

    implNode->setScaleX(nodeOptions->scale()->x());
    implNode->setScaleY(nodeOptions->scale()->y());

    implNode->setColor(Color3B(red, green, blue));
    implNode->setOpacity(alpha);

    /*bool flipX = nodeOptions->flipX() != 0;
    bool flipY = nodeOptions->flipY() != 0;

    if (flipX != false)
        sprite->setFlippedX(flipX);
    if (flipY != false)
        sprite->setFlippedY(flipY);*/
}

Node* SpineSkeletonReader::createNodeWithFlatBuffers(const flatbuffers::Table* spriteOptions)
{
    Node* node = nullptr;
    // #Hack
    setPropsWithFlatBuffers((Node*)(&node), (Table*)spriteOptions);

    return node;
}

int SpineSkeletonReader::getResourceType(std::string_view key)
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

#endif
