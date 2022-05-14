/****************************************************************************
Copyright (c) 2013      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2021 Bytedance Inc.

https://adxeproject.github.io/

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

#include "2d/CCFontFreeType.h"

#include "freetype/ftfntfmt.h"

#include FT_BBOX_H
#include "2d/CCFontAtlas.h"
#include "base/CCDirector.h"
#include "base/ccUTF8.h"
#include "freetype/ftmodapi.h"
#include "platform/CCFileUtils.h"
#include "platform/CCFileStream.h"

NS_CC_BEGIN

FT_Library FontFreeType::_FTlibrary;
bool FontFreeType::_FTInitialized           = false;
bool FontFreeType::_streamParsingEnabled    = true;
bool FontFreeType::_doNativeBytecodeHinting = true;
const int FontFreeType::DistanceMapSpread   = 6;

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

static std::unordered_map<std::string, DataRef> s_cacheFontData;

// ------ freetype2 stream parsing support ---
static unsigned long ft_stream_read_callback(FT_Stream stream,
                                             unsigned long offset,
                                             unsigned char* buf,
                                             unsigned long size)
{
    auto fd = (FileStream*)stream->descriptor.pointer;
    if (!fd)
        return 1;
    if (!size && offset >= stream->size)
        return 1;

    if (stream->pos != offset)
        fd->seek(offset, SEEK_SET);

    if (buf)
        return fd->read(buf, size);

    return 0;
}

static void ft_stream_close_callback(FT_Stream stream)
{
    const auto* fd = (FileStream*)stream->descriptor.pointer;
    delete fd;
    stream->size               = 0;
    stream->descriptor.pointer = nullptr;
}

FontFreeType* FontFreeType::create(std::string_view fontName,
                                   float fontSize,
                                   GlyphCollection glyphs,
                                   std::string_view customGlyphs,
                                   bool distanceFieldEnabled /* = false */,
                                   float outline /* = 0 */)
{
    FontFreeType* tempFont = new FontFreeType(distanceFieldEnabled, outline);

    tempFont->setGlyphCollection(glyphs, customGlyphs);

    if (tempFont->loadFontFace(fontName, fontSize))
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

        const FT_Int spread = DistanceMapSpread;
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
FontFreeType::FontFreeType(bool distanceFieldEnabled /* = false */, float outline /* = 0 */)
: _fontFace(nullptr)
, _stroker(nullptr)
, _encoding(FT_ENCODING_UNICODE)
, _distanceFieldEnabled(distanceFieldEnabled)
, _outlineSize(0.0f)
, _ascender(0)
, _descender(0)
, _lineHeight(0)
, _usedGlyphs(GlyphCollection::ASCII)
{
    if (outline > 0.0f)
    {
        _outlineSize = outline * CC_CONTENT_SCALE_FACTOR();
        FT_Stroker_New(FontFreeType::getFTLibrary(), &_stroker);
        FT_Stroker_Set(_stroker,
            (int)(_outlineSize * 64),
            FT_STROKER_LINECAP_ROUND,
            FT_STROKER_LINEJOIN_ROUND,
            0);
    }
}
// clang-format on

bool FontFreeType::loadFontFace(std::string_view fontName, float fontSize)
{
    FT_Face face;
    // save font name locally
    _fontName = fontName;
    _fontSize = fontSize;

    if (_streamParsingEnabled)
    {
        auto fullPath = FileUtils::getInstance()->fullPathForFilename(_fontName);
        if (fullPath.empty())
            return false;

        auto fs = FileUtils::getInstance()->openFileStream(fullPath, FileStream::Mode::READ);
        if (!fs)
        {
            return false;
        }

        std::unique_ptr<FT_StreamRec> fts(new FT_StreamRec());
        fts->read               = ft_stream_read_callback;
        fts->close              = ft_stream_close_callback;
        fts->size               = static_cast<unsigned long>(fs->size());
        fts->descriptor.pointer = fs.release();  // transfer ownership to FT_Open_Face

        FT_Open_Args args = {0};
        args.flags        = FT_OPEN_STREAM;
        args.stream       = fts.get();

        if (FT_Open_Face(getFTLibrary(), &args, 0, &face))
            return false;

        _fontStream = std::move(fts);
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

    if (FT_Select_Charmap(face, FT_ENCODING_UNICODE))
    {
        int foundIndex = -1;
        for (int charmapIndex = 0; charmapIndex < face->num_charmaps; charmapIndex++)
        {
            if (face->charmaps[charmapIndex]->encoding != FT_ENCODING_NONE)
            {
                foundIndex = charmapIndex;
                break;
            }
        }

        if (foundIndex == -1)
        {
            return false;
        }

        _encoding = face->charmaps[foundIndex]->encoding;
        if (FT_Select_Charmap(face, _encoding))
        {
            return false;
        }
    }

    // set the requested font size
    int dpi            = 72;
    int fontSizePoints = (int)(64.f * fontSize * CC_CONTENT_SCALE_FACTOR());
    if (FT_Set_Char_Size(face, fontSizePoints, fontSizePoints, dpi, dpi))
        return false;

    // store the face globally
    _fontFace = face;

    // Notes:
    //  a. Since freetype 2.8.1 the TT matrics isn't sync to size_matrics, see the function 'tt_size_request' in
    //  truetype/ttdriver.c b. The TT spec always asks for ROUND, not FLOOR or CEIL, see also the function
    //  'tt_size_reset' in truetype/ttobjs.c
    // ** Please see description of FT_Size_Metrics_ in freetype.h about this solution
    // FT_PIX_ROUND is copy from freetype/internal/ftobjs.h
    auto& size_metrics = _fontFace->size->metrics;
    if (_doNativeBytecodeHinting && !strcmp(FT_Get_Font_Format(face), "TrueType"))
    {
#if !defined(FT_PIX_ROUND)
#    define FT_TYPEOF(type)
#    define FT_PIX_FLOOR(x) ((x) & ~FT_TYPEOF(x) 63)
#    define FT_PIX_ROUND(x) FT_PIX_FLOOR((x) + 32)
#endif
        _ascender  = FT_PIX_ROUND(FT_MulFix(face->ascender, size_metrics.y_scale));
        _descender = FT_PIX_ROUND(FT_MulFix(face->descender, size_metrics.y_scale));
    }
    else
    {
        _ascender  = size_metrics.ascender;
        _descender = size_metrics.descender;
    }

    _lineHeight = (_ascender - _descender) >> 6;

    // done and good
    return true;
}

FontFreeType::~FontFreeType()
{
    if (_FTInitialized)
    {
        if (_stroker)
        {
            FT_Stroker_Done(_stroker);
        }
        if (_fontFace)
        {
            FT_Done_Face(_fontFace);
        }
    }

    auto iter = s_cacheFontData.find(_fontName);
    if (iter != s_cacheFontData.end())
    {
        iter->second.referenceCount -= 1;
        if (iter->second.referenceCount == 0)
        {
            s_cacheFontData.erase(iter);
        }
    }
}

FontAtlas* FontFreeType::newFontAtlas()
{
    auto fontAtlas = new FontAtlas(this);
    if (_usedGlyphs != GlyphCollection::DYNAMIC)
    {
        std::u32string utf32;
        if (StringUtils::UTF8ToUTF32(getGlyphCollection(), utf32))
        {
            fontAtlas->prepareLetterDefinitions(utf32);
        }
    }
    return fontAtlas;
}

int* FontFreeType::getHorizontalKerningForTextUTF32(const std::u32string& text, int& outNumLetters) const
{
    if (!_fontFace)
        return nullptr;

    outNumLetters = static_cast<int>(text.length());

    if (!outNumLetters)
        return nullptr;

    int* sizes = new int[outNumLetters];
    memset(sizes, 0, outNumLetters * sizeof(int));

    bool hasKerning = FT_HAS_KERNING(_fontFace) != 0;
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
    int glyphIndex1 = FT_Get_Char_Index(_fontFace, static_cast<FT_ULong>(firstChar));

    if (!glyphIndex1)
        return 0;

    // get the ID to the char we need
    int glyphIndex2 = FT_Get_Char_Index(_fontFace, static_cast<FT_ULong>(secondChar));

    if (!glyphIndex2)
        return 0;

    FT_Vector kerning;

    if (FT_Get_Kerning(_fontFace, glyphIndex1, glyphIndex2, FT_KERNING_DEFAULT, &kerning))
        return 0;

    return (static_cast<int>(kerning.x >> 6));
}

int FontFreeType::getFontAscender() const
{
    return _ascender >> 6;
}

const char* FontFreeType::getFontFamily() const
{
    if (!_fontFace)
        return nullptr;

    return _fontFace->family_name;
}

unsigned char* FontFreeType::getGlyphBitmap(uint32_t theChar,
                                            int32_t& outWidth,
                                            int32_t& outHeight,
                                            Rect& outRect,
                                            int& xAdvance)
{
    unsigned char* ret = nullptr;

    do
    {
        if (_fontFace == nullptr)
            break;

        // @remark: glyphIndex=0 means charactor is mssing on current font face
        auto glyphIndex = FT_Get_Char_Index(_fontFace, static_cast<FT_ULong>(theChar));
#if defined(COCOS2D_DEBUG) && COCOS2D_DEBUG > 0
        if (glyphIndex == 0)
        {
            char32_t ntcs[2] = {theChar, (char32_t)0};
            std::u32string_view charUTF32(ntcs, 1);
            std::string charUTF8;
            cocos2d::StringUtils::UTF32ToUTF8(charUTF32, charUTF8);

            if (charUTF8 == "\n")
                charUTF8 = "\\n";
            cocos2d::log("The font face: %s doesn't contains char: <%s>", _fontFace->charmap->face->family_name,
                         charUTF8.c_str());
            return nullptr;
        }
#endif
        if (FT_Load_Glyph(_fontFace, glyphIndex, FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT))
            break;
        if (_distanceFieldEnabled && _fontFace->glyph->bitmap.buffer)
        {
            // Require freetype version > 2.11.0, because freetype 2.11.0 sdf has memory access bug, see:
            // https://gitlab.freedesktop.org/freetype/freetype/-/issues/1077
            FT_Render_Glyph(_fontFace->glyph, FT_Render_Mode::FT_RENDER_MODE_SDF);
        }

        auto& metrics       = _fontFace->glyph->metrics;
        outRect.origin.x    = static_cast<float>(metrics.horiBearingX >> 6);
        outRect.origin.y    = static_cast<float>(-(metrics.horiBearingY >> 6));
        outRect.size.width  = static_cast<float>((metrics.width >> 6));
        outRect.size.height = static_cast<float>((metrics.height >> 6));

        xAdvance = (static_cast<int>(_fontFace->glyph->metrics.horiAdvance >> 6));

        outWidth  = _fontFace->glyph->bitmap.width;
        outHeight = _fontFace->glyph->bitmap.rows;
        ret       = _fontFace->glyph->bitmap.buffer;

        if (_outlineSize > 0 && outWidth > 0 && outHeight > 0)
        {
            auto copyBitmap = new unsigned char[outWidth * outHeight];
            memcpy(copyBitmap, ret, outWidth * outHeight * sizeof(unsigned char));

            FT_BBox bbox;
            auto outlineBitmap = getGlyphBitmapWithOutline(glyphIndex, bbox);
            if (outlineBitmap == nullptr)
            {
                ret = nullptr;
                delete[] copyBitmap;
                break;
            }

            int glyphMinX = (int)outRect.origin.x;
            int glyphMaxX = (int)(outRect.origin.x + outWidth);
            int glyphMinY = (int)(-outHeight - outRect.origin.y);
            int glyphMaxY = (int)-outRect.origin.y;

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

            outRect.origin.x = (float)blendImageMinX;
            outRect.origin.y = -blendImageMaxY + _outlineSize;

            unsigned char* blendImage = nullptr;
            if (blendWidth > 0 && blendHeight > 0)
            {
                FT_Pos index, index2;
                auto imageSize = blendWidth * blendHeight * 2;
                blendImage     = new unsigned char[imageSize];
                memset(blendImage, 0, imageSize);

                auto px = outlineMinX - blendImageMinX;
                auto py = blendImageMaxY - outlineMaxY;
                for (int x = 0; x < outlineWidth; ++x)
                {
                    for (int y = 0; y < outlineHeight; ++y)
                    {
                        index                 = px + x + ((py + y) * blendWidth);
                        index2                = x + (y * outlineWidth);
                        blendImage[2 * index] = outlineBitmap[index2];
                    }
                }

                px = glyphMinX - blendImageMinX;
                py = blendImageMaxY - glyphMaxY;
                for (int x = 0; x < outWidth; ++x)
                {
                    for (int y = 0; y < outHeight; ++y)
                    {
                        index                     = px + x + ((y + py) * blendWidth);
                        index2                    = x + (y * outWidth);
                        blendImage[2 * index + 1] = copyBitmap[index2];
                    }
                }
            }

            outRect.size.width  = (float)blendWidth;
            outRect.size.height = (float)blendHeight;
            outWidth            = blendWidth;
            outHeight           = blendHeight;

            delete[] outlineBitmap;
            delete[] copyBitmap;
            ret = blendImage;
        }

        return ret;
    } while (0);

    outRect.size.width  = 0;
    outRect.size.height = 0;
    xAdvance            = 0;

    return nullptr;
}

unsigned char* FontFreeType::getGlyphBitmapWithOutline(unsigned int glyphIndex, FT_BBox& bbox)
{
    unsigned char* ret = nullptr;
    if (FT_Load_Glyph(_fontFace, glyphIndex, FT_LOAD_NO_BITMAP) == 0)
    {
        if (_fontFace->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
        {
            FT_Glyph glyph;
            if (FT_Get_Glyph(_fontFace->glyph, &glyph) == 0)
            {
                FT_Glyph_StrokeBorder(&glyph, _stroker, 0, 1);
                if (glyph->format == FT_GLYPH_FORMAT_OUTLINE)
                {
                    FT_Outline* outline = &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;
                    FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_GRIDFIT, &bbox);
                    int32_t width = (bbox.xMax - bbox.xMin) >> 6;
                    int32_t rows  = (bbox.yMax - bbox.yMin) >> 6;

                    FT_Bitmap bmp;
                    bmp.buffer = new unsigned char[width * rows];
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

void FontFreeType::renderCharAt(unsigned char* dest,
                                int posX,
                                int posY,
                                unsigned char* bitmap,
                                int32_t bitmapWidth,
                                int32_t bitmapHeight)
{
    int iX = posX;
    int iY = posY;

    if (_outlineSize > 0)
    {
        unsigned char tempChar;
        for (int32_t y = 0; y < bitmapHeight; ++y)
        {
            int32_t bitmap_y = y * bitmapWidth;

            for (int x = 0; x < bitmapWidth; ++x)
            {
                tempChar                                                 = bitmap[(bitmap_y + x) * 2];
                dest[(iX + (iY * FontAtlas::CacheTextureWidth)) * 2]     = tempChar;
                tempChar                                                 = bitmap[(bitmap_y + x) * 2 + 1];
                dest[(iX + (iY * FontAtlas::CacheTextureWidth)) * 2 + 1] = tempChar;

                iX += 1;
            }

            iX = posX;
            iY += 1;
        }
        delete[] bitmap;
    }
    else
    {
        for (int32_t y = 0; y < bitmapHeight; ++y)
        {
            int32_t bitmap_y = y * bitmapWidth;

            for (int x = 0; x < bitmapWidth; ++x)
            {
                unsigned char cTemp = bitmap[bitmap_y + x];

                // the final pixel
                dest[(iX + (iY * FontAtlas::CacheTextureWidth))] = cTemp;

                iX += 1;
            }

            iX = posX;
            iY += 1;
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
    case cocos2d::GlyphCollection::DYNAMIC:
        break;
    case cocos2d::GlyphCollection::NEHE:
        glyphCollection = _glyphNEHE;
        break;
    case cocos2d::GlyphCollection::ASCII:
        glyphCollection = _glyphASCII;
        break;
    case cocos2d::GlyphCollection::CUSTOM:
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

NS_CC_END
