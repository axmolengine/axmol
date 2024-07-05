/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2009      Leonardo Kasperavičius
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
#include "2d/ParticleSystemQuad.h"
#include <algorithm>
#include <stddef.h>  // offsetof
#include "base/Types.h"
#include "2d/SpriteFrame.h"
#include "2d/ParticleBatchNode.h"
#include "renderer/TextureAtlas.h"
#include "renderer/Renderer.h"
#include "base/Director.h"
#include "base/EventType.h"
#include "base/Configuration.h"
#include "base/EventListenerCustom.h"
#include "base/EventDispatcher.h"
#include "base/UTF8.h"
#include "renderer/Shaders.h"
#include "renderer/backend/ProgramState.h"
#include "2d/TweenFunction.h"

NS_AX_BEGIN

ParticleSystemQuad::ParticleSystemQuad()
{
    auto& pipelinePS = _quadCommand.getPipelineDescriptor().programState;
    auto* program    = backend::Program::getBuiltinProgram(backend::ProgramType::POSITION_TEXTURE_COLOR);

    //!!! support etc1 with alpha?
    pipelinePS = (new backend::ProgramState(program));

    _mvpMatrixLocaiton = pipelinePS->getUniformLocation("u_MVPMatrix");
    _textureLocation   = pipelinePS->getUniformLocation("u_tex0");
}

ParticleSystemQuad::~ParticleSystemQuad()
{
    if (nullptr == _batchNode)
    {
        AX_SAFE_FREE(_quads);
        AX_SAFE_FREE(_indices);
    }

    AX_SAFE_RELEASE_NULL(_quadCommand.getPipelineDescriptor().programState);
}

// implementation ParticleSystemQuad

ParticleSystemQuad* ParticleSystemQuad::create(std::string_view filename)
{
    ParticleSystemQuad* ret = new ParticleSystemQuad();
    if (ret->initWithFile(filename))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return ret;
}

ParticleSystemQuad* ParticleSystemQuad::createWithTotalParticles(int numberOfParticles)
{
    AXASSERT(numberOfParticles <= 10000,
             "Adding more than 10000 particles will crash the renderer, the mesh generated has an index format of "
             "U_SHORT (uint16_t)");

    ParticleSystemQuad* ret = new ParticleSystemQuad();
    if (ret->initWithTotalParticles(numberOfParticles))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return ret;
}

ParticleSystemQuad* ParticleSystemQuad::create(ValueMap& dictionary)
{
    ParticleSystemQuad* ret = new ParticleSystemQuad();
    if (ret->initWithDictionary(dictionary))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return ret;
}

// implementation ParticleSystemQuad
//  overriding the init method
bool ParticleSystemQuad::initWithTotalParticles(int numberOfParticles)
{
    // base initialization
    if (ParticleSystem::initWithTotalParticles(numberOfParticles))
    {
        // allocating data space
        if (!this->allocMemory())
        {
            this->release();
            return false;
        }

        initIndices();
        //        setupVBO();

#if AX_ENABLE_CACHE_TEXTURE_DATA
        // Need to listen the event only when not use batchnode, because it will use VBO
        auto listener = EventListenerCustom::create(EVENT_RENDERER_RECREATED,
                                                    AX_CALLBACK_1(ParticleSystemQuad::listenRendererRecreated, this));
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
#endif

        return true;
    }
    return false;
}

