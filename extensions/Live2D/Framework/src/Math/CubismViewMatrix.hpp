/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismMatrix44.hpp"

namespace Live2D { namespace Cubism { namespace Framework {
/**
 * Handles 4x4 matrix utilities for changing camera position
 */
class CubismViewMatrix : public CubismMatrix44
{
public:
    /**
     * Constructor
     */
    CubismViewMatrix();

    /**
     * Destructor
     */
    virtual ~CubismViewMatrix();

    /**
     * Moves by the given values.<br>
     * The movement amount is adjusted with respect to the screen size and its maximum value.
     *
     * @param x Translation amount along the X-axis
     * @param y Translation amount along the Y-axis
     */
    void        AdjustTranslate(csmFloat32 x, csmFloat32 y);

    /**
     * Scales by the given values.<br>
     * The scaling factor is adjusted considering the maximum and minimum scaling rates.
     *
     * @param cx Center position along the X-axis for scaling
     * @param cy Center position along the Y-axis for scaling
     * @param scale Scaling factor
     */
    void        AdjustScale(csmFloat32 cx, csmFloat32 cy, csmFloat32 scale);

    /**
     * Sets the range on the logical coordinates corresponding to the device.
     *
     * @param left X-axis position of the left edge
     * @param right X-axis position of the right edge
     * @param bottom Y-axis position of the bottom edge
     * @param top Y-axis position of the top edge
     */
    void        SetScreenRect(csmFloat32 left, csmFloat32 right, csmFloat32 bottom, csmFloat32 top);

    /**
     * Sets the movable range on the logical coordinates corresponding to the device.
     *
     * @param left X-axis position of the left edge
     * @param right X-axis position of the right edge
     * @param bottom Y-axis position of the bottom edge
     * @param top Y-axis position of the top edge
     */
    void        SetMaxScreenRect(csmFloat32 left, csmFloat32 right, csmFloat32 bottom, csmFloat32 top);

    /**
     * Sets the maximum scaling factor.
     *
     * @param maxScale Maximum scaling factor
     */
    void        SetMaxScale(csmFloat32 maxScale);

    /**
     * Sets the minimum scaling factor.
     *
     * @param minScale Minimum scaling factor
     */
    void        SetMinScale(csmFloat32 minScale);

    /**
     * Returns the maximum scaling factor.
     *
     * @return Maximum scaling factor
     */
    csmFloat32  GetMaxScale() const;

    /**
     * Returns the minimum scaling factor.
     *
     * @return Minimum scaling factor
     */
    csmFloat32  GetMinScale() const;

    /**
     * Checks if the scaling factor is at its maximum.
     *
     * @return true if the scaling factor is at its maximum; otherwise false.
     */
    csmBool     IsMaxScale() const;

    /**
     * Checks if the scaling factor is at its minimum.
     *
     * @return true if the scaling factor is at its minimum; otherwise false.
     */
    csmBool     IsMinScale() const;

    /**
     * Returns the X-axis position of the left edge on the logical coordinates corresponding to the device.
     *
     * @return X-axis position of the left edge on the logical coordinates corresponding to the device
     */
    csmFloat32  GetScreenLeft() const;

    /**
     * Returns the X-axis position of the right edge on the logical coordinates corresponding to the device.
     *
     * @return X-axis position of the right edge on the logical coordinates corresponding to the device
     */
    csmFloat32  GetScreenRight() const;

    /**
     * Returns the Y-axis position of the bottom edge on the logical coordinates corresponding to the device.
     *
     * @return Y-axis position of the bottom edge on the logical coordinates corresponding to the device
     */
    csmFloat32  GetScreenBottom() const;

    /**
     * Returns the Y-axis position of the top edge on the logical coordinates corresponding to the device.
     *
     * @return Y-axis position of the top edge on the logical coordinates corresponding to the device
     */
    csmFloat32  GetScreenTop() const;

    /**
     * Returns the maximum X-axis position of the left edge.
     *
     * @return Maximum X-axis position of the left edge
     */
    csmFloat32  GetMaxLeft() const;

    /**
     * Returns the maximum X-axis position of the right edge.
     *
     * @return Maximum X-axis position of the right edge
     */
    csmFloat32  GetMaxRight() const;

    /**
     * Returns the maximum Y-axis position of the bottom edge.
     *
     * @return Maximum Y-axis position of the bottom edge
     */
    csmFloat32  GetMaxBottom() const;

    /**
     * Returns the maximum Y-axis position of the top edge.
     *
     * @return Maximum Y-axis position of the top edge
     */
    csmFloat32  GetMaxTop() const;

private:
    csmFloat32  _screenLeft;
    csmFloat32  _screenRight;
    csmFloat32  _screenTop;
    csmFloat32  _screenBottom;
    csmFloat32  _maxLeft;
    csmFloat32  _maxRight;
    csmFloat32  _maxTop;
    csmFloat32  _maxBottom;
    csmFloat32  _maxScale;
    csmFloat32  _minScale;
};

}}}
