/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "2d/FontFallback.h"

#include "2d/FontFreeType.h"
#include "platform/SystemFonts.h"
#include "platform/SystemFontRequest.h"
#include "platform/SystemFontResult.h"

#include "ft2build.h"
#include FT_FREETYPE_H

static void addFontFace(std::vector<ax::FontFreeType*>& fonts, std::string_view path)
{
    constexpr int face_size = 30;
    constexpr float outline = 0;

    fonts.emplace_back(ax::FontFreeType::create(path, face_size, ax::GlyphCollection::DYNAMIC, "",
                                                ax::FontFreeType::isGlobalSDFEnabled(), outline));
    fonts.back()->retain();
}

static std::string findFont(std::span<ax::FontFreeType* const> fonts,
                            char32_t charCode,
                            std::string_view family,
                            bool bold,
                            bool italic,
                            bool exactOnly)
{
    size_t secondChoice = fonts.size();
    int choiceScore = 0;

    for (size_t i = 0, n = fonts.size(); i != n; ++i)
    {
        const FT_UInt index = fonts[i]->getCharIndex(charCode);

        if (index == 0)
            continue;

        int score = (fonts[i]->getFontFamily() == family)
          + (fonts[i]->isBold() == bold)
          + (fonts[i]->isItalic() == italic);

        if (score > choiceScore)
          {
            choiceScore = score;
            secondChoice = i;

            if (score == 3)
              return std::string(fonts[i]->getFontName());
          }
    }

    if (!exactOnly && (secondChoice < fonts.size()))
        return std::string(fonts[secondChoice]->getFontName());

    return {};
}

ax::FontFallback::FontFallback(std::span<const std::string> fontFiles, bool enableSystemFonts)
    : _paths(fontFiles.begin(), fontFiles.end()), _systemFonts(enableSystemFonts ? &SystemFonts::getInstance() : nullptr)
{
    _fonts.reserve(fontFiles.size());

    for (const std::string& path : _paths)
        addFontFace(_fonts, path);
}

ax::FontFallback::~FontFallback()
{
    for (FontFreeType* p : _fonts)
        p->release();

    for (FontFreeType* p : _cachedSystemFonts)
        p->release();
}

std::string ax::FontFallback::lookupFontFaceForCodepoint(char32_t charCode, std::string_view family, bool bold, bool italic)
{
    // Prefer user-provided fonts, even if the family does not exactly match.
    std::string r = findFont(_fonts, charCode, family, bold, italic, false);

    if (!r.empty())
        return r;

    // If we recycle a previously-found system font we want it to match
    // exactly, because otherwise we should fall back to the system fonts instead.
    r = findFont(_cachedSystemFonts, charCode, family, bold, italic, true);

    if (!r.empty() || !_systemFonts)
        return r;

    const SystemFontRequest request {std::u32string_view(&charCode, 1), family, bold, italic, true};
    const SystemFontResult result = _systemFonts->findFont(request);

    if (!result.path.empty())
      {
        // Store it in the font list to avoid a system scan in the next
        // requests. We first check that it was not already added because since
        // we did an exact search in the cached system fonts above we may have
        // excluded the font only for it to be the best match in system fonts
        // anyway.

        bool newFont = true;

        for (const FontFreeType* f : _cachedSystemFonts)
          if (f->getFontName() == result.path)
            {
              newFont = false;
              break;
            }

        if (newFont)
            addFontFace(_cachedSystemFonts, result.path);
      }

    return result.path;
}
