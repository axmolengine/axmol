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

#include "axmol/tlx/hlookup.hpp"
#include "RHITypes.h"

namespace ax::rhi
{
/**
 * @addtogroup _rhi
 * @{
 */

// The builtin sampler index
struct SamplerIndex
{
    enum : uint32_t
    {
        // --- Linear sampling ---
        LinearClamp,   // Linear, clamp to edge
        LinearWrap,    // Linear, repeat
        LinearMirror,  // Linear, mirror repeat
        LinearBorder,  // Linear, border color

        // --- Point sampling ---
        PointClamp,   // Nearest, clamp to edge
        PointWrap,    // Nearest, repeat
        PointMirror,  // Nearest, mirror repeat
        PointBorder,  // Nearest, border color

        // --- Linear + Mipmap ---
        LinearMipClamp,   // Linear min/mag, mip linear, clamp
        LinearMipWrap,    // Linear min/mag, mip linear, wrap
        LinearMipMirror,  // Linear min/mag, mip linear, mirror
        LinearMipBorder,  // Linear min/mag, mip linear, border

        // --- Anisotropic filtering ---
        AnisoClamp,   // Anisotropic, clamp to edge
        AnisoWrap,    // Anisotropic, repeat
        AnisoMirror,  // Anisotropic, mirror repeat
        AnisoBorder,  // Anisotropic, border color

        // --- Depth comparison samplers (shadow maps) ---
        ShadowCmpClamp,   // Compare sampler, clamp to edge
        ShadowCmpWrap,    // Compare sampler, repeat
        ShadowCmpMirror,  // Compare sampler, mirror repeat
        ShadowCmpBorder,  // Compare sampler, border color

        // --- Special cases ---
        LinearNoMipClamp,  // Linear min/mag, no mip, clamp (UI, 2D sprites)
        PointNoMipClamp,   // Point min/mag, no mip, clamp (pixel art)

        //
        Count
    };
};


class SamplerCache
{
public:

    static constexpr uint32_t MAX_SAMPLER_COUNT = 256;

    static SamplerCache* getInstance();
    static void destroyInstance();

    ~SamplerCache();

    void removeAllSamplers();

    void invalidateAll();

    SamplerHandle getSampler(const SamplerDesc& desc);

private:
    axstd::hash_map<uint32_t, SamplerHandle> _samplers;
};

// end of _rhi group
/// @}

}  // namespace ax::rhi
