/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

//
// Copyright 2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

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

#include "base/etc2.h"
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <type_traits>
#include <algorithm>
#include <limits>

static const char ketc2Magic[] = {'P', 'K', 'M', ' ', '2', '0'};

static const etc2_uint32 ETC2_PKM_FORMAT_OFFSET         = 6;
static const etc2_uint32 ETC2_PKM_ENCODED_WIDTH_OFFSET  = 8;
static const etc2_uint32 ETC2_PKM_ENCODED_HEIGHT_OFFSET = 10;
static const etc2_uint32 ETC2_PKM_WIDTH_OFFSET          = 12;
static const etc2_uint32 ETC2_PKM_HEIGHT_OFFSET         = 14;

static void writeBEUint16(etc2_byte* pOut, etc2_uint32 data)
{
    pOut[0] = (etc2_byte)(data >> 8);
    pOut[1] = (etc2_byte)data;
}

static etc2_uint32 etc2ReadBEUint16(const etc2_byte* pIn)
{
    return (pIn[0] << 8) | pIn[1];
}

// Check if a PKM header is correctly formatted.

etc2_bool etc2_pkm_is_valid(const etc2_byte* pHeader)
{
    if (memcmp(pHeader, ketc2Magic, sizeof(ketc2Magic)))
    {
        return false;
    }
    etc2_uint32 format        = etc2ReadBEUint16(pHeader + ETC2_PKM_FORMAT_OFFSET);
    etc2_uint32 encodedWidth  = etc2ReadBEUint16(pHeader + ETC2_PKM_ENCODED_WIDTH_OFFSET);
    etc2_uint32 encodedHeight = etc2ReadBEUint16(pHeader + ETC2_PKM_ENCODED_HEIGHT_OFFSET);
    etc2_uint32 width         = etc2ReadBEUint16(pHeader + ETC2_PKM_WIDTH_OFFSET);
    etc2_uint32 height        = etc2ReadBEUint16(pHeader + ETC2_PKM_HEIGHT_OFFSET);
    return (format == ETC2_RGB_NO_MIPMAPS || format == ETC2_RGBA_NO_MIPMAPS) && encodedWidth >= width &&
           encodedWidth - width < 4 && encodedHeight >= height && encodedHeight - height < 4;
}

// Read the image width from a PKM header

etc2_uint32 etc2_pkm_get_width(const etc2_byte* pHeader)
{
    return etc2ReadBEUint16(pHeader + ETC2_PKM_WIDTH_OFFSET);
}

// Read the image height from a PKM header

etc2_uint32 etc2_pkm_get_height(const etc2_byte* pHeader)
{
    return etc2ReadBEUint16(pHeader + ETC2_PKM_HEIGHT_OFFSET);
}

etc2_uint32 etc2_pkm_get_format(const etc2_byte* pHeader)
{
    return etc2ReadBEUint16(pHeader + ETC2_PKM_FORMAT_OFFSET);
}

// The etc2 software decode implementation is modified from angleproject, and only support ETC2_RGB and ETC2_RGBA
#define UNREACHABLE()

namespace
{
namespace priv
{
template <typename T>
inline T* OffsetDataPointer(uint8_t* data, size_t y, size_t z, size_t rowPitch, size_t depthPitch)
{
    return reinterpret_cast<T*>(data + (y * rowPitch) + (z * depthPitch));
}

template <typename T>
inline const T* OffsetDataPointer(const uint8_t* data, size_t y, size_t z, size_t rowPitch, size_t depthPitch)
{
    return reinterpret_cast<const T*>(data + (y * rowPitch) + (z * depthPitch));
}
}  // namespace priv

namespace gl
{
template <typename _Ty>
const _Ty& clamp(const _Ty& v, const _Ty& lo, const _Ty& hi)
{
    assert(!(hi < lo));
    return v < lo ? lo : hi < v ? hi : v;
}
// https://stackoverflow.com/a/37581284
template <typename T>
static constexpr double normalize(T value)
{
    return value < 0 ? -static_cast<double>(value) / std::numeric_limits<T>::min()
                     : static_cast<double>(value) / std::numeric_limits<T>::max();
}

template <typename destType, typename sourceType>
destType bitCast(const sourceType& source)
{
    size_t copySize = std::min(sizeof(destType), sizeof(sourceType));
    destType output;
    memcpy(&output, &source, copySize);
    return output;
}

inline unsigned short float32ToFloat16(float fp32)
{
    unsigned int fp32i = bitCast<unsigned int>(fp32);
    unsigned int sign  = (fp32i & 0x80000000) >> 16;
    unsigned int abs   = fp32i & 0x7FFFFFFF;

    if (abs > 0x7F800000)
    {  // NaN
        return 0x7FFF;
    }
    else if (abs > 0x47FFEFFF)
    {  // Infinity
        return static_cast<uint16_t>(sign | 0x7C00);
    }
    else if (abs < 0x38800000)  // Denormal
    {
        unsigned int mantissa = (abs & 0x007FFFFF) | 0x00800000;
        int e                 = 113 - (abs >> 23);

        if (e < 24)
        {
            abs = mantissa >> e;
        }
        else
        {
            abs = 0;
        }

        return static_cast<unsigned short>(sign | (abs + 0x00000FFF + ((abs >> 13) & 1)) >> 13);
    }
    else
    {
        return static_cast<unsigned short>(sign | (abs + 0xC8000000 + 0x00000FFF + ((abs >> 13) & 1)) >> 13);
    }
}
}  // namespace gl

/* The ETC format load from google/angle */
struct R8G8B8A8
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t A;
};

