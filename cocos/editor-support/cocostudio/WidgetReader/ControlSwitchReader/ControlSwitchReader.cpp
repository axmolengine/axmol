/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org

 http://www.cocos2d-x.org

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

#include "editor-support/cocostudio/WidgetReader/ControlSwitchReader/ControlSwitchReader.h"

#include "2d/CCSprite.h"
#include "2d/CCSpriteFrameCache.h"
#include "platform/CCFileUtils.h"

#include "editor-support/cocostudio/CSParseBinary_generated.h"
#include "editor-support/cocostudio/FlatBuffersSerialize.h"
#include "editor-support/cocostudio/WidgetReader/NodeReader/NodeReader.h"

#include "extensions/cocos-ext.h"


#include "flatbuffers/flatbuffers.h"

USING_NS_CC;
using namespace flatbuffers;

namespace cocostudio
{
    static Sprite* createSprite(const cocos2d::ResourceData& fileData)
    {
        return fileData.type == 0 ? Sprite::create(fileData.file) : Sprite::createWithSpriteFrameName(fileData.file);
    }

    IMPLEMENT_CLASS_NODE_READER_INFO(ControlSwitchReader)

        ControlSwitchReader::ControlSwitchReader()
    {

    }

    ControlSwitchReader::~ControlSwitchReader()
    {

    }

    static ControlSwitchReader* _instanceSpriteReader = nullptr;

    ControlSwitchReader* ControlSwitchReader::getInstance()
    {
        if (!_instanceSpriteReader)
        {
            _instanceSpriteReader = new (std::nothrow) ControlSwitchReader();
        }

        return _instanceSpriteReader;
    }

    void ControlSwitchReader::purge()
    {
        CC_SAFE_DELETE(_instanceSpriteReader);
    }

    void ControlSwitchReader::destroyInstance()
    {
        CC_SAFE_DELETE(_instanceSpriteReader);
    }

