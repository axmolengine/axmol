/****************************************************************************
Copyright (c) 2011      ForzeField Studios S.L.
Copyright (c) 2010-2012 cocos2d-x.org
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
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN false EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "axmol/2d/MotionStreak.h"
#include "axmol/math/Vertex.h"
#include "axmol/base/Director.h"
#include "axmol/base/Utils.h"
#include "axmol/renderer/TextureCache.h"
#include "axmol/renderer/Texture2D.h"
#include "axmol/renderer/Renderer.h"
#include "axmol/renderer/Shaders.h"
#include "axmol/rhi/ProgramState.h"

namespace ax
{

MotionStreak::MotionStreak()
{
    _customCommand.setDrawType(CustomCommand::DrawType::ARRAY);
    _customCommand.setPrimitiveType(CustomCommand::PrimitiveType::TRIANGLE_STRIP);
}

MotionStreak::~MotionStreak()
{
    AX_SAFE_RELEASE(_texture);
    AX_SAFE_FREE(_pointState);
    AX_SAFE_FREE(_pointVertexes);
}

MotionStreak* MotionStreak::create(float fade, float minSeg, float stroke, const Color32& color, std::string_view path)
{
    MotionStreak* ret = new MotionStreak();
    if (ret->initWithFade(fade, minSeg, stroke, color, path))
    {
        ret->autorelease();
        return ret;
    }

    AX_SAFE_DELETE(ret);
    return nullptr;
}

MotionStreak* MotionStreak::create(float fade, float minSeg, float stroke, const Color32& color, Texture2D* texture)
{
    MotionStreak* ret = new MotionStreak();
    if (ret->initWithFade(fade, minSeg, stroke, color, texture))
    {
        ret->autorelease();
        return ret;
    }

    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool MotionStreak::initWithFade(float fade, float minSeg, float stroke, const Color32& color, std::string_view path)
{
    AXASSERT(!path.empty(), "Invalid filename");

    Texture2D* texture = _director->getTextureCache()->addImage(path);
    return initWithFade(fade, minSeg, stroke, color, texture);
}

bool MotionStreak::initWithFade(float fade, float minSeg, float stroke, const Color32& color, Texture2D* texture)
{
    Node::setPosition(Vec2::ZERO);
    setAnchorPoint(Vec2::ZERO);
    setIgnoreAnchorPointForPosition(true);
    _startingPositionInitialized = false;

    _positionR.setZero();
    _fastMode = true;
    _minSeg   = (minSeg == -1.0f) ? stroke / 5.0f : minSeg;
    _minSeg *= _minSeg;

    _stroke    = stroke;
    _fadeDelta = 1.0f / fade;

    // Fix #629 Motion streak vsync off crash
    double interval = _director->getAnimationInterval();
    double fps      = 1 / interval;
    if (roundf(fps) > 240.0)
        fps = 240.0;

    _maxPoints = (int)(fade * fps) + 2;

    _pointState    = (float*)malloc(sizeof(float) * _maxPoints);
    _pointVertexes = (Vec2*)malloc(sizeof(Vec2) * _maxPoints);

    constexpr size_t vertexSize = sizeof(_vertices[0]);

    auto vertexCount  = _maxPoints * 2;
    _vertices.resize(vertexCount);
    memset(_vertices.data(), 0, sizeof(_vertices[0]) * vertexCount);
    _customCommand.createVertexBuffer(vertexSize, vertexCount, CustomCommand::BufferUsage::DYNAMIC);

    _customCommand.updateVertexBuffer(_vertices.data(), vertexSize * vertexCount);

    setTexture(texture);
    setColor(color);
    scheduleUpdate();

    return true;
}

void MotionStreak::setPosition(const Vec2& position)
{
    if (!_startingPositionInitialized)
    {
        _startingPositionInitialized = true;
    }
    _positionR = position;
}

void MotionStreak::setPosition(float x, float y)
{
    if (!_startingPositionInitialized)
    {
        _startingPositionInitialized = true;
    }
    _positionR.x = x;
    _positionR.y = y;
}

const Vec2& MotionStreak::getPosition() const
{
    return _positionR;
}

void MotionStreak::getPosition(float* x, float* y) const
{
    *x = _positionR.x;
    *y = _positionR.y;
}

float MotionStreak::getPositionX() const
{
    return _positionR.x;
}

Vec3 MotionStreak::getPosition3D() const
{
    return Vec3(_positionR.x, _positionR.y, getPositionZ());
}

void MotionStreak::setPositionX(float x)
{
    if (!_startingPositionInitialized)
    {
        _startingPositionInitialized = true;
    }
    _positionR.x = x;
}

float MotionStreak::getPositionY() const
{
    return _positionR.y;
}

void MotionStreak::setPositionY(float y)
{
    if (!_startingPositionInitialized)
    {
        _startingPositionInitialized = true;
    }
    _positionR.y = y;
}

void MotionStreak::tintWithColor(const Color32& color)
{
    setColor(color);

    // Fast assignation
    for (unsigned int i = 0; i < _nuPoints * 2; ++i)
    {
        _vertices[i].color = color;
    }
}

Texture2D* MotionStreak::getTexture() const
{
    return _texture;
}

void MotionStreak::setTexture(Texture2D* texture)
{
    if (texture == nullptr)
        texture = _director->getTextureCache()->getWhiteTexture();

    if (_texture != texture)
    {
        AX_SAFE_RETAIN(texture);
        AX_SAFE_RELEASE(_texture);
        _texture = texture;

        setProgramStateWithRegistry(rhi::ProgramType::POS_UV_COLOR_2D, _texture);
    }
}

bool MotionStreak::setProgramState(rhi::ProgramState* programState, bool ownPS /*= false*/)
{
    if (Node::setProgramState(programState, ownPS))
    {
        AXASSERT(programState, "argument should not be nullptr");
        auto& pipelineDesc        = _customCommand.getPipelineDesc();
        pipelineDesc.programState = _programState;

        _mvpMatrixLocaiton = _programState->getUniformLocation("u_MVPMatrix");

        updateProgramStateTexture(_texture);
        return true;
    }
    return false;
}

