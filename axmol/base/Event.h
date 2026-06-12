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

#include "axmol/base/Object.h"
#include "axmol/platform/PlatformMacros.h"

#include <stdint.h>
#include <type_traits>

/**
 * @addtogroup base
 * @{
 */

namespace ax
{

class Node;

enum class InputPhase
{
    PointerDown,
    PointerUp,
    PointerMove,
    PointerCancel,
    PointerScroll,
    KeyDown,
    KeyUp,
    KeyRepeat,
};

enum class PointerType
{
    Mouse,
    Touch,
    Pen,
};

/**
 * @brief Mouse / pen button indices.
 *
 * This enum-like struct defines integer indices for common mouse/pen buttons.
 * The values are intended to be used as button indices (0..n-1) when reporting
 * which single button triggered an event. Use -1 to indicate "no button"
 * (for example, touch events or pure move events).
 *
 * Mapping convention:
 *  - 0 => left button
 *  - 1 => right button
 *  - 2 => middle button
 *  - n => additional buttons (if present) use subsequent indices
 *
 * @note Use the separate bitmask field (e.g., pressedButtons) to represent the
 *       current set of pressed buttons; bit i corresponds to button index i.
 */
struct InputButton
{
    enum
    {
        None    = -1,  //!< No button triggered (touch or no-button event)
        Primary = 0,   //!< Primary button: mouse left / pen tip
        Secondary,     //!< Secondary button: mouse right / pen barrel
        Tertiary,      //!< Tertiary button: mouse middle / pen eraser
        // Alias
        Left   = Primary,
        Right  = Secondary,
        Middle = Tertiary
        // Additional indices may follow (4, 5, ...) for extra buttons
    };
};

/**
 * @brief Internal representation of raw pointer input from the platform.
 *
 * This structure is used internally by the InputSystem to capture native
 * input events (touch, mouse, pen, etc.) coming from the OS or windowing layer.
 * Fields contain unprocessed, screen-space values and raw attributes such as
 * pointer id, button index, pressed-button bitmask and pressure.
 *
 * @note Pointer id rules:
 * - Touch / Pen: non-negative integer ids assigned per active contact by the platform.
 * - Mouse: the identifier is set to 0.
 *
 * @note Button semantics:
 * - The @c button field is a single button index indicating which button
 *   triggered this event. It uses the convention:
 *     - -1 (InputButton::None) => no button triggered (e.g., touch or pure move)
 *     - 0..n-1 => button index (0 typically = left, 1 = right, 2 = middle)
 * - For PointerMove events the @c button value MUST be -1.
 * - Use @c pressedButtons to inspect the current set of pressed buttons.
 *
 * @note pressedButtons bitmask:
 * - Each bit corresponds to a button index: bit i represents whether button
 *   index i is currently pressed (1 = pressed, 0 = released).
 * - Example:
 *     - bit 0 (1u << 0) => left button
 *     - bit 1 (1u << 1) => right button
 *     - bit 2 (1u << 2) => middle button
 * - For touch events, @c pressedButtons is typically 0.
 *
 * @note Coordinate / transform:
 * - Values @c x and @c y are raw native screen-space coordinates. The
 *   InputSystem is responsible for applying input scaling and transforming
 *   these into engine/view coordinates before constructing and dispatching
 *   a higher-level PointerEvent to listeners.
 *
 * @warning This type is internal to the input pipeline and should not be
 *          exposed directly to user code or public APIs.
 */
struct PointerInputState
{
    intptr_t id{-1};                    //!< Unique identifier for the pointer (platform-provided or mapped).
    float pressure{1.0f};               //!< Raw pressure value reported by the platform (if available).
    int32_t button{InputButton::None};  //!< Triggering button index (0..n-1), or -1 when none (touch / no button).
    uint32_t pressedButtons{0};         //!< Bitmask of currently pressed buttons; bit i corresponds to button index i.
    PointerType type{PointerType::Mouse};  //!< Type of pointer (Mouse, Touch, Pen).
};

/** @class Event
 * @brief Base class of all kinds of events.
 */
class AX_DLL Event : public Object
{
public:
    /** Type Event type.*/
    enum class Type
    {
        POINTER,
        KEYBOARD,
        ACCELERATION,
        FOCUS,
        GAME_CONTROLLER,
        CUSTOM
    };

    /** Constructor */
    Event(Type type);

public:
    /** Destructor.
     */
    virtual ~Event();

    /** Gets the event type.
     *
     * @return The event type.
     */
    Type getType() const { return _type; }

    /** Stops propagation for current event.
     */
    void stopPropagation() { _isStopped = true; }

    /** Checks whether the event has been stopped.
     *
     * @return True if the event has been stopped.
     */
    bool isStopped() const { return _isStopped; }

    /** Gets current target of the event.
     * @return The target with which the event associates.
     * @note It's only available when the event listener is associated with node.
     *        It returns 0 when the listener is associated with fixed priority.
     */
    Node* getCurrentTarget() { return _currentTarget; }

    // Syntactic sugar for safe downcasting without explicit static_cast in business logic
    template <typename _Ty>
    [[nodiscard]] const _Ty* as() const
    {
        if constexpr (std::is_base_of_v<Event, _Ty>)
        {
            return static_cast<const _Ty*>(this);
        }
        return nullptr;
    }

protected:
    /** Sets current target */
    void setCurrentTarget(Node* target) { _currentTarget = target; }

    Type _type;  ///< Event type

    bool _isStopped;       ///< whether the event has been stopped.
    Node* _currentTarget;  ///< Current target

    friend class EventDispatcher;
};

}  // namespace ax

// end of base group
/// @}
