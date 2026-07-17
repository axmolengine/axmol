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
#include "axmol/rhi/SamplerRegistry.h"
#include "axmol/rhi/GraphicsCore.h"
#include "axmol/tlx/singleton.hpp"
#include <bit>

namespace ax::rhi
{
namespace
{
constexpr std::string_view kSamplerPresetNames[] = {
    "LinearClamp", "LinearWrap", "LinearMirror", "LinearBorder",
    "PointClamp", "PointWrap", "PointMirror", "PointBorder",
    "LinearMipClamp", "LinearMipWrap", "LinearMipMirror", "LinearMipBorder",
    "AnisoClamp", "AnisoWrap", "AnisoMirror", "AnisoBorder",
    "ShadowCmpClamp", "ShadowCmpWrap", "ShadowCmpMirror", "ShadowCmpBorder",
    "LinearNoMipClamp", "PointNoMipClamp",
};

static_assert(std::size(kSamplerPresetNames) == SamplerPreset::Count);
}  // namespace

SamplerRegistry* SamplerRegistry::getInstance()
{
    return tlx::singleton<SamplerRegistry>::instance();
}
void SamplerRegistry::destroyInstance()
{
    tlx::singleton<SamplerRegistry>::destroy();
}

SamplerRegistry::SamplerRegistry()
{
    _driver = axdrv;

    createBuiltinSamplers();
}

SamplerRegistry::~SamplerRegistry()
{
    removeAllSamplers();
}

void SamplerRegistry::rebuild()
{
    removeAllSamplers();

    createBuiltinSamplers();
}

void SamplerRegistry::removeAllSamplers()
{
    _driver->waitForGPU();

    for (auto&& sampler : _samplers)
        _driver->destroySampler(sampler);

    _samplers.clear();
    _samplerDescs.clear();
    _samplerIdsByName.clear();
    _samplersRegistry.clear();

    _nextSamplerIndex = 0;
}

SamplerId SamplerRegistry::registerSampler(std::string_view name, const SamplerDesc& desc)
{
    if (name.empty())
    {
        AXLOGE("SamplerRegistry rejects empty sampler names");
        AXASSERT(false, "Sampler name must not be empty");
        return {};
    }

    std::string samplerName{name};
    auto it = _samplerIdsByName.find(samplerName);
    if (it != _samplerIdsByName.end())
    {
        const auto& existingDesc = getDesc(it->second);
        if (std::bit_cast<uint32_t>(existingDesc) != std::bit_cast<uint32_t>(desc))
        {
            AXLOGE("Sampler '{}' is already registered with a different descriptor", name);
            AXASSERT(false, "Sampler names are immutable and cannot be redefined");
            return {};
        }
        return it->second;
    }

    if (_nextSamplerIndex >= MAX_SAMPLER_COUNT)
    {
        AXLOGE("Sampler registry is full");
        AXASSERT(false, "Sampler registry is full");
        return {};
    }

    const auto samplerIndex = _nextSamplerIndex++;
    auto samplerHandle      = _driver->createSampler(desc);

    _samplers.emplace_back(samplerHandle);
    _samplerDescs.emplace_back(desc);
    _samplerIdsByName.emplace(std::move(samplerName), SamplerId{static_cast<uint16_t>(samplerIndex)});
    _samplersRegistry.emplace(std::bit_cast<uint32_t>(desc), samplerIndex);

    return SamplerId{static_cast<uint16_t>(samplerIndex)};
}

SamplerId SamplerRegistry::find(std::string_view name) const
{
    auto it = _samplerIdsByName.find(std::string{name});
    return it != _samplerIdsByName.end() ? it->second : SamplerId{};
}

const SamplerDesc& SamplerRegistry::getDesc(SamplerId id) const
{
    AXASSERT(id && id.value < _samplerDescs.size(), "invalid SamplerId");
    return _samplerDescs[id.value];
}

SamplerPreset::enum_type SamplerRegistry::registerSampler(const SamplerDesc& desc)
{
    if (_nextSamplerIndex >= MAX_SAMPLER_COUNT)
    {
        throw std::runtime_error("sampler registry is full");
    }

    auto key = std::bit_cast<uint32_t>(desc);
    auto it  = _samplersRegistry.find(key);
    if (it != _samplersRegistry.end())
        return static_cast<SamplerPreset::enum_type>(it->second);

    auto samplerIndex  = _nextSamplerIndex++;
    auto samplerHandle = _driver->createSampler(desc);
    _samplers.emplace_back(samplerHandle);
    _samplerDescs.emplace_back(desc);

    _samplersRegistry.emplace(key, samplerIndex);

    return static_cast<SamplerPreset::enum_type>(samplerIndex);
}

SamplerHandle SamplerRegistry::getSampler(const SamplerDesc& desc)
{
    const auto samplerIndex = registerSampler(desc);
    return getSampler(samplerIndex);
}

SamplerHandle SamplerRegistry::getSampler(SamplerId samplerId)
{
    if (samplerId && samplerId.value < _samplers.size())
        return _samplers[samplerId.value];

    AXLOGE("SamplerId {} is not registered", samplerId.value);
    AXASSERT(false, "SamplerId is not registered");
    return nullptr;
}

SamplerHandle SamplerRegistry::getSampler(SamplerPreset::enum_type samplerIndex)
{
    return getSampler(SamplerId{static_cast<uint16_t>(samplerIndex)});
}

void SamplerRegistry::createBuiltinSamplers()
{
    // --- Linear sampling ---
    {
        SamplerDesc d{};
        d.minFilter = SamplerFilter::MIN_LINEAR;
        d.magFilter = SamplerFilter::MAG_LINEAR;
        d.mipFilter = SamplerFilter::MIP_DEFAULT;

        d.sAddressMode = SamplerAddressMode::CLAMP;
        d.tAddressMode = SamplerAddressMode::CLAMP;
        registerBuiltinSampler(SamplerPreset::LinearClamp, d);

        d.sAddressMode = SamplerAddressMode::REPEAT;
        d.tAddressMode = SamplerAddressMode::REPEAT;
        registerBuiltinSampler(SamplerPreset::LinearWrap, d);

        d.sAddressMode = SamplerAddressMode::MIRROR;
        d.tAddressMode = SamplerAddressMode::MIRROR;
        registerBuiltinSampler(SamplerPreset::LinearMirror, d);

        d.sAddressMode = SamplerAddressMode::BORDER;
        d.tAddressMode = SamplerAddressMode::BORDER;
        registerBuiltinSampler(SamplerPreset::LinearBorder, d);
    }

    // --- Point sampling ---
    {
        SamplerDesc d{};
        d.minFilter = SamplerFilter::MIN_NEAREST;
        d.magFilter = SamplerFilter::MAG_NEAREST;
        d.mipFilter = SamplerFilter::MIP_DEFAULT;

        d.sAddressMode = SamplerAddressMode::CLAMP;
        d.tAddressMode = SamplerAddressMode::CLAMP;
        registerBuiltinSampler(SamplerPreset::PointClamp, d);

        d.sAddressMode = SamplerAddressMode::REPEAT;
        d.tAddressMode = SamplerAddressMode::REPEAT;
        registerBuiltinSampler(SamplerPreset::PointWrap, d);

        d.sAddressMode = SamplerAddressMode::MIRROR;
        d.tAddressMode = SamplerAddressMode::MIRROR;
        registerBuiltinSampler(SamplerPreset::PointMirror, d);

        d.sAddressMode = SamplerAddressMode::BORDER;
        d.tAddressMode = SamplerAddressMode::BORDER;
        registerBuiltinSampler(SamplerPreset::PointBorder, d);
    }

    // --- Linear + Mipmap ---
    {
        SamplerDesc d{};
        d.minFilter = SamplerFilter::MIN_LINEAR;
        d.magFilter = SamplerFilter::MAG_LINEAR;
        d.mipFilter = SamplerFilter::MIP_LINEAR;

        d.sAddressMode = SamplerAddressMode::CLAMP;
        d.tAddressMode = SamplerAddressMode::CLAMP;
        registerBuiltinSampler(SamplerPreset::LinearMipClamp, d);

        d.sAddressMode = SamplerAddressMode::REPEAT;
        d.tAddressMode = SamplerAddressMode::REPEAT;
        registerBuiltinSampler(SamplerPreset::LinearMipWrap, d);

        d.sAddressMode = SamplerAddressMode::MIRROR;
        d.tAddressMode = SamplerAddressMode::MIRROR;
        registerBuiltinSampler(SamplerPreset::LinearMipMirror, d);

        d.sAddressMode = SamplerAddressMode::BORDER;
        d.tAddressMode = SamplerAddressMode::BORDER;
        registerBuiltinSampler(SamplerPreset::LinearMipBorder, d);
    }

    // --- Anisotropic filtering ---
    {
        SamplerDesc d{};
        d.minFilter  = SamplerFilter::MIN_ANISOTROPIC;
        d.magFilter  = SamplerFilter::MAG_LINEAR;
        d.mipFilter  = SamplerFilter::MIP_LINEAR;
        d.anisotropy = 0xF;

        d.sAddressMode = SamplerAddressMode::CLAMP;
        registerBuiltinSampler(SamplerPreset::AnisoClamp, d);

        d.sAddressMode = SamplerAddressMode::REPEAT;
        registerBuiltinSampler(SamplerPreset::AnisoWrap, d);

        d.sAddressMode = SamplerAddressMode::MIRROR;
        registerBuiltinSampler(SamplerPreset::AnisoMirror, d);

        d.sAddressMode = SamplerAddressMode::BORDER;
        registerBuiltinSampler(SamplerPreset::AnisoBorder, d);
    }

    // --- Depth comparison samplers ---
    {
        SamplerDesc d{};
        d.minFilter   = SamplerFilter::MIN_LINEAR;
        d.magFilter   = SamplerFilter::MAG_LINEAR;
        d.mipFilter   = SamplerFilter::MIP_DEFAULT;
        d.compareFunc = CompareFunc::LESS;

        d.sAddressMode = SamplerAddressMode::CLAMP;
        registerBuiltinSampler(SamplerPreset::ShadowCmpClamp, d);

        d.sAddressMode = SamplerAddressMode::REPEAT;
        registerBuiltinSampler(SamplerPreset::ShadowCmpWrap, d);

        d.sAddressMode = SamplerAddressMode::MIRROR;
        registerBuiltinSampler(SamplerPreset::ShadowCmpMirror, d);

        d.sAddressMode = SamplerAddressMode::BORDER;
        registerBuiltinSampler(SamplerPreset::ShadowCmpBorder, d);
    }

    // --- Special cases ---
    {
        SamplerDesc d{};
        d.minFilter    = SamplerFilter::MIN_LINEAR;
        d.magFilter    = SamplerFilter::MAG_LINEAR;
        d.mipFilter    = SamplerFilter::MIP_NEAREST;  // no mip
        d.sAddressMode = SamplerAddressMode::CLAMP;
        registerBuiltinSampler(SamplerPreset::LinearNoMipClamp, d);

        d.minFilter = SamplerFilter::MIN_NEAREST;
        d.magFilter = SamplerFilter::MAG_NEAREST;
        registerBuiltinSampler(SamplerPreset::PointNoMipClamp, d);
    }

    AXASSERT(_nextSamplerIndex == SamplerPreset::Count, "SamplerPreset built-ins must be compact");
}

SamplerId SamplerRegistry::registerBuiltinSampler(SamplerPreset::enum_type preset, const SamplerDesc& desc)
{
    auto id = registerSampler(kSamplerPresetNames[preset], desc);
    AXASSERT(id.value == static_cast<uint16_t>(preset), "SamplerPreset and built-in SamplerId mismatch");
    return id;
}

}  // namespace ax::rhi
