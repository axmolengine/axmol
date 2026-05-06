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
#pragma once

#include <span>
#include <unordered_map>
#include <vector>
#include "axmol/scene/Node.h"
#include "axmol/2d/TMXXMLParser.h"
#include "axmol/renderer/CustomCommand.h"

namespace ax
{

class TMXMapInfo;
class TMXLayerInfo;
class TMXTilesetInfo;
class TMXTileAnimManager;
class Texture2D;
class Sprite;

namespace rhi
{
class Buffer;
}

/**
 * @addtogroup _2d
 * @{
 */

/**
 * !!! uncomment if you want reduce bandwidth of GPU, then the tiled layer size will be limited to 128x128
 */
#define AX_FAST_TILEMAP_32_BIT_INDICES 1

/**
 * Per-tileset rendering batch used by FastTMXLayer.
 * Holds the index buffer, draw commands, and tileset metadata for one tileset
 * contributing tiles to a layer. The shared vertex buffer lives in FastTMXLayer.
 */
struct TilesetBatch
{
    TMXTilesetInfo* tilesetInfo = nullptr;
    Texture2D* texture          = nullptr;
    rhi::Buffer* indexBuffer    = nullptr;

#ifdef AX_FAST_TILEMAP_32_BIT_INDICES
    std::vector<unsigned int> indices;
#else
    std::vector<unsigned short> indices;
#endif

    std::map<int, int> indicesVertexZOffsets;
    std::unordered_map<int, int> indicesVertexZNumber;
    std::unordered_map<int, CustomCommand*> customCommands;

    rhi::UniformLocation mvpMatrixLocation;
    rhi::UniformLocation textureLocation;
    rhi::UniformLocation alphaValueLocation;
};

/** @brief FastTMXLayer represents the TMX layer.

 * It is a subclass of SpriteBatchNode. By default the tiles are rendered using a TextureAtlas.
 * If you modify a tile on runtime, then, that tile will become a Sprite, otherwise no Sprite objects are created.
 * The benefits of using Sprite objects as tiles are:
 * - tiles (Sprite) can be rotated/scaled/moved with a nice API.

 * If the layer contains a property named "cc_vertexz" with an integer (in can be positive or negative),
 * then all the tiles belonging to the layer will use that value as their OpenGL vertex Z for depth.

 * On the other hand, if the "cc_vertexz" property has the "automatic" value, then the tiles will use an automatic
 vertex Z value.
 * Also before drawing the tiles, GL_ALPHA_TEST will be enabled, and disabled after drawing them. The used alpha func
 will be:

 * glAlphaFunc( GL_GREATER, value ).

 * "value" by default is 0, but you can change it from Tiled by adding the "cc_alpha_func" property to the layer.
 * The value 0 should work for most cases, but if you have tiles that are semi-transparent, then you might want to use a
 different
 * value, like 0.5.

 * For further information, please see the programming guide:
 * http://www.cocos2d-iphone.org/wiki/doku.php/prog_guide:tiled_maps

 * @since v3.2
 */

class AX_DLL FastTMXLayer : public Node
{
public:
    /** Possible orientations of the TMX map */
    static const int FAST_TMX_ORIENTATION_ORTHO;
    static const int FAST_TMX_ORIENTATION_HEX;
    static const int FAST_TMX_ORIENTATION_ISO;

    /** Creates a FastTMXLayer with an tileset info, a layer info and a map info.
     *
     * @param tilesetInfo An tileset info.
     * @param layerInfo A layer info.
     * @param mapInfo A map info.
     * @return Return an autorelease object.
     */
    static FastTMXLayer* create(TMXTilesetInfo* tilesetInfo, TMXLayerInfo* layerInfo, TMXMapInfo* mapInfo);

    /** Creates a FastTMXLayer with multiple tilesets (one layer may reference tiles from several tilesets). */
    static FastTMXLayer* create(std::span<TMXTilesetInfo* const> tilesets,
                                TMXLayerInfo* layerInfo,
                                TMXMapInfo* mapInfo);
    /**
     */
    FastTMXLayer();
    /**
     * @lua NA
     */
    virtual ~FastTMXLayer();

