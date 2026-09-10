/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT

 The Axmol physics 2d/3d constants

 ****************************************************************************/

#pragma once

namespace ax::phconsts
{
// pixels per meter
inline constexpr float PixelsPerMeter = 10.0f;

// same with B2_HUGE
inline constexpr float LinearHuge = 1e5f * PixelsPerMeter;
inline constexpr float LinearSlop = 0.005f;

// (N)
inline constexpr float MaxForce = 1e5f;  // |F| <= MaxForce

// (rad / deg)
inline constexpr float MaxAngleDeg = 180.0f * 0.99f;  // Box2D v3 limit
inline constexpr float MaxAngleRad = 3.14159265359f * 0.99f;

// mass/density/inertia ...
inline constexpr float UpperBound = 1e6f;
inline constexpr float LowerBound = 1e-6f;

// (m/s)
inline constexpr float MaxLinearVelocity  = 1e4f;
inline constexpr float MaxAngularVelocity = 1e4f;

// damping
inline constexpr float MaxDamping = 1e3f;

// preferred worker counts
inline constexpr int PreferredWorkers2D = 2;
inline constexpr int PreferredWorkers3D = 4;

}  // namespace ax::phconsts
