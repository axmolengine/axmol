/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Handles the direction the model is facing
 */
class CubismTargetPoint
{
public:
    /**
     * Constructor
     */
    CubismTargetPoint();

    /**
     * Destructor
     */
    virtual ~CubismTargetPoint();

    /**
     * Updates the state.
     *
     * @param deltaTimeSeconds Time step in seconds [seconds]
     */
    void        Update(csmFloat32 deltaTimeSeconds);

    /**
     * Returns the direction value along the X-axis.
     *
     * @return Direction value of the face along the X-axis (-1.0 to 1.0)
     */
    csmFloat32  GetX() const;

    /**
     * Returns the direction value along the Y-axis.
     *
     * @return Direction value of the face along the Y-axis (-1.0 to 1.0)
     */
    csmFloat32  GetY() const;

    /**
     * Sets the target direction values.
     *
     * @param x Direction value along the X-axis (-1.0 to 1.0)
     * @param y Direction value along the Y-axis (-1.0 to 1.0)
     */
    void        Set(csmFloat32 x, csmFloat32 y);

private:
    csmFloat32  _faceTargetX;
    csmFloat32  _faceTargetY;
    csmFloat32  _faceX;
    csmFloat32  _faceY;
    csmFloat32  _faceVX;
    csmFloat32  _faceVY;
    csmFloat32  _lastTimeSeconds;
    csmFloat32  _userTimeSeconds;

};

}}}
