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

#include "axmol/2d/FontFNT.h"
#include "axmol/2d/FontAtlas.h"
#include "axmol/2d/SpriteFrameCache.h"
#include "axmol/platform/FileUtils.h"
#include "axmol/base/Environment.h"
#include "axmol/base/Director.h"
#include "axmol/base/Map.h"
#include "axmol/base/text_utils.h"
#include "axmol/renderer/TextureCache.h"
#include "axmol/tlx/charconv.hpp"

#include <cmath>

namespace ax
{

/**
 * @addtogroup GUI
 * @{
 * @addtogroup label
 * @{
 */

enum
{
    kLabelAutomaticWidth = -1,
};

struct _FontDefHashElement;

template <typename _Tp>
static size_t parseNumParam(std::string_view line, std::string_view key, _Tp& outValue, size_t offset = 0)
{
    auto pos = line.find(key, offset);
    if (pos == std::string_view::npos)
        return std::string_view::npos;

    auto substr = line.substr(pos + key.size());

    auto [last, ec] = tlx::from_chars(substr, outValue);
    if (ec != std::errc{})
        return std::string_view::npos;

    return last - line.data();
}

//
// FNTConfig Cache - free functions
//
static StringMap<BMFontConfiguration*>* s_configurations = nullptr;

BMFontConfiguration* FNTConfigLoadFile(std::string_view fntFile)
{
    BMFontConfiguration* ret = nullptr;

    if (s_configurations == nullptr)
    {
        s_configurations = new StringMap<BMFontConfiguration*>();
    }

    ret = s_configurations->at(fntFile);
    if (ret == nullptr)
    {
        ret = BMFontConfiguration::create(fntFile);
        if (ret)
        {
            s_configurations->insert(fntFile, ret);
        }
    }

    return ret;
}

//
// BitmapFontConfiguration
//

BMFontConfiguration* BMFontConfiguration::create(std::string_view FNTfile)
{
    BMFontConfiguration* ret = new BMFontConfiguration();
    if (ret->initWithFNTfile(FNTfile))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool BMFontConfiguration::initWithFNTfile(std::string_view FNTfile)
{
    _characterSet = this->parseConfigFile(FNTfile);
    return !_characterSet.empty();
}

const std::set<unsigned int>& BMFontConfiguration::getCharacterSet() const
{
    return _characterSet;
}

BMFontConfiguration::BMFontConfiguration() : _commonHeight(0), _characterSet(), _fontSize(0) {}

BMFontConfiguration::~BMFontConfiguration()
{
    AXLOGV("deallocing BMFontConfiguration: {}", fmt::ptr(this));
    this->purgeFontDefDictionary();
    this->purgeKerningDictionary();
    _atlasName.clear();
}

std::string BMFontConfiguration::description() const
{
    return fmt::format("<BMFontConfiguration = {} | Glphys:{} Kernings:{} | Image = {}>", fmt::ptr(this),
                       static_cast<int>(_fontDefDictionary.size()), static_cast<int>(_kerningDictionary.size()),
                       _atlasName);
}

void BMFontConfiguration::purgeKerningDictionary()
{
    _kerningDictionary.clear();
}

void BMFontConfiguration::purgeFontDefDictionary()
{
    _fontDefDictionary.clear();
}

std::set<unsigned int> BMFontConfiguration::parseConfigFile(std::string_view controlFile)
{
    std::string data = FileUtils::getInstance()->getStringFromFile(controlFile);
    if (data.empty())
    {
        return {};
    }
    if (data.starts_with("BMF"sv))
    {
        // Handle fnt file of binary format
        std::set<unsigned int> ret =
            parseBinaryConfigFile((unsigned char*)&data.front(), static_cast<uint32_t>(data.size()), controlFile);
        return ret;
    }
    if (data[0] == 0)
    {
        AXLOGW("axmol: Error parsing FNTfile {}", controlFile);
        return {};
    }
    auto contents    = data.c_str();
    auto contentsLen = data.size();

    std::set<unsigned int> validCharsString;

    char line[512] = {0};

    auto next         = strchr(contents, '\n');
    auto base         = contents;
    size_t parseCount = 0;
    while (next)
    {
        size_t lineLength = ((int)(next - base));
        memcpy(line, contents + parseCount, lineLength);
        line[lineLength] = 0;

        std::string_view linesv{line, lineLength};

        parseCount += lineLength + 1;
        if (parseCount < contentsLen)
        {
            base = next + 1;
            next = strchr(base, '\n');
        }
        else
        {
            next = nullptr;
        }

        if (memcmp(line, "info face", 9) == 0)
        {
            // FIXME: info parsing is incomplete
            // Not needed for the Hiero editors, but needed for the AngelCode editor
            //            [self parseInfoArguments:line];
            this->parseInfoArguments(linesv);
        }
        // Check to see if the start of the line is something we are interested in
        else if (memcmp(line, "common lineHeight", 17) == 0)
        {
            this->parseCommonArguments(linesv);
        }
        else if (memcmp(line, "page id", 7) == 0)
        {
            this->parseImageFileName(linesv, controlFile);
        }
        else if (memcmp(line, "chars c", 7) == 0)
        {
            // Ignore this line
        }
        else if (memcmp(line, "char", 4) == 0)
        {
            // Parse the current line and create a new CharDef
            unsigned int charID = this->parseCharacterDefinition(linesv);
            validCharsString.insert(charID);
        }
        else if (memcmp(line, "kerning first", 13) == 0)
        {
            this->parseKerningEntry(linesv);
        }
    }

    return validCharsString;
}

std::set<unsigned int> BMFontConfiguration::parseBinaryConfigFile(unsigned char* pData,
                                                                  uint32_t size,
                                                                  std::string_view controlFile)
{
    /* based on http://www.angelcode.com/products/bmfont/doc/file_format.html file format */

    std::set<unsigned int> validCharsString;
    uint32_t remains = size;

    AXASSERT(pData[3] == 3, "Only version 3 is supported");

    pData += 4;
    remains -= 4;

    while (remains > 0)
    {
        unsigned char blockId = pData[0];
        pData += 1;
        remains -= 1;
        uint32_t blockSize = 0;
        memcpy(&blockSize, pData, 4);

        pData += 4;
        remains -= 4;

        if (blockId == 1)
        {
            /*
             fontSize       2   int      0
             bitField       1   bits     2  bit 0: smooth, bit 1: unicode, bit 2: italic, bit 3: bold, bit 4:
             fixedHeight, bits 5-7: reserved charSet        1   uint     3 stretchH       2   uint     4 aa 1   uint 6
             paddingUp      1   uint     7
             paddingRight   1   uint     8
             paddingDown    1   uint     9
             paddingLeft    1   uint     10
             spacingHoriz   1   uint     11
             spacingVert    1   uint     12
             outline        1   uint     13 added with version 2
             fontName       n+1 string   14 null terminated string with length n
             */

            memcpy(&_fontSize, pData, 2);
            _padding.top    = (unsigned char)pData[7];
            _padding.right  = (unsigned char)pData[8];
            _padding.bottom = (unsigned char)pData[9];
            _padding.left   = (unsigned char)pData[10];
        }
        else if (blockId == 2)
        {
            /*
             lineHeight 2   uint    0
             base       2   uint    2
             scaleW     2   uint    4
             scaleH     2   uint    6
             pages      2   uint    8
             bitField   1   bits    10  bits 0-6: reserved, bit 7: packed
             alphaChnl  1   uint    11
             redChnl    1   uint    12
             greenChnl  1   uint    13
             blueChnl   1   uint    14
             */

            uint16_t lineHeight = 0;
            memcpy(&lineHeight, pData, 2);
            _commonHeight = lineHeight;

            uint16_t scaleW = 0;
            memcpy(&scaleW, pData + 4, 2);
            uint16_t scaleH = 0;
            memcpy(&scaleH, pData + 6, 2);

            AXASSERT(scaleW <= Environment::getInstance()->getMaxTextureSize() &&
                         scaleH <= Environment::getInstance()->getMaxTextureSize(),
                     "CCLabelBMFont: page can't be larger than supported");

            uint16_t pages = 0;
            memcpy(&pages, pData + 8, 2);
            AXASSERT(pages == 1, "CCBitfontAtlas: only supports 1 page");
        }
        else if (blockId == 3)
        {
            /*
             pageNames 	p*(n+1) 	strings 	0 	p null terminated strings, each with length n
             */

            std::string_view value = (const char*)pData;
            AXASSERT(value.size() < blockSize, "Block size should be less then string");

            _atlasName = FileUtils::getInstance()->fullPathFromRelativeFile(value, controlFile);
        }
        else if (blockId == 4)
        {
            /*
             id         4   uint    0+c*20  These fields are repeated until all characters have been described
             x          2   uint    4+c*20
             y          2   uint    6+c*20
             width      2   uint    8+c*20
             height     2   uint    10+c*20
             xoffset    2   int     12+c*20
             yoffset    2   int     14+c*20
             xadvance   2   int     16+c*20
             page       1   uint    18+c*20
             chnl       1   uint    19+c*20
             */

            uint32_t count = blockSize / 20;

            for (uint32_t i = 0; i < count; i++)
            {
                uint32_t charId = 0;
                memcpy(&charId, pData + (i * 20), 4);

                BMFontDef& fontDef = _fontDefDictionary[charId];
                fontDef.charID     = charId;

                uint16_t charX = 0;
                memcpy(&charX, pData + (i * 20) + 4, 2);
                fontDef.rect.origin.x = charX;

                uint16_t charY = 0;
                memcpy(&charY, pData + (i * 20) + 6, 2);
                fontDef.rect.origin.y = charY;

                uint16_t charWidth = 0;
                memcpy(&charWidth, pData + (i * 20) + 8, 2);
                fontDef.rect.size.width = charWidth;

                uint16_t charHeight = 0;
                memcpy(&charHeight, pData + (i * 20) + 10, 2);
                fontDef.rect.size.height = charHeight;

                int16_t xoffset = 0;
                memcpy(&xoffset, pData + (i * 20) + 12, 2);
                fontDef.xOffset = xoffset;

                int16_t yoffset = 0;
                memcpy(&yoffset, pData + (i * 20) + 14, 2);
                fontDef.yOffset = yoffset;

                int16_t xadvance = 0;
                memcpy(&xadvance, pData + (i * 20) + 16, 2);
                fontDef.xAdvance = xadvance;

                validCharsString.insert(fontDef.charID);
            }
        }
        else if (blockId == 5)
        {
            /*
             first  4   uint    0+c*10 	These fields are repeated until all kerning pairs have been described
             second 4   uint    4+c*10
             amount 2   int     8+c*10
             */

            uint32_t count = blockSize / 20;

            for (uint32_t i = 0; i < count; i++)
            {
                uint32_t first = 0;
                memcpy(&first, pData + (i * 10), 4);
                uint32_t second = 0;
                memcpy(&second, pData + (i * 10) + 4, 4);
                int16_t amount = 0;
                memcpy(&amount, pData + (i * 10) + 8, 2);

                uint64_t key            = ((uint64_t)first << 32) | ((uint64_t)second & 0xffffffffll);
                _kerningDictionary[key] = amount;
            }
        }

        pData += blockSize;
        remains -= blockSize;
    }

    return validCharsString;
}

void BMFontConfiguration::parseImageFileName(std::string_view line, std::string_view fntFile)
{
    //////////////////////////////////////////////////////////////////////////
    // line to parse:
    // page id=0 file="bitmapFontTest.png"
    //////////////////////////////////////////////////////////////////////////

    // page ID. Sanity check
    int pageId{-1};
    auto pos = parseNumParam(line, "page id=", pageId);
    AXASSERT(pageId == 0, "LabelBMFont file could not be found");

    // file
    std::string_view fileName;
    auto start = line.find("file=\"", pos);
    if (start != std::string_view::npos)
    {
        start += 6;
        auto end = line.find('"', start);
        if (end != std::string_view::npos)
            fileName = line.substr(start, end - start);
    }

    _atlasName = FileUtils::getInstance()->fullPathFromRelativeFile(fileName, fntFile);
}

void BMFontConfiguration::parseInfoArguments(std::string_view line)
{
    //////////////////////////////////////////////////////////////////////////
    // possible lines to parse:
    // info face="Script" size=32 bold=0 italic=0 charset="" unicode=1 stretchH=100 smooth=1 aa=1 padding=1,4,3,2
    // spacing=0,0 outline=0 info face="Cracked" size=36 bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1
    // padding=0,0,0,0 spacing=1,1
    //////////////////////////////////////////////////////////////////////////
    auto pos = parseNumParam(line, "size="sv, _fontSize);

    pos = line.find("padding=", pos);
    if (pos != std::string_view::npos)
        tlx::from_chars(line.substr(pos + 8), ',', _padding.top, _padding.right, _padding.bottom, _padding.left);
}

void BMFontConfiguration::parseCommonArguments(std::string_view line)
{
    //////////////////////////////////////////////////////////////////////////
    // line to parse:
    // common lineHeight=104 base=26 scaleW=1024 scaleH=512 pages=1 packed=0
    //////////////////////////////////////////////////////////////////////////

    // Height
    parseNumParam(line, "lineHeight="sv, _commonHeight);

#if defined(_AX_DEBUG) && _AX_DEBUG > 0
    // scaleW. sanity check
    int value{-1};
    auto pos = parseNumParam(line, "scaleW="sv, value);

    int maxTextureSize = Environment::getInstance()->getMaxTextureSize();
    AXASSERT(value <= maxTextureSize, "CCLabelBMFont: page can't be larger than supported");

    // scaleH. sanity check
    pos = parseNumParam(line, "scaleH="sv, value, pos);
    AXASSERT(value <= maxTextureSize, "CCLabelBMFont: page can't be larger than supported");

    // pages. sanity check
    parseNumParam(line, "pages="sv, value, pos);
    AXASSERT(value == 1, "CCBitfontAtlas: only supports 1 page");
#endif
    // packed (ignore) What does this mean ??
}

unsigned int BMFontConfiguration::parseCharacterDefinition(std::string_view line)
{
    unsigned int charID = 0;

    //////////////////////////////////////////////////////////////////////////
    // line to parse:
    // char id=32   x=0     y=0     width=0     height=0     xoffset=0     yoffset=44    xadvance=14     page=0  chnl=0
    //////////////////////////////////////////////////////////////////////////

    // Character ID
    auto pos = parseNumParam(line, "id="sv, charID);

    BMFontDef& characterDefinition = _fontDefDictionary[charID];
    characterDefinition.charID     = charID;

    // Character x
    pos = parseNumParam(line, "x="sv, characterDefinition.rect.origin.x, pos);
    // Character y
    pos = parseNumParam(line, "y="sv, characterDefinition.rect.origin.y, pos);
    // Character width
    pos = parseNumParam(line, "width="sv, characterDefinition.rect.size.width, pos);
    // Character height
    pos = parseNumParam(line, "height="sv, characterDefinition.rect.size.height, pos);
    // Character xoffset
    pos = parseNumParam(line, "xoffset="sv, characterDefinition.xOffset, pos);
    // Character yoffset
    pos = parseNumParam(line, "yoffset="sv, characterDefinition.yOffset, pos);
    // Character xadvance
    parseNumParam(line, "xadvance="sv, characterDefinition.xAdvance, pos);

    return charID;
}

void BMFontConfiguration::parseKerningEntry(std::string_view line)
{
    //////////////////////////////////////////////////////////////////////////
    // line to parse:
    // kerning first=121  second=44  amount=-7
    //////////////////////////////////////////////////////////////////////////

    int first, second, amount;
    auto pos = parseNumParam(line, "first="sv, first);

    pos = parseNumParam(line, "second="sv, second, pos);

    parseNumParam(line, "amount="sv, amount, pos);

    uint64_t key = ((uint64_t)first << 32) | ((uint64_t)second & 0xffffffffll);

    _kerningDictionary[key] = amount;
}

FontFNT* FontFNT::create(std::string_view fntFilePath, const Rect& imageRect, bool imageRotated)
{
    const auto newConf = FNTConfigLoadFile(fntFilePath);
    if (!newConf)
        return nullptr;

    const auto tempFont = new FontFNT(newConf, imageRect, imageRotated);

    if (!tempFont)
    {
        return nullptr;
    }
    tempFont->setFontSize((float)newConf->_fontSize);
    tempFont->autorelease();
    return tempFont;
}

FontFNT* FontFNT::create(std::string_view fntFilePath, std::string_view subTextureKey)
{
    const auto newConf = FNTConfigLoadFile(fntFilePath);
    if (!newConf)
        return nullptr;

    const auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(subTextureKey);
    if (!frame)
    {
        return nullptr;
    }
    auto tempFont = new FontFNT(newConf, frame->getRectInPixels(), frame->isRotated());

    if (!tempFont)
    {
        return nullptr;
    }
    tempFont->setFontSize((float)newConf->_fontSize);
    tempFont->autorelease();
    return tempFont;
}

FontFNT* FontFNT::create(std::string_view fntFilePath)
{
    const auto newConf = FNTConfigLoadFile(fntFilePath);
    if (!newConf)
        return nullptr;

    // add the texture
    const auto tempTexture = Director::getInstance()->getTextureCache()->addImage(newConf->getAtlasName());
    if (!tempTexture)
    {
        return nullptr;
    }
    FontFNT* tempFont = new FontFNT(newConf);

    tempFont->setFontSize((float)newConf->_fontSize);
    tempFont->autorelease();
    return tempFont;
}

FontFNT::FontFNT(BMFontConfiguration* theContfig, const Rect& imageRect, bool imageRotated)
    : _configuration(theContfig), _imageRectInPoints(AX_RECT_PIXELS_TO_POINTS(imageRect)), _imageRotated(imageRotated)
{
    _configuration->retain();
}

FontFNT::FontFNT(BMFontConfiguration* theContfig)
    : _configuration(theContfig), _imageRectInPoints(Rect::ZERO), _imageRotated(false)
{}

FontFNT::~FontFNT()
{
    _configuration->release();
}

void FontFNT::purgeCachedData()
{
    if (s_configurations)
    {
        s_configurations->clear();
        AX_SAFE_DELETE(s_configurations);
    }
}

int* FontFNT::getHorizontalKerningForTextUTF32(const std::u32string& text, int& outNumLetters) const
{
    outNumLetters = static_cast<int>(text.length());

    if (!outNumLetters)
        return nullptr;

    int* sizes = new int[outNumLetters];
    for (int c = 0; c < outNumLetters; ++c)
    {
        if (c < (outNumLetters - 1))
            sizes[c] = getHorizontalKerningForChars(text[c], text[c + 1]);
        else
            sizes[c] = 0;
    }

    return sizes;
}

int FontFNT::getHorizontalKerningForChars(char32_t firstChar, char32_t secondChar) const
{
    int ret      = 0;
    uint64_t key = ((uint64_t)firstChar << 32) | ((uint64_t)secondChar & 0xffffffffll);

    auto iter = _configuration->_kerningDictionary.find(key);

    if (iter != _configuration->_kerningDictionary.end())
    {
        ret = iter->second;
    }

    return ret;
}

void FontFNT::setFontSize(float fontSize)
{
    _fontSize = fontSize;
}

int FontFNT::getOriginalFontSize() const
{
    return _configuration->_fontSize;
}

FontAtlas* FontFNT::newFontAtlas()
{
    // check that everything is fine with the BMFontCofniguration
    if (_configuration->_fontDefDictionary.empty())
        return nullptr;

    size_t numGlyphs = _configuration->_characterSet.size();
    if (numGlyphs == 0)
        return nullptr;

    if (_configuration->_commonHeight == 0)
        return nullptr;

    FontAtlas* tempAtlas = new FontAtlas(this);

    // common height
    int originalFontSize     = _configuration->_fontSize;
    float originalLineHeight = (float)_configuration->_commonHeight;
    float factor             = 0.0f;
    if (std::abs(_fontSize - originalFontSize) < FLT_EPSILON)
    {
        factor = 1.0f;
    }
    else
    {
        factor = _fontSize / originalFontSize;
    }

    tempAtlas->setLineHeight(originalLineHeight * factor);

    auto rw = _imageRectInPoints.size.width;
    auto rh = _imageRectInPoints.size.height;

    if (_imageRotated)
        std::swap(rw, rh);

    const auto left  = _imageRectInPoints.origin.x;
    const auto right = _imageRectInPoints.origin.x + rw;
    const auto top   = _imageRectInPoints.origin.y;

    for (auto&& e : _configuration->_fontDefDictionary)
    {
        BMFontDef& fontDef = e.second;

        FontLetterDefinition tempDefinition;

        const auto tempRect = AX_RECT_PIXELS_TO_POINTS(fontDef.rect);

        tempDefinition.offsetX = fontDef.xOffset;
        tempDefinition.offsetY = fontDef.yOffset;

        if (_imageRotated)
        {
            tempDefinition.U = right - tempRect.origin.y - tempRect.size.height;
            tempDefinition.V = tempRect.origin.x + top;
        }
        else
        {
            tempDefinition.U = tempRect.origin.x + left;
            tempDefinition.V = tempRect.origin.y + top;
        }

        tempDefinition.width  = tempRect.size.width;
        tempDefinition.height = tempRect.size.height;

        // carloX: only one texture supported FOR NOW
        tempDefinition.textureID = 0;

        tempDefinition.validDefinition = true;
        tempDefinition.xAdvance        = fontDef.xAdvance;
        tempDefinition.rotated         = _imageRotated;

        // add the new definition
        tempAtlas->addLetterDefinition(fontDef.charID, tempDefinition);
    }

    // add the texture (only one texture for now)

    Texture2D* tempTexture = Director::getInstance()->getTextureCache()->addImage(_configuration->getAtlasName());
    if (!tempTexture)
    {
        AX_SAFE_RELEASE(tempAtlas);
        return nullptr;
    }

    // add the texture
    tempAtlas->setTexture(0, tempTexture);

    // done
    return tempAtlas;
}

void FontFNT::reloadBMFontResource(std::string_view fntFilePath)
{
    if (s_configurations == nullptr)
    {
        s_configurations = new StringMap<BMFontConfiguration*>();
    }

    BMFontConfiguration* ret = s_configurations->at(fntFilePath);
    if (ret != nullptr)
    {
        s_configurations->erase(fntFilePath);
    }

    ret = BMFontConfiguration::create(fntFilePath);
    if (ret)
    {
        s_configurations->insert(fntFilePath, ret);
        Director::getInstance()->getTextureCache()->reloadTexture(ret->getAtlasName());
    }
}

}  // namespace ax
