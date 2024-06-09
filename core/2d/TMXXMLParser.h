/****************************************************************************
Copyright (c) 2009-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#ifndef __AX_TM_XML_PARSER__
#define __AX_TM_XML_PARSER__

/// @cond DO_NOT_SHOW

#include "math/Math.h"
#include "platform/SAXParser.h"
#include "base/Vector.h"
#include "base/Map.h"
#include "base/Value.h"
#include "2d/TMXObjectGroup.h"  // needed for Vector<TMXObjectGroup*> for binding

#include <string>

NS_AX_BEGIN

class TMXLayerInfo;
class TMXTilesetInfo;

/** @file
 * Internal TMX parser
 *
 * IMPORTANT: These classed should not be documented using doxygen strings
 * since the user should not use them.
 *
 */

/**
 * @addtogroup tilemap_parallax_nodes
 * @{
 */

enum
{
    TMXLayerAttribNone   = 1 << 0,
    TMXLayerAttribBase64 = 1 << 1,
    TMXLayerAttribGzip   = 1 << 2,
    TMXLayerAttribZlib   = 1 << 3,
    TMXLayerAttribCSV    = 1 << 4,
};

enum
{
    TMXPropertyNone,
    TMXPropertyMap,
    TMXPropertyLayer,
    TMXPropertyObjectGroup,
    TMXPropertyObject,
    TMXPropertyTile,
    TMXPropertyAnimation
};

typedef enum TMXTileFlags_
{
    kTMXTileHorizontalFlag = 0x80000000,
    kTMXTileVerticalFlag   = 0x40000000,
    kTMXTileDiagonalFlag   = 0x20000000,
    kTMXFlipedAll          = (kTMXTileHorizontalFlag | kTMXTileVerticalFlag | kTMXTileDiagonalFlag),
    kTMXFlippedMask        = ~(kTMXFlipedAll)
} TMXTileFlags;

/**
 * @addtogroup _2d
 * @{
 */

/** Possible orientations of the TMX map. */
enum
{
    /** Orthogonal orientation. */
    TMXOrientationOrtho,

    /** Hexagonal orientation. */
    TMXOrientationHex,

    /** Isometric orientation. */
    TMXOrientationIso,

    /** Isometric staggered orientation. */
    TMXOrientationStaggered,
};

/** Possible stagger axis of the TMX map. */
enum
{
    /** Stagger Axis x. */
    TMXStaggerAxis_X,

    /** Stagger Axis y. */
    TMXStaggerAxis_Y,
};

/** Possible stagger index of the TMX map. */
enum
{
    /** Stagger Index: Odd */
    TMXStaggerIndex_Odd,

    /** Stagger Index: Even */
    TMXStaggerIndex_Even,
};

/** @brief TMXTileAnimFrame contains the information about the frame of a animated tile like:
- Frame gid
- duration of this frame

This information is obtained from the TMX file.
*/
struct AX_DLL TMXTileAnimFrame
{
    TMXTileAnimFrame(uint32_t tileID, float duration);
    /** gid of the frame */
    uint32_t _tileID = 0;
    /** duration of the frame */
    float _duration = 0.0f;
};

/** @brief TMXTileAnimFlag contains animated tile position and flag
This information is obtained from the TMX file.
*/
struct AX_DLL TMXTileAnimFlag
{
    /** position of tile */
    ax::Vec2 _tilePos;
    /** flag of a tile */
    uint32_t _flag = 0;
};

/** @brief TMXTileAnimInfo contains the information about the animated tile like:
- Animated Tile gid
- frames the animated tile contains

This information is obtained from the TMX file.
*/
struct AX_DLL TMXTileAnimInfo : public Object
{
    static TMXTileAnimInfo* create(uint32_t tileID);
    explicit TMXTileAnimInfo(uint32_t tileID);
    uint32_t _tileID = 0;
    std::vector<TMXTileAnimFrame> _frames;
};

// Bits on the far end of the 32-bit global tile ID (GID's) are used for tile flags

