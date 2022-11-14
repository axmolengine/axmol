/****************************************************************************
 Copyright (c) 2013      Zynga Inc.
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2021 Bytedance Inc.

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
#include "2d/CCFontAtlasCache.h"

#include "base/CCDirector.h"
#include "2d/CCFontFNT.h"
#include "2d/CCFontFreeType.h"
#include "2d/CCFontAtlas.h"
#include "2d/CCFontCharMap.h"
#include "2d/CCLabel.h"
#include "platform/CCFileUtils.h"

NS_AX_BEGIN

hlookup::string_map<FontAtlas*> FontAtlasCache::_atlasMap;
#define ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE 255

void FontAtlasCache::purgeCachedData()
{
    auto atlasMapCopy = _atlasMap;
    for (auto&& atlas : atlasMapCopy)
    {
        auto refCount = atlas.second->getReferenceCount();
        atlas.second->release();
        if (refCount != 1)
            atlas.second->purgeTexturesAtlas();
    }
    _atlasMap.clear();
}

FontAtlas* FontAtlasCache::getFontAtlasTTF(const _ttfConfig* config)
{
    auto realFontFilename = config->fontFilePath;  // resolves real file path, to prevent storing multiple atlases for the same file.
    bool useDistanceField = config->distanceFieldEnabled;
    if (config->outlineSize > 0)
    {
        useDistanceField = false;
    }

    std::string key;
    char keyPrefix[ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE];
    snprintf(keyPrefix, ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE, useDistanceField ? "df %.2f %d " : "%.2f %d ",
             config->fontSize, config->outlineSize);
    std::string atlasName(keyPrefix);
    atlasName += realFontFilename;

    auto it = _atlasMap.find(atlasName);

    if (it == _atlasMap.end())
    {
        auto font = FontFreeType::create(realFontFilename, config->fontSize, config->glyphs, config->customGlyphs,
                                         useDistanceField, (float)config->outlineSize);
        if (font)
        {
            auto tempAtlas = font->newFontAtlas();
            if (tempAtlas)
            {
                return _atlasMap.emplace(atlasName, tempAtlas).first->second;
            }
        }
    }
    else
        return it->second;

    return nullptr;
}

FontAtlas* FontAtlasCache::getFontAtlasFNT(std::string_view fontFileName)
{
    return getFontAtlasFNT(fontFileName, Rect::ZERO, false);
}

FontAtlas* FontAtlasCache::getFontAtlasFNT(std::string_view fontFileName, std::string_view subTextureKey)
{
    const auto realFontFilename = fontFileName;  // resolves real file path, to prevent storing multiple atlases for the same file.
    std::string atlasName{subTextureKey};
    atlasName.append(" ", 1).append(realFontFilename);

    const auto it = _atlasMap.find(atlasName);
    if (it == _atlasMap.end())
    {
        const auto font = FontFNT::create(realFontFilename, subTextureKey);

        if (font)
        {
            const auto tempAtlas = font->newFontAtlas();
            if (tempAtlas)
            {
                _atlasMap[atlasName] = tempAtlas;
                return _atlasMap[atlasName];
            }
        }
    }
    else
        return it->second;

    return nullptr;
}

FontAtlas* FontAtlasCache::getFontAtlasFNT(std::string_view fontFileName, const Rect& imageRect, bool imageRotated)
{
    const auto realFontFilename = fontFileName;  // resolves real file path, to prevent storing multiple atlases for the same file.
    char keyPrefix[ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE];
    snprintf(keyPrefix, ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE, "%.2f %.2f ", imageRect.origin.x, imageRect.origin.y);
    std::string atlasName(keyPrefix);
    atlasName += realFontFilename;

    const auto it = _atlasMap.find(atlasName);
    if (it == _atlasMap.end())
    {
        const auto font = FontFNT::create(realFontFilename, imageRect, imageRotated);

        if (font)
        {
            const auto tempAtlas = font->newFontAtlas();
            if (tempAtlas)
            {
                _atlasMap[atlasName] = tempAtlas;
                return _atlasMap[atlasName];
            }
        }
    }
    else
        return it->second;

    return nullptr;
}

FontAtlas* FontAtlasCache::getFontAtlasFNT(std::string_view fontFileName, const Vec2& imageOffset)
{
    return getFontAtlasFNT(fontFileName, Rect(imageOffset.x, imageOffset.y, 0, 0), false);
}

FontAtlas* FontAtlasCache::getFontAtlasCharMap(std::string_view plistFile)
{
    std::string_view atlasName = plistFile;

    auto it = _atlasMap.find(atlasName);
    if (it == _atlasMap.end())
    {
        auto font = FontCharMap::create(plistFile);

        if (font)
        {
            auto tempAtlas = font->newFontAtlas();
            if (tempAtlas)
            {
                hlookup::set_item(_atlasMap, atlasName, tempAtlas);  // _atlasMap[atlasName] = tempAtlas;
                return tempAtlas;
            }
        }
    }
    else
        return it->second;

    return nullptr;
}

FontAtlas* FontAtlasCache::getFontAtlasCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap)
{
    char key[ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE];
    snprintf(key, sizeof(key), "name:%p_%d_%d_%d", texture->getBackendTexture(), itemWidth, itemHeight, startCharMap);
    std::string atlasName = key;

    auto it = _atlasMap.find(atlasName);
    if (it == _atlasMap.end())
    {
        auto font = FontCharMap::create(texture, itemWidth, itemHeight, startCharMap);

        if (font)
        {
            auto tempAtlas = font->newFontAtlas();
            if (tempAtlas)
            {
                _atlasMap[atlasName] = tempAtlas;
                return _atlasMap[atlasName];
            }
        }
    }
    else
        return it->second;

    return nullptr;
}

FontAtlas* FontAtlasCache::getFontAtlasCharMap(std::string_view charMapFile,
                                               int itemWidth,
                                               int itemHeight,
                                               int startCharMap)
{
    char keyPrefix[ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE];
    snprintf(keyPrefix, ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE, "%d %d %d ", itemWidth, itemHeight, startCharMap);
    std::string atlasName(keyPrefix);
    atlasName += charMapFile;

    auto it = _atlasMap.find(atlasName);
    if (it == _atlasMap.end())
    {
        auto font = FontCharMap::create(charMapFile, itemWidth, itemHeight, startCharMap);

        if (font)
        {
            auto tempAtlas = font->newFontAtlas();
            if (tempAtlas)
            {
                _atlasMap[atlasName] = tempAtlas;
                return _atlasMap[atlasName];
            }
        }
    }
    else
        return it->second;

    return nullptr;
}

bool FontAtlasCache::releaseFontAtlas(FontAtlas* atlas)
{
    if (nullptr != atlas)
    {
        if (atlas->getReferenceCount() == 1)
        {
            for (auto&& item : _atlasMap)
            {
                if (item.second == atlas)
                {
                    _atlasMap.erase(item.first);
                    break;
                }
            }
        }
        atlas->release();
        return true;
    }

    return false;
}

void FontAtlasCache::reloadFontAtlasFNT(std::string_view fontFileName, const Rect& imageRect, bool imageRotated)
{
    char keyPrefix[ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE];
    snprintf(keyPrefix, ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE, "%.2f %.2f ", imageRect.origin.x, imageRect.origin.y);
    std::string atlasName(keyPrefix);
    atlasName += fontFileName;

    auto it = _atlasMap.find(atlasName);
    if (it != _atlasMap.end())
    {
        AX_SAFE_RELEASE_NULL(it->second);
        _atlasMap.erase(it);
    }
    FontFNT::reloadBMFontResource(fontFileName);
    auto font = FontFNT::create(fontFileName, imageRect, imageRotated);
    if (font)
    {
        auto tempAtlas = font->newFontAtlas();
        if (tempAtlas)
        {
            _atlasMap[atlasName] = tempAtlas;
        }
    }
}

void FontAtlasCache::reloadFontAtlasFNT(std::string_view fontFileName, const Vec2& imageOffset)
{
    reloadFontAtlasFNT(fontFileName, Rect(imageOffset.x, imageOffset.y, 0, 0), false);
}

void FontAtlasCache::unloadFontAtlasTTF(std::string_view fontFileName)
{
    auto item = _atlasMap.begin();
    while (item != _atlasMap.end())
    {
        if (item->first.find(fontFileName) != std::string::npos)
        {
            AX_SAFE_RELEASE_NULL(item->second);
            item = _atlasMap.erase(item);
        }
        else
            item++;
    }
}

NS_AX_END
