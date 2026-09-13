/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include <string_view>
#include <limits>

/* freetype fwd decls */

typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_StreamRec_* FT_Stream;
typedef struct FT_FaceRec_* FT_Face;
typedef struct FT_StrokerRec_* FT_Stroker;
typedef struct FT_BBox_ FT_BBox;
typedef struct FT_SizeRec_* FT_Size;

namespace ax
{

/**
 * @addtogroup _2d
 * @{
 */
struct GlyphResolution
{
    static constexpr unsigned InvalidGlyph = std::numeric_limits<unsigned>::max();

    unsigned glyphIndex{InvalidGlyph};
    std::string_view family;  // weak reference to face->family_name
    FT_Face face{nullptr};
};

class IFontEngine
{
public:
    /*
     * @param charCode the UTF-32 unicode codepoint
     * @param style    the FreeType face->style_flags
     * @return Pointer to GlyphResolution if the glyph was successfully resolved;
     *         nullptr if resolution failed (e.g. glyph not found in any font/fallback).
     */
    virtual const GlyphResolution* resolveGlyph(char32_t charCode, unsigned int style) const = 0;
};

}  // namespace ax
