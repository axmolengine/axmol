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
#pragma once

#include "axmol/2d/Font.h"
#include "axmol/2d/IFontEngine.h"
#include <string>

namespace ax
{

struct GlyphSize
{
    int width{0};
    int height{0};
};

struct GlyphMetrics
{
    float bboxWidth{0};
    float bboxHeight{0};
    float horiBearingX{0};
    float horiBearingY{0};
    int xAdvance{0};
};

/**
 * @addtogroup _2d
 * @{
 */

/** @class FontFreeType
 * @brief FontFreeType is a class of rendering ttf/ttc characters
 */
class AX_DLL FontFreeType : public Font
{
public:
    static const int DistanceMapSpread;
    static constexpr int DEFAULT_BASE_FONT_SIZE = 32;

    /**
     * Set font engine for ttf fallback render support
     * @since axmol-2.1.3
     */
    static void setFontEngine(IFontEngine*);

    /**
     * @brief Enable or disable stream-based font parsing in FontFreeType.
     *
     * Controls whether FontFreeType uses stream parsing when loading font files.
     * - When enabled (`true`), font data is parsed incrementally as it is read,
     *   reducing peak memory usage for very large font files.
     * - When disabled (`false`), the entire font file is loaded into memory
     *   before parsing, which is simpler but may cause high memory consumption.
     *
     * @param bEnabled Pass `true` to enable stream parsing, or `false` to disable it.
     *
     * @note Stream parsing is recommended for large font files to avoid keeping
     *       the full file content resident in memory. For small fonts, disabling
     *       may simplify debugging and slightly improve performance.
     *
     * @warning To enable stream parsing, ensure that font files are not compressed
     *          in your application package. For example, on Android:
     *          1. Disable `.ttf` compression in the APK (see:
     *             https://learn.simdsoft.com/notes/#build-apk-config-nocompress-file-type-at-appbuildgradle)
     *          2. Or manually uncompress `.ttf` files to disk before use.
     * @default By default, `_streamParsingEnabled` is set to `false`.
     */
    static void setStreamParsingEnabled(bool bEnabled) { _streamParsingEnabled = bEnabled; }
    static bool isStreamParsingEnabled() { return _streamParsingEnabled; }

    /**
     * @brief Set the Missing Glyph Character, by default render square for missing characters
     *
     * @param charCode
     */
    static void setMissingGlyphCharacter(char32_t charCode) { _mssingGlyphCharacter = charCode; };

    /**
     * @brief Whether enable SDF text rendering globally, by default: disabled, since axmol-2.1.0
     *
     * @param enabled
     */
    static void setGlobalSDFEnabled(bool enabled) { _globalSDFEnabled = enabled; }
    static bool isGlobalSDFEnabled() { return _globalSDFEnabled; }

    AX_DEPRECATED(2.9.2) static void setShareDistanceFieldEnabled(bool enabled) { setGlobalSDFEnabled(enabled); }
    AX_DEPRECATED(2.9.2) static bool isShareDistanceFieldEnabled() { return isGlobalSDFEnabled(); }

    /**
     * @brief TrueType fonts with native bytecode hinting * *
     *
     *   All applications that handle TrueType fonts with native hinting must
     *   be aware that TTFs expect different rounding of vertical font
     *   dimensions.  The application has to cater for this, especially if it
     *   wants to rely on a TTF's vertical data (for example, to properly align
     *   box characters vertically).
     *   - Since freetype-2.8.1 TureType matrics isn't sync to size_matrics
     *   - By default it's enabled for compatible with cocos2d-x-4.0 or older with freetype-2.5.5
     *   - Please see freetype.h
     */
    static void setNativeBytecodeHintingEnabled(bool bEnabled) { _doNativeBytecodeHinting = bEnabled; }
    static bool isNativeBytecodeHintingEnabled() { return _doNativeBytecodeHinting; }

    static FontFreeType* create(std::string_view fontPath,
                                int faceSize,
                                GlyphCollection glyphs,
                                std::string_view customGlyphs,
                                bool distanceFieldEnabled = false,
                                float outline             = 0);

    /**
     * create fallback font with font face info & mainFont
     */
    static FontFreeType* createFallbackFont(const GlyphResolution& glyphResolution, FontFreeType* mainFont);

    static void shutdownFreeType();

    bool isDistanceFieldEnabled() const { return _distanceFieldEnabled; }

    float getOutlineSize() const { return _outlineSize; }

    void renderCharAt(uint8_t* dest,
                      int posX,
                      int posY,
                      uint8_t* bitmap,
                      int bitmapWidth,
                      int bitmapHeight,
                      int atlasWidth,
                      int atlasHeight);

    int* getHorizontalKerningForTextUTF32(const std::u32string& text, int& outNumLetters) const override;

    uint8_t* getGlyphBitmap(char32_t charCode,
                            GlyphSize& glyphSize,
                            GlyphMetrics& glyphMetrics,
                            const GlyphResolution*& fallbackRes,
                            bool& sharedBitmapData);

    uint8_t* getGlyphBitmapByIndex(unsigned int glyphIndex,
                                   GlyphSize& glyphSize,
                                   GlyphMetrics& glyphMetrics,
                                   bool& sharedBitmapData);

    int getFontAscender() const;
    const char* getFontFamily() const;
    std::string_view getFontName() const { return _fontName; }

    FontAtlas* newFontAtlas() override;
    int getFontMaxHeight() const override { return _lineHeight; }

    std::string_view getGlyphCollection() const;

    static void releaseFont(std::string_view fontName);

    static FT_Library getFTLibrary();

private:
    static FT_Library _FTlibrary;
    static bool _FTInitialized;
    static bool _streamParsingEnabled;
    static bool _doNativeBytecodeHinting;
    static bool _globalSDFEnabled;
    static char32_t _mssingGlyphCharacter;
    static IFontEngine* _fontEngine;

    static bool initFreeType();

    FontFreeType(std::string_view fontName, int faceSize, bool distanceFieldEnabled = false, float outline = 0);
    virtual ~FontFreeType();

    bool init();
    bool initWithFace(FT_Face face);

    int getHorizontalKerningForChars(uint64_t firstChar, uint64_t secondChar) const;
    uint8_t* getGlyphBitmapWithOutline(unsigned int glyphIndex, FT_BBox& bbox);

    void setGlyphCollection(GlyphCollection glyphs, std::string_view customGlyphs);

    FT_Face _ftFace;
    FT_Size _ftSize;
    FT_Stroker _ftStroker;
    FT_Stream _ftStream;

    std::string _fontName;  // font family or path
    int _faceSize;
    bool _distanceFieldEnabled;
    bool _ownFontFace;
    float _outlineSize;
    int _ascender;
    int _descender;
    int _lineHeight;
    unsigned int _styleFlags{0};  // the freetype font style flags

    GlyphCollection _usedGlyphs;
    std::string _customGlyphs;
};

// end of _2d group
/// @}

}  // namespace ax
