/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2009      Jason Booth
Copyright (c) 2009      Robert J Payne
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include "2d/CCSpriteFrameCache.h"

#include <vector>


#include "2d/CCSprite.h"
#include "2d/CCAutoPolygon.h"
#include "2d/CCPlistSpriteSheetLoader.h"
#include "platform/CCFileUtils.h"
#include "base/ccMacros.h"
#include "base/CCDirector.h"
#include "renderer/CCTexture2D.h"
#include "base/CCNinePatchImageParser.h"

using namespace std;

NS_CC_BEGIN

static SpriteFrameCache* _sharedSpriteFrameCache = nullptr;

SpriteFrameCache* SpriteFrameCache::getInstance()
{
    if (!_sharedSpriteFrameCache)
    {
        _sharedSpriteFrameCache = new (std::nothrow) SpriteFrameCache();
        _sharedSpriteFrameCache->init();
    }

    return _sharedSpriteFrameCache;
}

void SpriteFrameCache::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedSpriteFrameCache);
}

bool SpriteFrameCache::init()
{
    _spriteFrames.reserve(20);
    clear();

    registerSpriteSheetLoader("PLIST", std::make_unique<PlistSpriteSheetLoader>());

    return true;
}

SpriteFrameCache::~SpriteFrameCache()
{
}

void SpriteFrameCache::addSpriteFramesWithFile(const std::string& spriteSheetFileName, Texture2D* texture, const std::string& format)
{
    auto* loader = getSpriteSheetLoader(format);
    if (loader)
    {
        loader->Load(spriteSheetFileName, texture, *this);
    }
}

void SpriteFrameCache::addSpriteFramesWithFile(const std::string& spriteSheetFileName, const std::string& format)
{
    auto* loader = getSpriteSheetLoader(format);
    if (loader)
    {
        loader->Load(spriteSheetFileName, *this);
    }
}

void SpriteFrameCache::addSpriteFramesWithFileContent(const Data& content, Texture2D* texture, const std::string& format)
{
    auto* loader = getSpriteSheetLoader(format);
    if (loader)
    {
        loader->Load(content, texture, *this);
    }
}

bool SpriteFrameCache::isSpriteFramesWithFileLoaded(const std::string& plist) const
{
    return isSpriteSheetInUse(plist) && isPlistFull(plist);
}

void SpriteFrameCache::addSpriteFrame(SpriteFrame* frame, const std::string& frameName)
{
    CCASSERT(frame, "frame should not be nil");

    const std::string name = "by#addSpriteFrame()";
    auto&& itr = _spriteSheets.find(name);
    if (itr != _spriteSheets.end())
    {
        insertFrame(itr->second, frameName, frame);
    }
    else
    {
        auto spriteSheet = std::make_shared<SpriteSheet>();
        spriteSheet->path = name;
        insertFrame(spriteSheet, frameName, frame);
    }
}

void SpriteFrameCache::removeSpriteFrames()
{
    clear();
}

void SpriteFrameCache::removeUnusedSpriteFrames()
{
    auto removed = false;
    std::vector<std::string> toRemoveFrames;

    const auto frames = getSpriteFrames();
    for (auto& iter : frames)
    {
        auto* spriteFrame = iter.second;
        if (spriteFrame->getReferenceCount() == 1)
        {
            toRemoveFrames.push_back(iter.first);
            spriteFrame->getTexture()->removeSpriteFrameCapInset(spriteFrame);
            CCLOG("cocos2d: SpriteFrameCache: removing unused frame: %s", iter.first.c_str());
            removed = true;
        }
    }

    if (removed)
    {
        eraseFrames(toRemoveFrames);
    }
}

void SpriteFrameCache::removeSpriteFrameByName(const std::string& name)
{
    // explicit nil handling
    if (name.empty())
        return;

    eraseFrame(name);
}

void SpriteFrameCache::removeSpriteFramesFromFile(const std::string& atlasPath)
{
    //const auto fullPath = FileUtils::getInstance()->fullPathForFilename(plist);
    //auto dict = FileUtils::getInstance()->getValueMapFromFile(fullPath);
    //if (dict.empty())
    //{
    //    CCLOG("cocos2d:SpriteFrameCache:removeSpriteFramesFromFile: create dict by %s fail.",plist.c_str());
    //    return;
    //}
    //removeSpriteFramesFromDictionary(dict);

    // remove it from the cache
    removeSpriteSheet(atlasPath);
}

void SpriteFrameCache::removeSpriteFramesFromFileContent(const std::string& plist_content)
{
    auto dict = FileUtils::getInstance()->getValueMapFromData(plist_content.data(), static_cast<int>(plist_content.size()));
    if (dict.empty())
    {
        CCLOG("cocos2d:SpriteFrameCache:removeSpriteFramesFromFileContent: create dict by fail.");
        return;
    }
    removeSpriteFramesFromDictionary(dict);
}

void SpriteFrameCache::removeSpriteFramesFromDictionary(ValueMap& dictionary)
{
    if (dictionary["frames"].getType() != cocos2d::Value::Type::MAP)
        return;

    const auto& framesDict = dictionary["frames"].asValueMap();
    std::vector<std::string> keysToRemove;

    for (const auto& iter : framesDict)
    {
        if (findFrame(iter.first))
        {
            keysToRemove.push_back(iter.first);
        }
    }

    eraseFrames(keysToRemove);
}

void SpriteFrameCache::removeSpriteFramesFromTexture(Texture2D* texture)
{
    std::vector<std::string> keysToRemove;

    for (auto& iter : getSpriteFrames())
    {
        auto key = iter.first;
        auto* frame = findFrame(key);
        if (frame && (frame->getTexture() == texture))
        {
            keysToRemove.push_back(key);
        }
    }

    eraseFrames(keysToRemove);
}

