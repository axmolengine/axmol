/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismMatrix44.hpp"
#include "Type/csmMap.hpp"

namespace Live2D { namespace Cubism { namespace Framework {
/**
 * Handles 4x4 matrices for setting model coordinates
 */
class CubismModelMatrix : public CubismMatrix44
{
public:
    /**
     * Constructor
     */
    CubismModelMatrix();

    /**
     * Constructor<br>
     * Creates with specified width and height.
     *
     * @param w Width
     * @param h Height
     */
    CubismModelMatrix(csmFloat32 w, csmFloat32 h);

    /**
     * Destructor
     */
    virtual ~CubismModelMatrix();

    /**
     * Sets the width.
     *
     * @param w Width
     */
    void    SetWidth(csmFloat32 w);

    /**
     * Sets the height.
     *
     * @param h Height
     */
    void    SetHeight(csmFloat32 h);

    /**
     * Sets the position.
     *
     * @param x Position along the X-axis
     * @param y Position along the Y-axis
     */
    void    SetPosition(csmFloat32 x, csmFloat32 y);

    /**
     * Sets the center position.
     *
     * @param x Center position along the X-axis
     * @param y Center position along the Y-axis
     *
     * @note Must be set after setting width or height to ensure correct scaling.
     */
    void    SetCenterPosition(csmFloat32 x, csmFloat32 y);

    /**
     * Sets the position of the top edge.
     *
     * @param y Y-axis position of the top edge
     */
    void    Top(csmFloat32 y);

    /**
     * Sets the position of the bottom edge.
     *
     * @param y Y-axis position of the bottom edge
     */
    void    Bottom(csmFloat32 y);

    /**
     * Sets the position of the left edge.
     *
     * @param x X-axis position of the left edge
     */
    void    Left(csmFloat32 x);

    /**
     * Sets the position of the right edge.
     *
     * @param x X-axis position of the right edge
     */
    void    Right(csmFloat32 x);

    /**
     * Sets the center position along the X-axis.
     *
     * @param x Center position along the X-axis
     */
    void    CenterX(csmFloat32 x);

    /**
     * Sets the position along the X-axis.
     *
     * @param x Position along the X-axis
     */
    void    SetX(csmFloat32 x);

    /**
     * Sets the center position along the Y-axis.
     *
     * @param y Center position along the Y-axis
     */
    void    CenterY(csmFloat32 y);

    /**
     * Sets the position along the Y-axis.
     *
     * @param y Position along the Y-axis
     */
    void    SetY(csmFloat32 y);

    /**
     * Sets the position based on layout information.
     *
     * @param layout Layout information
     */
    void    SetupFromLayout(csmMap<csmString, csmFloat32>& layout);

private:
    csmFloat32  _width;
    csmFloat32  _height;
};

}}}
