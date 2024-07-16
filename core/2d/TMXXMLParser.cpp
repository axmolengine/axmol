/****************************************************************************
Copyright (c) 2011      Максим Аксенов
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

#include "2d/TMXXMLParser.h"
#include <unordered_map>
#include <sstream>
#include <regex>
//  #include "2d/TMXTiledMap.h"
#include "base/ZipUtils.h"
#include "base/Director.h"
#include "base/Utils.h"
#include "platform/FileUtils.h"

// using namespace std;

NS_AX_BEGIN

// implementation TMXLayerInfo
TMXLayerInfo::TMXLayerInfo() : _name(""), _tiles(nullptr), _ownTiles(true) {}

TMXLayerInfo::~TMXLayerInfo()
{
    AXLOGV("deallocing TMXLayerInfo: {}", fmt::ptr(this));
    if (_ownTiles && _tiles)
    {
        free(_tiles);
        _tiles = nullptr;
    }
}

ValueMap& TMXLayerInfo::getProperties()
{
    return _properties;
}

void TMXLayerInfo::setProperties(ValueMap var)
{
    _properties = var;
}

// implementation TMXTilesetInfo
TMXTilesetInfo::TMXTilesetInfo() : _firstGid(0), _tileSize(Vec2::ZERO), _spacing(0), _margin(0), _imageSize(Vec2::ZERO)
{}

TMXTilesetInfo::~TMXTilesetInfo()
{
    AXLOGV("deallocing TMXTilesetInfo: {}", fmt::ptr(this));
}

Rect TMXTilesetInfo::getRectForGID(uint32_t gid)
{
    Rect rect;
    rect.size = _tileSize;
    gid &= kTMXFlippedMask;
    gid = gid - _firstGid;
    // max_x means the column count in tile map
    // in the origin:
    // max_x = (int)((_imageSize.width - _margin*2 + _spacing) / (_tileSize.width + _spacing));
    // but in editor "Tiled", _margin variable only effect the left side
    // for compatible with "Tiled", change the max_x calculation
    int max_x = (int)((_imageSize.width - _margin + _spacing) / (_tileSize.width + _spacing));

    rect.origin.x = (gid % max_x) * (_tileSize.width + _spacing) + _margin;
    rect.origin.y = (gid / max_x) * (_tileSize.height + _spacing) + _margin;
    return rect;
}

// implementation TMXMapInfo

TMXMapInfo* TMXMapInfo::create(std::string_view tmxFile)
{
    TMXMapInfo* ret = new TMXMapInfo();
    if (ret->initWithTMXFile(tmxFile))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

TMXMapInfo* TMXMapInfo::createWithXML(std::string_view tmxString, std::string_view resourcePath)
{
    TMXMapInfo* ret = new TMXMapInfo();
    if (ret->initWithXML(tmxString, resourcePath))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

void TMXMapInfo::internalInit(std::string_view tmxFileName, std::string_view resourcePath)
{
    if (!tmxFileName.empty())
    {
        _TMXFileName = FileUtils::getInstance()->fullPathForFilename(tmxFileName);
    }

    if (!resourcePath.empty())
    {
        _resources = resourcePath;
    }

    _objectGroups.reserve(4);

    // tmp vars
    _currentString     = "";
    _storingCharacters = false;
    _layerAttribs      = TMXLayerAttribNone;
    _parentElement     = TMXPropertyNone;
    _currentFirstGID   = -1;
}

bool TMXMapInfo::initWithXML(std::string_view tmxString, std::string_view resourcePath)
{
    internalInit("", resourcePath);
    return parseXMLString(tmxString);
}

bool TMXMapInfo::initWithTMXFile(std::string_view tmxFile)
{
    internalInit(tmxFile, "");
    return parseXMLFile(_TMXFileName);
}

TMXMapInfo::TMXMapInfo()
    : _orientation(TMXOrientationOrtho)
    , _staggerAxis(TMXStaggerAxis_Y)
    , _staggerIndex(TMXStaggerIndex_Even)
    , _hexSideLength(0)
    , _mapSize(Vec2::ZERO)
    , _tileSize(Vec2::ZERO)
    , _parentElement(0)
    , _parentGID(0)
    , _layerAttribs(0)
    , _storingCharacters(false)
    , _xmlTileIndex(0)
    , _currentFirstGID(-1)
    , _recordFirstGID(true)
{}

TMXMapInfo::~TMXMapInfo()
{
    AXLOGV("deallocing TMXMapInfo: {}", fmt::ptr(this));
}

bool TMXMapInfo::parseXMLString(std::string_view xmlString)
{
    size_t len = xmlString.size();
    if (len <= 0)
        return false;

    SAXParser parser;

    if (false == parser.init("UTF-8"))
    {
        return false;
    }

    parser.setDelegator(this);

    return parser.parse(xmlString.data(), len, SAXParser::ParseOption::TRIM_WHITESPACE);
}

bool TMXMapInfo::parseXMLFile(std::string_view xmlFilename)
{
    SAXParser parser;

    if (false == parser.init("UTF-8"))
    {
        return false;
    }

    parser.setDelegator(this);

    return parser.parse(xmlFilename, SAXParser::ParseOption::TRIM_WHITESPACE);
}

// the XML parser calls here with all the elements
void TMXMapInfo::startElement(void* /*ctx*/, const char* name, const char** atts)
{
    TMXMapInfo* tmxMapInfo  = this;
    std::string elementName = name;
    ValueMap attributeDict;
    if (atts && atts[0])
    {
        for (int i = 0; atts[i]; i += 2)
        {
            std::string key   = atts[i];
            std::string value = atts[i + 1];
            attributeDict.emplace(key, Value(value));
        }
    }
    if (elementName == "map")
    {
        std::string version = attributeDict["version"].asString();
        AXLOGD("TMXFormat: TMX version: {}", version);

        std::string orientationStr = attributeDict["orientation"].asString();
        if (orientationStr == "orthogonal")
        {
            tmxMapInfo->setOrientation(TMXOrientationOrtho);
        }
        else if (orientationStr == "isometric")
        {
            tmxMapInfo->setOrientation(TMXOrientationIso);
        }
        else if (orientationStr == "hexagonal")
        {
            tmxMapInfo->setOrientation(TMXOrientationHex);
        }
        else if (orientationStr == "staggered")
        {
            tmxMapInfo->setOrientation(TMXOrientationStaggered);
        }
        else
        {
            AXLOGD("TMXFomat: Unsupported orientation: {}", tmxMapInfo->getOrientation());
        }

        std::string staggerAxisStr = attributeDict["staggeraxis"].asString();
        if (staggerAxisStr == "x")
        {
            tmxMapInfo->setStaggerAxis(TMXStaggerAxis_X);
        }
        else if (staggerAxisStr == "y")
        {
            tmxMapInfo->setStaggerAxis(TMXStaggerAxis_Y);
        }

        std::string staggerIndex = attributeDict["staggerindex"].asString();
        if (staggerIndex == "odd")
        {
            tmxMapInfo->setStaggerIndex(TMXStaggerIndex_Odd);
        }
        else if (staggerIndex == "even")
        {
            tmxMapInfo->setStaggerIndex(TMXStaggerIndex_Even);
        }

        auto hexSideLength = attributeDict["hexsidelength"].asInt();
        tmxMapInfo->setHexSideLength(hexSideLength);

        Vec2 s;
        s.width  = attributeDict["width"].asFloat();
        s.height = attributeDict["height"].asFloat();
        tmxMapInfo->setMapSize(s);

        s.width  = attributeDict["tilewidth"].asFloat();
        s.height = attributeDict["tileheight"].asFloat();
        tmxMapInfo->setTileSize(s);

        // The parent element is now "map"
        tmxMapInfo->setParentElement(TMXPropertyMap);
    }
    else if (elementName == "tileset")
    {
        // If this is an external tileset then start parsing that
        std::string externalTilesetFilename = attributeDict["source"].asString();
        if (externalTilesetFilename != "")
        {
            _externalTilesetFilename = externalTilesetFilename;

            // Tileset file will be relative to the map file. So we need to convert it to an absolute path
            if (_TMXFileName.find_last_of('/') != std::string::npos)
            {
                std::string dir         = _TMXFileName.substr(0, _TMXFileName.find_last_of('/') + 1);
                externalTilesetFilename = dir + externalTilesetFilename;
            }
            else
            {
                externalTilesetFilename = _resources + "/" + externalTilesetFilename;
            }
            externalTilesetFilename = FileUtils::getInstance()->fullPathForFilename(externalTilesetFilename);

            _currentFirstGID = attributeDict["firstgid"].asInt();
            if (_currentFirstGID < 0)
            {
                _currentFirstGID = 0;
            }
            _recordFirstGID = false;

            tmxMapInfo->parseXMLFile(externalTilesetFilename);
        }
        else
        {
            TMXTilesetInfo* tileset = new TMXTilesetInfo();
            tileset->_name          = attributeDict["name"].asString();

            if (_recordFirstGID)
            {
                // unset before, so this is tmx file.
                tileset->_firstGid = attributeDict["firstgid"].asInt();

                if (tileset->_firstGid < 0)
                {
                    tileset->_firstGid = 0;
                }
            }
            else
            {
                tileset->_firstGid = _currentFirstGID;
                _currentFirstGID   = 0;
            }

            tileset->_spacing = attributeDict["spacing"].asInt();
            tileset->_margin  = attributeDict["margin"].asInt();
            Vec2 s;
            s.width            = attributeDict["tilewidth"].asFloat();
            s.height           = attributeDict["tileheight"].asFloat();
            tileset->_tileSize = s;

            tmxMapInfo->getTilesets().pushBack(tileset);
            tileset->release();
        }
    }
    else if (elementName == "tile")
    {
        if (tmxMapInfo->getParentElement() == TMXPropertyLayer)
        {
            TMXLayerInfo* layer = tmxMapInfo->getLayers().back();
            Vec2 layerSize      = layer->_layerSize;
            uint32_t gid        = static_cast<uint32_t>(attributeDict["gid"].asUnsignedInt());
            int tilesAmount     = static_cast<int>(layerSize.width * layerSize.height);

            if (_xmlTileIndex < tilesAmount)
            {
                layer->_tiles[_xmlTileIndex++] = gid;
            }
        }
        else
        {
            TMXTilesetInfo* info = tmxMapInfo->getTilesets().back();
            tmxMapInfo->setParentGID(info->_firstGid + attributeDict["id"].asInt());
            tmxMapInfo->getTileProperties()[tmxMapInfo->getParentGID()] = Value(ValueMap());
            tmxMapInfo->setParentElement(TMXPropertyTile);
        }
    }
    else if (elementName == "layer")
    {
        TMXLayerInfo* layer = new TMXLayerInfo();
        layer->_name        = attributeDict["name"].asString();

        Vec2 s;
        s.width           = attributeDict["width"].asFloat();
        s.height          = attributeDict["height"].asFloat();
        layer->_layerSize = s;

        Value& visibleValue = attributeDict["visible"];
        layer->_visible     = visibleValue.isNull() ? true : visibleValue.asBool();

        Value& opacityValue = attributeDict["opacity"];
        layer->_opacity     = opacityValue.isNull() ? 255 : (unsigned char)(255.0f * opacityValue.asFloat());

        float x = attributeDict["x"].asFloat();
        float y = attributeDict["y"].asFloat();
        layer->_offset.set(x, y);

        tmxMapInfo->getLayers().pushBack(layer);
        layer->release();

        // The parent element is now "layer"
        tmxMapInfo->setParentElement(TMXPropertyLayer);
    }
    else if (elementName == "objectgroup")
    {
        TMXObjectGroup* objectGroup = new TMXObjectGroup();
        objectGroup->setGroupName(attributeDict["name"].asString());
        Vec2 positionOffset;
        positionOffset.x = attributeDict["x"].asFloat() * tmxMapInfo->getTileSize().width;
        positionOffset.y = attributeDict["y"].asFloat() * tmxMapInfo->getTileSize().height;
        objectGroup->setPositionOffset(positionOffset);

        tmxMapInfo->getObjectGroups().pushBack(objectGroup);
        objectGroup->release();

        // The parent element is now "objectgroup"
        tmxMapInfo->setParentElement(TMXPropertyObjectGroup);
    }
    else if (elementName == "tileoffset")
    {
        TMXTilesetInfo* tileset = tmxMapInfo->getTilesets().back();

        float tileOffsetX = attributeDict["x"].asFloat();

        float tileOffsetY = attributeDict["y"].asFloat();

        tileset->_tileOffset = Vec2(tileOffsetX, tileOffsetY);
    }
    else if (elementName == "image")
    {
        TMXTilesetInfo* tileset = tmxMapInfo->getTilesets().back();

        // build full path
        std::string imagename       = attributeDict["source"].asString();
        tileset->_originSourceImage = imagename;

        if (_TMXFileName.find_last_of('/') != std::string::npos)
        {
            std::string dir       = _TMXFileName.substr(0, _TMXFileName.find_last_of('/') + 1);
            tileset->_sourceImage = dir + imagename;
        }
        else
        {
            tileset->_sourceImage = _resources + (_resources.size() ? "/" : "") + imagename;
        }
    }
    else if (elementName == "data")
    {
        std::string encoding    = attributeDict["encoding"].asString();
        std::string compression = attributeDict["compression"].asString();

        if (encoding == "")
        {
            tmxMapInfo->setLayerAttribs(tmxMapInfo->getLayerAttribs() | TMXLayerAttribNone);

            TMXLayerInfo* layer = tmxMapInfo->getLayers().back();
            Vec2 layerSize      = layer->_layerSize;
            auto tilesAmount     = static_cast<size_t>(layerSize.width * layerSize.height);

            layer->_tiles =
                (uint32_t*)axstd::pod_vector<uint32_t>(tilesAmount, 0U).release_pointer();
        }
        else if (encoding == "base64")
        {
            int layerAttribs = tmxMapInfo->getLayerAttribs();
            tmxMapInfo->setLayerAttribs(layerAttribs | TMXLayerAttribBase64);
            tmxMapInfo->setStoringCharacters(true);

            if (compression == "gzip")
            {
                layerAttribs = tmxMapInfo->getLayerAttribs();
                tmxMapInfo->setLayerAttribs(layerAttribs | TMXLayerAttribGzip);
            }
            else if (compression == "zlib")
            {
                layerAttribs = tmxMapInfo->getLayerAttribs();
                tmxMapInfo->setLayerAttribs(layerAttribs | TMXLayerAttribZlib);
            }
            AXASSERT(compression == "" || compression == "gzip" || compression == "zlib",
                     "TMX: unsupported compression method");
        }
        else if (encoding == "csv")
        {
            int layerAttribs = tmxMapInfo->getLayerAttribs();
            tmxMapInfo->setLayerAttribs(layerAttribs | TMXLayerAttribCSV);
            tmxMapInfo->setStoringCharacters(true);
        }
    }
    else if (elementName == "object")
    {
        TMXObjectGroup* objectGroup = tmxMapInfo->getObjectGroups().back();

        // The value for "type" was blank or not a valid class name
        // Create an instance of TMXObjectInfo to store the object and its properties
        ValueMap dict;
        // Parse everything automatically
        const char* keys[] = {"name", "type", "width", "height", "gid", "id"};

        for (const auto& key : keys)
        {
            Value value = attributeDict[key];
            dict[key]   = value;
        }

        // But X and Y since they need special treatment
        // X
        int x = attributeDict["x"].asInt();
        // Y
        int y = attributeDict["y"].asInt();

        Vec2 p(x + objectGroup->getPositionOffset().x, _mapSize.height * _tileSize.height - y -
                                                           objectGroup->getPositionOffset().y -
                                                           attributeDict["height"].asInt());
        p         = AX_POINT_PIXELS_TO_POINTS(p);
        dict["x"] = Value(p.x);
        dict["y"] = Value(p.y);

        int width  = attributeDict["width"].asInt();
        int height = attributeDict["height"].asInt();
        Vec2 s(width, height);
        s              = AX_SIZE_PIXELS_TO_POINTS(s);
        dict["width"]  = Value(s.width);
        dict["height"] = Value(s.height);

        dict["rotation"] = attributeDict["rotation"].asDouble();

        // Add the object to the objectGroup
        objectGroup->getObjects().emplace_back(Value(dict));

        // The parent element is now "object"
        tmxMapInfo->setParentElement(TMXPropertyObject);
    }
    else if (elementName == "property")
    {
        _currentPropertyKey.clear();
        tmxMapInfo->setStoringCharacters(true);
        if (tmxMapInfo->getParentElement() == TMXPropertyNone)
        {
            AXLOGD("TMX tile map: Parent element is unsupported. Cannot add property named '{}' with value '{}'",
                  attributeDict["name"].asString(), attributeDict["value"].asString());
            tmxMapInfo->setStoringCharacters(false);
        }
        else if (tmxMapInfo->getParentElement() == TMXPropertyMap)
        {
            // The parent element is the map
            Value value     = attributeDict["value"];
            std::string key = attributeDict["name"].asString();
            tmxMapInfo->getProperties().emplace(key, value);
            _currentPropertyKey = key;
        }
        else if (tmxMapInfo->getParentElement() == TMXPropertyLayer)
        {
            // The parent element is the last layer
            TMXLayerInfo* layer = tmxMapInfo->getLayers().back();
            Value value         = attributeDict["value"];
            std::string key     = attributeDict["name"].asString();
            // Add the property to the layer
            layer->getProperties().emplace(key, value);
            _currentPropertyKey = key;
        }
        else if (tmxMapInfo->getParentElement() == TMXPropertyObjectGroup)
        {
            // The parent element is the last object group
            TMXObjectGroup* objectGroup = tmxMapInfo->getObjectGroups().back();
            Value value                 = attributeDict["value"];
            std::string key             = attributeDict["name"].asString();
            objectGroup->getProperties().emplace(key, value);
            _currentPropertyKey = key;
        }
        else if (tmxMapInfo->getParentElement() == TMXPropertyObject)
        {
            // The parent element is the last object
            TMXObjectGroup* objectGroup = tmxMapInfo->getObjectGroups().back();
            ValueMap& dict              = objectGroup->getObjects().rbegin()->asValueMap();

            std::string propertyName = attributeDict["name"].asString();
            dict[propertyName]       = attributeDict["value"];
            _currentPropertyKey      = propertyName;
        }
        else if (tmxMapInfo->getParentElement() == TMXPropertyTile)
        {
            ValueMap& dict = tmxMapInfo->getTileProperties().at(tmxMapInfo->getParentGID()).asValueMap();

            std::string propertyName = attributeDict["name"].asString();
            dict[propertyName]       = attributeDict["value"];
            _currentPropertyKey      = propertyName;
        }
    }
    else if (elementName == "polygon")
    {
        // find parent object's dict and add polygon-points to it
        TMXObjectGroup* objectGroup = _objectGroups.back();
        ValueMap& dict              = objectGroup->getObjects().rbegin()->asValueMap();

        // get points value string
        std::string value = attributeDict["points"].asString();
        if (!value.empty())
        {
            ValueVector pointsArray;
            pointsArray.reserve(10);

            // parse points string into a space-separated set of points
            std::stringstream pointsStream(value);
            std::string pointPair;
            while (std::getline(pointsStream, pointPair, ' '))
            {
                // parse each point combo into a comma-separated x,y point
                std::stringstream pointStream(pointPair);
                std::string xStr, yStr;

                ValueMap pointDict;

                // set x
                if (std::getline(pointStream, xStr, ','))
                {
                    int x          = atoi(xStr.c_str()) + (int)objectGroup->getPositionOffset().x;
                    pointDict["x"] = Value(x);
                }

                // set y
                if (std::getline(pointStream, yStr, ','))
                {
                    int y          = atoi(yStr.c_str()) + (int)objectGroup->getPositionOffset().y;
                    pointDict["y"] = Value(y);
                }

                // add to points array
                pointsArray.emplace_back(Value(pointDict));
            }

            dict["points"] = Value(pointsArray);
        }
    }
    else if (elementName == "polyline")
    {
        // find parent object's dict and add polyline-points to it
        TMXObjectGroup* objectGroup = _objectGroups.back();
        ValueMap& dict              = objectGroup->getObjects().rbegin()->asValueMap();

        // get points value string
        std::string value = attributeDict["points"].asString();
        if (!value.empty())
        {
            ValueVector pointsArray;
            pointsArray.reserve(10);

            // parse points string into a space-separated set of points
            std::stringstream pointsStream(value);
            std::string pointPair;
            while (std::getline(pointsStream, pointPair, ' '))
            {
                // parse each point combo into a comma-separated x,y point
                std::stringstream pointStream(pointPair);
                std::string xStr, yStr;

                ValueMap pointDict;

                // set x
                if (std::getline(pointStream, xStr, ','))
                {
                    int x          = atoi(xStr.c_str()) + (int)objectGroup->getPositionOffset().x;
                    pointDict["x"] = Value(x);
                }

                // set y
                if (std::getline(pointStream, yStr, ','))
                {
                    int y          = atoi(yStr.c_str()) + (int)objectGroup->getPositionOffset().y;
                    pointDict["y"] = Value(y);
                }

                // add to points array
                pointsArray.emplace_back(Value(pointDict));
            }

            dict["polylinePoints"] = Value(pointsArray);
        }
    }
    else if (elementName == "animation")
    {
        TMXTilesetInfo* info = tmxMapInfo->getTilesets().back();
        info->_animationInfo.insert(tmxMapInfo->getParentGID(), TMXTileAnimInfo::create(tmxMapInfo->getParentGID()));
        tmxMapInfo->setParentElement(TMXPropertyAnimation);
    }
    else if (elementName == "frame")
    {
        TMXTilesetInfo* info = tmxMapInfo->getTilesets().back();
        auto animInfo        = info->_animationInfo.at(tmxMapInfo->getParentGID());
        // calculate gid of frame
        animInfo->_frames.emplace_back(
            TMXTileAnimFrame(info->_firstGid + attributeDict["tileid"].asInt(), attributeDict["duration"].asFloat()));
    }
}

