/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"
#include "math.h"

#ifndef NULL
#   define  NULL  0
#endif

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Handles 2D vectors
 */
struct CubismVector2
{
    csmFloat32 X;       ///< X-axis value
    csmFloat32 Y;       ///< Y-axis value

    /**
     * Constructor
     */
    CubismVector2(): X(0.0f), Y(0.0f)
    {}

    /**
     * Constructor<br>
     * Initializes with specified values.
     *
     * @param x Value along the X-axis
     * @param y Value along the Y-axis
     */
    CubismVector2(csmFloat32 x, csmFloat32 y) : X(x), Y(y)
    {}

    /**
     * Adds two vectors.<br>
     * a + b = result
     *
     * @param a Value a
     * @param b Value b
     *
     * @return Result of a + b
     */
    friend CubismVector2 operator+(const CubismVector2& a, const CubismVector2& b);

    /**
     * Subtracts one vector from another.<br>
     * a - b = result
     *
     * @param a Value a
     * @param b Value b
     *
     * @return Result of a - b
     */
    friend CubismVector2 operator-(const CubismVector2& a, const CubismVector2& b);

    /**
     * Multiplies a vector by a scalar.<br>
     * vector * scalar = result
     *
     * @param vector Value vector (vector quantity)
     * @param scalar Value scalar (scalar quantity)
     *
     * @return Result of vector * scalar (vector quantity)
     */
    friend CubismVector2 operator*(const CubismVector2& vector, const csmFloat32 scalar);

    /**
     * Multiplies a scalar by a vector.<br>
     * scalar * vector = result
     *
     * @param scalar Value scalar (scalar quantity)
     * @param vector Value vector (vector quantity)
     *
     * @return Result of scalar * vector (vector quantity)
     */
    friend CubismVector2 operator*(const csmFloat32 scalar, const CubismVector2& vector);

    /**
     * Divides a vector by a scalar.<br>
     * vector / scalar = result
     *
     * @param vector Value vector (vector quantity)
     * @param scalar Value scalar (scalar quantity)
     *
     * @return Result of vector / scalar (vector quantity)
     */
    friend CubismVector2 operator/(const CubismVector2& vector, const csmFloat32 scalar);

    /**
     * Adds a vector to this vector.
     *
     * @param rhs Value to add
     *
     * @return Result of addition
     */
    const CubismVector2& operator+=(const CubismVector2& rhs);

    /**
     * Subtracts a vector from this vector.
     *
     * @param rhs Value to subtract
     *
     * @return Result of subtraction
     */
    const CubismVector2& operator-=(const CubismVector2& rhs);

    /**
     * Multiplies this vector by another vector.
     *
     * @param rhs Value to multiply by
     *
     * @return Result of multiplication
     */
    const CubismVector2& operator*=(const CubismVector2& rhs);

    /**
     * Divides this vector by another vector.
     *
     * @param rhs Value to divide by
     *
     * @return Result of division
     */
    const CubismVector2& operator/=(const CubismVector2& rhs);

    /**
     * Multiplies this vector by a scalar.
     *
     * @param scalar Value to multiply by
     *
     * @return Result of multiplication
     */
    const CubismVector2& operator*=(const csmFloat32 scalar);

    /**
     * Divides this vector by a scalar.
     *
     * @param scalar Value to divide by
     *
     * @return Result of division
     */
    const CubismVector2& operator/=(const csmFloat32 scalar);

    /**
     * Checks if the value is equal.
     *
     * @param rhs Value to check
     *
     * @return true if the value is equal; otherwise false.
     */
    csmBool operator==(const CubismVector2& rhs) const;

    /**
     * Checks if the value is not equal.
     *
     * @param rhs Value to check
     *
     * @return true if the value is not equal; otherwise false.
     */
    csmBool operator!=(const CubismVector2& rhs) const;

    /**
     * Normalizes the vector.
     */
    void Normalize();

    /**
     * Returns the length of the vector.
     *
     * @return Length of the vector
     */
    csmFloat32 GetLength() const;

    /**
     * Returns the distance between this vector and the given position vector.
     *
     * @param a Position vector
     *
     * @return Distance between the vectors
     */
    csmFloat32 GetDistanceWith(CubismVector2 a) const;

    /**
     * Calculates the dot product of this vector and the given vector.
     *
     * @param a Vector to be multiplied
     *
     * @return Result of the dot product
     */
    csmFloat32 Dot(const CubismVector2& a) const;
};

CubismVector2 operator+(const CubismVector2& a, const CubismVector2& b);
CubismVector2 operator-(const CubismVector2& a, const CubismVector2& b);
CubismVector2 operator*(const CubismVector2& vector, const csmFloat32 scalar);
CubismVector2 operator*(const csmFloat32 scalar, const CubismVector2& vector);
CubismVector2 operator/(const CubismVector2& vector, const csmFloat32 scalar);

}}}

//------------------------- LIVE2D NAMESPACE ------------
