/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2011 HKASoftware
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

/*
 Original rewrite of TMXLayer was based on HKTMXTiledMap by HKASoftware http://hkasoftware.com
 Further info: http://www.cocos2d-iphone.org/forums/topic/hktmxtiledmap/

 It was rewritten again, and only a small part of the original HK ideas/code remains in this implementation

 */
#include "axmol/2d/FastTMXLayer.h"
#include <span>
#include <stddef.h>  // offsetof
#include "axmol/base/Types.h"
#include "axmol/2d/FastTMXTiledMap.h"
#include "axmol/2d/Sprite.h"
#include "axmol/scene/Camera.h"
#include "axmol/renderer/TextureCache.h"
#include "axmol/renderer/Renderer.h"
#include "axmol/renderer/Shaders.h"
#include "axmol/rhi/DriverContext.h"
#include "axmol/rhi/Buffer.h"
#include "axmol/base/Director.h"
#include "axmol/base/text_utils.h"
#include "axmol/rhi/ProgramState.h"

namespace ax
{

const int FastTMXLayer::FAST_TMX_ORIENTATION_ORTHO = 0;
const int FastTMXLayer::FAST_TMX_ORIENTATION_HEX   = 1;
const int FastTMXLayer::FAST_TMX_ORIENTATION_ISO   = 2;

// FastTMXLayer - init & alloc & dealloc
FastTMXLayer* FastTMXLayer::create(TMXTilesetInfo* tilesetInfo, TMXLayerInfo* layerInfo, TMXMapInfo* mapInfo)
{
    return tilesetInfo ? create(std::span<TMXTilesetInfo* const>{&tilesetInfo, 1}, layerInfo, mapInfo) : nullptr;
}

FastTMXLayer* FastTMXLayer::create(std::span<TMXTilesetInfo* const> tilesets,
                                   TMXLayerInfo* layerInfo,
                                   TMXMapInfo* mapInfo)
{
    FastTMXLayer* ret = new FastTMXLayer();
    if (ret->initWithTilesets(tilesets, layerInfo, mapInfo))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool FastTMXLayer::initWithTilesetInfo(TMXTilesetInfo* tilesetInfo, TMXLayerInfo* layerInfo, TMXMapInfo* mapInfo)
{
    if (tilesetInfo)
    {
        TMXTilesetInfo* ts[] = {tilesetInfo};
        return initWithTilesets(ts, layerInfo, mapInfo);
    }
    return initWithTilesets({}, layerInfo, mapInfo);
}

bool FastTMXLayer::initWithTilesets(std::span<TMXTilesetInfo* const> tilesets,
                                    TMXLayerInfo* layerInfo,
                                    TMXMapInfo* mapInfo)
{
    if (!layerInfo || !mapInfo)
        return false;

    auto* cache = _director->getTextureCache();
    _batches.reserve(tilesets.size());
    for (auto* ts : tilesets)
    {
        if (!ts)
            continue;
        TilesetBatch batch;
        batch.tilesetInfo = ts;
        AX_SAFE_RETAIN(batch.tilesetInfo);
        batch.texture = cache->addImage(ts->_sourceImage);
        if (batch.texture)
            batch.texture->retain();
        _batches.push_back(std::move(batch));
    }

    // layerInfo
    _layerName  = layerInfo->_name;
    _layerSize  = layerInfo->_layerSize;
    _tiles      = layerInfo->_tiles;
    _quadsDirty = true;
    setOpacity(layerInfo->_opacity);
    setProperties(layerInfo->getProperties());

    // mapInfo
    _mapTileSize      = mapInfo->getTileSize();
    _layerOrientation = mapInfo->getOrientation();
    _staggerAxis      = mapInfo->getStaggerAxis();
    _staggerIndex     = mapInfo->getStaggerIndex();

    Vec2 offset = this->calculateLayerOffset(layerInfo->_offset);
    this->setPosition(AX_POINT_PIXELS_TO_POINTS(offset));
    this->setContentSize(
        AX_SIZE_PIXELS_TO_POINTS(Vec2(_layerSize.width * _mapTileSize.width, _layerSize.height * _mapTileSize.height)));

    this->tileToNodeTransform();
    _useAutomaticVertexZ = false;
    _vertexZvalue        = 0;
    recomputeMaxTileSize();
    return true;
}

FastTMXLayer::FastTMXLayer() {}

FastTMXLayer::~FastTMXLayer()
{
    AX_SAFE_RELEASE(_tileAnimManager);
    for (auto& batch : _batches)
    {
        AX_SAFE_RELEASE(batch.tilesetInfo);
        AX_SAFE_RELEASE(batch.texture);
        AX_SAFE_RELEASE(batch.indexBuffer);
        for (auto& [_, cmd] : batch.customCommands)
        {
            cmd->releasePSVL();
            delete cmd;
        }
    }
    AX_SAFE_FREE(_tiles);
    AX_SAFE_RELEASE(_vertexBuffer);
}

void FastTMXLayer::setTileSet(TMXTilesetInfo* info)
{
    if (!info)
        return;

    for (auto& batch : _batches)
    {
        AX_SAFE_RELEASE(batch.tilesetInfo);
        AX_SAFE_RELEASE(batch.texture);
        AX_SAFE_RELEASE(batch.indexBuffer);
        for (auto& [_, cmd] : batch.customCommands)
        {
            cmd->releasePSVL();
            delete cmd;
        }
    }
    _batches.clear();

    TilesetBatch batch;
    batch.tilesetInfo = info;
    AX_SAFE_RETAIN(batch.tilesetInfo);
    batch.texture = _director->getTextureCache()->addImage(info->_sourceImage);
    if (batch.texture)
        batch.texture->retain();
    _batches.push_back(std::move(batch));

    _quadsDirty = true;
    recomputeMaxTileSize();
}

void FastTMXLayer::recomputeMaxTileSize()
{
    _maxTileSize = Vec2::zero;
    for (auto& batch : _batches)
    {
        _maxTileSize.x = std::max(_maxTileSize.x, batch.tilesetInfo->_tileSize.x);
        _maxTileSize.y = std::max(_maxTileSize.y, batch.tilesetInfo->_tileSize.y);
    }
}

int FastTMXLayer::batchIndexForGID(uint32_t gid) const
{
    const uint32_t bareGID = gid & kTMXFlippedMask;
    if (bareGID == 0)
        return -1;
    // _batches is sorted descending by firstGid — first match wins.
    for (int i = 0; i < (int)_batches.size(); ++i)
    {
        if (bareGID >= static_cast<uint32_t>(_batches[i].tilesetInfo->_firstGid))
            return i;
    }
    return -1;
}

void FastTMXLayer::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    updateTotalQuads();

    const float refTileX = _batches.empty() ? 1.0f : _batches[0].tilesetInfo->_tileSize.x;
    auto cam             = Camera::getVisitingCamera();
    if (!cam)
        return;
    if (flags != 0 || _dirty || _quadsDirty || !_cameraPositionDirty.fuzzyEquals(cam->getPosition(), refTileX) ||
        _cameraZoomDirty != cam->getZoom())
    {
        _cameraPositionDirty = cam->getPosition();
        auto zoom = _cameraZoomDirty = cam->getZoom();
        Vec2 s                       = _director->getVisibleSize();
        const Vec2& anchor           = getAnchorPoint();
        auto rect                    = Rect(cam->getPositionX() - s.width * zoom * (anchor.x == 0.0f ? 0.5f : anchor.x),
                                            cam->getPositionY() - s.height * zoom * (anchor.y == 0.0f ? 0.5f : anchor.y), s.width * zoom,
                                            s.height * zoom);

        Mat4 inv = transform;
        inv.inverse();
        rect = RectApplyTransform(rect, inv);

        updateTiles(rect);
        updateIndexBuffers();
        updatePrimitives();
        _dirty = false;
    }

    const auto& projectionMat = Camera::getVisitingViewProjectionMatrix();
    Mat4 finalMat             = projectionMat * _modelViewTransform;
    // Submit batches lowest-firstGid first so base/terrain tiles (low GIDs) draw behind
    // overlay/object tiles (high GIDs) that share the same vertexZ.
    for (auto it = _batches.rbegin(); it != _batches.rend(); ++it)
    {
        for (const auto& [_, cmd] : it->customCommands)
        {
            if (cmd->getIndexDrawCount() > 0)
            {
                // All commands in a batch share the same program, so the cached location is valid.
                cmd->unsafePS()->setUniform(it->mvpMatrixLocation, finalMat.m, sizeof(finalMat.m));
                renderer->addCommand(cmd);
            }
        }
    }
}

void FastTMXLayer::updateTiles(const Rect& culledRect)
{
    Rect visibleTiles        = Rect(culledRect.origin, culledRect.size);
    Vec2 mapTileSize         = AX_SIZE_PIXELS_TO_POINTS(_mapTileSize);
    Vec2 tileSize            = AX_SIZE_PIXELS_TO_POINTS(_maxTileSize);
    Mat4 nodeToTileTransform = _tileToNodeTransform.getInversed();
    // transform to tile
    visibleTiles = RectApplyTransform(visibleTiles, nodeToTileTransform);
    // tile coordinate is upside-down, so we need to make the tile coordinate use top-left for the start point.
    visibleTiles.origin.y += 1;

    // if x=0.7, width=9.5, we need to draw number 0~10 of tiles, and so is height.
    visibleTiles.size.width  = ceil(visibleTiles.origin.x + visibleTiles.size.width) - floor(visibleTiles.origin.x);
    visibleTiles.size.height = ceil(visibleTiles.origin.y + visibleTiles.size.height) - floor(visibleTiles.origin.y);
    visibleTiles.origin.x    = floor(visibleTiles.origin.x);
    visibleTiles.origin.y    = floor(visibleTiles.origin.y);

    // for the bigger tiles.
    int tilesOverX = 0;
    int tilesOverY = 0;
    // for diagonal orientation tiles
    float tileSizeMax = std::max(tileSize.width, tileSize.height);
    if (_layerOrientation == FAST_TMX_ORIENTATION_ORTHO)
    {
        tilesOverX = (int)ceil(tileSizeMax / mapTileSize.width) - 1;
        tilesOverY = (int)ceil(tileSizeMax / mapTileSize.height) - 1;

        if (tilesOverX < 0)
            tilesOverX = 0;
        if (tilesOverY < 0)
            tilesOverY = 0;
    }
    else if (_layerOrientation == FAST_TMX_ORIENTATION_ISO)
    {
        Rect overTileRect(0, 0, tileSizeMax - mapTileSize.width, tileSizeMax - mapTileSize.height);
        if (overTileRect.size.width < 0)
            overTileRect.size.width = 0;
        if (overTileRect.size.height < 0)
            overTileRect.size.height = 0;
        overTileRect = RectApplyTransform(overTileRect, nodeToTileTransform);

        tilesOverX = (int)(ceil(overTileRect.origin.x + overTileRect.size.width) - floor(overTileRect.origin.x));
        tilesOverY = (int)(ceil(overTileRect.origin.y + overTileRect.size.height) - floor(overTileRect.origin.y));
    }
    else
    {
        // do nothing, do not support
        // AXASSERT(0, "TMX invalid value");
    }

    for (auto& batch : _batches)
    {
        batch.indicesVertexZNumber.clear();
        for (const auto& [vz, offset] : batch.indicesVertexZOffsets)
            batch.indicesVertexZNumber[vz] = offset;
    }

    int yBegin = static_cast<int>(std::max(0.f, visibleTiles.origin.y - tilesOverY));
    int yEnd =
        static_cast<int>(std::min(_layerSize.height, visibleTiles.origin.y + visibleTiles.size.height + tilesOverY));
    int xBegin = static_cast<int>(std::max(0.f, visibleTiles.origin.x - tilesOverX));
    int xEnd =
        static_cast<int>(std::min(_layerSize.width, visibleTiles.origin.x + visibleTiles.size.width + tilesOverX));

    for (int y = yBegin; y < yEnd; ++y)
    {
        for (int x = xBegin; x < xEnd; ++x)
        {
            int tileIndex = getTileIndexByPos(x, y);
            if (_tiles[tileIndex] == 0)
                continue;

            int bi = _tilesetBatchIndex[tileIndex];
            if (bi < 0)
                continue;
            auto& batch = _batches[bi];

            int vertexZ = getVertexZForPos(Vec2((float)x, (float)y));
            auto iter   = batch.indicesVertexZNumber.find(vertexZ);
            if (iter == batch.indicesVertexZNumber.end())
                continue;
            int offset = iter->second;
            iter->second++;

            auto quadIndex = static_cast<decltype(batch.indices)::value_type>(_tileToQuadIndex[tileIndex]);
            batch.indices[6 * offset + 0] = quadIndex * 4 + 0;
            batch.indices[6 * offset + 1] = quadIndex * 4 + 1;
            batch.indices[6 * offset + 2] = quadIndex * 4 + 2;
            batch.indices[6 * offset + 3] = quadIndex * 4 + 3;
            batch.indices[6 * offset + 4] = quadIndex * 4 + 2;
            batch.indices[6 * offset + 5] = quadIndex * 4 + 1;

        }  // for x
    }  // for y

    for (auto& batch : _batches)
    {
        for (const auto& [vz, offset] : batch.indicesVertexZOffsets)
        {
            batch.indicesVertexZNumber[vz] -= offset;
            if (batch.indicesVertexZNumber[vz] == 0)
                batch.indicesVertexZNumber.erase(vz);
        }
    }
}

void FastTMXLayer::updateVertexBuffer()
{
    if (_totalQuads.empty())
        return;
    unsigned int vertexBufferSize = (unsigned int)(sizeof(V3F_T2F_C4B) * _totalQuads.size() * 4);
    if (!_vertexBuffer)
    {
        // Allocate for the maximum possible tile count so the buffer never needs to be
        // recreated when setTileGID adds tiles that were absent on the first draw.
        // Recreating the buffer would also invalidate existing CustomCommand references.
        const unsigned int maxSize = (unsigned int)(sizeof(V3F_T2F_C4B) * _layerSize.width * _layerSize.height * 4);
        _vertexBuffer              = axdrv->createBuffer(maxSize, rhi::BufferType::VERTEX, rhi::BufferUsage::STATIC);
    }
    _vertexBuffer->updateData(&_totalQuads[0], vertexBufferSize);
}

void FastTMXLayer::updateIndexBuffers()
{
    for (auto& batch : _batches)
    {
        if (batch.indices.empty())
            continue;
        const auto size = sizeof(decltype(batch.indices)::value_type) * batch.indices.size();
        if (!batch.indexBuffer)
        {
            // Pre-allocate at maximum possible size so the buffer never needs to be
            // recreated when setTileGID adds tiles. Recreating would invalidate existing
            // CustomCommand references (same reasoning as the vertex buffer).
            const auto maxSize = sizeof(decltype(batch.indices)::value_type) * 6 /* 6 indices per quad */ *
                                 static_cast<size_t>(_layerSize.width) * static_cast<size_t>(_layerSize.height);
            batch.indexBuffer = axdrv->createBuffer(maxSize, rhi::BufferType::INDEX, rhi::BufferUsage::DYNAMIC);
        }
        batch.indexBuffer->updateData(batch.indices.data(), size);
    }
}

// FastTMXLayer - setup Tiles
void FastTMXLayer::setupTiles()
{
    float maxTileHeight = 0.f;
    for (auto& batch : _batches)
    {
        if (batch.texture)
        {
            batch.tilesetInfo->_imageSize = batch.texture->getPixelSize();
            batch.texture->setAliasTexParameters();
        }
        maxTileHeight = std::max(maxTileHeight, batch.tilesetInfo->_tileSize.height);
    }

    this->parseInternalProperties();

    auto& screenSize = _director->getCanvasSize();

    switch (_layerOrientation)
    {
    case FAST_TMX_ORIENTATION_ORTHO:
        _screenGridSize.width  = ceil(screenSize.width / _mapTileSize.width) + 1;
        _screenGridSize.height = ceil(screenSize.height / _mapTileSize.height) + 1;
        _screenGridSize.height += maxTileHeight / _mapTileSize.height;
        break;
    case FAST_TMX_ORIENTATION_ISO:
        _screenGridSize.width  = ceil(screenSize.width / _mapTileSize.width) + 2;
        _screenGridSize.height = ceil(screenSize.height / (_mapTileSize.height / 2)) + 4;
        break;
    case FAST_TMX_ORIENTATION_HEX:
    default:
        AXLOGE("FastTMX does not support type {}", _layerOrientation);
        break;
    }

    _screenTileCount = (int)(_screenGridSize.width * _screenGridSize.height);

    // Check any batch for animation info.
    bool hasAnyAnim = false;
    for (auto& batch : _batches)
        hasAnyAnim = hasAnyAnim || !batch.tilesetInfo->_animationInfo.empty();

    if (hasAnyAnim)
    {
        /// FastTMXLayer: anim support
        for (int y = 0; y < _layerSize.height; y++)
        {
            for (int x = 0; x < _layerSize.width; x++)
            {
                int newX = x;
                // fix correct render ordering in Hexagonal maps when stagger axis == x
                if (_staggerAxis == TMXStaggerAxis_X && _layerOrientation == TMXOrientationHex)
                {
                    if (_staggerIndex == TMXStaggerIndex_Odd)
                    {
                        if (x >= _layerSize.width / 2)
                            newX = (x - std::ceil(_layerSize.width / 2)) * 2 + 1;
                        else
                            newX = x * 2;
                    }
                    else
                    {
                        // TMXStaggerIndex_Even
                        if (x >= static_cast<int>(_layerSize.width / 2))
                            newX = (x - static_cast<int>(_layerSize.width / 2)) * 2;
                        else
                            newX = x * 2 + 1;
                    }
                }

                int pos        = static_cast<int>(newX + _layerSize.width * y);
                uint32_t gid   = _tiles[pos];
                uint32_t flags = 0;

                // issue#1098 TileMap flipped/rotated animation bug.
                flags |= gid & kTMXFlipedAll;
                gid &= ~kTMXFlipedAll;

                // gid are stored in little endian.
                // if host is big endian, then swap
                // if( o == CFByteOrderBigEndian )
                //    gid = CFSwapInt32( gid );
                /* We support little endian.*/

                // FIXME:: gid == 0 --> empty tile
                if (gid != 0)
                {
                    int bi = batchIndexForGID(gid);
                    if (bi >= 0 && _batches[bi].tilesetInfo->_animationInfo.find(gid) !=
                                       _batches[bi].tilesetInfo->_animationInfo.end())
                        _animTileCoord[gid].emplace_back(TMXTileAnimFlag{Vec2(newX, y), flags});
                }
            }
        }

        if (hasTileAnimation())
        {
            _tileAnimManager = new TMXTileAnimManager(this);
        }
    }
}

Mat4 FastTMXLayer::tileToNodeTransform()
{
    float w    = _mapTileSize.width / AX_CONTENT_SCALE_FACTOR();
    float h    = _mapTileSize.height / AX_CONTENT_SCALE_FACTOR();
    float offY = (_layerSize.height - 1) * h;

    switch (_layerOrientation)
    {
    case FAST_TMX_ORIENTATION_ORTHO:
    {
        _tileToNodeTransform =
            Mat4(w, 0.0f, 0.0f, 0.0f, 0.0f, -h, 0.0f, offY, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0, 0.0f, 1.0f);

        return _tileToNodeTransform;
    }
    case FAST_TMX_ORIENTATION_ISO:
    {
        float offX = (_layerSize.width - 1) * w / 2;
        _tileToNodeTransform =
            Mat4(w / 2, -w / 2, 0.0f, offX, -h / 2, -h / 2, 0.0f, offY, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        return _tileToNodeTransform;
    }
    case FAST_TMX_ORIENTATION_HEX:
    {
        _tileToNodeTransform = Mat4(h * sqrtf(0.75), 0, 0, 0, -h / 2, -h, 0, offY, 0, 0, 1, 0, 0, 0, 0, 1);
        return _tileToNodeTransform;
    }
    default:
    {
        _tileToNodeTransform = Mat4::identity;
        return _tileToNodeTransform;
    }
    }
}

void FastTMXLayer::updatePrimitives()
{
    for (auto& batch : _batches)
    {
        for (auto& [_, cmd] : batch.customCommands)
            cmd->setIndexDrawInfo(0, 0);

        if (!batch.texture || !batch.indexBuffer)
            continue;

        auto blendfunc = batch.texture->hasPremultipliedAlpha() ? BlendFunc::ALPHA_PREMULTIPLIED
                                                                : BlendFunc::ALPHA_NON_PREMULTIPLIED;
        for (const auto& [vertexZ, count] : batch.indicesVertexZNumber)
        {
            int start = batch.indicesVertexZOffsets.at(vertexZ);

            auto cmdIt = batch.customCommands.find(vertexZ);
            if (cmdIt == batch.customCommands.end())
            {
                auto* command = new CustomCommand();
                command->setVertexBuffer(_vertexBuffer);

#ifdef AX_FAST_TILEMAP_32_BIT_INDICES
                CustomCommand::IndexFormat indexFormat = CustomCommand::IndexFormat::U_INT;
#else
                CustomCommand::IndexFormat indexFormat = CustomCommand::IndexFormat::U_SHORT;
#endif
                command->setIndexBuffer(batch.indexBuffer, indexFormat);
                command->setIndexDrawInfo(start * 6, count * 6);

                ProgramState* ps{nullptr};
                VertexLayout* vl{nullptr};
                if (_useAutomaticVertexZ)
                {
                    auto* program = axpm->getBuiltinProgram(rhi::ProgramType::POSITION_TEXTURE_COLOR_ALPHA_TEST);
                    ps            = new rhi::ProgramState(program);
                    vl            = program->getVertexLayout();
                    batch.alphaValueLocation = ps->getUniformLocation("u_alpha_value");
                    ps->setUniform(batch.alphaValueLocation, &_alphaFuncValue, sizeof(_alphaFuncValue));
                }
                else
                {
                    auto* program = axpm->getBuiltinProgram(rhi::ProgramType::POSITION_TEXTURE_COLOR);
                    ps            = new rhi::ProgramState(program);
                    vl            = program->getVertexLayout();
                }

                command->setOwnPSVL(ps, vl, RenderCommand::ADOPT_FLAG_PS);

                batch.mvpMatrixLocation = ps->getUniformLocation("u_MVPMatrix");
                batch.textureLocation   = ps->getUniformLocation("u_tex0");
                ps->setTexture(batch.textureLocation, 0, batch.texture->getRHITexture());
                command->init(_globalZOrder, blendfunc);

                batch.customCommands[vertexZ] = command;
            }
            else
            {
                cmdIt->second->setIndexDrawInfo(start * 6, count * 6);
            }
        }
    }
}

void FastTMXLayer::setOpacity(uint8_t opacity)
{
    Node::setOpacity(opacity);
    _quadsDirty = true;
}

void FastTMXLayer::updateTotalQuads()
{
    if (!_quadsDirty)
        return;

    const int tileCount = (int)(_layerSize.width * _layerSize.height);
    _tileToQuadIndex.assign(tileCount, -1);
    _tilesetBatchIndex.assign(tileCount, -1);
    _totalQuads.resize(tileCount);

    for (auto& batch : _batches)
    {
        batch.indicesVertexZOffsets.clear();
    }

    const float csf   = AX_CONTENT_SCALE_FACTOR();
    const uint8_t opa = getDisplayedOpacity();

    // Per-batch constants: precomputed once per tileset, looked up by batch index in the tile loop.
    struct BatchCache
    {
        Vec2 tileSize;
        Vec2 tileOffset;
        Vec2 texSize;
        Color32 color;
    };
    std::vector<BatchCache> batchCache(_batches.size());
    for (int i = 0; i < static_cast<int>(_batches.size()); ++i)
    {
        auto* ts          = _batches[i].tilesetInfo;
        const bool premul = _batches[i].texture && _batches[i].texture->hasPremultipliedAlpha();
        Color32 c         = Color32::white;
        c.a               = opa;
        if (premul)
        {
            const float a = opa / 255.0f;
            c.r           = static_cast<uint8_t>(c.r * a);
            c.g           = static_cast<uint8_t>(c.g * a);
            c.b           = static_cast<uint8_t>(c.b * a);
        }
        batchCache[i] = {AX_SIZE_PIXELS_TO_POINTS(ts->_tileSize),
                         Vec2(ts->_tileOffset.x / csf, -ts->_tileOffset.y / csf), ts->_imageSize, c};
    }

    // Cache the last resolved batch index and its GID range to skip re-scanning for
    // consecutive tiles from the same tileset, which is the common case in clustered maps.
    int cachedBi      = -1;
    uint32_t cachedLo = 0;
    uint32_t cachedHi = 0;

    int quadIndex = 0;
    for (int y = 0; y < _layerSize.height; ++y)
    {
        for (int x = 0; x < _layerSize.width; ++x)
        {
            int tileIndex    = getTileIndexByPos(x, y);
            uint32_t tileGID = _tiles[tileIndex];
            if (tileGID == 0)
                continue;

            const uint32_t bareGID = tileGID & kTMXFlippedMask;
            int bi;
            if (cachedBi >= 0 && bareGID >= cachedLo && bareGID < cachedHi)
            {
                bi = cachedBi;
            }
            else
            {
                bi = batchIndexForGID(tileGID);
                if (bi >= 0)
                {
                    cachedBi = bi;
                    cachedLo = static_cast<uint32_t>(_batches[bi].tilesetInfo->_firstGid);
                    cachedHi = (bi > 0) ? static_cast<uint32_t>(_batches[bi - 1].tilesetInfo->_firstGid) : UINT32_MAX;
                }
            }
            if (bi < 0)
                continue;

            _tileToQuadIndex[tileIndex]   = quadIndex;
            _tilesetBatchIndex[tileIndex] = bi;

            auto& quad     = _totalQuads[quadIndex];
            auto& batch    = _batches[bi];
            const auto& bc = batchCache[bi];

            Vec3 nodePos(float(x), float(y), 0);
            _tileToNodeTransform.transformPoint(&nodePos);

            int zPos    = getVertexZForPos(Vec2((float)x, (float)y));
            float z     = (float)zPos;
            auto& vzMap = batch.indicesVertexZOffsets;
            auto vzIt   = vzMap.find(zPos);
            if (vzIt == vzMap.end())
                vzMap[zPos] = 1;
            else
                vzIt->second++;

            // vertex positions
            float left, right, top, bottom;
            if (tileGID & kTMXTileDiagonalFlag)
            {
                left   = nodePos.x + bc.tileOffset.x;
                right  = nodePos.x + bc.tileSize.height + bc.tileOffset.x;
                bottom = nodePos.y + bc.tileSize.width + bc.tileOffset.y;
                top    = nodePos.y + bc.tileOffset.y;
            }
            else
            {
                left   = nodePos.x + bc.tileOffset.x;
                right  = nodePos.x + bc.tileSize.width + bc.tileOffset.x;
                bottom = nodePos.y + bc.tileSize.height + bc.tileOffset.y;
                top    = nodePos.y + bc.tileOffset.y;
            }
            if (tileGID & kTMXTileVerticalFlag)
                std::swap(top, bottom);
            if (tileGID & kTMXTileHorizontalFlag)
                std::swap(left, right);

            if (tileGID & kTMXTileDiagonalFlag)
            {
                // FIXME: not working correctly
                quad.bl.position = {left, bottom, z};
                quad.br.position = {left, top, z};
                quad.tl.position = {right, bottom, z};
                quad.tr.position = {right, top, z};
            }
            else
            {
                quad.bl.position = {left, bottom, z};
                quad.br.position = {right, bottom, z};
                quad.tl.position = {left, top, z};
                quad.tr.position = {right, top, z};
            }

            // UV coords — only the per-tile rect lookup stays inside the loop.
            Rect tileTexture    = batch.tilesetInfo->getRectForGID(tileGID);
            const float uleft   = tileTexture.origin.x / bc.texSize.width;
            const float uright  = uleft + tileTexture.size.width / bc.texSize.width;
            const float ubottom = tileTexture.origin.y / bc.texSize.height;
            const float utop    = ubottom + tileTexture.size.height / bc.texSize.height;

            quad.bl.texCoord = {uleft, ubottom};
            quad.br.texCoord = {uright, ubottom};
            quad.tl.texCoord = {uleft, utop};
            quad.tr.texCoord = {uright, utop};

            quad.bl.color = quad.br.color = quad.tl.color = quad.tr.color = bc.color;

            ++quadIndex;
        }
    }

    _totalQuads.resize(quadIndex);

    // Convert per-batch vertexZ counts into cumulative start offsets, then size index arrays.
    for (auto& batch : _batches)
    {
        int batchTileCount = 0;
        for (auto& [_, cnt] : batch.indicesVertexZOffsets)
            batchTileCount += cnt;
        batch.indices.resize(6 * batchTileCount);

        int offset = 0;
        for (auto& [_, val] : batch.indicesVertexZOffsets)
        {
            std::swap(offset, val);
            offset += val;
        }
    }

    updateVertexBuffer();
    _quadsDirty = false;
}

// removing / getting tiles
Sprite* FastTMXLayer::getTileAt(const Vec2& tileCoordinate)
{
    AXASSERT(tileCoordinate.x < _layerSize.width && tileCoordinate.y < _layerSize.height && tileCoordinate.x >= 0 &&
                 tileCoordinate.y >= 0,
             "TMXLayer: invalid position");
    AXASSERT(_tiles, "TMXLayer: the tiles map has been released");

    TMXTileFlags flags;
    Sprite* tile = nullptr;
    int gid      = this->getTileGIDAt(tileCoordinate, &flags);

    // if GID == 0, then no tile is present
    if (gid)
    {
        int index = (int)tileCoordinate.x + (int)(tileCoordinate.y * _layerSize.width);

        auto it = _spriteContainer.find(index);
        if (it != _spriteContainer.end())
        {
            tile = it->second.first;
        }
        else
        {
            // tile not created yet. create it
            int bi = batchIndexForGID(gid);
            if (bi < 0)
                return nullptr;
            auto& tileBatch = _batches[bi];
            Rect rect       = tileBatch.tilesetInfo->getRectForGID(gid);
            rect            = AX_RECT_PIXELS_TO_POINTS(rect);
            tile            = Sprite::createWithTexture(tileBatch.texture, rect);

            setupTileSprite(tile, tileCoordinate, gid | flags);

            tile->setTag(index);
            this->addChild(tile, index);

            _spriteContainer.insert(
                std::pair<int, std::pair<Sprite*, uint32_t>>(index, std::pair<Sprite*, uint32_t>(tile, gid | flags)));

            // tile is converted to sprite.
            setFlaggedTileGIDByIndex(index, 0);
        }
    }
    return tile;
}

int FastTMXLayer::getTileGIDAt(const Vec2& tileCoordinate, TMXTileFlags* flags /* = nullptr*/)
{
    AXASSERT(tileCoordinate.x < _layerSize.width && tileCoordinate.y < _layerSize.height && tileCoordinate.x >= 0 &&
                 tileCoordinate.y >= 0,
             "TMXLayer: invalid position");
    AXASSERT(_tiles, "TMXLayer: the tiles map has been released");

    int idx = static_cast<int>(((int)tileCoordinate.x + (int)tileCoordinate.y * _layerSize.width));

    // Bits on the far end of the 32-bit global tile ID are used for tile flags
    int tile = _tiles[idx];
    decltype(_spriteContainer)::iterator it;
    // converted to sprite.
    if (tile == 0 && (it = _spriteContainer.find(idx)) != _spriteContainer.end())
    {
        tile = it->second.second;
    }

    // issue1264, flipped tiles can be changed dynamically
    if (flags)
    {
        *flags = (TMXTileFlags)(tile & kTMXFlipedAll);
    }

    return (tile & kTMXFlippedMask);
}

Vec2 FastTMXLayer::getPositionAt(const Vec2& pos)
{
    return PointApplyTransform(pos, _tileToNodeTransform);
}

int FastTMXLayer::getVertexZForPos(const Vec2& pos)
{
    int ret    = 0;
    int maxVal = 0;
    if (_useAutomaticVertexZ)
    {
        switch (_layerOrientation)
        {
        case FAST_TMX_ORIENTATION_ISO:
            maxVal = static_cast<int>(_layerSize.width + _layerSize.height);
            ret    = static_cast<int>(-(maxVal - (pos.x + pos.y)));
            break;
        case FAST_TMX_ORIENTATION_ORTHO:
            ret = static_cast<int>(-(_layerSize.height - pos.y));
            break;
        case FAST_TMX_ORIENTATION_HEX:
            AXASSERT(0, "TMX Hexa vertexZ not supported");
            break;
        default:
            AXASSERT(0, "TMX invalid value");
            break;
        }
    }
    else
    {
        ret = _vertexZvalue;
    }

    return ret;
}

void FastTMXLayer::removeTileAt(const Vec2& tileCoordinate)
{

    AXASSERT(tileCoordinate.x < _layerSize.width && tileCoordinate.y < _layerSize.height && tileCoordinate.x >= 0 &&
                 tileCoordinate.y >= 0,
             "TMXLayer: invalid position");

    int gid = this->getTileGIDAt(tileCoordinate);

    if (gid)
    {

        int z = (int)tileCoordinate.x + (int)(tileCoordinate.y * _layerSize.width);

        // remove tile from GID map
        setFlaggedTileGIDByIndex(z, 0);

        // remove it from sprites
        auto it = _spriteContainer.find(z);
        if (it != _spriteContainer.end())
        {
            this->removeChild(it->second.first);
        }
    }
}

void FastTMXLayer::setFlaggedTileGIDByIndex(int index, uint32_t gid)
{
    if (gid == _tiles[index])
        return;
    _tiles[index] = gid;
    _quadsDirty   = true;
    _dirty        = true;
}

void FastTMXLayer::removeChild(Node* node, bool cleanup)
{
    int tag = node->getTag();
    auto it = _spriteContainer.find(tag);
    if (it != _spriteContainer.end() && it->second.first == node)
    {
        _spriteContainer.erase(it);
    }
    Node::removeChild(node, cleanup);
}

// TMXLayer - Properties
Value FastTMXLayer::getProperty(std::string_view propertyName) const
{
    auto propItr = _properties.find(propertyName);
    if (propItr != _properties.end())
        return propItr->second;

    return Value();
}

void FastTMXLayer::parseInternalProperties()
{
    auto vertexz = getProperty("cc_vertexz");
    if (vertexz.isNull())
        return;

    std::string vertexZStr = vertexz.asString();
    // If "automatic" is on, then parse the "cc_alpha_func" too
    if (vertexZStr == "automatic")
    {
        _useAutomaticVertexZ = true;
        auto alphaFuncVal    = getProperty("cc_alpha_func");
        _alphaFuncValue      = alphaFuncVal.asFloat();
    }
    else
    {
        _vertexZvalue = vertexz.asInt();
    }
}

// CCTMXLayer2 - obtaining positions, offset
Vec2 FastTMXLayer::calculateLayerOffset(const Vec2& pos)
{
    Vec2 ret;
    switch (_layerOrientation)
    {
    case FAST_TMX_ORIENTATION_ORTHO:
        ret.set(pos.x * _mapTileSize.width, -pos.y * _mapTileSize.height);
        break;
    case FAST_TMX_ORIENTATION_ISO:
        ret.set((_mapTileSize.width / 2) * (pos.x - pos.y), (_mapTileSize.height / 2) * (-pos.x - pos.y));
        break;
    case FAST_TMX_ORIENTATION_HEX:
    default:
        AXASSERT(pos.isZero(), "offset for this map not implemented yet");
        break;
    }
    return ret;
}

// TMXLayer - adding / remove tiles
void FastTMXLayer::setTileGID(int gid, const Vec2& tileCoordinate)
{
    setTileGID(gid, tileCoordinate, (TMXTileFlags)0);
}

void FastTMXLayer::setTileGID(int gid, const Vec2& tileCoordinate, TMXTileFlags flags)
{
    AXASSERT(tileCoordinate.x < _layerSize.width && tileCoordinate.y < _layerSize.height && tileCoordinate.x >= 0 &&
                 tileCoordinate.y >= 0,
             "TMXLayer: invalid position");
    AXASSERT(_tiles, "TMXLayer: the tiles map has been released");
    AXASSERT(gid == 0 || batchIndexForGID(gid) >= 0, "TMXLayer: invalid gid");

    TMXTileFlags currentFlags;
    int currentGID = getTileGIDAt(tileCoordinate, &currentFlags);

    if (currentGID == gid && currentFlags == flags)
        return;

    const uint32_t gidAndFlags = gid | flags;

    // setting gid=0 is equal to remove the tile
    if (gid == 0)
    {
        removeTileAt(tileCoordinate);
    }
    // empty tile. create a new one
    else if (currentGID == 0)
    {
        int z = (int)tileCoordinate.x + (int)(tileCoordinate.y * _layerSize.width);
        setFlaggedTileGIDByIndex(z, gidAndFlags);
    }
    // modifying an existing tile with a non-empty tile
    else
    {
        int z   = (int)tileCoordinate.x + (int)(tileCoordinate.y * _layerSize.width);
        auto it = _spriteContainer.find(z);
        if (it != _spriteContainer.end())
        {
            Sprite* sprite = it->second.first;
            int bi         = batchIndexForGID(gid);
            Rect rect      = (bi >= 0) ? _batches[bi].tilesetInfo->getRectForGID(gid) : Rect::zero;
            rect           = AX_RECT_PIXELS_TO_POINTS(rect);
            if (bi >= 0 && _batches[bi].texture)
                sprite->setTexture(_batches[bi].texture);

            sprite->setTextureRect(rect, false, rect.size);
            this->reorderChild(sprite, z);
            if (flags)
            {
                setupTileSprite(sprite, tileCoordinate, gidAndFlags);
            }

            it->second.second = gidAndFlags;
        }
        else
        {
            setFlaggedTileGIDByIndex(z, gidAndFlags);
        }
    }
}

void FastTMXLayer::setupTileSprite(Sprite* sprite, const Vec2& pos, uint32_t gid)
{
    auto tempPosAt             = getPositionAt(pos);
    auto tempSpriteContentSize = sprite->getContentSize();

    sprite->setPositionZ((float)getVertexZForPos(pos));
    sprite->setOpacity(this->getOpacity());

    // fix issue #1283 too;  put the anchor in the middle for ease of rotation.
    sprite->setAnchorPoint(Vec2(0.5f, 0.5f));
    int bi                        = batchIndexForGID(gid);
    const Vec2 tileOff            = (bi >= 0) ? _batches[bi].tilesetInfo->_tileOffset : Vec2::zero;
    const float spriteTileOffsetX = tileOff.x / AX_CONTENT_SCALE_FACTOR();
    const float spriteTileOffsetY = -tileOff.y / AX_CONTENT_SCALE_FACTOR();
    sprite->setPosition(tempPosAt.x + std::roundf(tempSpriteContentSize.height / 2) + spriteTileOffsetX,
                        tempPosAt.y + std::roundf(tempSpriteContentSize.width / 2) + spriteTileOffsetY);

    // issue 1264, flip can be undone as well
    sprite->setFlippedX(false);
    sprite->setFlippedY(false);
    sprite->setRotation(0.0f);

    // Rotation in tiled is achieved using 3 flipped states, flipping across the horizontal, vertical, and diagonal axes
    // of the tiles.
    if (gid & kTMXTileDiagonalFlag)
    {
        uint32_t flag = gid & (kTMXTileHorizontalFlag | kTMXTileVerticalFlag);

        // handle the 4 diagonally flipped states.
        if (flag == kTMXTileHorizontalFlag)
        {
            sprite->setRotation(90.0f);
        }
        else if (flag == kTMXTileVerticalFlag)
        {
            sprite->setRotation(270.0f);
        }
        else if (flag == (kTMXTileVerticalFlag | kTMXTileHorizontalFlag))
        {
            sprite->setRotation(90.0f);
            sprite->setFlippedX(true);
        }
        else
        {
            sprite->setRotation(270.0f);
            sprite->setFlippedX(true);
        }
    }
    else
    {
        if (gid & kTMXTileHorizontalFlag)
        {
            sprite->setFlippedX(true);
        }

        if (gid & kTMXTileVerticalFlag)
        {
            sprite->setFlippedY(true);
        }
    }
}

std::string FastTMXLayer::getDescription() const
{
    return fmt::format("<FastTMXLayer | tag = {}, size = {},{}>", _tag, (int)_mapTileSize.width,
                       (int)_mapTileSize.height);
}

TMXTileAnimManager::TMXTileAnimManager(FastTMXLayer* layer)
{
    _layer = layer;
    for (const auto& p : *_layer->getAnimTileCoord())
    {
        int bi = _layer->batchIndexForGID(p.first);
        if (bi < 0)
            continue;
        TMXTileAnimInfo* animInfo = _layer->getBatches()[bi].tilesetInfo->_animationInfo.at(p.first);
        for (auto&& tile : p.second)
        {
            _tasks.pushBack(TMXTileAnimTask::create(_layer, animInfo, tile._tilePos, tile._flag));
        }
    }
}

TMXTileAnimManager* TMXTileAnimManager::create(FastTMXLayer* layer)
{
    TMXTileAnimManager* ret = new TMXTileAnimManager(layer);
    ret->autorelease();
    return ret;
}

void TMXTileAnimManager::startAll()
{
    if (_started || _tasks.empty())
        return;
    _started = true;
    for (auto&& task : _tasks)
    {
        task->start();
    }
}

void TMXTileAnimManager::stopAll()
{
    if (!_started)
        return;
    _started = false;
    for (auto&& task : _tasks)
    {
        task->stop();
    }
}

TMXTileAnimTask::TMXTileAnimTask(FastTMXLayer* layer, TMXTileAnimInfo* animation, const Vec2& tilePos, uint32_t flag)
{
    _layer        = layer;
    _animation    = animation;
    _frameCount   = static_cast<uint32_t>(_animation->_frames.size());
    _tilePosition = tilePos;
    _flag         = flag;
    _key          = fmt::format("TickAnimOnTilePos({},{})", _tilePosition.x, _tilePosition.y);
}

void TMXTileAnimTask::tickAndScheduleNext(float dt)
{
    setCurrFrame();
    auto* parent = _layer->getParent();
    if (!parent)
        return;
    parent->scheduleOnce(AX_CALLBACK_1(TMXTileAnimTask::tickAndScheduleNext, this),
                         _animation->_frames[_currentFrame]._duration / 1000.0f, _key);
}

void TMXTileAnimTask::start()
{
    _isRunning = true;
    tickAndScheduleNext(0.0f);
}

void TMXTileAnimTask::stop()
{
    _isRunning   = false;
    auto* parent = _layer->getParent();
    if (parent)
        parent->unschedule(_key);
}

void TMXTileAnimTask::setCurrFrame()
{
    _currentFrame = _nextFrame;
    _layer->setTileGID((int)_animation->_frames[_currentFrame]._tileID, _tilePosition, (TMXTileFlags)_flag);
    _nextFrame = (_currentFrame + 1) % _frameCount;
}

TMXTileAnimTask* TMXTileAnimTask::create(FastTMXLayer* layer,
                                         TMXTileAnimInfo* animation,
                                         const Vec2& tilePos,
                                         uint32_t flag)
{
    TMXTileAnimTask* ret = new TMXTileAnimTask(layer, animation, tilePos, flag);
    ret->autorelease();
    return ret;
}

}  // namespace ax