/* The ETC format load from google/angle */

using IntensityModifier = const int[4];

// Table 3.17.2 sorted according to table 3.17.3
// clang-format off
    static IntensityModifier intensityModifierDefault[] =
    {
        {  2,   8,  -2,   -8 },
        {  5,  17,  -5,  -17 },
        {  9,  29,  -9,  -29 },
        { 13,  42, -13,  -42 },
        { 18,  60, -18,  -60 },
        { 24,  80, -24,  -80 },
        { 33, 106, -33, -106 },
        { 47, 183, -47, -183 },
    };
// clang-format on

// Table C.12, intensity modifier for non opaque punchthrough alpha
// clang-format off
    static IntensityModifier intensityModifierNonOpaque[] =
    {
        { 0,   8, 0,   -8 },
        { 0,  17, 0,  -17 },
        { 0,  29, 0,  -29 },
        { 0,  42, 0,  -42 },
        { 0,  60, 0,  -60 },
        { 0,  80, 0,  -80 },
        { 0, 106, 0, -106 },
        { 0, 183, 0, -183 },
    };
// clang-format on

static const int kNumPixelsInBlock = 16;

struct ETC2Block
{
    // Decodes unsigned single or dual channel ETC2 block to 8-bit color
    void decodeAsSingleETC2Channel(uint8_t* dest,
                                   size_t x,
                                   size_t y,
                                   size_t w,
                                   size_t h,
                                   size_t destPixelStride,
                                   size_t destRowPitch,
                                   bool isSigned) const
    {
        for (size_t j = 0; j < 4 && (y + j) < h; j++)
        {
            uint8_t* row = dest + (j * destRowPitch);
            for (size_t i = 0; i < 4 && (x + i) < w; i++)
            {
                uint8_t* pixel = row + (i * destPixelStride);
                if (isSigned)
                {
                    *pixel = clampSByte(getSingleETC2Channel(i, j, isSigned));
                }
                else
                {
                    *pixel = clampByte(getSingleETC2Channel(i, j, isSigned));
                }
            }
        }
    }