// pointRect should be in Texture coordinates, not pixel coordinates
void ParticleSystemQuad::initTexCoordsWithRect(const Rect& pointRect)
{
    // convert to Tex coords

    Rect rect =
        Rect(pointRect.origin.x * AX_CONTENT_SCALE_FACTOR(), pointRect.origin.y * AX_CONTENT_SCALE_FACTOR(),
             pointRect.size.width * AX_CONTENT_SCALE_FACTOR(), pointRect.size.height * AX_CONTENT_SCALE_FACTOR());

    float wide = (float)pointRect.size.width;
    float high = (float)pointRect.size.height;

    if (_texture)
    {
        wide = (float)_texture->getPixelsWide();
        high = (float)_texture->getPixelsHigh();
    }

#if AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL
    float left   = (rect.origin.x * 2 + 1) / (wide * 2);
    float bottom = (rect.origin.y * 2 + 1) / (high * 2);
    float right  = left + (rect.size.width * 2 - 2) / (wide * 2);
    float top    = bottom + (rect.size.height * 2 - 2) / (high * 2);
#else
    float left   = rect.origin.x / wide;
    float bottom = rect.origin.y / high;
    float right  = left + rect.size.width / wide;
    float top    = bottom + rect.size.height / high;
#endif  // ! AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL

    // Important. Texture in cocos2d are inverted, so the Y component should be inverted
    std::swap(top, bottom);

    V3F_C4B_T2F_Quad* quads = nullptr;
    unsigned int start = 0, end = 0;
    if (_batchNode)
    {
        quads = _batchNode->getTextureAtlas()->getQuads();
        start = _atlasIndex;
        end   = _atlasIndex + _totalParticles;
    }
    else
    {
        quads = _quads;
        start = 0;
        end   = _totalParticles;
    }

    for (unsigned int i = start; i < end; i++)
    {
        // bottom-left vertex:
        quads[i].bl.texCoords.u = left;
        quads[i].bl.texCoords.v = bottom;
        // bottom-right vertex:
        quads[i].br.texCoords.u = right;
        quads[i].br.texCoords.v = bottom;
        // top-left vertex:
        quads[i].tl.texCoords.u = left;
        quads[i].tl.texCoords.v = top;
        // top-right vertex:
        quads[i].tr.texCoords.u = right;
        quads[i].tr.texCoords.v = top;
    }
}

void ParticleSystemQuad::updateTexCoords()
{
    if (_texture)
    {
        const Vec2& s = _texture->getContentSize();
        initTexCoordsWithRect(Rect(0, 0, s.width, s.height));
    }
}

void ParticleSystemQuad::setTextureWithRect(Texture2D* texture, const Rect& rect)
{
    // Only update the texture if is different from the current one
    if (!_texture || texture->getBackendTexture() != _texture->getBackendTexture())
    {
        ParticleSystem::setTexture(texture);

        auto programState = _quadCommand.getPipelineDescriptor().programState;
        programState->setTexture(_texture->getBackendTexture());
    }

    this->initTexCoordsWithRect(rect);
}

void ParticleSystemQuad::setTexture(Texture2D* texture)
{
    const Vec2& s = texture->getContentSize();
    this->setTextureWithRect(texture, Rect(0, 0, s.width, s.height));
}

void ParticleSystemQuad::setDisplayFrame(SpriteFrame* spriteFrame)
{
    AXASSERT(spriteFrame->getOffsetInPixels().isZero(), "QuadParticle only supports SpriteFrames with no offsets");

    this->setTextureWithRect(spriteFrame->getTexture(), spriteFrame->getRect());
}

void ParticleSystemQuad::initIndices()
{
    for (int i = 0; i < _totalParticles; ++i)
    {
        const unsigned int i6 = i * 6;
        const unsigned int i4 = i * 4;
        _indices[i6 + 0]      = (unsigned short)i4 + 0;
        _indices[i6 + 1]      = (unsigned short)i4 + 1;
        _indices[i6 + 2]      = (unsigned short)i4 + 2;

        _indices[i6 + 5] = (unsigned short)i4 + 1;
        _indices[i6 + 4] = (unsigned short)i4 + 2;
        _indices[i6 + 3] = (unsigned short)i4 + 3;
    }
}

inline void updatePosWithParticle(V3F_C4B_T2F_Quad* quad,
                                  const Vec2& newPosition,
                                  float size,
                                  float scaleInSize,
                                  float rotation,
                                  float staticRotation)
{
    // vertices
    float size_2 = size / 2;
    float x1     = -size_2 * scaleInSize;
    float y1     = -size_2 * scaleInSize;

    float x2 = size_2 * scaleInSize;
    float y2 = size_2 * scaleInSize;
    float x  = newPosition.x;
    float y  = newPosition.y;

    float r  = (float)-AX_DEGREES_TO_RADIANS(rotation + staticRotation);
    float cr = cosf(r);
    float sr = sinf(r);
    float ax = x1 * cr - y1 * sr + x;
    float ay = x1 * sr + y1 * cr + y;
    float bx = x2 * cr - y1 * sr + x;
    float by = x2 * sr + y1 * cr + y;
    float cx = x2 * cr - y2 * sr + x;
    float cy = x2 * sr + y2 * cr + y;
    float dx = x1 * cr - y2 * sr + x;
    float dy = x1 * sr + y2 * cr + y;

    // bottom-left
    quad->bl.vertices.x = ax;
    quad->bl.vertices.y = ay;

    // bottom-right vertex:
    quad->br.vertices.x = bx;
    quad->br.vertices.y = by;

    // top-left vertex:
    quad->tl.vertices.x = dx;
    quad->tl.vertices.y = dy;

    // top-right vertex:
    quad->tr.vertices.x = cx;
    quad->tr.vertices.y = cy;
}