/** @brief TMXLayerInfo contains the information about the layers like:
- Layer name
- Layer size
- Layer opacity at creation time (it can be modified at runtime)
- Whether the layer is visible (if it's not visible, then the CocosNode won't be created)

This information is obtained from the TMX file.
*/
class AX_DLL TMXLayerInfo : public Object
{
public:
    /**
     * @js ctor
     */
    TMXLayerInfo();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~TMXLayerInfo();

    void setProperties(ValueMap properties);
    ValueMap& getProperties();

    ValueMap _properties;
    std::string _name;
    Vec2 _layerSize;
    uint32_t* _tiles;
    bool _visible;
    unsigned char _opacity;
    bool _ownTiles;
    Vec2 _offset;
};

/** @brief TMXTilesetInfo contains the information about the tilesets like:
- Tileset name
- Tileset spacing
- Tileset margin
- size of the tiles
- Image used for the tiles
- Image size

This information is obtained from the TMX file.
*/
class AX_DLL TMXTilesetInfo : public Object
{
public:
    std::string _name;
    int _firstGid;
    Vec2 _tileSize;
    int _spacing;
    int _margin;
    Vec2 _tileOffset;
    //! filename containing the tiles (should be spritesheet / texture atlas)
    std::string _sourceImage;
    //! size in pixels of the image
    Vec2 _imageSize;
    std::string _originSourceImage;
    //! map from gid of animated tile to its animation info
    Map<uint32_t, TMXTileAnimInfo*> _animationInfo;

public:
    /**
     * @js ctor
     */
    TMXTilesetInfo();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~TMXTilesetInfo();
    Rect getRectForGID(uint32_t gid);
};

/** @brief TMXMapInfo contains the information about the map like:
- Map orientation (hexagonal, isometric or orthogonal)
- Tile size
- Map size

And it also contains:
- Layers (an array of TMXLayerInfo objects)
- Tilesets (an array of TMXTilesetInfo objects)
- ObjectGroups (an array of TMXObjectGroupInfo objects)

This information is obtained from the TMX file.

*/
class AX_DLL TMXMapInfo : public Object, public SAXDelegator
{
public:
    /** creates a TMX Format with a tmx file */
    static TMXMapInfo* create(std::string_view tmxFile);
    /** creates a TMX Format with an XML string and a TMX resource path */
    static TMXMapInfo* createWithXML(std::string_view tmxString, std::string_view resourcePath);

    /**
     * @js ctor
     */
    TMXMapInfo();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~TMXMapInfo();

    /** initializes a TMX format with a  tmx file */
    bool initWithTMXFile(std::string_view tmxFile);
    /** initializes a TMX format with an XML string and a TMX resource path */
    bool initWithXML(std::string_view tmxString, std::string_view resourcePath);
    /** initializes parsing of an XML file, either a tmx (Map) file or tsx (Tileset) file */
    bool parseXMLFile(std::string_view xmlFilename);
    /* initializes parsing of an XML string, either a tmx (Map) string or tsx (Tileset) string */
    bool parseXMLString(std::string_view xmlString);

    ValueMapIntKey& getTileProperties() { return _tileProperties; };
    void setTileProperties(const ValueMapIntKey& tileProperties) { _tileProperties = tileProperties; }

    /// map orientation
    int getOrientation() const { return _orientation; }
    void setOrientation(int orientation) { _orientation = orientation; }

    /// map staggeraxis
    int getStaggerAxis() const { return _staggerAxis; }
    void setStaggerAxis(int staggerAxis) { _staggerAxis = staggerAxis; }

    /// map stagger index
    int getStaggerIndex() const { return _staggerIndex; }
    void setStaggerIndex(int staggerIndex) { _staggerIndex = staggerIndex; }

    /// map hexsidelength
    int getHexSideLength() const { return _hexSideLength; }
    void setHexSideLength(int hexSideLength) { _hexSideLength = hexSideLength; }

    /// map width & height
    const Vec2& getMapSize() const { return _mapSize; }
    void setMapSize(const Vec2& mapSize) { _mapSize = mapSize; }

