/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismMatrix44.hpp"
#include "CubismMath.hpp"

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

CubismMatrix44 CubismMatrix44::GetInvert() const
{
    csmFloat32 r00 = _tr[0];
    csmFloat32 r10 = _tr[1];
    csmFloat32 r20 = _tr[2];
    csmFloat32 r01 = _tr[4];
    csmFloat32 r11 = _tr[5];
    csmFloat32 r21 = _tr[6];
    csmFloat32 r02 = _tr[8];
    csmFloat32 r12 = _tr[9];
    csmFloat32 r22 = _tr[10];

    csmFloat32 tx = _tr[12];
    csmFloat32 ty = _tr[13];
    csmFloat32 tz = _tr[14];

    csmFloat32 det = r00 * (r11 * r22 - r12 * r21) -
                     r01 * (r10 * r22 - r12 * r20) +
                     r02 * (r10 * r21 - r11 * r20);

    CubismMatrix44 dst;

    if (CubismMath::AbsF(det) < CubismMath::Epsilon)
    {
        dst.LoadIdentity();
        return dst;
    }

    csmFloat32 invDet = 1.0f / det;

    csmFloat32 inv00 = (r11 * r22 - r12 * r21) * invDet;
    csmFloat32 inv01 = -(r01 * r22 - r02 * r21) * invDet;
    csmFloat32 inv02 = (r01 * r12 - r02 * r11) * invDet;
    csmFloat32 inv10 = -(r10 * r22 - r12 * r20) * invDet;
    csmFloat32 inv11 = (r00 * r22 - r02 * r20) * invDet;
    csmFloat32 inv12 = -(r00 * r12 - r02 * r10) * invDet;
    csmFloat32 inv20 = (r10 * r21 - r11 * r20) * invDet;
    csmFloat32 inv21 = -(r00 * r21 - r01 * r20) * invDet;
    csmFloat32 inv22 = (r00 * r11 - r01 * r10) * invDet;

    dst._tr[0] = inv00;
    dst._tr[1] = inv10;
    dst._tr[2] = inv20;
    dst._tr[3] = 0.0f;
    dst._tr[4] = inv01;
    dst._tr[5] = inv11;
    dst._tr[6] = inv21;
    dst._tr[7] = 0.0f;
    dst._tr[8] = inv02;
    dst._tr[9] = inv12;
    dst._tr[10] = inv22;
    dst._tr[11] = 0.0f;

    dst._tr[12] = -(inv00 * tx + inv01 * ty + inv02 * tz);
    dst._tr[13] = -(inv10 * tx + inv11 * ty + inv12 * tz);
    dst._tr[14] = -(inv20 * tx + inv21 * ty + inv22 * tz);
    dst._tr[15] = 1.0f;

    return dst;
}

}}}
