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

#include <stdint.h>

#define KTX_V1_HEADER_SIZE 64

#define KTX_V1_MAGIC "KTX 11"

// ktxv1 header, refer to: https://www.khronos.org/registry/KTX/specs/1.0/ktxspec_v1.html
struct KTXv1Header
{
    struct InternalFormat
    {
        enum
        {
            // ETC1
            ETC1_RGB8   = 0x8D64,
            ETC1_ALPHA8 = ETC1_RGB8,
            // ETC2
            ETC2_R11                       = 0x9270,
            ETC2_SIGNED_R11                = 0x9271,
            ETC2_RG11                      = 0x9272,
            ETC2_SIGNED_RG11               = 0x9273,
            ETC2_RGB8                      = 0x9274,
            ETC2_SRGB8                     = 0x9275,
            ETC2_RGB8A1                    = 0x9276,
            ETC2_SRGB8_PUNCHTHROUGH_ALPHA1 = 0x9277,
            ETC2_RGBA8                     = 0x9278,
            // ATITC
            ATC_RGB_AMD                     = 0x8C92,
            ATC_RGBA_EXPLICIT_ALPHA_AMD     = 0x8C93,
            ATC_RGBA_INTERPOLATED_ALPHA_AMD = 0x87EE,
        };
    };

    // enum class BaseInternalFormat
    //{
    //    ETC2_R11    = 0x1903,
    //    ETC2_RG11   = 0x8227,
    //    ETC1_RGB8   = 0x1907,
    //    ETC1_ALPHA8 = ETC1_RGB8,
    //    ETC2_RGB8   = 0x1907,
    //    ETC2_RGB8A1 = 0x1908,
    //    ETC2_RGBA8  = 0x1908,
    //};

    uint8_t identifier[12];
    uint32_t endianness;
    uint32_t glType;
    uint32_t glTypeSize;
    uint32_t glFormat;
    uint32_t glInternalFormat;
    uint32_t glBaseInternalFormat;
    uint32_t pixelWidth;
    uint32_t pixelHeight;
    uint32_t pixelDepth;
    uint32_t numberOfArrayElements;
    uint32_t numberOfFaces;
    uint32_t numberOfMipmapLevels;
    uint32_t bytesOfKeyValueData;
};