    /** Returns the tile gid at a given tile coordinate. It also returns the tile flags.
     *
     * @param tileCoordinate The tile coordinate.
     * @param flags A TMXTileFlags.
     * @return The tile gid at a given tile coordinate. It also returns the tile flags.
     */
    int getTileGIDAt(const Vec2& tileCoordinate, TMXTileFlags* flags = nullptr);

    /** Sets the tile gid (gid = tile global id) at a given tile coordinate.
     * The Tile GID can be obtained by using the method "tileGIDAt" or by using the TMX editor -> Tileset Mgr +1.
     * If a tile is already placed at that position, then it will be removed.
     * @param gid The gid value.
     * @param tileCoordinate The tile coordinate.
     */
    void setTileGID(int gid, const Vec2& tileCoordinate);

    /** Sets the tile gid (gid = tile global id) at a given tile coordinate.
     * The Tile GID can be obtained by using the method "tileGIDAt" or by using the TMX editor -> Tileset Mgr +1.
     * If a tile is already placed at that position, then it will be removed.
     * Use withFlags if the tile flags need to be changed as well.
     *
     * @param gid A integer value,it will be sets the tile gid.
     * @param tileCoordinate The tile coordinate.
     * @param flags A TMXTileFlags.
     */
    void setTileGID(int gid, const Vec2& tileCoordinate, TMXTileFlags flags);

    /** Removes a tile at given tile coordinate.
     *
     * @param tileCoordinate The tile Coordinate.
     */
    void removeTileAt(const Vec2& tileCoordinate);

    /** Returns the position in points of a given tile coordinate.
     *
     * @param tileCoordinate The tile Coordinate.
     * @return The position in points of a given tile coordinate.
     */
    Vec2 getPositionAt(const Vec2& tileCoordinate);

    /** Return the value for the specific property name.
     *
     * @param propertyName The value for the specific property name.
     * @return The value for the specific property name.
     */
    Value getProperty(std::string_view propertyName) const;

    /** Creates the tiles. */
    void setupTiles();

    /** Get the tile layer name.
     *
     * @return The tile layer name.
     */
    std::string_view getLayerName() { return _layerName; }

    /** Set the tile layer name.
     *
     * @param layerName The new layer name.
     */
    void setLayerName(std::string_view layerName) { _layerName = layerName; }

    /** Gets the size of the layer in tiles.
     *
     * @return The size of the layer in tiles.
     */
    const Vec2& getLayerSize() const { return _layerSize; }

    /** Set the size of the layer in tiles.
     *
     * @param size The new size of the layer in tiles.
     */
    void setLayerSize(const Vec2& size) { _layerSize = size; }

    /** Gets the size of the map's tile (could be different from the tile's size).
     *
     * @return The size of the map's tile (could be different from the tile's size).
     */
    const Vec2& getMapTileSize() const { return _mapTileSize; }

    /** Set the size of the map's tile.
     *
     * @param size The new size of the map's tile.
     */
    void setMapTileSize(const Vec2& size) { _mapTileSize = size; }

    /** Pointer to the map of tiles.
     * @lua NA
     * @return The pointer to the map of tiles.
     */
    const uint32_t* getTiles() const { return _tiles; };

    /** Set the pointer to the map of tiles.
     *
     * @param tiles The pointer to the map of tiles.
     */
    void setTiles(uint32_t* tiles)
    {
        _tiles      = tiles;
        _quadsDirty = true;
    };

    /** Tileset information for the layer.
     *
     * @return Tileset information for the layer.
     */
    TMXTilesetInfo* getTileSet() const { return _batches.empty() ? nullptr : _batches[0].tilesetInfo; }

    /** Set the tileset information for the layer (updates the primary batch). */
    void setTileSet(TMXTilesetInfo* info);

    /** Layer orientation, which is the same as the map orientation.
     *
     * @return Layer orientation, which is the same as the map orientation.
     */
    int getLayerOrientation() const { return _layerOrientation; }

