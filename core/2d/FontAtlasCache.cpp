/****************************************************************************
 Copyright (c) 2013      Zynga Inc.
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
#include "2d/FontAtlasCache.h"

#include "base/Director.h"
#include "2d/FontFNT.h"
#include "2d/FontFreeType.h"
#include "2d/FontAtlas.h"
#include "2d/FontCharMap.h"
#include "2d/Label.h"
#include "platform/FileUtils.h"
#include "base/format.h"

namespace ax
{

hlookup::string_map<FontAtlas*> FontAtlasCache::_atlasMap;

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

void FontAtlasCache::preloadFontAtlas(std::string_view fontatlasFile)
{
    FontAtlas::loadFontAtlas(fontatlasFile, _atlasMap);
}

FontAtlas* FontAtlasCache::getFontAtlasTTF(_ttfConfig* config)
{
    auto& realFontFilename = config->fontFilePath;
    bool useDistanceField  = config->distanceFieldEnabled;
    int outlineSize        = useDistanceField ? 0 : config->outlineSize;

    // underlaying font engine (freetype2) only support int type, so convert to int avoid precision issue
    if (!config->distanceFieldEnabled)
        config->faceSize = static_cast<int>(config->fontSize);

    auto scaledFaceSize = static_cast<int>(config->faceSize * AX_CONTENT_SCALE_FACTOR());

    std::string atlasName =
        config->distanceFieldEnabled
                                ? fmt::format("df {} {}", scaledFaceSize, realFontFilename)
                                : fmt::format("{} {} {}", scaledFaceSize, outlineSize, realFontFilename);
    auto it = _atlasMap.find(atlasName);

    if (it == _atlasMap.end())
    {
        auto font = FontFreeType::create(realFontFilename, scaledFaceSize, config->glyphs, config->customGlyphs,
                                         useDistanceField, static_cast<float>(outlineSize));
        if (font)
        {
            auto tempAtlas = font->newFontAtlas();
            if (tempAtlas)
                return _atlasMap.emplace(std::move(atlasName), tempAtlas).first->second;
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
    const auto& realFontFilename = fontFileName;
    auto atlasName               = fmt::format("{} {}", subTextureKey, realFontFilename);
    const auto it                = _atlasMap.find(atlasName);
    if (it == _atlasMap.end())
    {
        const auto font = FontFNT::create(realFontFilename, subTextureKey);

        if (font)
        {
            const auto tempAtlas = font->newFontAtlas();
            if (tempAtlas)
                return _atlasMap.emplace(std::move(atlasName), tempAtlas).first->second;
        }
    }
    else
        return it->second;

    return nullptr;
}

FontAtlas* FontAtlasCache::getFontAtlasFNT(std::string_view fontFileName, const Rect& imageRect, bool imageRotated)
{
    // resolves real file path, to prevent storing multiple atlases for the same file.
    const auto& realFontFilename = fontFileName;

    auto atlasName = fmt::format("{:.2f} {:.2f} {}", imageRect.origin.x, imageRect.origin.y, realFontFilename);

    const auto it = _atlasMap.find(atlasName);
    if (it == _atlasMap.end())
    {
        const auto font = FontFNT::create(realFontFilename, imageRect, imageRotated);

        if (font)
        {
            const auto tempAtlas = font->newFontAtlas();
            if (tempAtlas)
                return _atlasMap.emplace(std::move(atlasName), tempAtlas).first->second;
        }
    }
    else
        return it->second;

    return nullptr;
}
#ifndef AX_CORE_PROFILE
FontAtlas* FontAtlasCache::getFontAtlasFNT(std::string_view fontFileName, const Vec2& imageOffset)
{
    return getFontAtlasFNT(fontFileName, Rect(imageOffset.x, imageOffset.y, 0, 0), false);
}
#endif
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
                return _atlasMap.emplace(atlasName, tempAtlas).first->second;
        }
    }
    else
        return it->second;

    return nullptr;
}

FontAtlas* FontAtlasCache::getFontAtlasCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap)
{
    auto atlasName = fmt::format("name:{}_{}_{}_{}", reinterpret_cast<uintptr_t>(texture->getBackendTexture()), itemWidth, itemHeight, startCharMap);

    auto it = _atlasMap.find(atlasName);
    if (it == _atlasMap.end())
    {
        auto font = FontCharMap::create(texture, itemWidth, itemHeight, startCharMap);

        if (font)
        {
            auto tempAtlas = font->newFontAtlas();
            if (tempAtlas)
                return _atlasMap.emplace(std::move(atlasName), tempAtlas).first->second;
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
    auto atlasName = fmt::format("{} {} {} {}", itemWidth, itemHeight, startCharMap, charMapFile);

    auto it = _atlasMap.find(atlasName);
    if (it == _atlasMap.end())
    {
        auto font = FontCharMap::create(charMapFile, itemWidth, itemHeight, startCharMap);

        if (font)
        {
            auto tempAtlas = font->newFontAtlas();
            if (tempAtlas)
                return _atlasMap.emplace(std::move(atlasName), tempAtlas).first->second;
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
    auto atlasName = fmt::format("{:.2f} {:.2f} {}", imageRect.origin.x, imageRect.origin.y, fontFileName);

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
            _atlasMap.emplace(std::move(atlasName), tempAtlas);
    }
}
#ifndef AX_CORE_PROFILE
void FontAtlasCache::reloadFontAtlasFNT(std::string_view fontFileName, const Vec2& imageOffset)
{
    reloadFontAtlasFNT(fontFileName, Rect(imageOffset.x, imageOffset.y, 0, 0), false);
}
#endif

void FontAtlasCache::unloadFontAtlasTTF(std::string_view fontFileName)
{
    for (auto iter = _atlasMap.begin(); iter != _atlasMap.end();)
    {
        if (iter->first.find(fontFileName) != std::string::npos)
        {
            AX_SAFE_RELEASE_NULL(iter->second);
            iter = _atlasMap.erase(iter);
            continue;
        }
        ++iter;
    }
}

}
