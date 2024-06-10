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

#ifndef _CCFontAtlas_h_
#define _CCFontAtlas_h_

/// @cond DO_NOT_SHOW

#include <string>
#include <unordered_map>

#include "platform/PlatformMacros.h"
#include "base/Object.h"
#include "platform/StdC.h"  // ssize_t on windows
#include "renderer/Texture2D.h"

#include "base/Map.h"
#include "2d/FontFreeType.h"

NS_AX_BEGIN

class Font;
class EventCustom;
class EventListenerCustom;
class FontFreeType;

struct FontLetterDefinition
{
    float U;
    float V;
    float width;
    float height;
    float offsetX;
    float offsetY;
    int textureID;
    bool validDefinition;
    int xAdvance;
    bool rotated;
};

class AX_DLL FontAtlas : public Object
{
public:
    static const int CacheTextureWidth;
    static const int CacheTextureHeight;
    static const char* CMD_PURGE_FONTATLAS;
    static const char* CMD_RESET_FONTATLAS;
    static void loadFontAtlas(std::string_view fontatlasFile, hlookup::string_map<FontAtlas*>& outAtlasMap);
    /**
     * @js ctor
     */
    FontAtlas(Font* theFont);
    FontAtlas(Font* theFont, int atlasWidth, int atlasHeight, float scaleFactor = 1.0f);
    /**
     * @js NA
     * @lua NA
     */
    virtual ~FontAtlas();

    void addLetterDefinition(char32_t utf32Char, const FontLetterDefinition& letterDefinition);
    bool getLetterDefinitionForChar(char32_t utf32Char, FontLetterDefinition& letterDefinition);

    bool prepareLetterDefinitions(const std::u32string& utf16String);

    const auto& getLetterDefinitions() const { return _letterDefinitions; }

    const std::unordered_map<unsigned int, Texture2D*>& getTextures() const { return _atlasTextures; }

    virtual void addNewPage();

    void addNewPageWithData(const uint8_t* data, size_t size);

    void setTexture(unsigned int slot, Texture2D* texture);
    Texture2D* getTexture(int slot);

    float getLineHeight() const { return _lineHeight; }
    void setLineHeight(float newHeight);

    std::string_view getFontName() const;

    const Font* getFont() const { return _font; }

    /** listen the event that renderer was recreated on Android/WP8
     It only has effect on Android and WP8.
     */
    void listenRendererRecreated(EventCustom* event);

    /** Removes textures atlas.
     It will purge the textures atlas and if multiple texture exist in the FontAtlas.
     */
    void purgeTexturesAtlas();

    /** sets font texture parameters:
     - GL_TEXTURE_MIN_FILTER = GL_LINEAR
     - GL_TEXTURE_MAG_FILTER = GL_LINEAR
     */
    void setAntiAliasTexParameters();

    /** sets font texture parameters:
    - GL_TEXTURE_MIN_FILTER = GL_NEAREST
    - GL_TEXTURE_MAG_FILTER = GL_NEAREST
    */
    void setAliasTexParameters();

protected:
    void initWithSettings(void* opaque /*simdjson::ondemand::document*/);

    void reset();

    void reinit();

    void releaseTextures();

    void findNewCharacters(const std::u32string& u32Text, std::unordered_set<char32_t>& charCodeSet);

    /**
     * Scale each font letter by scaleFactor.
     *
     * @param scaleFactor A float scale factor for scaling font letter info.
     */
    void scaleFontLetterDefinition(float scaleFactor);

    void updateTextureContent(backend::PixelFormat format, int startY);

    std::unordered_map<unsigned int, Texture2D*> _atlasTextures;
    std::unordered_map<char32_t, FontLetterDefinition> _letterDefinitions;

    StringMap<FontFreeType*> _missingFallbackFonts; // maybe style no needs?
    std::unordered_map<char32_t, std::pair<FontFreeType*, unsigned int>> _missingGlyphFallbackFonts;

    Font* _font                 = nullptr;
    FontFreeType* _fontFreeType = nullptr;

    int _width         = 0;  // atlas width
    int _height        = 0;  // atlas height
    float _scaleFactor = 1.0f;

    float _lineHeight = 0.f;

    // Dynamic GlyphCollection related stuff
    int _currentPage                  = -1;
    backend::PixelFormat _pixelFormat = backend::PixelFormat::NONE;
    int _strideShift                  = 0;
    uint8_t* _currentPageData         = nullptr;
    int _currentPageDataSize          = 0;

    float _currentPageOrigX = 0;
    float _currentPageOrigY = 0;
    int _letterPadding      = 0;
    int _letterEdgeExtend   = 0;

    int _fontAscender                               = 0;
    EventListenerCustom* _rendererRecreatedListener = nullptr;
    bool _antialiasEnabled                          = true;
    int _currLineHeight                             = 0;

    friend class Label;
};

NS_AX_END

/// @endcond
#endif /* defined(__cocos2d_libs__CCFontAtlas__) */
