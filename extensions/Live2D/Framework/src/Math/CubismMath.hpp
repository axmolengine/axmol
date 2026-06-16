/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <cmath>
#include "Type/CubismBasicType.hpp"
#include "Math/CubismVector2.hpp"

//--------- LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Handles numerical calculations and other utility functions
 */
class CubismMath
{
public:
    static const csmFloat32 Pi;
    static const csmFloat32 Epsilon;

    /**
     * Returns the value of the first argument clamped within the range of the minimum and maximum values.
     *
     *@param value Value to be clamped
     *@param min Minimum value of the range
     *@param max Maximum value of the range
     *
     *@return Value clamped within the range of minimum and maximum values
     */
    static csmFloat32 RangeF(csmFloat32 value, csmFloat32 min, csmFloat32 max)
    {
        if (value < min) value = min;
        else if (value > max) value = max;
        return value;
    };

    /**
     * Returns the value of the sine function.
     *
     *@param x Angle value in radians [rad]
     *
     *@return Value of the sine function sin(x)
     */
    static csmFloat32 SinF(csmFloat32 x)
    {
        return sinf(x);
    };

    /**
     * Returns the value of the cosine function.
     *
     *@param x Angle value in radians [rad]
     *
     *@return Value of the cosine function cos(x)
     */
    static csmFloat32 CosF(csmFloat32 x)
    {
        return cosf(x);
    };

    /**
     * Returns the absolute value.
     *
     * @param x Value to calculate the absolute value
     *
     * @return Absolute value of the input
     */
    static csmFloat32 AbsF(csmFloat32 x)
    {
        return std::fabs(x);
    };

    /**
     * Returns the square root.
     *
     * @param x Value to calculate the square root
     *
     * @return Square root of the input
     */
    static csmFloat32 SqrtF(csmFloat32 x)
    {
        return sqrtf(x);
    };

    /**
     * Returns the value processed with sine wave easing.<br>
     * Can be used for easing during fade-in and fade-out.
     *
     * @param value Value to be eased
     *
     * @return Value processed with sine wave easing
     */
    static csmFloat32 GetEasingSine(csmFloat32 value)
    {
        if (value < 0.0f) return 0.0f;
        else if (value > 1.0f) return 1.0f;

        return static_cast<csmFloat32>(0.5f - 0.5f * CosF(value * Pi));
    }

    /**
     * Returns the greater value.
     *
     * @param l Left-hand value
     * @param r Right-hand value
     *
     * @return Greater value
     */
    static csmFloat32 Max(csmFloat32 l, csmFloat32 r)
    {
        return (l > r) ? l : r;
    }

    /**
     * Returns the smaller value.
     *
     * @param l Left-hand value
     * @param r Right-hand value
     *
     * @return Smaller value
     */
    static csmFloat32 Min(csmFloat32 l, csmFloat32 r)
    {
        return (l > r) ? r : l;
    }

    /**
     * Returns the value clamped within the specified range.
     *
     * @param val Value to be clamped within the range
     * @param min Minimum value
     * @param max Maximum value
     * @return Clamped value within the range
     */
    static csmInt32 Clamp(csmInt32 val, csmInt32 min, csmInt32 max);

    /**
     * Returns the value clamped within the specified range.
     *
     * @param val Value to be checked within the range
     * @param min Minimum value
     * @param max Maximum value
     * @return Clamped value within the range
     */
    static csmFloat32 ClampF(csmFloat32 val, csmFloat32 min, csmFloat32 max);

    /**
     * Converts an angle from degrees to radians.
     *
     * @param degrees Angle in degrees [degree]
     *
     * @return Angle in radians [rad]
     */
    static csmFloat32 DegreesToRadian(csmFloat32 degrees);

    /**
     * Converts an angle from radians to degrees.
     *
     * @param radian Angle in radians [rad]
     *
     * @return Angle in degrees [degree]
     */
    static csmFloat32 RadianToDegrees(csmFloat32 radian);

    /**
     * Calculates the angle between two vectors.
     *
     * @param from Starting vector
     * @param to Ending vector
     *
     * @return Angle in radians [rad]
     */
    static csmFloat32 DirectionToRadian(CubismVector2 from, CubismVector2 to);

    /**
     * Calculates the angle between two vectors.
     *
     * @param from Starting vector
     * @param to Ending vector
     *
     * @return Angle in radians [rad]
     */
    static csmFloat32 DirectionToDegrees(CubismVector2 from, CubismVector2 to);

    /**
     * Calculates the direction vector from an angle.
     *
     * @param totalAngle Angle in radians [rad]
     *
     * @return Direction vector
     */
    static CubismVector2 RadianToDirection(csmFloat32 totalAngle);

    /**
     * Finds the solution of a quadratic equation when the cubic coefficient of the cubic equation is zero.<br>
     *          a * x^2 + b * x + c = 0
     *
     * @param a Coefficient of the quadratic term
     * @param b Coefficient of the linear term
     * @param c Constant term
     *
     * @return Solution of the quadratic equation
     */
    static csmFloat32 QuadraticEquation(csmFloat32 a, csmFloat32 b, csmFloat32 c);

    /**
     * Finds the solution of a cubic equation corresponding to the Bezier's t-value using Cardano's formula.<br>
     * Returns the solution in the range 0.0-1.0 when it is a multiple root.<br>
     *          a * x^3 + b * x^2 + c * x + d = 0
     *
     * @param a Coefficient of the cubic term
     * @param b Coefficient of the quadratic term
     * @param c Coefficient of the linear term
     * @param d Constant term
     *
     * @return Solution in the range 0.0-1.0
     */
    static csmFloat32 CardanoAlgorithmForBezier(csmFloat32 a, csmFloat32 b, csmFloat32 c, csmFloat32 d);

    /**
     * Calculates the remainder of a floating-point division.
     *
     * @param dividend Dividend (value to be divided)
     * @param divisor Divisor (value to divide by)
     * @return Remainder
     */
    static csmFloat32 ModF(csmFloat32 dividend, csmFloat32 divisor);

private:
    CubismMath();
};

}}}

//--------- LIVE2D NAMESPACE ------------
