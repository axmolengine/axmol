/****************************************************************************
Copyright (c) 2009-2010 Ricardo Quesada
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
#include "axmol/2d/FastTMXTiledMap.h"
#include "axmol/2d/FastTMXLayer.h"
#include "axmol/base/text_utils.h"

namespace ax
{

// implementation FastTMXTiledMap

FastTMXTiledMap* FastTMXTiledMap::create(std::string_view tmxFile, bool allowInvisibleLayers)
{
    FastTMXTiledMap* ret = new FastTMXTiledMap();
    if (ret->initWithTMXFile(tmxFile, allowInvisibleLayers))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

FastTMXTiledMap* FastTMXTiledMap::createWithXML(std::string_view tmxString,
                                                std::string_view resourcePath,
                                                bool allowInvisibleLayers)
{
    FastTMXTiledMap* ret = new FastTMXTiledMap();
    if (ret->initWithXML(tmxString, resourcePath, allowInvisibleLayers))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool FastTMXTiledMap::initWithTMXFile(std::string_view tmxFile, bool allowInvisibleLayers)
{
    AXASSERT(!tmxFile.empty(), "FastTMXTiledMap: tmx file should not be empty");

    setContentSize(Vec2::ZERO);

    TMXMapInfo* mapInfo = TMXMapInfo::create(tmxFile);

    if (!mapInfo)
    {
        return false;
    }
    AXASSERT(!mapInfo->getTilesets().empty(), "FastTMXTiledMap: Map not found. Please check the filename.");
    buildWithMapInfo(mapInfo, allowInvisibleLayers);

    _tmxFile = tmxFile;

    return true;
}

bool FastTMXTiledMap::initWithXML(std::string_view tmxString, std::string_view resourcePath, bool allowInvisibleLayers)
{
    setContentSize(Vec2::ZERO);

    TMXMapInfo* mapInfo = TMXMapInfo::createWithXML(tmxString, resourcePath);
    if (!mapInfo)
        return false;

    AXASSERT(!mapInfo->getTilesets().empty(), "FastTMXTiledMap: Map not found. Please check the filename.");
    buildWithMapInfo(mapInfo, allowInvisibleLayers);

    return true;
}

FastTMXTiledMap::FastTMXTiledMap() : _mapSize(Vec2::ZERO), _tileSize(Vec2::ZERO), _mapOrientation(0), _mapInfo(nullptr)
{}

FastTMXTiledMap::~FastTMXTiledMap()
{
    AX_SAFE_RELEASE(_mapInfo);
}

// private
FastTMXLayer* FastTMXTiledMap::parseLayer(TMXLayerInfo* layerInfo, TMXMapInfo* mapInfo)
{
    auto tilesets = tilesetsForLayer(layerInfo, mapInfo);
    if (tilesets.empty())
        return nullptr;

    FastTMXLayer* layer = FastTMXLayer::create(tilesets, layerInfo, mapInfo);
    if (!layer)
        return nullptr;

    // tell the layerinfo to release the ownership of the tiles map.
    layerInfo->_ownTiles = false;
    layer->setupTiles();

    return layer;
}

std::vector<TMXTilesetInfo*> FastTMXTiledMap::tilesetsForLayer(TMXLayerInfo* layerInfo, TMXMapInfo* mapInfo)
{
    Vec2 size         = layerInfo->_layerSize;
    auto& tilesets    = mapInfo->getTilesets();
    const int tsCount = static_cast<int>(tilesets.size());

    // Collect every tileset whose GID range [firstGid, nextFirstGid) is referenced by this layer.
    // Iterate descending so the result is already in the order batchIndexForGID expects.
    std::vector<TMXTilesetInfo*> used;
    for (int i = tsCount - 1; i >= 0; --i)
    {
        TMXTilesetInfo* ts = tilesets[i];
        if (!ts)
            continue;
        const uint32_t lo = static_cast<uint32_t>(ts->_firstGid);
        // Upper bound: next tileset's firstGid, or the maximum GID value if this is the last.
        const uint32_t hi =
            (i + 1 < tsCount && tilesets[i + 1]) ? static_cast<uint32_t>(tilesets[i + 1]->_firstGid) : UINT32_MAX;
        for (int y = 0; y < static_cast<int>(size.height); ++y)
        {
            for (int x = 0; x < static_cast<int>(size.width); ++x)
            {
                uint32_t gid = layerInfo->_tiles[x + static_cast<int>(size.width) * y] & kTMXFlippedMask;
                if (gid >= lo && gid < hi)
                {
                    used.push_back(ts);
                    goto next_tileset;
                }
            }
        }
    next_tileset:;
    }

    if (used.empty())
        AXLOGW("axmol: Warning: TMX Layer '{}' has no tiles", layerInfo->_name);

    // Result is in descending firstGid order (matches batchIndexForGID scan direction).
    return used;
}

TMXTilesetInfo* FastTMXTiledMap::tilesetForLayer(TMXLayerInfo* layerInfo, TMXMapInfo* mapInfo)
{
    auto v = tilesetsForLayer(layerInfo, mapInfo);
    return v.empty() ? nullptr : v.front();
}

void FastTMXTiledMap::buildWithMapInfo(TMXMapInfo* mapInfo, bool allowInvisibleLayers)
{
    _mapSize        = mapInfo->getMapSize();
    _tileSize       = mapInfo->getTileSize();
    _mapOrientation = mapInfo->getOrientation();

    _objectGroups = mapInfo->getObjectGroups();

    _properties = mapInfo->getProperties();

    _tileProperties = mapInfo->getTileProperties();

    int idx = 0;

    auto& layers = mapInfo->getLayers();
    for (const auto& layerInfo : layers)
    {
        if (!layerInfo->_visible && !allowInvisibleLayers)
        {
            continue;
        }

        FastTMXLayer* child = parseLayer(layerInfo, mapInfo);
        if (child == nullptr)
        {
            idx++;
            continue;
        }
        addChild(child, idx, idx);

        child->setVisible(layerInfo->_visible);

        // update content size with the max size
        const Vec2& childSize = child->getContentSize();
        Vec2 currentSize      = this->getContentSize();
        currentSize.width     = std::max(currentSize.width, childSize.width);
        currentSize.height    = std::max(currentSize.height, childSize.height);
        this->setContentSize(currentSize);

        idx++;
    }

    _layerCount = idx;

    _mapInfo = mapInfo;
    AX_SAFE_RETAIN(_mapInfo);
}

// public
FastTMXLayer* FastTMXTiledMap::getLayer(std::string_view layerName) const
{
    AXASSERT(!layerName.empty(), "Invalid layer name!");

    for (auto&& child : _children)
    {
        FastTMXLayer* layer = dynamic_cast<FastTMXLayer*>(child);
        if (layer)
        {
            if (layerName == layer->getLayerName())
            {
                return layer;
            }
        }
    }

    // layer not found
    return nullptr;
}

TMXObjectGroup* FastTMXTiledMap::getObjectGroup(std::string_view groupName) const
{
    AXASSERT(!groupName.empty(), "Invalid group name!");

    if (!_objectGroups.empty())
    {
        for (const auto& objectGroup : _objectGroups)
        {
            if (objectGroup && objectGroup->getGroupName() == groupName)
            {
                return objectGroup;
            }
        }
    }

    // objectGroup not found
    return nullptr;
}

Value FastTMXTiledMap::getProperty(std::string_view propertyName) const
{
    auto propsItr = _properties.find(propertyName);
    if (propsItr != _properties.end())
        return propsItr->second;

    return Value();
}

Value FastTMXTiledMap::getPropertiesForGID(int GID) const
{
    auto propsItr = _tileProperties.find(GID);
    if (propsItr != _tileProperties.end())
        return propsItr->second;

    return Value();
}

std::string FastTMXTiledMap::getDescription() const
{
    return fmt::format("<FastTMXTiledMap | Tag = {}, Layers = {}", _tag, static_cast<int>(_children.size()));
}

void FastTMXTiledMap::setTileAnimEnabled(bool enabled)
{
    for (auto&& child : _children)
    {
        FastTMXLayer* layer = dynamic_cast<FastTMXLayer*>(child);
        if (layer)
        {
            if (layer->hasTileAnimation())
            {
                if (enabled)
                    layer->getTileAnimManager()->startAll();
                else
                    layer->getTileAnimManager()->stopAll();
            }
        }
    }
}

TMXTilesetInfo* FastTMXTiledMap::getTilesetInfo(std::string_view tsxNameString)
{
    if (_mapInfo == nullptr)
        return nullptr;

    auto tileSets = _mapInfo->getTilesets();
    for (auto tileSet : tileSets)
    {
        if (tileSet->_name == tsxNameString)
        {
            return tileSet;
        }
    }
    return nullptr;
}

Vector<FastTMXLayer*> FastTMXTiledMap::getLayers() const
{
    Vector<FastTMXLayer*> layers;
    for (auto child : _children)
    {
        auto layer = dynamic_cast<FastTMXLayer*>(child);
        if (layer)
        {
            layers.pushBack(layer);
        }
    }
    return layers;
}

}  // namespace ax
