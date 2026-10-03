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

#include "2d/FontAtlas.h"
#if AX_TARGET_PLATFORM != AX_PLATFORM_WIN32 && AX_TARGET_PLATFORM != AX_PLATFORM_ANDROID
#elif AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
#    include "platform/android/jni/Java_dev_axmol_lib_AxmolEngine.h"
#endif
#include <algorithm>
#include "2d/FontFreeType.h"
#include "2d/FontAtlasCache.h"
#include "2d/Label.h"
#include "base/UTF8.h"
#include "base/Director.h"
#include "base/EventListenerCustom.h"
#include "base/EventDispatcher.h"
#include "base/EventType.h"

#include "simdjson/simdjson.h"
#include "zlib.h"
#include "base/ZipUtils.h"

#include "base/PaddedString.h"

namespace ax
{

const int FontAtlas::CacheTextureWidth     = 512;
const int FontAtlas::CacheTextureHeight    = 512;
const char* FontAtlas::CMD_PURGE_FONTATLAS = "__ax_PURGE_FONTATLAS";
const char* FontAtlas::CMD_RESET_FONTATLAS = "__ax_RESET_FONTATLAS";

void FontAtlas::loadFontAtlas(std::string_view fontatlasFile, hlookup::string_map<FontAtlas*>& outAtlasMap)
{
    using namespace simdjson;

    try
    {
        auto strJson = PaddedString::load(fontatlasFile);
        ondemand::parser parser;
        ondemand::document settings = parser.iterate(strJson);
        std::string_view type       = settings["type"];
        if (type != "fontatlas")
        {
            AXLOGE("Load fontatlas {} fail, invalid asset type: {}", fontatlasFile, type);
            return;
        }

        // std::string_view version   = settings["version"];
        std::string_view atlasName = settings["atlasName"];

        auto it = outAtlasMap.find(atlasName);
        if (it != outAtlasMap.end())
        {
            if (it->second->getReferenceCount() != 1)
            {
                AXLOGE("Load fontatlas {} fail, due to exist fontatlas with same key {} and in used", fontatlasFile,
                       atlasName);
                return;
            }
            else
                it->second->release();
            outAtlasMap.erase(it);
        }

        std::string_view sourceFont = settings["sourceFont"];
        int faceSize                = static_cast<int>(static_cast<int64_t>(settings["faceSize"]));
        auto font = FontFreeType::create(sourceFont, faceSize, GlyphCollection::DYNAMIC, ""sv, true, 0.0f);
        if (!font)
        {
            AXLOGE("Load fontatils {} fail due to create source font {} fail", fontatlasFile, sourceFont);
            return;
        }

        int atlasDim[2];

        auto atliasDim = settings["atlasDim"].get_array();
        int index      = 0;
        for (auto value : atliasDim)
        {
            atlasDim[index++] = static_cast<int>(value.get_int64());
            if (index >= 2)
                break;
        }

        auto fontAtlas = new FontAtlas(font, atlasDim[0], atlasDim[1], AX_CONTENT_SCALE_FACTOR());

        try
        {
            fontAtlas->initWithSettings(&settings);
            outAtlasMap.emplace(atlasName, fontAtlas);
        }
        catch (std::exception& ex)
        {
            fontAtlas->release();
            throw ex;  // rethrow
        }
    }
    catch (std::exception& ex)
    {
        AXLOGE("Load fontatils {} fail due to exception occured: {}", fontatlasFile, ex.what());
    }
}

FontAtlas::FontAtlas(Font* theFont)
    : FontAtlas(theFont, CacheTextureWidth, CacheTextureHeight, AX_CONTENT_SCALE_FACTOR())
{}

FontAtlas::FontAtlas(Font* theFont, int atlasWidth, int atlasHeight, float scaleFactor)
    : _font(theFont), _width(atlasWidth), _height(atlasHeight), _scaleFactor(scaleFactor)
{
    _font->retain();

    _fontFreeType = dynamic_cast<FontFreeType*>(_font);
    if (_fontFreeType)
    {
        _lineHeight       = (float)_font->getFontMaxHeight();
        _fontAscender     = _fontFreeType->getFontAscender();
        _letterEdgeExtend = 2;

        auto outlineSize = _fontFreeType->getOutlineSize();
        if (_fontFreeType->hasColors())
          {
            _strideShift         = 2;
            _pixelFormat         = backend::PixelFormat::RGBA8;
            _currentPageDataSize = _width * _height << _strideShift;

            // We don't consider outlineSize in the line's height because no
            // outline is drawn for color fonts.
          }
        else if (outlineSize > 0)
        {
            _strideShift         = 1;
            _pixelFormat         = backend::PixelFormat::RG8;
            _currentPageDataSize = _width * _height << _strideShift;

            _lineHeight += 2 * outlineSize;
        }
        else
        {
            _strideShift         = 0;
            _pixelFormat         = backend::PixelFormat::R8;
            _currentPageDataSize = _width * _height;
        }

        if (_fontFreeType->isDistanceFieldEnabled())
        {
            _letterPadding += 2 * FontFreeType::DistanceMapSpread * AX_CONTENT_SCALE_FACTOR();
        }

#if AX_ENABLE_CACHE_TEXTURE_DATA
        auto eventDispatcher = Director::getInstance()->getEventDispatcher();

        _rendererRecreatedListener = EventListenerCustom::create(
            EVENT_RENDERER_RECREATED, AX_CALLBACK_1(FontAtlas::listenRendererRecreated, this));
        eventDispatcher->addEventListenerWithFixedPriority(_rendererRecreatedListener, 1);
#endif
    }
}

void FontAtlas::reinit()
{
    if (!_currentPageData)
        _currentPageData = new uint8_t[_currentPageDataSize];
    _currentPage = -1;

    addNewPage();
}

FontAtlas::~FontAtlas()
{
#if AX_ENABLE_CACHE_TEXTURE_DATA
    if (_fontFreeType && _rendererRecreatedListener)
    {
        auto eventDispatcher = Director::getInstance()->getEventDispatcher();
        eventDispatcher->removeEventListener(_rendererRecreatedListener);
        _rendererRecreatedListener = nullptr;
    }
#endif

    _font->release();
    releaseTextures();

    AX_SAFE_DELETE_ARRAY(_currentPageData);
}

void FontAtlas::initWithSettings(void* opaque /*simdjson::ondemand::document*/)
{
    if (!_currentPageData)
        _currentPageData = new uint8_t[_currentPageDataSize];
    _currentPage = -1;

    simdjson::ondemand::document& settings = *(simdjson::ondemand::document*)opaque;

    // pages
    for (auto page : settings["pages"].get_array())
    {
        auto comprData   = utils::base64Decode(page);
        auto uncomprData = ZipUtils::decompressGZ(std::span{comprData}, _currentPageDataSize);
        addNewPageWithData(uncomprData.data(), uncomprData.size());
    }

    _currentPageOrigX = static_cast<float>(settings["pageX"].get_double());
    _currentPageOrigY = static_cast<float>(settings["pageY"].get_double());

    // letters
    FontLetterDefinition tempDef;
    tempDef.rotated         = false;
    tempDef.validDefinition = true;

    std::string strCharCode;
    for (auto field : settings["letters"].get_object())
    {
        strCharCode       = static_cast<std::string_view>(field.unescaped_key());
        auto letterInfo   = field.value();
        tempDef.U         = static_cast<float>(letterInfo["U"].get_double());
        tempDef.V         = static_cast<float>(letterInfo["V"].get_double());
        tempDef.xAdvance  = static_cast<int>(letterInfo["advance"].get_double());
        tempDef.width     = static_cast<float>(letterInfo["width"].get_double());
        tempDef.height    = static_cast<float>(letterInfo["height"].get_double());
        tempDef.offsetX   = static_cast<float>(letterInfo["offsetX"].get_double());
        tempDef.offsetY   = static_cast<float>(letterInfo["offsetY"].get_double());
        tempDef.textureID = static_cast<int>(letterInfo["page"].get_int64());

        auto charCode = atoi(strCharCode.c_str());

        tempDef.U /= _scaleFactor;
        tempDef.V /= _scaleFactor;
        tempDef.width /= _scaleFactor;
        tempDef.height /= _scaleFactor;
        _letterDefinitions.emplace(charCode, tempDef);
    }
}

void FontAtlas::reset()
{
    releaseTextures();

    _currLineHeight   = 0;
    _currentPageOrigX = 0;
    _currentPageOrigY = 0;
    _letterDefinitions.clear();

    reinit();
}

void FontAtlas::releaseTextures()
{
    for (auto&& item : _atlasTextures)
    {
        item.second->release();
    }
    _atlasTextures.clear();
}

void FontAtlas::purgeTexturesAtlas()
{
    if (_fontFreeType)
    {
        reset();
        auto eventDispatcher = Director::getInstance()->getEventDispatcher();
        eventDispatcher->dispatchCustomEvent(CMD_PURGE_FONTATLAS, this);
        eventDispatcher->dispatchCustomEvent(CMD_RESET_FONTATLAS, this);
    }
}

void FontAtlas::listenRendererRecreated(EventCustom* /*event*/)
{
    purgeTexturesAtlas();
}

void FontAtlas::addLetterDefinition(char32_t utf32Char, const FontLetterDefinition& letterDefinition)
{
    _letterDefinitions[utf32Char] = letterDefinition;
}

void FontAtlas::scaleFontLetterDefinition(float scaleFactor)
{
    for (auto&& fontDefinition : _letterDefinitions)
    {
        auto& letterDefinition = fontDefinition.second;
        letterDefinition.width *= scaleFactor;
        letterDefinition.height *= scaleFactor;
        letterDefinition.offsetX *= scaleFactor;
        letterDefinition.offsetY *= scaleFactor;
        letterDefinition.scale *= scaleFactor;
        letterDefinition.xAdvance = (int)(letterDefinition.xAdvance * scaleFactor);
    }
}

bool FontAtlas::getLetterDefinitionForChar(char32_t utf32Char, FontLetterDefinition& letterDefinition)
{
    auto outIterator = _letterDefinitions.find(utf32Char);

    if (outIterator != _letterDefinitions.end())
    {
        letterDefinition = (*outIterator).second;
        return letterDefinition.validDefinition;
    }
    else
    {
        return false;
    }
}

const FontLetterDefinition& FontAtlas::getLetterDefinition(char32_t utf32Char) const
{
  return _letterDefinitions.find(utf32Char)->second;
}

void FontAtlas::findNewCharacters(const std::u32string& u32Text, std::unordered_set<char32_t>& charset)
{
    if (_letterDefinitions.empty())
    {
        std::copy(u32Text.begin(), u32Text.end(), std::inserter(charset, charset.end()));
    }
    else
    {
        for (auto&& charCode : u32Text)
            if (_letterDefinitions.find(charCode) == _letterDefinitions.end())
                charset.insert(charCode);
    }
}

bool FontAtlas::prepareLetterDefinitions(const std::u32string& utf32Text, IFontEngine* fallback)
{
    if (_fontFreeType == nullptr)
    {
        return false;
    }

    if (!_currentPageData)
        reinit();

    std::unordered_set<char32_t> charCodeSet;
    findNewCharacters(utf32Text, charCodeSet);
    if (charCodeSet.empty())
    {
        return false;
    }

    int adjustForDistanceMap = _letterPadding / 2;
    int adjustForExtend      = _letterEdgeExtend / 2;
    int bitmapWidth          = 0;
    int bitmapHeight         = 0;
    int glyphHeight;
    Rect tempRect;
    FontLetterDefinition tempDef;

    int startY = (int)_currentPageOrigY;

    for (char32_t charCode : charCodeSet)
    {
        if (StringUtils::isVariantSelector(charCode))
        {
            // Variant selectors are not supported yet. As a workaround we
            // accept the letter definiton but give it no dimension.
            tempDef                      = {};
            tempDef.validDefinition      = true;
            _letterDefinitions[charCode] = tempDef;
            continue;
        }

        TTFConfig fallbackFont;
        const FontFreeTypeBitmap bitmap = _fontFreeType->getGlyphBitmap(charCode, bitmapWidth, bitmapHeight, tempRect,
                                                                        tempDef.xAdvance, fallback, &fallbackFont);

        bool valid            = true;

        if (bitmap.bitmap && bitmapWidth > 0 && bitmapHeight > 0)
          {
            startY = renderChar(bitmap, bitmapWidth, bitmapHeight, startY, tempRect, tempDef);
          }
        else
        {
            if (!fallbackFont.fontFilePath.empty())
            {
                // The font does not contain the glyph but has provided another font
                // which would have it.
                FontAtlas* const atlas = FontAtlasCache::getFontAtlasTTF(&fallbackFont);

                if (atlas->getOrCreateLetter(charCode, tempDef))
                {
                    const FontFreeType* const altFont = atlas->_fontFreeType;
                    tempDef.scale = (float)_fontFreeType->getFontMaxHeight() / altFont->getFontMaxHeight();
                    tempDef.xAdvance *= tempDef.scale;

                    Texture2D* const texture = atlas->getTexture(tempDef.textureID);

                    // We must keep the texture in our list as it becomes part
                    // of this atlas' interface.
                    const auto [it, inserted] = _sharedTextures.emplace(texture, _nextSharedTextureID);

                    if (inserted)
                    {
                        texture->retain();
                        _atlasTextures[_nextSharedTextureID] = texture;
                        tempDef.textureID = _nextSharedTextureID;
                        --_nextSharedTextureID;
                    }
                    else
                      tempDef.textureID = it->second;
                }
                else
                    valid = false;
            }
            else
                valid = false;
        }

        if (!valid)
        {
            tempDef.validDefinition = tempDef.xAdvance >= 0;
            tempDef.width           = 0;
            tempDef.height          = 0;
            tempDef.U               = 0;
            tempDef.V               = 0;
            tempDef.offsetX         = 0;
            tempDef.offsetY         = 0;
            tempDef.scale           = 1;
            tempDef.textureID       = 0;
            tempDef.rotated         = false;
            _currentPageOrigX += 1;
        }

        _letterDefinitions[charCode] = tempDef;
    }

    updateTextureContent(startY);

    return true;
}

bool FontAtlas::getOrCreateLetter(char32_t charCode, FontLetterDefinition& letterDefinition)
{
    const auto it = _letterDefinitions.find(charCode);

    if (it != _letterDefinitions.end())
    {
        letterDefinition = it->second;
        return true;
    }

    int bitmapWidth  = 0;
    int bitmapHeight = 0;
    Rect tempRect;

    const FontFreeTypeBitmap bitmap = _fontFreeType->getGlyphBitmap(charCode, bitmapWidth, bitmapHeight, tempRect,
                                                          letterDefinition.xAdvance, nullptr);

    if (!bitmap.bitmap || !bitmapWidth || !bitmapHeight)
        return false;

    if (!_currentPageData)
        reinit();

    const int startY = renderChar(bitmap, bitmapWidth, bitmapHeight, _currentPageOrigY, tempRect, letterDefinition);
    updateTextureContent(startY);

    _letterDefinitions[charCode] = letterDefinition;

    return true;
}

int FontAtlas::renderChar(const FontFreeTypeBitmap& bitmap, int bitmapWidth, int bitmapHeight, int startY, const Rect& rect, FontLetterDefinition& letterDefinition)
{
    const int adjustForDistanceMap = _letterPadding / 2;
    const int adjustForExtend      = _letterEdgeExtend / 2;

    letterDefinition.validDefinition = true;
    letterDefinition.width           = rect.size.width + _letterPadding + _letterEdgeExtend;
    letterDefinition.height          = rect.size.height + _letterPadding + _letterEdgeExtend;
    letterDefinition.offsetX         = rect.origin.x - adjustForDistanceMap - adjustForExtend;
    letterDefinition.offsetY         = _fontAscender + rect.origin.y - adjustForDistanceMap - adjustForExtend;
    letterDefinition.scale = 1;

    if (_currentPageOrigX + letterDefinition.width > _width)
    {
        _currentPageOrigY += _currLineHeight;
        _currLineHeight   = 0;
        _currentPageOrigX = 0;
        if (_currentPageOrigY + _lineHeight + _letterPadding + _letterEdgeExtend >= _height)
        {
            updateTextureContent(startY);

            startY = 0;

            addNewPage();
        }
    }
    int glyphHeight = bitmapHeight + _letterPadding + _letterEdgeExtend;
    _currLineHeight = std::max(glyphHeight, _currLineHeight);
    _fontFreeType->renderCharAt(_currentPageData, (int)_currentPageOrigX + adjustForExtend,
                                (int)_currentPageOrigY + adjustForExtend, bitmap, bitmapWidth, bitmapHeight, _width,
                                _height);

    letterDefinition.U         = _currentPageOrigX;
    letterDefinition.V         = _currentPageOrigY;
    letterDefinition.textureID = _currentPage;
    _currentPageOrigX += letterDefinition.width + 1;
    // take from pixels to points
    letterDefinition.width   = letterDefinition.width / _scaleFactor;
    letterDefinition.height  = letterDefinition.height / _scaleFactor;
    letterDefinition.U       = letterDefinition.U / _scaleFactor;
    letterDefinition.V       = letterDefinition.V / _scaleFactor;
    letterDefinition.rotated = false;

    return startY;
}

void FontAtlas::updateTextureContent(int startY)
{
    auto data = _currentPageData + (_width * (int)startY << _strideShift);
    _atlasTextures[_currentPage]->updateWithSubData(data, 0, startY, _width,
                                                    (std::min)((int)_currentPageOrigY - startY + _currLineHeight, _height));
}

void FontAtlas::addNewPage()
{
    memset(_currentPageData, 0, _currentPageDataSize);
    addNewPageWithData(_currentPageData, _currentPageDataSize);

    _currentPageOrigY = 0;
}

void FontAtlas::addNewPageWithData(const uint8_t* data, size_t size)
{
    assert(_currentPageDataSize == size);

    auto texture = new Texture2D();
    texture->initWithData(data, _currentPageDataSize, _pixelFormat, _width, _height);

    if (_antialiasEnabled)
        texture->setAntiAliasTexParameters();
    else
        texture->setAliasTexParameters();

    setTexture(++_currentPage, texture);
    texture->release();
}

void FontAtlas::setTexture(unsigned int slot, Texture2D* texture)
{
    texture->retain();
    _atlasTextures[slot] = texture;
}

Texture2D* FontAtlas::getTexture(int slot)
{
    return _atlasTextures[slot];
}

void FontAtlas::setLineHeight(float newHeight)
{
    _lineHeight = newHeight;
}

std::string_view FontAtlas::getFontName() const
{
    std::string_view fontName = _fontFreeType ? _fontFreeType->getFontName() : ""sv;
    if (fontName.empty())
        return fontName;
    auto idx = fontName.rfind('/');
    if (idx != std::string::npos)
    {
        return fontName.substr(idx + 1);
    }
    idx = fontName.rfind('\\');
    if (idx != std::string::npos)
    {
        return fontName.substr(idx + 1);
    }
    return fontName;
}

void FontAtlas::setAliasTexParameters()
{
    if (_antialiasEnabled)
    {
        _antialiasEnabled = false;
        for (const auto& tex : _atlasTextures)
        {
            tex.second->setAliasTexParameters();
        }
    }
}

backend::PixelFormat FontAtlas::getPixelFormat() const
{
    return _pixelFormat;
}

void FontAtlas::setAntiAliasTexParameters()
{
    if (!_antialiasEnabled)
    {
        _antialiasEnabled = true;
        for (const auto& tex : _atlasTextures)
        {
            tex.second->setAntiAliasTexParameters();
        }
    }
}

}
