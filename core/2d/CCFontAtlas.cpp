/****************************************************************************
 Copyright (c) 2013      Zynga Inc.
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2021-2022 Bytedance Inc.

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

#include "2d/CCFontAtlas.h"
#if AX_TARGET_PLATFORM != AX_PLATFORM_WIN32 && AX_TARGET_PLATFORM != AX_PLATFORM_ANDROID
#elif AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
#    include "platform/android/jni/Java_org_axmol_lib_AxmolEngine.h"
#endif
#include <algorithm>
#include "2d/CCFontFreeType.h"
#include "base/ccUTF8.h"
#include "base/CCDirector.h"
#include "base/CCEventListenerCustom.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventType.h"

NS_AX_BEGIN

const int FontAtlas::CacheTextureWidth     = 512;
const int FontAtlas::CacheTextureHeight    = 512;
const char* FontAtlas::CMD_PURGE_FONTATLAS = "__cc_PURGE_FONTATLAS";
const char* FontAtlas::CMD_RESET_FONTATLAS = "__cc_RESET_FONTATLAS";

FontAtlas::FontAtlas(Font* theFont) : _font(theFont)
{
    _font->retain();

    _fontFreeType = dynamic_cast<FontFreeType*>(_font);
    if (_fontFreeType)
    {
        _lineHeight       = (float)_font->getFontMaxHeight();
        _fontAscender     = _fontFreeType->getFontAscender();
        _letterEdgeExtend = 2;

        auto outlineSize = _fontFreeType->getOutlineSize();
        if (outlineSize > 0)
        {
            _strideShift         = 1;
            _pixelFormat         = backend::PixelFormat::LA8;
            _currentPageDataSize = CacheTextureWidth * CacheTextureHeight << _strideShift;

#if defined(AX_USE_METAL)
            _currentPageDataSizeRGBA = CacheTextureWidth * CacheTextureHeight * 4;
#endif

            _lineHeight += 2 * outlineSize;
        }
        else
        {
            _strideShift         = 0;
            _pixelFormat         = backend::PixelFormat::A8;
            _currentPageDataSize = CacheTextureWidth * CacheTextureHeight;
        }

        if (_fontFreeType->isDistanceFieldEnabled())
        {
            _letterPadding += 2 * FontFreeType::DistanceMapSpread;
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

#if defined(AX_USE_METAL)
    if (_strideShift && !_currentPageDataRGBA)
        _currentPageDataRGBA = new uint8_t[_currentPageDataSizeRGBA];
#endif

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
#if defined(AX_USE_METAL)
    AX_SAFE_DELETE_ARRAY(_currentPageDataRGBA);
#endif
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

bool FontAtlas::prepareLetterDefinitions(const std::u32string& utf32Text)
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

    auto scaleFactor = AX_CONTENT_SCALE_FACTOR();
    auto pixelFormat = _pixelFormat;

    int startY = (int)_currentPageOrigY;

    for (auto&& charCode : charCodeSet)
    {
        auto bitmap = _fontFreeType->getGlyphBitmap(charCode, bitmapWidth, bitmapHeight, tempRect, tempDef.xAdvance);
        if (bitmap && bitmapWidth > 0 && bitmapHeight > 0)
        {
            tempDef.validDefinition = true;
            tempDef.width           = tempRect.size.width + _letterPadding + _letterEdgeExtend;
            tempDef.height          = tempRect.size.height + _letterPadding + _letterEdgeExtend;
            tempDef.offsetX         = tempRect.origin.x - adjustForDistanceMap - adjustForExtend;
            tempDef.offsetY         = _fontAscender + tempRect.origin.y - adjustForDistanceMap - adjustForExtend;

            if (_currentPageOrigX + tempDef.width > CacheTextureWidth)
            {
                _currentPageOrigY += _currLineHeight;
                _currLineHeight   = 0;
                _currentPageOrigX = 0;
                if (_currentPageOrigY + _lineHeight + _letterPadding + _letterEdgeExtend >= CacheTextureHeight)
                {
                    updateTextureContent(pixelFormat, startY);

                    startY = 0;
                    addNewPage();
                }
            }
            glyphHeight = static_cast<int>(bitmapHeight) + _letterPadding + _letterEdgeExtend;
            if (glyphHeight > _currLineHeight)
            {
                _currLineHeight = glyphHeight;
            }
            _fontFreeType->renderCharAt(_currentPageData, (int)_currentPageOrigX + adjustForExtend,
                                        (int)_currentPageOrigY + adjustForExtend, bitmap, bitmapWidth, bitmapHeight);

            tempDef.U         = _currentPageOrigX;
            tempDef.V         = _currentPageOrigY;
            tempDef.textureID = _currentPage;
            _currentPageOrigX += tempDef.width + 1;
            // take from pixels to points
            tempDef.width   = tempDef.width / scaleFactor;
            tempDef.height  = tempDef.height / scaleFactor;
            tempDef.U       = tempDef.U / scaleFactor;
            tempDef.V       = tempDef.V / scaleFactor;
            tempDef.rotated = false;
        }
        else
        {
            if (bitmap)
                delete[] bitmap;

            tempDef.validDefinition = !!tempDef.xAdvance;
            tempDef.width           = 0;
            tempDef.height          = 0;
            tempDef.U               = 0;
            tempDef.V               = 0;
            tempDef.offsetX         = 0;
            tempDef.offsetY         = 0;
            tempDef.textureID       = 0;
            tempDef.rotated         = false;
            _currentPageOrigX += 1;
        }

        _letterDefinitions[charCode] = tempDef;
    }

    updateTextureContent(pixelFormat, startY);

    return true;
}

void FontAtlas::updateTextureContent(backend::PixelFormat format, int startY)
{
#if !defined(AX_USE_METAL)
    auto data = _currentPageData + (CacheTextureWidth * (int)startY << _strideShift);
    _atlasTextures[_currentPage]->updateWithSubData(data, 0, startY, CacheTextureWidth,
                                                    (int)_currentPageOrigY - startY + _currLineHeight);
#else
    unsigned char* data = nullptr;
    if (_strideShift)
    {
        int nLen = CacheTextureWidth * ((int)_currentPageOrigY - startY + _currLineHeight);
        data     = _currentPageData + CacheTextureWidth * (int)startY * 2;
        memset(_currentPageDataRGBA, 0, 4 * nLen);
        for (auto i = 0; i < nLen; i++)
        {
            _currentPageDataRGBA[i * 4]     = data[i * 2];
            _currentPageDataRGBA[i * 4 + 3] = data[i * 2 + 1];
        }
        _atlasTextures[_currentPage]->updateWithSubData(_currentPageDataRGBA, 0, startY, CacheTextureWidth,
                                                        (int)_currentPageOrigY - startY + _currLineHeight);
    }
    else
    {
        data = _currentPageData + CacheTextureWidth * (int)startY;
        _atlasTextures[_currentPage]->updateWithSubData(data, 0, startY, CacheTextureWidth,
                                                        (int)_currentPageOrigY - startY + _currLineHeight);
    }
#endif
}

void FontAtlas::addNewPage()
{
    auto texture = new Texture2D();

    memset(_currentPageData, 0, _currentPageDataSize);

#if !defined(AX_USE_METAL)
    texture->initWithData(_currentPageData, _currentPageDataSize, _pixelFormat, CacheTextureWidth, CacheTextureHeight);
#else
    if (_strideShift)
    {
        memset(_currentPageDataRGBA, 0, _currentPageDataSizeRGBA);
        texture->initWithData(_currentPageDataRGBA, _currentPageDataSizeRGBA, backend::PixelFormat::RGBA8,
                              CacheTextureWidth, CacheTextureHeight);
    }
    else
    {
        texture->initWithData(_currentPageData, _currentPageDataSize, _pixelFormat, CacheTextureWidth,
                              CacheTextureHeight);
    }
#endif

    if (_antialiasEnabled)
        texture->setAntiAliasTexParameters();
    else
        texture->setAliasTexParameters();

    setTexture(++_currentPage, texture);
    texture->release();

    _currentPageOrigY = 0;
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

NS_AX_END
