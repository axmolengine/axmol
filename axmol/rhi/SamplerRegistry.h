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

namespace ax
{
namespace rhi
{
/**
 * @addtogroup _rhi
 * @{
 */

class GraphicsDevice;

struct SamplerId
{
    static constexpr uint16_t INVALID = 0xffff;

    uint16_t value{INVALID};

    constexpr explicit operator bool() const { return value != INVALID; }
    constexpr bool operator==(const SamplerId&) const = default;
};

class AX_API SamplerRegistry
{
public:
    static constexpr uint32_t MAX_SAMPLER_COUNT = 256;

    static SamplerRegistry* getInstance();
    static void destroyInstance();

    SamplerRegistry();
    ~SamplerRegistry();

    /**
     * Recreates all native sampler objects from the registered sampler descriptions.
     *
     * Used after graphics device or rendering context recreation.
     */
    void rebuild();

    /**
     * Registers a named sampler description.
     *
     * If the name is already registered, the existing sampler identifier is returned.
     *
     * @param name Unique sampler name used by shader reflection.
     * @param desc Sampler state description.
     * @return Identifier of the registered sampler.
     */
    SamplerId registerSampler(std::string_view name, const SamplerDesc& desc);

    /**
     * Finds a previously registered sampler by name.
     *
     * @param name Registered sampler name.
     * @return The sampler identifier, or an invalid identifier if not found.
     */
    SamplerId find(std::string_view name) const;

    /**
     * Returns the sampler description associated with an identifier.
     *
     * @param id Valid sampler identifier.
     * @return The registered sampler description.
     */
    const SamplerDesc& getSamplerDesc(SamplerId id) const;

    /**
     * Returns the native sampler handle associated with an identifier.
     *
     * @param samplerId Valid sampler identifier.
     * @return The backend-native sampler handle.
     */
    SamplerHandle getSampler(SamplerId samplerId);

    /**
     * Returns the native sampler handle for a built-in sampler preset.
     *
     * @param samplerIndex Built-in sampler preset index.
     * @return The backend-native sampler handle.
     */
    SamplerHandle getSampler(SamplerPreset::enum_type samplerIndex);

    /**
     * Returns a cached native sampler handle matching the description.
     *
     * A new sampler is registered and created when no matching entry exists.
     *
     * @param desc Sampler state description.
     * @return The backend-native sampler handle.
     */
    SamplerHandle getSampler(const SamplerDesc& desc);

    /**
     * Registers or finds an unnamed sampler description.
     *
     * @param desc Sampler state description.
     * @return The matching sampler preset/index.
     */
    SamplerPreset::enum_type registerSampler(const SamplerDesc& desc);

private:
    void removeAllSamplers();
    void createBuiltinSamplers();
    SamplerId registerBuiltinSampler(SamplerPreset::enum_type preset, const SamplerDesc& desc);

    tlx::pod_vector<SamplerHandle> _samplers;
    tlx::pod_vector<SamplerDesc> _samplerDescs;
    tlx::string_map<SamplerId> _samplerIdsByName;

    tlx::hash_map<uint32_t, uint32_t> _samplersRegistry;  // legacy desc => sampler index registry

    GraphicsDevice* _driver{nullptr};

    uint32_t _nextSamplerIndex{0};
};

// end of _rhi group
/// @}
}  // namespace rhi

using SamplerRegistry = rhi::SamplerRegistry;

}  // namespace ax