    // Decodes RGB block to rgba8
    void decodeAsRGB(uint8_t* dest,
                     size_t x,
                     size_t y,
                     size_t w,
                     size_t h,
                     size_t destRowPitch,
                     const uint8_t alphaValues[4][4],
                     bool punchThroughAlpha) const
    {
        bool opaqueBit                  = u.idht.mode.idm.diffbit;
        bool nonOpaquePunchThroughAlpha = punchThroughAlpha && !opaqueBit;
        // Select mode
        if (u.idht.mode.idm.diffbit || punchThroughAlpha)
        {
            const auto& block = u.idht.mode.idm.colors.diff;
            int r             = (block.R + block.dR);
            int g             = (block.G + block.dG);
            int b             = (block.B + block.dB);
            if (r < 0 || r > 31)
            {
                decodeTBlock(dest, x, y, w, h, destRowPitch, alphaValues, nonOpaquePunchThroughAlpha);
            }
            else if (g < 0 || g > 31)
            {
                decodeHBlock(dest, x, y, w, h, destRowPitch, alphaValues, nonOpaquePunchThroughAlpha);
            }
            else if (b < 0 || b > 31)
            {
                decodePlanarBlock(dest, x, y, w, h, destRowPitch, alphaValues);
            }
            else
            {
                decodeDifferentialBlock(dest, x, y, w, h, destRowPitch, alphaValues, nonOpaquePunchThroughAlpha);
            }
        }
        else
        {
            decodeIndividualBlock(dest, x, y, w, h, destRowPitch, alphaValues, nonOpaquePunchThroughAlpha);
        }
    }

private:
    union
    {
        // Individual, differential, H and T modes
        struct
        {
            union
            {
                // Individual and differential modes
                struct
                {
                    union
                    {
                        struct  // Individual colors
                        {
                            unsigned char R2 : 4;
                            unsigned char R1 : 4;
                            unsigned char G2 : 4;
                            unsigned char G1 : 4;
                            unsigned char B2 : 4;
                            unsigned char B1 : 4;
                        } indiv;
                        struct  // Differential colors
                        {
                            signed char dR : 3;
                            unsigned char R : 5;
                            signed char dG : 3;
                            unsigned char G : 5;
                            signed char dB : 3;
                            unsigned char B : 5;
                        } diff;
                    } colors;
                    bool flipbit : 1;
                    bool diffbit : 1;
                    unsigned char cw2 : 3;
                    unsigned char cw1 : 3;
                } idm;
                // T mode
                struct
                {
                    // Byte 1
                    unsigned char TR1b : 2;
                    unsigned char TunusedB : 1;
                    unsigned char TR1a : 2;
                    unsigned char TunusedA : 3;
                    // Byte 2
                    unsigned char TB1 : 4;
                    unsigned char TG1 : 4;
                    // Byte 3
                    unsigned char TG2 : 4;
                    unsigned char TR2 : 4;
                    // Byte 4
                    unsigned char Tdb : 1;
                    bool Tflipbit : 1;
                    unsigned char Tda : 2;
                    unsigned char TB2 : 4;
                } tm;
                // H mode
                struct
                {
                    // Byte 1
                    unsigned char HG1a : 3;
                    unsigned char HR1 : 4;
                    unsigned char HunusedA : 1;
                    // Byte 2
                    unsigned char HB1b : 2;
                    unsigned char HunusedC : 1;
                    unsigned char HB1a : 1;
                    unsigned char HG1b : 1;
                    unsigned char HunusedB : 3;
                    // Byte 3
                    unsigned char HG2a : 3;
                    unsigned char HR2 : 4;
                    unsigned char HB1c : 1;
                    // Byte 4
                    unsigned char Hdb : 1;
                    bool Hflipbit : 1;
                    unsigned char Hda : 1;
                    unsigned char HB2 : 4;
                    unsigned char HG2b : 1;
                } hm;
            } mode;
            unsigned char pixelIndexMSB[2];
            unsigned char pixelIndexLSB[2];
        } idht;
        // planar mode
        struct
        {
            // Byte 1
            unsigned char GO1 : 1;
            unsigned char RO : 6;
            unsigned char PunusedA : 1;
            // Byte 2
            unsigned char BO1 : 1;
            unsigned char GO2 : 6;
            unsigned char PunusedB : 1;
            // Byte 3
            unsigned char BO3a : 2;
            unsigned char PunusedD : 1;
            unsigned char BO2 : 2;
            unsigned char PunusedC : 3;
            // Byte 4
            unsigned char RH2 : 1;
            bool Pflipbit : 1;
            unsigned char RH1 : 5;
            unsigned char BO3b : 1;
            // Byte 5
            unsigned char BHa : 1;
            unsigned char GH : 7;
            // Byte 6
            unsigned char RVa : 3;
            unsigned char BHb : 5;
            // Byte 7
            unsigned char GVa : 5;
            unsigned char RVb : 3;
            // Byte 8
            unsigned char BV : 6;
            unsigned char GVb : 2;
        } pblk;
        // Single channel block
        struct
        {
            union
            {
                unsigned char us;
                signed char s;
            } base_codeword;
            unsigned char table_index : 4;
            unsigned char multiplier : 4;
            unsigned char mc1 : 2;
            unsigned char mb : 3;
            unsigned char ma : 3;
            unsigned char mf1 : 1;
            unsigned char me : 3;
            unsigned char md : 3;
            unsigned char mc2 : 1;
            unsigned char mh : 3;
            unsigned char mg : 3;
            unsigned char mf2 : 2;
            unsigned char mk1 : 2;
            unsigned char mj : 3;
            unsigned char mi : 3;
            unsigned char mn1 : 1;
            unsigned char mm : 3;
            unsigned char ml : 3;
            unsigned char mk2 : 1;
            unsigned char mp : 3;
            unsigned char mo : 3;
            unsigned char mn2 : 2;
        } scblk;
    } u;