SpriteFrame* SpriteFrameCache::getSpriteFrameByName(const std::string& name)
{
    auto* frame = findFrame(name);
    if (!frame)
    {
        CCLOG("cocos2d: SpriteFrameCache: Frame '%s' isn't found", name.c_str());
    }
    return frame;
}

bool SpriteFrameCache::reloadTexture(const std::string& spriteSheetFileName)
{
    CCASSERT(!spriteSheetFileName.empty(), "plist filename should not be nullptr");

    const auto spriteSheetItr = _spriteSheets.find(spriteSheetFileName);
    if (spriteSheetItr == _spriteSheets.end())
    {
        return false; // Sprite sheet wasn't loaded, so don't reload it
    }

    const auto format = spriteSheetItr->second->format;

    if (isSpriteSheetInUse(spriteSheetFileName))
    {
        removeSpriteSheet(spriteSheetFileName); // we've removed the associated entry, so spriteSheetItr is no longer valid
    }
    else
    {
        //If one plist hasn't be loaded, we don't load it here.
        return false;
    }

    auto* loader = getSpriteSheetLoader(format);
    if (loader)
    {
        loader->Reload(spriteSheetFileName, *this);
    }
    return true;
}

void SpriteFrameCache::insertFrame(const std::shared_ptr<SpriteSheet>& spriteSheet, const std::string& frameName, SpriteFrame* spriteFrame)
{
    spriteSheet->frames.insert(frameName);
    _spriteFrames.insert(frameName, spriteFrame);   //add SpriteFrame
    _spriteSheets[spriteSheet->path] = spriteSheet;
    _spriteFrameToSpriteSheetMap[frameName] = spriteSheet; //insert index frameName->plist
}

bool SpriteFrameCache::eraseFrame(const std::string& frameName)
{
    _spriteFrames.erase(frameName); //drop SpriteFrame
    const auto itFrame = _spriteFrameToSpriteSheetMap.find(frameName);
    if (itFrame != _spriteFrameToSpriteSheetMap.end())
    {
        auto& spriteSheet = itFrame->second;
        spriteSheet->full = false;
        spriteSheet->frames.erase(frameName);

        if (spriteSheet->frames.empty())
        {
            _spriteSheets.erase(spriteSheet->path);
        }

        _spriteFrameToSpriteSheetMap.erase(itFrame);    //update index frame->plist

        // erase all sprite sheets if all frames are erased
        //if (_spriteFrameToSpriteSheetMap.empty())
        //{
        //    _spriteSheets.clear();
        //}
        return true;
    }
    return false;
}

bool SpriteFrameCache::eraseFrames(const std::vector<std::string>& frames)
{
    auto ret = false;
    for (const auto& frame : frames)
    {
        ret |= eraseFrame(frame);
    }

    return ret;
}

bool SpriteFrameCache::removeSpriteSheet(const std::string& spriteSheetFileName)
{
    auto it = _spriteSheets.find(spriteSheetFileName);
    if (it == _spriteSheets.end()) return false;

    auto& frames = it->second->frames;
    for (const auto& f : frames)
    {
        // !!do not!! call `_spriteFrames.erase(f);` to erase SpriteFrame
        // only erase index here
        _spriteFrames.erase(f);
        _spriteFrameToSpriteSheetMap.erase(f);                             //erase plist frame frameName->plist
    }
    _spriteSheets.erase(spriteSheetFileName);                            //update index plist->[frameNames]

    return true;
}

void SpriteFrameCache::clear()
{
    _spriteSheets.clear();
    _spriteFrameToSpriteSheetMap.clear();
    _spriteFrames.clear();
}

bool SpriteFrameCache::hasFrame(const std::string& frame) const
{
    return _spriteFrameToSpriteSheetMap.find(frame) != _spriteFrameToSpriteSheetMap.end();
}

bool SpriteFrameCache::isSpriteSheetInUse(const std::string& spriteSheetFileName) const
{
    const auto spriteSheetItr = _spriteSheets.find(spriteSheetFileName);
    return spriteSheetItr != _spriteSheets.end() && !spriteSheetItr->second->frames.empty();
}

SpriteFrame* SpriteFrameCache::findFrame(const std::string& frame)
{
    return _spriteFrames.at(frame);
}

void SpriteFrameCache::addSpriteFrameCapInset(SpriteFrame* spriteFrame, const Rect& capInsets, Texture2D* texture)
{
    texture->addSpriteFrameCapInset(spriteFrame, capInsets);
}

Map<std::string, SpriteFrame*>& SpriteFrameCache::getSpriteFrames()
{
    return _spriteFrames;
}

void SpriteFrameCache::registerSpriteSheetLoader(std::string formatId, std::unique_ptr<ISpriteSheetLoader> loader)
{
    if (_spriteSheetLoaders.find(formatId) != _spriteSheetLoaders.end())
    {
        return;
    }

    _spriteSheetLoaders.emplace(std::move(formatId), std::move(loader));
}

void SpriteFrameCache::deregisterSpriteSheetLoader(const std::string& formatId)
{
    auto&& itr = _spriteSheetLoaders.find(formatId);
    if (itr != _spriteSheetLoaders.end())
    {
        _spriteSheetLoaders.erase(itr);
    }
}

ISpriteSheetLoader* SpriteFrameCache::getSpriteSheetLoader(const std::string& formatId)
{
    auto&& itr = _spriteSheetLoaders.find(formatId);
    if (itr != _spriteSheetLoaders.end())
    {
        return itr->second.get();
    }

    return nullptr;
}

NS_CC_END
