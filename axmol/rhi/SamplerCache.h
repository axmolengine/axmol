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
#include "axmol/tlx/pod_vector.hpp"
#include "RHITypes.h"

namespace ax::rhi
{
/**
 * @addtogroup _rhi
 * @{
 */

class DriverBase;

class SamplerCache
{
public:
    static constexpr uint32_t MAX_SAMPLER_COUNT = 256;

    static SamplerCache* getInstance();
    static void destroyInstance();

    SamplerCache();
    ~SamplerCache();

    void invalidateAll();

    SamplerHandle getSampler(SamplerIndex::enum_type samplerIndex);
    SamplerHandle getSampler(const SamplerDesc& desc);

    SamplerIndex::enum_type registerSampler(const SamplerDesc& desc);

private:
    void removeAllSamplers();
    void createBuiltinSamplers();
    void createBuiltinSampler(uint32_t samplerIndex, const SamplerDesc& desc);

    axstd::pod_vector<SamplerHandle> _builtinSamplers;
    axstd::hash_map<SamplerIndex::enum_type, SamplerHandle> _customSamplers;

    axstd::hash_map<uint32_t, uint32_t> _samplersRegsitry;  // sampler desc => sampler index registry

    DriverBase* _driver{nullptr};

    uint32_t _nextSamplerIndex{0};
};

// end of _rhi group
/// @}

}  // namespace ax::rhi