    static unsigned char clampByte(int value) { return static_cast<unsigned char>(gl::clamp(value, 0, 255)); }

    static signed char clampSByte(int value) { return static_cast<signed char>(gl::clamp(value, -128, 127)); }

    template <typename T>
    static T renormalizeEAC(int value)
    {
        int upper = 0;
        int lower = 0;
        int shift = 0;

        if (std::is_same<T, int16_t>::value)
        {
            // The spec states that -1024 invalid and should be clamped to -1023
            upper = 1023;
            lower = -1023;
            shift = 5;
        }
        else if (std::is_same<T, uint16_t>::value)
        {
            upper = 2047;
            lower = 0;
            shift = 5;
        }
        else
        {
            // We currently only support renormalizing int16_t or uint16_t
            UNREACHABLE();
        }

        return static_cast<T>(gl::clamp(value, lower, upper)) << shift;
    }

    static R8G8B8A8 createRGBA(int red, int green, int blue, int alpha)
    {
        R8G8B8A8 rgba;
        rgba.R = clampByte(red);
        rgba.G = clampByte(green);
        rgba.B = clampByte(blue);
        rgba.A = clampByte(alpha);
        return rgba;
    }

    static R8G8B8A8 createRGBA(int red, int green, int blue) { return createRGBA(red, green, blue, 255); }

    static int extend_4to8bits(int x) { return (x << 4) | x; }
    static int extend_5to8bits(int x) { return (x << 3) | (x >> 2); }
    static int extend_6to8bits(int x) { return (x << 2) | (x >> 4); }
    static int extend_7to8bits(int x) { return (x << 1) | (x >> 6); }

    void decodeIndividualBlock(uint8_t* dest,
                               size_t x,
                               size_t y,
                               size_t w,
                               size_t h,
                               size_t destRowPitch,
                               const uint8_t alphaValues[4][4],
                               bool nonOpaquePunchThroughAlpha) const
    {
        const auto& block = u.idht.mode.idm.colors.indiv;
        int r1            = extend_4to8bits(block.R1);
        int g1            = extend_4to8bits(block.G1);
        int b1            = extend_4to8bits(block.B1);
        int r2            = extend_4to8bits(block.R2);
        int g2            = extend_4to8bits(block.G2);
        int b2            = extend_4to8bits(block.B2);
        decodeIndividualOrDifferentialBlock(dest, x, y, w, h, destRowPitch, r1, g1, b1, r2, g2, b2, alphaValues,
                                            nonOpaquePunchThroughAlpha);
    }

    void decodeDifferentialBlock(uint8_t* dest,
                                 size_t x,
                                 size_t y,
                                 size_t w,
                                 size_t h,
                                 size_t destRowPitch,
                                 const uint8_t alphaValues[4][4],
                                 bool nonOpaquePunchThroughAlpha) const
    {
        const auto& block = u.idht.mode.idm.colors.diff;
        int b1            = extend_5to8bits(block.B);
        int g1            = extend_5to8bits(block.G);
        int r1            = extend_5to8bits(block.R);
        int r2            = extend_5to8bits(block.R + block.dR);
        int g2            = extend_5to8bits(block.G + block.dG);
        int b2            = extend_5to8bits(block.B + block.dB);
        decodeIndividualOrDifferentialBlock(dest, x, y, w, h, destRowPitch, r1, g1, b1, r2, g2, b2, alphaValues,
                                            nonOpaquePunchThroughAlpha);
    }

