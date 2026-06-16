/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Type/CubismBasicType.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Handles 4x4 matrices
 */
class CubismMatrix44
{
public:
    /**
     * Constructor
     */
    CubismMatrix44();

    /**
     * Destructor
     */
    virtual ~CubismMatrix44();

    /**
     * Multiplies the two given matrices and stores the result in the destination matrix.
     *
     * @param a Matrix a
     * @param b Matrix b
     * @param dst Destination matrix for storing the result
     */
    static void Multiply(csmFloat32* a, csmFloat32* b, csmFloat32* dst);

    /**
     * Sets the identity matrix.
     */
    void            LoadIdentity();

    /**
     * Returns the matrix as an array of floating-point numbers.
     *
     * @return 4x4 matrix represented by 16 floating-point numbers
     */
    csmFloat32*     GetArray();

    /**
     * Sets the matrix.
     *
     * @param tr 4x4 matrix represented by 16 floating-point numbers
     */
    void            SetMatrix(csmFloat32* tr);

    /**
     * Returns the scaling factor along the X-axis.
     *
     * @return Scaling factor along the X-axis
     */
    csmFloat32      GetScaleX() const;

    /**
     * Returns the scaling factor along the Y-axis.
     *
     * @return Scaling factor along the Y-axis
     */
    csmFloat32      GetScaleY() const;

    /**
     * Returns the translation value along the X-axis.
     *
     * @return Translation value along the X-axis
     */
    csmFloat32      GetTranslateX() const;

    /**
     * Returns the translation value along the Y-axis.
     *
     * @return Translation value along the Y-axis
     */
    csmFloat32      GetTranslateY() const;

    /**
     * Calculates the value along the X-axis using the current matrix.
     *
     * @param src Value along the X-axis
     *
     * @return Calculated value along the X-axis using the current matrix
     */
    csmFloat32      TransformX(csmFloat32 src);

    /**
     * Calculates the value along the Y-axis using the current matrix.
     *
     * @param src Value along the Y-axis
     *
     * @return Calculated value along the Y-axis using the current matrix
     */
    csmFloat32      TransformY(csmFloat32 src);

    /**
     * Calculates the inverse value along the X-axis using the current matrix.
     *
     * @param src Value along the X-axis
     *
     * @return Calculated inverse value along the X-axis using the current matrix
     */
    csmFloat32      InvertTransformX(csmFloat32 src);

    /**
     * Calculates the inverse value along the Y-axis using the current matrix.
     *
     * @param src Value along the Y-axis
     *
     * @return Calculated inverse value along the Y-axis using the current matrix
     */
    csmFloat32      InvertTransformY(csmFloat32 src);

    /**
     * Moves relatively based on the current matrix position.
     *
     * @param x Translation amount along the X-axis
     * @param y Translation amount along the Y-axis
     */
    void            TranslateRelative(csmFloat32 x, csmFloat32 y);

    /**
     * Moves the current matrix position to the specified location.
     *
     * @param x Translation value along the X-axis
     * @param y Translation value along the Y-axis
     */
    void            Translate(csmFloat32 x, csmFloat32 y);

    /**
     * Moves the current matrix position along the X-axis to the specified location.
     *
     * @param x Translation value along the X-axis
     */
    void            TranslateX(csmFloat32 x);

    /**
     * Moves the current matrix position along the Y-axis to the specified location.
     *
     * @param x Translation value along the Y-axis
     */
    void            TranslateY(csmFloat32 y);

    /**
     * Sets the scaling factor relative to the current matrix.
     *
     * @param x Scaling factor along the X-axis
     * @param y Scaling factor along the Y-axis
     */
    void            ScaleRelative(csmFloat32 x, csmFloat32 y);

    /**
     * Sets the scaling factor of the current matrix to the specified value.
     *
     * @param x Scaling factor along the X-axis
     * @param y Scaling factor along the Y-axis
     */
    void            Scale(csmFloat32 x, csmFloat32 y);

    /**
     * Multiplies the current matrix by the given matrix.
     *
     * @param m Matrix to multiply with the current matrix
     */
    void            MultiplyByMatrix(CubismMatrix44* m);

    /**
     * Calculates the inverse matrix of the current matrix.
     *
     * @return inverse matrix of the current matrix; identity matrix when epsilon is small.
     */
    CubismMatrix44 GetInvert() const;

protected:
    csmFloat32  _tr[16];
};

}}}
