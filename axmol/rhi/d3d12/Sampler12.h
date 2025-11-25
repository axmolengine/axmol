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
#include <string_view>

namespace ax::rhi::d3d12
{

static constexpr std::string_view BuiltinSamplers =
    R"(
/// --------- Builtin Sampler State BEGIN -------
// --- Linear sampling ---
SamplerState LinearClamp : register(s0, space1);
SamplerState LinearWrap : register(s1, space1);
SamplerState LinearMirror : register(s2, space1);
SamplerState LinearBorder : register(s3, space1);

// --- Point sampling ---
SamplerState PointClamp : register(s4, space1);
SamplerState PointWrap : register(s5, space1);
SamplerState PointMirror : register(s6, space1);
SamplerState PointBorder : register(s7, space1);

// --- Linear + Mipmap ---
SamplerState LinearMipClamp : register(s8, space1);
SamplerState LinearMipWrap : register(s9, space1);
SamplerState LinearMipMirror : register(s10, space1);
SamplerState LinearMipBorder : register(s11, space1);

// --- Anisotropic filtering ---
SamplerState AnisoClamp : register(s12, space1);
SamplerState AnisoWrap : register(s13, space1);
SamplerState AnisoMirror : register(s14, space1);
SamplerState AnisoBorder : register(s15, space1);

// --- Depth comparison samplers ---
SamplerComparisonState ShadowCmpClamp : register(s16, space1);
SamplerComparisonState ShadowCmpWrap : register(s17, space1);
SamplerComparisonState ShadowCmpMirror : register(s18, space1);
SamplerComparisonState ShadowCmpBorder : register(s19, space1);

// --- Special cases ---
SamplerState LinearNoMipClamp : register(s20, space1);
SamplerState PointNoMipClamp : register(s21, space1);

/// ======== Builtin Sampler States END =========

)"sv;

}