    void decodeIndividualOrDifferentialBlock(uint8_t* dest,
                                             size_t x,
                                             size_t y,
                                             size_t w,
                                             size_t h,
                                             size_t destRowPitch,
                                             int r1,
                                             int g1,
                                             int b1,
                                             int r2,
                                             int g2,
                                             int b2,
                                             const uint8_t alphaValues[4][4],
                                             bool nonOpaquePunchThroughAlpha) const
    {
        const IntensityModifier* intensityModifier =
            nonOpaquePunchThroughAlpha ? intensityModifierNonOpaque : intensityModifierDefault;

        R8G8B8A8 subblockColors0[4];
        R8G8B8A8 subblockColors1[4];
        for (size_t modifierIdx = 0; modifierIdx < 4; modifierIdx++)
        {
            const int i1                 = intensityModifier[u.idht.mode.idm.cw1][modifierIdx];
            subblockColors0[modifierIdx] = createRGBA(r1 + i1, g1 + i1, b1 + i1);

            const int i2                 = intensityModifier[u.idht.mode.idm.cw2][modifierIdx];
            subblockColors1[modifierIdx] = createRGBA(r2 + i2, g2 + i2, b2 + i2);
        }

        if (u.idht.mode.idm.flipbit)
        {
            uint8_t* curPixel = dest;
            for (size_t j = 0; j < 2 && (y + j) < h; j++)
            {
                R8G8B8A8* row = reinterpret_cast<R8G8B8A8*>(curPixel);
                for (size_t i = 0; i < 4 && (x + i) < w; i++)
                {
                    row[i]   = subblockColors0[getIndex(i, j)];
                    row[i].A = alphaValues[j][i];
                }
                curPixel += destRowPitch;
            }
            for (size_t j = 2; j < 4 && (y + j) < h; j++)
            {
                R8G8B8A8* row = reinterpret_cast<R8G8B8A8*>(curPixel);
                for (size_t i = 0; i < 4 && (x + i) < w; i++)
                {
                    row[i]   = subblockColors1[getIndex(i, j)];
                    row[i].A = alphaValues[j][i];
                }
                curPixel += destRowPitch;
            }
        }
        else
        {
            uint8_t* curPixel = dest;
            for (size_t j = 0; j < 4 && (y + j) < h; j++)
            {
                R8G8B8A8* row = reinterpret_cast<R8G8B8A8*>(curPixel);
                for (size_t i = 0; i < 2 && (x + i) < w; i++)
                {
                    row[i]   = subblockColors0[getIndex(i, j)];
                    row[i].A = alphaValues[j][i];
                }
                for (size_t i = 2; i < 4 && (x + i) < w; i++)
                {
                    row[i]   = subblockColors1[getIndex(i, j)];
                    row[i].A = alphaValues[j][i];
                }
                curPixel += destRowPitch;
            }
        }
        if (nonOpaquePunchThroughAlpha)
        {
            decodePunchThroughAlphaBlock(dest, x, y, w, h, destRowPitch);
        }
    }

    void decodeTBlock(uint8_t* dest,
                      size_t x,
                      size_t y,
                      size_t w,
                      size_t h,
                      size_t destRowPitch,
                      const uint8_t alphaValues[4][4],
                      bool nonOpaquePunchThroughAlpha) const
    {
        // Table C.8, distance index for T and H modes
        const auto& block = u.idht.mode.tm;

        int r1 = extend_4to8bits(block.TR1a << 2 | block.TR1b);
        int g1 = extend_4to8bits(block.TG1);
        int b1 = extend_4to8bits(block.TB1);
        int r2 = extend_4to8bits(block.TR2);
        int g2 = extend_4to8bits(block.TG2);
        int b2 = extend_4to8bits(block.TB2);

        static int distance[8] = {3, 6, 11, 16, 23, 32, 41, 64};
        const int d            = distance[block.Tda << 1 | block.Tdb];

        const R8G8B8A8 paintColors[4] = {
            createRGBA(r1, g1, b1),
            createRGBA(r2 + d, g2 + d, b2 + d),
            createRGBA(r2, g2, b2),
            createRGBA(r2 - d, g2 - d, b2 - d),
        };

        uint8_t* curPixel = dest;
        for (size_t j = 0; j < 4 && (y + j) < h; j++)
        {
            R8G8B8A8* row = reinterpret_cast<R8G8B8A8*>(curPixel);
            for (size_t i = 0; i < 4 && (x + i) < w; i++)
            {
                row[i]   = paintColors[getIndex(i, j)];
                row[i].A = alphaValues[j][i];
            }
            curPixel += destRowPitch;
        }

        if (nonOpaquePunchThroughAlpha)
        {
            decodePunchThroughAlphaBlock(dest, x, y, w, h, destRowPitch);
        }
    }

