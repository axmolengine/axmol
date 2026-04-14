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

#include "axmol/2d/FontFreeType.h"
#include "axmol/2d/FontAtlas.h"
#include "axmol/base/Director.h"
#include "axmol/base/text_utils.h"
#include "axmol/tlx/filesystem.hpp"
#include "freetype/ftmodapi.h"
#include "axmol/platform/FileUtils.h"
#include "axmol/platform/FileStream.h"
#include "axmol/platform/Application.h"

#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_STROKER_H
#include FT_BBOX_H
#include FT_FONT_FORMATS_H
#include FT_SIZES_H
#include FT_MULTIPLE_MASTERS_H

namespace ax
{

FT_Library FontFreeType::_FTlibrary         = nullptr;
bool FontFreeType::_FTInitialized           = false;
bool FontFreeType::_streamParsingEnabled    = false;
bool FontFreeType::_doNativeBytecodeHinting = true;
bool FontFreeType::_globalSDFEnabled        = false;
const int FontFreeType::DistanceMapSpread   = 6;
IFontEngine* FontFreeType::_fontEngine      = nullptr;

// By default, will render square when character glyph missing in current font
char32_t FontFreeType::_mssingGlyphCharacter = 0;

using namespace std::string_view_literals;
constexpr std::string_view _glyphASCII =
    "\"!#$%&'()*+,-./"
    "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
    "¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþ "sv;
constexpr std::string_view _glyphNEHE =
    "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ "sv;

typedef struct _DataRef
{
    Data data;
    unsigned int referenceCount = 0;
} DataRef;

static tlx::string_map<DataRef> s_cacheFontData;

// ------ freetype2 stream parsing support ---
static unsigned long ft_stream_read_callback(FT_Stream stream, unsigned long offset, uint8_t* buf, unsigned long size)
{
    auto fstm = (IFileStream*)stream->descriptor.pointer;
    if (!fstm)
        return 1;
    if (!size && offset >= stream->size)
        return 1;

    if (stream->pos != offset)
        fstm->seek(offset, SEEK_SET);

    if (buf)
        return fstm->read(buf, static_cast<unsigned int>(size));

    return 0;
}

static void ft_stream_close_callback(FT_Stream stream)
{
    const auto* fstrm = (IFileStream*)stream->descriptor.pointer;
    delete fstrm;
    stream->size               = 0;
    stream->descriptor.pointer = nullptr;
}

FontFreeType* FontFreeType::createFallbackFont(const GlyphResolution& glyphRes, FontFreeType* mainFont)
{
    if (glyphRes.face)
    {
        FontFreeType* tempFont = new FontFreeType(glyphRes.family, mainFont->_faceSize,
                                                  mainFont->isDistanceFieldEnabled(), mainFont->getOutlineSize());
        tempFont->setGlyphCollection(mainFont->_usedGlyphs, mainFont->getGlyphCollection());
        if (tempFont->initWithFace(glyphRes.face))
        {
            tempFont->autorelease();
            return tempFont;
        }
        delete tempFont;
    }
    return nullptr;
}

void FontFreeType::setFontEngine(IFontEngine* fe)
{
    _fontEngine = fe;
}

FontFreeType* FontFreeType::create(std::string_view fontName,
                                   int faceSize,
                                   GlyphCollection glyphs,
                                   std::string_view customGlyphs,
                                   bool distanceFieldEnabled /* = false */,
                                   float outline /* = 0 */)
{
    FontFreeType* tempFont = new FontFreeType(fontName, faceSize, distanceFieldEnabled, outline);

    tempFont->setGlyphCollection(glyphs, customGlyphs);

    if (tempFont->init())
    {
        tempFont->autorelease();
        return tempFont;
    }

    delete tempFont;
    return nullptr;
}

bool FontFreeType::initFreeType()
{
    if (!_FTInitialized)
    {
        // begin freetype
        if (FT_Init_FreeType(&_FTlibrary))
            return false;

        const FT_Int spread = DistanceMapSpread * AX_CONTENT_SCALE_FACTOR();
        FT_Property_Set(_FTlibrary, "sdf", "spread", &spread);
        FT_Property_Set(_FTlibrary, "bsdf", "spread", &spread);

        _FTInitialized = true;
    }

    return _FTInitialized;
}

void FontFreeType::shutdownFreeType()
{
    if (_FTInitialized)
    {
        FT_Done_FreeType(_FTlibrary);
        s_cacheFontData.clear();
        _FTInitialized = false;
    }
}

FT_Library FontFreeType::getFTLibrary()
{
    initFreeType();
    return _FTlibrary;
}

// clang-format off
FontFreeType::FontFreeType(std::string_view fontName, int faceSize, bool distanceFieldEnabled /* = false */, float outline /* = 0 */)
: _ftFace(nullptr)
, _ftSize(nullptr)
, _ftStroker(nullptr)
, _ftStream(nullptr)
, _fontName(fontName)
, _faceSize(faceSize)
, _distanceFieldEnabled(distanceFieldEnabled)
, _ownFontFace(false)
, _outlineSize(0.0f)
, _ascender(0)
, _descender(0)
, _lineHeight(0)
, _usedGlyphs(GlyphCollection::ASCII)
{
    if (outline > 0.0f)
    {
        _outlineSize = outline * AX_CONTENT_SCALE_FACTOR();
        FT_Stroker_New(FontFreeType::getFTLibrary(), &_ftStroker);
        FT_Stroker_Set(_ftStroker,
            (int)(_outlineSize * 64),
            FT_STROKER_LINECAP_ROUND,
            FT_STROKER_LINEJOIN_ROUND,
            0);
    }
}
// clang-format on

FontFreeType::~FontFreeType()
{
    if (_FTInitialized)
    {
        if (_ftStroker)
            FT_Stroker_Done(_ftStroker);

        if (_ftSize)
            FT_Done_Size(_ftSize);

        if (_ownFontFace && _ftFace)
            FT_Done_Face(_ftFace);
    }

    AX_SAFE_DELETE(_ftStream);

    auto iter = s_cacheFontData.find(_fontName);
    if (iter != s_cacheFontData.end())
    {
        iter->second.referenceCount -= 1;
        if (iter->second.referenceCount == 0)
            s_cacheFontData.erase(iter);
    }
}

bool FontFreeType::init()
{
    FT_Face face{nullptr};
    if (_streamParsingEnabled)
    {
        auto fullPath = FileUtils::getInstance()->fullPathForFilename(_fontName);
        if (fullPath.empty())
            return false;
        FT_Error error = 0;
        if (FileUtils::isAbsolutePathInternal(fullPath))
        {
            error = FT_New_Face(getFTLibrary(), fullPath.c_str(), 0, &face);
        }
        else
        {
            auto fs = FileUtils::getInstance()->openFileStream(fullPath, IFileStream::Mode::READ);
            if (!fs)
            {
                return false;
            }

            FT_Stream fts           = new FT_StreamRec();
            fts->read               = ft_stream_read_callback;
            fts->close              = ft_stream_close_callback;
            fts->size               = static_cast<unsigned long>(fs->size());
            fts->descriptor.pointer = fs.release();  // transfer ownership to FT_Open_Face

            FT_Open_Args args = {};
            args.flags        = FT_OPEN_STREAM;
            args.stream       = fts;

            _ftStream = fts;

            error = FT_Open_Face(getFTLibrary(), &args, 0, &face);
        }

        if (error)
            return false;
    }
    else
    {
        DataRef* sharableData;
        auto it = s_cacheFontData.find(_fontName);
        if (it != s_cacheFontData.end())
        {
            sharableData = &it->second;
        }
        else
        {
            sharableData       = &s_cacheFontData[_fontName];
            sharableData->data = FileUtils::getInstance()->getDataFromFile(_fontName);
        }

        ++sharableData->referenceCount;
        auto& data = sharableData->data;
        if (data.isNull() ||
            FT_New_Memory_Face(getFTLibrary(), data.getBytes(), static_cast<FT_Long>(data.getSize()), 0, &face))
            return false;
    }

    _ownFontFace = true;
    return initWithFace(face);
}

bool FontFreeType::initWithFace(FT_Face face)
{
    do
    {
        if (!face->charmap || face->charmap->encoding != FT_ENCODING_UNICODE)
            break;

        AX_BREAK_IF(FT_New_Size(face, &_ftSize));

        AX_BREAK_IF(FT_Activate_Size(_ftSize));

        if (_distanceFieldEnabled)
        {
            if (FT_Set_Pixel_Sizes(face, 0, _faceSize))
                break;
        }
        else
        {
            // set the requested font size
            int dpi   = 72;
            int units = _faceSize << 6;
            if (FT_Set_Char_Size(face, 0, units, dpi, dpi))
                break;
        }

        std::string_view font_format = FT_Get_Font_Format(face);

        // Notes:
        //  a. Since freetype 2.8.1 the TT matrics isn't sync to size_matrics, see the function 'tt_size_request' in
        //  truetype/ttdriver.c b. The TT spec always asks for ROUND, not FLOOR or CEIL, see also the function
        //  'tt_size_reset' in truetype/ttobjs.c
        // ** Please see description of FT_Size_Metrics_ in freetype.h about this solution
        // FT_PIX_ROUND is copy from freetype/internal/ftobjs.h
        auto& size_metrics = face->size->metrics;
        if (_doNativeBytecodeHinting && font_format == "TrueType"sv)
        {
#if !defined(FT_PIX_ROUND)
#    define FT_TYPEOF(type)
#    define FT_PIX_FLOOR(x) ((x) & ~FT_TYPEOF(x) 63)
#    define FT_PIX_ROUND(x) FT_PIX_FLOOR((x) + 32)
#endif
            _ascender  = static_cast<int>(FT_PIX_ROUND(FT_MulFix(face->ascender, size_metrics.y_scale)));
            _descender = static_cast<int>(FT_PIX_ROUND(FT_MulFix(face->descender, size_metrics.y_scale)));
        }
        else
        {
            _ascender  = static_cast<int>(size_metrics.ascender);
            _descender = static_cast<int>(size_metrics.descender);
        }

        _lineHeight = static_cast<int>((_ascender - _descender) >> 6);

        _styleFlags = face->style_flags;

        _ftFace = face;

        // done and good
        return true;
    } while (false);

    AXLOGI("Init font '{}' failed, only unicode ttf/ttc was supported.", _fontName);
    return false;
}

FontAtlas* FontFreeType::newFontAtlas()
{
    auto fontAtlas = new FontAtlas(this);
    if (_usedGlyphs != GlyphCollection::DYNAMIC)
    {
        std::u32string utf32;
        if (text_utils::UTF8ToUTF32(getGlyphCollection(), utf32))
        {
            fontAtlas->prepareLetterDefinitions(utf32);
        }
    }
    return fontAtlas;
}

int* FontFreeType::getHorizontalKerningForTextUTF32(const std::u32string& text, int& outNumLetters) const
{
    if (!_ftFace)
        return nullptr;

    outNumLetters = static_cast<int>(text.length());

    if (!outNumLetters)
        return nullptr;

    int* sizes = new int[outNumLetters];
    memset(sizes, 0, outNumLetters * sizeof(int));

    bool hasKerning = FT_HAS_KERNING(_ftFace) != 0;
    if (hasKerning)
    {
        for (int c = 1; c < outNumLetters; ++c)
        {
            sizes[c] = getHorizontalKerningForChars(text[c - 1], text[c]);
        }
    }

    return sizes;
}

int FontFreeType::getHorizontalKerningForChars(uint64_t firstChar, uint64_t secondChar) const
{
    // get the ID to the char we need
    auto glyphIndex1 = FT_Get_Char_Index(_ftFace, static_cast<FT_ULong>(firstChar));

    if (!glyphIndex1)
        return 0;

    // get the ID to the char we need
    auto glyphIndex2 = FT_Get_Char_Index(_ftFace, static_cast<FT_ULong>(secondChar));

    if (!glyphIndex2)
        return 0;

    FT_Vector kerning;

    if (FT_Get_Kerning(_ftFace, glyphIndex1, glyphIndex2, FT_KERNING_DEFAULT, &kerning))
        return 0;

    return (static_cast<int>(kerning.x >> 6));
}

int FontFreeType::getFontAscender() const
{
    return _ascender >> 6;
}

const char* FontFreeType::getFontFamily() const
{
    if (!_ftFace)
        return nullptr;

    return _ftFace->family_name;
}

uint8_t* FontFreeType::getGlyphBitmap(char32_t charCode,
                                      GlyphSize& glyphSize,
                                      GlyphMetrics& glyphMetrics,
                                      const GlyphResolution*& outFallbackRes,
                                      bool& sharedBitmapData)
{
    // @remark: glyphIndex=0 means charactor is mssing on current font face
    auto glyphIndex = FT_Get_Char_Index(_ftFace, static_cast<FT_ULong>(charCode));
    if (glyphIndex == 0)
    {
        if (charCode == '\n' || charCode == '\r')
            return nullptr;
#if defined(_AX_DEBUG) && _AX_DEBUG > 0
        char32_t ntcs[2] = {charCode, (char32_t)0};
        std::u32string_view charUTF32(ntcs, 1);
        std::string charUTF8;
        ax::text_utils::UTF32ToUTF8(charUTF32, charUTF8);
        AXLOGW("The font face: {} doesn't contains char: <{}>", _ftFace->charmap->face->family_name, charUTF8);
#endif

        if (_fontEngine)
        {  // resolving glyph by font engine
            auto res = _fontEngine->resolveGlyph(charCode, _styleFlags);
            if (res)
            {
                outFallbackRes = res;
                return nullptr;
            }
        }

        // Not found charCode in system fallback fonts
        if (_mssingGlyphCharacter != 0)
        {
            if (_mssingGlyphCharacter == 0x1A)
            {
                glyphMetrics.xAdvance = 0;
                return nullptr;  // don't render anything for this character
            }
            // Try get new glyph index with missing glyph character code
            glyphIndex = FT_Get_Char_Index(_ftFace, static_cast<FT_ULong>(_mssingGlyphCharacter));
        }
    }

    return getGlyphBitmapByIndex(glyphIndex, glyphSize, glyphMetrics, sharedBitmapData);
}

uint8_t* FontFreeType::getGlyphBitmapByIndex(unsigned int glyphIndex,
                                             GlyphSize& glyphSize,
                                             GlyphMetrics& glyphMetrics,
                                             bool& sharedBitmapData)
{
    uint8_t* ret = nullptr;

    do
    {
        FT_Activate_Size(_ftSize);
        if (FT_Load_Glyph(_ftFace, glyphIndex, FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT))
            break;

        auto glyph = _ftFace->glyph;
        if (_distanceFieldEnabled && glyph->bitmap.buffer)
        {
            // Require freetype version > 2.11.0, because freetype 2.11.0 sdf has memory access bug, see:
            // https://gitlab.freedesktop.org/freetype/freetype/-/issues/1077
            FT_Render_Glyph(glyph, FT_Render_Mode::FT_RENDER_MODE_SDF);
        }

        glyphSize.width  = glyph->bitmap.width;
        glyphSize.height = glyph->bitmap.rows;

        auto& metrics = glyph->metrics;

        glyphMetrics.bboxWidth    = static_cast<float>(metrics.width >> 6);
        glyphMetrics.bboxHeight   = static_cast<float>(metrics.height >> 6);
        glyphMetrics.horiBearingX = static_cast<float>(metrics.horiBearingX >> 6);
        glyphMetrics.horiBearingY = static_cast<float>(metrics.horiBearingY >> 6);
        glyphMetrics.xAdvance     = static_cast<int>(metrics.horiAdvance >> 6);

        if (_outlineSize > 0 && glyphSize.width > 0 && glyphSize.height > 0) [[unlikely]]
        {
            sharedBitmapData       = false;
            const auto sizeInBytes = glyphSize.width * glyphSize.height;
            auto copyBitmap        = new uint8_t[sizeInBytes];
            memcpy(copyBitmap, glyph->bitmap.buffer, sizeInBytes);

            FT_BBox bbox;
            auto outlineBitmap = getGlyphBitmapWithOutline(glyphIndex, bbox);
            if (outlineBitmap == nullptr)
            {
                ret = nullptr;
                delete[] copyBitmap;
                break;
            }

            int glyphMinX = static_cast<int>(glyphMetrics.horiBearingX);
            int glyphMaxX = static_cast<int>(glyphMetrics.horiBearingX + glyphSize.width);
            int glyphMinY = static_cast<int>(glyphMetrics.horiBearingY - glyphSize.height);
            int glyphMaxY = static_cast<int>(glyphMetrics.horiBearingY);

            auto outlineMinX   = bbox.xMin >> 6;
            auto outlineMaxX   = bbox.xMax >> 6;
            auto outlineMinY   = bbox.yMin >> 6;
            auto outlineMaxY   = bbox.yMax >> 6;
            auto outlineWidth  = outlineMaxX - outlineMinX;
            auto outlineHeight = outlineMaxY - outlineMinY;

            auto blendImageMinX = MIN(outlineMinX, glyphMinX);
            auto blendImageMaxY = MAX(outlineMaxY, glyphMaxY);
            auto blendWidth     = MAX(outlineMaxX, glyphMaxX) - blendImageMinX;
            auto blendHeight    = blendImageMaxY - MIN(outlineMinY, glyphMinY);

            metrics.horiBearingX = (float)blendImageMinX;
            // FreeType: horiBearingY = distance from baseline to top
            metrics.horiBearingY = blendImageMaxY - _outlineSize;

            uint8_t* blendImage = nullptr;
            if (blendWidth > 0 && blendHeight > 0)
            {
                FT_Pos index, index2;
                auto imageSize = blendWidth * blendHeight * 2;
                blendImage     = new uint8_t[imageSize];
                memset(blendImage, 0, imageSize);

                auto px = outlineMinX - blendImageMinX;
                auto py = blendImageMaxY - outlineMaxY;
                for (int y = 0; y < outlineHeight; ++y)
                {
                    for (int x = 0; x < outlineWidth; ++x)
                    {
                        index                 = px + x + ((py + y) * blendWidth);
                        index2                = x + (y * outlineWidth);
                        blendImage[2 * index] = outlineBitmap[index2];
                    }
                }

                px = glyphMinX - blendImageMinX;
                py = blendImageMaxY - glyphMaxY;
                for (int y = 0; y < glyphSize.height; ++y)
                {
                    for (int x = 0; x < glyphSize.width; ++x)
                    {
                        index                     = px + x + ((y + py) * blendWidth);
                        index2                    = x + (y * glyphSize.width);
                        blendImage[2 * index + 1] = copyBitmap[index2];
                    }
                }
            }

            glyphMetrics.bboxWidth  = static_cast<float>(blendWidth);
            glyphMetrics.bboxHeight = static_cast<float>(blendHeight);
            glyphSize.width         = static_cast<int>(blendWidth);
            glyphSize.height        = static_cast<int>(blendHeight);

            delete[] outlineBitmap;
            delete[] copyBitmap;
            ret = blendImage;
        }
        else
        {
            ret              = glyph->bitmap.buffer;
            sharedBitmapData = true;
        }

        return ret;
    } while (0);

    glyphMetrics.bboxWidth  = 0;
    glyphMetrics.bboxHeight = 0;
    glyphMetrics.xAdvance   = 0;

    return nullptr;
}

uint8_t* FontFreeType::getGlyphBitmapWithOutline(unsigned int glyphIndex, FT_BBox& bbox)
{
    uint8_t* ret = nullptr;
    if (FT_Load_Glyph(_ftFace, glyphIndex, FT_LOAD_NO_BITMAP) == 0)
    {
        auto outline_glyph = _ftFace->glyph;
        if (outline_glyph->format == FT_GLYPH_FORMAT_OUTLINE)
        {
            FT_Glyph glyph{nullptr};
            if (FT_Get_Glyph(outline_glyph, &glyph) == 0)
            {
                FT_Glyph_StrokeBorder(&glyph, _ftStroker, 0, 1);
                if (glyph->format == FT_GLYPH_FORMAT_OUTLINE)
                {
                    FT_Outline* outline = &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;
                    FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_GRIDFIT, &bbox);
                    int32_t width = static_cast<int32_t>((bbox.xMax - bbox.xMin) >> 6);
                    int32_t rows  = static_cast<int32_t>((bbox.yMax - bbox.yMin) >> 6);

                    FT_Bitmap bmp;
                    bmp.buffer = new uint8_t[width * rows];
                    memset(bmp.buffer, 0, width * rows);
                    bmp.width      = (int)width;
                    bmp.rows       = (int)rows;
                    bmp.pitch      = (int)width;
                    bmp.pixel_mode = FT_PIXEL_MODE_GRAY;
                    bmp.num_grays  = 256;

                    FT_Raster_Params params;
                    memset(&params, 0, sizeof(params));
                    params.source = outline;
                    params.target = &bmp;
                    params.flags  = FT_RASTER_FLAG_AA;
                    FT_Outline_Translate(outline, -bbox.xMin, -bbox.yMin);
                    FT_Outline_Render(_FTlibrary, outline, &params);

                    ret = bmp.buffer;
                }
                FT_Done_Glyph(glyph);
            }
        }
    }

