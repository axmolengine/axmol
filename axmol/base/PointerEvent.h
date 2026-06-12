/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#pragma once

#include "axmol/base/Event.h"
#include "axmol/base/Object.h"
#include "axmol/math/Math.h"

/**
 * @addtogroup base
 * @{
 */

namespace ax
{

class InputSystem;
class Camera;

/** @class PointerEvent
 * @brief Pointer event.
 */
class AX_DLL PointerEvent : public Event
{
    friend class InputSystem;

public:
    using CaptureBits = uint32_t;

    enum CaptureBit : CaptureBits
    {
        CAPTURE_NONE = 0,
        CAPTURED     = 1u << 31,
    };

    /**
     * Constructor.
     */
    PointerEvent();

    /** Returns the current touch location in Axmol world 2d-coordinates.
     *
     * @return The current touch location in Axmol coordinates.
     */
    Vec2 getLocation() const;

    /** Returns the previous touch location in Axmol world 2d-coordinates.
     *
     * @return The previous touch location in Axmol world 2d-coordinates.
     */
    Vec2 getPreviousLocation() const;

    /** Returns the start touch location in Axmol world 2d-coordinates.
     *
     * @return The start touch location in Axmol world 2d-coordinates.
     */
    Vec2 getStartLocation() const;

    /** Returns the delta of 2 current touches locations in Axmol world 2d-coordinates
     *
     * @return The delta position between the current location and the previous location in Axmol world
     * 2d-coordinates
     */
    Vec2 getDelta() const;

    /** Returns the current touch location in screen coordinates.
     *
     * @return The current touch location in screen coordinates.
     */
    Vec2 getScreenLocation() const;

    /** Returns the previous touch location in screen coordinates.
     *
     * @return The previous touch location in screen coordinates.
     */
    Vec2 getPreviousScreenLocation() const;

    /** Returns the start touch location in screen coordinates.
     *
     * @return The start touch location in screen coordinates.
     */
    Vec2 getStartScreenLocation() const;

    /** Get mouse scroll data of axis.
     *
     * @return The scroll data of axis.
     */
    const Vec2& getScrollDelta() const { return _scrollDelta; }
    float getScrollX() const { return _scrollDelta.x; }
    float getScrollY() const { return _scrollDelta.y; }

    /** Get pointer id.
     * @lua getId
     *
     * @return The id of pointer.
     */
    intptr_t getPointerId() const { return _pointerId; }

    /**
     * @brief Get the pointer device type for this event.
     *
     * Returns the type of pointer that produced the event (for example,
     * Mouse, Touch, or Pen).
     *
     * @return PointerType The pointer device type.
     */
    PointerType getPointerType() const { return _pointerType; }

    /**
     * @brief Get the index of the button that triggered this event.
     *
     * The value is a button index (0..n-1) when a physical button triggered
     * the event. Use -1 to indicate "no button" (for example, touch events
     * or pure move events).
     *
     * @return int The triggering button index, or -1 if none.
     *
     * @note This field represents the single button that caused this event.
     *       To inspect the current set of pressed buttons, use getPressedButtons().
     */
    int getButton() const { return _button; }

    /**
     * @brief Get the bitmask of currently pressed buttons.
     *
     * Each bit corresponds to a button index: bit i represents whether
     * button index i is currently pressed (1 = pressed, 0 = released).
     * For example, bit 0 = left button, bit 1 = right button, bit 2 = middle button.
     *
     * @return uint32_t Bitmask of pressed buttons; 0 means no buttons pressed.
     *
     * @note The mapping between button indices and bits is: bit i <-> button index i.
     *       For touch events, this value is typically 0.
     */
    uint32_t getPressedButtons() const { return _pressedButtons; }

    /**
     * @brief Check whether specified button index pressed
     */
    bool isButtonPressed(int buttonIndex) const;

    /**
     * @brief Checks if the primary input is currently pressed.
     *
     * This method returns true when the primary input source is actively
     * pressed and not in the PointerUp phase.
     *
     * - **Mouse**: True if the primary button (typically the left button) is pressed.
     * - **Touch**: True if the first touch point is active (touch down).
     * - **Pen**: True if the pen tip (primary button) is pressed.
     *
     * Use this method to detect the main interaction across devices
     * (mouse left drag, first finger touch drag, or pen tip press).
     *
     * @return True if the primary input is pressed, false otherwise.
     */
    bool isPrimaryPressed() const;