    void decodeHBlock(uint8_t* dest,
                      size_t x,
                      size_t y,
                      size_t w,
                      size_t h,
                      size_t destRowPitch,
                      const uint8_t alphaValues[4][4],
                      bool nonOpaquePunchThroughAlpha) const
    {
        // Table C.8, distance index for T and H modes
        const auto& block = u.idht.mode.hm;

        int r1 = extend_4to8bits(block.HR1);
        int g1 = extend_4to8bits(block.HG1a << 1 | block.HG1b);
        int b1 = extend_4to8bits(block.HB1a << 3 | block.HB1b << 1 | block.HB1c);
        int r2 = extend_4to8bits(block.HR2);
        int g2 = extend_4to8bits(block.HG2a << 1 | block.HG2b);
        int b2 = extend_4to8bits(block.HB2);

        static const int distance[8] = {3, 6, 11, 16, 23, 32, 41, 64};
        const int orderingTrickBit   = ((r1 << 16 | g1 << 8 | b1) >= (r2 << 16 | g2 << 8 | b2) ? 1 : 0);
        const int d                  = distance[(block.Hda << 2) | (block.Hdb << 1) | orderingTrickBit];

        const R8G8B8A8 paintColors[4] = {
            createRGBA(r1 + d, g1 + d, b1 + d),
            createRGBA(r1 - d, g1 - d, b1 - d),
            createRGBA(r2 + d, g2 + d, b2 + d),
            createRGBA(r2 - d, g2 - d, b2 - d),
        };

        uint8_t* curPixel = dest;
        for (size_t j = 0; j < 4 && (y + j) < h; j++)
        {
            R8G8B8A8* row = reinterpret_cast<R8G8B8A8*>(curPixel);
            for (size_t i = 0; i < 4 && (x + i) < w; i++)
            {
                row[i]   = paintColors[getIndex(i, j)];
                row[i].A = alphaValues[j][i];
            }
            curPixel += destRowPitch;
        }

        if (nonOpaquePunchThroughAlpha)
        {
            decodePunchThroughAlphaBlock(dest, x, y, w, h, destRowPitch);
        }
    }

    void decodePlanarBlock(uint8_t* dest,
                           size_t x,
                           size_t y,
                           size_t w,
                           size_t h,
                           size_t pitch,
                           const uint8_t alphaValues[4][4]) const
    {
        int ro = extend_6to8bits(u.pblk.RO);
        int go = extend_7to8bits(u.pblk.GO1 << 6 | u.pblk.GO2);
        int bo = extend_6to8bits(u.pblk.BO1 << 5 | u.pblk.BO2 << 3 | u.pblk.BO3a << 1 | u.pblk.BO3b);
        int rh = extend_6to8bits(u.pblk.RH1 << 1 | u.pblk.RH2);
        int gh = extend_7to8bits(u.pblk.GH);
        int bh = extend_6to8bits(u.pblk.BHa << 5 | u.pblk.BHb);
        int rv = extend_6to8bits(u.pblk.RVa << 3 | u.pblk.RVb);
        int gv = extend_7to8bits(u.pblk.GVa << 2 | u.pblk.GVb);
        int bv = extend_6to8bits(u.pblk.BV);

        uint8_t* curPixel = dest;
        for (size_t j = 0; j < 4 && (y + j) < h; j++)
        {
            R8G8B8A8* row = reinterpret_cast<R8G8B8A8*>(curPixel);

            int ry = static_cast<int>(j) * (rv - ro) + 2;
            int gy = static_cast<int>(j) * (gv - go) + 2;
            int by = static_cast<int>(j) * (bv - bo) + 2;
            for (size_t i = 0; i < 4 && (x + i) < w; i++)
            {
                row[i] = createRGBA(((static_cast<int>(i) * (rh - ro) + ry) >> 2) + ro,
                                    ((static_cast<int>(i) * (gh - go) + gy) >> 2) + go,
                                    ((static_cast<int>(i) * (bh - bo) + by) >> 2) + bo, alphaValues[j][i]);
            }
            curPixel += pitch;
        }
    }

    // Index for individual, differential, H and T modes
    size_t getIndex(size_t x, size_t y) const
    {
        size_t bitIndex  = x * 4 + y;
        size_t bitOffset = bitIndex & 7;
        size_t lsb       = (u.idht.pixelIndexLSB[1 - (bitIndex >> 3)] >> bitOffset) & 1;
        size_t msb       = (u.idht.pixelIndexMSB[1 - (bitIndex >> 3)] >> bitOffset) & 1;
        return (msb << 1) | lsb;
    }

    void decodePunchThroughAlphaBlock(uint8_t* dest, size_t x, size_t y, size_t w, size_t h, size_t destRowPitch) const
    {
        uint8_t* curPixel = dest;
        for (size_t j = 0; j < 4 && (y + j) < h; j++)
        {
            R8G8B8A8* row = reinterpret_cast<R8G8B8A8*>(curPixel);
            for (size_t i = 0; i < 4 && (x + i) < w; i++)
            {
                if (getIndex(i, j) == 2)  //  msb == 1 && lsb == 0
                {
                    row[i] = createRGBA(0, 0, 0, 0);
                }
            }
            curPixel += destRowPitch;
        }
    }