void TMXMapInfo::endElement(void* /*ctx*/, const char* name)
{
    TMXMapInfo* tmxMapInfo  = this;
    std::string elementName = name;

    if (elementName == "data")
    {
        if (tmxMapInfo->getLayerAttribs() & TMXLayerAttribBase64)
        {
            tmxMapInfo->setStoringCharacters(false);

            TMXLayerInfo* layer = tmxMapInfo->getLayers().back();
            auto currentString = tmxMapInfo->getCurrentString();

            auto buffer = utils::base64Decode(currentString);
            if (buffer.empty())
            {
                AXLOGW("TiledMap: decode data error");
                return;
            }

            if (tmxMapInfo->getLayerAttribs() & (TMXLayerAttribGzip | TMXLayerAttribZlib))
            {
                Vec2 s                  = layer->_layerSize;
                // int sizeHint = s.width * s.height * sizeof(uint32_t);
                ssize_t sizeHint = s.width * s.height * sizeof(unsigned int);

                buffer = ZipUtils::decompressGZ(std::span{buffer}, static_cast<int>(sizeHint));
                AXASSERT(buffer.size() == sizeHint, "inflatedLen should be equal to sizeHint!");

                if (buffer.empty())
                {
                    AXLOGW("TiledMap: inflate data error");
                    return;
                }

                layer->_tiles = reinterpret_cast<uint32_t*>(buffer.release_pointer());
            }
            else
            {
                layer->_tiles = reinterpret_cast<uint32_t*>(buffer.release_pointer());
            }

            tmxMapInfo->setCurrentString("");
        }
        else if (tmxMapInfo->getLayerAttribs() & TMXLayerAttribCSV)
        {
            TMXLayerInfo* layer = tmxMapInfo->getLayers().back();

            tmxMapInfo->setStoringCharacters(false);
            auto currentString = tmxMapInfo->getCurrentString();

            std::vector<std::string> gidTokens;
            std::stringstream filestr;
            filestr << currentString;
            std::string sRow;
            while (std::getline(filestr, sRow, '\n'))
            {
                std::string sGID;
                std::istringstream rowstr(sRow);
                while (std::getline(rowstr, sGID, ','))
                {
                    gidTokens.emplace_back(sGID);
                }
            }

            // 32-bits per gid
            axstd::pod_vector<uint32_t> buffer(gidTokens.size());
            uint32_t* bufferPtr = buffer.data();
            for (const auto& gidToken : gidTokens)
            {
                auto tileGid = (uint32_t)strtoul(gidToken.c_str(), nullptr, 10);
                *bufferPtr   = tileGid;
                bufferPtr++;
            }

            layer->_tiles = buffer.release_pointer();

            tmxMapInfo->setCurrentString("");
        }
        else if (tmxMapInfo->getLayerAttribs() & TMXLayerAttribNone)
        {
            _xmlTileIndex = 0;
        }
    }
    else if (elementName == "map")
    {
        // The map element has ended
        tmxMapInfo->setParentElement(TMXPropertyNone);
    }
    else if (elementName == "layer")
    {
        // The layer element has ended
        tmxMapInfo->setParentElement(TMXPropertyNone);
    }
    else if (elementName == "objectgroup")
    {
        // The objectgroup element has ended
        tmxMapInfo->setParentElement(TMXPropertyNone);
    }
    else if (elementName == "object")
    {
        // The object element has ended
        tmxMapInfo->setParentElement(TMXPropertyNone);
    }
    else if (elementName == "tileset")
    {
        _recordFirstGID = true;
    }
    else if (elementName == "animation")
    {
        tmxMapInfo->setParentElement(TMXPropertyNone);
    }
    else if (elementName == "property")
    {
        tmxMapInfo->setStoringCharacters(false);
        auto currentString = tmxMapInfo->getCurrentString();
        if (!currentString.empty() && !_currentPropertyKey.empty())
        {
            if (tmxMapInfo->getParentElement() == TMXPropertyMap)
            {
                auto& valueMap = tmxMapInfo->getProperties();
                if (valueMap.contains(_currentPropertyKey) == true)
                {
                    valueMap[_currentPropertyKey] = currentString;
                }
            }
            else if (tmxMapInfo->getParentElement() == TMXPropertyLayer)
            {
                auto layer     = tmxMapInfo->getLayers().back();
                auto& valueMap = layer->getProperties();
                if (valueMap.contains(_currentPropertyKey) == true)
                {
                    valueMap[_currentPropertyKey] = currentString;
                }
            }
            else if (tmxMapInfo->getParentElement() == TMXPropertyObjectGroup)
            {
                auto objectGroup = tmxMapInfo->getObjectGroups().back();
                auto& valueMap   = objectGroup->getProperties();
                if (valueMap.contains(_currentPropertyKey) == true)
                {
                    valueMap[_currentPropertyKey] = currentString;
                }
            }
            else if (tmxMapInfo->getParentElement() == TMXPropertyObject)
            {
                auto objectGroup          = tmxMapInfo->getObjectGroups().back();
                ValueMap& dict            = objectGroup->getObjects().rbegin()->asValueMap();
                dict[_currentPropertyKey] = currentString;
            }
            else if (tmxMapInfo->getParentElement() == TMXPropertyTile)
            {
                ValueMap& dict            = tmxMapInfo->getTileProperties().at(tmxMapInfo->getParentGID()).asValueMap();
                dict[_currentPropertyKey] = currentString;
            }
        }
        tmxMapInfo->setCurrentString("");
        _currentPropertyKey.clear();
    }
}

void TMXMapInfo::textHandler(void* /*ctx*/, const char* ch, size_t len)
{
    TMXMapInfo* tmxMapInfo = this;
    std::string text(ch, 0, len);

    if (!_currentPropertyKey.empty())
    {
        text = std::regex_replace(text, std::regex("[\r]"), "");
    }
    else
    {
        text = std::regex_replace(text, std::regex("[\n\r ]"), "");
    }

    if (tmxMapInfo->isStoringCharacters())
    {
        std::string currentString{tmxMapInfo->getCurrentString()};
        currentString += text;
        tmxMapInfo->setCurrentString(currentString);
    }
}

TMXTileAnimFrame::TMXTileAnimFrame(uint32_t tileID, float duration) : _tileID(tileID), _duration(duration) {}

TMXTileAnimInfo::TMXTileAnimInfo(uint32_t tileID) : _tileID(tileID) {}

TMXTileAnimInfo* TMXTileAnimInfo::create(uint32_t tileID)
{
    TMXTileAnimInfo* ret = new TMXTileAnimInfo(tileID);
    ret->autorelease();
    return ret;
}

NS_AX_END
