#pragma once

#include "axmol/base/Data.h"
#include "axmol/rhi/RHITypes.h"

#include <functional>
#include <string_view>

namespace ax
{

class AX_DLL ImageLoader
{
public:
    struct DecodedImage
    {
        Data data;
        int width = 0;
        int height = 0;
        rhi::PixelFormat pixelFormat = rhi::PixelFormat::RGBA8;
        bool premultipliedAlpha = false;
    };

    using Decoder = std::function<bool(const uint8_t*, ssize_t, DecodedImage&)>;

    /** Registers a decoder for a file extension. */
    static void registerDecoder(std::string_view extension, Decoder decoder);

    /** Uses O(1) extension lookup to decode the data. */
    static bool decode(std::string_view extension, const uint8_t* data, ssize_t dataLen, DecodedImage& result);
};

}  // namespace ax