void ParticleSystemQuad::updateParticleQuads()
{
    if (_particleCount <= 0)
    {
        return;
    }

    Vec2 currentPosition;
    if (_positionType == PositionType::FREE)
    {
        currentPosition = this->convertToWorldSpace(Vec2::ZERO);
    }
    else if (_positionType == PositionType::RELATIVE)
    {
        currentPosition = _position;
    }

    V3F_C4B_T2F_Quad* startQuad;
    Vec2 pos = Vec2::ZERO;
    if (_batchNode)
    {
        V3F_C4B_T2F_Quad* batchQuads = _batchNode->getTextureAtlas()->getQuads();
        startQuad                    = &(batchQuads[_atlasIndex]);
        pos                          = _position;
    }
    else
    {
        startQuad = &(_quads[0]);
    }

    if (_positionType == PositionType::FREE)
    {
        Vec3 p1(currentPosition.x, currentPosition.y, 0);
        Mat4 worldToNodeTM = getWorldToNodeTransform();
        worldToNodeTM.transformPoint(&p1);
        Vec3 p2;
        Vec2 newPos;
        float* startX               = _particleData.startPosX;
        float* startY               = _particleData.startPosY;
        float* x                    = _particleData.posx;
        float* y                    = _particleData.posy;
        float* s                    = _particleData.size;
        float* r                    = _particleData.rotation;
        float* sr                   = _particleData.staticRotation;
        float* sid                  = _particleData.scaleInDelta;
        float* sil                  = _particleData.scaleInLength;
        V3F_C4B_T2F_Quad* quadStart = startQuad;
        if (_isScaleInAllocated)
        {
            for (int i = 0; i < _particleCount;
                 ++i, ++startX, ++startY, ++x, ++y, ++quadStart, ++s, ++r, ++sr, ++sid, ++sil)
            {
                p2.set(*startX, *startY, 0);
                worldToNodeTM.transformPoint(&p2);
                newPos.set(*x, *y);
                p2 = p1 - p2;
                newPos.x -= p2.x - pos.x;
                newPos.y -= p2.y - pos.y;
                updatePosWithParticle(quadStart, newPos, *s, tweenfunc::expoEaseOut(*sid / *sil), *r, *sr);
            }
        }
        else
        {
            for (int i = 0; i < _particleCount; ++i, ++startX, ++startY, ++x, ++y, ++quadStart, ++s, ++r, ++sr)
            {
                p2.set(*startX, *startY, 0);
                worldToNodeTM.transformPoint(&p2);
                newPos.set(*x, *y);
                p2 = p1 - p2;
                newPos.x -= p2.x - pos.x;
                newPos.y -= p2.y - pos.y;
                updatePosWithParticle(quadStart, newPos, *s, 1.0F, *r, *sr);
            }
        }
    }
    else if (_positionType == PositionType::RELATIVE)
    {
        Vec2 newPos;
        float* startX               = _particleData.startPosX;
        float* startY               = _particleData.startPosY;
        float* x                    = _particleData.posx;
        float* y                    = _particleData.posy;
        float* s                    = _particleData.size;
        float* r                    = _particleData.rotation;
        float* sr                   = _particleData.staticRotation;
        float* sid                  = _particleData.scaleInDelta;
        float* sil                  = _particleData.scaleInLength;
        V3F_C4B_T2F_Quad* quadStart = startQuad;
        if (_isScaleInAllocated)
        {
            for (int i = 0; i < _particleCount;
                 ++i, ++startX, ++startY, ++x, ++y, ++quadStart, ++s, ++r, ++sr, ++sid, ++sil)
            {
                newPos.set(*x, *y);
                newPos.x = *x - (currentPosition.x - *startX);
                newPos.y = *y - (currentPosition.y - *startY);
                newPos += pos;
                updatePosWithParticle(quadStart, newPos, *s, tweenfunc::expoEaseOut(*sid / *sil), *r, *sr);
            }
        }
        else
        {
            for (int i = 0; i < _particleCount; ++i, ++startX, ++startY, ++x, ++y, ++quadStart, ++s, ++r, ++sr)
            {
                newPos.set(*x, *y);
                newPos.x = *x - (currentPosition.x - *startX);
                newPos.y = *y - (currentPosition.y - *startY);
                newPos += pos;
                updatePosWithParticle(quadStart, newPos, *s, 1.0F, *r, *sr);
            }
        }
    }
    else
    {
        Vec2 newPos;
        float* startX               = _particleData.startPosX;
        float* startY               = _particleData.startPosY;
        float* x                    = _particleData.posx;
        float* y                    = _particleData.posy;
        float* s                    = _particleData.size;
        float* r                    = _particleData.rotation;
        float* sr                   = _particleData.staticRotation;
        float* sid                  = _particleData.scaleInDelta;
        float* sil                  = _particleData.scaleInLength;
        V3F_C4B_T2F_Quad* quadStart = startQuad;
        if (_isScaleInAllocated)
        {
            for (int i = 0; i < _particleCount;
                 ++i, ++startX, ++startY, ++x, ++y, ++quadStart, ++s, ++r, ++sr, ++sid, ++sil)
            {
                newPos.set(*x + pos.x, *y + pos.y);
                updatePosWithParticle(quadStart, newPos, *s, tweenfunc::expoEaseOut(*sid / *sil), *r, *sr);
            }
        }
        else
        {
            for (int i = 0; i < _particleCount; ++i, ++startX, ++startY, ++x, ++y, ++quadStart, ++s, ++r, ++sr)
            {
                newPos.set(*x + pos.x, *y + pos.y);
                updatePosWithParticle(quadStart, newPos, *s, 1.0F, *r, *sr);
            }
        }
    }

    V3F_C4B_T2F_Quad* quad = startQuad;
    float* r               = _particleData.colorR;
    float* g               = _particleData.colorG;
    float* b               = _particleData.colorB;
    float* a               = _particleData.colorA;

    if (_isOpacityFadeInAllocated)
    {
        float* fadeDt = _particleData.opacityFadeInDelta;
        float* fadeLn = _particleData.opacityFadeInLength;

        // HSV calculation is expensive, so we should skip it if it's not enabled.
        if (_isHSVAllocated)
        {
            float* hue = _particleData.hue;
            float* sat = _particleData.sat;
            float* val = _particleData.val;

            if (_opacityModifyRGB)
            {
                auto hsv = HSV();
                for (int i = 0; i < _particleCount;
                     ++i, ++quad, ++r, ++g, ++b, ++a, ++hue, ++sat, ++val, ++fadeDt, ++fadeLn)
                {
                    hsv.fromRgba({*r, *g, *b, *a * (*fadeDt / *fadeLn)});
                    hsv.h += *hue;
                    hsv.s     = abs(*sat);
                    hsv.v     = abs(*val);
                    auto colF = hsv.toColor4F();
                    quad->bl.colors.set(colF.r * colF.a * 255.0F, colF.g * colF.a * 255.0F, colF.b * colF.a * 255.0F,
                                        colF.a * 255.0F);
                    quad->br.colors.set(colF.r * colF.a * 255.0F, colF.g * colF.a * 255.0F, colF.b * colF.a * 255.0F,
                                        colF.a * 255.0F);
                    quad->tl.colors.set(colF.r * colF.a * 255.0F, colF.g * colF.a * 255.0F, colF.b * colF.a * 255.0F,
                                        colF.a * 255.0F);
                    quad->tr.colors.set(colF.r * colF.a * 255.0F, colF.g * colF.a * 255.0F, colF.b * colF.a * 255.0F,
                                        colF.a * 255.0F);
                }
            }
            else
            {
                auto hsv = HSV();
                for (int i = 0; i < _particleCount;
                     ++i, ++quad, ++r, ++g, ++b, ++a, ++hue, ++sat, ++val, ++fadeDt, ++fadeLn)
                {
                    hsv.fromRgba({*r, *g, *b, *a * (*fadeDt / *fadeLn)});
                    hsv.h += *hue;
                    hsv.s    = abs(*sat);
                    hsv.v    = abs(*val);
                    auto col = hsv.toColor4B();
                    quad->bl.colors.set(col.r, col.g, col.b, col.a);
                    quad->br.colors.set(col.r, col.g, col.b, col.a);
                    quad->tl.colors.set(col.r, col.g, col.b, col.a);
                    quad->tr.colors.set(col.r, col.g, col.b, col.a);
                }
            }
        }
        else
        {
            // set color
            if (_opacityModifyRGB)
            {
                for (int i = 0; i < _particleCount; ++i, ++quad, ++r, ++g, ++b, ++a, ++fadeDt, ++fadeLn)
                {
                    uint8_t colorR = *r * *a * 255;
                    uint8_t colorG = *g * *a * 255;
                    uint8_t colorB = *b * *a * 255;
                    uint8_t colorA = *a * (*fadeDt / *fadeLn) * 255;
                    quad->bl.colors.set(colorR, colorG, colorB, colorA);
                    quad->br.colors.set(colorR, colorG, colorB, colorA);
                    quad->tl.colors.set(colorR, colorG, colorB, colorA);
                    quad->tr.colors.set(colorR, colorG, colorB, colorA);
                }
            }
            else
            {
                for (int i = 0; i < _particleCount; ++i, ++quad, ++r, ++g, ++b, ++a, ++fadeDt, ++fadeLn)
                {
                    uint8_t colorR = *r * 255;
                    uint8_t colorG = *g * 255;
                    uint8_t colorB = *b * 255;
                    uint8_t colorA = *a * (*fadeDt / *fadeLn) * 255;
                    quad->bl.colors.set(colorR, colorG, colorB, colorA);
                    quad->br.colors.set(colorR, colorG, colorB, colorA);
                    quad->tl.colors.set(colorR, colorG, colorB, colorA);
                    quad->tr.colors.set(colorR, colorG, colorB, colorA);
                }
            }
        }
    }
    else
    {
        // HSV calculation is expensive, so we should skip it if it's not enabled.
        if (_isHSVAllocated)
        {
            float* hue = _particleData.hue;
            float* sat = _particleData.sat;
            float* val = _particleData.val;

            if (_opacityModifyRGB)
            {
                auto hsv = HSV();
                for (int i = 0; i < _particleCount; ++i, ++quad, ++r, ++g, ++b, ++a, ++hue, ++sat, ++val)
                {
                    hsv.fromRgba({*r, *g, *b, *a});
                    hsv.h += *hue;
                    hsv.s     = abs(*sat);
                    hsv.v     = abs(*val);
                    auto colF = hsv.toColor4F();
                    quad->bl.colors.set(colF.r * colF.a * 255.0F, colF.g * colF.a * 255.0F, colF.b * colF.a * 255.0F,
                                        colF.a * 255.0F);
                    quad->br.colors.set(colF.r * colF.a * 255.0F, colF.g * colF.a * 255.0F, colF.b * colF.a * 255.0F,
                                        colF.a * 255.0F);
                    quad->tl.colors.set(colF.r * colF.a * 255.0F, colF.g * colF.a * 255.0F, colF.b * colF.a * 255.0F,
                                        colF.a * 255.0F);
                    quad->tr.colors.set(colF.r * colF.a * 255.0F, colF.g * colF.a * 255.0F, colF.b * colF.a * 255.0F,
                                        colF.a * 255.0F);
                }
            }
            else
            {
                auto hsv = HSV();
                for (int i = 0; i < _particleCount; ++i, ++quad, ++r, ++g, ++b, ++a, ++hue, ++sat, ++val)
                {
                    hsv.fromRgba({*r, *g, *b, *a});
                    hsv.h += *hue;
                    hsv.s    = abs(*sat);
                    hsv.v    = abs(*val);
                    auto col = hsv.toColor4B();
                    quad->bl.colors.set(col.r, col.g, col.b, col.a);
                    quad->br.colors.set(col.r, col.g, col.b, col.a);
                    quad->tl.colors.set(col.r, col.g, col.b, col.a);
                    quad->tr.colors.set(col.r, col.g, col.b, col.a);
                }
            }
        }
        else
        {
            // set color
            if (_opacityModifyRGB)
            {
                for (int i = 0; i < _particleCount; ++i, ++quad, ++r, ++g, ++b, ++a)
                {
                    uint8_t colorR = *r * *a * 255;
                    uint8_t colorG = *g * *a * 255;
                    uint8_t colorB = *b * *a * 255;
                    uint8_t colorA = *a * 255;
                    quad->bl.colors.set(colorR, colorG, colorB, colorA);
                    quad->br.colors.set(colorR, colorG, colorB, colorA);
                    quad->tl.colors.set(colorR, colorG, colorB, colorA);
                    quad->tr.colors.set(colorR, colorG, colorB, colorA);
                }
            }
            else
            {
                for (int i = 0; i < _particleCount; ++i, ++quad, ++r, ++g, ++b, ++a)
                {
                    uint8_t colorR = *r * 255;
                    uint8_t colorG = *g * 255;
                    uint8_t colorB = *b * 255;
                    uint8_t colorA = *a * 255;
                    quad->bl.colors.set(colorR, colorG, colorB, colorA);
                    quad->br.colors.set(colorR, colorG, colorB, colorA);
                    quad->tl.colors.set(colorR, colorG, colorB, colorA);
                    quad->tr.colors.set(colorR, colorG, colorB, colorA);
                }
            }
        }
    }

    // The reason for using for-loops separately for every property is because
    // When the processor needs to read from or write to a location in memory,
    // it first checks whether a copy of that data is in the cpu's cache.
    // And wether if every property's memory of the particle system is continuous,
    // for the purpose of improving cache hit rate, we should process only one property in one for-loop.
    // It was proved to be effective especially for low-end devices.
    if ((_isLifeAnimated || _isEmitterAnimated || _isLoopAnimated) && _isAnimAllocated)
    {
        V3F_C4B_T2F_Quad* quad    = startQuad;
        unsigned short* cellIndex = _particleData.animCellIndex;

        ParticleFrameDescriptor index;
        for (int i = 0; i < _particleCount; ++i, ++quad, ++cellIndex)
        {
            float left = 0.0F, bottom = 0.0F, top = 1.0F, right = 1.0F;

            // TODO: index.isRotated should be treated accordingly

            auto iter = _animationIndices.find(*cellIndex);
            if (iter == _animationIndices.end())
                index.rect = {0, 0, float(_texture->getPixelsWide()), float(_texture->getPixelsHigh())};
            else
                index = iter->second;

            auto texWidth  = _texture->getPixelsWide();
            auto texHeight = _texture->getPixelsHigh();

            left  = index.rect.origin.x / texWidth;
            right = (index.rect.origin.x + index.rect.size.x) / texWidth;

            top    = index.rect.origin.y / texHeight;
            bottom = (index.rect.origin.y + index.rect.size.y) / texHeight;

            quad->bl.texCoords.u = left;
            quad->bl.texCoords.v = bottom;

            quad->br.texCoords.u = right;
            quad->br.texCoords.v = bottom;

            quad->tl.texCoords.u = left;
            quad->tl.texCoords.v = top;

            quad->tr.texCoords.u = right;
            quad->tr.texCoords.v = top;
        }
    }
}

