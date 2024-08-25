/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
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

#include "PlatformMacros.h"
#include "renderer/Texture2D.h"

namespace ax
{

namespace backend
{
namespace PixelFormatUtils
{
typedef ax::backend::PixelFormat PixelFormat;

struct PixelFormatDescriptor
{
    uint8_t bpp;  // bitsPerPixel: !!!regard bpp=0 as invalid pixel format
    uint8_t blockWidth;
    uint8_t blockHeight;
    uint8_t blockSize;
    uint8_t minBlockX;
    uint8_t minBlockY;
    bool alpha;  // with alpha channel?
    const char* name;
};

const PixelFormatDescriptor& getFormatDescriptor(PixelFormat format);
uint32_t computeRowPitch(PixelFormat format, uint32_t width);
inline uint8_t getBitsPerPixel(PixelFormat format)
{
    return getFormatDescriptor(format).bpp;
}
inline bool isCompressed(PixelFormat format)
{
    return format < PixelFormat::RGBA8;
}

/**convert functions*/

/**
Convert the format to the format param you specified, if the format is PixelFormat::NONE, it will detect it
automatically and convert to the closest format for you. It will return the converted format to you. if the outData !=
data, you must delete it manually.
*/
PixelFormat convertDataToFormat(const unsigned char* data,
                                size_t dataLen,
                                PixelFormat originFormat,
                                PixelFormat format,
                                unsigned char** outData,
                                size_t* outDataLen);
};  // namespace PixelFormatUtils
}  // namespace backend
}
