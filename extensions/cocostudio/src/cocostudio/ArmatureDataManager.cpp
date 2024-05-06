/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.

https://axmolengine.github.io/

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

#include "2d/SpriteFrameCache.h"

#include "ArmatureDataManager.h"
#include "TransformHelp.h"
#include "DataReaderHelper.h"
#include "SpriteFrameCacheHelper.h"

USING_NS_AX;

namespace cocostudio
{
static ArmatureDataManager* s_sharedArmatureDataManager = nullptr;

ArmatureDataManager* ArmatureDataManager::getInstance()
{
    if (s_sharedArmatureDataManager == nullptr)
    {
        s_sharedArmatureDataManager = new ArmatureDataManager();
        if (!s_sharedArmatureDataManager->init())
        {
            AX_SAFE_DELETE(s_sharedArmatureDataManager);
        }
    }
    return s_sharedArmatureDataManager;
}

void ArmatureDataManager::destroyInstance()
{
    SpriteFrameCacheHelper::purge();
    DataReaderHelper::purge();
    AX_SAFE_RELEASE_NULL(s_sharedArmatureDataManager);
}

ArmatureDataManager::ArmatureDataManager(void)
{
    _armarureDatas.clear();
    _animationDatas.clear();
    _textureDatas.clear();
    _autoLoadSpriteFile = false;
}

ArmatureDataManager::~ArmatureDataManager(void)
{
    _animationDatas.clear();
    _armarureDatas.clear();
    _textureDatas.clear();

    _relativeDatas.clear();
}

bool ArmatureDataManager::init()
{
    bool bRet = false;
    do
    {
        _armarureDatas.clear();
        _animationDatas.clear();
        _textureDatas.clear();

        bRet = true;
    } while (0);

    return bRet;
}

void ArmatureDataManager::removeArmatureFileInfo(std::string_view configFilePath)
{
    if (RelativeData* data = getRelativeData(configFilePath))
    {
        for (std::string str : data->armatures)
        {
            removeArmatureData(str);
        }

        for (std::string str : data->animations)
        {
            removeAnimationData(str);
        }

        for (std::string str : data->textures)
        {
            removeTextureData(str);
        }

        for (std::string str : data->plistFiles)
        {
            SpriteFrameCacheHelper::getInstance()->removeSpriteFrameFromFile(str);
        }

        _relativeDatas.erase(configFilePath);
        DataReaderHelper::getInstance()->removeConfigFile(configFilePath);
    }
}

void ArmatureDataManager::addArmatureData(std::string_view id,
                                          ArmatureData* armatureData,
                                          std::string_view configFilePath)
{
    if (RelativeData* data = getRelativeData(configFilePath))
    {
        data->armatures.emplace_back(std::string{id});
    }

    _armarureDatas.insert(id, armatureData);
}

ArmatureData* ArmatureDataManager::getArmatureData(std::string_view id)
{
    return dynamic_cast<ArmatureData*>(_armarureDatas.at(id));
}

void ArmatureDataManager::removeArmatureData(std::string_view id)
{
    _armarureDatas.erase(id);
}

void ArmatureDataManager::addAnimationData(std::string_view id,
                                           AnimationData* animationData,
                                           std::string_view configFilePath)
{
    if (RelativeData* data = getRelativeData(configFilePath))
    {
        data->animations.emplace_back(std::string{id});
    }

    _animationDatas.insert(id, animationData);
}

AnimationData* ArmatureDataManager::getAnimationData(std::string_view id)
{
    return dynamic_cast<AnimationData*>(_animationDatas.at(id));
}

void ArmatureDataManager::removeAnimationData(std::string_view id)
{
    _animationDatas.erase(id);
}

void ArmatureDataManager::addTextureData(std::string_view id, TextureData* textureData, std::string_view configFilePath)
{
    if (RelativeData* data = getRelativeData(configFilePath))
    {
        data->textures.emplace_back(std::string{id});
    }

    _textureDatas.insert(id, textureData);
}

TextureData* ArmatureDataManager::getTextureData(std::string_view id)
{
    return dynamic_cast<TextureData*>(_textureDatas.at(id));
}

void ArmatureDataManager::removeTextureData(std::string_view id)
{
    _textureDatas.erase(id);
}

void ArmatureDataManager::addArmatureFileInfo(std::string_view configFilePath)
{
    addRelativeData(configFilePath);

    _autoLoadSpriteFile = true;
    DataReaderHelper::getInstance()->addDataFromFile(configFilePath);
}

void ArmatureDataManager::addArmatureFileInfoAsync(std::string_view configFilePath, Object* target, SEL_SCHEDULE selector)
{
    addRelativeData(configFilePath);

    _autoLoadSpriteFile = true;
    DataReaderHelper::getInstance()->addDataFromFileAsync("", "", configFilePath, target, selector);
}

void ArmatureDataManager::addArmatureFileInfo(std::string_view imagePath,
                                              std::string_view plistPath,
                                              std::string_view configFilePath)
{
    addRelativeData(configFilePath);

    _autoLoadSpriteFile = false;
    DataReaderHelper::getInstance()->addDataFromFile(configFilePath);
    addSpriteFrameFromFile(plistPath, imagePath, configFilePath);
}

void ArmatureDataManager::addArmatureFileInfoAsync(std::string_view imagePath,
                                                   std::string_view plistPath,
                                                   std::string_view configFilePath,
                                                   Object* target,
                                                   SEL_SCHEDULE selector)
{
    addRelativeData(configFilePath);

    _autoLoadSpriteFile = false;
    DataReaderHelper::getInstance()->addDataFromFileAsync(imagePath, plistPath, configFilePath, target, selector);
    addSpriteFrameFromFile(plistPath, imagePath, configFilePath);
}

void ArmatureDataManager::addSpriteFrameFromFile(std::string_view plistPath,
                                                 std::string_view imagePath,
                                                 std::string_view configFilePath)
{
    if (RelativeData* data = getRelativeData(configFilePath))
    {
        data->plistFiles.emplace_back(std::string{plistPath});
    }
    SpriteFrameCacheHelper::getInstance()->addSpriteFrameFromFile(plistPath, imagePath);
}

bool ArmatureDataManager::isAutoLoadSpriteFile()
{
    return _autoLoadSpriteFile;
}

const ax::StringMap<ArmatureData*>& ArmatureDataManager::getArmatureDatas() const
{
    return _armarureDatas;
}
const ax::StringMap<AnimationData*>& ArmatureDataManager::getAnimationDatas() const
{
    return _animationDatas;
}
const ax::StringMap<TextureData*>& ArmatureDataManager::getTextureDatas() const
{
    return _textureDatas;
}

void ArmatureDataManager::addRelativeData(std::string_view configFilePath)
{
    if (_relativeDatas.find(configFilePath) == _relativeDatas.end())
    {
        _relativeDatas[configFilePath] = RelativeData();
    }
}

RelativeData* ArmatureDataManager::getRelativeData(std::string_view configFilePath)
{
    return &_relativeDatas[configFilePath];
}

}  // namespace cocostudio
