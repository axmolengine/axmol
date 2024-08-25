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
#include "2d/FastTMXLayer.h"
#include <stddef.h>  // offsetof
#include "base/Types.h"
#include "2d/FastTMXTiledMap.h"
#include "2d/Sprite.h"
#include "2d/Camera.h"
#include "renderer/TextureCache.h"
#include "renderer/Renderer.h"
#include "renderer/Shaders.h"
#include "renderer/backend/DriverBase.h"
#include "renderer/backend/Buffer.h"
#include "base/Director.h"
#include "base/UTF8.h"
#include "renderer/backend/ProgramState.h"

namespace ax
{

const int FastTMXLayer::FAST_TMX_ORIENTATION_ORTHO = 0;
const int FastTMXLayer::FAST_TMX_ORIENTATION_HEX   = 1;
const int FastTMXLayer::FAST_TMX_ORIENTATION_ISO   = 2;

// FastTMXLayer - init & alloc & dealloc
FastTMXLayer* FastTMXLayer::create(TMXTilesetInfo* tilesetInfo, TMXLayerInfo* layerInfo, TMXMapInfo* mapInfo)
{
    FastTMXLayer* ret = new FastTMXLayer();
    if (ret->initWithTilesetInfo(tilesetInfo, layerInfo, mapInfo))
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
        _texture = _director->getTextureCache()->addImage(tilesetInfo->_sourceImage);
        _texture->retain();
    }

    // layerInfo
    _layerName  = layerInfo->_name;
    _layerSize  = layerInfo->_layerSize;
    _tiles      = layerInfo->_tiles;
    _quadsDirty = true;
    setOpacity(layerInfo->_opacity);
    setProperties(layerInfo->getProperties());

    // tilesetInfo
    _tileSet = tilesetInfo;
    AX_SAFE_RETAIN(_tileSet);

    // mapInfo
    _mapTileSize      = mapInfo->getTileSize();
    _layerOrientation = mapInfo->getOrientation();
    _staggerAxis      = mapInfo->getStaggerAxis();
    _staggerIndex     = mapInfo->getStaggerIndex();

    // offset (after layer orientation is set);
    Vec2 offset = this->calculateLayerOffset(layerInfo->_offset);
    this->setPosition(AX_POINT_PIXELS_TO_POINTS(offset));

    this->setContentSize(
        AX_SIZE_PIXELS_TO_POINTS(Vec2(_layerSize.width * _mapTileSize.width, _layerSize.height * _mapTileSize.height)));

    this->tileToNodeTransform();

    _useAutomaticVertexZ = false;
    _vertexZvalue        = 0;

    return true;
}

FastTMXLayer::FastTMXLayer() {}

FastTMXLayer::~FastTMXLayer()
{
    AX_SAFE_RELEASE(_tileSet);
    AX_SAFE_RELEASE(_texture);
    AX_SAFE_FREE(_tiles);
    AX_SAFE_RELEASE(_vertexBuffer);
    AX_SAFE_RELEASE(_indexBuffer);

    for (auto&& e : _customCommands)
    {
        AX_SAFE_RELEASE(e.second->getPipelineDescriptor().programState);
        delete e.second;
    }
}