    /**
     * @brief Checks whether this event was routed through pointer capture.
     */
    bool isCaptured() const { return (_captureBits & CaptureBit::CAPTURED) != 0; }

    /**
     * @brief Returns the normalized pressure applied to the touch surface.
     *
     * This value represents the force intensity of the touch, universally supported across
     * multi-touch screens, digital styluses (e.g., Apple Pencil), and pressure-sensitive devices.
     *
     * @return A normalized float value typically ranging from 0.0f to 1.0f:
     * - **0.0f**: No pressure applied or hovering.
     * - **1.0f**: Standard full press intensity.
     * - Values **> 1.0f** may be returned on platforms supporting deep press (e.g., iOS 3D Touch).
     *
     * @note For legacy devices or platforms lacking pressure-sensing hardware, this safely
     * falls back to a constant **1.0f** to guarantee standard touch interactions.
     */
    float getPressure() const { return _pressure; }

    /**
     * @brief Get the phase of this pointer event.
     *
     * Returns the input phase that describes the current state
     * of the pointer interaction, such as when a mouse button,
     * touch, or pen contact is pressed, released, moved, or canceled.
     *
     * @return InputPhase enumeration value indicating the event phase:
     *         - InputPhase::PointerDown   when the pointer makes contact
     *         - InputPhase::PointerUp     when the pointer contact ends
     *         - InputPhase::PointerMove   when the pointer moves
     *         - InputPhase::PointerCancel when the pointer interaction is canceled
     *         - InputPhase::PointerScroll when the pointer performs a scroll
     *         - InputPhase::PointerEnter  when the pointer enters a region
     *         - InputPhase::PointerLeave  when the pointer leaves a region
     *
     * @note This reflects only the state of this PointerEvent
     *       instance and does not query the global pointer state.
     */
    InputPhase getPhase() const { return _phase; }

    /**
     * @brief Indicates whether this pointer is the primary pointer.
     *
     * Returns true if the pointer is considered primary. The primary pointer
     * is the one that controls the mouse cursor. For touch input, the first
     * finger that touches the screen is marked as primary. For mouse input,
     * the pointer is always primary.
     *
     * @return True if this is the primary pointer, false otherwise.
     */
    bool isPrimary() const { return _primary; }

    /** Set the touch information. It always used to monitor touch event.
     *
     * @param id A given id
     * @param point A given point in screen coordinate.
     * @param pressure maximum possible force for 3d touch.
     */
    [[internal]] void setPointerInfo(InputPhase phase, Vec2 point, const PointerInputState& inputState);
    [[internal]] void setCaptureBits(CaptureBits bits) { _captureBits = bits; }

    /** Set mouse scroll data.
     *
     * @param scrollX The scroll data of x axis.
     * @param scrollY The scroll data of y axis.
     */
    [[internal]] void setScrollData(Vec2 delta) { _scrollDelta = delta; }
    [[internal]] void setScrollData(float scrollX, float scrollY) { _scrollDelta = Vec2{scrollX, scrollY}; }

    [[internal]] void setCamera(const Camera* camera) { _camera = camera; }

    const Camera* getCamera() const { return _camera; }

protected:
    void setPhase(InputPhase phase) { _phase = phase; }

    void setPrimary(bool bval) { _primary = bval; }
    const Camera* _camera{nullptr};
    intptr_t _pointerId{-1};
    InputPhase _phase{InputPhase::PointerDown};
    PointerType _pointerType{PointerType::Mouse};
    int _button{InputButton::Left};
    uint32_t _pressedButtons{0};
    Vec2 _startPoint;
    Vec2 _point;
    Vec2 _prevPoint;
    Vec2 _scrollDelta;
    // Vec2 _tilt;
    float _pressure{1.0f};
    CaptureBits _captureBits{0};
    bool _startPointCaptured{false};
    bool _primary{true};
};

}  // namespace ax

// end of base group
/// @}
