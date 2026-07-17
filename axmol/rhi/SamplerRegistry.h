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
#include "axmol/tlx/vector.hpp"
#include "RHITypes.h"
#include <string>

namespace ax::rhi
{
/**
 * @addtogroup _rhi
 * @{
 */

class DriverBase;

struct SamplerId
{
    static constexpr uint16_t INVALID = 0xffff;

    uint16_t value{INVALID};

    constexpr explicit operator bool() const { return value != INVALID; }
    constexpr bool operator==(const SamplerId&) const = default;
};

class SamplerRegistry
{
public:
    static constexpr uint32_t MAX_SAMPLER_COUNT = 256;

    static SamplerRegistry* getInstance();
    static void destroyInstance();

    SamplerRegistry();
    ~SamplerRegistry();

    void rebuild();

    SamplerId registerSampler(std::string_view name, const SamplerDesc& desc);
    SamplerId find(std::string_view name) const;
    const SamplerDesc& getDesc(SamplerId id) const;

    SamplerHandle getSampler(SamplerId samplerId);
    SamplerHandle getSampler(SamplerPreset::enum_type samplerIndex);
    SamplerHandle getSampler(const SamplerDesc& desc);

    SamplerPreset::enum_type registerSampler(const SamplerDesc& desc);

private:
    void removeAllSamplers();
    void createBuiltinSamplers();
    SamplerId registerBuiltinSampler(SamplerPreset::enum_type preset, const SamplerDesc& desc);

    tlx::pod_vector<SamplerHandle> _samplers;
    tlx::pod_vector<SamplerDesc> _samplerDescs;
    tlx::hash_map<std::string, SamplerId> _samplerIdsByName;

    tlx::hash_map<uint32_t, uint32_t> _samplersRegistry;  // legacy desc => sampler index registry

    DriverBase* _driver{nullptr};

    uint32_t _nextSamplerIndex{0};
};

// end of _rhi group
/// @}

}  // namespace ax::rhi
