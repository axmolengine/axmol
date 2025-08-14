/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2009      Jason Booth
Copyright (c) 2009      Robert J Payne
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include "2d/SpriteFrameCache.h"

#include <vector>

#include "2d/Sprite.h"
#include "2d/AutoPolygon.h"
#include "2d/PlistSpriteSheetLoader.h"
#include "platform/FileUtils.h"
#include "base/Macros.h"
#include "base/Director.h"
#include "renderer/Texture2D.h"
#include "base/NinePatchImageParser.h"
#include "xxhash.h"

using namespace std;

namespace ax
{

static SpriteFrameCache* _sharedSpriteFrameCache = nullptr;

static uint64_t computeHash(const std::string_view& v)
{
    return XXH64(v.data(), v.length(), 0);
}

SpriteFrameCache* SpriteFrameCache::getInstance()
{
    if (!_sharedSpriteFrameCache)
    {
        _sharedSpriteFrameCache = new SpriteFrameCache();
        _sharedSpriteFrameCache->init();
    }

    return _sharedSpriteFrameCache;
}

void SpriteFrameCache::destroyInstance()
{
    AX_SAFE_DELETE(_sharedSpriteFrameCache);
}

bool SpriteFrameCache::init()
{
    _spriteFrames.reserve(20);
    clear();

    registerSpriteSheetLoader(std::make_shared<PlistSpriteSheetLoader>());

    return true;
}

SpriteFrameCache::~SpriteFrameCache() {}

void SpriteFrameCache::addSpriteFramesWithFile(std::string_view spriteSheetFileName,
                                               std::string_view textureFileName,
                                               uint32_t spriteSheetFormat)
{
    auto* loader = getSpriteSheetLoader(spriteSheetFormat);
    if (loader)
    {
        loader->load(spriteSheetFileName, textureFileName, *this);
    }
}

void SpriteFrameCache::addSpriteFramesWithFile(std::string_view spriteSheetFileName,
                                               Texture2D* texture,
                                               uint32_t spriteSheetFormat)
{
    auto* loader = getSpriteSheetLoader(spriteSheetFormat);
    if (loader)
    {
        loader->load(spriteSheetFileName, texture, *this);
    }
}

void SpriteFrameCache::addSpriteFramesWithFile(std::string_view spriteSheetFileName, uint32_t spriteSheetFormat)
{
    auto* loader = getSpriteSheetLoader(spriteSheetFormat);
    if (loader)
    {
        loader->load(spriteSheetFileName, *this);
    }
}

void SpriteFrameCache::addSpriteFramesWithFileContent(const Data& content,
                                                      Texture2D* texture,
                                                      uint32_t spriteSheetFormat)
{
    auto* loader = getSpriteSheetLoader(spriteSheetFormat);
    if (loader)
    {
        loader->load(content, texture, *this);
    }
}

bool SpriteFrameCache::isSpriteFramesWithFileLoaded(std::string_view plist) const
{
    auto sheetId = computeHash(plist);
    return isSpriteSheetInUse(sheetId) && isPlistFull(sheetId);
}

void SpriteFrameCache::addSpriteFrame(SpriteFrame* frame, std::string_view frameName)
{
    AXASSERT(frame, "frame should not be nil");

    const auto name = computeHash("by#addSpriteFrame()");
    auto&& itr      = _spriteSheets.find(name);
    if (itr != _spriteSheets.end())
    {
        insertFrame(itr->second, frameName, frame);
    }
    else
    {
        auto spriteSheet  = std::make_shared<SpriteSheet>();
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
    std::vector<uint64_t> toRemoveFrames;

    const auto& frames = getSpriteFrames();
    for (auto&& iter : frames)
    {
        auto* spriteFrame = iter.second;
        if (spriteFrame->getReferenceCount() == 1)
        {
            toRemoveFrames.emplace_back(iter.first);
            spriteFrame->getTexture()->removeSpriteFrameCapInset(spriteFrame);
            AXLOGD("SpriteFrameCache: removing unused frame: {}", spriteFrame->getName());
            removed = true;
        }
    }

    if (removed)
    {
        eraseFrames(toRemoveFrames);
    }
}

void SpriteFrameCache::removeUnusedSpriteSheets()
{
    std::vector<uint64_t> willRemoveSpriteSheetFileNames;
    for (auto&& it : _spriteSheets)
    {
        bool isUsed = false;
        for (auto&& frame : it.second->frames)
        {
            auto spriteFrame = findFrame(frame);
            if (spriteFrame && spriteFrame->getReferenceCount() > 1)
            {
                isUsed = true;
                break;
            }
        }

        if (!isUsed)
            willRemoveSpriteSheetFileNames.push_back(it.first);
    }

    for (auto& sheetId : willRemoveSpriteSheetFileNames)
    {
        AXLOGD("SpriteFrameCache: removing unused sprite sheet file : {}", sheetId);
        removeSpriteSheet(sheetId);
    }
}

void SpriteFrameCache::removeSpriteFrameByName(std::string_view name)
{
    // explicit nil handling
    if (name.empty())
        return;

    eraseFrame(name);
}

void SpriteFrameCache::removeSpriteFramesFromFile(std::string_view atlasPath)
{
    // const auto fullPath = FileUtils::getInstance()->fullPathForFilename(plist);
    // auto dict = FileUtils::getInstance()->getValueMapFromFile(fullPath);
    // if (dict.empty())
    //{
    //     AXLOGD("axmol:SpriteFrameCache:removeSpriteFramesFromFile: create dict by {} fail.",plist);
    //     return;
    // }
    // removeSpriteFramesFromDictionary(dict);

    // remove it from the cache
    removeSpriteSheet(computeHash(atlasPath));
}

void SpriteFrameCache::removeSpriteFramesFromFileContent(std::string_view plist_content)
{
    auto dict =
        FileUtils::getInstance()->getValueMapFromData(plist_content.data(), static_cast<int>(plist_content.size()));
    if (dict.empty())
    {
        AXLOGD("SpriteFrameCache:removeSpriteFramesFromFileContent: create dict by fail.");
        return;
    }
    removeSpriteFramesFromDictionary(dict);
}

void SpriteFrameCache::removeSpriteFramesFromDictionary(ValueMap& dictionary)
{
    if (dictionary["frames"].getType() != ax::Value::Type::MAP)
        return;

    const auto& framesDict = dictionary["frames"].asValueMap();
    std::vector<uint64_t> keysToRemove;

    for (const auto& iter : framesDict)
    {
        auto frameId = computeHash(iter.first);
        if (findFrame(frameId))
        {
            keysToRemove.emplace_back(frameId);
        }
    }

    eraseFrames(keysToRemove);
}

void SpriteFrameCache::removeSpriteFramesFromTexture(Texture2D* texture)
{
    std::vector<uint64_t> keysToRemove;

    for (auto&& iter : getSpriteFrames())
    {
        auto* frame = findFrame(iter.first);
        if (frame && (frame->getTexture() == texture))
        {
            keysToRemove.emplace_back(iter.first);
        }
    }

    eraseFrames(keysToRemove);
}

SpriteFrame* SpriteFrameCache::getSpriteFrameByName(std::string_view name)
{
    auto* frame = findFrame(name);
    if (!frame)
    {
        AXLOGD("axmol: SpriteFrameCache: Frame '{}' isn't found", name);
    }
    return frame;
}

bool SpriteFrameCache::reloadTexture(std::string_view spriteSheetFileName)
{
    AXASSERT(!spriteSheetFileName.empty(), "plist filename should not be nullptr");
    auto sheetId              = computeHash(spriteSheetFileName);
    const auto spriteSheetItr = _spriteSheets.find(sheetId);
    if (spriteSheetItr == _spriteSheets.end())
    {
        return false;  // Sprite sheet wasn't loaded, so don't reload it
    }

    const auto format = spriteSheetItr->second->format;

    if (isSpriteSheetInUse(sheetId))
    {
        removeSpriteSheet(sheetId);  // we've removed the associated entry, so spriteSheetItr is no longer valid
    }
    else
    {
        // If one plist hasn't be loaded, we don't load it here.
        return false;
    }

    auto* loader = getSpriteSheetLoader(format);
    if (loader)
    {
        loader->reload(spriteSheetFileName, *this);
    }
    return true;
}

void SpriteFrameCache::insertFrame(const std::shared_ptr<SpriteSheet>& spriteSheet,
                                   std::string_view frameName,
                                   SpriteFrame* spriteFrame)
{
    auto frameId = computeHash(frameName);
    spriteFrame->setName(frameName);
    spriteSheet->frames.emplace(frameId);
    _spriteFrames.insert(frameId, spriteFrame);  // add SpriteFrame
    if (spriteSheet->pathId == (uint64_t)-1)
        spriteSheet->pathId = computeHash(spriteSheet->path);
    hlookup::set_item(_spriteSheets, spriteSheet->pathId, spriteSheet);
    hlookup::set_item(_spriteFrameToSpriteSheetMap, frameId, spriteSheet);
}

bool SpriteFrameCache::eraseFrame(std::string_view frameName)
{
    return eraseFrame(computeHash(frameName));
}

bool SpriteFrameCache::eraseFrame(uint64_t frameId)
{
    // drop SpriteFrame
    const auto itFrame = _spriteFrameToSpriteSheetMap.find(frameId);
    bool found         = itFrame != _spriteFrameToSpriteSheetMap.end();
    if (found)
    {
        auto& spriteSheet = itFrame->second;
        spriteSheet->full = false;
        spriteSheet->frames.erase(frameId);

        if (spriteSheet->frames.empty())
        {
            _spriteSheets.erase(computeHash(spriteSheet->path));
        }

        _spriteFrameToSpriteSheetMap.erase(itFrame);  // update index frame->plist

        // erase all sprite sheets if all frames are erased
        // if (_spriteFrameToSpriteSheetMap.empty())
        //{
        //    _spriteSheets.clear();
        //}
    }
    _spriteFrames.erase(frameId);
    return found;
}

bool SpriteFrameCache::eraseFrames(const std::vector<uint64_t>& frames)
{
    auto ret = false;
    for (const auto& frame : frames)
    {
        ret |= eraseFrame(frame);
    }

    return ret;
}

bool SpriteFrameCache::removeSpriteSheet(uint64_t sheetId)
{
    auto it = _spriteSheets.find(sheetId);
    if (it == _spriteSheets.end())
        return false;

    auto& frames = it->second->frames;
    for (const auto& f : frames)
    {
        // !!do not!! call `_spriteFrames.erase(f);` to erase SpriteFrame
        // only erase index here
        _spriteFrames.erase(f);
        _spriteFrameToSpriteSheetMap.erase(f);  // erase plist frame frameName->plist
    }
    _spriteSheets.erase(sheetId);  // update index plist->[frameNames]

    return true;
}

void SpriteFrameCache::clear()
{
    _spriteSheets.clear();
    _spriteFrameToSpriteSheetMap.clear();
    _spriteFrames.clear();
}

bool SpriteFrameCache::isSpriteSheetInUse(uint64_t sheetId) const
{
    const auto spriteSheetItr = _spriteSheets.find(sheetId);
    return spriteSheetItr != _spriteSheets.end() && !spriteSheetItr->second->frames.empty();
}

SpriteFrame* SpriteFrameCache::findFrame(std::string_view frame)
{
    return _spriteFrames.at(computeHash(frame));
}

SpriteFrame* SpriteFrameCache::findFrame(uint64_t frameId)
{
    return _spriteFrames.at(frameId);
}

void SpriteFrameCache::addSpriteFrameCapInset(SpriteFrame* spriteFrame, const Rect& capInsets, Texture2D* texture)
{
    texture->addSpriteFrameCapInset(spriteFrame, capInsets);
}

const ax::Map<uint64_t, SpriteFrame*>& SpriteFrameCache::getSpriteFrames()
{
    return _spriteFrames;
}

void SpriteFrameCache::registerSpriteSheetLoader(std::shared_ptr<ISpriteSheetLoader> loader)
{
    auto format = loader->getFormat();

    if (_spriteSheetLoaders.find(format) != _spriteSheetLoaders.end())
    {
        return;
    }

    _spriteSheetLoaders.emplace(format, std::move(loader));
}

void SpriteFrameCache::deregisterSpriteSheetLoader(uint32_t spriteSheetFormat)
{
    auto&& itr = _spriteSheetLoaders.find(spriteSheetFormat);
    if (itr != _spriteSheetLoaders.end())
    {
        _spriteSheetLoaders.erase(itr);
    }
}

ISpriteSheetLoader* SpriteFrameCache::getSpriteSheetLoader(uint32_t spriteSheetFormat)
{
    auto&& itr = _spriteSheetLoaders.find(spriteSheetFormat);
    if (itr != _spriteSheetLoaders.end())
    {
        return itr->second.get();
    }

    return nullptr;
}

}  // namespace ax
