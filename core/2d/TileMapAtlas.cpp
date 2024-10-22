/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
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
#include "2d/TileMapAtlas.h"
#include "platform/FileUtils.h"
#include "renderer/TextureAtlas.h"
#include "base/TGAlib.h"
#include "base/Director.h"
#include "base/UTF8.h"

namespace ax
{

// implementation TileMapAtlas

TileMapAtlas* TileMapAtlas::create(std::string_view tile, std::string_view mapFile, int tileWidth, int tileHeight)
{
    TileMapAtlas* ret = new TileMapAtlas();
    if (ret->initWithTileFile(tile, mapFile, tileWidth, tileHeight))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool TileMapAtlas::initWithTileFile(std::string_view tile, std::string_view mapFile, int tileWidth, int tileHeight)
{
    this->loadTGAfile(mapFile);
    this->calculateItemsToRender();

    if (AtlasNode::initWithTileFile(tile, tileWidth, tileHeight, _itemsToRender))
    {
        this->updateAtlasValues();
        this->setContentSize(Vec2((float)(_TGAInfo->width * _itemWidth), (float)(_TGAInfo->height * _itemHeight)));
        return true;
    }
    return false;
}

TileMapAtlas::TileMapAtlas() : _itemsToRender(0), _TGAInfo(nullptr) {}

TileMapAtlas::~TileMapAtlas()
{
    if (_TGAInfo)
    {
        tgaDestroy(_TGAInfo);
    }
}

void TileMapAtlas::releaseMap()
{
    if (_TGAInfo)
    {
        tgaDestroy(_TGAInfo);
    }
    _TGAInfo = nullptr;
}

void TileMapAtlas::calculateItemsToRender()
{
    AXASSERT(_TGAInfo != nullptr, "tgaInfo must be non-nil");

    _itemsToRender = 0;
    for (int x = 0; x < _TGAInfo->width; x++)
    {
        for (int y = 0; y < _TGAInfo->height; y++)
        {
            Color3B* ptr  = (Color3B*)_TGAInfo->imageData;
            Color3B value = ptr[x + y * _TGAInfo->width];
            if (value.r)
            {
                ++_itemsToRender;
            }
        }
    }
}

void TileMapAtlas::loadTGAfile(std::string_view file)
{
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(file);

    //    //Find the path of the file
    //    NSBundle *mainBndl = [Director sharedDirector].loadingBundle;
    //    String *resourcePath = [mainBndl resourcePath];
    //    String * path = [resourcePath stringByAppendingPathComponent:file];

    _TGAInfo = tgaLoad(fullPath.c_str());
#if 1
    if (_TGAInfo->status != TGA_OK)
    {
        AXASSERT(0, "TileMapAtlasLoadTGA : TileMapAtlas cannot load TGA file");
    }
#endif
}

// TileMapAtlas - Atlas generation / updates
void TileMapAtlas::setTile(const Color3B& tile, const Vec2& position)
{
    AXASSERT(_TGAInfo != nullptr, "tgaInfo must not be nil");
    AXASSERT(position.x < _TGAInfo->width, "Invalid position.x");
    AXASSERT(position.y < _TGAInfo->height, "Invalid position.x");
    AXASSERT(tile.r != 0, "R component must be non 0");

    Color3B* ptr  = (Color3B*)_TGAInfo->imageData;
    Color3B value = ptr[(unsigned int)(position.x + position.y * _TGAInfo->width)];
    if (value.r == 0)
    {
        AXLOGD("Value.r must be non 0.");
    }
    else
    {
        ptr[(unsigned int)(position.x + position.y * _TGAInfo->width)] = tile;

        // FIXME:: this method consumes a lot of memory
        // FIXME:: a tree of something like that shall be implemented
        std::string key = StringUtils::toString(position.x) + "," + StringUtils::toString(position.y);
        int num         = _posToAtlasIndex[key].asInt();

        this->updateAtlasValueAt(position, tile, num);
    }
}

Color3B TileMapAtlas::getTileAt(const Vec2& position) const
{
    AXASSERT(_TGAInfo != nullptr, "tgaInfo must not be nil");
    AXASSERT(position.x < _TGAInfo->width, "Invalid position.x");
    AXASSERT(position.y < _TGAInfo->height, "Invalid position.y");

    Color3B* ptr  = (Color3B*)_TGAInfo->imageData;
    Color3B value = ptr[(unsigned int)(position.x + position.y * _TGAInfo->width)];

    return value;
}

void TileMapAtlas::updateAtlasValueAt(const Vec2& pos, const Color3B& value, int index)
{
    AXASSERT(index >= 0 && index < _textureAtlas->getCapacity(), "updateAtlasValueAt: Invalid index");

    V3F_C4F_T2F_Quad* quad = &((_textureAtlas->getQuads())[index]);

    int x     = pos.x;
    int y     = pos.y;
    float row = (float)(value.r % _itemsPerRow);
    float col = (float)(value.r / _itemsPerRow);

    float textureWide = (float)(_textureAtlas->getTexture()->getPixelsWide());
    float textureHigh = (float)(_textureAtlas->getTexture()->getPixelsHigh());

    float itemWidthInPixels  = _itemWidth * AX_CONTENT_SCALE_FACTOR();
    float itemHeightInPixels = _itemHeight * AX_CONTENT_SCALE_FACTOR();

#if AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL
    float left   = (2 * row * itemWidthInPixels + 1) / (2 * textureWide);
    float right  = left + (itemWidthInPixels * 2 - 2) / (2 * textureWide);
    float top    = (2 * col * itemHeightInPixels + 1) / (2 * textureHigh);
    float bottom = top + (itemHeightInPixels * 2 - 2) / (2 * textureHigh);
#else
    float left   = (row * itemWidthInPixels) / textureWide;
    float right  = left + itemWidthInPixels / textureWide;
    float top    = (col * itemHeightInPixels) / textureHigh;
    float bottom = top + itemHeightInPixels / textureHigh;
#endif

    quad->tl.texCoord.u = left;
    quad->tl.texCoord.v = top;
    quad->tr.texCoord.u = right;
    quad->tr.texCoord.v = top;
    quad->bl.texCoord.u = left;
    quad->bl.texCoord.v = bottom;
    quad->br.texCoord.u = right;
    quad->br.texCoord.v = bottom;

    quad->bl.position.x = (float)(x * _itemWidth);
    quad->bl.position.y = (float)(y * _itemHeight);
    quad->bl.position.z = 0.0f;
    quad->br.position.x = (float)(x * _itemWidth + _itemWidth);
    quad->br.position.y = (float)(y * _itemHeight);
    quad->br.position.z = 0.0f;
    quad->tl.position.x = (float)(x * _itemWidth);
    quad->tl.position.y = (float)(y * _itemHeight + _itemHeight);
    quad->tl.position.z = 0.0f;
    quad->tr.position.x = (float)(x * _itemWidth + _itemWidth);
    quad->tr.position.y = (float)(y * _itemHeight + _itemHeight);
    quad->tr.position.z = 0.0f;

    Color color(_displayedColor, _displayedOpacity / 255.0f);
    quad->tr.color = color;
    quad->tl.color = color;
    quad->br.color = color;
    quad->bl.color = color;

    _textureAtlas->setDirty(true);
    ssize_t totalQuads = _textureAtlas->getTotalQuads();
    if (index + 1 > totalQuads)
    {
        _textureAtlas->increaseTotalQuadsWith(index + 1 - totalQuads);
    }
}

void TileMapAtlas::updateAtlasValues()
{
    AXASSERT(_TGAInfo != nullptr, "tgaInfo must be non-nil");

    int total = 0;

    for (int x = 0; x < _TGAInfo->width; x++)
    {
        for (int y = 0; y < _TGAInfo->height; y++)
        {
            if (total < _itemsToRender)
            {
                Color3B* ptr  = (Color3B*)_TGAInfo->imageData;
                Color3B value = ptr[x + y * _TGAInfo->width];

                if (value.r != 0)
                {
                    this->updateAtlasValueAt(Vec2(x, y), value, total);

                    std::string key       = fmt::format("{},{}", x, y);
                    _posToAtlasIndex[key] = total;

                    total++;
                }
            }
        }
    }
}

}
