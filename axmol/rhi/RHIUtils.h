/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/platform/PlatformMacros.h"
#include "axmol/rhi/RHITypes.h"
#include <cmath>

/**
 * @addtogroup _rhi
 * @{
 */

namespace ax::rhi
{
namespace RHIUtils
{
struct PixelFormatDesc
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

const PixelFormatDesc& getFormatDesc(PixelFormat format);
uint32_t computeRowPitch(PixelFormat format, uint32_t width);
uint32_t computeDataSize(PixelFormat format, uint32_t width, uint32_t height);
inline uint8_t getBitsPerPixel(PixelFormat format)
{
    return getFormatDesc(format).bpp;
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

inline uint8_t computeMipLevels(int width, int height)
{
    return static_cast<uint8_t>(std::floor(std::log2((std::max)(width, height))) + 1);
}

std::string_view vendorToString(uint32_t vendorId);

}  // namespace RHIUtils
}  // namespace ax::rhi

// end of _rhi group
/// @}
