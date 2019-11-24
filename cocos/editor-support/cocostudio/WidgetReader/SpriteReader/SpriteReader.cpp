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

#include "editor-support/cocostudio/WidgetReader/SpriteReader/SpriteReader.h"

#include "2d/CCSprite.h"
#include "2d/CCSpriteFrameCache.h"
#include "platform/CCFileUtils.h"

#include "editor-support/cocostudio/CSParseBinary_generated.h"
#include "editor-support/cocostudio/FlatBuffersSerialize.h"
#include "editor-support/cocostudio/WidgetReader/NodeReader/NodeReader.h"


#include "flatbuffers/flatbuffers.h"
#include "editor-support/cocostudio/ext/glslutils.hpp"

USING_NS_CC;
using namespace flatbuffers;

namespace cocostudio
{
    IMPLEMENT_CLASS_NODE_READER_INFO(SpriteReader)
    
    SpriteReader::SpriteReader()
    {
        
    }
    
    SpriteReader::~SpriteReader()
    {
        
    }
    
    static SpriteReader* _instanceSpriteReader = nullptr;
    
    SpriteReader* SpriteReader::getInstance()
    {
        if (!_instanceSpriteReader)
        {
            _instanceSpriteReader = new (std::nothrow) SpriteReader();
        }
        
        return _instanceSpriteReader;
    }
    
    void SpriteReader::purge()
    {
        CC_SAFE_DELETE(_instanceSpriteReader);
    }
    
    void SpriteReader::destroyInstance()
    {
        CC_SAFE_DELETE(_instanceSpriteReader);
    }
    
