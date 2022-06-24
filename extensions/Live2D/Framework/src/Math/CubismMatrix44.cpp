/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismMatrix44.hpp"

namespace Live2D { namespace Cubism { namespace Framework {
CubismMatrix44::CubismMatrix44()
{
    LoadIdentity();
}

CubismMatrix44::~CubismMatrix44()
{ }

void CubismMatrix44::LoadIdentity()
{
    csmFloat32 c[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    SetMatrix(c);
}

csmFloat32* CubismMatrix44::GetArray()
{
    return _tr;
}

void CubismMatrix44::Multiply(csmFloat32* a, csmFloat32* b, csmFloat32* dst)
{
    csmFloat32 c[16] = {
                        0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f
                        };
    csmInt32 n = 4;

    for (csmInt32 i = 0; i < n; ++i)
    {
        for (csmInt32 j = 0; j < n; ++j)
        {
            for (csmInt32 k = 0; k < n; ++k)
            {
                c[j + i * 4] += a[k + i * 4] * b[j + k * 4];
            }
        }
    }

    for (csmInt32 i = 0; i < 16; ++i)
    {
        dst[i] = c[i];
    }
}

void CubismMatrix44::TranslateRelative(csmFloat32 x, csmFloat32 y)
{
    csmFloat32 tr1[16] = {
                          1.0f, 0.0f, 0.0f, 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f,
                          0.0f, 0.0f, 1.0f, 0.0f,
                          x,    y,    0.0f, 1.0f
                          };

    Multiply(tr1, _tr, _tr);
}

void CubismMatrix44::Translate(csmFloat32 x, csmFloat32 y)
{
    _tr[12] = x;
    _tr[13] = y;
}

void CubismMatrix44::ScaleRelative(csmFloat32 x, csmFloat32 y)
{
    csmFloat32 tr1[16] = {
                          x,      0.0f,   0.0f, 0.0f,
                          0.0f,   y,      0.0f, 0.0f,
                          0.0f,   0.0f,   1.0f, 0.0f,
                          0.0f,   0.0f,   0.0f, 1.0f
                          };

    Multiply(tr1, _tr, _tr);
}

void CubismMatrix44::Scale(csmFloat32 x, csmFloat32 y)
{
    _tr[0] = x;
    _tr[5] = y;
}

csmFloat32 CubismMatrix44::TransformX(csmFloat32 src)
{
    return _tr[0] * src + _tr[12];
}

csmFloat32 CubismMatrix44::InvertTransformX(csmFloat32 src)
{
    return (src - _tr[12]) / _tr[0];
}

csmFloat32 CubismMatrix44::TransformY(csmFloat32 src)
{
    return _tr[5] * src + _tr[13];
}

csmFloat32 CubismMatrix44::InvertTransformY(csmFloat32 src)
{
    return (src - _tr[13]) / _tr[5];
}

void CubismMatrix44::SetMatrix(csmFloat32* tr)
{
    for (csmInt32 i = 0; i < 16; ++i)
    {
        _tr[i] = tr[i];
    }
}

csmFloat32 CubismMatrix44::GetScaleX() const
{
    return _tr[0];
}

csmFloat32 CubismMatrix44::GetScaleY() const
{
    return _tr[5];
}

csmFloat32 CubismMatrix44::GetTranslateX() const
{
    return _tr[12];
}

csmFloat32 CubismMatrix44::GetTranslateY() const
{
    return _tr[13];
}

void CubismMatrix44::TranslateX(csmFloat32 x)
{
    _tr[12] = x;
}

void CubismMatrix44::TranslateY(csmFloat32 y)
{
    _tr[13] = y;
}

void CubismMatrix44::MultiplyByMatrix(CubismMatrix44* m)
{
    Multiply(m->GetArray(), _tr, _tr);
}
}}}
