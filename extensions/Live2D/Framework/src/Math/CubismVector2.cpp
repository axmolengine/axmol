/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismVector2.hpp"
#include "Math/CubismMath.hpp"

namespace Live2D { namespace Cubism { namespace Framework {
CubismVector2 operator+(const CubismVector2& a, const CubismVector2& b)
{
    return CubismVector2(a.X + b.X, a.Y + b.Y);
}

CubismVector2 operator-(const CubismVector2& a, const CubismVector2& b)
{
    return CubismVector2(a.X - b.X, a.Y - b.Y);
}

CubismVector2 operator*(const CubismVector2& vector, const csmFloat32 scalar)
{
    return CubismVector2(vector.X * scalar, vector.Y * scalar);
}

CubismVector2 operator*(const csmFloat32 scalar, const CubismVector2& vector)
{
    return CubismVector2(vector.X * scalar, vector.Y * scalar);
}

CubismVector2 operator/(const CubismVector2& vector, const csmFloat32 scalar)
{
    return CubismVector2(vector.X / scalar, vector.Y / scalar);
}

const CubismVector2& CubismVector2::operator+=(const CubismVector2& rhs)
{
    X += rhs.X;
    Y += rhs.Y;

    return *this;
}

const CubismVector2& CubismVector2::operator-=(const CubismVector2& rhs)
{
    X -= rhs.X;
    Y -= rhs.Y;

    return *this;
}

const CubismVector2& CubismVector2::operator*=(const CubismVector2& rhs)
{
    X *= rhs.X;
    Y *= rhs.Y;

    return *this;
}

const CubismVector2& CubismVector2::operator/=(const CubismVector2& rhs)
{
    X /= rhs.X;
    Y /= rhs.Y;

    return *this;
}

const CubismVector2& CubismVector2::operator*=(const csmFloat32 scalar)
{
    X *= scalar;
    Y *= scalar;

    return *this;
}

const CubismVector2& CubismVector2::operator/=(const csmFloat32 scalar)
{
    X /= scalar;
    Y /= scalar;

    return *this;
}

csmBool CubismVector2::operator==(const CubismVector2& rhs) const
{
    return (X == rhs.X) && (Y == rhs.Y);
}

csmBool CubismVector2::operator!=(const CubismVector2& rhs) const
{
    return !(*this == rhs);
}

csmFloat32 CubismVector2::GetLength() const
{
    return CubismMath::SqrtF(X * X + Y * Y);
}

csmFloat32 CubismVector2::GetDistanceWith(CubismVector2 a) const
{
    return CubismMath::SqrtF(((X - a.X) * (X - a.X)) + ((Y - a.Y) * (Y - a.Y)));
}

csmFloat32 CubismVector2::Dot(const CubismVector2& a) const
{
    return (X * a.X) + (Y * a.Y);
}

void CubismVector2::Normalize()
{
    const csmFloat32 length = powf((X * X) + (Y * Y), 0.5f);

    X = X / length;
    Y = Y / length;
}

}}}
