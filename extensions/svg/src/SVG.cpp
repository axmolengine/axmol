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
#include "SVG.h"

#include "axmol/2d/Sprite.h"
#include "axmol/base/Director.h"
#include "axmol/base/Logging.h"
#include "axmol/platform/FileUtils.h"
#include "axmol/platform/Image.h"
#include "axmol/renderer/TextureCache.h"

#include <lunasvg.h>

#include <string.h>
#include <string>

namespace
{
bool isInvalidSize(int width, int height)
{
    return width == 0 || height == 0 || width < -1 || height < -1;
}

std::string makeTextureKey(std::string_view path, int width, int height)
{
    std::string key(path);
    key += "#svg:";
    key += std::to_string(width);
    key += 'x';
    key += std::to_string(height);
    return key;
}
}

NS_AX_EXT_BEGIN

Texture2D* loadSVGTexture(std::string_view path, int width, int height)
{
    if (path.empty() || isInvalidSize(width, height))
    {
        AXLOGW("Invalid SVG path or render size: {} ({}x{})", path, width, height);
        return nullptr;
    }

    auto* fileUtils = FileUtils::getInstance();
    auto* director  = Director::getInstance();
    if (!fileUtils || !director || !director->getTextureCache())
        return nullptr;

    const auto fullPath = fileUtils->fullPathForFilename(path);
    if (fullPath.empty())
    {
        AXLOGW("SVG file not found: {}", path);
        return nullptr;
    }

    const auto cacheKey = makeTextureKey(fullPath, width, height);
    auto* textureCache  = director->getTextureCache();
    if (auto* texture = textureCache->getTextureForKey(cacheKey))
        return texture;

    auto data = fileUtils->getDataFromFile(fullPath);
    if (data.isNull())
    {
        AXLOGW("Failed to read SVG file: {}", fullPath);
        return nullptr;
    }

    auto document = lunasvg::Document::loadFromData(reinterpret_cast<const char*>(data.getBytes()),
                                                    static_cast<size_t>(data.getSize()));
    if (!document)
    {
        AXLOGW("Failed to parse SVG file: {}", fullPath);
        return nullptr;
    }

    auto bitmap = document->renderToBitmap(width, height);
    if (bitmap.isNull())
    {
        AXLOGW("Failed to render SVG file: {}", fullPath);
        return nullptr;
    }

    bitmap.convertToRGBA();

    const auto bitmapWidth  = bitmap.width();
    const auto bitmapHeight = bitmap.height();
    const auto bitmapStride = bitmap.stride();
    Data pixels;
    auto* pixelData = pixels.resize(static_cast<ssize_t>(bitmapWidth) * bitmapHeight * 4);
    if (!pixelData)
    {
        AXLOGW("Failed to allocate SVG bitmap: {}", fullPath);
        return nullptr;
    }

    const auto* source = bitmap.data();
    for (int y = 0; y < bitmapHeight; ++y)
    {
        memcpy(pixelData + static_cast<size_t>(y) * bitmapWidth * 4,
               source + static_cast<size_t>(y) * bitmapStride, static_cast<size_t>(bitmapWidth) * 4);
    }

    auto* image = new Image();
    if (!image->initWithRawData(pixelData, static_cast<ssize_t>(pixels.size()), bitmapWidth, bitmapHeight, 8, false))
    {
        image->release();
        AXLOGW("Failed to create image from SVG: {}", fullPath);
        return nullptr;
    }

    auto* texture = textureCache->addImage(image, cacheKey);
    image->release();
    return texture;
}

Sprite* createSVGSprite(std::string_view path, int width, int height)
{
    auto* texture = loadSVGTexture(path, width, height);
    return texture ? Sprite::createWithTexture(texture) : nullptr;
}

NS_AX_EXT_END