    /// tiles width & height
    const Vec2& getTileSize() const { return _tileSize; }
    void setTileSize(const Vec2& tileSize) { _tileSize = tileSize; }

    /// Layers
    const Vector<TMXLayerInfo*>& getLayers() const { return _layers; }
    Vector<TMXLayerInfo*>& getLayers() { return _layers; }
    void setLayers(const Vector<TMXLayerInfo*>& layers) { _layers = layers; }

    /// tilesets
    const Vector<TMXTilesetInfo*>& getTilesets() const { return _tilesets; }
    Vector<TMXTilesetInfo*>& getTilesets() { return _tilesets; }
    void setTilesets(const Vector<TMXTilesetInfo*>& tilesets) { _tilesets = tilesets; }

    /// ObjectGroups
    const Vector<TMXObjectGroup*>& getObjectGroups() const { return _objectGroups; }
    Vector<TMXObjectGroup*>& getObjectGroups() { return _objectGroups; }
    void setObjectGroups(const Vector<TMXObjectGroup*>& groups) { _objectGroups = groups; }

    /// parent element
    int getParentElement() const { return _parentElement; }
    void setParentElement(int element) { _parentElement = element; }

    /// parent GID
    int getParentGID() const { return _parentGID; }
    void setParentGID(int gid) { _parentGID = gid; }

    /// layer attribs
    int getLayerAttribs() const { return _layerAttribs; }
    void setLayerAttribs(int layerAttribs) { _layerAttribs = layerAttribs; }

    /// is storing characters?
    bool isStoringCharacters() const { return _storingCharacters; }
    void setStoringCharacters(bool storingCharacters) { _storingCharacters = storingCharacters; }

    /// properties
    const ValueMap& getProperties() const { return _properties; }
    ValueMap& getProperties() { return _properties; }
    void setProperties(const ValueMap& properties) { _properties = properties; }

    // implement pure virtual methods of SAXDelegator
    /**
     * @js NA
     * @lua NA
     */
    void startElement(void* ctx, const char* name, const char** atts) override;
    /**
     * @js NA
     * @lua NA
     */
    void endElement(void* ctx, const char* name) override;
    /**
     * @js NA
     * @lua NA
     */
    void textHandler(void* ctx, const char* ch, size_t len) override;

    std::string_view getCurrentString() const { return _currentString; }
    void setCurrentString(std::string_view currentString) { _currentString = currentString; }
    std::string_view getTMXFileName() const { return _TMXFileName; }
    void setTMXFileName(std::string_view fileName) { _TMXFileName = fileName; }
    std::string_view getExternalTilesetFileName() const { return _externalTilesetFilename; }

protected:
    void internalInit(std::string_view tmxFileName, std::string_view resourcePath);

    /// map orientation
    int _orientation;
    /// map staggerAxis
    int _staggerAxis;
    /// map staggerIndex
    int _staggerIndex;
    /// map hexsidelength
    int _hexSideLength;
    /// map width & height
    Vec2 _mapSize;
    /// tiles width & height
    Vec2 _tileSize;
    /// Layers
    Vector<TMXLayerInfo*> _layers;
    /// tilesets
    Vector<TMXTilesetInfo*> _tilesets;
    /// ObjectGroups
    Vector<TMXObjectGroup*> _objectGroups;
    /// parent element
    int _parentElement;
    /// parent GID
    int _parentGID;
    /// layer attribs
    int _layerAttribs;
    /// is storing characters?
    bool _storingCharacters;
    /// properties
    ValueMap _properties;
    //! xml format tile index
    int _xmlTileIndex;

    //! tmx filename
    std::string _TMXFileName;
    // tmx resource path
    std::string _resources;
    //! current string
    std::string _currentString;
    //! tile properties
    ValueMapIntKey _tileProperties;
    int _currentFirstGID;
    bool _recordFirstGID;
    std::string _externalTilesetFilename;
    //! current property key
    std::string _currentPropertyKey;
};

// end of tilemap_parallax_nodes group
/// @}

NS_AX_END

/// @endcond
#endif