    /** Set Layer orientation, which is the same as the map orientation.
     *
     * @param orientation Layer orientation, which is the same as the map orientation.
     */
    void setLayerOrientation(int orientation) { _layerOrientation = orientation; }

    /** Properties from the layer. They can be added using Tiled.
     *
     * @return Properties from the layer. They can be added using Tiled.
     */
    const ValueMap& getProperties() const { return _properties; }

    /** Properties from the layer. They can be added using Tiled.
     *
     * @return Properties from the layer. They can be added using Tiled.
     */
    ValueMap& getProperties() { return _properties; }

    /** Set the properties to the layer.
     *
     * @param properties The properties to the layer.
     */
    void setProperties(const ValueMap& properties) { _properties = properties; }

    /** Returns the tile (Sprite) at a given a tile coordinate.
     * The returned Sprite will be already added to the TMXLayer. Don't add it again.
     * The Sprite can be treated like any other Sprite: rotated, scaled, translated, opacity, color, etc.
     * You can remove either by calling:
     * - layer->removeChild(sprite, cleanup);
     *
     * @return Returns the tile (Sprite) at a given a tile coordinate.
     */
    Sprite* getTileAt(const Vec2& tileCoordinate);

    /** Set an sprite to the tile,with the tile coordinate and gid.
     *
     * @param sprite A Sprite.
     * @param pos The tile coordinate.
     * @param gid The tile gid.
     */
    void setupTileSprite(Sprite* sprite, const Vec2& pos, uint32_t gid);

    //
    // Override
    //
    std::string getDescription() const override;
    void draw(Renderer* renderer, const Mat4& transform, uint32_t flags) override;
    void removeChild(Node* child, bool cleanup = true) override;

    /** Map from gid of animated tile to its instance.
     *
     * @return Map from gid of animated tile to its instance.
     */
    const std::unordered_map<uint32_t, std::vector<TMXTileAnimFlag>>* getAnimTileCoord() { return &_animTileCoord; }

    bool hasTileAnimation() const { return !_animTileCoord.empty(); }

    TMXTileAnimManager* getTileAnimManager() const { return _tileAnimManager; }

    bool initWithTilesetInfo(TMXTilesetInfo* tilesetInfo, TMXLayerInfo* layerInfo, TMXMapInfo* mapInfo);
    bool initWithTilesets(std::span<TMXTilesetInfo* const> tilesets, TMXLayerInfo* layerInfo, TMXMapInfo* mapInfo);

    /** Returns the batch index owning the given GID, or -1 if the GID is empty/unknown. */
    int batchIndexForGID(uint32_t gid) const;

    /** Read-only access to all tileset batches (needed by TMXTileAnimManager). */
    const std::vector<TilesetBatch>& getBatches() const { return _batches; }

protected:
    void setOpacity(uint8_t opacity) override;

    void updateTiles(const Rect& culledRect);
    Vec2 calculateLayerOffset(const Vec2& offset);

    /* The layer recognizes some special properties, like cc_vertexz */
    void parseInternalProperties();

    Mat4 tileToNodeTransform();
    Rect tileBoundsForClipTransform(const Mat4& tileToClip);

    int getVertexZForPos(const Vec2& pos);

    // Flip flags is packed into gid
    void setFlaggedTileGIDByIndex(int index, uint32_t gid);

    //
    void updateTotalQuads();

    int getTileIndexByPos(int x, int y) const { return x + y * (int)_layerSize.width; }

    void updateVertexBuffer();
    void updateIndexBuffers();
    void updatePrimitives();
    void recomputeMaxTileSize();

    //! name of the layer
    std::string _layerName;

    /** size of the layer in tiles */
    Vec2 _layerSize;
    /** size of the map's tile (could be different from the tile's size) */
    Vec2 _mapTileSize;
    /** pointer to the map of tiles */
    uint32_t* _tiles = nullptr;
    /** All tileset batches for this layer (one per tileset used, sorted descending by firstGid). */
    std::vector<TilesetBatch> _batches;
    /** Layer orientation, which is the same as the map orientation */
    int _layerOrientation = FAST_TMX_ORIENTATION_ORTHO;
    int _staggerAxis      = TMXStaggerAxis_Y;
    int _staggerIndex     = TMXStaggerIndex_Even;
    /** properties from the layer. They can be added using Tiled */
    ValueMap _properties;

