/****************************************************************************
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

#include <optional>
#include <string>
#include <string_view>

#include "axmol/base/Event.h"
#include "axmol/math/Ray.h"
#include "axmol/math/Math.h"

/**
 * @addtogroup base
 * @{
 */

namespace ax
{

/** @class XRInputEvent
 * @brief Self-contained XR input event for hand/controller buttons, axes and poses.
 */
class AX_DLL XRInputEvent : public Event
{
public:
    enum class EventType
    {
        Button,
        Axis,
        Pose,
    };

    enum class Hand
    {
        Left,
        Right,
    };

    enum class Input
    {
        Trigger,
        Grip,
        Thumbstick,
        ThumbstickClick,
        Menu,
        A,
        B,
        X,
        Y,
        AimPose,
        GripPose,
    };

    enum class Phase
    {
        Pressed,
        Released,
        Changed,
        Active,
        Inactive,
    };

    struct State
    {
        EventType eventType{EventType::Button};
        Hand hand{Hand::Left};
        Input input{Input::Trigger};
        Phase phase{Phase::Changed};
        float value{0.0f};
        Vec2 axis{Vec2::zero};
        bool poseValid{false};
        std::optional<Ray> aimRay;
        bool gripPoseValid{false};
        Mat4 gripPose{Mat4::identity};
        std::string interactionProfile;
    };

    explicit XRInputEvent(const State& state);

    EventType getEventType() const { return _state.eventType; }
    Hand getHand() const { return _state.hand; }
    Input getInput() const { return _state.input; }
    Phase getPhase() const { return _state.phase; }
    float getValue() const { return _state.value; }
    const Vec2& getAxis() const { return _state.axis; }
    bool isPoseValid() const { return _state.poseValid; }
    bool hasAimRay() const { return _state.aimRay.has_value(); }
    const std::optional<Ray>& getAimRay() const { return _state.aimRay; }
    bool isGripPoseValid() const { return _state.gripPoseValid; }
    const Mat4& getGripPose() const { return _state.gripPose; }
    std::string_view getInteractionProfile() const { return _state.interactionProfile; }

private:
    State _state;
};

using EventXRInput = XRInputEvent;

}  // namespace ax

// end of base group
/// @}