void FastTMXLayer::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    updateTotalQuads();

    auto cam = Camera::getVisitingCamera();
    if (flags != 0 || _dirty || _quadsDirty || !_cameraPositionDirty.fuzzyEquals(cam->getPosition(), _tileSet->_tileSize.x) ||
        _cameraZoomDirty != cam->getZoom())
    {
        _cameraPositionDirty = cam->getPosition();
        auto zoom            = _cameraZoomDirty = cam->getZoom();
        Vec2 s               = _director->getVisibleSize();
        const Vec2& anchor   = getAnchorPoint();
        auto rect            = Rect(cam->getPositionX() - s.width * zoom * (anchor.x == 0.0f ? 0.5f : anchor.x),
                                    cam->getPositionY() - s.height * zoom * (anchor.y == 0.0f ? 0.5f : anchor.y),
                                    s.width * zoom, s.height * zoom);

        rect.origin.x -= _tileSet->_tileSize.x;
        rect.origin.y -= _tileSet->_tileSize.y;
        rect.size.x += s.x * zoom / 2 + _tileSet->_tileSize.x * zoom;
        rect.size.y += s.y * zoom / 2 + _tileSet->_tileSize.y * zoom;

        Mat4 inv = transform;
        inv.inverse();
        rect = RectApplyTransform(rect, inv);

        updateTiles(rect);
        updateIndexBuffer();
        updatePrimitives();
        _dirty = false;
    }

    const auto& projectionMat = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    Mat4 finalMat             = projectionMat * _modelViewTransform;
    for (const auto& e : _customCommands)
    {
        if (e.second->getIndexDrawCount() > 0)
        {
            auto mvpmatrixLocation = e.second->getPipelineDescriptor().programState->getUniformLocation("u_MVPMatrix");
            e.second->getPipelineDescriptor().programState->setUniform(mvpmatrixLocation, finalMat.m,
                                                                       sizeof(finalMat.m));
            renderer->addCommand(e.second);
        }
    }
}

void FastTMXLayer::updateTiles(const Rect& culledRect)
{
    Rect visibleTiles        = Rect(culledRect.origin, culledRect.size * _director->getContentScaleFactor());
    Vec2 mapTileSize         = AX_SIZE_PIXELS_TO_POINTS(_mapTileSize);
    Vec2 tileSize            = AX_SIZE_PIXELS_TO_POINTS(_tileSet->_tileSize);
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

    _indicesVertexZNumber.clear();

    for (const auto& iter : _indicesVertexZOffsets)
    {
        _indicesVertexZNumber[iter.first] = iter.second;
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

            int vertexZ = getVertexZForPos(Vec2((float)x, (float)y));
            auto iter   = _indicesVertexZNumber.find(vertexZ);
            int offset  = iter->second;
            iter->second++;

            auto quadIndex = static_cast<decltype(_indices)::value_type>(_tileToQuadIndex[tileIndex]);
            _indices[6 * offset + 0] = quadIndex * 4 + 0;
            _indices[6 * offset + 1] = quadIndex * 4 + 1;
            _indices[6 * offset + 2] = quadIndex * 4 + 2;
            _indices[6 * offset + 3] = quadIndex * 4 + 3;
            _indices[6 * offset + 4] = quadIndex * 4 + 2;
            _indices[6 * offset + 5] = quadIndex * 4 + 1;

        }  // for x
    }      // for y

    for (const auto& iter : _indicesVertexZOffsets)
    {
        _indicesVertexZNumber[iter.first] -= iter.second;
        if (_indicesVertexZNumber[iter.first] == 0)
        {
            _indicesVertexZNumber.erase(iter.first);
        }
    }
}

void FastTMXLayer::updateVertexBuffer()
{
    unsigned int vertexBufferSize = (unsigned int)(sizeof(V3F_C4B_T2F) * _totalQuads.size() * 4);
    if (!_vertexBuffer)
    {
        _vertexBuffer = backend::DriverBase::getInstance()->newBuffer(vertexBufferSize, backend::BufferType::VERTEX, backend::BufferUsage::STATIC);
    }
    _vertexBuffer->updateData(&_totalQuads[0], vertexBufferSize);
}

void FastTMXLayer::updateIndexBuffer()
{
    auto indexBufferSize = (sizeof(decltype(_indices)::value_type) * _indices.size());
    if (!_indexBuffer)
    {
        _indexBuffer = backend::DriverBase::getInstance()->newBuffer(indexBufferSize, backend::BufferType::INDEX, backend::BufferUsage::DYNAMIC);
    }
    _indexBuffer->updateData(&_indices[0], indexBufferSize);
}