    Offset<Table> ControlSwitchReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
        flatbuffers::FlatBufferBuilder *builder)
    {
        auto temp = NodeReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
        auto nodeOptions = *(Offset<WidgetOptions>*)(&temp);

        bool on = false;
        bool enabled = true;

        int maskImageResourceType = 0;
        std::string maskImage = "";
        std::string maskImagePlistFile = "";

        int onImageResourceType = 0;
        std::string onImage = "";
        std::string onImagePlistFile = "";

        int offImageResourceType = 0;
        std::string offImage = "";
        std::string offImagePlistFile = "";

        int thumbImageResourceType = 0;
        std::string thumbImage = "";
        std::string thumbImagePlistFile = "";

        cocos2d::BlendFunc blendFunc = cocos2d::BlendFunc::ALPHA_PREMULTIPLIED;

        // attributes
        auto attribute =  objectData.first_attribute();
        while (attribute)
        {
            std::string name = attribute.name();
            std::string value = attribute.value();

            if (name == "On")
            {
                on = value == "True" ? true : false;
            }
            
            attribute = attribute.next_attribute();
        }

        //// child elements
        // child elements
        auto child = objectData.first_child();
        while (child)
        {
            std::string name = child.name();

            if (name == "MaskImageData")
            {
                std::string texture = "";
                std::string texturePng = "";

                attribute = child.first_attribute();

                while (attribute)
                {
                    name = attribute.name();
                    std::string value = attribute.value();

                    if (name == "Path")
                    {
                        maskImage = value;
                    }
                    else if (name == "Type")
                    {
                        maskImageResourceType = getResourceType(value);
                    }
                    else if (name == "Plist")
                    {
                        maskImagePlistFile = value;
                        texture = value;
                    }

                    attribute = attribute.next_attribute();
                }

                if (maskImageResourceType == 1)
                {
                    FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                    fbs->_textures.push_back(builder->CreateString(texture));
                }
            }
            else if (name == "OnImageData")
            {
                std::string texture = "";
                std::string texturePng = "";

                attribute = child.first_attribute();

                while (attribute)
                {
                    name = attribute.name();
                    std::string value = attribute.value();

                    if (name == "Path")
                    {
                        onImage = value;
                    }
                    else if (name == "Type")
                    {
                        onImageResourceType = getResourceType(value);
                    }
                    else if (name == "Plist")
                    {
                        onImagePlistFile = value;
                        texture = value;
                    }

                    attribute = attribute.next_attribute();
                }

                if (onImageResourceType == 1)
                {
                    FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                    fbs->_textures.push_back(builder->CreateString(texture));
                }
            }
            else if (name == "OffImageData")
            {
                std::string texture = "";
                std::string texturePng = "";

                attribute = child.first_attribute();

                while (attribute)
                {
                    name = attribute.name();
                    std::string value = attribute.value();

                    if (name == "Path")
                    {
                        offImage = value;
                    }
                    else if (name == "Type")
                    {
                        offImageResourceType = getResourceType(value);
                    }
                    else if (name == "Plist")
                    {
                        offImagePlistFile = value;
                        texture = value;
                    }

                    attribute = attribute.next_attribute();
                }

                if (offImageResourceType == 1)
                {
                    FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                    fbs->_textures.push_back(builder->CreateString(texture));
                }
            }
            else if (name == "ThumbImageData")
            {
                std::string texture = "";
                std::string texturePng = "";

                attribute = child.first_attribute();

                while (attribute)
                {
                    name = attribute.name();
                    std::string value = attribute.value();

                    if (name == "Path")
                    {
                        thumbImage = value;
                    }
                    else if (name == "Type")
                    {
                        thumbImageResourceType = getResourceType(value);
                    }
                    else if (name == "Plist")
                    {
                        thumbImagePlistFile = value;
                        texture = value;
                    }

                    attribute = attribute.next_attribute();
                }

                if (thumbImageResourceType == 1)
                {
                    FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                    fbs->_textures.push_back(builder->CreateString(texture));
                }
            }

            child = child.next_sibling();
        }

        auto options = CreateControlSwitchOptions(*builder,
            nodeOptions,
            CreateResourceData(*builder,
                builder->CreateString(maskImage),
                builder->CreateString(maskImagePlistFile),
                maskImageResourceType),
            CreateResourceData(*builder,
                builder->CreateString(onImage),
                builder->CreateString(onImagePlistFile),
                onImageResourceType),
            CreateResourceData(*builder,
                builder->CreateString(offImage),
                builder->CreateString(offImagePlistFile),
                offImageResourceType),
            CreateResourceData(*builder,
                builder->CreateString(thumbImage),
                builder->CreateString(thumbImagePlistFile),
                thumbImageResourceType),
            on,
            enabled);

            return *(Offset<Table>*)(&options);
    }

    void ControlSwitchReader::setPropsWithFlatBuffers(cocos2d::Node *node,
        const flatbuffers::Table* spriteOptions)
    {
        Node** ppResult = (Node**)(node);

        auto options = (ControlSwitchOptions*)spriteOptions;

        //load background image
        bool backGroundFileExist = false;
        std::string backGroundErrorFilePath = "";
        auto backGroundDic = cocos2d::wext::makeResourceData(options->maskImage());
        int backGroundType = backGroundDic.type;
        std::string& backGroundTexturePath = backGroundDic.file;
        //cocos2d::wext::onBeforeLoadObjectAsset(checkBox, backGroundDic, 0);
        switch (backGroundType)
        {
        case 0:
        {
            if (FileUtils::getInstance()->isFileExist(backGroundTexturePath))
            {
                backGroundFileExist = true;
            }
            else
            {
                backGroundErrorFilePath = backGroundTexturePath;
                backGroundFileExist = false;
            }
            break;
        }

        case 1:
        {
            std::string plist = backGroundDic.plist;
            SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(backGroundTexturePath);
            if (spriteFrame)
            {
                backGroundFileExist = true;
            }
            else
            {
                if (FileUtils::getInstance()->isFileExist(plist))
                {
                    ValueMap value = FileUtils::getInstance()->getValueMapFromFile(plist);
                    ValueMap metadata = value["metadata"].asValueMap();
                    std::string textureFileName = metadata["textureFileName"].asString();
                    if (!FileUtils::getInstance()->isFileExist(textureFileName))
                    {
                        backGroundErrorFilePath = textureFileName;
                    }
                }
                else
                {
                    backGroundErrorFilePath = plist;
                }
                backGroundFileExist = false;
            }
            break;
        }

        default:
            break;
        }
        /*if (backGroundFileExist)
        {
            checkBox->loadTextureBackGround(backGroundTexturePath, (Widget::TextureResType)backGroundType);
        }*/

        //load background selected image
        bool backGroundSelectedfileExist = false;
        std::string backGroundSelectedErrorFilePath = "";
        auto backGroundSelectedDic = cocos2d::wext::makeResourceData(options->onImage());
        int backGroundSelectedType = backGroundSelectedDic.type;
        std::string& backGroundSelectedTexturePath = backGroundSelectedDic.file;
        // cocos2d::wext::onBeforeLoadObjectAsset(checkBox, backGroundSelectedDic, 1);
        switch (backGroundSelectedType)
        {
        case 0:
        {
            if (FileUtils::getInstance()->isFileExist(backGroundSelectedTexturePath))
            {
                backGroundSelectedfileExist = true;
            }
            else
            {
                backGroundSelectedErrorFilePath = backGroundSelectedTexturePath;
                backGroundSelectedfileExist = false;
            }
            break;
        }

        case 1:
        {
            std::string plist = backGroundSelectedDic.plist;
            SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(backGroundSelectedTexturePath);
            if (spriteFrame)
            {
                backGroundSelectedfileExist = true;
            }
            else
            {
                if (FileUtils::getInstance()->isFileExist(plist))
                {
                    ValueMap value = FileUtils::getInstance()->getValueMapFromFile(plist);
                    ValueMap metadata = value["metadata"].asValueMap();
                    std::string textureFileName = metadata["textureFileName"].asString();
                    if (!FileUtils::getInstance()->isFileExist(textureFileName))
                    {
                        backGroundSelectedErrorFilePath = textureFileName;
                    }
                }
                else
                {
                    backGroundSelectedErrorFilePath = plist;
                }
                backGroundSelectedfileExist = false;
            }
            break;
        }

        default:
            break;
        }
        /*if (backGroundSelectedfileExist)
        {
            checkBox->loadTextureBackGroundSelected(backGroundSelectedTexturePath, (Widget::TextureResType)backGroundSelectedType);
        }*/

        //load frontCross image
        bool frontCrossFileExist = false;
        std::string frontCrossErrorFilePath = "";
        auto frontCrossDic = cocos2d::wext::makeResourceData(options->offImage());
        int frontCrossType = frontCrossDic.type;
        std::string& frontCrossFileName = frontCrossDic.file;
        //cocos2d::wext::onBeforeLoadObjectAsset(checkBox, frontCrossDic, 2);
        switch (frontCrossType)
        {
        case 0:
        {
            if (FileUtils::getInstance()->isFileExist(frontCrossFileName))
            {
                frontCrossFileExist = true;
            }
            else
            {
                frontCrossErrorFilePath = frontCrossFileName;
                frontCrossFileExist = false;
            }
            break;
        }

        case 1:
        {
            std::string plist = frontCrossDic.plist;
            SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frontCrossFileName);
            if (spriteFrame)
            {
                frontCrossFileExist = true;
            }
            else
            {
                if (FileUtils::getInstance()->isFileExist(plist))
                {
                    ValueMap value = FileUtils::getInstance()->getValueMapFromFile(plist);
                    ValueMap metadata = value["metadata"].asValueMap();
                    std::string textureFileName = metadata["textureFileName"].asString();
                    if (!FileUtils::getInstance()->isFileExist(textureFileName))
                    {
                        frontCrossErrorFilePath = textureFileName;
                    }
                }
                else
                {
                    frontCrossErrorFilePath = plist;
                }
                frontCrossFileExist = false;
            }
            break;
        }

        default:
            break;
        }
        /*if (frontCrossFileExist)
        {
            checkBox->loadTextureFrontCross(frontCrossFileName, (Widget::TextureResType)frontCrossType);
        }*/

        //load backGroundBoxDisabledData
        bool backGroundBoxDisabledFileExist = false;
        std::string backGroundBoxDisabledErrorFilePath = "";
        auto backGroundDisabledDic = cocos2d::wext::makeResourceData(options->thumbImage());
        int backGroundDisabledType = backGroundDisabledDic.type;
        std::string& backGroundDisabledFileName = backGroundDisabledDic.file;
        // cocos2d::wext::onBeforeLoadObjectAsset(checkBox, backGroundDisabledDic, 3);
        switch (backGroundDisabledType)
        {
        case 0:
        {
            if (FileUtils::getInstance()->isFileExist(backGroundDisabledFileName))
            {
                backGroundBoxDisabledFileExist = true;
            }
            else
            {
                backGroundBoxDisabledErrorFilePath = backGroundDisabledFileName;
                backGroundBoxDisabledFileExist = false;
            }
            break;
        }

        case 1:
        {
            std::string plist = backGroundDisabledDic.plist;
            SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(backGroundDisabledFileName);
            if (spriteFrame)
            {
                backGroundBoxDisabledFileExist = true;
            }
            else
            {
                if (FileUtils::getInstance()->isFileExist(plist))
                {
                    ValueMap value = FileUtils::getInstance()->getValueMapFromFile(plist);
                    ValueMap metadata = value["metadata"].asValueMap();
                    std::string textureFileName = metadata["textureFileName"].asString();
                    if (!FileUtils::getInstance()->isFileExist(textureFileName))
                    {
                        backGroundBoxDisabledErrorFilePath = textureFileName;
                    }
                }
                else
                {
                    backGroundBoxDisabledErrorFilePath = plist;
                }
                backGroundBoxDisabledFileExist = false;
            }
            break;
        }

        default:
            break;
        }
        /*if (backGroundBoxDisabledFileExist)
        {
            checkBox->loadTextureBackGroundDisabled(backGroundDisabledFileName, (Widget::TextureResType)backGroundDisabledType);
        }*/

        auto implNode = extension::ControlSwitch::create(createSprite(backGroundDic),
            createSprite(backGroundSelectedDic),
            createSprite(frontCrossDic),
            createSprite(backGroundDisabledDic));

        implNode->setOn(options->on());
        implNode->setTouchEnabled(options->enabled());
        // skeletonAnimation->setAnimation(0, options->animation()->c_str(), options->loop());
        // skeletonAnimation->setSkin(options->animation()->c_str());
        
        *ppResult = implNode;

        auto nodeReader = NodeReader::getInstance();
        nodeReader->setPropsWithFlatBuffers(implNode, (Table*)(options->nodeOptions()));

        // set Node Options
       // auto nodeOptions = options->nodeOptions();

       // GLubyte alpha = (GLubyte)nodeOptions->color()->a();
       // GLubyte red = (GLubyte)nodeOptions->color()->r();
       // GLubyte green = (GLubyte)nodeOptions->color()->g();
       // GLubyte blue = (GLubyte)nodeOptions->color()->b();

        //skeletonAnimation->setScaleX(nodeOptions->scale()->scaleX());
        //skeletonAnimation->setScaleY(nodeOptions->scale()->scaleY());

        //skeletonAnimation->setColor(Color3B(red, green, blue));
        //skeletonAnimation->setOpacity(alpha);

        /*bool flipX = nodeOptions->flipX() != 0;
        bool flipY = nodeOptions->flipY() != 0;

        if (flipX != false)
            sprite->setFlippedX(flipX);
        if (flipY != false)
            sprite->setFlippedY(flipY);*/
    }

    Node* ControlSwitchReader::createNodeWithFlatBuffers(const flatbuffers::Table *spriteOptions)
    {
        Node* spineAnimation = nullptr;
        // Sprite* sprite = wext::aSprite(); // Sprite::create();
        // #Hack
        setPropsWithFlatBuffers((Node*)(&spineAnimation), (Table*)spriteOptions);

        return spineAnimation;
    }

    int ControlSwitchReader::getResourceType(std::string key)
    {
        if (key == "Normal" || key == "Default")
        {
            return 	0;
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
}