// overriding draw method
void ParticleSystemQuad::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    // quad command
    if (_particleCount > 0)
    {
        auto programState = _quadCommand.getPipelineDescriptor().programState;

        ax::Mat4 projectionMat = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
        programState->setUniform(_mvpMatrixLocaiton, projectionMat.m, sizeof(projectionMat.m));

        _quadCommand.init(_globalZOrder, _texture, _blendFunc, _quads, _particleCount, transform, flags);
        renderer->addCommand(&_quadCommand);
    }
}

void ParticleSystemQuad::setTotalParticles(int tp)
{
    // If we are setting the total number of particles to a number higher
    // than what is allocated, we need to allocate new arrays
    if (tp > _allocatedParticles)
    {
        // Allocate new memory
        size_t quadsSize   = sizeof(_quads[0]) * tp * 1;
        size_t indicesSize = sizeof(_indices[0]) * tp * 6 * 1;

        _particleData.release();
        if (!_particleData.init(tp))
        {
            AXLOGW("Particle system: not enough memory");
            return;
        }
        V3F_C4B_T2F_Quad* quadsNew = (V3F_C4B_T2F_Quad*)realloc(_quads, quadsSize);
        unsigned short* indicesNew = (unsigned short*)realloc(_indices, indicesSize);

        if (quadsNew && indicesNew)
        {
            // Assign pointers
            _quads   = quadsNew;
            _indices = indicesNew;

            // Clear the memory
            memset(_quads, 0, quadsSize);
            memset(_indices, 0, indicesSize);

            _allocatedParticles = tp;
        }
        else
        {
            // Out of memory, failed to resize some array
            if (quadsNew)
                _quads = quadsNew;
            if (indicesNew)
                _indices = indicesNew;

            AXLOGW("Particle system: out of memory");
            return;
        }

        _totalParticles = tp;

        // Init particles
        if (_batchNode)
        {
            for (int i = 0; i < _totalParticles; i++)
            {
                _particleData.atlasIndex[i] = i;
            }
        }

        initIndices();
        //        setupVBO();

        // fixed https://axmol.dev//issues/3990
        // Updates texture coords.
        updateTexCoords();
    }
    else
    {
        _totalParticles = tp;
    }

    // fixed issue #5762
    // reset the emission rate
    setEmissionRate(_totalParticles / _life);

    resetSystem();
}