// FastTMXLayer - setup Tiles
void FastTMXLayer::setupTiles()
{
    // Optimization: quick hack that sets the image size on the tileset
    _tileSet->_imageSize = _texture->getContentSizeInPixels();

    // By default all the tiles are aliased
    // pros: easier to render
    // cons: difficult to scale / rotate / etc.
    _texture->setAliasTexParameters();

    // CFByteOrder o = CFByteOrderGetCurrent();

    // Parse cocos2d properties
    this->parseInternalProperties();

    auto& screenSize = _director->getWinSize();

    switch (_layerOrientation)
    {
    case FAST_TMX_ORIENTATION_ORTHO:
        _screenGridSize.width  = ceil(screenSize.width / _mapTileSize.width) + 1;
        _screenGridSize.height = ceil(screenSize.height / _mapTileSize.height) + 1;

        // tiles could be bigger than the grid, add additional rows if needed
        _screenGridSize.height += _tileSet->_tileSize.height / _mapTileSize.height;
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

    if (!_tileSet->_animationInfo.empty())
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

                int pos = static_cast<int>(newX + _layerSize.width * y);
                uint32_t gid = _tiles[pos];
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
                    if (_tileSet->_animationInfo.find(gid) != _tileSet->_animationInfo.end())
                    {
                        _animTileCoord[gid].emplace_back(TMXTileAnimFlag{Vec2(newX, y), flags});
                    }
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
        _tileToNodeTransform = Mat4::IDENTITY;
        return _tileToNodeTransform;
    }
    }
}

