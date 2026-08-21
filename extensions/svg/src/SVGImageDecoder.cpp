#include "axmol/platform/ImageLoader.h"
#include "SVGImageDecoder.h"

#include <lunasvg.h>

#include <algorithm>
#include <stdint.h>
#include <string.h>

namespace
{
bool isSVG(const uint8_t* data, ssize_t dataLen)
{
    if (!data || dataLen <= 0)
        return false;

    const auto* begin = reinterpret_cast<const char*>(data);
    const auto* end   = begin + std::min<ssize_t>(dataLen, 4096);
    for (auto it = begin; it + 4 <= end; ++it)
    {
        if (*it == '<' && memcmp(it + 1, "svg", 3) == 0)
            return true;
    }
    return false;
}

bool decodeSVG(const uint8_t* data, ssize_t dataLen, ax::ImageLoader::DecodedImage& result)
{
    if (!isSVG(data, dataLen))
        return false;

    auto document = lunasvg::Document::loadFromData(reinterpret_cast<const char*>(data), static_cast<size_t>(dataLen));
    if (!document)
        return false;

    auto bitmap = document->renderToBitmap();
    if (bitmap.isNull())
    {
        return false;
    }

    const auto width  = bitmap.width();
    const auto height = bitmap.height();
    const auto stride = bitmap.stride();
    auto* pixels      = result.data.resize(static_cast<ssize_t>(width) * height * 4);
    if (!pixels)
        return false;

    // LunaSVG stores ARGB32 premultiplied pixels. Use its conversion routine
    // so the result is plain RGBA and can be uploaded consistently by all
    // Axmol render backends.
    bitmap.convertToRGBA();
    const auto* source = bitmap.data();
    for (int y = 0; y < height; ++y)
    {
        memcpy(pixels + static_cast<size_t>(y) * width * 4, source + static_cast<size_t>(y) * stride,
               static_cast<size_t>(width) * 4);
    }

    result.width              = width;
    result.height             = height;
    result.pixelFormat        = ax::rhi::PixelFormat::RGBA8;
    result.premultipliedAlpha = false;
    return true;
}

}  // namespace

void ax::svg::registerImageDecoder()
{
    ax::ImageLoader::registerDecoder("svg", decodeSVG);
}
