/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#include "sceneext/SpriteFrameCacheHelper.h"
#include "axmol/platform/FileUtils.h"
#include "axmol/2d/SpriteFrame.h"
#include "axmol/2d/SpriteFrameCache.h"

namespace ax::ext
{

SpriteFrameCacheHelper* SpriteFrameCacheHelper::_spriteFrameCacheHelper = nullptr;

SpriteFrameCacheHelper* SpriteFrameCacheHelper::getInstance()
{
    if (!_spriteFrameCacheHelper)
    {
        _spriteFrameCacheHelper = new SpriteFrameCacheHelper();
    }

    return _spriteFrameCacheHelper;
}

void SpriteFrameCacheHelper::destroyInstance()
{
    delete _spriteFrameCacheHelper;
    _spriteFrameCacheHelper = nullptr;
}

void SpriteFrameCacheHelper::retainSpriteFrames(std::string_view plistPath)
{
    auto it = _usingSpriteFrames.find(plistPath);
    if (it != _usingSpriteFrames.end())
        return;

    std::string fullPath   = FileUtils::getInstance()->fullPathForFilename(plistPath);
    ValueMap dict          = FileUtils::getInstance()->getValueMapFromFile(fullPath);
    auto spriteFramesCache = SpriteFrameCache::getInstance();
    ValueMap& framesDict   = dict["frames"].asValueMap();

    std::vector<SpriteFrame*> vec;
    for (auto iter = framesDict.begin(); iter != framesDict.end(); ++iter)
    {
        auto& spriteFrameName = iter->first;

        SpriteFrame* spriteFrame = spriteFramesCache->findFrame(spriteFrameName);
        AXASSERT(spriteFrame, "spriteframe is null!");

        vec.emplace_back(spriteFrame);
        AX_SAFE_RETAIN(spriteFrame);
    }
    _usingSpriteFrames[plistPath] = std::move(vec);
}

void SpriteFrameCacheHelper::releaseSpriteFrames(std::string_view plistPath)
{
    auto it = _usingSpriteFrames.find(plistPath);
    if (it == _usingSpriteFrames.end())
        return;

    auto& vec    = it->second;
    auto itFrame = vec.begin();
    while (itFrame != vec.end())
    {
        AX_SAFE_RELEASE(*itFrame);
        ++itFrame;
    }
    vec.clear();
    _usingSpriteFrames.erase(it);
}

void SpriteFrameCacheHelper::removeSpriteFrameFromFile(std::string_view plistPath)
{
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(plistPath);
    releaseSpriteFrames(plistPath);
}

void SpriteFrameCacheHelper::addSpriteFrameFromFile(std::string_view plistPath, std::string_view imagePath)
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistPath, imagePath);
    retainSpriteFrames(plistPath);
}

SpriteFrameCacheHelper::SpriteFrameCacheHelper() {}

SpriteFrameCacheHelper::~SpriteFrameCacheHelper()
{
    auto i = _usingSpriteFrames.begin();
    while (i != _usingSpriteFrames.end())
    {
        auto j = i++;
        removeSpriteFrameFromFile(j->first);
    }
}

}  // namespace ax::ext
