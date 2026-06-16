/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismMath.hpp"
#include "Utils/CubismDebug.hpp"

namespace Live2D {namespace Cubism {namespace Framework {

const csmFloat32 CubismMath::Pi = 3.1415926535897932384626433832795f;
const csmFloat32 CubismMath::Epsilon = 0.00001f;

csmInt32 CubismMath::Clamp(csmInt32 val, csmInt32 min, csmInt32 max)
{
    if (val < min)
    {
        return min;
    }
    else if (max < val)
    {
        return max;
    }

    return val;
}

csmFloat32 CubismMath::ClampF(csmFloat32 val, csmFloat32 min, csmFloat32 max)
{
    if (val < min)
    {
        return min;
    }
    else if (max < val)
    {
        return max;
    }

    return val;
}

csmFloat32 CubismMath::DegreesToRadian(csmFloat32 degrees)
{
    return (degrees / 180.0f) * Pi;
}

csmFloat32 CubismMath::RadianToDegrees(csmFloat32 radian)
{
    return (radian * 180.0f) / Pi;
}

csmFloat32 CubismMath::DirectionToRadian(CubismVector2 from, CubismVector2 to)
{
    csmFloat32 q1;
    csmFloat32 q2;
    csmFloat32 ret;

    q1 = atan2f(to.Y, to.X);
    q2 = atan2f(from.Y, from.X);

    ret = q1 - q2;

    while (ret < -Pi)
    {
        ret += Pi * 2.0f;
    }

    while (ret > Pi)
    {
        ret -= Pi * 2.0f;
    }

    return ret;
}

csmFloat32 CubismMath::DirectionToDegrees(CubismVector2 from, CubismVector2 to)
{
    csmFloat32 radian;
    csmFloat32 degree;

    radian = DirectionToRadian(from, to);
    degree = RadianToDegrees(radian);

    if ((to.X - from.X) > 0.0f)
    {
        degree = -degree;
    }

    return degree;
}

CubismVector2 CubismMath::RadianToDirection(csmFloat32 totalAngle)
{
    CubismVector2 ret;

    ret.X = CubismMath::SinF(totalAngle);
    ret.Y = CubismMath::CosF(totalAngle);

    return ret;
}

csmFloat32 CubismMath::QuadraticEquation(csmFloat32 a, csmFloat32 b, csmFloat32 c)
{
    if (CubismMath::AbsF(a) < CubismMath::Epsilon)
    {
        if (CubismMath::AbsF(b) < CubismMath::Epsilon)
        {
            return -c;
        }
        return -c / b;
    }

    return -(b + CubismMath::SqrtF(b * b - 4.0f * a * c)) / (2.0f * a);
}

csmFloat32 CubismMath::CardanoAlgorithmForBezier(csmFloat32 a, csmFloat32 b, csmFloat32 c, csmFloat32 d)
{
    if ( CubismMath::AbsF( a ) < CubismMath::Epsilon )
    {
        return CubismMath::RangeF( QuadraticEquation(b, c, d), 0.0f, 1.0f);
    }

    csmFloat32 ba = b / a;
    csmFloat32 ca = c / a;
    csmFloat32 da = d / a;


    csmFloat32 p = (3.0f * ca - ba*ba) / 3.0f;
    csmFloat32 p3 = p / 3.0f;
    csmFloat32 q = (2.0f * ba*ba*ba - 9.0f * ba*ca + 27.0f * da) / 27.0f;
    csmFloat32 q2 = q / 2.0f;
    csmFloat32 discriminant = q2*q2 + p3*p3*p3;

    const csmFloat32 center = 0.5f;
    const csmFloat32 threshold = center + 0.01f;

    if (discriminant < 0.0f) {
        csmFloat32 mp3 = -p / 3.0f;
        csmFloat32 mp33 = mp3*mp3*mp3;
        csmFloat32 r = CubismMath::SqrtF(mp33);
        csmFloat32 t = -q / (2.0f * r);
        csmFloat32 cosphi = RangeF(t, -1.0f, 1.0f);
        csmFloat32 phi = acos(cosphi);
        csmFloat32 crtr = cbrt(r);
        csmFloat32 t1 = 2.0f * crtr;

        csmFloat32 root1 = t1 * CubismMath::CosF(phi / 3.0f) - ba / 3.0f;
        if ( abs( root1 - center) < threshold)
        {
            return RangeF( root1, 0.0f, 1.0f);
        }

        csmFloat32 root2 = t1 * CubismMath::CosF((phi + 2.0f * CubismMath::Pi) / 3.0f) - ba / 3.0f;
        if (abs(root2 - center) < threshold)
        {
            return RangeF(root2, 0.0f, 1.0f);
        }

        csmFloat32 root3 = t1 * CubismMath::CosF((phi + 4.0f * CubismMath::Pi) / 3.0f) - ba / 3.0f;
        return RangeF(root3, 0.0f, 1.0f);
    }

    if (discriminant == 0.0f) {
        csmFloat32 u1;
        if (q2 < 0.0f)
        {
            u1 = cbrt(-q2);
        }
        else
        {
            u1 = -cbrt(q2);
        }

        csmFloat32 root1 = 2.0f * u1 - ba / 3.0f;
        if (abs(root1 - center) < threshold)
        {
            return RangeF(root1, 0.0f, 1.0f);
        }

        csmFloat32 root2 = -u1 - ba / 3.0f;
        return RangeF(root2, 0.0f, 1.0f);
    }

    csmFloat32 sd = CubismMath::SqrtF(discriminant);
    csmFloat32 u1 = cbrt(sd - q2);
    csmFloat32 v1 = cbrt(sd + q2);
    csmFloat32 root1 = u1 - v1 - ba / 3.0f;
    return RangeF(root1, 0.0f, 1.0f);
}

csmFloat32 CubismMath::ModF(csmFloat32 dividend, csmFloat32 divisor)
{
    if (
        !isfinite(dividend) ||
        divisor == 0 ||
        isnan(dividend) ||
        isnan(divisor)
        ) {
        CubismLogWarning("dividend: %f, divisor: %f ModF() returns 'NaN'.", dividend, divisor);
        return NAN;
    }

    // 絶対値に変換する。
    const csmFloat32 absDividend = CubismMath::AbsF(dividend);
    const csmFloat32 absDivisor = CubismMath::AbsF(divisor);

    // 絶対値で割り算する。
    csmFloat32 result = absDividend - floorf(absDividend / absDivisor) * absDivisor;

    // 符号を被除数のものに指定する。
    return copysign(result, dividend);
}

}}}