    return ret;
}

void FontFreeType::renderCharAt(uint8_t* dest,
                                int posX,
                                int posY,
                                uint8_t* bitmap,
                                int bitmapWidth,
                                int bitmapHeight,
                                int atlasWidth,
                                int atlasHeight)
{
    const int iX = posX;
    int iY       = posY;

    if (_outlineSize > 0)
    {
        for (int32_t y = 0; y < bitmapHeight; ++y)
        {
            int32_t bitmap_y = y * bitmapWidth;
            memcpy(dest + (iX + (iY * atlasWidth)) * 2, bitmap + bitmap_y * 2, bitmapWidth * 2);
            ++iY;
        }
    }
    else
    {
        for (int32_t y = 0; y < bitmapHeight; ++y)
        {
            int32_t bitmap_y = y * bitmapWidth;
            memcpy(dest + (iX + (iY * atlasWidth)), bitmap + bitmap_y, bitmapWidth);
            ++iY;
        }
    }
}

void FontFreeType::setGlyphCollection(GlyphCollection glyphs, std::string_view customGlyphs)
{
    _usedGlyphs = glyphs;
    if (glyphs == GlyphCollection::CUSTOM)
    {
        _customGlyphs = customGlyphs;
    }
}

std::string_view FontFreeType::getGlyphCollection() const
{
    std::string_view glyphCollection;
    switch (_usedGlyphs)
    {
    case ax::GlyphCollection::DYNAMIC:
        break;
    case ax::GlyphCollection::NEHE:
        glyphCollection = _glyphNEHE;
        break;
    case ax::GlyphCollection::ASCII:
        glyphCollection = _glyphASCII;
        break;
    case ax::GlyphCollection::CUSTOM:
        glyphCollection = _customGlyphs;
        break;
    default:
        break;
    }

    return glyphCollection;
}

void FontFreeType::releaseFont(std::string_view fontName)
{
    auto item = s_cacheFontData.begin();
    while (s_cacheFontData.end() != item)
    {
        if (item->first.find(fontName) != std::string::npos)
            item = s_cacheFontData.erase(item);
        else
            item++;
    }
}

}  // namespace ax
