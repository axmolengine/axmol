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

}  // namespace ax::phconsts
