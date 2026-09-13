/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
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