    uint16_t RGB8ToRGB565(const R8G8B8A8& rgba) const
    {
        return (static_cast<uint16_t>(rgba.R >> 3) << 11) | (static_cast<uint16_t>(rgba.G >> 2) << 5) |
               (static_cast<uint16_t>(rgba.B >> 3) << 0);
    }

    // Single channel utility functions
    int getSingleEACChannel(size_t x, size_t y, bool isSigned) const
    {
        int codeword   = isSigned ? u.scblk.base_codeword.s : u.scblk.base_codeword.us;
        int multiplier = (u.scblk.multiplier == 0) ? 1 : u.scblk.multiplier * 8;
        return codeword * 8 + 4 + getSingleChannelModifier(x, y) * multiplier;
    }

    int getSingleETC2Channel(size_t x, size_t y, bool isSigned) const
    {
        int codeword = isSigned ? u.scblk.base_codeword.s : u.scblk.base_codeword.us;
        return codeword + getSingleChannelModifier(x, y) * u.scblk.multiplier;
    }

    int getSingleChannelIndex(size_t x, size_t y) const
    {
        assert(x < 4 && y < 4);

        // clang-format off
            switch (x * 4 + y)
            {
            case 0: return u.scblk.ma;
            case 1: return u.scblk.mb;
            case 2: return u.scblk.mc1 << 1 | u.scblk.mc2;
            case 3: return u.scblk.md;
            case 4: return u.scblk.me;
            case 5: return u.scblk.mf1 << 2 | u.scblk.mf2;
            case 6: return u.scblk.mg;
            case 7: return u.scblk.mh;
            case 8: return u.scblk.mi;
            case 9: return u.scblk.mj;
            case 10: return u.scblk.mk1 << 1 | u.scblk.mk2;
            case 11: return u.scblk.ml;
            case 12: return u.scblk.mm;
            case 13: return u.scblk.mn1 << 2 | u.scblk.mn2;
            case 14: return u.scblk.mo;
            case 15: return u.scblk.mp;
            default: UNREACHABLE(); return 0;
            }
        // clang-format on
    }

    int getSingleChannelModifier(size_t x, size_t y) const
    {
        // clang-format off
            static const int modifierTable[16][8] =
            {
                { -3, -6,  -9, -15, 2, 5, 8, 14 },
                { -3, -7, -10, -13, 2, 6, 9, 12 },
                { -2, -5,  -8, -13, 1, 4, 7, 12 },
                { -2, -4,  -6, -13, 1, 3, 5, 12 },
                { -3, -6,  -8, -12, 2, 5, 7, 11 },
                { -3, -7,  -9, -11, 2, 6, 8, 10 },
                { -4, -7,  -8, -11, 3, 6, 7, 10 },
                { -3, -5,  -8, -11, 2, 4, 7, 10 },
                { -2, -6,  -8, -10, 1, 5, 7,  9 },
                { -2, -5,  -8, -10, 1, 4, 7,  9 },
                { -2, -4,  -8, -10, 1, 3, 7,  9 },
                { -2, -5,  -7, -10, 1, 4, 6,  9 },
                { -3, -4,  -7, -10, 2, 3, 6,  9 },
                { -1, -2,  -3, -10, 0, 1, 2,  9 },
                { -4, -6,  -8,  -9, 3, 5, 7,  8 },
                { -3, -5,  -7,  -9, 2, 4, 6,  8 }
            };
        // clang-format on

        return modifierTable[u.scblk.table_index][getSingleChannelIndex(x, y)];
    }
};

