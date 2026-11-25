/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "platform/linux/SystemFonts-linux.h"

#include "platform/SystemFontRequest.h"
#include "platform/SystemFontResult.h"

#include "base/Logging.h"

#include <fontconfig/fontconfig.h>

static int gSystemFontsInstances = 0;

ax::SystemFonts& ax::SystemFonts::getInstance()
{
    static SystemFonts result;
    return result;
}

ax::SystemFonts::SystemFonts()
{
    if ((gSystemFontsInstances == 0) && !FcInit())
        AXLOGE("Failed to init FontConfig.");

    ++gSystemFontsInstances;
}

ax::SystemFonts::~SystemFonts()
{
    --gSystemFontsInstances;

    if (gSystemFontsInstances == 0)
        FcFini();
}

ax::SystemFontResult ax::SystemFonts::findFont(const SystemFontRequest& request)
{
    FcPattern* const pattern = FcPatternCreate();

    FcCharSet* charSet = nullptr;

    if (!request.chars.empty())
    {
        charSet = FcCharSetCreate();
        for (char32_t c : request.chars)
            FcCharSetAddChar(charSet, c);

        FcPatternAddCharSet(pattern, FC_CHARSET, charSet);
    }

    if (!request.family.empty())
        FcPatternAddString(pattern, FC_FAMILY, (const FcChar8*)request.family.data());

    FcPatternAddBool(pattern, FC_COLOR, request.color ? FcTrue : FcFalse);

    if (request.bold)
    {
        FcRange* const weight = FcRangeCreateDouble(FC_WEIGHT_BOLD, FC_WEIGHT_EXTRABLACK);
        FcPatternAddRange(pattern, FC_WEIGHT, weight);
        FcRangeDestroy(weight);
    }
    else
        FcPatternAddInteger(pattern, FC_WEIGHT, FC_WEIGHT_REGULAR);

    FcPatternAddInteger(pattern, FC_SLANT, request.italic ? FC_SLANT_ITALIC : FC_SLANT_ROMAN);

    FcConfigSubstitute(nullptr, pattern, FcMatchPattern);
    FcDefaultSubstitute(pattern);

    FcResult match_result;
    FcFontSet* const fontSet = FcFontSort(nullptr, pattern, FcTrue, nullptr, &match_result);
    FcPattern* selectedFont  = nullptr;
    int bestCoverageScore    = 0;
    int bestStyleScore       = 0;

    bool selectedIsBold   = false;
    bool selectedIsItalic = false;
    bool selectedIsColor  = false;

    for (std::size_t i = 0; i != fontSet->nfont; ++i)
    {
        FcPattern* const candidate = fontSet->fonts[i];

        FcCharSet* s = nullptr;
        if (FcPatternGetCharSet(candidate, FC_CHARSET, 0, &s) != FcResultMatch)
            continue;

        int coverageScore = 0;

        for (char32_t c : request.chars)
            coverageScore += FcCharSetHasChar(s, c);

        if (coverageScore < bestCoverageScore)
            continue;

        int weight = 0;
        FcPatternGetInteger(candidate, FC_WEIGHT, 0, &weight);
        const bool isBold = weight > FC_WEIGHT_EXTRABOLD;

        int slant = 0;
        FcPatternGetInteger(candidate, FC_SLANT, 0, &slant);
        const bool isItalic = slant == FC_SLANT_ITALIC;

        FcBool isColor = FcFalse;
        FcPatternGetBool(candidate, FC_COLOR, 0, &isColor);

        FcChar8* family = nullptr;
        FcPatternGetString(candidate, FC_FAMILY, 0, &family);

        const int styleScore = (request.family.empty() || (family && ((const char*)family == request.family))) +
                               (request.bold == isBold) + (request.italic == isItalic) + (request.color == isColor);

        if ((coverageScore > bestCoverageScore) ||
            ((coverageScore == bestCoverageScore) && (styleScore > bestStyleScore)))
        {
            bestCoverageScore = coverageScore;
            bestStyleScore    = styleScore;
            selectedFont      = candidate;

            selectedIsBold   = isBold;
            selectedIsItalic = isItalic;
            selectedIsColor  = isColor;

            if ((styleScore == 4) && (coverageScore == request.chars.size()))
                break;
        }
    }

    std::string path;

    if (selectedFont)
    {
        FcChar8* p;

        if (FcPatternGetString(selectedFont, FC_FILE, 0, &p) == FcResultMatch)
            path = (char*)p;
    }

    FcFontSetDestroy(fontSet);
    FcPatternDestroy(pattern);

    if (charSet)
        FcCharSetDestroy(charSet);

    return {path, selectedIsBold, selectedIsItalic, selectedIsColor};
}

std::string ax::SystemFonts::fontForFamily(std::string_view family)
{
    FcPattern* const pattern = FcPatternCreate();
    FcPatternAddString(pattern, FC_FAMILY, (const FcChar8*)family.data());

    FcConfigSubstitute(nullptr, pattern, FcMatchPattern);
    FcDefaultSubstitute(pattern);

    FcResult match_result;
    FcPattern* font = FcFontMatch(0, pattern, &match_result);

    std::string path;

    if (font)
    {
        FcChar8* p;

        if (FcPatternGetString(font, FC_FILE, 0, &p) == FcResultMatch)
            path = (char*)p;
    }

    FcPatternDestroy(font);
    FcPatternDestroy(pattern);

    return path;
}
