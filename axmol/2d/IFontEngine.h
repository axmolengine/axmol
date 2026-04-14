/****************************************************************************
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
