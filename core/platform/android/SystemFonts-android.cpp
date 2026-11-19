/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "platform/android/SystemFonts-android.h"

#include "platform/SystemFontRequest.h"
#include "platform/SystemFontResult.h"

#include "2d/FontFreeType.h"

#include "base/UTF8.h"

#include <algorithm>
#include <filesystem>

#include "ft2build.h"
#include FT_FREETYPE_H

struct ax::SystemFonts::CacheEntry
{
    std::string path;
    std::string family;
    bool bold;
    bool italic;
    bool color;
};

ax::SystemFonts& ax::SystemFonts::getInstance()
{
    static SystemFonts result;
    return result;
}

ax::SystemFonts::SystemFonts()
{
    // Android API level 29 has an API to retrieve system fonts but it does not
    // allow to filter on other criteria than the font family and the available
    // glyphs. We use an ad-hoc solution here to customize our filters and to
    // support older versions of Android.
    const FT_Library freetype = FontFreeType::getFTLibrary();

    for (const std::filesystem::directory_entry& e : std::filesystem::recursive_directory_iterator("/system/fonts"))
    {
        std::string path = e.path().string();
        FT_Face face;

        if (FT_New_Face(freetype, path.c_str(), 0, &face))
            continue;

        const bool color = FT_HAS_COLOR(face);

        if (color && FT_IS_SCALABLE(face))
            // Scalable colored fonts are not supported yet. They require an SVG
            // library to render the glyphs.
            continue;

        _cache.emplace_back(CacheEntry{std::move(path), face->family_name, bool(face->style_flags & FT_STYLE_FLAG_BOLD),
                                       bool(face->style_flags & FT_STYLE_FLAG_ITALIC), color});

        FT_Done_Face(face);
    }
}

ax::SystemFonts::~SystemFonts() = default;

ax::SystemFontResult ax::SystemFonts::findFont(const SystemFontRequest& request)
{
    const FT_Library freetype = FontFreeType::getFTLibrary();

    const size_t cacheSize = _cache.size();
    size_t selected        = cacheSize;
    int bestCoverageScore  = 0;
    int bestStyleScore     = 0;

    for (size_t i = 0; i != cacheSize; ++i)
    {
        const int styleScore = (request.family.empty() || (_cache[i].family == request.family)) +
                               (request.bold == _cache[i].bold) + (request.italic == _cache[i].italic) +
                               (request.color == _cache[i].color);

        // If we already have full coverage and the style does not improve, no
        // need to check the current font.
        if ((styleScore < bestStyleScore) && (bestCoverageScore == request.chars.size()))
            continue;

        FT_Face face;

        if (FT_New_Face(freetype, _cache[i].path.c_str(), 0, &face))
            continue;

        int coverageScore = 0;
        for (char32_t c : request.chars)
            coverageScore += FT_Get_Char_Index(face, (FT_ULong)c) != 0;

        if (coverageScore < bestCoverageScore)
            continue;

        FT_Done_Face(face);

        if ((coverageScore > bestCoverageScore) ||
            ((coverageScore == bestCoverageScore) && (styleScore > bestStyleScore)))
        {
            bestCoverageScore = coverageScore;
            bestStyleScore    = styleScore;
            selected          = i;

            if ((styleScore == 4) && (coverageScore == request.chars.size()))
                break;
        }
    }

    if (selected == cacheSize)
        return {};

    return {_cache[selected].path, _cache[selected].bold, _cache[selected].italic, _cache[selected].color};
}