void MotionStreak::setBlendFunc(const BlendFunc& blendFunc)
{
    _blendFunc = blendFunc;
}

const BlendFunc& MotionStreak::getBlendFunc() const
{
    return _blendFunc;
}

void MotionStreak::setOpacity(uint8_t /*opacity*/)
{
    AXASSERT(false, "Set opacity no supported");
}

uint8_t MotionStreak::getOpacity() const
{
    AXASSERT(false, "Opacity no supported");
    return 0;
}

void MotionStreak::setOpacityModifyRGB(bool /*bValue*/) {}

bool MotionStreak::isOpacityModifyRGB() const
{
    return false;
}

void MotionStreak::update(float delta)
{
    if (!_startingPositionInitialized)
        return;

    delta *= _fadeDelta;

    unsigned int newIdx, newIdx2, i, i2;
    unsigned int mov = 0;

    // Update current points
    for (i = 0; i < _nuPoints; i++)
    {
        _pointState[i] -= delta;

        if (_pointState[i] <= 0)
            mov++;
        else
        {
            newIdx = i - mov;

            if (mov > 0)
            {
                // Move data
                _pointState[newIdx] = _pointState[i];

                // Move point
                _pointVertexes[newIdx] = _pointVertexes[i];

                // Move pos
                i2                     = i * 2;
                newIdx2                = newIdx * 2;
                _vertices[newIdx2].position     = _vertices[i2].position;
                _vertices[newIdx2 + 1].position = _vertices[i2 + 1].position;

                // Move color
                _vertices[newIdx2].color     = _vertices[i2].color;
                _vertices[newIdx2 + 1].color = _vertices[i2 + 1].color;
            }
            else
                newIdx2 = newIdx * 2;

            const uint8_t alpha           = (uint8_t)(_pointState[newIdx] * 255.0f);
            _vertices[newIdx2].color.a    = alpha;
            _vertices[newIdx2].color.a    = alpha;
        }
    }
    _nuPoints -= mov;

    // Append new point
    bool appendNewPoint = true;
    if (_nuPoints >= _maxPoints)
        appendNewPoint = false;
    else if (_nuPoints > 0)
    {
        bool a1 = _pointVertexes[_nuPoints - 1].getDistanceSq(_positionR) < _minSeg;
        bool a2 =
            (_nuPoints == 1) ? false : (_pointVertexes[_nuPoints - 2].getDistanceSq(_positionR) < (_minSeg * 2.0f));
        if (a1 || a2)
            appendNewPoint = false;
    }

    if (appendNewPoint)
    {
        _pointVertexes[_nuPoints] = _positionR;
        _pointState[_nuPoints]    = 1.0f;

        // Color assignment
        const auto offset           = _nuPoints * 2;
        auto tmpColor               = _displayedColor;
        tmpColor.a                  = 255;
        _vertices[offset].color     = tmpColor;
        _vertices[offset + 1].color = tmpColor;

        // Generate polygon
        if (_nuPoints > 0 && _fastMode)
        {
            if (_nuPoints > 1)
            {
                vertexLineToPolygon(_pointVertexes, _stroke, _vertices.data(), _nuPoints, 1);
            }
            else
            {
                vertexLineToPolygon(_pointVertexes, _stroke, _vertices.data(), 0, 2);
            }
        }

        ++_nuPoints;
    }

    if (!_fastMode)
        vertexLineToPolygon(_pointVertexes, _stroke, _vertices.data(), 0, _nuPoints);

    // Updated Tex Coords only if they are different than previous step
    if (_nuPoints && _previousNuPoints != _nuPoints)
    {
        float texDelta = 1.0f / _nuPoints;
        for (i = 0; i < _nuPoints; ++i)
        {
            _vertices[i * 2].texCoord     = Tex2F(0, texDelta * i);
            _vertices[i * 2 + 1].texCoord = Tex2F(1, texDelta * i);
        }

        _previousNuPoints = _nuPoints;
    }
}

void MotionStreak::reset()
{
    _nuPoints = 0;
}

void MotionStreak::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    if (_nuPoints <= 1)
        return;

    auto drawCount = _nuPoints * 2;

    _customCommand.init(_globalZOrder, _blendFunc);
    _customCommand.setVertexDrawInfo(0, drawCount);
    renderer->addCommand(&_customCommand);

    auto programState = _customCommand.getPipelineDesc().programState;

    const auto& projectionMat = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    Mat4 finalMat             = projectionMat * transform;
    programState->setUniform(_mvpMatrixLocaiton, finalMat.m, sizeof(Mat4));

    _customCommand.updateVertexBuffer(_vertices.data(), sizeof(_vertices[0]) * drawCount);
}

}