    Offset<Table> SpriteReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                             flatbuffers::FlatBufferBuilder *builder)
    {
        auto temp = NodeReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
        auto nodeOptions = *(Offset<WidgetOptions>*)(&temp);
        
        std::string path = "";
        std::string plistFile = "";
        int resourceType = 0;
        bool intelliShadingEnabled = false;
        cocos2d::Vec3 hsv;
        cocos2d::Vec3 filter;
        
        cocos2d::BlendFunc blendFunc = cocos2d::BlendFunc::ALPHA_PREMULTIPLIED;
        
        // attributes
        auto attribute =  objectData.first_attribute();
        while (attribute)
        {
            std::string attriname = attribute.name();
            std::string value = attribute.value();

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
            std::string name = child.name();
            pugi::xml_attribute attribute;
            if (name == "FileData")
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
                    fbs->_textures.push_back(builder->CreateString(texture));                    
                }
            }
            else if (name == "BlendFunc")
            {
                attribute = child.first_attribute();
                while (attribute)
                {
                    name = attribute.name();
                    std::string value = attribute.value();
                    
                    if (name == "Src")
                    {
                        blendFunc.src = (backend::BlendFactor)atoi(value.c_str());
                    }
                    else if (name == "Dst")
                    {
                        blendFunc.dst = (backend::BlendFactor)atoi(value.c_str());
                    }
                    
                    attribute = attribute.next_attribute();
                }
            }
            else if (name == "HSV") {
                attribute = child.first_attribute();
                while (attribute)
                {
                    name = attribute.name();
                    std::string value = attribute.value();

                    if (name == "X")
                    {
                        hsv.x = atof(value.c_str());
                    }
                    else if (name == "Y")
                    {
                        hsv.y = atof(value.c_str());
                    }
                    else if (name == "Z")
                    {
                        hsv.z = atof(value.c_str());
                    }

                    attribute = attribute.next_attribute();
                }
            }
            else if (name == "FilterRGB") {
                attribute = child.first_attribute();
                while (attribute)
                {
                    name = attribute.name();
                    std::string value = attribute.value();

                    if (name == "X")
                    {
                        filter.x = atof(value.c_str());
                    }
                    else if (name == "Y")
                    {
                        filter.y = atof(value.c_str());
                    }
                    else if (name == "Z")
                    {
                        filter.z = atof(value.c_str());
                    }

                    attribute = attribute.next_attribute();
                }
            }
            
            child = child.next_sibling();
        }
        
        flatbuffers::BlendFunc f_blendFunc((int)blendFunc.src, (int)blendFunc.dst);

        auto options = CreateSpriteOptions(*builder,
                                           nodeOptions,
                                           CreateResourceData(*builder,
                                                              builder->CreateString(path),
                                                              builder->CreateString(plistFile),
                                                              resourceType),
                                           &f_blendFunc, 
                                           intelliShadingEnabled, 
                                           (const FVec3*)&hsv, 
                                           (const FVec3*)&filter);
        
        return *(Offset<Table>*)(&options);
    }
    
    void SpriteReader::setPropsWithFlatBuffers(cocos2d::Node *node,
                                                   const flatbuffers::Table* spriteOptions)
    {
        Sprite *sprite = static_cast<Sprite*>(node);
        auto options = (SpriteOptions*)spriteOptions;
        
        auto nodeReader = NodeReader::getInstance();
        nodeReader->setPropsWithFlatBuffers(node, (Table*)(options->nodeOptions()));
        
        auto fileNameData = cocos2d::wext::makeResourceData(options->fileNameData());

        cocos2d::wext::onBeforeLoadObjectAsset(node, fileNameData, 0);
        
        int resourceType = fileNameData.type;
        std::string& path = fileNameData.file;
        
        std::string errorFilePath = "";
        
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
                std::string& plist = fileNameData.plist;
                SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(path);
                if (spriteFrame)
                {
                    sprite->setSpriteFrame(spriteFrame);
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
            cocos2d::BlendFunc blendFunc = cocos2d::BlendFunc::ALPHA_PREMULTIPLIED;
            blendFunc.src = (backend::BlendFactor)f_blendFunc->src();
            blendFunc.dst = (backend::BlendFactor)f_blendFunc->dst();
            sprite->setBlendFunc(blendFunc);
        }
        
        auto nodeOptions = options->nodeOptions();
        
        GLubyte alpha       = (GLubyte)nodeOptions->color()->a();
        GLubyte red         = (GLubyte)nodeOptions->color()->r();
        GLubyte green       = (GLubyte)nodeOptions->color()->g();
        GLubyte blue        = (GLubyte)nodeOptions->color()->b();
        
        if (alpha != 255)
        {
            sprite->setOpacity(alpha);
        }
        if (red != 255 || green != 255 || blue != 255)
        {
            sprite->setColor(Color3B(red, green, blue));
        }
        
        bool flipX   = nodeOptions->flipX() != 0;
        bool flipY   = nodeOptions->flipY() != 0;
        
        if(flipX != false)
            sprite->setFlippedX(flipX);
        if(flipY != false)
            sprite->setFlippedY(flipY);

        if (options->intelliShadingEnabled()) {
            auto hsv = options->hsv();
            auto filter = options->filter();
            if (hsv != nullptr && filter != nullptr) {
                glslutils::enableNodeIntelliShading(sprite,
                    true,
                    Vec3(hsv->x(), hsv->y(), hsv->z()),
                    Vec3(filter->x(), filter->y(), filter->z()));
            }
        }
    }
    
    Node* SpriteReader::createNodeWithFlatBuffers(const flatbuffers::Table *spriteOptions)
    {
        Sprite* sprite = wext::aSprite();// Sprite::create();
        
        setPropsWithFlatBuffers(sprite, (Table*)spriteOptions);
        
        return sprite;
    }
    
    int SpriteReader::getResourceType(std::string key)
    {
        if(key == "Normal" || key == "Default")
        {
            return 	0;
        }
        
        FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
        if(fbs->_isSimulator)
        {
            if(key == "MarkedSubImage")
            {
                return 0;
            }
        }
        return 1;
    }
}
