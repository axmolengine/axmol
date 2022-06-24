/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismViewMatrix.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

CubismViewMatrix::CubismViewMatrix()
                                    : _screenLeft(0.0f)
                                    , _screenRight(0.0f)
                                    , _screenTop(0.0f)
                                    , _screenBottom(0.0f)
                                    , _maxLeft(0.0f)
                                    , _maxRight(0.0f)
                                    , _maxTop(0.0f)
                                    , _maxBottom(0.0f)
                                    , _maxScale(0.0f)
                                    , _minScale(0.0f)
{ }

CubismViewMatrix::~CubismViewMatrix()
{ }

csmFloat32 CubismViewMatrix::GetMaxScale() const
{
    return _maxScale;
}

csmFloat32 CubismViewMatrix::GetMinScale() const
{
    return _minScale;
}

csmFloat32 CubismViewMatrix::GetScreenLeft() const
{
    return _screenLeft;
}

csmFloat32 CubismViewMatrix::GetScreenRight() const
{
    return _screenRight;
}

csmFloat32 CubismViewMatrix::GetScreenBottom() const
{
    return _screenBottom;
}

csmFloat32 CubismViewMatrix::GetScreenTop() const
{
    return _screenTop;
}

csmFloat32 CubismViewMatrix::GetMaxLeft() const
{
    return _maxLeft;
}

csmFloat32 CubismViewMatrix::GetMaxRight() const
{
    return _maxRight;
}

csmFloat32 CubismViewMatrix::GetMaxBottom() const
{
    return _maxBottom;
}

csmFloat32 CubismViewMatrix::GetMaxTop() const
{
    return _maxTop;
}

csmBool CubismViewMatrix::IsMaxScale() const
{
    return GetScaleX() >= _maxScale;
}

csmBool CubismViewMatrix::IsMinScale() const
{
    return GetScaleX() <= _minScale;
}


void CubismViewMatrix::AdjustTranslate(csmFloat32 x, csmFloat32 y)
{
    if (_tr[0] * _maxLeft + (_tr[12] + x) > _screenLeft)
    {
        x = _screenLeft - _tr[0] * _maxLeft - _tr[12];
    }

    if (_tr[0] * _maxRight + (_tr[12] + x) < _screenRight)
    {
        x = _screenRight - _tr[0] * _maxRight - _tr[12];
    }


    if (_tr[5] * _maxTop + (_tr[13] + y) < _screenTop)
    {
        y = _screenTop - _tr[5] * _maxTop - _tr[13];
    }

    if (_tr[5] * _maxBottom + (_tr[13] + y) > _screenBottom)
    {
        y = _screenBottom - _tr[5] * _maxBottom - _tr[13];
    }

    csmFloat32 tr1[] = {
                        1.0f,   0.0f,   0.0f, 0.0f,
                        0.0f,   1.0f,   0.0f, 0.0f,
                        0.0f,   0.0f,   1.0f, 0.0f,
                        x,      y,      0.0f, 1.0f
                        };
    Multiply(tr1, _tr, _tr);
}

void CubismViewMatrix::AdjustScale(csmFloat32 cx, csmFloat32 cy, csmFloat32 scale)
{
    csmFloat32 MaxScale = GetMaxScale();
    csmFloat32 MinScale = GetMinScale();

    csmFloat32 targetScale = scale * _tr[0]; //

    if (targetScale < MinScale)
    {
        if (_tr[0] > 0.0f)
        {
            scale = MinScale / _tr[0];
        }
    }
    else if (targetScale > MaxScale)
    {
        if (_tr[0] > 0.0f)
        {
            scale = MaxScale / _tr[0];
        }
    }

    csmFloat32 tr1[16] = {
                          1.0f, 0.0f, 0.0f, 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f,
                          0.0f, 0.0f, 1.0f, 0.0f,
                          cx,   cy,   0.0f, 1.0f
                          };
    csmFloat32 tr2[16] = {
                          scale, 0.0f,  0.0f, 0.0f,
                          0.0f,  scale, 0.0f, 0.0f,
                          0.0f,  0.0f,  1.0f, 0.0f,
                          0.0f,  0.0f,  0.0f, 1.0f
                          };
    csmFloat32 tr3[16] = {
                          1.0f, 0.0f, 0.0f, 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f,
                          0.0f, 0.0f, 1.0f, 0.0f,
                          -cx,  -cy,  0.0f, 1.0f
                          };

    Multiply(tr3, _tr, _tr);
    Multiply(tr2, _tr, _tr);
    Multiply(tr1, _tr, _tr);
}

void CubismViewMatrix::SetScreenRect(csmFloat32 left, csmFloat32 right, csmFloat32 bottom, csmFloat32 top)
{
    _screenLeft = left;
    _screenRight = right;
    _screenTop = top;
    _screenBottom = bottom;
}

void CubismViewMatrix::SetMaxScreenRect(csmFloat32 left, csmFloat32 right, csmFloat32 bottom, csmFloat32 top)
{
    _maxLeft = left;
    _maxRight = right;
    _maxTop = top;
    _maxBottom = bottom;
}

void CubismViewMatrix::SetMaxScale(csmFloat32 maxScale)
{
    _maxScale = maxScale;
}

void CubismViewMatrix::SetMinScale(csmFloat32 minScale)
{
    _minScale = minScale;
}

}}}