    /** map from gid of animated tile to its instance. Also useful for optimization*/
    std::unordered_map<uint32_t, std::vector<TMXTileAnimFlag>> _animTileCoord;
    /** pointer to the tile animation manager of this layer */
    TMXTileAnimManager* _tileAnimManager = nullptr;

    /** Maps tile-flat-index → batch index in _batches (-1 for empty tiles). Parallel to _tileToQuadIndex. */
    std::vector<int> _tilesetBatchIndex;

    /** container for sprite children. map<index, pair<sprite, gid> > */
    std::map<int, std::pair<Sprite*, uint32_t>> _spriteContainer;

    Vec2 _screenGridSize;
    Rect _screenGridRect;
    int _screenTileCount = 0;

    int _vertexZvalue         = 0;
    bool _useAutomaticVertexZ = false;

    /** tile coordinate to node coordinate transform */
    Mat4 _tileToNodeTransform;
    /** data for rendering */
    bool _quadsDirty          = true;
    Vec2 _cameraPositionDirty = {INFINITY, INFINITY};
    float _cameraZoomDirty    = NAN;

    std::vector<int> _tileToQuadIndex;
    std::vector<V3F_T2F_C4B_Quad> _totalQuads;
    bool _dirty = true;

    rhi::Buffer* _vertexBuffer = nullptr;

    /** Maximum tile size across all batches, in pixels. Cached at batch-change time. */
    Vec2 _maxTileSize;

    float _alphaFuncValue = 0.f;
};

/** @brief TMXTileAnimTask represents the frame-tick task of an animated tile.
 * It is a assistant class for TMXTileAnimTicker.
 */
class AX_DLL TMXTileAnimTask : public Object
{
public:
    TMXTileAnimTask(FastTMXLayer* layer, TMXTileAnimInfo* animation, const Vec2& tilePos, uint32_t flag = 0);
    static TMXTileAnimTask* create(FastTMXLayer* layer,
                                   TMXTileAnimInfo* animation,
                                   const Vec2& tilePos,
                                   uint32_t flag = 0);
    /** start the animation task */
    void start();
    /** stop the animation task */
    void stop();
    bool isRunning() const { return _isRunning; }

protected:
    /** tile flag */
    uint32_t _flag = 0;

    /** set texture of tile to current frame */
    void setCurrFrame();
    /** tick to next frame and schedule next tick */
    void tickAndScheduleNext(float dt);

    bool _isRunning = false;
    /** key of schedule task for specific animated tile */
    std::string _key;
    FastTMXLayer* _layer = nullptr;
    /** position of the animated tile */
    Vec2 _tilePosition;
    /** AnimationInfo on this tile */
    TMXTileAnimInfo* _animation = nullptr;
    /** Index of the frame that should be drawn currently */
    uint32_t _currentFrame = 0;
    uint32_t _nextFrame    = 0;
    uint32_t _frameCount   = 0;
};

/** @brief TMXTileAnimManager controls all tile animation of a layer.
 */
class AX_DLL TMXTileAnimManager : public Object
{
public:
    static TMXTileAnimManager* create(FastTMXLayer* layer);
    explicit TMXTileAnimManager(FastTMXLayer* layer);

    /** start all tile animations */
    void startAll();
    /** stop all tile animations */
    void stopAll();

    /** get vector of tasks */
    const Vector<TMXTileAnimTask*>& getTasks() const { return _tasks; }

protected:
    bool _started = false;
    /** vector contains all tasks of this layer */
    Vector<TMXTileAnimTask*> _tasks;
    FastTMXLayer* _layer = nullptr;
};

// @API compatible
typedef FastTMXLayer TMXLayer;

// end of tilemap_parallax_nodes group
/// @}
}  // namespace ax