void ParticleSystemQuad::listenRendererRecreated(EventCustom* /*event*/)
{
    // when comes to foreground in android, _buffersVBO and _VAOname is a wild handle
    // before recreating, we need to reset them to 0
    //    memset(_buffersVBO, 0, sizeof(_buffersVBO));
    //    if (Configuration::getInstance()->supportsShareableVAO())
    //    {
    //        _VAOname = 0;
    //        setupVBOandVAO();
    //    }
    //    else
    //    {
    //        setupVBO();
    //    }
}

bool ParticleSystemQuad::allocMemory()
{
    AXASSERT(!_batchNode, "Memory should not be alloced when not using batchNode");

    AX_SAFE_FREE(_quads);
    AX_SAFE_FREE(_indices);

    _quads   = (V3F_C4B_T2F_Quad*)malloc(_totalParticles * sizeof(V3F_C4B_T2F_Quad));
    _indices = (unsigned short*)malloc(_totalParticles * 6 * sizeof(unsigned short));

    if (!_quads || !_indices)
    {
        AXLOGW("axmol: Particle system: not enough memory");
        AX_SAFE_FREE(_quads);
        AX_SAFE_FREE(_indices);

        return false;
    }

    memset(_quads, 0, _totalParticles * sizeof(V3F_C4B_T2F_Quad));
    memset(_indices, 0, _totalParticles * 6 * sizeof(unsigned short));

    return true;
}