// clang-format off
    static const uint8_t DefaultETCAlphaValues[4][4] =
    {
        { 255, 255, 255, 255 },
        { 255, 255, 255, 255 },
        { 255, 255, 255, 255 },
        { 255, 255, 255, 255 },
    };

    void LoadETC2RGB8ToRGBA8(size_t width,
        size_t height,
        size_t depth,
        const uint8_t* input,
        size_t inputRowPitch,
        size_t inputDepthPitch,
        uint8_t* output,
        size_t outputRowPitch,
        size_t outputDepthPitch,
        bool punchthroughAlpha)
    {
        for (size_t z = 0; z < depth; z++)
        {
            for (size_t y = 0; y < height; y += 4)
            {
                const ETC2Block* sourceRow =
                    priv::OffsetDataPointer<ETC2Block>(input, y / 4, z, inputRowPitch, inputDepthPitch);
                uint8_t* destRow =
                    priv::OffsetDataPointer<uint8_t>(output, y, z, outputRowPitch, outputDepthPitch);

                for (size_t x = 0; x < width; x += 4)
                {
                    const ETC2Block* sourceBlock = sourceRow + (x / 4);
                    uint8_t* destPixels = destRow + (x * 4);

                    sourceBlock->decodeAsRGB(destPixels, x, y, width, height, outputRowPitch,
                        DefaultETCAlphaValues, punchthroughAlpha);
                }
            }
        }
    }

    void LoadETC2RGBA8ToRGBA8(size_t width,
        size_t height,
        size_t depth,
        const uint8_t* input,
        size_t inputRowPitch,
        size_t inputDepthPitch,
        uint8_t* output,
        size_t outputRowPitch,
        size_t outputDepthPitch,
        bool srgb)
    {
        uint8_t decodedAlphaValues[4][4];

        for (size_t z = 0; z < depth; z++)
        {
            for (size_t y = 0; y < height; y += 4)
            {
                const ETC2Block* sourceRow =
                    priv::OffsetDataPointer<ETC2Block>(input, y / 4, z, inputRowPitch, inputDepthPitch);
                uint8_t* destRow =
                    priv::OffsetDataPointer<uint8_t>(output, y, z, outputRowPitch, outputDepthPitch);

                for (size_t x = 0; x < width; x += 4)
                {
                    const ETC2Block* sourceBlockAlpha = sourceRow + (x / 2);
                    sourceBlockAlpha->decodeAsSingleETC2Channel(
                        reinterpret_cast<uint8_t*>(decodedAlphaValues), x, y, width, height, 1, 4,
                        false);

                    uint8_t* destPixels = destRow + (x * 4);
                    const ETC2Block* sourceBlockRGB = sourceBlockAlpha + 1;
                    sourceBlockRGB->decodeAsRGB(destPixels, x, y, width, height, outputRowPitch,
                        decodedAlphaValues, false);
                }
            }
        }
    }

    void LoadETC2RGB8ToRGBA8(size_t width,
        size_t height,
        size_t depth,
        const uint8_t* input,
        size_t inputRowPitch,
        size_t inputDepthPitch,
        uint8_t* output,
        size_t outputRowPitch,
        size_t outputDepthPitch)
    {
        return LoadETC2RGB8ToRGBA8(width, height, depth, input, inputRowPitch, inputDepthPitch, output, outputRowPitch, outputDepthPitch, false);
    }

    void LoadETC2RGBA8ToRGBA8(size_t width,
        size_t height,
        size_t depth,
        const uint8_t* input,
        size_t inputRowPitch,
        size_t inputDepthPitch,
        uint8_t* output,
        size_t outputRowPitch,
        size_t outputDepthPitch) {
        return LoadETC2RGBA8ToRGBA8(width, height, depth, input, inputRowPitch, inputDepthPitch, output, outputRowPitch, outputDepthPitch, false);
    }

    size_t ComputeETC2RowPitch(size_t width, size_t blockWidth, size_t bytesPerPixel) {
        return ((width + blockWidth - 1) / blockWidth) * bytesPerPixel;
    }

    size_t ComputeETC2DepthPitch(size_t height, size_t blockHeight, size_t rowPitch)
    {
        return ((height + blockHeight - 1) / blockHeight) * rowPitch;
    }

    typedef void (*LoadTextureFunction)(size_t width,
        size_t height,
        size_t depth,
        const uint8_t* input,
        size_t inputRowPitch,
        size_t inputDepthPitch,
        uint8_t* output,
        size_t outputRowPitch,
        size_t outputDepthPitch);
}

int etc2_decode_image(int format, const etc2_byte* input, etc2_byte* output, etc2_uint32 width, etc2_uint32 height)
{
    size_t outputRowPitch = 4 * width;
    size_t outputDepthPitch = outputRowPitch * height;

    size_t bytesPerPixel = 0;
    LoadTextureFunction loadTexture = nullptr;
    switch (format) {
    case ETC2_RGBA_NO_MIPMAPS:
        bytesPerPixel = 16;
        loadTexture = &LoadETC2RGBA8ToRGBA8;
        break;
    case ETC2_RGB_NO_MIPMAPS:
        bytesPerPixel = 8;
        loadTexture = &LoadETC2RGB8ToRGBA8;
        break;
    }

    if (loadTexture) {
        size_t inputRowPitch = ComputeETC2RowPitch(width, 4 /*blockWidth*/, bytesPerPixel);
        size_t inputDepthPitch = ComputeETC2DepthPitch(height, 4 /*blockHeight*/, inputRowPitch);
        loadTexture(width, height, 1, input, inputRowPitch, inputDepthPitch, output, outputRowPitch, outputDepthPitch);
        return 0;
    }

    return -1;
}