void FastTMXLayer::updatePrimitives()
{
    auto blendfunc =
        _texture->hasPremultipliedAlpha() ? BlendFunc::ALPHA_PREMULTIPLIED : BlendFunc::ALPHA_NON_PREMULTIPLIED;
    for (const auto& iter : _indicesVertexZNumber)
    {
        int start = _indicesVertexZOffsets.at(iter.first);

        auto commandIter = _customCommands.find(iter.first);
        if (_customCommands.end() == commandIter)
        {
            auto command = new CustomCommand();
            command->setVertexBuffer(_vertexBuffer);

#ifdef AX_FAST_TILEMAP_32_BIT_INDICES
            CustomCommand::IndexFormat indexFormat = CustomCommand::IndexFormat::U_INT;
#else
            CustomCommand::IndexFormat indexFormat = CustomCommand::IndexFormat::U_SHORT;
#endif
            command->setIndexBuffer(_indexBuffer, indexFormat);

            command->setIndexDrawInfo(start * 6, iter.second * 6);

            auto& pipelineDescriptor = command->getPipelineDescriptor();

            if (_useAutomaticVertexZ)
            {
                AX_SAFE_RELEASE(pipelineDescriptor.programState);
                auto* program =
                    backend::Program::getBuiltinProgram(backend::ProgramType::POSITION_TEXTURE_COLOR_ALPHA_TEST);
                auto programState               = new backend::ProgramState(program);
                pipelineDescriptor.programState = programState;
                _alphaValueLocation             = pipelineDescriptor.programState->getUniformLocation("u_alpha_value");
                pipelineDescriptor.programState->setUniform(_alphaValueLocation, &_alphaFuncValue,
                                                            sizeof(_alphaFuncValue));
            }
            else
            {
                AX_SAFE_RELEASE(pipelineDescriptor.programState);
                auto* program     = backend::Program::getBuiltinProgram(backend::ProgramType::POSITION_TEXTURE_COLOR);
                auto programState = new backend::ProgramState(program);
                pipelineDescriptor.programState = programState;
            }

            _mvpMatrixLocaiton = pipelineDescriptor.programState->getUniformLocation("u_MVPMatrix");
            _textureLocation   = pipelineDescriptor.programState->getUniformLocation("u_tex0");
            pipelineDescriptor.programState->setTexture(_textureLocation, 0, _texture->getBackendTexture());
            command->init(_globalZOrder, blendfunc);

            _customCommands[iter.first] = command;
        }
        else
        {
            commandIter->second->setIndexDrawInfo(start * 6, iter.second * 6);
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
    if (_quadsDirty)
    {
        Vec2 tileSize = AX_SIZE_PIXELS_TO_POINTS(_tileSet->_tileSize);
        Vec2 texSize  = _tileSet->_imageSize;
        _tileToQuadIndex.clear();
        _totalQuads.resize(int(_layerSize.width * _layerSize.height));
        _indices.resize(6 * int(_layerSize.width * _layerSize.height));
        _tileToQuadIndex.resize(int(_layerSize.width * _layerSize.height), -1);
        _indicesVertexZOffsets.clear();

        auto color = Color4B::WHITE;
        color.a    = getDisplayedOpacity();

        if (_texture->hasPremultipliedAlpha())
        {
            auto alpha = color.a / 255.0f;
            color.r    = static_cast<uint8_t>(color.r * alpha);
            color.g    = static_cast<uint8_t>(color.g * alpha);
            color.b    = static_cast<uint8_t>(color.b * alpha);
        }

        int quadIndex = 0;
        for (int y = 0; y < _layerSize.height; ++y)
        {
            for (int x = 0; x < _layerSize.width; ++x)
            {
                int tileIndex = getTileIndexByPos(x, y);
                int tileGID   = _tiles[tileIndex];

                if (tileGID == 0)
                    continue;

                _tileToQuadIndex[tileIndex] = quadIndex;

                auto& quad = _totalQuads[quadIndex];

                Vec3 nodePos(float(x), float(y), 0);
                _tileToNodeTransform.transformPoint(&nodePos);

                float left, right, top, bottom, z;

                int zPos  = getVertexZForPos(Vec2((float)x, (float)y));
                z         = (float)zPos;
                auto iter = _indicesVertexZOffsets.find(zPos);
                if (iter == _indicesVertexZOffsets.end())
                {
                    _indicesVertexZOffsets[zPos] = 1;
                }
                else
                {
                    iter->second++;
                }
                // vertices
                if (tileGID & kTMXTileDiagonalFlag)
                {
                    left   = nodePos.x;
                    right  = nodePos.x + tileSize.height;
                    bottom = nodePos.y + tileSize.width;
                    top    = nodePos.y;
                }
                else
                {
                    left   = nodePos.x;
                    right  = nodePos.x + tileSize.width;
                    bottom = nodePos.y + tileSize.height;
                    top    = nodePos.y;
                }

                if (tileGID & kTMXTileVerticalFlag)
                    std::swap(top, bottom);
                if (tileGID & kTMXTileHorizontalFlag)
                    std::swap(left, right);

                if (tileGID & kTMXTileDiagonalFlag)
                {
                    // FIXME: not working correctly
                    quad.bl.vertices.x = left;
                    quad.bl.vertices.y = bottom;
                    quad.bl.vertices.z = z;
                    quad.br.vertices.x = left;
                    quad.br.vertices.y = top;
                    quad.br.vertices.z = z;
                    quad.tl.vertices.x = right;
                    quad.tl.vertices.y = bottom;
                    quad.tl.vertices.z = z;
                    quad.tr.vertices.x = right;
                    quad.tr.vertices.y = top;
                    quad.tr.vertices.z = z;
                }
                else
                {
                    quad.bl.vertices.x = left;
                    quad.bl.vertices.y = bottom;
                    quad.bl.vertices.z = z;
                    quad.br.vertices.x = right;
                    quad.br.vertices.y = bottom;
                    quad.br.vertices.z = z;
                    quad.tl.vertices.x = left;
                    quad.tl.vertices.y = top;
                    quad.tl.vertices.z = z;
                    quad.tr.vertices.x = right;
                    quad.tr.vertices.y = top;
                    quad.tr.vertices.z = z;
                }

                // texcoords
                Rect tileTexture = _tileSet->getRectForGID(tileGID);
                left             = (tileTexture.origin.x / texSize.width);
                right            = left + (tileTexture.size.width / texSize.width);
                bottom           = (tileTexture.origin.y / texSize.height);
                top              = bottom + (tileTexture.size.height / texSize.height);

                // issue#1085 OpenGL sub-pixel horizontal-vertical lines pixel-tolerance fix.
                float ptx = 1.0 / (_tileSet->_imageSize.x * tileSize.x);
                float pty = 1.0 / (_tileSet->_imageSize.y * tileSize.y);

                quad.bl.texCoords.u = left + ptx;
                quad.bl.texCoords.v = bottom + pty;
                quad.br.texCoords.u = right - ptx;
                quad.br.texCoords.v = bottom + pty;
                quad.tl.texCoords.u = left + ptx;
                quad.tl.texCoords.v = top - pty;
                quad.tr.texCoords.u = right - ptx;
                quad.tr.texCoords.v = top - pty;

                quad.bl.colors = color;
                quad.br.colors = color;
                quad.tl.colors = color;
                quad.tr.colors = color;

                ++quadIndex;
            }
        }

        int offset = 0;
        for (auto&& vertexZOffset : _indicesVertexZOffsets)
        {
            std::swap(offset, vertexZOffset.second);
            offset += vertexZOffset.second;
        }
        updateVertexBuffer();

        _quadsDirty = false;
    }
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
            Rect rect = _tileSet->getRectForGID(gid);
            rect      = AX_RECT_PIXELS_TO_POINTS(rect);
            tile      = Sprite::createWithTexture(_texture, rect);

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
    auto it  = _spriteContainer.find(idx);

    // converted to sprite.
    if (tile == 0 && it != _spriteContainer.end())
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
    AXASSERT(gid == 0 || gid >= _tileSet->_firstGid, "TMXLayer: invalid gid");

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
            Rect rect      = _tileSet->getRectForGID(gid);
            rect           = AX_RECT_PIXELS_TO_POINTS(rect);

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
    auto tempPosAt = getPositionAt(pos);
    auto tempSpriteContentSize = sprite->getContentSize();

    sprite->setPositionZ((float)getVertexZForPos(pos));
    sprite->setOpacity(this->getOpacity());

    // fix issue #1283 too;  put the anchor in the middle for ease of rotation.
    sprite->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprite->setPosition(tempPosAt.x + std::roundf(tempSpriteContentSize.height / 2),
                        tempPosAt.y + std::roundf(tempSpriteContentSize.width / 2));

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
        for (auto&& tile : p.second)
        {
            _tasks.pushBack(TMXTileAnimTask::create(_layer, _layer->getTileSet()->_animationInfo.at(p.first), tile._tilePos, tile._flag));
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
    std::stringstream ss;
    ss << "TickAnimOnTilePos(" << _tilePosition.x << "," << _tilePosition.y << ")";
    _key = ss.str();
}

void TMXTileAnimTask::tickAndScheduleNext(float dt)
{
    setCurrFrame();
    _layer->getParent()->scheduleOnce(AX_CALLBACK_1(TMXTileAnimTask::tickAndScheduleNext, this),
                                      _animation->_frames[_currentFrame]._duration / 1000.0f, _key);
}

void TMXTileAnimTask::start()
{
    _isRunning = true;
    tickAndScheduleNext(0.0f);
}

void TMXTileAnimTask::stop()
{
    _isRunning = false;
    _layer->getParent()->unschedule(_key);
}

void TMXTileAnimTask::setCurrFrame()
{
    _layer->setTileGID(_animation->_frames[_currentFrame]._tileID, _tilePosition, (TMXTileFlags)_flag);
    _currentFrame = (_currentFrame + 1) % _frameCount;
}

TMXTileAnimTask* TMXTileAnimTask::create(FastTMXLayer* layer, TMXTileAnimInfo* animation, const Vec2& tilePos, uint32_t flag)
{
    TMXTileAnimTask* ret = new TMXTileAnimTask(layer, animation, tilePos, flag);
    ret->autorelease();
    return ret;
}

}