void ParticleSystemQuad::setBatchNode(ParticleBatchNode* batchNode)
{
    if (_batchNode != batchNode)
    {
        ParticleBatchNode* oldBatch = _batchNode;

        ParticleSystem::setBatchNode(batchNode);

        // NEW: is self render ?
        if (!batchNode)
        {
            allocMemory();
            initIndices();
            setTexture(oldBatch->getTexture());
            //            setupVBO();
        }
        // OLD: was it self render ? cleanup
        else if (!oldBatch)
        {
            // copy current state to batch
            V3F_C4B_T2F_Quad* batchQuads = _batchNode->getTextureAtlas()->getQuads();
            V3F_C4B_T2F_Quad* quad       = &(batchQuads[_atlasIndex]);
            memcpy(quad, _quads, _totalParticles * sizeof(_quads[0]));

            AX_SAFE_FREE(_quads);
            AX_SAFE_FREE(_indices);
        }
    }
}

ParticleSystemQuad* ParticleSystemQuad::create()
{
    ParticleSystemQuad* particleSystemQuad = new ParticleSystemQuad();
    if (particleSystemQuad->init())
    {
        particleSystemQuad->autorelease();
        return particleSystemQuad;
    }
    AX_SAFE_DELETE(particleSystemQuad);
    return nullptr;
}

std::string ParticleSystemQuad::getDescription() const
{
    return StringUtils::format("<ParticleSystemQuad | Tag = %d, Total Particles = %d>", _tag, _totalParticles);
}
NS_AX_END
